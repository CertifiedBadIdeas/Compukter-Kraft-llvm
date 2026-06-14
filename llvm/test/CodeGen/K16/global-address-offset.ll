; RUN: llc -mtriple=k16 < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r %t | FileCheck %s --check-prefix=OBJ

%Frame = type { [16 x i32], i32, i32, i32 }

@FRAME = internal global %Frame zeroinitializer, align 4

define void @store_base(i32 %v) {
; ASM-LABEL: store_base:
; ASM: const32 r0, FRAME{{$}}
; ASM: store32 [r0], r1
entry:
  store volatile i32 %v, ptr @FRAME, align 4
  ret void
}

define void @store_resume_pc(i32 %v) {
; ASM-LABEL: store_resume_pc:
; ASM: const32 r0, FRAME+64
; ASM: store32 [r0], r1
entry:
  store volatile i32 %v, ptr getelementptr inbounds (%Frame, ptr @FRAME, i32 0, i32 1), align 4
  ret void
}

define i32 @load_reg14() {
; ASM-LABEL: load_reg14:
; ASM: const32 r0, FRAME+56
; ASM: load32 r0, [r0]
entry:
  %x = load volatile i32, ptr getelementptr inbounds (%Frame, ptr @FRAME, i32 0, i32 0, i32 14), align 4
  ret i32 %x
}

; OBJ-DAG: R_K16_ABS32 .bss 0x0
; OBJ-DAG: R_K16_ABS32 .bss 0x40
; OBJ-DAG: R_K16_ABS32 .bss 0x38
