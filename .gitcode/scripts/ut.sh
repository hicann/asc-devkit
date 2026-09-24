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
set -x
cd ${WORKSPACE}
echo $(grep -E "^VERSION_ID=" /etc/os-release | cut -d'"' -f2)
if sudo update-alternatives --set gcc /usr/bin/gcc-16 2>/dev/null; then
    echo "Switched to gcc-16"
elif sudo update-alternatives --set gcc /usr/bin/gcc-15 2>/dev/null; then
    echo "Switched to gcc-15"
elif sudo update-alternatives --set gcc /usr/bin/gcc-14 2>/dev/null; then
    echo "gcc-16/15 not available, fell back to gcc-14"
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
export PATH=/opt/buildtools/python-3.10.2/bin:$PATH
gcc --version
source /home/jenkins/Ascend/cann/bin/setenv.bash
set +e
if [ "$TARGET_BRANCH" = "master" ] || [ "$TARGET_BRANCH" == "9.1.0-beta.2" ]; then
    bash build.sh --${ut_type} --cov --cann_3rd_lib_path="/home/jenkins/opensource" -f "pr_filelist.txt"
    ret=$?
    coverage_save="true"
elif [ "$TARGET_BRANCH" = "exp/kadc" ]; then
    echo "not need run ut"
    exit 0
    ret=$?
elif [ "$TARGET_BRANCH" = "feature/tensor_api_from_9.0.0" ]; then
  if [ "${ut_type}" = "UT_Test_basic_test_three" ] || [ "${ut_type}" == "UT_Test_basic_test_two" ] || [ "${ut_type}" == "UT_Test_basic_test_one" ]; then
    bash build.sh --${ut_type} --cann_3rd_lib_path="/home/jenkins/opensource"
    coverage_save="true"
  else
    echo "not need run ut"
    exit 0
  fi
  ret=$?
else
  if [ "${ut_type}" = "UT_Test_basic_test_three" ] || [ "${ut_type}" == "UT_Test_basic_test_two" ] || [ "${ut_type}" == "UT_Test_basic_test_one" ] || [ "${ut_type}" == "UT_Test_adv_test*" ]; then
    bash build.sh --${ut_type} --cov --cann_3rd_lib_path="/home/jenkins/opensource"
    coverage_save="true"
  else
    echo "not need run ut"
    exit 0
  fi
  ret=$?
fi

if [ $ret -ne 200 ] && [ $ret -ne 0 ]; then
    echo "run ut fail"
    exit 1
fi

if [ $ret -eq 0 ]; then
    if [ "$coverage_save" = "true" ];then
    echo "ut_process=coverage" >> $ATOMGIT_OUTPUT
    else
    echo "ut_process=ut_cov" >> $ATOMGIT_OUTPUT
    fi
fi
exit 0
