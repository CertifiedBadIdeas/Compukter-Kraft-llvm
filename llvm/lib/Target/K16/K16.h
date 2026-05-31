//===-- K16.h - Top-level interface for K16 ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_K16_H
#define LLVM_LIB_TARGET_K16_K16_H

#include "llvm/PassRegistry.h"
#include "llvm/Support/CodeGen.h"

namespace llvm {

class FunctionPass;
class K16TargetMachine;

FunctionPass *createK16ISelDag(K16TargetMachine &TM,
                                 CodeGenOptLevel OptLevel);

void initializeK16AsmPrinterPass(PassRegistry &);
void initializeK16DAGToDAGISelLegacyPass(PassRegistry &);

} // namespace llvm

#endif
