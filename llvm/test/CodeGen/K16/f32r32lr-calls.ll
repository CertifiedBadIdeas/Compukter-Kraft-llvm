; RUN: llc -mtriple=k16 -mattr=+f32r32lr -o - < %s | FileCheck %s --check-prefix=R32
; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=DEFAULT

define i32 @caller8() {
; R32-LABEL: caller8:
; R32: addi r30, r30, -{{[0-9]+}}
; R32: const32 r0, 1
; R32: const32 r1, 2
; R32: const32 r2, 3
; R32: const32 r3, 4
; R32: const32 r4, 5
; R32: const32 r5, 6
; R32: const32 r6, 7
; R32: const32 r7, 8
; R32-NOT: store32 [r30]
; R32: call32 sum8
; R32: ret
;
; DEFAULT-LABEL: caller8:
; DEFAULT: addi r15, r15, -{{[0-9]+}}
; DEFAULT: store32 [r{{[0-9]+}}], r{{[0-9]+}}
; DEFAULT: const32 r1, 1
; DEFAULT: const32 r2, 2
; DEFAULT: const32 r3, 3
; DEFAULT: call32 sum8
  %value = call i32 @sum8(i32 1, i32 2, i32 3, i32 4,
                          i32 5, i32 6, i32 7, i32 8)
  ret i32 %value
}

define i32 @sum8(i32 %a, i32 %b, i32 %c, i32 %d,
                 i32 %e, i32 %f, i32 %g, i32 %h) {
; R32-LABEL: sum8:
; R32: add {{r[0-9]+}}, r0, r1
; R32: r7
; R32: ret
  %ab = add i32 %a, %b
  %abc = add i32 %ab, %c
  %abcd = add i32 %abc, %d
  %abcde = add i32 %abcd, %e
  %abcdef = add i32 %abcde, %f
  %abcdefg = add i32 %abcdef, %g
  %abcdefgh = add i32 %abcdefg, %h
  ret i32 %abcdefgh
}

define i32 @non_leaf(i32 %value) {
; R32-LABEL: non_leaf:
; R32: addi r30, r30, -{{[0-9]+}}
; R32: store32 [r30{{.*}}], r31
; R32: call32 leaf
; R32: load32 r31, [r30
; R32: addi r30, r30, {{[0-9]+}}
; R32: ret
  %result = call i32 @leaf(i32 %value)
  %sum = add i32 %result, %value
  ret i32 %sum
}

declare i32 @leaf(i32)
