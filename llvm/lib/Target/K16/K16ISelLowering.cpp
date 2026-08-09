//===-- K16ISelLowering.cpp - K16 DAG Lowering Implementation --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16ISelLowering.h"
#include "K16MachineFunctionInfo.h"
#include "K16Subtarget.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/MathExtras.h"
#include <climits>
#include <cstring>

using namespace llvm;

static const MCPhysReg K16ArgRegs[] = {K16::R1, K16::R2, K16::R3};
static const MCPhysReg K16RetRegs[] = {K16::R0, K16::R1, K16::R2, K16::R3};
static const MCPhysReg K16F32R32ArgRegs[] = {
    K16::R0, K16::R1, K16::R2, K16::R3,
    K16::R4, K16::R5, K16::R6, K16::R7};
static const MCPhysReg K16F32R32RetRegs[] = {K16::R0, K16::R1};
static constexpr unsigned K16StackSlotBytes = 4;
static constexpr unsigned K16ReturnPcBytes = 4;
static constexpr unsigned K16MaxArgumentAlignment = 8;

namespace {

struct K16OutgoingArgLocation {
  MCRegister Reg;
  unsigned StackOffset = 0;
  unsigned ByValCopyOffset = UINT_MAX;
};

} // namespace

static bool isSupportedK16CallingConv(CallingConv::ID CallConv) {
  switch (CallConv) {
  case CallingConv::C:
  case CallingConv::Fast:
  case CallingConv::Cold:
    return true;
  default:
    return false;
  }
}

static StringRef getK16SpecialCalleeName(SDValue Callee) {
  if (auto *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    return E->getSymbol();
  if (auto *G = dyn_cast<GlobalAddressSDNode>(Callee))
    return G->getGlobal()->getName();
  return StringRef();
}

static bool isK16HaltOnceCallee(SDValue Callee) {
  return getK16SpecialCalleeName(Callee) == "__k16_halt_once";
}

static bool isK16IretOnceCallee(SDValue Callee) {
  return getK16SpecialCalleeName(Callee) == "__k16_iret_once";
}

static bool getK16ReadCsrCallee(SDValue Callee, unsigned &Csr) {
  StringRef Name = getK16SpecialCalleeName(Callee);
  if (Name == "__k16_read_trap_cause") {
    Csr = 2;
    return true;
  }
  if (Name == "__k16_read_trap_pc") {
    Csr = 3;
    return true;
  }
  if (Name == "__k16_read_trap_value") {
    Csr = 4;
    return true;
  }
  if (Name == "__k16_read_interrupt_pending") {
    Csr = 7;
    return true;
  }
  return false;
}

static bool getK16WriteCsrCallee(SDValue Callee, unsigned &Csr) {
  StringRef Name = getK16SpecialCalleeName(Callee);
  if (Name == "__k16_write_trap_vector") {
    Csr = 1;
    return true;
  }
  if (Name == "__k16_write_interrupt_enable") {
    Csr = 5;
    return true;
  }
  if (Name == "__k16_write_interrupt_mask") {
    Csr = 6;
    return true;
  }
  return false;
}

K16TargetLowering::K16TargetLowering(const TargetMachine &TM,
                                         const K16Subtarget &STI)
    : TargetLowering(TM, STI), Subtarget(STI) {
  addRegisterClass(MVT::i32, &K16::GPRRegClass);
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(STI.hasF32R32LR() ? K16::SP32
                                                         : K16::SP);
  setBooleanContents(ZeroOrOneBooleanContent);
  setMinFunctionAlignment(Align(2));
  setPrefFunctionAlignment(Align(2));
  setMaxAtomicSizeInBitsSupported(0);
  setMinimumJumpTableEntries(UINT_MAX);

  setOperationAction(ISD::BR_JT, MVT::Other, Expand);
  setOperationAction(ISD::MULHU, MVT::i32, Legal);
  setOperationAction(ISD::MULHS, MVT::i32, Legal);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setLoadExtAction(ISD::EXTLOAD, MVT::i32, MVT::i1, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i32, MVT::i1, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i32, MVT::i1, Promote);
  setOperationAction(ISD::FSHL, MVT::i32, Expand);
  setOperationAction(ISD::FSHR, MVT::i32, Expand);
  setOperationAction(ISD::ROTL, MVT::i32, Expand);
  setOperationAction(ISD::ROTR, MVT::i32, Expand);
  setOperationAction(ISD::CTLZ, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ, MVT::i32, Expand);
  setOperationAction(ISD::CTPOP, MVT::i32, Expand);
  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::SELECT, MVT::i32, Legal);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  setOperationAction(ISD::VAARG, MVT::Other, Expand);
  setOperationAction(ISD::VACOPY, MVT::Other, Expand);
  setOperationAction(ISD::VAEND, MVT::Other, Expand);
  setOperationAction(ISD::VASTART, MVT::Other, Custom);
}

