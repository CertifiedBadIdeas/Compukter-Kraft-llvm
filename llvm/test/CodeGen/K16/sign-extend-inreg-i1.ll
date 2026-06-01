; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

define i32 @sext_i1(i32 %value) {
; CHECK-LABEL: sext_i1:
; CHECK: const32 r[[MASK:[0-9]+]], 1
; CHECK: and r[[BIT:[0-9]+]], r1, r[[MASK]]
; CHECK: const32 r[[ZERO:[0-9]+]], 0
; CHECK: sub r0, r[[ZERO]], r[[BIT]]
; CHECK: ret
  %bit = trunc i32 %value to i1
  %extended = sext i1 %bit to i32
  ret i32 %extended
}
