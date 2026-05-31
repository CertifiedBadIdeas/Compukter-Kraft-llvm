; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

declare i32 @callee(i32)

define i32 @caller(i32 %saved) {
; CHECK-LABEL: caller:
; CHECK: sub r15, r15, r13
; CHECK: store32 [r13], r1
; CHECK: call32 callee
; CHECK: load32 r1, [r13]
; CHECK: add r0, r1, r0
; CHECK: add r15, r15, r13
; CHECK: ret
  %value = call i32 @callee(i32 1)
  %sum = add i32 %saved, %value
  ret i32 %sum
}
