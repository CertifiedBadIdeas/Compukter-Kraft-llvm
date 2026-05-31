; RUN: llc -mtriple=k16 < %s | FileCheck %s

define void @round_f32_to_f16(ptr %out, float %value) {
; CHECK-LABEL: round_f32_to_f16:
; CHECK: call32 __truncsfhf2
; CHECK: ret
  %result = fptrunc float %value to half
  store half %result, ptr %out, align 2
  ret void
}

define void @round_f64_to_f16(ptr %out, double %value) {
; CHECK-LABEL: round_f64_to_f16:
; CHECK: call32 __truncdfhf2
; CHECK: ret
  %result = fptrunc double %value to half
  store half %result, ptr %out, align 2
  ret void
}

define float @round_f64_to_f32(double %value) {
; CHECK-LABEL: round_f64_to_f32:
; CHECK: call32 __truncdfsf2
; CHECK: ret
  %result = fptrunc double %value to float
  ret float %result
}
