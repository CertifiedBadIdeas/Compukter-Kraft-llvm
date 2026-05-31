// RUN: %clang --target=k16 -ffreestanding -fno-builtin -nostdlib -c %s -o %t.o
// RUN: llvm-readobj -h -S %t.o | FileCheck %s --check-prefix=OBJ

int main(void) {
  return 42;
}

// OBJ: Format: elf32-unknown
// OBJ: Machine: 0x5258
// OBJ: Name: .text.k16
