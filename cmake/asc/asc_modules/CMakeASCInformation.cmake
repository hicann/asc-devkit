# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

include(CMakeCommonLanguageInclude)

# extension for the output of a compile for a single file
if(UNIX)
    set(CMAKE_ASC_OUTPUT_EXTENSION .o)
else()
    set(CMAKE_ASC_OUTPUT_EXTENSION .obj)
endif()

set(CMAKE_INCLUDE_FLAG_ASC "-I")
set(CMAKE_ASC_COMPILE_OPTIONS_PIC "-fPIC")

# CMAKE_BUILD_TYPE: ASC officially supports Debug | Release; other values fall through with a warning.
if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
    message(WARNING
        "ASC: CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}' is not in the officially supported set (Debug | Release).")
endif()

# CMAKE_ASC_STANDARD: defaults to 17 and maps to the compiler's -std option.
if(NOT DEFINED CMAKE_ASC_STANDARD OR CMAKE_ASC_STANDARD STREQUAL "")
    set(CMAKE_ASC_STANDARD "${CMAKE_ASC_STANDARD_DEFAULT}")
endif()
if(NOT CMAKE_ASC_STANDARD MATCHES "^[0-9]+$")
    message(FATAL_ERROR
        "ASC: CMAKE_ASC_STANDARD='${CMAKE_ASC_STANDARD}' must be numeric, such as 17 or 20.")
endif()
if(CMAKE_ASC_STANDARD LESS 17)
    message(WARNING
        "ASC: CMAKE_ASC_STANDARD='${CMAKE_ASC_STANDARD}' is below the recommended C++17.")
endif()

set(_ASC_STANDARD_FLAG "-std=c++${CMAKE_ASC_STANDARD}")

# Per-config FLAGS default values; *_INIT only seeds cache on first configure.
if(NOT DEFINED CMAKE_ASC_FLAGS_DEBUG_INIT)
    set(CMAKE_ASC_FLAGS_DEBUG_INIT "-O0 -g")
endif()
if(NOT DEFINED CMAKE_ASC_FLAGS_RELEASE_INIT)
    set(CMAKE_ASC_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
endif()

# Inherit user-level flags from $ENV{ASCFLAGS}.
set(CMAKE_ASC_FLAGS_INIT "$ENV{ASCFLAGS} ${CMAKE_ASC_FLAGS_INIT}")

# Materialize CMAKE_ASC_FLAGS and CMAKE_ASC_FLAGS_<CONFIG> as cache STRINGs.
cmake_initialize_per_config_variable(CMAKE_ASC_FLAGS "Flags used by the ASC compiler")

# Auto-inject --npu-arch=<arch>.
if(CMAKE_ASC_ARCHITECTURES)
    string(APPEND CMAKE_ASC_FLAGS " --npu-arch=${CMAKE_ASC_ARCHITECTURES}")
endif()

# CMAKE_ASC_ENABLE_SIMT: when ON, inject --enable-simt into the compile rule.
option(CMAKE_ASC_ENABLE_SIMT
    "Pass --enable-simt to bisheng so ASC sources are compiled with SIMT support" OFF)
set(_ASC_SIMT_FLAG "")
if(CMAKE_ASC_ENABLE_SIMT)
    set(_ASC_SIMT_FLAG "--enable-simt")
endif()

# CMAKE_ASC_COMPILER_LAUNCHER: prefix prepended to ASC compile commands (e.g. ccache).
set(_ASC_COMPILER_LAUNCHER_PREFIX "")
if(CMAKE_ASC_COMPILER_LAUNCHER)
    string(JOIN " " _ASC_COMPILER_LAUNCHER_PREFIX ${CMAKE_ASC_COMPILER_LAUNCHER})
endif()

# CMAKE_ASC_LINKER_LAUNCHER: prefix prepended to ASC link commands.
set(_ASC_LINKER_LAUNCHER_PREFIX "")
if(CMAKE_ASC_LINKER_LAUNCHER)
    string(JOIN " " _ASC_LINKER_LAUNCHER_PREFIX ${CMAKE_ASC_LINKER_LAUNCHER})
endif()

# CMAKE_ASC_COMPILER_AR: archive tool for ASC static libraries; falls back to <CMAKE_AR>.
if(CMAKE_ASC_COMPILER_AR)
    set(_ASC_AR_TOOL "${CMAKE_ASC_COMPILER_AR}")
else()
    set(_ASC_AR_TOOL "<CMAKE_AR>")
endif()

# CMAKE_ASC_COMPILER_LINKER: link driver for ASC targets; falls back to <CMAKE_ASC_COMPILER>.
if(CMAKE_ASC_COMPILER_LINKER)
    set(_ASC_LINK_DRIVER "${CMAKE_ASC_COMPILER_LINKER}")
else()
    set(_ASC_LINK_DRIVER "<CMAKE_ASC_COMPILER>")
endif()

if(CMAKE_GENERATOR MATCHES "Ninja" AND CMAKE_VERSION VERSION_LESS "3.20")
    set(CMAKE_DEPFILE_FLAGS_ASC "-MD -MT $out -MF $DEP_FILE")
else()
    set(CMAKE_DEPFILE_FLAGS_ASC "-MD -MT <DEP_TARGET> -MF <DEP_FILE>")
endif()
if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER) AND CMAKE_GENERATOR MATCHES "Makefiles|WMake")
    # dependencies are computed by the compiler itself
    set(CMAKE_ASC_DEPFILE_FORMAT gcc)
    set(CMAKE_ASC_DEPENDS_USE_COMPILER TRUE)
