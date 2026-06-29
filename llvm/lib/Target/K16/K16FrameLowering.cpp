//===-- K16FrameLowering.cpp - K16 Frame Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16FrameLowering.h"
#include "K16InstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include <cstdint>

using namespace llvm;

static bool fitsI16(uint64_t Value) { return Value <= INT16_MAX; }

K16FrameLowering::K16FrameLowering()
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(8), 0,
                          Align(8)) {}

bool K16FrameLowering::hasFPImpl(const MachineFunction &) const {
  return false;
}

bool K16FrameLowering::hasReservedCallFrame(const MachineFunction &) const {
  return true;
}

MachineBasicBlock::iterator K16FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  MachineInstr &Old = *I;
  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
  uint64_t Amount = TII->getFrameSize(Old);
  if (Amount != 0 && !hasReservedCallFrame(MF)) {
    DebugLoc DL = Old.getDebugLoc();
    unsigned Opcode = Old.getOpcode();
    if (fitsI16(Amount)) {
      BuildMI(MBB, I, DL, TII->get(K16::ADDI), K16::SP)
          .addReg(K16::SP)
          .addImm(Opcode == K16::ADJCALLSTACKDOWN ? -int64_t(Amount)
                                                  : int64_t(Amount));
      return MBB.erase(I);
    }
    BuildMI(MBB, I, DL, TII->get(K16::CONST32), K16::R13).addImm(Amount);
    if (Opcode == K16::ADJCALLSTACKDOWN) {
      BuildMI(MBB, I, DL, TII->get(K16::SUB), K16::SP)
          .addReg(K16::SP)
          .addReg(K16::R13, RegState::Kill);
    } else {
      assert(Opcode == K16::ADJCALLSTACKUP);
      BuildMI(MBB, I, DL, TII->get(K16::ADD), K16::SP)
          .addReg(K16::SP)
          .addReg(K16::R13, RegState::Kill);
    }
  }
  return MBB.erase(I);
}

void K16FrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  uint64_t StackSize = MF.getFrameInfo().getStackSize();
  if (MF.getFrameInfo().hasCalls())
    StackSize += 4;
  if (StackSize == 0)
    return;

  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator Insert = MBB.begin();
  DebugLoc DL = MBB.findDebugLoc(Insert);
  if (fitsI16(StackSize)) {
    BuildMI(MBB, Insert, DL, TII->get(K16::ADDI), K16::SP)
        .addReg(K16::SP)
        .addImm(-int64_t(StackSize));
    return;
  }
  BuildMI(MBB, Insert, DL, TII->get(K16::CONST32), K16::R13)
      .addImm(StackSize);
  BuildMI(MBB, Insert, DL, TII->get(K16::SUB), K16::SP)
      .addReg(K16::SP)
      .addReg(K16::R13, RegState::Kill);
}

void K16FrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  uint64_t StackSize = MF.getFrameInfo().getStackSize();
  if (MF.getFrameInfo().hasCalls())
    StackSize += 4;
  if (StackSize == 0)
    return;

  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator Insert = MBB.getFirstTerminator();
  DebugLoc DL = MBB.findDebugLoc(Insert);
  if (fitsI16(StackSize)) {
    BuildMI(MBB, Insert, DL, TII->get(K16::ADDI), K16::SP)
        .addReg(K16::SP)
        .addImm(StackSize);
    return;
  }
  BuildMI(MBB, Insert, DL, TII->get(K16::CONST32), K16::R13)
      .addImm(StackSize);
  BuildMI(MBB, Insert, DL, TII->get(K16::ADD), K16::SP)
      .addReg(K16::SP)
      .addReg(K16::R13, RegState::Kill);
}
