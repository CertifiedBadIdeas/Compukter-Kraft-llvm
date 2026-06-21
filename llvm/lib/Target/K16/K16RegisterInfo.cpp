//===-- K16RegisterInfo.cpp - K16 Register Information ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16RegisterInfo.h"
#include "K16FrameLowering.h"
#include "K16InstrInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "K16GenRegisterInfo.inc"

K16RegisterInfo::K16RegisterInfo() : K16GenRegisterInfo(K16::R14) {}

const MCPhysReg *
K16RegisterInfo::getCalleeSavedRegs(const MachineFunction *) const {
  return CSR_NoRegs_SaveList;
}

const uint32_t *
K16RegisterInfo::getCallPreservedMask(const MachineFunction &,
                                        CallingConv::ID) const {
  return CSR_CallPreserved_RegMask;
}

BitVector K16RegisterInfo::getReservedRegs(const MachineFunction &) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(K16::FP);
  Reserved.set(K16::R13);
  Reserved.set(K16::SP);
  Reserved.set(K16::R14);
  return Reserved;
}

const TargetRegisterClass *
K16RegisterInfo::getPointerRegClass(unsigned) const {
  return &K16::GPRRegClass;
}

bool K16RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int,
                                            unsigned, RegScavenger *) const {
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

  unsigned FIOperandNum = 0;
  while (!MI.getOperand(FIOperandNum).isFI())
    ++FIOperandNum;

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  int64_t Offset = MFI.getObjectOffset(FrameIndex) + MFI.getStackSize();
  if (MFI.hasCalls())
    Offset += 4;
  DebugLoc DL = MI.getDebugLoc();

  if (MI.getOpcode() == K16::FRAMEADDR) {
    Register DstReg = MI.getOperand(0).getReg();
    BuildMI(MBB, II, DL, TII->get(K16::CONST32), K16::R13).addImm(Offset);
    BuildMI(MBB, II, DL, TII->get(K16::ADD), DstReg)
        .addReg(K16::SP)
        .addReg(K16::R13, RegState::Kill);
    MI.eraseFromParent();
    return true;
  }

  BuildMI(MBB, II, DL, TII->get(K16::CONST32), K16::R13).addImm(Offset);
  BuildMI(MBB, II, DL, TII->get(K16::ADD), K16::R13)
      .addReg(K16::SP)
      .addReg(K16::R13, RegState::Kill);
  MI.getOperand(FIOperandNum).ChangeToRegister(K16::R13, false);
  return false;
}

Register K16RegisterInfo::getFrameRegister(const MachineFunction &) const {
  return K16::SP;
}
