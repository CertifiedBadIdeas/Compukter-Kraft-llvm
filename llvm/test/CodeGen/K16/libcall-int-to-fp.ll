; RUN: llc -mtriple=k16 < %s | FileCheck %s

define float @signed_i32_to_f32(i32 %value) {
; CHECK-LABEL: signed_i32_to_f32:
; CHECK: call32 __floatsisf
; CHECK: ret
  %result = sitofp i32 %value to float
  ret float %result
}

define double @signed_i32_to_f64(i32 %value) {
; CHECK-LABEL: signed_i32_to_f64:
; CHECK: call32 __floatsidf
; CHECK: ret
  %result = sitofp i32 %value to double
  ret double %result
}

define float @signed_i64_to_f32(i64 %value) {
; CHECK-LABEL: signed_i64_to_f32:
; CHECK: call32 __floatdisf
; CHECK: ret
  %result = sitofp i64 %value to float
  ret float %result
}

define double @signed_i64_to_f64(i64 %value) {
; CHECK-LABEL: signed_i64_to_f64:
; CHECK: call32 __floatdidf
; CHECK: ret
  %result = sitofp i64 %value to double
  ret double %result
}

define float @signed_i128_to_f32(i128 %value) {
; CHECK-LABEL: signed_i128_to_f32:
; CHECK: call32 __floattisf
; CHECK: ret
  %result = sitofp i128 %value to float
  ret float %result
}

define double @signed_i128_to_f64(i128 %value) {
; CHECK-LABEL: signed_i128_to_f64:
; CHECK: call32 __floattidf
; CHECK: ret
  %result = sitofp i128 %value to double
  ret double %result
}

define float @unsigned_i32_to_f32(i32 %value) {
; CHECK-LABEL: unsigned_i32_to_f32:
; CHECK: call32 __floatunsisf
; CHECK: ret
  %result = uitofp i32 %value to float
  ret float %result
}

define double @unsigned_i32_to_f64(i32 %value) {
; CHECK-LABEL: unsigned_i32_to_f64:
; CHECK: call32 __floatunsidf
; CHECK: ret
  %result = uitofp i32 %value to double
  ret double %result
}

define float @unsigned_i64_to_f32(i64 %value) {
; CHECK-LABEL: unsigned_i64_to_f32:
; CHECK: call32 __floatundisf
; CHECK: ret
  %result = uitofp i64 %value to float
  ret float %result
}

define double @unsigned_i64_to_f64(i64 %value) {
; CHECK-LABEL: unsigned_i64_to_f64:
; CHECK: call32 __floatundidf
; CHECK: ret
  %result = uitofp i64 %value to double
  ret double %result
}

define float @unsigned_i128_to_f32(i128 %value) {
; CHECK-LABEL: unsigned_i128_to_f32:
; CHECK: call32 __floatuntisf
; CHECK: ret
  %result = uitofp i128 %value to float
  ret float %result
}

define double @unsigned_i128_to_f64(i128 %value) {
; CHECK-LABEL: unsigned_i128_to_f64:
; CHECK: call32 __floatuntidf
; CHECK: ret
  %result = uitofp i128 %value to double
  ret double %result
}
