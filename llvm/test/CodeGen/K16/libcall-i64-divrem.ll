; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i64 @unsigned_divide_i64(i64 %lhs, i64 %rhs) {
; CHECK-LABEL: unsigned_divide_i64:
; CHECK: call32 __udivdi3
; CHECK: ret
  %value = udiv i64 %lhs, %rhs
  ret i64 %value
}

define i64 @signed_divide_i64(i64 %lhs, i64 %rhs) {
; CHECK-LABEL: signed_divide_i64:
; CHECK: call32 __divdi3
; CHECK: ret
  %value = sdiv i64 %lhs, %rhs
  ret i64 %value
}

define i64 @unsigned_remainder_i64(i64 %lhs, i64 %rhs) {
; CHECK-LABEL: unsigned_remainder_i64:
; CHECK: call32 __umoddi3
; CHECK: ret
  %value = urem i64 %lhs, %rhs
  ret i64 %value
}

define i64 @signed_remainder_i64(i64 %lhs, i64 %rhs) {
; CHECK-LABEL: signed_remainder_i64:
; CHECK: call32 __moddi3
; CHECK: ret
  %value = srem i64 %lhs, %rhs
  ret i64 %value
}

define i128 @unsigned_divide_i128(i128 %lhs, i128 %rhs) {
; CHECK-LABEL: unsigned_divide_i128:
; CHECK: call32 __udivti3
; CHECK: ret
  %value = udiv i128 %lhs, %rhs
  ret i128 %value
}

define i128 @signed_divide_i128(i128 %lhs, i128 %rhs) {
; CHECK-LABEL: signed_divide_i128:
; CHECK: call32 __divti3
; CHECK: ret
  %value = sdiv i128 %lhs, %rhs
  ret i128 %value
}

define i128 @unsigned_remainder_i128(i128 %lhs, i128 %rhs) {
; CHECK-LABEL: unsigned_remainder_i128:
; CHECK: call32 __umodti3
; CHECK: ret
  %value = urem i128 %lhs, %rhs
  ret i128 %value
}

define i128 @signed_remainder_i128(i128 %lhs, i128 %rhs) {
; CHECK-LABEL: signed_remainder_i128:
; CHECK: call32 __modti3
; CHECK: ret
  %value = srem i128 %lhs, %rhs
  ret i128 %value
}