endif()

# -shared to create .so for shared library
if(NOT DEFINED CMAKE_SHARED_LIBRARY_CREATE_ASC_FLAGS)
    set(CMAKE_SHARED_LIBRARY_CREATE_ASC_FLAGS ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS})
endif()
# used for -Wl,-soname when creating shared library
if(NOT DEFINED CMAKE_SHARED_LIBRARY_SONAME_ASC_FLAG)
  set(CMAKE_SHARED_LIBRARY_SONAME_ASC_FLAG ${CMAKE_SHARED_LIBRARY_SONAME_C_FLAG})
endif()
# used for -Wl,-rpath when link executable has shared library
if(NOT DEFINED CMAKE_EXECUTABLE_RUNTIME_ASC_FLAG)
    set(CMAKE_EXECUTABLE_RUNTIME_ASC_FLAG ${CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG})
endif()

# rule variable to compile a single .o file
# CMAKE_ASC_COMPILER: bisheng
if(NOT CMAKE_ASC_COMPILE_OBJECT)
    set(CMAKE_ASC_COMPILE_OBJECT "${_ASC_COMPILER_LAUNCHER_PREFIX} <CMAKE_ASC_COMPILER> <DEFINES> <INCLUDES> \
${_ASC_SIMT_FLAG} ${_ASC_STANDARD_FLAG} <FLAGS> -o <OBJECT> -c --asc-aicore-lang <SOURCE>")
endif()

