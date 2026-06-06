; RUN: llc -mtriple=k16 -o - < %s | FileCheck %s --check-prefix=ASM
; RUN: llc -mtriple=k16 -filetype=obj -o %t < %s
; RUN: llvm-readobj -x .text.k16 %t | FileCheck %s --check-prefix=OBJ

; ASM-LABEL: install_interrupts:
; ASM: write_csr 1,
; ASM: write_csr 6,
; ASM: write_csr 5,
; ASM-NOT: call32 __k16_write_trap_vector
; ASM-NOT: call32 __k16_write_interrupt_mask
; ASM-NOT: call32 __k16_write_interrupt_enable

; ASM-LABEL: read_trap_state:
; ASM: read_csr {{r[0-9]+}}, 2
; ASM: read_csr {{r[0-9]+}}, 3
; ASM: read_csr {{r[0-9]+}}, 4
; ASM: read_csr {{r[0-9]+}}, 7
; ASM-NOT: call32 __k16_read_trap_cause
; ASM-NOT: call32 __k16_read_trap_pc
; ASM-NOT: call32 __k16_read_trap_value
; ASM-NOT: call32 __k16_read_interrupt_pending

; ASM-LABEL: return_from_interrupt:
; ASM: iret
; ASM-NOT: call32 __k16_iret_once

; OBJ: Hex dump of section '.text.k16':
; OBJ: 0400

declare void @__k16_write_trap_vector(i32)
declare void @__k16_write_interrupt_mask(i32)
declare void @__k16_write_interrupt_enable(i32)
declare i32 @__k16_read_trap_cause()
declare i32 @__k16_read_trap_pc()
declare i32 @__k16_read_trap_value()
declare i32 @__k16_read_interrupt_pending()
declare void @__k16_iret_once()

define void @install_interrupts(i32 %vector) {
  call void @__k16_write_trap_vector(i32 %vector)
  call void @__k16_write_interrupt_mask(i32 1)
  call void @__k16_write_interrupt_enable(i32 1)
  ret void
}

define i32 @read_trap_state() {
  %cause = call i32 @__k16_read_trap_cause()
  %pc = call i32 @__k16_read_trap_pc()
  %value = call i32 @__k16_read_trap_value()
  %pending = call i32 @__k16_read_interrupt_pending()
  %sum0 = add i32 %cause, %pc
  %sum1 = add i32 %sum0, %value
  %sum2 = add i32 %sum1, %pending
  ret i32 %sum2
}

define void @return_from_interrupt() {
  call void @__k16_iret_once()
  unreachable
}
