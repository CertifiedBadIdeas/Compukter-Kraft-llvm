add_llvm_component_group(K16)

set(LLVM_TARGET_DEFINITIONS K16.td)

tablegen(LLVM K16GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM K16GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM K16GenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM K16GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM K16GenDAGISel.inc -gen-dag-isel)

add_public_tablegen_target(K16CommonTableGen)

add_llvm_target(K16CodeGen
  K16AsmPrinter.cpp
  K16FrameLowering.cpp
  K16ISelDAGToDAG.cpp
  K16ISelLowering.cpp
  K16InstrInfo.cpp
  K16RegisterInfo.cpp
  K16Subtarget.cpp
  K16TargetMachine.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  Core
  MC
  K16Desc
  K16Info
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  K16
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
