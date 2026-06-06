//===-- K16ISelLowering.cpp - K16 DAG Lowering Implementation --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16ISelLowering.h"
#include "K16Subtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/ADT/StringRef.h"
#include <climits>
#include <cstring>

using namespace llvm;

static const MCPhysReg K16ArgRegs[] = {K16::R1, K16::R2, K16::R3};
static const MCPhysReg K16RetRegs[] = {K16::R0, K16::R1, K16::R2, K16::R3};
static constexpr unsigned K16StackSlotBytes = 4;
static constexpr unsigned K16ReturnPcBytes = 4;

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
    : TargetLowering(TM, STI) {
  addRegisterClass(MVT::i32, &K16::GPRRegClass);
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(K16::SP);
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
}

SDValue K16TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  if (!isSupportedK16CallingConv(CallConv))
    report_fatal_error("K16 unsupported function calling convention");
  if (IsVarArg)
    report_fatal_error("K16 varargs are not implemented");

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT PtrVT = getPointerTy(MF.getDataLayout());

  for (unsigned I = 0, E = Ins.size(); I != E; ++I) {
    if (Ins[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 function arguments");

    if (I < std::size(K16ArgRegs)) {
      Register VReg = RegInfo.createVirtualRegister(&K16::GPRRegClass);
      RegInfo.addLiveIn(K16ArgRegs[I], VReg);
      InVals.push_back(DAG.getCopyFromReg(Chain, DL, VReg, MVT::i32));
      continue;
    }

    int64_t Offset =
        K16ReturnPcBytes + (I - std::size(K16ArgRegs)) * K16StackSlotBytes;
    int FI = MFI.CreateFixedObject(K16StackSlotBytes, Offset, true);
    SDValue Addr = DAG.getFrameIndex(FI, PtrVT);
    InVals.push_back(DAG.getLoad(
        MVT::i32, DL, Chain, Addr,
        MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
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
  if (Outs.size() > std::size(K16RetRegs))
    report_fatal_error("K16 supports at most four i32 return values");

  SDValue Glue;
  SmallVector<SDValue, 8> RetOps(1, Chain);

  for (unsigned I = 0, E = Outs.size(); I != E; ++I) {
    if (Outs[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 return values");

    Chain = DAG.getCopyToReg(Chain, DL, K16RetRegs[I], OutVals[I], Glue);
    Glue = Chain.getValue(1);
    RetOps[0] = Chain;
    RetOps.push_back(DAG.getRegister(K16RetRegs[I], MVT::i32));
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

  CLI.IsTailCall = false;

  if (!isSupportedK16CallingConv(CLI.CallConv))
    report_fatal_error("K16 unsupported call calling convention");
  if (CLI.IsVarArg)
    report_fatal_error("K16 varargs calls are not implemented");
  if (CLI.Ins.size() > std::size(K16RetRegs))
    report_fatal_error("K16 calls support at most four i32 return values");
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

  unsigned StackArgCount =
      CLI.Outs.size() > std::size(K16ArgRegs)
          ? CLI.Outs.size() - std::size(K16ArgRegs)
          : 0;
  unsigned StackArgBytes = StackArgCount * K16StackSlotBytes;
  Chain = DAG.getCALLSEQ_START(Chain, StackArgBytes, 0, DL);

  SmallVector<std::pair<MCRegister, SDValue>, 3> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  for (unsigned I = 0, E = CLI.OutVals.size(); I != E; ++I) {
    if (CLI.Outs[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 call arguments");
    if (I < std::size(K16ArgRegs)) {
      RegsToPass.push_back({K16ArgRegs[I], CLI.OutVals[I]});
      continue;
    }

    if (!StackPtr.getNode())
      StackPtr = DAG.getCopyFromReg(Chain, DL, K16::SP, MVT::i32);
    unsigned Offset = (I - std::size(K16ArgRegs)) * K16StackSlotBytes;
    SDValue Addr =
        Offset == 0
            ? StackPtr
            : DAG.getNode(ISD::ADD, DL, MVT::i32, StackPtr,
                          DAG.getConstant(Offset, DL, MVT::i32));
    MemOpChains.push_back(DAG.getStore(
        Chain, DL, CLI.OutVals[I], Addr,
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

  Chain = DAG.getCALLSEQ_END(Chain, StackArgBytes, 0, InGlue, DL);
  InGlue = Chain.getValue(1);

  for (unsigned I = 0, E = CLI.Ins.size(); I != E; ++I) {
    if (CLI.Ins[I].VT != MVT::i32)
      report_fatal_error("K16 only supports i32 call return values");
    SDValue Ret =
        DAG.getCopyFromReg(Chain, DL, K16RetRegs[I], MVT::i32, InGlue);
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
