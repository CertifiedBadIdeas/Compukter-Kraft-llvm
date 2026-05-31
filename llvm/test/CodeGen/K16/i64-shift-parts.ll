; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i64 @shift_left_i64(i64 %value, i32 %amount) {
; CHECK-LABEL: shift_left_i64:
; CHECK: shl
; CHECK: ret
entry:
  %wide_amount = zext i32 %amount to i64
  %shifted = shl i64 %value, %wide_amount
  ret i64 %shifted
}

define i64 @shift_right_i64(i64 %value, i32 %amount) {
; CHECK-LABEL: shift_right_i64:
; CHECK: shr
; CHECK: ret
entry:
  %wide_amount = zext i32 %amount to i64
  %shifted = lshr i64 %value, %wide_amount
  ret i64 %shifted
}
