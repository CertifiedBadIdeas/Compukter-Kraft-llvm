; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @branch_signed_i8_negative(i32 %value) {
; CHECK-LABEL: branch_signed_i8_negative:
; CHECK: shl
; CHECK: sar
; CHECK: lt_s [[COND:r[0-9]+]],
; CHECK: br{{n?z}} [[COND]],
; CHECK: ret
entry:
  %narrow = trunc i32 %value to i8
  %wide = sext i8 %narrow to i32
  %negative = icmp slt i32 %wide, 0
  br i1 %negative, label %negative_block, label %non_negative_block

negative_block:
  ret i32 1

non_negative_block:
  ret i32 0
}
