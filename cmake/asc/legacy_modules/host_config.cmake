# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(ascend910b_list ascend910b1 ascend910b2 ascend910b2c ascend910b3 ascend910b4 ascend910b4-1 ascend910_9391 ascend910_9381 ascend910_9372 ascend910_9392 ascend910_9382 ascend910_9362)
set(ascend910_list  ascend910a ascend910proa ascend910b ascend910prob ascend910premiuma)
set(ascend310p_list ascend310p1 ascend310p3 ascend310p5 ascend310p7 ascend310p3vir01 ascend310p3vir02 ascend310p3vir04 ascend310p3vir08)
set(ascend310b_list ascend310b1 ascend310b2 ascend310b3 ascend310b4)
set(ascend950_list   ascend950pr_9599 ascend950pr_958a ascend950pr_9589 ascend950pr_958b ascend950pr_9579 ascend950pr_957b ascend950pr_957c ascend950pr_957d
    ascend950pr_950z ascend950dt_950x ascend950dt_950y ascend950dt_95a1 ascend950dt_95a2 ascend950dt_9591 ascend950dt_9592  ascend950dt_9595 ascend950dt_9596
    ascend950dt_9581 ascend950dt_9582 ascend950dt_9583 ascend950dt_9584 ascend950dt_9585 ascend950dt_9586 ascend950dt_9587 ascend950dt_9588 ascend950dt_9571
    ascend950dt_9572 ascend950dt_9573 ascend950dt_9574 ascend950dt_9575 ascend950dt_9576 ascend950dt_9577 ascend950dt_9578 ascend350_354f   ascend350_355e)
set(kirinx90_list kirinx90)
set(kirin9030_list kirin9030)
set(kirindev0000_list kirindev0000)
set(kirindev0001_list kirindev0001)
set(kirindev0002_list kirindev0002)
set(kirindev0003_list kirindev0003)
set(MC62_list MC62CM13AA MC62DM23AA MC62CM13AB MC62DM23AB MC62CM12AA MC62DM22AA MC62CM12AC MC62DM22AC MC62CM12AD MC62DM22AD MC62CM12AE MC62DM22AE MC62CM12AF MC62DM22AF MC62CM12AB MC62DM22AB)
set(all_product ${ascend910b_list} ${ascend910_list} ${ascend310p_list} ${ascend950_list} ${kirinx90_list} ${kirin9030_list} ${kirindev0000_list} ${kirindev0001_list} ${kirindev0002_list} ${kirindev0003_list} ${MC62_list})

if(NOT DEFINED SOC_VERSION)
    message(FATAL_ERROR "SOC_VERSION value not set.")
endif()

string(TOLOWER "${SOC_VERSION}" _LOWER_SOC_VERSION)

if(_LOWER_SOC_VERSION IN_LIST ascend950_list)
    set(DYNAMIC_MODE ON)
    set(BUILD_MODE   c310)
elseif(_LOWER_SOC_VERSION IN_LIST ascend910b_list)
    set(DYNAMIC_MODE ON)
    set(BUILD_MODE   c220)
elseif(_LOWER_SOC_VERSION IN_LIST ascend910_list)
    set(BUILD_MODE   c100)
elseif(_LOWER_SOC_VERSION IN_LIST ascend310p_list)
    set(BUILD_MODE   m200)
elseif(_LOWER_SOC_VERSION IN_LIST ascend310b_list)
    set(BUILD_MODE   m300)
elseif(_LOWER_SOC_VERSION IN_LIST kirinx90_list)
    set(BUILD_MODE   l300)
elseif(_LOWER_SOC_VERSION IN_LIST kirin9030_list)
    set(BUILD_MODE   l311)
elseif(_LOWER_SOC_VERSION IN_LIST kirindev0000_list)
    set(BUILD_MODE   5101)
elseif(_LOWER_SOC_VERSION IN_LIST kirindev0001_list)
    set(BUILD_MODE   5161)
elseif(_LOWER_SOC_VERSION IN_LIST kirindev0002_list)
    set(BUILD_MODE   5165)
elseif(_LOWER_SOC_VERSION IN_LIST kirindev0003_list)
    set(BUILD_MODE   5163)
elseif(_LOWER_SOC_VERSION IN_LIST MC62_list)
    set(BUILD_MODE   m510)
else()
    message(FATAL_ERROR "SOC_VERSION ${SOC_VERSION} does not support, the support list is ${all_product}")
endif()

if(NOT DEFINED RUN_MODE)
    set(RUN_MODE "npu")
endif()

if(NOT DEFINED ASCEND_KERNEL_LAUNCH_ONLY)
    set(ASCEND_KERNEL_LAUNCH_ONLY OFF)
endif()

if (NOT EXISTS "${ASCEND_CANN_PACKAGE_PATH}")
    message(FATAL_ERROR "${ASCEND_CANN_PACKAGE_PATH} does not exist, please check the setting of ASCEND_CANN_PACKAGE_PATH.")
endif()

set(ASCEND_PYTHON_EXECUTABLE "python3" CACHE STRING "python executable program")

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_LOWER_PROCESSOR)
if(EXISTS ${ASCEND_CANN_PACKAGE_PATH}/x86_64-linux/ccec_compiler AND SYSTEM_LOWER_PROCESSOR STREQUAL "x86_64")
    set(ASCENDC_DEVKIT_PATH ${ASCEND_CANN_PACKAGE_PATH}/x86_64-linux)
elseif(EXISTS ${ASCEND_CANN_PACKAGE_PATH}/aarch64-linux/ccec_compiler AND SYSTEM_LOWER_PROCESSOR STREQUAL "aarch64")
    set(ASCENDC_DEVKIT_PATH ${ASCEND_CANN_PACKAGE_PATH}/aarch64-linux)
elseif(EXISTS ${ASCEND_CANN_PACKAGE_PATH}/tools/ccec_compiler)
    set(ASCENDC_DEVKIT_PATH ${ASCEND_CANN_PACKAGE_PATH}/tools)
elseif(EXISTS ${ASCEND_CANN_PACKAGE_PATH}/compiler/ccec_compiler)
    set(ASCENDC_DEVKIT_PATH ${ASCEND_CANN_PACKAGE_PATH}/compiler)
else()
    set(ASCENDC_DEVKIT_PATH ${ASCEND_CANN_PACKAGE_PATH}/ascendc_devkit)
endif()

set(CCEC_PATH           ${ASCENDC_DEVKIT_PATH}/ccec_compiler/bin)
set(CCEC_LINKER        "${CCEC_PATH}/ld.lld")

set(ASCENDC_RUNTIME_OBJ_TARGET       ascendc_runtime_obj)
set(ASCENDC_RUNTIME_STATIC_TARGET    ascendc_runtime_static)
set(ASCENDC_RUNTIME_CONFIG           ascendc_runtime.cmake)
set(ASCENDC_PACK_KERNEL              ${ASCEND_CANN_PACKAGE_PATH}/bin/ascendc_pack_kernel)
set(ASCENDC_RUNTIME                  ${ASCEND_CANN_PACKAGE_PATH}/lib64/libascendc_runtime.a)

set(CMAKE_SKIP_RPATH TRUE)
include(ExternalProject)
