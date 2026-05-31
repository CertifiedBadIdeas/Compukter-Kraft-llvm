; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare float @llvm.fma.f32(float, float, float)
declare double @llvm.fma.f64(double, double, double)

define float @fma_f32(float %lhs, float %rhs, float %addend) {
; CHECK-LABEL: fma_f32:
; CHECK: call32 fmaf
; CHECK: ret
  %result = call float @llvm.fma.f32(float %lhs, float %rhs, float %addend)
  ret float %result
}

define double @fma_f64(double %lhs, double %rhs, double %addend) {
; CHECK-LABEL: fma_f64:
; CHECK: call32 fma
; CHECK: ret
  %result = call double @llvm.fma.f64(double %lhs, double %rhs, double %addend)
  ret double %result
}
