; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare float @llvm.minnum.f32(float, float)
declare double @llvm.minnum.f64(double, double)
declare float @llvm.maxnum.f32(float, float)
declare double @llvm.maxnum.f64(double, double)
declare float @llvm.minimum.f32(float, float)
declare double @llvm.minimum.f64(double, double)
declare float @llvm.maximum.f32(float, float)
declare double @llvm.maximum.f64(double, double)

define float @minnum_f32(float %lhs, float %rhs) {
; CHECK-LABEL: minnum_f32:
; CHECK: call32 fminf
; CHECK: ret
  %result = call float @llvm.minnum.f32(float %lhs, float %rhs)
  ret float %result
}

define double @minnum_f64(double %lhs, double %rhs) {
; CHECK-LABEL: minnum_f64:
; CHECK: call32 fmin
; CHECK: ret
  %result = call double @llvm.minnum.f64(double %lhs, double %rhs)
  ret double %result
}

define float @maxnum_f32(float %lhs, float %rhs) {
; CHECK-LABEL: maxnum_f32:
; CHECK: call32 fmaxf
; CHECK: ret
  %result = call float @llvm.maxnum.f32(float %lhs, float %rhs)
  ret float %result
}

define double @maxnum_f64(double %lhs, double %rhs) {
; CHECK-LABEL: maxnum_f64:
; CHECK: call32 fmax
; CHECK: ret
  %result = call double @llvm.maxnum.f64(double %lhs, double %rhs)
  ret double %result
}

define float @minimum_f32(float %lhs, float %rhs) {
; CHECK-LABEL: minimum_f32:
; CHECK: call32 fminimumf
; CHECK: ret
  %result = call float @llvm.minimum.f32(float %lhs, float %rhs)
  ret float %result
}

define double @minimum_f64(double %lhs, double %rhs) {
; CHECK-LABEL: minimum_f64:
; CHECK: call32 fminimum
; CHECK: ret
  %result = call double @llvm.minimum.f64(double %lhs, double %rhs)
  ret double %result
}

define float @maximum_f32(float %lhs, float %rhs) {
; CHECK-LABEL: maximum_f32:
; CHECK: call32 fmaximumf
; CHECK: ret
  %result = call float @llvm.maximum.f32(float %lhs, float %rhs)
  ret float %result
}

define double @maximum_f64(double %lhs, double %rhs) {
; CHECK-LABEL: maximum_f64:
; CHECK: call32 fmaximum
; CHECK: ret
  %result = call double @llvm.maximum.f64(double %lhs, double %rhs)
  ret double %result
}
