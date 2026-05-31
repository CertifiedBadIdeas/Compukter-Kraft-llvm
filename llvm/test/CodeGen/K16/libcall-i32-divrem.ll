; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @unsigned_divide(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: unsigned_divide:
; CHECK: call32 __udivsi3
; CHECK: ret
  %value = udiv i32 %lhs, %rhs
  ret i32 %value
}

define i32 @signed_divide(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: signed_divide:
; CHECK: call32 __divsi3
; CHECK: ret
  %value = sdiv i32 %lhs, %rhs
  ret i32 %value
}

define i32 @unsigned_remainder(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: unsigned_remainder:
; CHECK: call32 __umodsi3
; CHECK: ret
  %value = urem i32 %lhs, %rhs
  ret i32 %value
}

define i32 @signed_remainder(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: signed_remainder:
; CHECK: call32 __modsi3
; CHECK: ret
  %value = srem i32 %lhs, %rhs
  ret i32 %value
}
