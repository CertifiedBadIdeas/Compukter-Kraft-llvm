//===-- K16MCCodeEmitter.cpp - Convert K16 code to machine code -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16MCTargetDesc.h"
#include "K16MCFixups.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

namespace {

class K16MCCodeEmitter : public MCCodeEmitter {
  MCContext &Ctx;

public:
  explicit K16MCCodeEmitter(MCContext &Ctx) : Ctx(Ctx) {}

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

private:
  uint16_t getRegEncoding(const MCOperand &Operand) const;
  static void emitWord(SmallVectorImpl<char> &CB, uint16_t Word);
  static void emitU32Operand(SmallVectorImpl<char> &CB,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCOperand &Operand, MCFixupKind Kind);
};

} // namespace

uint16_t K16MCCodeEmitter::getRegEncoding(const MCOperand &Operand) const {
  if (!Operand.isReg())
    report_fatal_error("K16 encoder expected a register operand");
  return Ctx.getRegisterInfo()->getEncodingValue(Operand.getReg());
}

void K16MCCodeEmitter::emitWord(SmallVectorImpl<char> &CB, uint16_t Word) {
  support::endian::write(CB, Word, llvm::endianness::little);
}

void K16MCCodeEmitter::emitU32Operand(SmallVectorImpl<char> &CB,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCOperand &Operand,
                                        MCFixupKind Kind) {
  if (Operand.isImm()) {
    uint32_t Imm = static_cast<uint32_t>(Operand.getImm());
    emitWord(CB, Imm & 0xffff);
    emitWord(CB, Imm >> 16);
    return;
  }

  if (!Operand.isExpr())
    report_fatal_error("K16 encoder expected an immediate or symbol operand");

  Fixups.push_back(MCFixup::create(CB.size(), Operand.getExpr(), Kind));
  emitWord(CB, 0);
  emitWord(CB, 0);
}

void K16MCCodeEmitter::encodeInstruction(
    const MCInst &MI, SmallVectorImpl<char> &CB,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  switch (MI.getOpcode()) {
  case K16::RET:
    emitWord(CB, 0x9000);
    return;
  case K16::CONST32: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    emitWord(CB, 0xe001 | (Dst << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(1),
                   MCFixupKind(K16::fixup_k16_abs32));
    return;
  }
  case K16::ADD: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2000 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::SUB: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2001 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::MUL: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x200c | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::MULHU: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x200d | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::MULHS: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x200e | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::AND: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2002 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::OR: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2003 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::XOR: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2004 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::SHL: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2005 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::SHR: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2006 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::SAR: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2007 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::EQ: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2008 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::NE: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x2009 | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::LTU: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x200a | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::LT_S: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Lhs = getRegEncoding(MI.getOperand(1));
    uint16_t Rhs = getRegEncoding(MI.getOperand(2));
    emitWord(CB, 0x200b | (Dst << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    return;
  }
  case K16::LOAD8: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Addr = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x4000 | (Dst << 8) | (Addr << 4));
    return;
  }
  case K16::LOAD16: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Addr = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x4001 | (Dst << 8) | (Addr << 4));
    return;
  }
  case K16::LOAD32: {
    uint16_t Dst = getRegEncoding(MI.getOperand(0));
    uint16_t Addr = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x4002 | (Dst << 8) | (Addr << 4));
    return;
  }
  case K16::STORE8: {
    uint16_t Addr = getRegEncoding(MI.getOperand(0));
    uint16_t Src = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x5000 | (Addr << 8) | (Src << 4));
    return;
  }
  case K16::STORE16: {
    uint16_t Addr = getRegEncoding(MI.getOperand(0));
    uint16_t Src = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x5001 | (Addr << 8) | (Src << 4));
    return;
  }
  case K16::STORE32: {
    uint16_t Addr = getRegEncoding(MI.getOperand(0));
    uint16_t Src = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x5002 | (Addr << 8) | (Src << 4));
    return;
  }
  case K16::CALL32:
    emitWord(CB, 0xe001 | (14 << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(0),
                   MCFixupKind(K16::fixup_k16_call32));
    emitWord(CB, 0x8000 | (14 << 8));
    return;
  case K16::CALLR: {
    uint16_t Target = getRegEncoding(MI.getOperand(0));
    emitWord(CB, 0x8000 | (Target << 8));
    return;
  }
  case K16::BR:
    emitWord(CB, 0xe001 | (14 << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(0),
                   MCFixupKind(K16::fixup_k16_abs32));
    emitWord(CB, 0x7000 | (14 << 8));
    return;
  case K16::BRNZ: {
    uint16_t Cond = getRegEncoding(MI.getOperand(0));
    emitWord(CB, 0x6000 | (Cond << 8) | 4);
    emitWord(CB, 0xe001 | (14 << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(1),
                   MCFixupKind(K16::fixup_k16_abs32));
    emitWord(CB, 0x7000 | (14 << 8));
    return;
  }
  case K16::BRZ: {
    uint16_t Cond = getRegEncoding(MI.getOperand(0));
    emitWord(CB, 0x6010 | (Cond << 8) | 4);
    emitWord(CB, 0xe001 | (14 << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(1),
                   MCFixupKind(K16::fixup_k16_abs32));
    emitWord(CB, 0x7000 | (14 << 8));
    return;
  }
  case K16::BRLTU: {
    uint16_t Lhs = getRegEncoding(MI.getOperand(0));
    uint16_t Rhs = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x200a | (14 << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    emitWord(CB, 0x6000 | (14 << 8) | 4);
    emitWord(CB, 0xe001 | (14 << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(2),
                   MCFixupKind(K16::fixup_k16_abs32));
    emitWord(CB, 0x7000 | (14 << 8));
    return;
  }
  case K16::BRUGE: {
    uint16_t Lhs = getRegEncoding(MI.getOperand(0));
    uint16_t Rhs = getRegEncoding(MI.getOperand(1));
    emitWord(CB, 0x200a | (14 << 8));
    emitWord(CB, (Lhs << 4) | Rhs);
    emitWord(CB, 0x6010 | (14 << 8) | 4);
    emitWord(CB, 0xe001 | (14 << 8));
    emitU32Operand(CB, Fixups, MI.getOperand(2),
                   MCFixupKind(K16::fixup_k16_abs32));
    emitWord(CB, 0x7000 | (14 << 8));
    return;
  }
  default:
    report_fatal_error("Unsupported K16 opcode for object emission");
  }
}

MCCodeEmitter *llvm::createK16MCCodeEmitter(const MCInstrInfo &,
                                             MCContext &Ctx) {
  return new K16MCCodeEmitter(Ctx);
}
