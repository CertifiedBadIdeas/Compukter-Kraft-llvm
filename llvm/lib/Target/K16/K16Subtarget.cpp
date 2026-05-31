//===-- K16Subtarget.cpp - K16 Subtarget Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "K16Subtarget.h"
#include "K16TargetMachine.h"
#include "llvm/CodeGen/LibcallLoweringInfo.h"

using namespace llvm;

#define DEBUG_TYPE "k16-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "K16GenSubtargetInfo.inc"

void K16Subtarget::anchor() {}

K16Subtarget::K16Subtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS,
                               const TargetMachine &TM)
    : K16GenSubtargetInfo(TT, CPU.empty() ? "generic" : CPU,
                            CPU.empty() ? "generic" : CPU, FS),
      InstrInfo(*this), FrameLowering(), TLInfo(TM, *this) {}

void K16Subtarget::initLibcallLoweringInfo(LibcallLoweringInfo &Info) const {
  Info.setLibcallImpl(RTLIB::MEMCPY, RTLIB::impl_memcpy);
  Info.setLibcallImpl(RTLIB::MEMMOVE, RTLIB::impl_memmove);
  Info.setLibcallImpl(RTLIB::MEMSET, RTLIB::impl_memset);
  Info.setLibcallImpl(RTLIB::SDIV_I32, RTLIB::impl___divsi3);
  Info.setLibcallImpl(RTLIB::UDIV_I32, RTLIB::impl___udivsi3);
  Info.setLibcallImpl(RTLIB::SREM_I32, RTLIB::impl___modsi3);
  Info.setLibcallImpl(RTLIB::UREM_I32, RTLIB::impl___umodsi3);
  Info.setLibcallImpl(RTLIB::SDIV_I64, RTLIB::impl___divdi3);
  Info.setLibcallImpl(RTLIB::UDIV_I64, RTLIB::impl___udivdi3);
  Info.setLibcallImpl(RTLIB::SREM_I64, RTLIB::impl___moddi3);
  Info.setLibcallImpl(RTLIB::UREM_I64, RTLIB::impl___umoddi3);
  Info.setLibcallImpl(RTLIB::SDIV_I128, RTLIB::impl___divti3);
  Info.setLibcallImpl(RTLIB::UDIV_I128, RTLIB::impl___udivti3);
  Info.setLibcallImpl(RTLIB::SREM_I128, RTLIB::impl___modti3);
  Info.setLibcallImpl(RTLIB::UREM_I128, RTLIB::impl___umodti3);
  Info.setLibcallImpl(RTLIB::ADD_F32, RTLIB::impl___addsf3);
  Info.setLibcallImpl(RTLIB::SUB_F32, RTLIB::impl___subsf3);
  Info.setLibcallImpl(RTLIB::MUL_F32, RTLIB::impl___mulsf3);
  Info.setLibcallImpl(RTLIB::DIV_F32, RTLIB::impl___divsf3);
  Info.setLibcallImpl(RTLIB::ADD_F64, RTLIB::impl___adddf3);
  Info.setLibcallImpl(RTLIB::SUB_F64, RTLIB::impl___subdf3);
  Info.setLibcallImpl(RTLIB::MUL_F64, RTLIB::impl___muldf3);
  Info.setLibcallImpl(RTLIB::DIV_F64, RTLIB::impl___divdf3);
  Info.setLibcallImpl(RTLIB::OEQ_F32, RTLIB::impl___eqsf2);
  Info.setLibcallImpl(RTLIB::UNE_F32, RTLIB::impl___nesf2);
  Info.setLibcallImpl(RTLIB::OGE_F32, RTLIB::impl___gesf2);
  Info.setLibcallImpl(RTLIB::OLT_F32, RTLIB::impl___ltsf2);
  Info.setLibcallImpl(RTLIB::OLE_F32, RTLIB::impl___lesf2);
  Info.setLibcallImpl(RTLIB::OGT_F32, RTLIB::impl___gtsf2);
  Info.setLibcallImpl(RTLIB::UO_F32, RTLIB::impl___unordsf2);
  Info.setLibcallImpl(RTLIB::OEQ_F64, RTLIB::impl___eqdf2);
  Info.setLibcallImpl(RTLIB::UNE_F64, RTLIB::impl___nedf2);
  Info.setLibcallImpl(RTLIB::OGE_F64, RTLIB::impl___gedf2);
  Info.setLibcallImpl(RTLIB::OLT_F64, RTLIB::impl___ltdf2);
  Info.setLibcallImpl(RTLIB::OLE_F64, RTLIB::impl___ledf2);
  Info.setLibcallImpl(RTLIB::OGT_F64, RTLIB::impl___gtdf2);
  Info.setLibcallImpl(RTLIB::UO_F64, RTLIB::impl___unorddf2);
  Info.setLibcallImpl(RTLIB::FPEXT_F16_F32, RTLIB::impl___extendhfsf2);
  Info.setLibcallImpl(RTLIB::FPEXT_F16_F64, RTLIB::impl___extendhfdf2);
  Info.setLibcallImpl(RTLIB::FPEXT_F32_F64, RTLIB::impl___extendsfdf2);
  Info.setLibcallImpl(RTLIB::FPROUND_F32_F16, RTLIB::impl___truncsfhf2);
  Info.setLibcallImpl(RTLIB::FPROUND_F64_F16, RTLIB::impl___truncdfhf2);
  Info.setLibcallImpl(RTLIB::FPTOSINT_F32_I32, RTLIB::impl___fixsfsi);
  Info.setLibcallImpl(RTLIB::FPTOSINT_F32_I64, RTLIB::impl___fixsfdi);
  Info.setLibcallImpl(RTLIB::FPTOSINT_F64_I32, RTLIB::impl___fixdfsi);
  Info.setLibcallImpl(RTLIB::FPTOSINT_F64_I64, RTLIB::impl___fixdfdi);
  Info.setLibcallImpl(RTLIB::FPTOUINT_F32_I32, RTLIB::impl___fixunssfsi);
  Info.setLibcallImpl(RTLIB::FPTOUINT_F32_I64, RTLIB::impl___fixunssfdi);
  Info.setLibcallImpl(RTLIB::FPTOUINT_F64_I32, RTLIB::impl___fixunsdfsi);
  Info.setLibcallImpl(RTLIB::FPTOUINT_F64_I64, RTLIB::impl___fixunsdfdi);
  Info.setLibcallImpl(RTLIB::SINTTOFP_I64_F32, RTLIB::impl___floatdisf);
  Info.setLibcallImpl(RTLIB::SINTTOFP_I64_F64, RTLIB::impl___floatdidf);
  Info.setLibcallImpl(RTLIB::UINTTOFP_I64_F32, RTLIB::impl___floatundisf);
  Info.setLibcallImpl(RTLIB::UINTTOFP_I64_F64, RTLIB::impl___floatundidf);
  Info.setLibcallImpl(RTLIB::CTLZ_I32, RTLIB::impl___clzsi2);
  Info.setLibcallImpl(RTLIB::CTPOP_I32, RTLIB::impl___popcountsi2);
}