SDValue K16TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::VASTART:
    return lowerVASTART(Op, DAG);
  default:
    llvm_unreachable("unexpected K16 custom lowering operation");
  }
}

SDValue K16TargetLowering::lowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  const auto *FuncInfo = MF.getInfo<K16MachineFunctionInfo>();
  SDValue VAListAddress = Op.getOperand(1);
  EVT PtrVT = VAListAddress.getValueType();
  SDValue FirstVarArg =
      DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(), PtrVT);
  const Value *SourceValue = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), SDLoc(Op), FirstVarArg, VAListAddress,
                      MachinePointerInfo(SourceValue));
}

SDValue K16TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  if (!isSupportedK16CallingConv(CallConv))
    report_fatal_error("K16 unsupported function calling convention");

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT PtrVT = getPointerTy(MF.getDataLayout());
  ArrayRef<MCPhysReg> ArgRegs = Subtarget.hasF32R32LR()
                                    ? ArrayRef(K16F32R32ArgRegs)
                                    : ArrayRef(K16ArgRegs);
  unsigned ReturnPcBytes = Subtarget.hasF32R32LR() ? 0 : K16ReturnPcBytes;

  for (unsigned I = 0, E = Ins.size(); I != E; ++I) {
    if (Ins[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 function arguments");

    if (I < ArgRegs.size()) {
      Register VReg = RegInfo.createVirtualRegister(&K16::GPRRegClass);
      RegInfo.addLiveIn(ArgRegs[I], VReg);
      InVals.push_back(DAG.getCopyFromReg(Chain, DL, VReg, MVT::i32));
      continue;
    }

    int64_t Offset =
        ReturnPcBytes + (I - ArgRegs.size()) * K16StackSlotBytes;
    int FI = MFI.CreateFixedObject(K16StackSlotBytes, Offset, true);
    SDValue Addr = DAG.getFrameIndex(FI, PtrVT);
    InVals.push_back(DAG.getLoad(
        MVT::i32, DL, Chain, Addr,
        MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
  }

  if (IsVarArg) {
    unsigned FixedStackArgCount = Ins.size() > ArgRegs.size()
                                      ? Ins.size() - ArgRegs.size()
                                      : 0;
    int64_t VarArgsOffset =
        ReturnPcBytes + FixedStackArgCount * K16StackSlotBytes;
    int FI = MFI.CreateFixedObject(K16StackSlotBytes, VarArgsOffset, true);
    MF.getInfo<K16MachineFunctionInfo>()->setVarArgsFrameIndex(FI);
  }

  return Chain;
}

SDValue K16TargetLowering::LowerReturn(
    SDValue Chain, CallingConv::ID CallConv, bool,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
    SelectionDAG &DAG) const {
  if (!isSupportedK16CallingConv(CallConv))
    report_fatal_error("K16 unsupported return calling convention");
  ArrayRef<MCPhysReg> RetRegs = Subtarget.hasF32R32LR()
                                    ? ArrayRef(K16F32R32RetRegs)
                                    : ArrayRef(K16RetRegs);
  if (Outs.size() > RetRegs.size())
    report_fatal_error("K16 has too many i32 return values for this ABI");

  SDValue Glue;
  SmallVector<SDValue, 8> RetOps(1, Chain);

  for (unsigned I = 0, E = Outs.size(); I != E; ++I) {
    if (Outs[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 return values");

    Chain = DAG.getCopyToReg(Chain, DL, RetRegs[I], OutVals[I], Glue);
    Glue = Chain.getValue(1);
    RetOps[0] = Chain;
    RetOps.push_back(DAG.getRegister(RetRegs[I], MVT::i32));
  }

  if (Glue.getNode())
    RetOps.push_back(Glue);

  return DAG.getNode(K16ISD::RET_FLAG, DL, MVT::Other, RetOps);
}

SDValue K16TargetLowering::LowerCall(
    CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc DL = CLI.DL;
  MachineFunction &MF = DAG.getMachineFunction();
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  SDValue InGlue;
  ArrayRef<MCPhysReg> ArgRegs = Subtarget.hasF32R32LR()
                                    ? ArrayRef(K16F32R32ArgRegs)
                                    : ArrayRef(K16ArgRegs);
  ArrayRef<MCPhysReg> RetRegs = Subtarget.hasF32R32LR()
                                    ? ArrayRef(K16F32R32RetRegs)
                                    : ArrayRef(K16RetRegs);
  unsigned ReturnPcBytes = Subtarget.hasF32R32LR() ? 0 : K16ReturnPcBytes;
  MCRegister StackReg = Subtarget.hasF32R32LR() ? K16::SP32 : K16::SP;

  CLI.IsTailCall = false;

  if (!isSupportedK16CallingConv(CLI.CallConv))
    report_fatal_error("K16 unsupported call calling convention");
  if (CLI.Ins.size() > RetRegs.size())
    report_fatal_error("K16 call has too many i32 return values for this ABI");
  if (CLI.Outs.empty() && CLI.Ins.empty() && isK16HaltOnceCallee(Callee))
    return DAG.getNode(K16ISD::HALT, DL, MVT::Other, Chain);
  if (CLI.Outs.empty() && CLI.Ins.empty() && isK16IretOnceCallee(Callee))
    return DAG.getNode(K16ISD::IRET, DL, MVT::Other, Chain);

  unsigned Csr = 0;
  if (CLI.Outs.empty() && CLI.Ins.size() == 1 &&
      CLI.Ins[0].VT == MVT::i32 && getK16ReadCsrCallee(Callee, Csr)) {
    SDValue CsrValue = DAG.getTargetConstant(Csr, DL, MVT::i32);
    SDValue Read = DAG.getNode(K16ISD::READ_CSR, DL,
                               DAG.getVTList(MVT::i32, MVT::Other), Chain,
                               CsrValue);
    InVals.push_back(Read);
    return Read.getValue(1);
  }
  if (CLI.Outs.size() == 1 && CLI.Outs[0].VT == MVT::i32 &&
      CLI.Ins.empty() && getK16WriteCsrCallee(Callee, Csr)) {
    SDValue CsrValue = DAG.getTargetConstant(Csr, DL, MVT::i32);
    return DAG.getNode(K16ISD::WRITE_CSR, DL, MVT::Other, Chain, CsrValue,
                       CLI.OutVals[0]);
  }

  unsigned FixedSlotCount = 0;
  for (const ISD::OutputArg &Out : CLI.Outs)
    if (!Out.Flags.isVarArg())
      ++FixedSlotCount;

  unsigned FixedStackBytes =
      FixedSlotCount > ArgRegs.size()
          ? (FixedSlotCount - ArgRegs.size()) * K16StackSlotBytes
          : 0;
  unsigned NextVarArgCalleeOffset = ReturnPcBytes + FixedStackBytes;
  unsigned FixedSlotIndex = 0;
  SmallVector<K16OutgoingArgLocation, 8> ArgLocations(CLI.Outs.size());

  for (unsigned I = 0, E = CLI.Outs.size(); I != E;) {
    const ISD::OutputArg &Out = CLI.Outs[I];
    if (!Out.Flags.isVarArg()) {
      if (Out.VT != MVT::i32)
        report_fatal_error("K16 only supports i32 fixed call fragments");
      if (FixedSlotIndex < ArgRegs.size())
        ArgLocations[I].Reg = ArgRegs[FixedSlotIndex];
      else
        ArgLocations[I].StackOffset =
            (FixedSlotIndex - ArgRegs.size()) * K16StackSlotBytes;
      ++FixedSlotIndex;
      ++I;
      continue;
    }

    unsigned GroupEnd = I + 1;
    while (GroupEnd != E && CLI.Outs[GroupEnd].Flags.isVarArg() &&
           CLI.Outs[GroupEnd].OrigArgIndex == Out.OrigArgIndex)
      ++GroupEnd;

    unsigned Alignment = Out.Flags.getNonZeroOrigAlign().value();
    if (Alignment > K16MaxArgumentAlignment)
      report_fatal_error(
          "K16 variadic arguments cannot require alignment above 8 bytes");

    unsigned GroupExtent = 0;
    for (unsigned Part = I; Part != GroupEnd; ++Part) {
      const ISD::OutputArg &Fragment = CLI.Outs[Part];
      unsigned ExpectedOffset = (Part - I) * K16StackSlotBytes;
      if (Fragment.VT != MVT::i32 || Fragment.PartOffset != ExpectedOffset)
        report_fatal_error(
            "K16 variadic arguments must lower to contiguous i32 fragments");
      GroupExtent = Fragment.PartOffset + K16StackSlotBytes;
    }

    GroupExtent = alignTo(GroupExtent, K16StackSlotBytes);
    NextVarArgCalleeOffset = alignTo(NextVarArgCalleeOffset, Alignment);
    unsigned GroupCallerOffset = NextVarArgCalleeOffset - ReturnPcBytes;
    for (unsigned Part = I; Part != GroupEnd; ++Part)
      ArgLocations[Part].StackOffset =
          GroupCallerOffset + CLI.Outs[Part].PartOffset;
    NextVarArgCalleeOffset += GroupExtent;
    I = GroupEnd;
  }

  unsigned CallFrameBytes = NextVarArgCalleeOffset - ReturnPcBytes;
  for (unsigned I = 0, E = CLI.Outs.size(); I != E; ++I) {
    const ISD::ArgFlagsTy &Flags = CLI.Outs[I].Flags;
    if (!Flags.isByVal())
      continue;
    unsigned Alignment = Flags.getNonZeroByValAlign().value();
    if (Alignment > K16MaxArgumentAlignment)
      report_fatal_error(
          "K16 aggregate arguments cannot require alignment above 8 bytes");
    unsigned CopyCalleeOffset =
        alignTo(ReturnPcBytes + CallFrameBytes, Alignment);
    ArgLocations[I].ByValCopyOffset = CopyCalleeOffset - ReturnPcBytes;
    CallFrameBytes = ArgLocations[I].ByValCopyOffset +
                     alignTo(Flags.getByValSize(), K16StackSlotBytes);
  }
  Chain = DAG.getCALLSEQ_START(Chain, CallFrameBytes, 0, DL);

  SmallVector<std::pair<MCRegister, SDValue>, 3> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  auto GetStackAddress = [&](unsigned Offset) {
    if (!StackPtr.getNode())
      StackPtr = DAG.getCopyFromReg(Chain, DL, StackReg, MVT::i32);
    if (Offset == 0)
      return StackPtr;
    return DAG.getNode(ISD::ADD, DL, MVT::i32, StackPtr,
                       DAG.getConstant(Offset, DL, MVT::i32));
  };

  for (unsigned I = 0, E = CLI.OutVals.size(); I != E; ++I) {
    const K16OutgoingArgLocation &Location = ArgLocations[I];
    SDValue OutValue = CLI.OutVals[I];
    if (Location.ByValCopyOffset != UINT_MAX) {
      const ISD::ArgFlagsTy &Flags = CLI.Outs[I].Flags;
      SDValue CopyAddress = GetStackAddress(Location.ByValCopyOffset);
      SDValue Size = DAG.getConstant(Flags.getByValSize(), DL, MVT::i32);
      MemOpChains.push_back(
          DAG.getMemcpy(Chain, DL, CopyAddress, OutValue, Size,
                        Flags.getNonZeroByValAlign(), /*isVolatile=*/false,
                        /*AlwaysInline=*/true, /*CI=*/nullptr, std::nullopt,
                        MachinePointerInfo::getStack(DAG.getMachineFunction(),
                                                     Location.ByValCopyOffset),
                        MachinePointerInfo()));
      OutValue = CopyAddress;
    }

    if (Location.Reg) {
      RegsToPass.push_back({Location.Reg, OutValue});
      continue;
    }

    unsigned Offset = Location.StackOffset;
    SDValue Addr = GetStackAddress(Offset);
    MemOpChains.push_back(DAG.getStore(
        Chain, DL, OutValue, Addr,
        MachinePointerInfo::getStack(DAG.getMachineFunction(), Offset)));
  }

  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  for (auto [Reg, Value] : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg, Value, InGlue);
    InGlue = Chain.getValue(1);
  }

  EVT PtrVT = getPointerTy(MF.getDataLayout());
  if (auto *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL, PtrVT,
                                        G->getOffset());
  } else if (auto *E = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), PtrVT);
  }

  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);
  for (auto [Reg, Value] : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg, Value.getValueType()));
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  Ops.push_back(
      DAG.getRegisterMask(TRI->getCallPreservedMask(MF, CLI.CallConv)));
  if (InGlue.getNode())
    Ops.push_back(InGlue);

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  Chain = DAG.getNode(K16ISD::CALL, DL, NodeTys, Ops);
  InGlue = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, CallFrameBytes, 0, InGlue, DL);
  InGlue = Chain.getValue(1);

  for (unsigned I = 0, E = CLI.Ins.size(); I != E; ++I) {
    if (CLI.Ins[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 call return values");
    SDValue Ret =
        DAG.getCopyFromReg(Chain, DL, RetRegs[I], MVT::i32, InGlue);
    Chain = Ret.getValue(1);
    InGlue = Chain.getValue(2);
    InVals.push_back(Ret);
  }

  return Chain;
}

const char *K16TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case K16ISD::RET_FLAG:
    return "K16ISD::RET_FLAG";
  case K16ISD::CALL:
    return "K16ISD::CALL";
  case K16ISD::HALT:
    return "K16ISD::HALT";
  case K16ISD::IRET:
    return "K16ISD::IRET";
  case K16ISD::READ_CSR:
    return "K16ISD::READ_CSR";
  case K16ISD::WRITE_CSR:
    return "K16ISD::WRITE_CSR";
  default:
    return nullptr;
  }
}
