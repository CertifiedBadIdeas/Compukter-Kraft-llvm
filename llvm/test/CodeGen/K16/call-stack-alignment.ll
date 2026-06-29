; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

declare i32 @callee()

define i32 @caller() {
; CHECK-LABEL: caller:
; CHECK: addi r15, r15, -4
; CHECK: call32 callee
; CHECK: addi r15, r15, 4
; CHECK: ret
entry:
  %value = call i32 @callee()
  ret i32 %value
}
