//===-- K16TargetInfo.h - K16 target implementation ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_TARGETINFO_K16TARGETINFO_H
#define LLVM_LIB_TARGET_K16_TARGETINFO_K16TARGETINFO_H

namespace llvm {
class Target;

Target &getTheK16Target();
} // namespace llvm

#endif
