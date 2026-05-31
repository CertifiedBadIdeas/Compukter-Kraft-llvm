; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @dense_switch(i32 %value) {
; CHECK-LABEL: dense_switch:
; CHECK-NOT: br_jt
; CHECK: ret
entry:
  switch i32 %value, label %default [
    i32 0, label %case0
    i32 1, label %case1
    i32 2, label %case2
    i32 3, label %case3
    i32 4, label %case4
    i32 5, label %case5
  ]

case0:
  ret i32 10

case1:
  ret i32 11

case2:
  ret i32 12

case3:
  ret i32 13

case4:
  ret i32 14

case5:
  ret i32 15

default:
  ret i32 99
}
