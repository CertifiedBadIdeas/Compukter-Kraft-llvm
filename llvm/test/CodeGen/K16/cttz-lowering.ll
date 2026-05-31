; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare i32 @llvm.cttz.i32(i32, i1 immarg)

define i32 @count_trailing_zeroes(i32 %value) {
; CHECK-LABEL: count_trailing_zeroes:
; CHECK-NOT: cttz
; CHECK: brz
; CHECK: mul
; CHECK: shr
; CHECK: load8
; CHECK: ret
entry:
  %count = call i32 @llvm.cttz.i32(i32 %value, i1 false)
  ret i32 %count
}
