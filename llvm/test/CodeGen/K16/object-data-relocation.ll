; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r %t | FileCheck %s --check-prefix=OBJ

@target = external global i32
@ptr = global ptr @target, align 4

; OBJ: R_K16_ABS32 target 0x0
