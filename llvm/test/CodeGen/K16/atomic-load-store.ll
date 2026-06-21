; RUN: llc -mtriple=k16 < %s | FileCheck %s
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s

define i32 @load_i8_monotonic(ptr %addr) {
; CHECK-LABEL: load_i8_monotonic:
; CHECK: load8
; CHECK: ret
entry:
  %value = load atomic i8, ptr %addr monotonic, align 1
  %extended = zext i8 %value to i32
  ret i32 %extended
}

define i32 @load_i16_monotonic(ptr %addr) {
; CHECK-LABEL: load_i16_monotonic:
; CHECK: load16
; CHECK: ret
entry:
  %value = load atomic i16, ptr %addr monotonic, align 2
  %extended = zext i16 %value to i32
  ret i32 %extended
}

define i32 @load_i32_monotonic(ptr %addr) {
; CHECK-LABEL: load_i32_monotonic:
; CHECK: load32
; CHECK: ret
entry:
  %value = load atomic i32, ptr %addr monotonic, align 4
  ret i32 %value
}

define void @store_i8_monotonic(ptr %addr, i8 %value) {
; CHECK-LABEL: store_i8_monotonic:
; CHECK: store8
; CHECK: ret
entry:
  store atomic i8 %value, ptr %addr monotonic, align 1
  ret void
}

define void @store_i16_monotonic(ptr %addr, i16 %value) {
; CHECK-LABEL: store_i16_monotonic:
; CHECK: store16
; CHECK: ret
entry:
  store atomic i16 %value, ptr %addr monotonic, align 2
  ret void
}

define void @store_i32_monotonic(ptr %addr, i32 %value) {
; CHECK-LABEL: store_i32_monotonic:
; CHECK: store32
; CHECK: ret
entry:
  store atomic i32 %value, ptr %addr monotonic, align 4
  ret void
}
