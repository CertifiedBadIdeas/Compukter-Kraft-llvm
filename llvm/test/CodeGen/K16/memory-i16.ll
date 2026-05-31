; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define void @store_i16(ptr %addr, i32 %value) {
; CHECK-LABEL: store_i16:
; CHECK: store16
; CHECK: ret
entry:
  %narrow = trunc i32 %value to i16
  store i16 %narrow, ptr %addr, align 2
  ret void
}

define void @store_i16_stack(i32 %value) {
; CHECK-LABEL: store_i16_stack:
; CHECK: store16
; CHECK: ret
entry:
  %slot = alloca i16, align 2
  %narrow = trunc i32 %value to i16
  store i16 %narrow, ptr %slot, align 2
  ret void
}
