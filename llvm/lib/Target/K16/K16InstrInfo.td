//===-- K16InstrInfo.td - K16 instruction definitions --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

def SDT_K16RetFlag : SDTypeProfile<0, 0, []>;
def SDT_K16Call : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                         [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def K16retflag : SDNode<"K16ISD::RET_FLAG", SDT_K16RetFlag,
                          [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def K16call : SDNode<"K16ISD::CALL", SDT_K16Call,
                       [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                        SDNPVariadic]>;

def frameaddr : Operand<iPTR>;
def calltarget : Operand<iPTR>;
def brtarget : Operand<OtherVT>;

class K16Inst<dag outs, dag ins, string asmstr, list<dag> pattern = [],
                int size = 2>
    : Instruction {
  let Namespace = "K16";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
  let Size = size;
}

class K16Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern = []>
    : K16Inst<outs, ins, asmstr, pattern, 0> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

let isReMaterializable = 1, isAsCheapAsAMove = 1, isMoveImm = 1 in
def CONST32 : K16Inst<(outs GPR:$dst), (ins i32imm:$imm),
                        "const32 $dst, $imm",
                        [(set GPR:$dst, imm:$imm)], 6>;

def ADD : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "add $dst, $lhs, $rhs",
                    [(set GPR:$dst, (add GPR:$lhs, GPR:$rhs))], 4>;

def SUB : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "sub $dst, $lhs, $rhs",
                    [(set GPR:$dst, (sub GPR:$lhs, GPR:$rhs))], 4>;

def MUL : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "mul $dst, $lhs, $rhs",
                    [(set GPR:$dst, (mul GPR:$lhs, GPR:$rhs))], 4>;

def MULHU : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                      "mulh_u $dst, $lhs, $rhs",
                      [(set GPR:$dst, (mulhu GPR:$lhs, GPR:$rhs))], 4>;

def MULHS : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                      "mulh_s $dst, $lhs, $rhs",
                      [(set GPR:$dst, (mulhs GPR:$lhs, GPR:$rhs))], 4>;

def AND : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "and $dst, $lhs, $rhs",
                    [(set GPR:$dst, (and GPR:$lhs, GPR:$rhs))], 4>;

def OR : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                   "or $dst, $lhs, $rhs",
                   [(set GPR:$dst, (or GPR:$lhs, GPR:$rhs))], 4>;

def XOR : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "xor $dst, $lhs, $rhs",
                    [(set GPR:$dst, (xor GPR:$lhs, GPR:$rhs))], 4>;

def SHL : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "shl $dst, $lhs, $rhs",
                    [(set GPR:$dst, (shl GPR:$lhs, GPR:$rhs))], 4>;
def : Pat<(shl GPR:$lhs, imm:$rhs), (SHL GPR:$lhs, (CONST32 imm:$rhs))>;

def SHR : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "shr $dst, $lhs, $rhs",
                    [(set GPR:$dst, (srl GPR:$lhs, GPR:$rhs))], 4>;
def : Pat<(srl GPR:$lhs, imm:$rhs), (SHR GPR:$lhs, (CONST32 imm:$rhs))>;

def SAR : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "sar $dst, $lhs, $rhs",
                    [(set GPR:$dst, (sra GPR:$lhs, GPR:$rhs))], 4>;
def : Pat<(sra GPR:$lhs, imm:$rhs), (SAR GPR:$lhs, (CONST32 imm:$rhs))>;

def EQ : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                   "eq $dst, $lhs, $rhs",
                   [], 4>;

def NE : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                   "ne $dst, $lhs, $rhs",
                   [], 4>;

def LTU : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                    "ltu $dst, $lhs, $rhs",
                    [], 4>;

def LT_S : K16Inst<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs),
                     "lt_s $dst, $lhs, $rhs",
                     [], 4>;

let mayLoad = 1 in
def LOAD8 : K16Inst<(outs GPR:$dst), (ins GPR:$addr),
                      "load8 $dst, [$addr]",
                      [(set GPR:$dst, (zextloadi8 GPR:$addr))]>;

def : Pat<(extloadi8 GPR:$addr), (LOAD8 GPR:$addr)>;
def : Pat<(sextloadi8 GPR:$addr),
          (SAR (SHL (LOAD8 GPR:$addr), (CONST32 24)), (CONST32 24))>;

let mayLoad = 1 in
def LOAD16 : K16Inst<(outs GPR:$dst), (ins GPR:$addr),
                       "load16 $dst, [$addr]",
                       [(set GPR:$dst, (zextloadi16 GPR:$addr))]>;

def : Pat<(extloadi16 GPR:$addr), (LOAD16 GPR:$addr)>;
def : Pat<(sextloadi16 GPR:$addr),
          (SAR (SHL (LOAD16 GPR:$addr), (CONST32 16)), (CONST32 16))>;

let mayLoad = 1 in
def LOAD32 : K16Inst<(outs GPR:$dst), (ins GPR:$addr),
                       "load32 $dst, [$addr]",
                       [(set GPR:$dst, (load GPR:$addr))]>;

let mayStore = 1 in
def STORE8 : K16Inst<(outs), (ins GPR:$addr, GPR:$src),
                       "store8 [$addr], $src",
                       [(truncstorei8 GPR:$src, GPR:$addr)]>;

let mayStore = 1 in
def STORE16 : K16Inst<(outs), (ins GPR:$addr, GPR:$src),
                        "store16 [$addr], $src",
                        [(truncstorei16 GPR:$src, GPR:$addr)]>;

let mayStore = 1 in
def STORE32 : K16Inst<(outs), (ins GPR:$addr, GPR:$src),
                        "store32 [$addr], $src",
                        [(store GPR:$src, GPR:$addr)]>;

let isCodeGenOnly = 1, Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : K16Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                   "#ADJCALLSTACKDOWN $amt1 $amt2",
                                   [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP : K16Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                 "#ADJCALLSTACKUP $amt1 $amt2",
                                 [(callseq_end timm:$amt1, timm:$amt2)]>;
}

def FRAMEADDR : K16Pseudo<(outs GPR:$dst), (ins frameaddr:$base),
                            "#FRAMEADDR $dst, $base", []>;

let isCall = 1, Defs = [R0, R14], Uses = [SP] in
def CALL32 : K16Inst<(outs), (ins calltarget:$target, variable_ops),
                       "call32 $target", [], 8>;

let isCall = 1, Defs = [R0, R14], Uses = [SP] in
def CALLR : K16Inst<(outs), (ins GPR:$target, variable_ops),
                      "call $target", [], 2>;

def : Pat<(K16call tglobaladdr:$target), (CALL32 tglobaladdr:$target)>;
def : Pat<(K16call texternalsym:$target), (CALL32 texternalsym:$target)>;
def : Pat<(K16call GPR:$target), (CALLR GPR:$target)>;

let isBranch = 1, isTerminator = 1, isBarrier = 1, Defs = [R14] in
def BR : K16Inst<(outs), (ins brtarget:$target),
                   "br $target",
                   [(br bb:$target)], 8>;

let isBranch = 1, isTerminator = 1, Defs = [R14] in
def BRZ : K16Inst<(outs), (ins GPR:$cond, brtarget:$target),
                    "brz $cond, $target",
                    [], 10>;

let isBranch = 1, isTerminator = 1, Defs = [R14] in
def BRNZ : K16Inst<(outs), (ins GPR:$cond, brtarget:$target),
                     "brnz $cond, $target",
                     [(brcond GPR:$cond, bb:$target)], 10>;

let isBranch = 1, isTerminator = 1, Defs = [R14] in
def BRLTU : K16Inst<(outs), (ins GPR:$lhs, GPR:$rhs, brtarget:$target),
                      "brltu $lhs, $rhs, $target",
                      [(brcc SETULT, GPR:$lhs, GPR:$rhs, bb:$target)], 14>;

let isBranch = 1, isTerminator = 1, Defs = [R14] in
def BRUGE : K16Inst<(outs), (ins GPR:$lhs, GPR:$rhs, brtarget:$target),
                      "bruge $lhs, $rhs, $target",
                      [(brcc SETUGE, GPR:$lhs, GPR:$rhs, bb:$target)], 14>;

def : Pat<(setult GPR:$lhs, GPR:$rhs), (LTU GPR:$lhs, GPR:$rhs)>;
def : Pat<(setugt GPR:$lhs, GPR:$rhs), (LTU GPR:$rhs, GPR:$lhs)>;
def : Pat<(setule GPR:$lhs, GPR:$rhs),
          (XOR (LTU GPR:$rhs, GPR:$lhs), (CONST32 1))>;
def : Pat<(setuge GPR:$lhs, GPR:$rhs),
          (XOR (LTU GPR:$lhs, GPR:$rhs), (CONST32 1))>;
def : Pat<(setlt GPR:$lhs, GPR:$rhs), (LT_S GPR:$lhs, GPR:$rhs)>;
def : Pat<(setgt GPR:$lhs, GPR:$rhs), (LT_S GPR:$rhs, GPR:$lhs)>;
def : Pat<(setle GPR:$lhs, GPR:$rhs),
          (XOR (LT_S GPR:$rhs, GPR:$lhs), (CONST32 1))>;
def : Pat<(setge GPR:$lhs, GPR:$rhs),
          (XOR (LT_S GPR:$lhs, GPR:$rhs), (CONST32 1))>;
def : Pat<(seteq GPR:$lhs, GPR:$rhs), (EQ GPR:$lhs, GPR:$rhs)>;
def : Pat<(setne GPR:$lhs, GPR:$rhs), (NE GPR:$lhs, GPR:$rhs)>;
def : Pat<(select GPR:$cond, GPR:$true_value, GPR:$false_value),
          (XOR GPR:$false_value,
               (AND (XOR GPR:$true_value, GPR:$false_value),
                    (SUB (CONST32 0), GPR:$cond)))>;
def : Pat<(sext_inreg GPR:$src, i8),
          (SAR (SHL GPR:$src, (CONST32 24)), (CONST32 24))>;
def : Pat<(sext_inreg GPR:$src, i16),
          (SAR (SHL GPR:$src, (CONST32 16)), (CONST32 16))>;
def : Pat<(brcc SETNE, GPR:$lhs, (i32 0), bb:$target),
          (BRNZ GPR:$lhs, bb:$target)>;
def : Pat<(brcc SETNE, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRNZ (SUB GPR:$lhs, GPR:$rhs), bb:$target)>;
def : Pat<(brcc SETEQ, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRZ (SUB GPR:$lhs, GPR:$rhs), bb:$target)>;
def : Pat<(brcc SETUGT, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRLTU GPR:$rhs, GPR:$lhs, bb:$target)>;
def : Pat<(brcc SETULE, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRUGE GPR:$rhs, GPR:$lhs, bb:$target)>;
def : Pat<(brcc SETLT, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRNZ (LT_S GPR:$lhs, GPR:$rhs), bb:$target)>;
def : Pat<(brcc SETGT, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRNZ (LT_S GPR:$rhs, GPR:$lhs), bb:$target)>;
def : Pat<(brcc SETGE, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRZ (LT_S GPR:$lhs, GPR:$rhs), bb:$target)>;
def : Pat<(brcc SETLE, GPR:$lhs, GPR:$rhs, bb:$target),
          (BRZ (LT_S GPR:$rhs, GPR:$lhs), bb:$target)>;

let isTerminator = 1, isReturn = 1, isBarrier = 1 in
def RET : K16Inst<(outs), (ins), "ret", [(K16retflag)]>;
