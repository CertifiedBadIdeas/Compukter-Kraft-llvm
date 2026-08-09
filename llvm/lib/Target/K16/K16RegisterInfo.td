//===-- K16RegisterInfo.td - K16 register definitions --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class K16Reg<bits<5> Enc, string Name> : Register<Name> {
  let Namespace = "K16";
  let HWEncoding{4-0} = Enc;
  let DwarfNumbers = [Enc];
}

def R0  : K16Reg<0,  "r0">;
def R1  : K16Reg<1,  "r1">;
def R2  : K16Reg<2,  "r2">;
def R3  : K16Reg<3,  "r3">;
def R4  : K16Reg<4,  "r4">;
def R5  : K16Reg<5,  "r5">;
def R6  : K16Reg<6,  "r6">;
def R7  : K16Reg<7,  "r7">;
def R8  : K16Reg<8,  "r8">;
def R9  : K16Reg<9,  "r9">;
def R10 : K16Reg<10, "r10">;
def R11 : K16Reg<11, "r11">;
def FP  : K16Reg<12, "r12">;
def R13 : K16Reg<13, "r13">;
def R14 : K16Reg<14, "r14">;
def SP  : K16Reg<15, "r15">;
def R16 : K16Reg<16, "r16">;
def R17 : K16Reg<17, "r17">;
def R18 : K16Reg<18, "r18">;
def R19 : K16Reg<19, "r19">;
def R20 : K16Reg<20, "r20">;
def R21 : K16Reg<21, "r21">;
def R22 : K16Reg<22, "r22">;
def R23 : K16Reg<23, "r23">;
def R24 : K16Reg<24, "r24">;
def R25 : K16Reg<25, "r25">;
def R26 : K16Reg<26, "r26">;
def R27 : K16Reg<27, "r27">;
def R28 : K16Reg<28, "r28">;
def R29 : K16Reg<29, "r29">;
def SP32 : K16Reg<30, "r30">;
def LR : K16Reg<31, "r31">;

def GPR : RegisterClass<"K16", [i32], 32,
  (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, FP, R13, R14, SP)>;

def GPR32 : RegisterClass<"K16", [i32], 32,
  (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, FP, R13, R14, SP,
       R16, R17, R18, R19, R20, R21, R22, R23, R24, R25, R26, R27, R28, R29,
       SP32, LR)>;

def CSR_NoRegs : CalleeSavedRegs<(add)>;
def CSR_CallPreserved : CalleeSavedRegs<(add FP, SP)>;
def CSR_F32R32LR_Save : CalleeSavedRegs<
  (add R16, R17, R18, R19, R20, R21, R22, R23, R24, R25, R26, R27, R28, R29,
       LR)>;
def CSR_F32R32LR_Preserved : CalleeSavedRegs<
  (add R16, R17, R18, R19, R20, R21, R22, R23, R24, R25, R26, R27, R28, R29,
       SP32)>;
