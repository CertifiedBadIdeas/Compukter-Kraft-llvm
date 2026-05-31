//===-- K16MCAsmInfo.cpp - K16 asm properties ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16MCAsmInfo.h"

using namespace llvm;

void K16MCAsmInfo::anchor() {}

K16MCAsmInfo::K16MCAsmInfo(const Triple &, const MCTargetOptions &)
    : MCAsmInfoELF() {
  IsLittleEndian = true;
  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";
  WeakRefDirective = "\t.weak\t";

  UsesELFSectionDirectiveForBSS = true;
  HasDotTypeDotSizeDirective = true;
  HasIdentDirective = false;

  SupportsDebugInformation = true;
  ExceptionsType = ExceptionHandling::None;
  MinInstAlignment = 4;
  CodePointerSize = 4;
}
