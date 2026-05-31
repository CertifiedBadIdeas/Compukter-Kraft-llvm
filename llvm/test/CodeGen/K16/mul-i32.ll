; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @mul_i32(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: mul_i32:
; CHECK: mul r0, r1, r2
; CHECK: ret
  %value = mul i32 %lhs, %rhs
  ret i32 %value
}
