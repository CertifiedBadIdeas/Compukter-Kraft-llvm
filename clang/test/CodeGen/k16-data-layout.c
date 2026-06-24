// RUN: %clang_cc1 -triple k16 -emit-llvm -o - %s | FileCheck %s

int main(void) {
  return 42;
}

// CHECK: target datalayout = "e-p:32:32-i32:32-i64:64-n32-S64"
