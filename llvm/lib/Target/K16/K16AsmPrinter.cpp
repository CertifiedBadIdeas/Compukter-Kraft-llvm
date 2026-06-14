//===-- K16AsmPrinter.cpp - K16 LLVM assembly writer -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/K16MCTargetDesc.h"
#include "K16.h"
#include "TargetInfo/K16TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

const MCExpr *withOffset(const MCExpr *Expr, int64_t Offset,
                         MCContext &Context) {
  if (Offset == 0)
    return Expr;

  const MCExpr *OffsetExpr =
      MCConstantExpr::create(Offset < 0 ? -Offset : Offset, Context);
  return Offset < 0 ? MCBinaryExpr::createSub(Expr, OffsetExpr, Context)
                    : MCBinaryExpr::createAdd(Expr, OffsetExpr, Context);
}

class K16AsmPrinter : public AsmPrinter {
public:
  explicit K16AsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer), ID) {}

  StringRef getPassName() const override { return "K16 Assembly Printer"; }
  void emitInstruction(const MachineInstr *MI) override;

  static char ID;
};

} // namespace

void K16AsmPrinter::emitInstruction(const MachineInstr *MI) {
  if (MI->getOpcode() == K16::ADJCALLSTACKDOWN ||
      MI->getOpcode() == K16::ADJCALLSTACKUP) {
    int64_t Amount = MI->getOperand(0).getImm();
    if (Amount == 0)
      return;

    MCInst Size;
    Size.setOpcode(K16::CONST32);
    Size.addOperand(MCOperand::createReg(K16::R13));
    Size.addOperand(MCOperand::createImm(Amount));
    EmitToStreamer(*OutStreamer, Size);

    MCInst Adjust;
    Adjust.setOpcode(MI->getOpcode() == K16::ADJCALLSTACKDOWN ? K16::SUB
                                                               : K16::ADD);
    Adjust.addOperand(MCOperand::createReg(K16::SP));
    Adjust.addOperand(MCOperand::createReg(K16::SP));
    Adjust.addOperand(MCOperand::createReg(K16::R13));
    EmitToStreamer(*OutStreamer, Adjust);
    return;
  }

  MCInst Inst;
  Inst.setOpcode(MI->getOpcode());
  for (const MachineOperand &MO : MI->operands()) {
    if (MO.isReg()) {
      if (MO.isImplicit())
        continue;
      Inst.addOperand(MCOperand::createReg(MO.getReg()));
      continue;
    }

    if (MO.isImm()) {
      Inst.addOperand(MCOperand::createImm(MO.getImm()));
      continue;
    }

    if (MO.isGlobal()) {
      MCSymbol *Symbol = getSymbol(MO.getGlobal());
      const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, OutContext);
      Inst.addOperand(MCOperand::createExpr(
          withOffset(Expr, MO.getOffset(), OutContext)));
      continue;
    }

    if (MO.isSymbol()) {
      MCSymbol *Symbol = OutContext.getOrCreateSymbol(MO.getSymbolName());
      const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, OutContext);
      Inst.addOperand(MCOperand::createExpr(
          withOffset(Expr, MO.getOffset(), OutContext)));
      continue;
    }

    if (MO.isCPI()) {
      MCSymbol *Symbol = GetCPISymbol(MO.getIndex());
      const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, OutContext);
      Inst.addOperand(MCOperand::createExpr(
          withOffset(Expr, MO.getOffset(), OutContext)));
      continue;
    }

    if (MO.isMBB()) {
      MCSymbol *Symbol = MO.getMBB()->getSymbol();
      Inst.addOperand(
          MCOperand::createExpr(MCSymbolRefExpr::create(Symbol, OutContext)));
      continue;
    }

    if (MO.isRegMask())
      continue;

    report_fatal_error("Unsupported K16 machine operand");
  }
  EmitToStreamer(*OutStreamer, Inst);
}

char K16AsmPrinter::ID = 0;

INITIALIZE_PASS(K16AsmPrinter, "k16-asm-printer",
                "K16 Assembly Printer", false, false)

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeK16AsmPrinter() {
  RegisterAsmPrinter<K16AsmPrinter> X(getTheK16Target());
}
