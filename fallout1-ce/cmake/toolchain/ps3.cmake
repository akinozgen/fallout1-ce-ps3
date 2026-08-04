# CMake toolchain file - PlayStation 3 (PSL1GHT / ps3dev)
#
# Usage (inside the ps3dev Docker image):
#   cmake -B build-ps3 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/ps3.cmake
#   cmake --build build-ps3 -j

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc64)

set(PS3DEV "$ENV{PS3DEV}")
if(NOT PS3DEV)
    set(PS3DEV "/usr/local/ps3dev")
endif()
set(PSL1GHT "$ENV{PSL1GHT}")
if(NOT PSL1GHT)
    set(PSL1GHT "${PS3DEV}")
endif()

set(CMAKE_C_COMPILER   "${PS3DEV}/ppu/bin/powerpc64-ps3-elf-gcc")
set(CMAKE_CXX_COMPILER "${PS3DEV}/ppu/bin/powerpc64-ps3-elf-g++")
set(CMAKE_ASM_COMPILER "${PS3DEV}/ppu/bin/powerpc64-ps3-elf-gcc")

set(CMAKE_FIND_ROOT_PATH "${PS3DEV}/ppu" "${PS3DEV}/portlibs/ppu" "${PSL1GHT}/ppu")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Link tests do not work without the lv2 runtime; make try_compile build a static lib.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(PS3_COMMON_FLAGS "-mcpu=cell -mhard-float -fmodulo-sched -ffunction-sections -fdata-sections -D__PS3__ -D__PSL1GHT__ -isystem ${PSL1GHT}/ppu/include")

set(CMAKE_C_FLAGS_INIT   "${PS3_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${PS3_COMMON_FLAGS} -D_GLIBCXX11_USE_C99_STDIO")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-L${PSL1GHT}/ppu/lib -Wl,--gc-sections")

# For the platform branches inside CMakeLists
set(PS3 TRUE)
