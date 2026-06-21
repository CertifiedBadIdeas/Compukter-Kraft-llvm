; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

; ASM-LABEL: caller:
; ASM: const32 r13, 4
; ASM: sub r15, r15, r13
; ASM: call32 callee
; ASM: const32 r13, 4
; ASM: add r15, r15, r13
; ASM: ret

; OBJ: Relocations [
; OBJ: Section ({{.*}}) .rela.text.k16 {
; OBJ: 0xC R_K16_CALL32 callee 0x0
; OBJ: }
; OBJ: Hex dump of section '.text.k16':
; OBJ-NEXT: 0x00000000 01ed0400 0000012f fd0001ee 00000000
; OBJ-NEXT: 0x00000010 008e01ed 04000000 002ffd00 0090

declare i32 @callee()

define i32 @caller() {
  %value = call i32 @callee()
  ret i32 %value
}
