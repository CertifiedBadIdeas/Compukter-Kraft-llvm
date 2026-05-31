; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

define i32 @stack_slot(i32 %value) {
; ASM-LABEL: stack_slot:
; ASM: const32 r13, 4
; ASM: sub r15, r15, r13
; ASM: store32 [r13], r1
; ASM: load32 r0, [r13]
; ASM: add r15, r15, r13
; ASM: ret

; OBJ: Hex dump of section '.text.k16':
  %slot = alloca i32
  store volatile i32 %value, ptr %slot
  %loaded = load volatile i32, ptr %slot
  ret i32 %loaded
}
