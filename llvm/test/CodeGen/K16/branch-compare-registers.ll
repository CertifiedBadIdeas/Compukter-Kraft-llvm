; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @checked_add_i8_shape(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: checked_add_i8_shape:
; CHECK: add
; CHECK: shl
; CHECK: sar
; CHECK: sub
; CHECK: brz
; CHECK: ret
entry:
  %sum = add i32 %lhs, %rhs
  %sum_i8 = trunc i32 %sum to i8
  %sum_sext = sext i8 %sum_i8 to i32
  %overflow = icmp ne i32 %sum_sext, %sum
  br i1 %overflow, label %overflowed, label %ok

overflowed:
  ret i32 1

ok:
  ret i32 0
}
