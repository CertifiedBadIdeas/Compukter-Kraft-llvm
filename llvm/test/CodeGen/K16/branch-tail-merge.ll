; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare void @sink(i32)

define i32 @tail_merge_branch(i32 %flag) {
; CHECK-LABEL: tail_merge_branch:
; CHECK: br
; CHECK: ret
entry:
  %is_zero = icmp eq i32 %flag, 0
  br i1 %is_zero, label %left, label %right

left:
  call void @sink(i32 1)
  br label %tail

right:
  call void @sink(i32 2)
  br label %tail

tail:
  ret i32 7
}
