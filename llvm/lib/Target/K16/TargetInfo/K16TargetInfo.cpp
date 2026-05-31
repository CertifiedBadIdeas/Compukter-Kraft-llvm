//===-- K16TargetInfo.cpp - K16 target implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/K16TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
Target &getTheK16Target() {
  static Target TheK16Target;
  return TheK16Target;
}
} // namespace llvm

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeK16TargetInfo() {
  llvm::RegisterTarget<llvm::Triple::k16> X(
      llvm::getTheK16Target(), "k16", "K16 32-bit", "K16");
}
