; RUN: llc -mtriple=k16 < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r %t | FileCheck %s --check-prefix=OBJ

declare void @external_target()

define ptr @external_addr() {
; ASM-LABEL: external_addr:
; ASM: const32 r0, external_target
; ASM: ret
entry:
  ret ptr @external_target
}

; OBJ: R_K16_ABS32
