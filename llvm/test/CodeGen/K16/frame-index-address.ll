; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define ptr @stack_slot_address() {
; CHECK-LABEL: stack_slot_address:
; CHECK: addi r15, r15, -8
; CHECK: addi r0, r15, 4
; CHECK: addi r15, r15, 8
; CHECK: ret
entry:
  %slot = alloca i32
  ret ptr %slot
}
