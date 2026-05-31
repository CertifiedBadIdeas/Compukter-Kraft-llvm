; RUN: llc -mtriple=k16 < %s | FileCheck %s

define float @extend_f16_to_f32(ptr %in) {
; CHECK-LABEL: extend_f16_to_f32:
; CHECK: call32 __extendhfsf2
; CHECK: ret
  %value = load half, ptr %in, align 2
  %result = fpext half %value to float
  ret float %result
}

define double @extend_f16_to_f64(ptr %in) {
; CHECK-LABEL: extend_f16_to_f64:
; CHECK: call32 __extendhfsf2
; CHECK: call32 __extendsfdf2
; CHECK: ret
  %value = load half, ptr %in, align 2
  %result = fpext half %value to double
  ret double %result
}
