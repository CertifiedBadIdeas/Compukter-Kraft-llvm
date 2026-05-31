include_directories(${CMAKE_CURRENT_BINARY_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_llvm_component_library(LLVMK16Info
  K16TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  K16
)

add_dependencies(LLVMK16Info K16CommonTableGen)
