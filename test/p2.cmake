set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR p2)
SET(CMAKE_CROSSCOMPILING 1)
set(CMAKE_C_COMPILER_WORKS 1)

# Allow overriding LLVM bin path from command line: -Dllvm=/path/to/bin
if(NOT DEFINED llvm)
	set(llvm "/opt/p2llvm/bin")
endif()

set(target_ops "-fno-exceptions --target=p2")

# Add freestanding libc and libp2 headers so std headers resolve for P2 target
set(P2_SYS_INCLUDE "-isystem ${CMAKE_CURRENT_LIST_DIR}/../libc/include -isystem ${CMAKE_CURRENT_LIST_DIR}/../libp2/include")

set(CMAKE_C_FLAGS "${target_ops} -Oz -ffunction-sections -fdata-sections ${P2_SYS_INCLUDE}" CACHE STRING "" FORCE)
set(CMAKE_C_LINK_FLAGS "--target=p2 -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS "${target_ops} -Oz -ffunction-sections -fdata-sections ${P2_SYS_INCLUDE}" CACHE STRING "" FORCE)
set(CMAKE_CXX_LINK_FLAGS "--target=p2 -Wl,--gc-sections")
set(CMAKE_C_COMPILER ${llvm}/clang)
set(CMAKE_CXX_COMPILER ${llvm}/clang++)
set(CMAKE_AR ${llvm}/llvm-ar)
set(CMAKE_RANLIB ${llvm}/llvm-ranlib)
set(CMAKE_OBJCOPY ${llvm}/llvm-objcopy)