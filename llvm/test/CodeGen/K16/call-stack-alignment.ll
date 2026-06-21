; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

declare i32 @callee()

define i32 @caller() {
; CHECK-LABEL: caller:
; CHECK: const32 r13, 4
; CHECK: sub r15, r15, r13
; CHECK: call32 callee
; CHECK: const32 r13, 4
; CHECK: add r15, r15, r13
; CHECK: ret
entry:
  %value = call i32 @callee()
  ret i32 %value
}

