; RUN: llc -mtriple=k16 -mattr=+f32r32lr -o - < %s | FileCheck %s --check-prefix=R32
; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=DEFAULT

define i32 @pressure(i32 %seed) {
; R32-LABEL: pressure:
; R32: r16
; R32: call32 opaque
; R32: ret
; DEFAULT-LABEL: pressure:
; DEFAULT-NOT: r16
; DEFAULT: call32 opaque
; DEFAULT: ret
  %v1 = add i32 %seed, 1
  %v2 = add i32 %seed, 2
  %v3 = add i32 %seed, 3
  %v4 = add i32 %seed, 4
  %v5 = add i32 %seed, 5
  %v6 = add i32 %seed, 6
  %v7 = add i32 %seed, 7
  %v8 = add i32 %seed, 8
  %v9 = add i32 %seed, 9
  %v10 = add i32 %seed, 10
  %v11 = add i32 %seed, 11
  %v12 = add i32 %seed, 12
  %opaque = call i32 @opaque(i32 %seed)
  %p1 = mul i32 %v1, %v2
  %p2 = mul i32 %v3, %v4
  %p3 = mul i32 %v5, %v6
  %p4 = mul i32 %v7, %v8
  %p5 = mul i32 %v9, %v10
  %p6 = mul i32 %v11, %v12
  %s1 = add i32 %p1, %p2
  %s2 = add i32 %p3, %p4
  %s3 = add i32 %p5, %p6
  %s4 = add i32 %s1, %s2
  %subtotal = add i32 %s4, %s3
  %result = add i32 %subtotal, %opaque
  ret i32 %result
}

declare i32 @opaque(i32)
