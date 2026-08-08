// RUN: %clang_cc1 -triple k16 -emit-llvm -o - %s | FileCheck %s

typedef __builtin_va_list va_list;

struct pair {
  int first;
  int second;
};

// CHECK-LABEL: define{{.*}} i32 @read_int(i32 noundef %{{.*}}, ...)
// CHECK: call void @llvm.va_start
// CHECK: va_arg ptr %{{.*}}, i32
// CHECK: call void @llvm.va_end
int read_int(int tag, ...) {
  va_list args;
  __builtin_va_start(args, tag);
  int value = __builtin_va_arg(args, int);
  __builtin_va_end(args);
  return value;
}

// CHECK-LABEL: define{{.*}} i32 @read_pair(i32 noundef %{{.*}}, ...)
// CHECK: va_arg ptr %{{.*}}, ptr
int read_pair(int tag, ...) {
  va_list args;
  __builtin_va_start(args, tag);
  struct pair value = __builtin_va_arg(args, struct pair);
  __builtin_va_end(args);
  return value.first + value.second;
}

// CHECK-LABEL: define{{.*}} i32 @call_read_int()
// CHECK: call i32 (i32, ...) @read_int(i32 noundef 7, i32 noundef 11)
int call_read_int(void) {
  return read_int(7, 11);
}

// CHECK-LABEL: define{{.*}} i32 @call_read_pair()
// CHECK: call i32 (i32, ...) @read_pair(i32 noundef 9, ptr noundef byval(%struct.pair) align 4 %{{.*}})
int call_read_pair(void) {
  struct pair value = {13, 17};
  return read_pair(9, value);
}

// CHECK-LABEL: define{{.*}} i32 @va_list_size()
// CHECK: ret i32 4
int va_list_size(void) {
  return sizeof(va_list);
}
