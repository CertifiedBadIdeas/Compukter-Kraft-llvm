; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define ptr @stack_slot_address() {
; CHECK-LABEL: stack_slot_address:
; CHECK: const32 r13, 4
; CHECK: sub r15, r15, r13
; CHECK: const32 r13, 0
; CHECK: add r0, r15, r13
; CHECK: add r15, r15, r13
; CHECK: ret
entry:
  %slot = alloca i32
  ret ptr %slot
}
