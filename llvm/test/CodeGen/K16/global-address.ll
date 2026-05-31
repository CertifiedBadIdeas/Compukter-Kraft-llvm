; RUN: llc -mtriple=k16 < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r %t | FileCheck %s --check-prefix=OBJ

@message = private unnamed_addr constant [4 x i8] c"K16\00", align 1

define ptr @message_addr() {
; ASM-LABEL: message_addr:
; ASM: const32 r0, message
; ASM: ret
entry:
  ret ptr @message
}

; OBJ: R_K16_ABS32
