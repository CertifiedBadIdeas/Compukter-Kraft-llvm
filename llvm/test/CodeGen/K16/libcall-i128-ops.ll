; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare i64 @llvm.ctlz.i64(i64, i1 immarg)
declare i128 @llvm.ctlz.i128(i128, i1 immarg)
declare i64 @llvm.ctpop.i64(i64)
declare i128 @llvm.ctpop.i128(i128)
declare { i32, i1 } @llvm.smul.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.smul.with.overflow.i64(i64, i64)
declare { i128, i1 } @llvm.smul.with.overflow.i128(i128, i128)

define i64 @multiply_i64(i64 %lhs, i64 %rhs) {
; CHECK-LABEL: multiply_i64:
; CHECK: ret
entry:
  %product = mul i64 %lhs, %rhs
  ret i64 %product
}

define i128 @shift_left_i128(i128 %value, i32 %amount) {
; CHECK-LABEL: shift_left_i128:
; CHECK: ret
entry:
  %wide_amount = zext i32 %amount to i128
  %shifted = shl i128 %value, %wide_amount
  ret i128 %shifted
}

define i128 @logical_shift_right_i128(i128 %value, i32 %amount) {
; CHECK-LABEL: logical_shift_right_i128:
; CHECK: ret
entry:
  %wide_amount = zext i32 %amount to i128
  %shifted = lshr i128 %value, %wide_amount
  ret i128 %shifted
}

define i128 @arithmetic_shift_right_i128(i128 %value, i32 %amount) {
; CHECK-LABEL: arithmetic_shift_right_i128:
; CHECK: ret
entry:
  %wide_amount = zext i32 %amount to i128
  %shifted = ashr i128 %value, %wide_amount
  ret i128 %shifted
}

define i128 @multiply_i128(i128 %lhs, i128 %rhs) {
; CHECK-LABEL: multiply_i128:
; CHECK: call32 __multi3
; CHECK: ret
entry:
  %product = mul i128 %lhs, %rhs
  ret i128 %product
}

define i32 @signed_multiply_overflow_i32(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: signed_multiply_overflow_i32:
; CHECK: ret
entry:
  %result = call { i32, i1 } @llvm.smul.with.overflow.i32(i32 %lhs, i32 %rhs)
  %product = extractvalue { i32, i1 } %result, 0
  ret i32 %product
}

define i64 @signed_multiply_overflow_i64(i64 %lhs, i64 %rhs) {
; CHECK-LABEL: signed_multiply_overflow_i64:
; CHECK: call32 __mulodi4
; CHECK: ret
entry:
  %result = call { i64, i1 } @llvm.smul.with.overflow.i64(i64 %lhs, i64 %rhs)
  %product = extractvalue { i64, i1 } %result, 0
  ret i64 %product
}

define i128 @signed_multiply_overflow_i128(i128 %lhs, i128 %rhs) {
; CHECK-LABEL: signed_multiply_overflow_i128:
; CHECK: call32 __muloti4
; CHECK: ret
entry:
  %result = call { i128, i1 } @llvm.smul.with.overflow.i128(i128 %lhs, i128 %rhs)
  %product = extractvalue { i128, i1 } %result, 0
  ret i128 %product
}

define i64 @count_leading_zeroes_i64(i64 %value) {
; CHECK-LABEL: count_leading_zeroes_i64:
; CHECK: ret
entry:
  %count = call i64 @llvm.ctlz.i64(i64 %value, i1 false)
  ret i64 %count
}

define i128 @count_leading_zeroes_i128(i128 %value) {
; CHECK-LABEL: count_leading_zeroes_i128:
; CHECK: ret
entry:
  %count = call i128 @llvm.ctlz.i128(i128 %value, i1 false)
  ret i128 %count
}

define i64 @population_count_i64(i64 %value) {
; CHECK-LABEL: population_count_i64:
; CHECK: ret
entry:
  %count = call i64 @llvm.ctpop.i64(i64 %value)
  ret i64 %count
}

define i128 @population_count_i128(i128 %value) {
; CHECK-LABEL: population_count_i128:
; CHECK: ret
entry:
  %count = call i128 @llvm.ctpop.i128(i128 %value)
  ret i128 %count
}
