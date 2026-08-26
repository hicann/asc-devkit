#!/usr/bin/env python
# coding=utf-8
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import sys
import os
import re


_NAMESPACE = "AscendC::tiling"
_LEGACY_TILING_STRUCTS = [
    "LogSoftMaxTiling",
    "SoftMaxTiling",
    "TConv3DApiTiling",
    "TConv3DBpFilterTiling",
    "Conv3DBpFilterParams",
    "TConv3DBpFilterBasicBlockTiling",
    "Conv3DBackpropFilterTilingData",
    "TConv3DBackpropInputTiling",
    "Conv3DBackpropInputTilingData",
    "Mc2ServerCfg",
    "Mc2HcommCfg",
    "Mc2InitTiling",
    "Mc2CcTiling",
    "TCubeTiling",
    "BatchNormTiling",
    "DeepNormTiling",
    "GroupNormTiling",
    "LayerNormGradBetaTiling",
    "LayerNormGradTiling",
    "LayerNormTiling",
    "LayerNormSeparateTiling",
    "RmsNormTiling",
    "UnPadTiling",
    "PadTiling",
    "TopkTiling",
    "ConfusionTransposeTiling",
]


def get_tilingdata_list(find_dir, file_set):
    for root, _, code_files in os.walk(find_dir, followlinks=True):
        for code_file in code_files:
            if code_file.endswith("tilingdata.h"):
                file_set.add(os.path.join(root, code_file))


def gen_tiling(tiling_header_file):
    single_tiling_source = ""
    single_legacy_tiling_export = ""
    if not os.path.exists(tiling_header_file):
        print("warning: no userdef tiling header file: ", tiling_header_file)
        return single_tiling_source
    print("generate tiling def header file: ", tiling_header_file)
    pattern = re.compile(r"[(](.*)[)]", re.S)

    def parse_legacy_tiling(struct_def):
        # export legacy tiling structs with 'using namespace' to ensure compatibility
        nonlocal single_legacy_tiling_export
        if struct_def in _LEGACY_TILING_STRUCTS:
            single_legacy_tiling_export += f"using {_NAMESPACE}::{struct_def};\n"

    with open(tiling_header_file, "r") as fd:
        lines = fd.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith("BEGIN_TILING_DATA_DEF"):
                single_tiling_source += "#pragma pack(push, 8)\n"
                single_tiling_source += "struct "
                struct_def = re.findall(pattern, line)[0]
                single_tiling_source += struct_def + " {\n"
                parse_legacy_tiling(struct_def)
            elif line.startswith("TILING_DATA_FIELD_DEF_ARR"):
                field_params = re.findall(pattern, line)[0]
                fds = field_params.split(",")
                single_tiling_source += "    {} {}[{}] = {{}};\n".format(
                    fds[0].strip(), fds[2].strip(), fds[1].strip()
                )
            elif line.startswith("TILING_DATA_FIELD_DEF_STRUCT"):
                field_params = re.findall(pattern, line)[0]
                fds = field_params.split(",")
                single_tiling_source += "    {} {};\n".format(
                    fds[0].strip(), fds[1].strip()
                )
            elif line.startswith("TILING_DATA_FIELD_DEF"):
                field_params = re.findall(pattern, line)[0]
                fds = field_params.split(",")
                single_tiling_source += "    {} {} = 0;\n".format(
                    fds[0].strip(), fds[1].strip()
                )
            elif line.startswith("END_TILING_DATA_DEF"):
                single_tiling_source += "};\n"
                single_tiling_source += "#pragma pack(pop)\n"
    return single_tiling_source, single_legacy_tiling_export


if __name__ == "__main__":
    if len(sys.argv) <= 2:
        raise RuntimeError("arguments must greater than 2")
    res = """#ifndef __TIKCFW_KERNEL_TILING_H_
#define __TIKCFW_KERNEL_TILING_H_

#if defined(ASCENDC_CPU_DEBUG)
#include <cstdint>
#include <cstring>
#endif

"""
    res += "namespace AscendC {\nnamespace tiling {\n"

    print("[LOG]:  ", sys.argv[1], sys.argv[2])
    file_set = set()
    get_tilingdata_list(sys.argv[1], file_set)
    file_list = sorted(file_set)

    tiling_source = ""
    legacy_tiling_export = ""
    for file in file_list:
        src, exp = gen_tiling(file)
        if src in tiling_source:
            continue
        tiling_source += src
        legacy_tiling_export += exp

    res += tiling_source + "} // namespace tiling\n} // namespace AscendC\n\n"
    res += legacy_tiling_export

    res += "#endif\n"

    generate_file = sys.argv[2]
    absolute_file = os.path.abspath(generate_file)
    generate_dir = os.path.dirname(generate_file)
    if not os.path.exists(generate_dir):
        os.makedirs(generate_dir, exist_ok=True)

    with os.fdopen(
        os.open(absolute_file, os.O_RDWR | os.O_CREAT | os.O_TRUNC), "w"
    ) as ofd:
        ofd.write(res)
