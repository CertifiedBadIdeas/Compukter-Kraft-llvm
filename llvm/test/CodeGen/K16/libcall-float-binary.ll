; RUN: llc -mtriple=k16 < %s | FileCheck %s

define float @add_f32(float %lhs, float %rhs) {
; CHECK-LABEL: add_f32:
; CHECK: call32 __addsf3
; CHECK: ret
  %result = fadd float %lhs, %rhs
  ret float %result
}

define float @sub_f32(float %lhs, float %rhs) {
; CHECK-LABEL: sub_f32:
; CHECK: call32 __subsf3
; CHECK: ret
  %result = fsub float %lhs, %rhs
  ret float %result
}

define float @mul_f32(float %lhs, float %rhs) {
; CHECK-LABEL: mul_f32:
; CHECK: call32 __mulsf3
; CHECK: ret
  %result = fmul float %lhs, %rhs
  ret float %result
}

define float @div_f32(float %lhs, float %rhs) {
; CHECK-LABEL: div_f32:
; CHECK: call32 __divsf3
; CHECK: ret
  %result = fdiv float %lhs, %rhs
  ret float %result
}

define double @add_f64(double %lhs, double %rhs) {
; CHECK-LABEL: add_f64:
; CHECK: call32 __adddf3
; CHECK: ret
  %result = fadd double %lhs, %rhs
  ret double %result
}

define double @sub_f64(double %lhs, double %rhs) {
; CHECK-LABEL: sub_f64:
; CHECK: call32 __subdf3
; CHECK: ret
  %result = fsub double %lhs, %rhs
  ret double %result
}

define double @mul_f64(double %lhs, double %rhs) {
; CHECK-LABEL: mul_f64:
; CHECK: call32 __muldf3
; CHECK: ret
  %result = fmul double %lhs, %rhs
  ret double %result
}

define double @div_f64(double %lhs, double %rhs) {
; CHECK-LABEL: div_f64:
; CHECK: call32 __divdf3
; CHECK: ret
  %result = fdiv double %lhs, %rhs
  ret double %result
}
