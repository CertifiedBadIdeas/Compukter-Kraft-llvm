//===-- K16MCTargetDesc.h - K16 Target Descriptions --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_MCTARGETDESC_K16MCTARGETDESC_H
#define LLVM_LIB_TARGET_K16_MCTARGETDESC_K16MCTARGETDESC_H

#include <cstdint>
#include <memory>

#define GET_REGINFO_ENUM
#include "K16GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "K16GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "K16GenSubtargetInfo.inc"

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCCodeEmitter *createK16MCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);

MCAsmBackend *createK16MCAsmBackend(const Target &T,
                                      const MCSubtargetInfo &STI,
                                      const MCRegisterInfo &MRI,
                                      const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createK16ELFObjectWriter(uint8_t OSABI);

} // namespace llvm

#endif
