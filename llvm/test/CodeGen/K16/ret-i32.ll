; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @main() {
; CHECK-LABEL: main:
; CHECK: const32 r0, 42
; CHECK: ret
  ret i32 42
}
