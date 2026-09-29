#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

# 编译 libnpuops_exception_dump.so（在已安装 CANN 的容器/环境内执行）
set -e
cd "$(dirname "$0")"

if [ -n "$ASCEND_TOOLKIT_HOME" ]; then
    TOOLKIT="$ASCEND_TOOLKIT_HOME"
else
    TOOLKIT="/usr/local/Ascend/ascend-toolkit/latest"
fi
source "$TOOLKIT/set_env.sh" 2>/dev/null || true

rm -rf build && mkdir -p build && cd build
cmake .. && make -j
mkdir -p ../lib
cp -f libnpuops_exception_dump.so ../lib/
echo "build output: $(dirname "$PWD")/lib/libnpuops_exception_dump.so"
