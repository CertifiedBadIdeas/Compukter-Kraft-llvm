// RUN: %clang_cc1 -E -dM -ffreestanding -triple k16 %s | FileCheck %s

// CHECK: #define __K16__ 1
// CHECK: #define __k16__ 1
