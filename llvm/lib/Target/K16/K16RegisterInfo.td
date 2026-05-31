//===-- K16RegisterInfo.td - K16 register definitions --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class K16Reg<bits<4> Enc, string Name> : Register<Name> {
  let Namespace = "K16";
  let HWEncoding{3-0} = Enc;
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

def GPR : RegisterClass<"K16", [i32], 32,
  (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, FP, R13, R14, SP)>;

def CSR_NoRegs : CalleeSavedRegs<(add)>;
def CSR_CallPreserved : CalleeSavedRegs<(add FP, SP)>;
