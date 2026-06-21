; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

define i32 @caller() {
; CHECK-LABEL: caller:
; CHECK: const32 r13, 12
; CHECK: sub r15, r15, r13
; CHECK: const32 [[ARG:r[0-9]+]], 4
; CHECK: store32 [r15], [[ARG]]
; CHECK: const32 r1, 1
; CHECK: const32 r2, 2
; CHECK: const32 r3, 3
; CHECK: call32 sum4
; CHECK: const32 r13, 12
; CHECK: add r15, r15, r13
; CHECK: ret
  %value = call i32 @sum4(i32 1, i32 2, i32 3, i32 4)
  ret i32 %value
}

define i32 @sum4(i32 %a, i32 %b, i32 %c, i32 %d) {
; CHECK-LABEL: sum4:
; CHECK: add r0, r1, r2
; CHECK: add r0, r0, r3
; CHECK: const32 [[RETURN_PC_BYTES:r[0-9]+]], 4
; CHECK: add [[STACK_ARG_ADDR:r[0-9]+]], r15, [[RETURN_PC_BYTES]]
; CHECK: load32 [[D:r[0-9]+]], [[[STACK_ARG_ADDR]]]
; CHECK: add r0, r0, [[D]]
; CHECK: ret
  %ab = add i32 %a, %b
  %abc = add i32 %ab, %c
  %sum = add i32 %abc, %d
  ret i32 %sum
}

define i32 @call_stack_arg_after_reserving_outgoing_arg(i32 %a, i32 %b, i32 %c, i32 %d) {
; CHECK-LABEL: call_stack_arg_after_reserving_outgoing_arg:
; CHECK: const32 r13, 12
; CHECK: sub r15, r15, r13
; CHECK: const32 [[STACK_ARG_OFFSET:r[0-9]+]], 16
; CHECK: add [[STACK_ARG_ADDR:r[0-9]+]], r15, [[STACK_ARG_OFFSET]]
; CHECK: load32 [[D:r[0-9]+]], [[[STACK_ARG_ADDR]]]
; CHECK: store32 [r15], [[D]]
; CHECK: call32 use_stack_arg
; CHECK: const32 r13, 12
; CHECK: add r15, r15, r13
; CHECK: ret
  %value = call i32 @use_stack_arg(i32 %d, i32 %d, i32 %d, i32 %d)
  ret i32 %value
}

declare i32 @use_stack_arg(i32, i32, i32, i32)
