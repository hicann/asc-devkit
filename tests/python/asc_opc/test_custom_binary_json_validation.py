#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import importlib
import json
import os
import sys
import types

import pytest


FILE_PATH = os.path.dirname(os.path.realpath(__file__))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
ASC_OPC_PATH = os.path.join(TOP_PATH, "tools/build/asc_opc/python/asc_opc_tool/")
OPC_STUB_PATH = os.path.join(TOP_PATH, "tests/python/asc_opc/stub/")
UTIL_PATH = os.path.join(TOP_PATH, "cmake/asc/fwk_modules/util/")


@pytest.fixture
def custom_binary_modules(monkeypatch):
    for path in (FRAMEWORK_PATH, ASC_OPC_PATH, OPC_STUB_PATH, UTIL_PATH):
        monkeypatch.syspath_prepend(path)

    log_stub = types.ModuleType("asc_op_compile_base.common.utils.log")
    for method_name in ("debug", "info", "warn", "warning", "error"):
        setattr(log_stub, method_name, lambda *_args, **_kwargs: None)
    utils_stub = types.ModuleType("asc_op_compile_base.common.utils")
    utils_stub.log = log_stub
    common_stub = types.ModuleType("asc_op_compile_base.common")
    common_stub.utils = utils_stub
    package_stub = types.ModuleType("asc_op_compile_base")
    package_stub.common = common_stub
    monkeypatch.setitem(sys.modules, "asc_op_compile_base", package_stub)
    monkeypatch.setitem(sys.modules, "asc_op_compile_base.common", common_stub)
    monkeypatch.setitem(sys.modules, "asc_op_compile_base.common.utils", utils_stub)
    monkeypatch.setitem(sys.modules, "asc_op_compile_base.common.utils.log", log_stub)

    op_manager_stub = types.ModuleType("op_manager")
    op_manager_stub.get_inout_info_from_opstore = lambda _op_type: (None, None)
    monkeypatch.setitem(sys.modules, "op_manager", op_manager_stub)
    monkeypatch.delitem(sys.modules, "op_compile_info_check", raising=False)
    monkeypatch.delitem(sys.modules, "split_opc_json_with_op_list", raising=False)

    check_module = importlib.import_module("op_compile_info_check")
    split_module = importlib.import_module("split_opc_json_with_op_list")
    constant_module = importlib.import_module("constant")
    return check_module, split_module.split_json_files, constant_module.OpcOptions


class RecordingLogger:
    def __init__(self):
        self.errors = []
        self.warnings = []

    def error(self, message, *args, **kwargs):
        self.errors.append(message % args if args else message)

    def debug(self, _message, *_args, **_kwargs):
        pass

    def warn(self, message, *args, **kwargs):
        self.warnings.append(message % args if args else message)


def _valid_config():
    return {
        "op_type": "AddCustomTemplate",
        "op_list": [
            {
                "bin_filename": "AddCustomTemplate_FP16",
                "simplified_key": "custom_fp16",
                "inputs": [
                    {
                        "name": "x",
                        "index": 0,
                        "dtype": "float16",
                        "format": "ND",
                        "shape": [-2],
                    },
                    {},
                ],
                "outputs": [
                    {
                        "name": "z",
                        "index": 0,
                        "dtype": "float16",
                        "format": "ND",
                        "shape": [-2],
                    }
                ],
            }
        ],
    }


def _check(tmp_path, content, modules):
    check_module, split_json_files, opc_options = modules
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")
    output_dir = tmp_path / "split"
    output_dir.mkdir()
    try:
        generated_files = split_json_files(
            str(path), str(output_dir), expected_op_type="AddCustomTemplate"
        )
    except ValueError:
        return False
    path = generated_files[0]
    return check_module.check_op_compilation_json(
        opc_options.INPUT_PARAM, {opc_options.INPUT_PARAM: str(path)}
    )[0]


def test_accepts_valid_custom_binary_dtype_and_format(tmp_path, custom_binary_modules):
    assert _check(tmp_path, _valid_config(), custom_binary_modules) is True


def test_accepts_format_stored_as_tuple_constant(tmp_path, custom_binary_modules):
    content = _valid_config()
    content["op_list"][0]["inputs"][0]["format"] = "DHWNC"

    assert _check(tmp_path, content, custom_binary_modules) is True


