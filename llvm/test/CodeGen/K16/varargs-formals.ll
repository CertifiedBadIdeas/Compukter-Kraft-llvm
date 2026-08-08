; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

declare void @llvm.va_start(ptr)

define void @va_start_after_register_fixed_arg(ptr %out, ...) {
; CHECK-LABEL: va_start_after_register_fixed_arg:
; CHECK: addi [[FIRST:r[0-9]+]], r15, 4
; CHECK: store32 [r1], [[FIRST]]
; CHECK: ret
  call void @llvm.va_start(ptr %out)
  ret void
}

define void @va_start_after_stack_fixed_arg(ptr %out, i32 %a, i32 %b,
                                            i32 %c, ...) {
; CHECK-LABEL: va_start_after_stack_fixed_arg:
; CHECK: addi [[FIRST:r[0-9]+]], r15, 8
; CHECK: store32 [r1], [[FIRST]]
; CHECK: ret
  call void @llvm.va_start(ptr %out)
  ret void
}

define i32 @fixed_arguments_still_use_registers_and_stack(i32 %a, i32 %b,
                                                          i32 %c, i32 %d) {
; CHECK-LABEL: fixed_arguments_still_use_registers_and_stack:
; CHECK: add r0, r1, r2
; CHECK: add r0, r0, r3
; CHECK: load32 [[D:r[0-9]+]], [r15 + 4]
; CHECK: add r0, r0, [[D]]
; CHECK: ret
  %ab = add i32 %a, %b
  %abc = add i32 %ab, %c
  %sum = add i32 %abc, %d
  ret i32 %sum
}
