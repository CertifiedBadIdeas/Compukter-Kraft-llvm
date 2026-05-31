add_llvm_component_library(LLVMK16Desc
  K16AsmBackend.cpp
  K16ELFObjectWriter.cpp
  K16InstPrinter.cpp
  K16MCCodeEmitter.cpp
  K16MCAsmInfo.cpp
  K16MCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  K16Info
  Support
  TargetParser

  ADD_TO_COMPONENT
  K16
)