@pytest.mark.parametrize(
    ("field", "value"),
    [
        ("dtype", "fp17"),
        ("dtype", ["float16"]),
        ("format", "NOT_A_FORMAT"),
        ("format", {"name": "ND"}),
    ],
)
def test_rejects_invalid_custom_binary_dtype_or_format(
    tmp_path, custom_binary_modules, field, value
):
    content = _valid_config()
    content["op_list"][0]["inputs"][0][field] = value

    assert _check(tmp_path, content, custom_binary_modules) is False


def test_rejects_invalid_dynamic_tensor_dtype(tmp_path, custom_binary_modules):
    content = _valid_config()
    content["op_list"][0]["inputs"] = [
        [
            {
                "name": "x",
                "index": 0,
                "dtype": "fp17",
                "format": "ND",
                "shape": [-2],
            }
        ]
    ]

    assert _check(tmp_path, content, custom_binary_modules) is False


def test_invalid_value_message_contains_file_and_field_path(
    tmp_path, monkeypatch, custom_binary_modules
):
    check_module, split_json_files, opc_options = custom_binary_modules
    content = _valid_config()
    content["op_list"][0]["outputs"][0]["format"] = "NOT_A_FORMAT"
    path = tmp_path / "input.json"
    path.write_text(json.dumps(content), encoding="utf-8")
    output_dir = tmp_path / "split"
    output_dir.mkdir()
    generated_files = split_json_files(
        str(path),
        str(output_dir),
        expected_op_type="AddCustomTemplate",
    )
    logger = RecordingLogger()
    monkeypatch.setattr(check_module, "logger", logger)

    result, _ = check_module.check_op_compilation_json(
        opc_options.INPUT_PARAM, {opc_options.INPUT_PARAM: generated_files[0]}
    )

    assert result is False
    error = logger.errors[-1]
    assert str(path) in error
    assert "op_list[0].outputs[0].format" in error
    assert "NOT_A_FORMAT" in error


def test_invalid_value_message_keeps_original_op_index(
    tmp_path, monkeypatch, custom_binary_modules
):
    check_module, split_json_files, opc_options = custom_binary_modules
    content = _valid_config()
    second_op = json.loads(json.dumps(content["op_list"][0]))
    second_op["bin_filename"] = "AddCustomTemplate_FP32"
    second_op["simplified_key"] = "custom_fp32"
    second_op["inputs"][0]["dtype"] = "fp17"
    content["op_list"].append(second_op)
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")
    output_dir = tmp_path / "split"
    output_dir.mkdir()
    generated_files = split_json_files(
        str(path),
        str(output_dir),
        expected_op_type="AddCustomTemplate",
    )
    logger = RecordingLogger()
    monkeypatch.setattr(check_module, "logger", logger)

    result, _ = check_module.check_op_compilation_json(
        opc_options.INPUT_PARAM, {opc_options.INPUT_PARAM: generated_files[1]}
    )

    assert result is False
    assert str(path) in logger.errors[-1]
    assert "op_list[1].inputs[0].dtype" in logger.errors[-1]


def test_standard_mode_keeps_existing_validation_behavior(
    tmp_path, custom_binary_modules
):
    content = _valid_config()
    del content["op_list"][0]["simplified_key"]
    content["op_list"][0]["inputs"][0]["dtype"] = "legacy_custom_dtype"

    assert _check(tmp_path, content, custom_binary_modules) is True


def test_simplified_key_presence_enables_custom_validation(
    tmp_path, custom_binary_modules
):
    _, split_json_files, _ = custom_binary_modules
    content = _valid_config()
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    generated_files = split_json_files(
        str(path), str(tmp_path), expected_op_type="AddCustomTemplate"
    )

    with open(generated_files[0], "r", encoding="utf-8") as split_file:
        split_json = json.load(split_file)
    assert split_json["_ascendc_custom_binary_json"] == [str(path), 0]


