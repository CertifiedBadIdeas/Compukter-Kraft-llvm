//===-- K16TargetMachine.cpp - K16 target implementation -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16TargetMachine.h"
#include "K16.h"
#include "TargetInfo/K16TargetInfo.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/GlobalObject.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include <optional>

using namespace llvm;

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeK16Target() {
  RegisterTargetMachine<K16TargetMachine> X(getTheK16Target());
  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeK16AsmPrinterPass(PR);
  initializeK16DAGToDAGISelLegacyPass(PR);
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

namespace {

class K16TargetObjectFile : public TargetLoweringObjectFileELF {
public:
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override {
    TargetLoweringObjectFileELF::Initialize(Ctx, TM);
    TextSection = Ctx.getELFSection(".text.k16", ELF::SHT_PROGBITS,
                                    ELF::SHF_EXECINSTR | ELF::SHF_ALLOC);
  }

  unsigned getTextSectionAlignment() const override { return 2; }

protected:
  MCSection *SelectSectionForGlobal(const GlobalObject *GO, SectionKind Kind,
                                    const TargetMachine &TM) const override {
    if (Kind.isText() && !GO->hasSection() && TM.getFunctionSections()) {
      SmallString<128> SectionName(".text.k16.");
      SmallString<128> MangledName;
      getMangler().getNameWithPrefix(MangledName, GO,
                                      /*CannotUsePrivateLabel=*/true);
      SectionName += MangledName;
      return getContext().getELFSection(
          SectionName, ELF::SHT_PROGBITS, ELF::SHF_EXECINSTR | ELF::SHF_ALLOC);
    }
    if (Kind.isText() && !GO->hasSection())
      return TextSection;
    return TargetLoweringObjectFileELF::SelectSectionForGlobal(GO, Kind, TM);
  }
};

} // namespace

K16TargetMachine::K16TargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, "e-p:32:32-i32:32-i64:64-n32-S64", TT, CPU,
                               FS, Options,
                               getEffectiveRelocModel(RM),
                               getEffectiveCodeModel(CM, CodeModel::Small),
                               OL),
      TLOF(std::make_unique<K16TargetObjectFile>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}

class K16PassConfig : public TargetPassConfig {
public:
  K16PassConfig(K16TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  K16TargetMachine &getK16TargetMachine() const {
    return getTM<K16TargetMachine>();
  }

  bool addInstSelector() override;
};

TargetPassConfig *K16TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new K16PassConfig(*this, PM);
}

bool K16PassConfig::addInstSelector() {
  addPass(createK16ISelDag(getK16TargetMachine(), getOptLevel()));
  return false;
}
