; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @is_not_equal(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: is_not_equal:
; CHECK: ne
; CHECK: ret
entry:
  %cmp = icmp ne i32 %lhs, %rhs
  %value = zext i1 %cmp to i32
  ret i32 %value
}

define i32 @is_equal(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: is_equal:
; CHECK: eq
; CHECK: ret
entry:
  %cmp = icmp eq i32 %lhs, %rhs
  %value = zext i1 %cmp to i32
  ret i32 %value
}

define i32 @is_unsigned_less_or_equal(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: is_unsigned_less_or_equal:
; CHECK: ltu
; CHECK: xor
; CHECK: ret
entry:
  %cmp = icmp ule i32 %lhs, %rhs
  %value = zext i1 %cmp to i32
  ret i32 %value
}

define i32 @is_unsigned_greater_or_equal(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: is_unsigned_greater_or_equal:
; CHECK: ltu
; CHECK: xor
; CHECK: ret
entry:
  %cmp = icmp uge i32 %lhs, %rhs
  %value = zext i1 %cmp to i32
  ret i32 %value
}

define i32 @is_signed_less_or_equal(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: is_signed_less_or_equal:
; CHECK: lt_s
; CHECK: xor
; CHECK: ret
entry:
  %cmp = icmp sle i32 %lhs, %rhs
  %value = zext i1 %cmp to i32
  ret i32 %value
}

define i32 @is_signed_greater_or_equal(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: is_signed_greater_or_equal:
; CHECK: lt_s
; CHECK: xor
; CHECK: ret
entry:
  %cmp = icmp sge i32 %lhs, %rhs
  %value = zext i1 %cmp to i32
  ret i32 %value
}
