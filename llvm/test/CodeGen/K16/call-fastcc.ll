; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

define i32 @caller() {
; CHECK-LABEL: caller:
; CHECK: const32 r1, 1
; CHECK: const32 r2, 2
; CHECK: call32 sum
; CHECK: ret
  %value = call fastcc i32 @sum(i32 1, i32 2)
  ret i32 %value
}

define fastcc i32 @sum(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: sum:
; CHECK: add r0, r1, r2
; CHECK: ret
  %value = add i32 %lhs, %rhs
  ret i32 %value
}
