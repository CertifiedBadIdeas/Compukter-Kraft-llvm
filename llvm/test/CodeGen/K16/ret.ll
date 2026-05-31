; RUN: llc -mtriple=k16 < %s | FileCheck %s

define void @main() {
; CHECK-LABEL: main:
; CHECK: ret
  ret void
}
