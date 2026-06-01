; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

; ASM-LABEL: wait_once:
; ASM: halt
; ASM: ret
; ASM-NOT: call32 __k16_halt_once

; OBJ: Hex dump of section '.text.k16':
; OBJ-NEXT: 0x00000000 01000090

declare void @__k16_halt_once()

define void @wait_once() {
  call void @__k16_halt_once()
  ret void
}
