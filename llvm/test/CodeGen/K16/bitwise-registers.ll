; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @and_regs(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: and_regs:
; CHECK: and r0, r1, r2
; CHECK: ret
  %value = and i32 %lhs, %rhs
  ret i32 %value
}

define i32 @and_const(i32 %value) {
; CHECK-LABEL: and_const:
; CHECK: const32
; CHECK: and
; CHECK: ret
  %masked = and i32 %value, 1
  ret i32 %masked
}

define i32 @or_regs(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: or_regs:
; CHECK: or r0, r1, r2
; CHECK: ret
  %value = or i32 %lhs, %rhs
  ret i32 %value
}

define i32 @xor_regs(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: xor_regs:
; CHECK: xor r0, r1, r2
; CHECK: ret
  %value = xor i32 %lhs, %rhs
  ret i32 %value
}

define i32 @logical_shift_right(i32 %lhs, i32 %rhs) {
; CHECK-LABEL: logical_shift_right:
; CHECK: shr r0, r1, r2
; CHECK: ret
  %value = lshr i32 %lhs, %rhs
  ret i32 %value
}

define i32 @logical_shift_right_const(i32 %value) {
; CHECK-LABEL: logical_shift_right_const:
; CHECK: const32
; CHECK: shr
; CHECK: ret
  %shifted = lshr i32 %value, 8
  ret i32 %shifted
}
