; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

; ASM-LABEL: caller:
; ASM: call32 callee
; ASM: ret

; OBJ: Relocations [
; OBJ: Section ({{.*}}) .rela.text.k16 {
; OBJ: 0x2 R_K16_CALL32 callee 0x0
; OBJ: }
; OBJ: Hex dump of section '.text.k16':
; OBJ-NEXT: 0x00000000 01ee0000 0000008e 0090

declare i32 @callee()

define i32 @caller() {
  %value = call i32 @callee()
  ret i32 %value
}
