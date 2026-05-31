//===-- K16InstrInfo.cpp - K16 Instruction Information ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16InstrInfo.h"
#include "K16Subtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "K16GenInstrInfo.inc"

void K16InstrInfo::anchor() {}

K16InstrInfo::K16InstrInfo(const K16Subtarget &STI)
    : K16GenInstrInfo(STI, RI, K16::ADJCALLSTACKDOWN, K16::ADJCALLSTACKUP),
      RI() {}

static bool isK16Branch(unsigned Opcode) {
  switch (Opcode) {
  case K16::BR:
  case K16::BRZ:
  case K16::BRNZ:
  case K16::BRLTU:
  case K16::BRUGE:
    return true;
  default:
    return false;
  }
}

static bool isK16ConditionalBranch(unsigned Opcode) {
  switch (Opcode) {
  case K16::BRZ:
  case K16::BRNZ:
  case K16::BRLTU:
  case K16::BRUGE:
    return true;
  default:
    return false;
  }
}

static MachineBasicBlock *getK16BranchTarget(const MachineInstr &MI) {
  switch (MI.getOpcode()) {
  case K16::BR:
    return MI.getOperand(0).getMBB();
  case K16::BRZ:
  case K16::BRNZ:
    return MI.getOperand(1).getMBB();
  case K16::BRLTU:
  case K16::BRUGE:
    return MI.getOperand(2).getMBB();
  default:
    llvm_unreachable("not a K16 branch");
  }
}

static void parseK16BranchCondition(const MachineInstr &MI,
                                    SmallVectorImpl<MachineOperand> &Cond) {
  Cond.clear();
  Cond.push_back(MachineOperand::CreateImm(MI.getOpcode()));
  switch (MI.getOpcode()) {
  case K16::BRZ:
  case K16::BRNZ:
    Cond.push_back(MI.getOperand(0));
    return;
  case K16::BRLTU:
  case K16::BRUGE:
    Cond.push_back(MI.getOperand(0));
    Cond.push_back(MI.getOperand(1));
    return;
  default:
    llvm_unreachable("not a conditional K16 branch");
  }
}

void K16InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I,
                                 const DebugLoc &DL, Register DestReg,
                                 Register SrcReg, bool KillSrc, bool,
                                 bool) const {
  if (DestReg == SrcReg)
    return;

  BuildMI(MBB, I, DL, get(K16::CONST32), K16::R13).addImm(0);
  BuildMI(MBB, I, DL, get(K16::ADD), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc))
      .addReg(K16::R13, RegState::Kill);
}

bool K16InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                 MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {
  TBB = FBB = nullptr;
  Cond.clear();

  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end() || !isUnpredicatedTerminator(*I))
    return false;

  MachineBasicBlock::iterator FirstUncond = MBB.end();
  int NumTerminators = 0;
  for (auto J = I.getReverse(); J != MBB.rend() && isUnpredicatedTerminator(*J);
       ++J) {
    if (!isK16Branch(J->getOpcode()))
      return true;
    ++NumTerminators;
    if (J->getOpcode() == K16::BR)
      FirstUncond = J.getReverse();
  }

  if (AllowModify && FirstUncond != MBB.end()) {
    while (std::next(FirstUncond) != MBB.end()) {
      std::next(FirstUncond)->eraseFromParent();
      --NumTerminators;
    }
    I = FirstUncond;
  }

  if (NumTerminators == 1 && I->getOpcode() == K16::BR) {
    TBB = getK16BranchTarget(*I);
    return false;
  }

  if (NumTerminators == 1 && isK16ConditionalBranch(I->getOpcode())) {
    TBB = getK16BranchTarget(*I);
    parseK16BranchCondition(*I, Cond);
    return false;
  }

  if (NumTerminators == 2 &&
      isK16ConditionalBranch(std::prev(I)->getOpcode()) &&
      I->getOpcode() == K16::BR) {
    TBB = getK16BranchTarget(*std::prev(I));
    FBB = getK16BranchTarget(*I);
    parseK16BranchCondition(*std::prev(I), Cond);
    return false;
  }

  return true;
}

unsigned K16InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                    int *BytesRemoved) const {
  if (BytesRemoved)
    *BytesRemoved = 0;

  unsigned Removed = 0;
  MachineBasicBlock::iterator I = MBB.end();
  while (I != MBB.begin()) {
    --I;
    if (I->isDebugInstr())
      continue;
    if (!isK16Branch(I->getOpcode()))
      break;

    if (BytesRemoved)
      *BytesRemoved += getInstSizeInBytes(*I);
    I->eraseFromParent();
    I = MBB.end();
    ++Removed;
    if (Removed == 2)
      break;
  }

  return Removed;
}

unsigned K16InstrInfo::insertBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *TBB,
                                    MachineBasicBlock *FBB,
                                    ArrayRef<MachineOperand> Cond,
                                    const DebugLoc &DL,
                                    int *BytesAdded) const {
  if (BytesAdded)
    *BytesAdded = 0;
  assert(TBB && "insertBranch must not be told to insert a fallthrough");

  if (Cond.empty()) {
    assert(!FBB && "unconditional branch with multiple successors");
    MachineInstr &MI = *BuildMI(&MBB, DL, get(K16::BR)).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded += getInstSizeInBytes(MI);
    return 1;
  }

  assert((Cond.size() == 2 || Cond.size() == 3) &&
         "K16 branch condition must contain opcode and operands");
  unsigned Opcode = Cond[0].getImm();
  MachineInstrBuilder MIB = BuildMI(&MBB, DL, get(Opcode));
  for (unsigned I = 1, E = Cond.size(); I != E; ++I)
    MIB.add(Cond[I]);
  MIB.addMBB(TBB);

  if (BytesAdded)
    *BytesAdded += getInstSizeInBytes(*MIB);
  if (!FBB)
    return 1;

  MachineInstr &MI = *BuildMI(&MBB, DL, get(K16::BR)).addMBB(FBB);
  if (BytesAdded)
    *BytesAdded += getInstSizeInBytes(MI);
  return 2;
}

bool K16InstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  if (Cond.empty())
    return true;

  switch (Cond[0].getImm()) {
  case K16::BRZ:
    Cond[0].setImm(K16::BRNZ);
    return false;
  case K16::BRNZ:
    Cond[0].setImm(K16::BRZ);
    return false;
  case K16::BRLTU:
    Cond[0].setImm(K16::BRUGE);
    return false;
  case K16::BRUGE:
    Cond[0].setImm(K16::BRLTU);
    return false;
  default:
    return true;
  }
}

void K16InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator I,
                                         Register SrcReg, bool isKill,
                                         int FrameIndex,
                                         const TargetRegisterClass *,
                                         Register, MachineInstr::MIFlag) const {
  BuildMI(MBB, I, DebugLoc(), get(K16::STORE32))
      .addFrameIndex(FrameIndex)
      .addReg(SrcReg, getKillRegState(isKill));
}

void K16InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          Register DestReg, int FrameIndex,
                                          const TargetRegisterClass *,
                                          Register, unsigned,
                                          MachineInstr::MIFlag) const {
  BuildMI(MBB, I, DebugLoc(), get(K16::LOAD32), DestReg)
      .addFrameIndex(FrameIndex);
}
