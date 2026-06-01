; RUN: llc -mtriple=k16 -function-sections -filetype=obj -o %t < %s
; RUN: llvm-readobj -S %t | FileCheck %s

; CHECK: Name: .text.k16.foo
; CHECK: Name: .text.k16.bar

define i32 @foo(i32 %lhs, i32 %rhs) {
  %sum = add i32 %lhs, %rhs
  ret i32 %sum
}

define i32 @bar(i32 %value) {
  %sum = add i32 %value, 1
  ret i32 %sum
}
