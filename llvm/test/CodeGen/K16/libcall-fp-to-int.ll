; RUN: llc -mtriple=k16 < %s | FileCheck %s

define i32 @signed_f32_to_i32(float %value) {
; CHECK-LABEL: signed_f32_to_i32:
; CHECK: call32 __fixsfsi
; CHECK: ret
  %result = fptosi float %value to i32
  ret i32 %result
}

define i64 @signed_f32_to_i64(float %value) {
; CHECK-LABEL: signed_f32_to_i64:
; CHECK: call32 __fixsfdi
; CHECK: ret
  %result = fptosi float %value to i64
  ret i64 %result
}

define i128 @signed_f32_to_i128(float %value) {
; CHECK-LABEL: signed_f32_to_i128:
; CHECK: call32 __fixsfti
; CHECK: ret
  %result = fptosi float %value to i128
  ret i128 %result
}

define i32 @signed_f64_to_i32(double %value) {
; CHECK-LABEL: signed_f64_to_i32:
; CHECK: call32 __fixdfsi
; CHECK: ret
  %result = fptosi double %value to i32
  ret i32 %result
}

define i64 @signed_f64_to_i64(double %value) {
; CHECK-LABEL: signed_f64_to_i64:
; CHECK: call32 __fixdfdi
; CHECK: ret
  %result = fptosi double %value to i64
  ret i64 %result
}

define i128 @signed_f64_to_i128(double %value) {
; CHECK-LABEL: signed_f64_to_i128:
; CHECK: call32 __fixdfti
; CHECK: ret
  %result = fptosi double %value to i128
  ret i128 %result
}

define i32 @unsigned_f32_to_i32(float %value) {
; CHECK-LABEL: unsigned_f32_to_i32:
; CHECK: call32 __fixunssfsi
; CHECK: ret
  %result = fptoui float %value to i32
  ret i32 %result
}

define i64 @unsigned_f32_to_i64(float %value) {
; CHECK-LABEL: unsigned_f32_to_i64:
; CHECK: call32 __fixunssfdi
; CHECK: ret
  %result = fptoui float %value to i64
  ret i64 %result
}

define i128 @unsigned_f32_to_i128(float %value) {
; CHECK-LABEL: unsigned_f32_to_i128:
; CHECK: call32 __fixunssfti
; CHECK: ret
  %result = fptoui float %value to i128
  ret i128 %result
}

define i32 @unsigned_f64_to_i32(double %value) {
; CHECK-LABEL: unsigned_f64_to_i32:
; CHECK: call32 __fixunsdfsi
; CHECK: ret
  %result = fptoui double %value to i32
  ret i32 %result
}

define i64 @unsigned_f64_to_i64(double %value) {
; CHECK-LABEL: unsigned_f64_to_i64:
; CHECK: call32 __fixunsdfdi
; CHECK: ret
  %result = fptoui double %value to i64
  ret i64 %result
}

define i128 @unsigned_f64_to_i128(double %value) {
; CHECK-LABEL: unsigned_f64_to_i128:
; CHECK: call32 __fixunsdfti
; CHECK: ret
  %result = fptoui double %value to i128
  ret i128 %result
}
