; RUN: llc -mtriple=k16 < %s | FileCheck %s

declare void @llvm.memcpy.p0.p0.i32(ptr nocapture, ptr nocapture readonly, i32, i1 immarg)
declare void @callee(ptr sret([28 x i8]) align 4, i32, ptr, i32)

define void @copy_sret(ptr sret([40 x i8]) align 4 %out, ptr %self, i32 %len, i32 %pat) nounwind {
; CHECK-LABEL: copy_sret:
; CHECK: call32 callee
; CHECK: call32 memcpy
; CHECK: ret
entry:
  %src = alloca [28 x i8], align 4
  %tmp = alloca [40 x i8], align 4
  call void @callee(ptr sret([28 x i8]) align 4 %src, i32 %pat, ptr %self, i32 %len)
  %p28 = getelementptr inbounds i8, ptr %tmp, i32 28
  store i32 0, ptr %p28, align 4
  %p32 = getelementptr inbounds i8, ptr %tmp, i32 32
  store i32 %len, ptr %p32, align 4
  call void @llvm.memcpy.p0.p0.i32(ptr align 4 %tmp, ptr align 4 %src, i32 28, i1 false)
  call void @llvm.memcpy.p0.p0.i32(ptr align 4 %out, ptr align 4 %tmp, i32 40, i1 false)
  ret void
}
