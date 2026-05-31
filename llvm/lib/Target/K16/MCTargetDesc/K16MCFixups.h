//===-- K16MCFixups.h - K16-specific fixup entries ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_MCTARGETDESC_K16MCFIXUPS_H
#define LLVM_LIB_TARGET_K16_MCTARGETDESC_K16MCFIXUPS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace K16 {
enum FixupKind {
  fixup_k16_abs32 = FirstTargetFixupKind,
  fixup_k16_call32,

  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
} // namespace K16
} // namespace llvm

#endif
