; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @load_signed_byte(ptr %addr) {
; CHECK-LABEL: load_signed_byte:
; CHECK: load8
; CHECK: shl
; CHECK: sar
; CHECK: ret
entry:
  %byte = load i8, ptr %addr, align 1
  %value = sext i8 %byte to i32
  ret i32 %value
}

define i32 @load_signed_halfword(ptr %addr) {
; CHECK-LABEL: load_signed_halfword:
; CHECK: load16
; CHECK: shl
; CHECK: sar
; CHECK: ret
entry:
  %half = load i16, ptr %addr, align 2
  %value = sext i16 %half to i32
  ret i32 %value
}
