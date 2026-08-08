//===-- K16MachineFunctionInfo.h - K16 machine function info -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_K16MACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_K16_K16MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include <cassert>

namespace llvm {

class K16MachineFunctionInfo : public MachineFunctionInfo {
  int VarArgsFrameIndex = 0;
  bool HasVarArgsFrameIndex = false;

public:
  K16MachineFunctionInfo(const Function &, const TargetSubtargetInfo *) {}

  MachineFunctionInfo *
  clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
        const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
      const override {
    return DestMF.cloneInfo<K16MachineFunctionInfo>(*this);
  }

  void setVarArgsFrameIndex(int Index) {
    assert(!HasVarArgsFrameIndex && "K16 varargs frame index set twice");
    VarArgsFrameIndex = Index;
    HasVarArgsFrameIndex = true;
  }

  int getVarArgsFrameIndex() const {
    assert(HasVarArgsFrameIndex && "K16 varargs frame index is not set");
    return VarArgsFrameIndex;
  }
};

} // namespace llvm

#endif
