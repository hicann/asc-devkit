#!/usr/bin/env python
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

import argparse
import os
import multiprocessing
import const_var

MAKEFILE_TEMPLATE = """
# shared library name
LIBRARY_NAME := {}

# src file list
SRCS := {}

# output dir
BUILD_DIR := {}
LIB_DIR := $(BUILD_DIR)/

# target objs
OBJS_CPP := $(SRCS:%.cpp=$(BUILD_DIR)/$(LIBRARY_NAME)/%.o)
OBJS := $(OBJS_CPP:%.cc=$(BUILD_DIR)/$(LIBRARY_NAME)/%.o)

CXX := {}
CXXFLAGS := -fPIC {}
LDFLAGS := -shared

TARGET := $(LIB_DIR)/lib$(LIBRARY_NAME).so

all: $(TARGET)
\t{}

$(shell mkdir -p $(dir $(OBJS)) $(LIB_DIR))

$(TARGET): $(OBJS)
\t$(CXX) $(LDFLAGS) -o $@ $^ {}

$(BUILD_DIR)/$(LIBRARY_NAME)/%.o: %.cpp
\t$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(LIBRARY_NAME)/%.o: %.cc
\t$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
\trm -rf $(BUILD_DIR)

.PHONY: all clean
"""


def args_parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--src-file", nargs="*", required=True, help="input src files")
    parser.add_argument("-d", "--output-dir", required=True, help="output dir")
    parser.add_argument("-o", "--output", required=True, help="output")
    parser.add_argument("-p", "--cann-path", required=True, help="cann path")
    parser.add_argument("-j", "--parallel-jobs", required=False, help="compile parallel jobs")
    parser.add_argument("-c", "--compile-options", nargs="*", required=False, help="compile options")
    parser.add_argument("-l", "--link-options", nargs="*", required=False, help="compile options")

    parser.add_argument("-c++", "--cxx-compiler", required=False, help="compile options")
    return parser.parse_args()


def gen_compile_make_file(param_args):
    mkfile = os.path.join(param_args.output_dir, "ascendc_all_ops.make")
    debug_content = ""
    compile_options = ""
    link_options = ""
    if param_args.link_options is not None:
        link_options = param_args.link_options[0]
    if param_args.compile_options is not None:
        compile_options = param_args.compile_options[0]
        if "-g" not in param_args.compile_options[0]:
            debug_content = "rm -rf $(BUILD_DIR)/$(LIBRARY_NAME)/"
    else:
        debug_content = "rm -rf $(BUILD_DIR)/$(LIBRARY_NAME)/"
    with os.fdopen(os.open(mkfile, const_var.WFLAGS, const_var.WMODES), "w") as fd:
        compile_cmd = MAKEFILE_TEMPLATE.format(
            param_args.output,
            param_args.src_file[0],
            param_args.output_dir,
            param_args.cxx_compiler,
            f"-D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 {compile_options} -I{param_args.cann_path}/include",
            debug_content,
            f"{link_options} -lexe_graph -lregister -ltiling_api -L{param_args.cann_path}/lib64",
        )
        fd.write(compile_cmd)


def run_compile_cmd(param_args):
    mkfile = os.path.join(param_args.output_dir, "ascendc_all_ops.make")
    if not os.path.exists(mkfile):
        raise RuntimeError("ascendc_all_ops.make not exist")
    system_cpu_count = 2 * multiprocessing.cpu_count() - 2
    if param_args.parallel_jobs is not None:
        system_cpu_count = min(system_cpu_count, int(param_args.parallel_jobs))
    parallel_compile_job = system_cpu_count if system_cpu_count > 0 else 1
    cmd = f"make -f {mkfile} -j{parallel_compile_job}"
    ret = os.system(cmd)
    if ret != 0:
        raise RuntimeError("compile ascend all ops failed")


if __name__ == "__main__":
    args = args_parse()
    gen_compile_make_file(args)
    run_compile_cmd(args)
