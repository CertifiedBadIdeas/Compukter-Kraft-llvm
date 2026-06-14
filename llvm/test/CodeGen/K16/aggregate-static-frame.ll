; RUN: llc -mtriple=k16 < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -r %t | FileCheck %s --check-prefix=OBJ

%Frame = type { [16 x i32], i32, i32, i32 }

@FRAME = internal global %Frame zeroinitializer, align 4

define i32 @load_stack_pointer() {
; ASM-LABEL: load_stack_pointer:
; ASM: const32 r0, FRAME+68
; ASM: load32 r0, [r0]
entry:
  %x = load volatile i32, ptr getelementptr inbounds (%Frame, ptr @FRAME, i32 0, i32 2), align 4
  ret i32 %x
}

define void @store_interrupt_enable(i32 %v) {
; ASM-LABEL: store_interrupt_enable:
; ASM: const32 r0, FRAME+72
; ASM: store32 [r0], r1
entry:
  store volatile i32 %v, ptr getelementptr inbounds (%Frame, ptr @FRAME, i32 0, i32 3), align 4
  ret void
}

define void @copy_frame(ptr %out) {
; ASM-LABEL: copy_frame:
; ASM: const32 r2, FRAME
; ASM: const32 r3, 76
; ASM: call32 memcpy
entry:
  call void @llvm.memcpy.p0.p0.i32(ptr align 4 %out, ptr align 4 @FRAME, i32 76, i1 false)
  ret void
}

declare void @llvm.memcpy.p0.p0.i32(ptr nocapture, ptr nocapture readonly, i32, i1 immarg)

; OBJ-DAG: R_K16_ABS32 .bss 0x44
; OBJ-DAG: R_K16_ABS32 .bss 0x48
; OBJ-DAG: R_K16_ABS32 .bss 0x0
; OBJ-DAG: R_K16_CALL32 memcpy 0x0
