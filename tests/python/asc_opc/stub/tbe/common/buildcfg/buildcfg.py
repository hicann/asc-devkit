#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

# from contextlib import contextmanager
class BuildConfig:
    # pylint: disable=no-member
    def __init__(self):
        pass

    def __enter__(self):
        # pylint: disable=protected-access
        # _api_internal._EnterBuildConfigScope(self)
        # if self.dump_pass_ir:
        # BuildConfig._dump_ir.enter()
        # return self
        print("stub llt atc opcompiler opc stub tbe common buildcfg buildcfg.py __enter__")

    def __exit__(self, ptype, value, trace):
        # if self.dump_pass_ir:
        # BuildConfig._dump_ir.exit()
        # _api_internal._ExitBuildConfigScope(self)
        print("stub llt atc opcompiler opc stub tbe common buildcfg buildcfg.py __exit__")


def build_config(**kwargs):
    print("build config")
    buildConfig = BuildConfig()
    return buildConfig


def set_current_build_config(key, value):
    print("set key: ", key, " : ", value)


def set_L1_info(key, value):
    return key, value
