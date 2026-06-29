; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

define i32 @stack_slot(i32 %value) {
; ASM-LABEL: stack_slot:
; ASM: addi r15, r15, -8
; ASM: store32 [r15 + 4], r1
; ASM: load32 r0, [r15 + 4]
; ASM: addi r15, r15, 8
; ASM: ret

; OBJ: Hex dump of section '.text.k16':
  %slot = alloca i32
  store volatile i32 %value, ptr %slot
  %loaded = load volatile i32, ptr %slot
  ret i32 %loaded
}
