//===-- K16.td - Describe the K16 target machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

def : Processor<"generic", NoItineraries, []>;

include "K16RegisterInfo.td"

defm : RemapAllTargetPseudoPointerOperands<GPR>;

include "K16InstrInfo.td"

def K16InstrInfo : InstrInfo;

def K16 : Target {
  let InstructionSet = K16InstrInfo;
}
