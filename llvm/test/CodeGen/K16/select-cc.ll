; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @select_eq(i32 %lhs, i32 %rhs, i32 %when_true, i32 %when_false) {
; CHECK-LABEL: select_eq:
; CHECK: eq
; CHECK: sub
; CHECK: xor
; CHECK: and
; CHECK: xor
; CHECK: ret
entry:
  %cmp = icmp eq i32 %lhs, %rhs
  %value = select i1 %cmp, i32 %when_true, i32 %when_false
  ret i32 %value
}