def test_simplified_key_presence_requires_key_for_every_binary(
    tmp_path, custom_binary_modules
):
    _, split_json_files, _ = custom_binary_modules
    content = _valid_config()
    second_op = json.loads(json.dumps(content["op_list"][0]))
    second_op["bin_filename"] = "AddCustomTemplate_FP32"
    del second_op["simplified_key"]
    content["op_list"].append(second_op)
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    with pytest.raises(ValueError, match=r"op_list\[1\]\.simplified_key"):
        split_json_files(str(path), str(tmp_path), expected_op_type="AddCustomTemplate")


@pytest.mark.parametrize(
    ("content", "expected_error"),
    [
        ([], "root: expected object"),
        (
            {"op_type": "AddCustomTemplate", "op_list": []},
            "op_list: expected non-empty array",
        ),
        (
            {"op_type": "AddCustomTemplate", "op_list": [None]},
            "op_list[0]: expected object",
        ),
    ],
)
def test_rejects_invalid_binary_json_structure(
    tmp_path, custom_binary_modules, content, expected_error
):
    _, split_json_files, _ = custom_binary_modules
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    with pytest.raises(
        ValueError, match=expected_error.replace("[", r"\[").replace("]", r"\]")
    ):
        split_json_files(
            str(path),
            str(tmp_path),
            expected_op_type="AddCustomTemplate",
        )


def test_rejects_duplicate_binary_filename(tmp_path, custom_binary_modules):
    _, split_json_files, _ = custom_binary_modules
    content = _valid_config()
    content["op_list"].append(dict(content["op_list"][0]))
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    with pytest.raises(ValueError, match="duplicate output"):
        split_json_files(
            str(path),
            str(tmp_path),
            expected_op_type="AddCustomTemplate",
        )


def test_rejects_binary_filename_without_op_type_prefix(
    tmp_path, custom_binary_modules
):
    _, split_json_files, _ = custom_binary_modules
    content = _valid_config()
    content["op_list"][0]["bin_filename"] = "Kernel_FP16"
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    with pytest.raises(
        ValueError,
        match=r"op_list\[0\]\.bin_filename: expected 'AddCustomTemplate' or prefix 'AddCustomTemplate_'",
    ):
        split_json_files(
            str(path),
            str(tmp_path),
            expected_op_type="AddCustomTemplate",
        )


def test_rejects_duplicate_simplified_key(tmp_path, custom_binary_modules):
    _, split_json_files, _ = custom_binary_modules
    content = _valid_config()
    second_op = json.loads(json.dumps(content["op_list"][0]))
    second_op["bin_filename"] = "AddCustomTemplate_FP32"
    content["op_list"].append(second_op)
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    with pytest.raises(ValueError, match="duplicate value 'custom_fp16'"):
        split_json_files(
            str(path),
            str(tmp_path),
            expected_op_type="AddCustomTemplate",
        )


def test_rejects_binary_filenames_with_same_output_name(
    tmp_path, custom_binary_modules
):
    _, split_json_files, _ = custom_binary_modules
    content = _valid_config()
    second_op = json.loads(json.dumps(content["op_list"][0]))
    second_op["bin_filename"] = "AddCustomTemplate_FP16.json"
    content["op_list"].append(second_op)
    path = tmp_path / "binary.json"
    path.write_text(json.dumps(content), encoding="utf-8")

    with pytest.raises(ValueError, match="duplicate output"):
        split_json_files(
            str(path),
            str(tmp_path),
            expected_op_type="AddCustomTemplate",
        )


def test_incremental_split_truncates_existing_json(tmp_path, custom_binary_modules):
    _, split_json_files, _ = custom_binary_modules
    path = tmp_path / "binary.json"
    output_dir = tmp_path / "split"
    output_dir.mkdir()
    path.write_text(json.dumps(_valid_config()), encoding="utf-8")
    generated_files = split_json_files(
        str(path), str(output_dir), expected_op_type="AddCustomTemplate"
    )

    shorter_config = {
        "op_type": "AddCustomTemplate",
        "op_list": [
            {
                "bin_filename": "AddCustomTemplate_FP16",
                "simplified_key": "custom_fp16",
            }
        ],
    }
    path.write_text(json.dumps(shorter_config), encoding="utf-8")
    split_json_files(str(path), str(output_dir), expected_op_type="AddCustomTemplate")

    with open(generated_files[0], "r", encoding="utf-8") as split_file:
        split_json = json.load(split_file)
    assert split_json["op_list"] == shorter_config["op_list"]
