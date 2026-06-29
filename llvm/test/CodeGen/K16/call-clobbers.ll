; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

declare i32 @callee(i32)

define i32 @caller(i32 %saved) {
; CHECK-LABEL: caller:
; CHECK: addi r15, r15, -12
; CHECK: store32 [r15 + 8], r1
; CHECK: call32 callee
; CHECK: load32 r1, [r15 + 8]
; CHECK: add r0, r1, r0
; CHECK: addi r15, r15, 12
; CHECK: ret
  %value = call i32 @callee(i32 1)
  %sum = add i32 %saved, %value
  ret i32 %sum
}
