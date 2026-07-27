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
sudo update-alternatives --set gcc /usr/bin/gcc-14
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
