; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

%Pair = type { i32, i32 }
%Frame = type { [16 x i32], i32, i32, i32 }

define i32 @use_pair(%Pair %pair) {
; CHECK-LABEL: use_pair:
; CHECK: add r0, r1, r2
; CHECK: ret
entry:
  %lo = extractvalue %Pair %pair, 0
  %hi = extractvalue %Pair %pair, 1
  %sum = add i32 %lo, %hi
  ret i32 %sum
}

define %Pair @make_pair(i32 %a, i32 %b) {
; CHECK-LABEL: make_pair:
; CHECK: add r0, r1
; CHECK: add r1, r2
; CHECK: ret
entry:
  %p0 = insertvalue %Pair poison, i32 %a, 0
  %p1 = insertvalue %Pair %p0, i32 %b, 1
  ret %Pair %p1
}

define void @take_frame(%Frame %frame, ptr %out) {
; CHECK-LABEL: take_frame:
; CHECK: const32 [[REG14_OFFSET:r[0-9]+]], 48
; CHECK: add [[REG14_ADDR:r[0-9]+]], r15, [[REG14_OFFSET]]
; CHECK: load32 [[REG14:r[0-9]+]], {{\[}}[[REG14_ADDR]]]
; CHECK: const32 [[SP_OFFSET:r[0-9]+]], 68
; CHECK: add [[SP_ADDR:r[0-9]+]], r15, [[SP_OFFSET]]
; CHECK: load32 [[OUT:r[0-9]+]], {{\[}}[[SP_ADDR]]]
; CHECK: store32 {{\[}}[[OUT]]], [[REG14]]
; CHECK: ret
entry:
  %reg14 = extractvalue %Frame %frame, 0, 14
  store i32 %reg14, ptr %out, align 4
  ret void
}
