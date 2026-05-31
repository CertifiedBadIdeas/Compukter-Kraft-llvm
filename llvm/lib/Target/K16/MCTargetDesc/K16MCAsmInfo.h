//===-- K16MCAsmInfo.h - K16 asm properties --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_MCTARGETDESC_K16MCASMINFO_H
#define LLVM_LIB_TARGET_K16_MCTARGETDESC_K16MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class K16MCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit K16MCAsmInfo(const Triple &TT, const MCTargetOptions &Options);
};

} // namespace llvm

#endif
