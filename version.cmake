# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

set_cann_package(asc-devkit VERSION "9.2.0")

set_cann_build_dependencies(runtime "CUR_MAJOR_MINOR_VER")
set_cann_build_dependencies(metadef "CUR_MAJOR_MINOR_VER")

set_cann_run_dependencies(runtime "CUR_MAJOR_MINOR_VER")
set_cann_run_dependencies(ge-executor "CUR_MAJOR_MINOR_VER")
set_cann_run_dependencies(metadef "CUR_MAJOR_MINOR_VER")
set_cann_run_dependencies(bisheng-compiler "CUR_MAJOR_MINOR_VER")
set_cann_run_dependencies(tbe-tik "CUR_MAJOR_MINOR_VER")
