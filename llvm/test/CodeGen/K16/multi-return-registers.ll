; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare { i32, i32 } @pair_from_callee(i32 %value)

define { i32, i32 } @return_pair(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: return_pair:
; CHECK: add r0, r1, r2
; CHECK: sub r1, r1, r2
; CHECK: ret
  %sum = add i32 %lhs, %rhs
  %diff = sub i32 %lhs, %rhs
  %pair0 = insertvalue { i32, i32 } poison, i32 %sum, 0
  %pair1 = insertvalue { i32, i32 } %pair0, i32 %diff, 1
  ret { i32, i32 } %pair1
}

define i32 @consume_pair(i32 %value) {
; CHECK-LABEL: consume_pair:
; CHECK: call32 pair_from_callee
; CHECK: add r0, r0, r1
; CHECK: ret
  %pair = call { i32, i32 } @pair_from_callee(i32 %value)
  %first = extractvalue { i32, i32 } %pair, 0
  %second = extractvalue { i32, i32 } %pair, 1
  %sum = add i32 %first, %second
  ret i32 %sum
}
