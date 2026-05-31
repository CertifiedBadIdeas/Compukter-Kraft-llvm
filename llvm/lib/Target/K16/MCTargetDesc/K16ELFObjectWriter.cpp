//===-- K16ELFObjectWriter.cpp - K16 ELF Writer -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16MCTargetDesc.h"
#include "K16MCFixups.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {

class K16ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  explicit K16ELFObjectWriter(uint8_t OSABI)
      : MCELFObjectTargetWriter(/*Is64Bit=*/false, OSABI, ELF::EM_K16,
                                /*HasRelocationAddend=*/true) {}

protected:
  unsigned getRelocType(const MCFixup &Fixup, const MCValue &,
                        bool IsPCRel) const override {
    switch (Fixup.getKind()) {
    case K16::fixup_k16_abs32:
      return ELF::R_K16_ABS32;
    case K16::fixup_k16_call32:
      return ELF::R_K16_CALL32;
    default:
      llvm_unreachable("invalid K16 fixup kind");
    }
  }
};

} // namespace

std::unique_ptr<MCObjectTargetWriter>
llvm::createK16ELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<K16ELFObjectWriter>(OSABI);
}
