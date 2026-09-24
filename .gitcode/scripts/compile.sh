#!/bin/bash
# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------
cd ${WORKSPACE}
echo $(grep -E "^VERSION_ID=" /etc/os-release | cut -d'"' -f2)
if [[ "${task_name}" == *ubuntu24* || "${task_name}" == *24* ]]; then
    if sudo update-alternatives --set gcc /usr/bin/gcc-16 2>/dev/null; then
        echo "Switched to gcc-16"
    elif sudo update-alternatives --set gcc /usr/bin/gcc-15 2>/dev/null; then
        echo "Switched to gcc-15"
    elif sudo update-alternatives --set gcc /usr/bin/gcc-14 2>/dev/null; then
        echo "gcc-16/15 not available, fell back to gcc-14"
    fi
    export PATH=/opt/buildtools/python-3.10.2/bin:$PATH
else
    if [[ -f "/opt/rh/devtoolset-7/enable" ]]; then
        echo "source devtoolset"
        source /opt/rh/devtoolset-7/enable
    fi
    rm -rf /home/jenkins/opensource/lib_cache
    ln -s /home/jenkins/opensource/ubuntu20/lib_cache /home/jenkins/opensource/lib_cache
fi

if gcc --version | head -n1 | grep -q "15\."; then
    rm -rf /home/jenkins/opensource/lib_cache
    if [ -d /home/jenkins/opensource/gcc15 ]; then
        rm -rf /home/jenkins/opensource/gcc15/lib_cache/abseil-cpp
        rm -rf /home/jenkins/opensource/gcc15/lib_cache/device/abseil-cpp
        ln -s /home/jenkins/opensource/gcc15/lib_cache/ /home/jenkins/opensource/lib_cache
    elif [ -d /home/jenkins/opensource/gcc15x86 ]; then
        rm -rf /home/jenkins/opensource/gcc15x86/lib_cache/abseil-cpp
        rm -rf /home/jenkins/opensource/gcc15x86/lib_cache/device/abseil-cpp
        ln -s /home/jenkins/opensource/gcc15x86/lib_cache/ /home/jenkins/opensource/lib_cache
    fi
elif gcc --version | head -n1 | grep -q "14\."; then
    gcc --version
else
    gcc --version
    rm -rf /home/jenkins/opensource/lib_cache
    ln -s /home/jenkins/opensource/ubuntu20/lib_cache /home/jenkins/opensource/lib_cache
fi
source /home/jenkins/Ascend/cann/bin/setenv.bash
set +e
case "${task_name}" in
    x86_compile_ubuntu24)
        sed -i "1i set(CMAKE_EXPORT_COMPILE_COMMANDS ON)" "CMakeLists.txt"
        ;;
esac
if [ "${TARGET_BRANCH}" == master ]; then
        bash build.sh --pkg --cann_3rd_lib_path="/home/jenkins/opensource" -f "pr_filelist.txt"
        echo "exec cmd: [bash build.sh --pkg --cann_3rd_lib_path="/home/jenkins/opensource" -f "pr_filelist.txt"]"
elif [ "${TARGET_BRANCH}" == exp/kadc ] || [ "${TARGET_BRANCH}" == feature/tensor_api_from_9.0.0 ]; then
        echo "not need build"
        mkdir -p build_out
        touch ${WORKSPACE}/build_out/cann-asc-devkit.run
else
        bash build.sh --pkg --cann_3rd_lib_path="/home/jenkins/opensource"
fi

if [[ "${task_name}" =~ x86_compile_ubuntu24 ]] && [ -f "build_out/"*.run ] && [ "${TARGET_BRANCH}" == master ]; then
    echo "api-check=compile" >> "${ATOMGIT_OUTPUT}"
else
    echo "api-check=continue" >> "${ATOMGIT_OUTPUT}"
fi
if [ ! -f "build_out/"*.run ]; then
        mkdir -p build_out
        touch build_out/cann-asc-devkit.run
fi
