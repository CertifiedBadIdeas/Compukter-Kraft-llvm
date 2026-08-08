; RUN: llc -mtriple=k16 < %s | FileCheck %s

@flag = global i1 false, align 1

define i32 @load_i1() {
; CHECK-LABEL: load_i1:
; CHECK:       load8
; CHECK:       ret
  %value = load i1, ptr @flag, align 1
  %extended = zext i1 %value to i32
  ret i32 %extended
}

define void @store_i1(i32 %value) {
; CHECK-LABEL: store_i1:
; CHECK:       store8
; CHECK:       ret
  %truncated = trunc i32 %value to i1
  store i1 %truncated, ptr @flag, align 1
  ret void
}
