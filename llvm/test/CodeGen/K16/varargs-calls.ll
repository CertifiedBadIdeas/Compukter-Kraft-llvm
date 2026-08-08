; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

%Pair = type { i32, i32 }
%WidePair = type { i64, i64 }

declare void @one_fixed(i32, ...)
declare void @three_fixed(i32, i32, i32, ...)
declare void @four_fixed(i32, i32, i32, i32, ...)

define void @unnamed_values_are_stack_only() {
; CHECK-LABEL: unnamed_values_are_stack_only:
; CHECK: addi r15, r15, -12
; CHECK: const32 [[FIRST:r[0-9]+]], 11
; CHECK: store32 [r15], [[FIRST]]
; CHECK: addi [[SECOND_ADDR:r[0-9]+]], r15, 4
; CHECK: const32 [[SECOND:r[0-9]+]], 22
; CHECK: store32 {{\[}}[[SECOND_ADDR]]{{\]}}, [[SECOND]]
; CHECK: const32 r1, 7
; CHECK: call32 one_fixed
; CHECK: addi r15, r15, 12
; CHECK: ret
  call void (i32, ...) @one_fixed(i32 7, i32 11, i32 22)
  ret void
}

define void @wide_values_are_aligned_and_little_endian() {
; CHECK-LABEL: wide_values_are_aligned_and_little_endian:
; CHECK: addi r15, r15, -28
; CHECK-DAG: addi [[I64_LO_ADDR:r[0-9]+]], r15, 4
; CHECK-DAG: const32 [[I64_LO:r[0-9]+]], 1432778632
; CHECK-DAG: store32 {{\[}}[[I64_LO_ADDR]]{{\]}}, [[I64_LO]]
; CHECK-DAG: addi [[I64_HI_ADDR:r[0-9]+]], r15, 8
; CHECK-DAG: const32 [[I64_HI:r[0-9]+]], 287454020
; CHECK-DAG: store32 {{\[}}[[I64_HI_ADDR]]{{\]}}, [[I64_HI]]
; CHECK-DAG: addi [[DOUBLE_LO_ADDR:r[0-9]+]], r15, 12
; CHECK-DAG: const32 [[DOUBLE_LO:r[0-9]+]], 0
; CHECK-DAG: store32 {{\[}}[[DOUBLE_LO_ADDR]]{{\]}}, [[DOUBLE_LO]]
; CHECK-DAG: addi [[DOUBLE_HI_ADDR:r[0-9]+]], r15, 16
; CHECK-DAG: const32 [[DOUBLE_HI:r[0-9]+]], 1072693248
; CHECK-DAG: store32 {{\[}}[[DOUBLE_HI_ADDR]]{{\]}}, [[DOUBLE_HI]]
; CHECK: call32 three_fixed
; CHECK: addi r15, r15, 28
; CHECK: ret
  call void (i32, i32, i32, ...) @three_fixed(
      i32 1, i32 2, i32 3, i64 1234605616436508552, double 1.000000e+00)
  ret void
}

define void @fixed_stack_precedes_unnamed_stream() {
; CHECK-LABEL: fixed_stack_precedes_unnamed_stream:
; CHECK: addi r15, r15, -12
; CHECK: const32 [[FIXED:r[0-9]+]], 4
; CHECK: store32 [r15], [[FIXED]]
; CHECK: addi [[UNNAMED_ADDR:r[0-9]+]], r15, 4
; CHECK: const32 [[UNNAMED:r[0-9]+]], 5
; CHECK: store32 {{\[}}[[UNNAMED_ADDR]]{{\]}}, [[UNNAMED]]
; CHECK: call32 four_fixed
; CHECK: addi r15, r15, 12
; CHECK: ret
  call void (i32, i32, i32, i32, ...) @four_fixed(
      i32 1, i32 2, i32 3, i32 4, i32 5)
  ret void
}

define void @indirect_variadic_call(ptr %callee, ptr %aggregate_copy) {
; CHECK-LABEL: indirect_variadic_call:
; CHECK: addi r15, r15, -20
; CHECK: load32
; CHECK: store32
; CHECK: store32 [r15],
; CHECK: call r0
; CHECK: addi r15, r15, 20
; CHECK: ret
  call void (i32, ...) %callee(
      i32 9, ptr byval(%Pair) align 4 %aggregate_copy)
  ret void
}

define void @aligned_aggregate_copy(ptr %aggregate) {
; CHECK-LABEL: aligned_aggregate_copy:
; CHECK: addi r15, r15, -28
; CHECK: addi [[COPY:r[0-9]+]], r15, 4
; CHECK: store32 [r15], [[COPY]]
; CHECK: call32 one_fixed
; CHECK: addi r15, r15, 28
; CHECK: ret
  call void (i32, ...) @one_fixed(
      i32 9, ptr byval(%WidePair) align 8 %aggregate)
  ret void
}

declare i32 @sum4(i32, i32, i32, i32)

define i32 @fixed_only_call_is_unchanged() {
; CHECK-LABEL: fixed_only_call_is_unchanged:
; CHECK: addi r15, r15, -12
; CHECK: store32 [r15],
; CHECK: const32 r1, 1
; CHECK: const32 r2, 2
; CHECK: const32 r3, 3
; CHECK: call32 sum4
; CHECK: addi r15, r15, 12
; CHECK: ret
  %result = call i32 @sum4(i32 1, i32 2, i32 3, i32 4)
  ret i32 %result
}
