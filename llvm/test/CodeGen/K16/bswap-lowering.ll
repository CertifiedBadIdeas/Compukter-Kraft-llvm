; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s

declare i32 @llvm.bswap.i32(i32)

define i32 @swap_bytes(i32 %value) {
; CHECK-LABEL: swap_bytes:
; CHECK-NOT: bswap
; CHECK: shr
; CHECK: shl
; CHECK: or
; CHECK: ret
  %swapped = call i32 @llvm.bswap.i32(i32 %value)
  ret i32 %swapped
}
