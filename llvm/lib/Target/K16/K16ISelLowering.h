//===-- K16ISelLowering.h - K16 DAG Lowering Interface ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_K16_K16ISELLOWERING_H
#define LLVM_LIB_TARGET_K16_K16ISELLOWERING_H

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class K16Subtarget;

namespace K16ISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  CALL,
  HALT,
  IRET,
  READ_CSR,
  WRITE_CSR,
};
} // namespace K16ISD

class K16TargetLowering : public TargetLowering {
public:
  explicit K16TargetLowering(const TargetMachine &TM,
                               const K16Subtarget &STI);

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;
  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;
  const char *getTargetNodeName(unsigned Opcode) const override;
};

} // namespace llvm

#endif