# Create a static archive incrementally for large object file counts.
if(NOT DEFINED CMAKE_ASC_ARCHIVE_CREATE)
    set(CMAKE_ASC_ARCHIVE_CREATE "${_ASC_AR_TOOL} qc <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()
# add without checking duplication
if(NOT DEFINED CMAKE_ASC_ARCHIVE_APPEND)
    set(CMAKE_ASC_ARCHIVE_APPEND "${_ASC_AR_TOOL} q <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()
if(NOT DEFINED CMAKE_ASC_ARCHIVE_FINISH)
    set(CMAKE_ASC_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")
endif()

# Link rule for ASC executables (consumed by add_executable with ASC sources).
if(NOT CMAKE_ASC_LINK_EXECUTABLE)
    set(CMAKE_ASC_LINK_EXECUTABLE "${_ASC_LINKER_LAUNCHER_PREFIX} ${_ASC_LINK_DRIVER} <FLAGS> <CMAKE_ASC_LINK_FLAGS> \
<LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()

# rule variable to create a shared library
if(NOT CMAKE_ASC_CREATE_SHARED_LIBRARY)
    set(CMAKE_ASC_CREATE_SHARED_LIBRARY "${_ASC_LINKER_LAUNCHER_PREFIX} ${_ASC_LINK_DRIVER} \
<CMAKE_SHARED_LIBRARY_ASC_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_ASC_FLAGS> \
<SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
endif()

# rule variable to create a shared module : for fatbin(device.o)
# normal output: libxxx.so     ASC output: xxx.o
if(NOT CMAKE_ASC_CREATE_SHARED_MODULE)
    set(CMAKE_SHARED_MODULE_PREFIX_ASC "")
    set(CMAKE_SHARED_MODULE_SUFFIX_ASC ".o")
    set(CMAKE_ASC_CREATE_SHARED_MODULE "${CMAKE_ASC_LLD_LINKER} -m aicorelinux -Ttext=0 <OBJECTS> -static -o <TARGET>")
endif()

set(CMAKE_ASC_INFORMATION_LOADED 1)   # 标记Cmake已经加载初始化ASC编程语言

if(NOT DEFINED CMAKE_ASC_RUN_MODE OR CMAKE_ASC_RUN_MODE STREQUAL "")
    set(CMAKE_ASC_RUN_MODE "npu")
endif()

set(_ASC_SUPPORTED_RUN_MODES npu cpu sim)
if(NOT CMAKE_ASC_RUN_MODE IN_LIST _ASC_SUPPORTED_RUN_MODES)
    message(FATAL_ERROR
        "ASC: CMAKE_ASC_RUN_MODE='${CMAKE_ASC_RUN_MODE}' is unsupported, should be npu, cpu, or sim.")
endif()

if(CMAKE_ASC_RUN_MODE STREQUAL "sim")
    set(_ARCH_TO_DIR_MAP
        "dav-2002" "dav_2002"
        "dav-2201" "dav_2201"
        "dav-3510" "dav_3510"
    )
    list(FIND _ARCH_TO_DIR_MAP "${CMAKE_ASC_ARCHITECTURES}" _index)
    if(_index GREATER -1)
        math(EXPR _val_index "${_index} + 1")
        list(GET _ARCH_TO_DIR_MAP ${_val_index} _ASC_INTERNAL_DIR)
        set(_ASC_SIM_PATH "$ENV{ASCEND_HOME_PATH}/tools/simulator/${_ASC_INTERNAL_DIR}/lib")
        link_libraries(
            -Wl,-rpath,${_ASC_SIM_PATH}
            -Wl,-L${_ASC_SIM_PATH}
            -Wl,--disable-new-dtags
            runtime_camodel
            npu_drv_camodel
        )
        message(STATUS "ASC Simulator enabled: ${_ASC_SIM_PATH}")
    else()
        message(FATAL_ERROR "Unsupported ASC architecture for simulator: ${CMAKE_ASC_ARCHITECTURES}")
    endif()
endif()

if(CMAKE_ASC_RUN_MODE STREQUAL "cpu")
    # CPU模式架构映射表
    set(_ARCH_TO_SOC_DIR_MAP
        "dav-2002" "2002" "Ascend310P1"
        "dav-2201" "2201" "Ascend910B1"
        "dav-3510" "3510" "Ascend950PR_9599"
    )
    list(FIND _ARCH_TO_SOC_DIR_MAP "${CMAKE_ASC_ARCHITECTURES}" _index)
    math(EXPR _remainder "${_index} % 3")
    if(_remainder EQUAL 0) # 只允许dav-2002、dav-2201、dav-3510
        math(EXPR _short_index "${_index} + 1")
        math(EXPR _soc_dir_index "${_index} + 2")
        list(GET _ARCH_TO_SOC_DIR_MAP ${_short_index} _SHORT_NPU_ARCH)
        list(GET _ARCH_TO_SOC_DIR_MAP ${_soc_dir_index} _SOC_DIR)

        # 添加编译选项
        string(APPEND CMAKE_ASC_FLAGS " -g -D_GLIBCXX_USE_CXX11_ABI=0 --run-mode=cpu")

        # 配置链接选项
        string(APPEND CMAKE_ASC_LINK_FLAGS " -Wl,--disable-new-dtags")

        # 配置包含目录
        include_directories($ENV{ASCEND_HOME_PATH}/tools/tikicpulib/lib/include)

        # 配置链接库
        link_libraries(
            -Wl,-rpath,$ENV{ASCEND_HOME_PATH}/lib64
            -Wl,-L$ENV{ASCEND_HOME_PATH}/lib64
            -Wl,-rpath,$ENV{ASCEND_HOME_PATH}/tools/tikicpulib/lib
            -Wl,-L$ENV{ASCEND_HOME_PATH}/tools/tikicpulib/lib
            -Wl,-rpath,$ENV{ASCEND_HOME_PATH}/tools/tikicpulib/lib/${_SOC_DIR}
            -Wl,-L$ENV{ASCEND_HOME_PATH}/tools/tikicpulib/lib/${_SOC_DIR}
            -Wl,-rpath,$ENV{ASCEND_HOME_PATH}/tools/simulator/${_SOC_DIR}/lib
            -Wl,-L$ENV{ASCEND_HOME_PATH}/tools/simulator/${_SOC_DIR}/lib
            -Wl,-Bdynamic,--no-as-needed
            ascendc_acl_stub
            $<$<STREQUAL:${CMAKE_ASC_ARCHITECTURES},dav-2002>:_pvmodel>
            $<$<OR:$<STREQUAL:${CMAKE_ASC_ARCHITECTURES},dav-2201>,$<STREQUAL:${CMAKE_ASC_ARCHITECTURES},dav-3510>>:pem_davinci>
            cpudebug_cceprint
            cpudebug_npuchk
            cpudebug_stubreg
            cpudebug
            c_sec
            stdc++
            pthread
            dl
        )

    else()
        message(FATAL_ERROR "Unsupported ASC architecture for CPU mode: ${CMAKE_ASC_ARCHITECTURES}, should be dav-2002, dav-2201, dav-3510")
    endif()
endif()
