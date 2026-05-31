; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @ordered_equal_f32(float %lhs, float %rhs) {
; CHECK-LABEL: ordered_equal_f32:
; CHECK: call32 __eqsf2
; CHECK: ret
  %cmp = fcmp oeq float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_not_equal_f32(float %lhs, float %rhs) {
; CHECK-LABEL: ordered_not_equal_f32:
; CHECK: call32 __nesf2
; CHECK: ret
  %cmp = fcmp une float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_less_f32(float %lhs, float %rhs) {
; CHECK-LABEL: ordered_less_f32:
; CHECK: call32 __ltsf2
; CHECK: ret
  %cmp = fcmp olt float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_less_equal_f32(float %lhs, float %rhs) {
; CHECK-LABEL: ordered_less_equal_f32:
; CHECK: call32 __lesf2
; CHECK: ret
  %cmp = fcmp ole float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_greater_f32(float %lhs, float %rhs) {
; CHECK-LABEL: ordered_greater_f32:
; CHECK: call32 __gtsf2
; CHECK: ret
  %cmp = fcmp ogt float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_greater_equal_f32(float %lhs, float %rhs) {
; CHECK-LABEL: ordered_greater_equal_f32:
; CHECK: call32 __gesf2
; CHECK: ret
  %cmp = fcmp oge float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @unordered_f32(float %lhs, float %rhs) {
; CHECK-LABEL: unordered_f32:
; CHECK: call32 __unordsf2
; CHECK: ret
  %cmp = fcmp uno float %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_equal_f64(double %lhs, double %rhs) {
; CHECK-LABEL: ordered_equal_f64:
; CHECK: call32 __eqdf2
; CHECK: ret
  %cmp = fcmp oeq double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_not_equal_f64(double %lhs, double %rhs) {
; CHECK-LABEL: ordered_not_equal_f64:
; CHECK: call32 __nedf2
; CHECK: ret
  %cmp = fcmp une double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_less_f64(double %lhs, double %rhs) {
; CHECK-LABEL: ordered_less_f64:
; CHECK: call32 __ltdf2
; CHECK: ret
  %cmp = fcmp olt double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_less_equal_f64(double %lhs, double %rhs) {
; CHECK-LABEL: ordered_less_equal_f64:
; CHECK: call32 __ledf2
; CHECK: ret
  %cmp = fcmp ole double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_greater_f64(double %lhs, double %rhs) {
; CHECK-LABEL: ordered_greater_f64:
; CHECK: call32 __gtdf2
; CHECK: ret
  %cmp = fcmp ogt double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @ordered_greater_equal_f64(double %lhs, double %rhs) {
; CHECK-LABEL: ordered_greater_equal_f64:
; CHECK: call32 __gedf2
; CHECK: ret
  %cmp = fcmp oge double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}

define i32 @unordered_f64(double %lhs, double %rhs) {
; CHECK-LABEL: unordered_f64:
; CHECK: call32 __unorddf2
; CHECK: ret
  %cmp = fcmp uno double %lhs, %rhs
  %result = zext i1 %cmp to i32
  ret i32 %result
}
