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
    sudo update-alternatives --set gcc /usr/bin/gcc-14
else
    if [[ -f "/opt/rh/devtoolset-7/enable" ]]; then
        echo "source devtoolset"
        source /opt/rh/devtoolset-7/enable
    fi
fi
gcc --version
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
