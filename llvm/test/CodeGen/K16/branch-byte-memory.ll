; RUN: llc -mtriple=k16 < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -h -S -r %t | FileCheck %s --check-prefix=OBJ

define void @copy_until(ptr %dst, ptr %src, i32 %n) {
; ASM-LABEL: copy_until:
; ASM: bruge
; ASM: load8
; ASM: store8
; ASM: br
; ASM: ret
entry:
  br label %loop

loop:
  %index = phi i32 [ 0, %entry ], [ %next, %body ]
  %keep_going = icmp ult i32 %index, %n
  br i1 %keep_going, label %body, label %done

body:
  %src_addr = getelementptr i8, ptr %src, i32 %index
  %byte = load i8, ptr %src_addr, align 1
  %dst_addr = getelementptr i8, ptr %dst, i32 %index
  store i8 %byte, ptr %dst_addr, align 1
  %next = add i32 %index, 1
  br label %loop

done:
  ret void
}

; OBJ: Format: elf32-unknown
; OBJ: Machine: 0x5258
; OBJ: Name: .text.k16
; OBJ: Relocations [
; OBJ: R_K16_ABS32
; OBJ: ]
