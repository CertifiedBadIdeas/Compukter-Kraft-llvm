//===-- K16Subtarget.h - Define Subtarget for K16 ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_K16SUBTARGET_H
#define LLVM_LIB_TARGET_K16_K16SUBTARGET_H

#include "K16FrameLowering.h"
#include "K16ISelLowering.h"
#include "K16InstrInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "K16GenSubtargetInfo.inc"

namespace llvm {

class K16Subtarget : public K16GenSubtargetInfo {
  void anchor();

  K16InstrInfo InstrInfo;
  K16FrameLowering FrameLowering;
  K16TargetLowering TLInfo;
  SelectionDAGTargetInfo TSInfo;

public:
  K16Subtarget(const Triple &TT, const std::string &CPU,
                 const std::string &FS, const TargetMachine &TM);

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const K16InstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const K16RegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }
  const K16FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const K16TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  void initLibcallLoweringInfo(LibcallLoweringInfo &Info) const override;
};

} // namespace llvm

#endif
