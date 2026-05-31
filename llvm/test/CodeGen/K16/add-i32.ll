; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @add(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: add:
; CHECK: add r0, r1, r2
; CHECK: ret
  %sum = add i32 %lhs, %rhs
  ret i32 %sum
}
