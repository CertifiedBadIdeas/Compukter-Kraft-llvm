; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -h -S -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

; OBJ: Format: elf32-unknown
; OBJ: Machine: 0x5258
; OBJ: Name: .text.k16
; OBJ: AddressAlignment: 2
; OBJ: Hex dump of section '.text.k16':
; OBJ-NEXT: 0x00000000 00201200 0090

define i32 @add(i32 %lhs, i32 %rhs) {
  %sum = add i32 %lhs, %rhs
  ret i32 %sum
}
