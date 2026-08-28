#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import json
import sys
from collections import defaultdict
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
UTIL_DIR = REPO_ROOT / "cmake/asc/fwk_modules/util"
sys.path.insert(0, str(UTIL_DIR))

import ascendc_bin_param_build


def test_parse_keeps_simplified_key_mode_as_text_until_final_resolution():
    mode_info = defaultdict(dict)

    ascendc_bin_param_build._process_simplified_key_mode_option(
        "AddCustomTemplate", "--simplified_key_mode=1", mode_info
    )

    assert mode_info["AddCustomTemplate"] == "1"


class _FakeOpDesc:
    def __init__(self):
        self.op_type = "AddCustomTemplate"
        self.simplified_key_mode = 0
        self.json_file = ""
        self.final_json_file = ""

    def set_soc_version(self, _soc):
        pass

    def set_out_path(self, _out_path):
        pass

    def set_kernel_template_input(self, _value):
        pass

    def gen_input_json_based_on_specified_json(self, json_file, _out_dir):
        self.final_json_file = json_file


def test_op_specific_config_overrides_all_config(tmp_path, monkeypatch):
    cfg_file = tmp_path / "op_info.ini"
    cfg_file.write_text("", encoding="utf-8")
    opc_file = tmp_path / "custom_opc_options.ini"
    opc_file.write_text(
        "ALL@ALL@--kernel-json-file=/all.json@--simplified_key_mode=0\n"
        "AddCustomTemplate@ALL@--kernel-json-file=/op.json@--simplified_key_mode=1\n",
        encoding="utf-8",
    )
    op_desc = _FakeOpDesc()
    monkeypatch.setattr(
        ascendc_bin_param_build.opdesc_parser,
        "get_op_desc",
        lambda *_args, **_kwargs: [op_desc],
    )

    ascendc_bin_param_build.gen_bin_param_file(
        str(cfg_file), str(tmp_path), "ascend910b", str(opc_file)
    )

    assert op_desc.final_json_file == "/op.json"
    assert op_desc.simplified_key_mode == 1


def test_build_command_passes_default_numeric_mode(tmp_path):
    builder = ascendc_bin_param_build.BinParamBuilder("AddCustomTemplate")
    builder.op_file = "add_custom_template"
    builder.op_intf = "add_custom_template"
    builder.soc = "ascend910b"
    builder.out_path = str(tmp_path)

    builder._write_build_cmd(
        str(tmp_path / "param.json"),
        "AddCustomTemplate_FP16",
        0,
        str(tmp_path),
    )

    command = (tmp_path / "AddCustomTemplate-add_custom_template-0.sh").read_text(
        encoding="utf-8"
    )
    assert "--simplified_key_mode=0" in command
    assert "--simplified_key_mode=None" not in command


def test_kernel_json_without_simplified_key_keeps_legacy_behavior(
    tmp_path, monkeypatch
):
    json_file = tmp_path / "legacy.json"
    json_file.write_text(
        json.dumps(
            {
                "op_type": "AddCustomTemplate",
                "op_list": [{"bin_filename": "AddCustomTemplate_FP16"}],
            }
        ),
        encoding="utf-8",
    )
    builder = ascendc_bin_param_build.BinParamBuilder("AddCustomTemplate")
    build_commands = []
    monkeypatch.setattr(
        builder,
        "_write_build_cmd",
        lambda *args: build_commands.append(args),
    )

    builder.gen_input_json_based_on_specified_json(str(json_file), str(tmp_path))

    assert len(build_commands) == 1
    split_json = json.loads(Path(build_commands[0][0]).read_text(encoding="utf-8"))
    assert "_ascendc_custom_binary_json" not in split_json
