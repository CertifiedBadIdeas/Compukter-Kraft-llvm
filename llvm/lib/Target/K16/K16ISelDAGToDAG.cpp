//===-- K16ISelDAGToDAG.cpp - K16 DAG to DAG ISel --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16.h"
#include "K16TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"

using namespace llvm;

#define DEBUG_TYPE "k16-isel"
#define PASS_NAME "K16 DAG->DAG Pattern Instruction Selection"

namespace {

static unsigned getK16LoadOpcode(EVT MemoryVT) {
  if (MemoryVT == MVT::i8)
    return K16::LOAD8;
  if (MemoryVT == MVT::i16)
    return K16::LOAD16;
  if (MemoryVT == MVT::i32)
    return K16::LOAD32;
  return 0;
}

static unsigned getK16StoreOpcode(EVT MemoryVT) {
  if (MemoryVT == MVT::i8)
    return K16::STORE8;
  if (MemoryVT == MVT::i16)
    return K16::STORE16;
  if (MemoryVT == MVT::i32)
    return K16::STORE32;
  return 0;
}

class K16DAGToDAGISel : public SelectionDAGISel {
public:
  K16DAGToDAGISel(K16TargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISel(TM, OptLevel) {}

  void Select(SDNode *Node) override;

#include "K16GenDAGISel.inc"
};

class K16DAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
  static char ID;

  K16DAGToDAGISelLegacy(K16TargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISelLegacy(
            ID, std::make_unique<K16DAGToDAGISel>(TM, OptLevel)) {}
};

} // namespace

char K16DAGToDAGISelLegacy::ID;

INITIALIZE_PASS(K16DAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)

FunctionPass *llvm::createK16ISelDag(K16TargetMachine &TM,
                                       CodeGenOptLevel OptLevel) {
  return new K16DAGToDAGISelLegacy(TM, OptLevel);
}

void K16DAGToDAGISel::Select(SDNode *Node) {
  if (Node->isMachineOpcode()) {
    Node->setNodeId(-1);
    return;
  }

  SDLoc DL(Node);
  switch (Node->getOpcode()) {
  case ISD::FrameIndex: {
    auto *FI = cast<FrameIndexSDNode>(Node);
    SDValue TargetFI = CurDAG->getTargetFrameIndex(FI->getIndex(), MVT::i32);
    CurDAG->SelectNodeTo(Node, K16::FRAMEADDR, MVT::i32, TargetFI);
    return;
  }
  case ISD::GlobalAddress: {
    auto *GA = cast<GlobalAddressSDNode>(Node);
    SDValue Target =
        CurDAG->getTargetGlobalAddress(GA->getGlobal(), DL, MVT::i32,
                                       GA->getOffset());
    CurDAG->SelectNodeTo(Node, K16::CONST32, MVT::i32, Target);
    return;
  }
  case ISD::ExternalSymbol: {
    auto *ES = cast<ExternalSymbolSDNode>(Node);
    SDValue Target = CurDAG->getTargetExternalSymbol(ES->getSymbol(), MVT::i32);
    CurDAG->SelectNodeTo(Node, K16::CONST32, MVT::i32, Target);
    return;
  }
  case ISD::ConstantPool: {
    auto *CP = cast<ConstantPoolSDNode>(Node);
    SDValue Target;
    if (CP->isMachineConstantPoolEntry()) {
      Target = CurDAG->getTargetConstantPool(CP->getMachineCPVal(), MVT::i32,
                                             CP->getAlign(), CP->getOffset());
    } else {
      Target = CurDAG->getTargetConstantPool(CP->getConstVal(), MVT::i32,
                                             CP->getAlign(), CP->getOffset());
    }
    CurDAG->SelectNodeTo(Node, K16::CONST32, MVT::i32, Target);
    return;
  }
  case ISD::LOAD: {
    auto *Load = cast<LoadSDNode>(Node);
    unsigned Opcode = getK16LoadOpcode(Load->getMemoryVT());
    if (Opcode != 0) {
      if (auto *FI = dyn_cast<FrameIndexSDNode>(Load->getBasePtr())) {
        SDValue TargetFI = CurDAG->getTargetFrameIndex(FI->getIndex(), MVT::i32);
        SDValue Ops[] = {TargetFI, Load->getChain()};
        CurDAG->SelectNodeTo(Node, Opcode, CurDAG->getVTList(MVT::i32, MVT::Other),
                             Ops);
        return;
      }
    }
    break;
  }
  case ISD::ATOMIC_LOAD: {
    auto *Load = cast<AtomicSDNode>(Node);
    unsigned Opcode = getK16LoadOpcode(Load->getMemoryVT());
    if (Opcode != 0) {
      SDValue Ops[] = {Load->getBasePtr(), Load->getChain()};
      CurDAG->SelectNodeTo(Node, Opcode,
                           CurDAG->getVTList(MVT::i32, MVT::Other), Ops);
      return;
    }
    break;
  }
  case ISD::STORE: {
    auto *Store = cast<StoreSDNode>(Node);
    unsigned Opcode = getK16StoreOpcode(Store->getMemoryVT());
    if (Opcode != 0) {
      if (auto *FI = dyn_cast<FrameIndexSDNode>(Store->getBasePtr())) {
        SDValue TargetFI = CurDAG->getTargetFrameIndex(FI->getIndex(), MVT::i32);
        SDValue Ops[] = {TargetFI, Store->getValue(), Store->getChain()};
        CurDAG->SelectNodeTo(Node, Opcode, MVT::Other, Ops);
        return;
      }
    }
    break;
  }
  case ISD::ATOMIC_STORE: {
    auto *Store = cast<AtomicSDNode>(Node);
    unsigned Opcode = getK16StoreOpcode(Store->getMemoryVT());
    if (Opcode != 0) {
      SDValue Ops[] = {Store->getBasePtr(), Store->getVal(),
                       Store->getChain()};
      CurDAG->SelectNodeTo(Node, Opcode, MVT::Other, Ops);
      return;
    }
    break;
  }
  }

  SelectCode(Node);
}
