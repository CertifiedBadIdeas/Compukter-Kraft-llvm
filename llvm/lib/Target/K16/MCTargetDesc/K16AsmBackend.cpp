//===-- K16AsmBackend.cpp - K16 Assembler Backend ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16MCTargetDesc.h"
#include "K16MCFixups.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

class K16AsmBackend : public MCAsmBackend {
public:
  K16AsmBackend() : MCAsmBackend(llvm::endianness::little) {}

  void applyFixup(const MCFragment &F, const MCFixup &Fixup,
                  const MCValue &Target, uint8_t *Data, uint64_t Value,
                  bool IsResolved) override {
    maybeAddReloc(F, Fixup, Target, Value, IsResolved);

    if (!IsResolved)
      return;

    if (Fixup.getKind() == K16::fixup_k16_abs32 ||
        Fixup.getKind() == K16::fixup_k16_call32)
      support::endian::write32le(Data, Value);
  }

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override {
    static const MCFixupKindInfo Infos[K16::NumTargetFixupKinds] = {
        {"fixup_k16_abs32", 0, 32, 0},
        {"fixup_k16_call32", 0, 32, 0},
    };

    if (Kind < FirstTargetFixupKind)
      return MCAsmBackend::getFixupKindInfo(Kind);

    assert(unsigned(Kind - FirstTargetFixupKind) <
               K16::NumTargetFixupKinds &&
           "invalid K16 fixup kind");
    return Infos[Kind - FirstTargetFixupKind];
  }

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createK16ELFObjectWriter(ELF::ELFOSABI_STANDALONE);
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *) const override {
    if (Count % 2 != 0)
      return false;

    for (uint64_t I = 0; I != Count; I += 2)
      OS.write("\0\0", 2);
    return true;
  }
};

} // namespace

MCAsmBackend *llvm::createK16MCAsmBackend(const Target &,
                                           const MCSubtargetInfo &,
                                           const MCRegisterInfo &,
                                           const MCTargetOptions &) {
  return new K16AsmBackend();
}
