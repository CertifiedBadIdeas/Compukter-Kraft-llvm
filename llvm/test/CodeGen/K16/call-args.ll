; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

define i32 @caller() {
; CHECK-LABEL: caller:
; CHECK: const32 r1, 1
; CHECK: const32 r2, 2
; CHECK: const32 r3, 3
; CHECK: call32 sum3
; CHECK: ret
  %value = call i32 @sum3(i32 1, i32 2, i32 3)
  ret i32 %value
}

define i32 @sum3(i32 %lhs, i32 %rhs, i32 %tail) {
; CHECK-LABEL: sum3:
; CHECK: add r0, r1, r2
; CHECK: add r0, r0, r3
; CHECK: ret
  %pair = add i32 %lhs, %rhs
  %sum = add i32 %pair, %tail
  ret i32 %sum
}
