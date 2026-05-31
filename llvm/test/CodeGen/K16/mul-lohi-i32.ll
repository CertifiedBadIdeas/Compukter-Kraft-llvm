; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i64 @signed_mul_lohi_i32(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: signed_mul_lohi_i32:
; CHECK: mul
; CHECK: mulh_s
; CHECK: ret
entry:
  %lhs64 = sext i32 %lhs to i64
  %rhs64 = sext i32 %rhs to i64
  %product = mul i64 %lhs64, %rhs64
  ret i64 %product
}

define i64 @unsigned_mul_lohi_i32(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: unsigned_mul_lohi_i32:
; CHECK: mul
; CHECK: mulh_u
; CHECK: ret
entry:
  %lhs64 = zext i32 %lhs to i64
  %rhs64 = zext i32 %rhs to i64
  %product = mul i64 %lhs64, %rhs64
  ret i64 %product
}
