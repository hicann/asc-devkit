#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
Setup asc_op_compile_base package
"""

import sys
import os

from setuptools import find_packages

# need to use distutils.core for correct placement of cython dll
if "--inplace" in sys.argv:
    from distutils.core import setup
    from distutils.extension import Extension
else:
    from setuptools import setup
    from setuptools.extension import Extension

# 指定共享库的路径
current_dir = os.getcwd()
print(current_dir)

shared_library_path = os.path.join(current_dir, "asc_op_compile_base", "c_api", "libasc_platform.so")
setup(
    name="asc_op_compile_base",
    version="0.1.0",
    description="asc_op_compile_base: base of op complication tool",
    zip_safe=False,
    install_requires=["numpy", "decorator", "attrs", "psutil"],
    packages=find_packages(),
    package_data={"asc_op_compile_base.c_api": [shared_library_path]},
    include_package_data=True,
)
