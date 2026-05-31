; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

define i32 @caller(ptr %callee, i32 %arg) {
; CHECK-LABEL: caller:
; CHECK-NOT: call32
; CHECK: call r{{[0-9]+}}
; CHECK: ret
  %value = call i32 %callee(i32 %arg)
  ret i32 %value
}
