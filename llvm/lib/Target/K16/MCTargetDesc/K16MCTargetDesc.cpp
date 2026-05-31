//===-- K16MCTargetDesc.cpp - K16 Target Descriptions -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16MCTargetDesc.h"
#include "K16InstPrinter.h"
#include "K16MCAsmInfo.h"
#include "TargetInfo/K16TargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include <string>

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "K16GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "K16GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "K16GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createK16MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitK16MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createK16MCRegisterInfo(const Triple &) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitK16MCRegisterInfo(X, K16::R14);
  return X;
}

static MCSubtargetInfo *
createK16MCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  std::string CPUName = std::string(CPU);
  if (CPUName.empty())
    CPUName = "generic";

  return createK16MCSubtargetInfoImpl(TT, CPUName, CPUName, FS);
}

static MCInstPrinter *createK16MCInstPrinter(const Triple &, unsigned Syntax,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  if (Syntax == 0)
    return new K16InstPrinter(MAI, MII, MRI);
  return nullptr;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeK16TargetMC() {
  Target &T = getTheK16Target();

  RegisterMCAsmInfo<K16MCAsmInfo> X(T);
  TargetRegistry::RegisterMCInstrInfo(T, createK16MCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(T, createK16MCRegisterInfo);
  TargetRegistry::RegisterMCSubtargetInfo(T, createK16MCSubtargetInfo);
  TargetRegistry::RegisterMCInstPrinter(T, createK16MCInstPrinter);
  TargetRegistry::RegisterMCCodeEmitter(T, createK16MCCodeEmitter);
  TargetRegistry::RegisterMCAsmBackend(T, createK16MCAsmBackend);
}
