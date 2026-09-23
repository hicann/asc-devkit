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
    set(CMAKE_AICPU_OUTPUT_EXTENSION .o)
else()
    set(CMAKE_AICPU_OUTPUT_EXTENSION .obj)
endif()

set(CMAKE_INCLUDE_FLAG_AICPU "-I")

set(CMAKE_DEPFILE_FLAGS_AICPU "-MD -MT <DEP_TARGET> -MF <DEP_FILE>")
if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER) AND CMAKE_GENERATOR MATCHES "Makefiles|WMake")
    # dependencies are computed by the compiler itself
    set(CMAKE_AICPU_DEPFILE_FORMAT gcc)
    set(CMAKE_AICPU_DEPENDS_USE_COMPILER TRUE)
endif()

# -shared to create .so for shared library
if(NOT DEFINED CMAKE_SHARED_LIBRARY_CREATE_AICPU_FLAGS)
    set(CMAKE_SHARED_LIBRARY_CREATE_AICPU_FLAGS ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS})
endif()
# used for -Wl,-soname when creating shared library
if(NOT DEFINED CMAKE_SHARED_LIBRARY_SONAME_AICPU_FLAG)
  set(CMAKE_SHARED_LIBRARY_SONAME_AICPU_FLAG ${CMAKE_SHARED_LIBRARY_SONAME_C_FLAG})
endif()
# used for -Wl,-rpath when link executable has shared library
if(NOT DEFINED CMAKE_EXECUTABLE_RUNTIME_AICPU_FLAG)
    set(CMAKE_EXECUTABLE_RUNTIME_AICPU_FLAG ${CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG})
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_LOWER_PROCESSOR)
set(kernel_compile_options_list -O2 -c -std=c++17 -fvisibility=default -fvisibility-inlines-hidden
    -D_GLIBCXX_USE_CXX11_ABI=0 -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE
    -I$ENV{ASCEND_HOME_PATH}/${SYSTEM_LOWER_PROCESSOR}-linux/asc/include/aicpu_api
    --cce-aicpu-L$ENV{ASCEND_HOME_PATH}/${SYSTEM_LOWER_PROCESSOR}-linux/lib64/device/lib64
    --cce-aicpu-laicpu_api
    --cce-aicpu-toolkit-path=$ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/bin
    --cce-aicpu-sysroot=$ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/sysroot
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include/c++/7.3.0
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include/c++/7.3.0/aarch64-target-linux-gnu
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include/c++/7.3.0/backward
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include/c++/14.3.0
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include/c++/14.3.0/aarch64-target-linux-gnu
    -isystem $ENV{ASCEND_HOME_PATH}/toolkit/toolchain/hcc/aarch64-target-linux-gnu/include/c++/14.3.0/backward
    )
list(JOIN kernel_compile_options_list " " KERNEL_OPTIONS_LIST)

if(NOT CMAKE_AICPU_COMPILE_OBJECT)
    set(CMAKE_AICPU_COMPILE_OBJECT
	    "<CMAKE_AICPU_COMPILER> ${KERNEL_OPTIONS_LIST} <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -x aicpu <SOURCE>")
endif()

# Create a static archive incrementally for large object file counts.
if(NOT DEFINED CMAKE_AICPU_ARCHIVE_CREATE)
    set(CMAKE_AICPU_ARCHIVE_CREATE "<CMAKE_AR> qc <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()
# add without checking duplication
if(NOT DEFINED CMAKE_AICPU_ARCHIVE_APPEND)
    set(CMAKE_AICPU_ARCHIVE_APPEND "<CMAKE_AR> q <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()
if(NOT DEFINED CMAKE_AICPU_ARCHIVE_FINISH)
    set(CMAKE_AICPU_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")
endif()

# rule variable to create a shared module
if(NOT CMAKE_AICPU_CREATE_SHARED_MODULE)
	set(CMAKE_AICPU_CREATE_SHARED_MODULE ${CMAKE_AICPU_CREATE_SHARED_LIBRARY})
endif()

set(DEFAULT_LINK_LIBS "")
set(DEFAULT_LINK_PATH "")
if(NOT CMAKE_AICPU_LINK_EXECUTABLE)
    set(CMAKE_AICPU_LINK_EXECUTABLE
	    "<CMAKE_AICPU_COMPILER> <FLAGS> <CMAKE_AICPU_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${DEFAULT_LINK_PATH} ${DEFAULT_LINK_LIBS}")
endif()

if(NOT CMAKE_AICPU_CREATE_SHARED_LIBRARY)
    set(CMAKE_AICPU_CREATE_SHARED_LIBRARY
	    "<CMAKE_AICPU_COMPILER> <CMAKE_SHARED_LIBRARY_AICPU_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_AICPU_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES> ${DEFAULT_LINK_PATH} ${DEFAULT_LINK_LIBS}")
endif()

set(CMAKE_AICPU_INFORMATION_LOADED 1)
