#!/usr/bin/python
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

"""Exercise real parsers with isolated context/logging dependencies, without an SDK."""

import ast
import copy
import hashlib
import json
import os
import re
import importlib.util
import shutil
import subprocess
import sys
import tempfile
import types
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
BUILD = ROOT / "tools/build/asc_op_compile_base"


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


class DeterministicLevelsTest(unittest.TestCase):
    def setUp(self):
        modules = patch.dict(sys.modules)
        modules.start()
        self.addCleanup(modules.stop)
        self.selected = None
        self.flag = False
        self.additions = {}
        for name in (
            "asc_op_compile_base",
            "asc_op_compile_base.common",
            "tbe",
            "tbe.common",
            "asc_op_compile_base.asc_op_compiler",
            "asc_op_compile_base.adapter",
        ):
            module = types.ModuleType(name)
            module.__path__ = []
            sys.modules[name] = module
        self.helper = load("asc_op_compile_base.common.deterministic", BUILD / "common/deterministic.py")
        load("asc_op_compile_base.adapter.deterministic", BUILD / "adapter/deterministic.py")
        for package in ("asc_op_compile_base.common", "tbe.common"):
            context = types.ModuleType(package + ".context")
            context.get_context = lambda: types.SimpleNamespace(
                get_addition=lambda name: (
                    self.selected if name == "deterministic_level_filter" else self.additions.get(name)
                ),
                add_addition=lambda name, value: self.additions.update({name: value}),
            )
            sys.modules[context.__name__] = context
            config = types.ModuleType(package + ".buildcfg")
            config.get_current_build_config = lambda name: self.flag if name == "enable_deterministic_mode" else 0
            sys.modules[config.__name__] = config
        log = types.ModuleType("asc_op_compile_base.common.utils.log_utils")
        log.AscendCLogLevel = types.SimpleNamespace(LOG_INFO=0, LOG_WARNING=1)
        sys.modules[log.__name__] = log
        self.parsers = []
        for package in ("asc_op_compiler", "adapter"):
            prefix = "asc_op_compile_base." + package
            utility = types.ModuleType(prefix + ".ascendc_common_utility")
            utility.CommonUtility = types.SimpleNamespace(print_compile_log=lambda *args: None)
            sys.modules[utility.__name__] = utility
            info = types.ModuleType(prefix + ".get_op_tiling")
            info.OpInfo = object
            sys.modules[info.__name__] = info
            sys.modules[prefix + ".log_utils"] = log
            self.parsers.append(load(prefix + ".template_tiling", BUILD / package / "template_tiling.py"))

    def declaration(self):
        return "@@ASCENDC_TPL_ARGS_DECL_AddCustom@@ = {@@ASCENDC_TPL_BOOL_DECL_B@@ = {0,1}}"

    def group(self, bit, flag=None, levels=None, legacy=False):
        text = "@@{@@ASCENDC_TPL_BOOL_SEL_B@@ = {" + str(bit) + "}"
        if flag is not None:
            text += ",@@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {" + str(flag).lower() + "}"
        if levels is not None:
            text += ",@@ASCENDC_TPL_DETERMINISTIC_LEVEL_SEL@@ = {" + levels + "}"
        if legacy:
            text += ",@@ASCENDC_TPL_LEGACY_SEL@@ = {true}"
        return text + "}"

    def initialize(self, parser, *groups):
        parser.extract_template_tiling_info(self.declaration(), "@@ASCENDC_TPL_LISTS@@ = {" + ",".join(groups) + "}")

    def test_defaults_and_conflicts(self):
        normalize = self.helper.normalize_deterministic
        self.assertEqual(normalize(), (False, (0,)))
        self.assertEqual(normalize(True), (True, (1,)))
        self.assertEqual(normalize(None, "1,2"), (True, (1, 2)))
        for level in range(4):
            self.assertEqual(normalize(None, [level]), (level != 0, (level,)))
        for flag, levels in (
            (True, "0"),
            (False, "1"),
            (False, "2"),
            (False, "3"),
            (True, "0,3"),
            (True, "1,1"),
            (True, "4"),
        ):
            with self.subTest(flag=flag, levels=levels), self.assertRaises(ValueError):
                normalize(flag, levels)

    def test_dt_keys_and_unopted_identity(self):
        key = "AddCustom/d=1,p=1/0,2/0,2/attrs"
        self.assertEqual(self.helper.level_simplified_keys([key], None), [key])
        self.assertEqual(
            self.helper.level_simplified_keys([key], "1,2"),
            ["AddCustom/d=1,p=1,dt=1/0,2/0,2/attrs", "AddCustom/d=1,p=1,dt=2/0,2/0,2/attrs"],
        )
        with self.assertRaises(ValueError):
            self.helper.level_simplified_keys(["AddCustom/d=1,dt=1/x"], "1")

    def test_production_simplified_key_entry_normalizes_compiler_ignore(self):
        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        load("constant", opc / "constant.py")
        logger = types.SimpleNamespace(
            info=lambda *_: None, warn=lambda *_: None, error=lambda *_: None, debug=lambda *_: None
        )
        utils = types.ModuleType("asc_op_compile_base.common.utils")
        utils.log = logger
        sys.modules[utils.__name__] = utils
        common = types.ModuleType("opc_common")
        common.check_input_and_output_for_simplified_key = lambda _: (False, False, False, False)
        common.check_attr_for_simpilified_key = lambda _: (False, False, "")
        sys.modules[common.__name__] = common
        module = load("deterministic_production_simplified_keys", opc / "simplified_key_utils.py")
        op, info = (
            {"simplified_key": "0,2/0,2/attrs"},
            {"op_type": "AddCustom", "impl_mode": "high_performance", "_deterministic_level_filter": (1, 2)},
        )
        keys, mode = module.generate_simplified_key({}, op, info, "ignore", None)
        self.assertEqual(mode, 2)
        self.assertEqual(keys, ["AddCustom/d=1,p=1,dt=1/0,2/0,2/attrs", "AddCustom/d=1,p=1,dt=2/0,2/0,2/attrs"])
        info["_deterministic_level_filter"] = (0, 1, 2)
        with self.assertRaises(ValueError):
            module.generate_simplified_key({}, op, info, "ignore", None)
        info["_deterministic_level_filter"] = (1, 2)
        info["optional_impl_mode_flag"] = True
        keys = module.generate_simplified_key({}, op, info, "ignore", None)[0]
        self.assertEqual(len(keys), 4)
        self.assertEqual(
            {key.split("/")[1] for key in keys}, {"d=1,p=0,dt=1", "d=1,p=1,dt=1", "d=1,p=0,dt=2", "d=1,p=1,dt=2"}
        )
        info.pop("optional_impl_mode_flag")
        info["_deterministic_level_filter"] = (0,)
        self.assertEqual(
            module.generate_simplified_key({}, op, info, "ignore", None)[0], ["AddCustom/d=0,p=1,dt=0/0,2/0,2/attrs"]
        )
        info["_deterministic_level_filter"] = (3,)
        with self.assertRaises(ValueError):
            module.generate_simplified_key({}, op, info, "false", None)
        del info["_deterministic_level_filter"]
        self.assertEqual(
            module.generate_simplified_key({}, op, info, "ignore", None)[0],
            ["AddCustom/d=0,p=1/0,2/0,2/attrs", "AddCustom/d=1,p=1/0,2/0,2/attrs"],
        )
        with self.assertRaises(ValueError):
            self.helper.level_simplified_keys(["AddCustom/d=0,p=1/x"], "1,2")

    def test_three_groups_metadata_does_not_change_key_bits(self):
        for parser in self.parsers:
            self.initialize(parser, self.group(0, False, "0"), self.group(0, True, "1,2"), self.group(1, True, "3"))
            for flag, selected, expected in (
                (False, "0", {0}),
                (True, "1,2", {0}),
                (True, "3", {1}),
                (False, None, {0}),
                (True, None, {0, 1}),
            ):
                self.flag, self.selected = flag, selected
                decoded = parser.decode_tiling()
                self.assertEqual(self.additions["declared_deterministic_level_groups"], [[0], [1, 2], [3]])
                self.assertEqual(set(decoded), expected)
                for key, metadata in decoded.items():
                    self.assertEqual(metadata["paramArgs"], [str(key)])
                    self.assertIn("deterministicLevels", metadata)

    def test_retired_mixed_group_declarations_are_rejected(self):
        for parser in self.parsers:
            for flag in (True, False, None):
                with self.subTest(parser=parser.__name__, flag=flag), self.assertRaises(ValueError):
                    self.initialize(parser, self.group(0, flag, "0,1,2"), self.group(1, None, "3"))

    def test_exact_request_boolean_conflicts_still_fail(self):
        for parser in self.parsers:
            self.initialize(parser, self.group(0, False, "0"), self.group(0, True, "1,2"), self.group(1, True, "3"))
            for flag, selected in ((True, "0"), (False, "1"), (False, "3"), (False, "0,1,2")):
                self.flag, self.selected = flag, selected
                with self.assertRaises(ValueError):
                    parser.decode_tiling()

    def test_level_zero_cannot_share_a_deterministic_binary(self):
        for helper in (self.helper, sys.modules["asc_op_compile_base.adapter.deterministic"]):
            for value in ("0,1,2", "2,0,1", "0,1", "0,3"):
                with self.subTest(value=value), self.assertRaises(ValueError):
                    helper.normalize_deterministic(None, value)
                with self.assertRaises(ValueError):
                    helper.level_simplified_keys(["AddCustom/d=1,p=1/x"], value)
            with self.assertRaises(ValueError):
                helper.normalize_deterministic("ignore", "3")

    def test_missing_metadata_completed_across_mixed_groups(self):
        for parser in self.parsers:
            self.initialize(parser, self.group(0, True), self.group(1, None, "3"))
            self.flag, self.selected = True, "3"
            self.assertEqual(set(parser.decode_tiling()), {1})
            self.selected = "1,2"
            decoded = parser.decode_tiling()
            self.assertEqual(set(decoded), {0})
            self.assertEqual(decoded[0]["deterministicLevels"], [1])
            self.initialize(parser, self.group(0), self.group(1, None, "3"))
            self.flag, self.selected = False, "0"
            self.assertEqual(parser.decode_tiling()[0]["deterministicLevels"], [0])

    def test_unfiltered_legacy_keeps_exact_dictionary_shape(self):
        for parser in self.parsers:
            self.initialize(parser, self.group(0, True), self.group(1, False))
            self.flag, self.selected = True, None
            decoded = parser.decode_tiling()
            self.assertEqual(set(decoded), {0, 1})
            self.assertTrue(all("deterministicLevels" not in item for item in decoded.values()))
            self.selected = "3"
            self.assertEqual(parser.decode_tiling(), {})

    def test_modern_offline_context_reaches_adapter_wrapper(self):
        parser = self.parsers[1]
        self.initialize(parser, self.group(0, False, "0"), self.group(1, True, "3"))
        self.flag, self.selected = True, "3"
        with patch.object(parser, "get_context", return_value=None), patch.object(
            parser, "get_current_build_config", return_value=False
        ):
            self.assertEqual(set(parser.decode_tiling()), {1})
        self.selected = None
        helper = sys.modules["asc_op_compile_base.adapter.deterministic"]
        self.assertEqual(helper.offline_selection(None, False), (None, False))

    def test_conflicting_and_duplicate_template_metadata_rejected(self):
        for parser in self.parsers:
            for flag, level in ((True, "0"), (False, "1"), (False, "2"), (False, "3")):
                with self.subTest(parser=parser.__name__, flag=flag, level=level), self.assertRaises(ValueError):
                    self.initialize(parser, self.group(0, flag, level))
            duplicate = self.group(0, True, "1")[:-1] + ",@@ASCENDC_TPL_DETERMINISTIC_LEVEL_SEL@@ = {2}}"
            with self.assertRaises(ValueError):
                self.initialize(parser, duplicate)

    def test_actual_support_info_producer_preserves_task_boolean(self):
        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        constants = load("support_info_test_constants", opc / "constant.py")
        tree = ast.parse((opc / "single_op_post_compile.py").read_text())
        cls = next(node for node in tree.body if isinstance(node, ast.ClassDef) and node.name == "SingleOpPostCompile")
        cls.body = [
            node for node in cls.body if isinstance(node, ast.FunctionDef) and node.name == "__generate_op_support_info"
        ]
        cls.bases = [ast.Name(id="Base", ctx=ast.Load())]
        ast.fix_missing_locations(cls)

        class Base:
            @staticmethod
            def gen_notnone_param(target, key, value):
                if value is not None:
                    target[key] = value

        namespace = {
            "Base": Base,
            "copy": copy,
            "OpcOptions": constants.OpcOptions,
            "CompileParam": constants.CompileParam,
            "SupportInfo": constants.SupportInfo,
            "logger": types.SimpleNamespace(info=lambda *_: None),
            "opc_log_full": lambda *_: None,
            "LogLevel": types.SimpleNamespace(DEBUG=0),
        }
        exec(
            compile(ast.Module(body=[cls], type_ignores=[]), str(opc / "single_op_post_compile.py"), "exec"), namespace
        )
        producer = namespace["SingleOpPostCompile"]()
        producer._opc_compile_args = {}
        producer._simplified_key_mode = None
        producer._SingleOpPostCompile__generate_static_key = lambda *_: "static"
        producer._SingleOpPostCompile__generate_simplified_key = lambda *_: []
        producer._SingleOpPostCompile__change_inoroutputs = lambda value: value
        producer.adjust_support_info_inputoutput = lambda *_: None
        raw = ["true"]
        producer.parse_info_from_compiled_json = lambda _: (raw[0], None, None, None)
        info = {"_deterministic_level_filter": (1, 2), "deterministic": "true"}
        generated = producer._SingleOpPostCompile__generate_op_support_info({}, info, "raw.json")
        self.assertEqual(generated["deterministic"], "true")
        self.assertEqual(generated["deterministicLevels"], [1, 2])
        self.assertEqual(info["deterministic"], "true")
        info["_deterministic_level_filter"] = (3,)
        self.assertEqual(
            producer._SingleOpPostCompile__generate_op_support_info({}, info, "raw.json")["deterministic"], "true"
        )
        info["_deterministic_level_filter"], raw[0] = (0,), "false"
        self.assertEqual(
            producer._SingleOpPostCompile__generate_op_support_info({}, info, "raw.json")["deterministic"], "false"
        )
        info["_deterministic_level_filter"] = (0, 1, 2)
        with self.assertRaises(ValueError):
            producer._SingleOpPostCompile__generate_op_support_info({}, info, "raw.json")

    def test_preprocess_discovery_returns_groups_before_inference_or_native(self):
        context = sys.modules["asc_op_compile_base.common.context"].get_context()
        self.additions["deterministic_discovery_only"] = True
        for parser, package in zip(self.parsers, ("asc_op_compiler", "adapter")):
            groups = ",".join([self.group(0, False, "0"), self.group(0, True, "1,2"), self.group(1, True, "3")])
            content = self.declaration() + "\n@@ASCENDC_TPL_LISTS@@ = {" + groups + "}\n"
            self.assertEqual(parser.discover_deterministic_groups(content), [[0], [1, 2], [3]])
            # The legacy domain is published from the declared selectors themselves.
            self.assertFalse(self.additions["declared_legacy_selector"])
            self.assertEqual(parser.discover_deterministic_groups("void old_kernel() {}"), [])
            path = BUILD / package / "kernel_info_infer.py"
            tree = ast.parse(path.read_text())
            method = next(
                node
                for node in ast.walk(tree)
                if isinstance(node, ast.FunctionDef) and node.name == "infer_info_from_ifile"
            )
            method.decorator_list = []
            namespace = {
                "__package__": "asc_op_compile_base." + package,
                "OpInfo": object,
                "CORE_TYPE_MIX": 0,
                "CommonUtility": types.SimpleNamespace(
                    is_v220=lambda: False, is_c310=lambda: False, is_920r1=lambda: False, is_920r2=lambda: False
                ),
                "global_var_storage": types.SimpleNamespace(get_variable=lambda _: False),
                "get_level_discovery_context": lambda: context,
                "discover_deterministic_groups": parser.discover_deterministic_groups,
            }
            exec(compile(ast.Module(body=[method], type_ignores=[]), str(path), "exec"), namespace)
            with tempfile.TemporaryDirectory() as directory:
                preprocessed = Path(directory) / "kernel.i"
                preprocessed.write_text(content)
                result = namespace["infer_info_from_ifile"](None, str(preprocessed), None, "kernel.cpp", "kernel")
                self.assertIsNone(result)
                self.assertTrue(self.additions["deterministic_discovery_complete"])
                self.assertEqual(self.additions["declared_deterministic_level_groups"], [[0], [1, 2], [3]])
                self.assertEqual(list(Path(directory).glob("*.o")), [])

    def test_discover_deterministic_groups_publishes_legacy_domain(self):
        groups = ",".join([self.group(0, False, "0"), self.group(0, True, "1,2"), self.group(1, True, "3")])
        content = self.declaration() + "\n@@ASCENDC_TPL_LISTS@@ = {" + groups + "}\n"
        legacy_content = self.declaration() + "\n@@ASCENDC_TPL_LISTS@@ = {" + self.group(0, legacy=True) + "}\n"
        self.additions["deterministic_discovery_only"] = True
        for parser in self.parsers:
            self.additions.pop("declared_legacy_selector", None)
            self.assertEqual(parser.discover_deterministic_groups(content), [[0], [1, 2], [3]])
            self.assertFalse(self.additions["declared_legacy_selector"])
            self.additions.pop("declared_legacy_selector", None)
            self.assertEqual(parser.discover_deterministic_groups(legacy_content), [])
            self.assertTrue(self.additions["declared_legacy_selector"])
            self.assertEqual(parser.discover_deterministic_groups("void old_kernel() {}"), [])

    def test_compile_boundary_skips_native_only_for_explicit_probe(self):
        for package in ("asc_op_compiler", "adapter"):
            path = BUILD / package / "compile_op.py"
            method = next(
                node
                for node in ast.parse(path.read_text()).body
                if isinstance(node, ast.FunctionDef) and node.name == "compile_op"
            )
            calls = []
            utility = types.SimpleNamespace(
                is_c310=lambda: False,
                is_920r1=lambda: False,
                is_920r2=lambda: False,
                is_m510=lambda: False,
                is_516r2=lambda: False,
                get_kernel_meta_dir=lambda: ".",
                get_distinct_filename_tag=lambda: "",
                print_compile_log=lambda *_: None,
            )
            namespace = {
                "__name__": "asc_op_compile_base." + package + ".compile_op",
                "__package__": "asc_op_compile_base." + package,
                "os": os,
                "OpInfo": object,
                "CommonUtility": utility,
                "CompileOptionTuple": lambda options, _: types.SimpleNamespace(compile_options=options),
                "LogUtil": types.SimpleNamespace(detail_log_print=lambda *_: None),
                "COMPILE_STAGE_MSG_INFO": {
                    key: key for key in ("compile_op_start", "preprocess_start", "preprocess_end")
                },
                "AscendCLogLevel": types.SimpleNamespace(LOG_INFO=1),
                "global_var_storage": types.SimpleNamespace(
                    global_storage_reset=lambda: None, get_variable=lambda _: False
                ),
                "_add_op_compile_options_by_customized_json": lambda *_: None,
                "compile_pre_process": lambda _, options: options,
                "DFXSectionGenerator": lambda: types.SimpleNamespace(dfx_info_reset=lambda _: None),
                "_update_compile_option": lambda *_: None,
                "_set_compile_info": lambda *_: None,
                "KernelInfoInfer": types.SimpleNamespace(
                    get_tiling_key_list_and_simple_infer_code_channel=lambda *_: None
                ),
                "get_context": sys.modules["asc_op_compile_base.common.context"].get_context,
                "compile_op_common_part": lambda *_: calls.append("native"),
            }
            exec(compile(ast.Module(body=[method], type_ignores=[]), str(path), "exec"), namespace)
            with tempfile.TemporaryDirectory() as directory:
                source = Path(directory) / "kernel.cpp"
                source.write_text("void kernel() {}")
                info = types.SimpleNamespace(kernel_name="AddCustom", impl_mode="")
                self.additions["deterministic_discovery_only"] = True
                namespace["compile_op"](str(source), "kernel", info)
                self.assertEqual(calls, [])
                self.additions["deterministic_discovery_only"] = False
                namespace["compile_op"](str(source), "kernel", info)
                self.assertEqual(calls, ["native"])

    def test_discovery_wrapper_paths_do_not_read_missing_output_json(self):
        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        constants = load("discovery_call_constants", opc / "constant.py")
        tree = ast.parse((opc / "single_op_compile.py").read_text())
        cls = next(node for node in tree.body if isinstance(node, ast.ClassDef) and node.name == "SingleOpCompile")
        cls.body = [
            node
            for node in cls.body
            if isinstance(node, ast.FunctionDef)
            and node.name in {"__call_op", "set_deterministic_context", "get_deterministic_discovery_result"}
        ]
        additions = {}
        context = types.SimpleNamespace(
            add_addition=lambda key, value: additions.update({key: value}),
            get_addition=lambda key: additions.get(key),
            add_op_info=lambda _: None,
        )

        class Scope:
            def __init__(self, mode):
                pass

            def __enter__(self):
                return context

            def __exit__(self, *args):
                pass

        def forbidden(*args):
            raise AssertionError("discovery must not access a compiled JSON")

        mode = ["false"]
        namespace = {
            "copy": copy,
            "OpcOptions": constants.OpcOptions,
            "CompileParam": constants.CompileParam,
            "OpModeType": constants.OpModeType,
            "LogLevel": types.SimpleNamespace(INFO=1),
            "logger": types.SimpleNamespace(info=lambda *_: None, debug=lambda *_: None),
            "operator_info": types.SimpleNamespace(OpInfo=lambda *_: types.SimpleNamespace()),
            "get_new_attrs_for_op_compile": lambda *_: [],
            "get_dynamic_compile_static_from_opstore": lambda _: mode[0],
            "infer_simplified_key_mode": lambda *_: [None] * 4,
            "op_context": types.SimpleNamespace(OpContext=Scope, get_context=lambda: context),
            "parse_levels": self.helper.parse_levels,
            "opc_log_full": lambda *_: None,
            "get_json_file_path": forbidden,
            "update_compile_info": forbidden,
        }
        exec(compile(ast.Module(body=[cls], type_ignores=[]), str(opc / "single_op_compile.py"), "exec"), namespace)
        wrapper = namespace["SingleOpCompile"]()
        wrapper._SingleOpCompile__get_kwargs = lambda: {}
        wrapper.get_op_info_for_context = lambda *_: None
        wrapper.set_extra_settings_for_context = lambda *_: None
        wrapper.set_kernel_spec_for_context = lambda *_: None
        wrapper._SingleOpCompile__op = {}
        wrapper._SingleOpCompile__opc_compile_args = {}

        def operator(*args, **kwargs):
            self.assertTrue(additions["deterministic_discovery_only"])
            self.assertEqual(additions["deterministic_level_filter"], (0,))
            self.assertTrue(additions["deterministic_declared_legacy"])
            additions.update(
                deterministic_discovery_complete=True, declared_deterministic_level_groups=[[0], [1, 2], [3]]
            )

        for dynamic, static_mode in ((True, "false"), (False, "true"), (False, "false")):
            additions.clear()
            mode[0] = static_mode
            wrapper._SingleOpCompile__op_info = {
                "_deterministic_discovery_only": True,
                "_deterministic_declared_legacy": True,
                "_deterministic_level_filter": (0,),
                constants.OpcOptions.DETERMINISTIC_LEVEL: "0",
                "op_func_attr": operator,
                "is_dynamic": dynamic,
                "inputs": [],
                "outputs": [],
                "op_type": "AddCustom",
            }
            self.assertEqual(wrapper._SingleOpCompile__call_op(), [[0], [1, 2], [3]])

    def test_offline_entry_expansion_and_legacy_dispatch(self):
        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        constants = load("deterministic_test_constants", opc / "constant.py")
        tree = ast.parse((opc / "op_compilation.py").read_text())
        cls = next(n for n in tree.body if isinstance(n, ast.ClassDef) and n.name == "OpCompilation")
        methods = {
            "__op_compile_by_json_info",
            "__auto_split_levels",
            "__single_op_compile_all_deterministic",
            "is_ascendc_op",
        }
        cls.body = [n for n in cls.body if isinstance(n, ast.FunctionDef) and n.name in methods]

        def update(key, value, path):
            data = json.loads(Path(path).read_text())
            data[key] = value
            Path(path).write_text(json.dumps(data))

        namespace = {
            "re": re,
            "copy": copy,
            "os": os,
            "shutil": shutil,
            "hashlib": hashlib,
            "json": json,
            "OpcOptions": constants.OpcOptions,
            "CompileParam": constants.CompileParam,
            "GraphDefParam": constants.GraphDefParam,
            "get_enable_vector_core_from_opstore": lambda _: False,
            "get_file_real_path": lambda output, name, suffix: str(Path(output) / (name + "." + suffix)),
            "read_json_file": lambda path: json.loads(Path(path).read_text()),
            "update_json_file": update,
            "logger": types.SimpleNamespace(
                info=lambda *_: None, warn=lambda *_: None, error=lambda *_: None, debug=lambda *_: None
            ),
        }
        exec(compile(ast.Module(body=[cls], type_ignores=[]), str(opc / "op_compilation.py"), "exec"), namespace)
        compilation = namespace["OpCompilation"]
        compilation.set_flag_before_compile = lambda *args: None
        compilation.set_l2_mode_flag = lambda *args: None
        compilation.get_l2_mode = lambda *args: "0"
        config = {"op_type": "AddCustom", "op_list": [{"bin_filename": "AddCustom_test"}]}
        for declared in (True, False):
            for requested in ("all", None, "true", "false"):
                with self.subTest(declared=declared, requested=requested), tempfile.TemporaryDirectory() as output:
                    native, probes = [], []
                    groups = [[0], [1, 2], [3]] if declared else []

                    def compile_device(compiler, op, info, idx):
                        name = op["bin_filename"]
                        info["kernel_name"] = name
                        if info.get("_deterministic_discovery_only"):
                            probes.append(name)
                            info["_deterministic_declares_legacy"] = declared
                            return groups
                        native.append((name, copy.deepcopy(info)))
                        data = {
                            "binFileName": name,
                            "supportInfo": {"deterministic": info.get("deterministic", "false")},
                        }
                        if info.get("_deterministic_declared_legacy") and requested is None:
                            data["supportInfo"]["deterministic"] = "ignore"
                        (Path(output) / (name + ".json")).write_text(json.dumps(data))
                        (Path(output) / (name + ".o")).write_bytes(b"device")

                    compilation._OpCompilation__single_op_compile = compile_device
                    compiler = compilation()
                    compiler._OpCompilation__compile_res = True
                    compiler._OpCompilation__is_ascendc = True
                    entry = Path(output) / "AddCustom.py"
                    entry.write_text("from ascendc import compile_op\n")
                    compiler._OpCompilation__opc_compile_args = {
                        "deterministic": requested,
                        "output": output,
                        "op_path": str(entry),
                    }
                    compiler._OpCompilation__op_compile_by_json_info(config)
                    self.assertTrue(compiler._OpCompilation__compile_res)
                    self.assertEqual(probes, ["AddCustom_test_level_discovery"])
                    if declared:
                        expected = ["0", "1,2", "3"]
                        self.assertEqual([info["deterministic_level"] for _, info in native[: len(expected)]], expected)
                        self.assertEqual(
                            [name for name, _ in native[: len(expected)]],
                            ["AddCustom_test_level" + level.replace(",", "_") for level in expected],
                        )
                        legacy_count = 1 if requested is None else 2
                        self.assertEqual(len(list(Path(output).glob("*.o"))), len(expected) + legacy_count)
                        self.assertTrue((Path(output) / "AddCustom_test.o").exists())
                        self.assertEqual((Path(output) / "AddCustom_test_deterministic.o").exists(), legacy_count == 2)
                        self.assertEqual(len(native), len(expected) + legacy_count)
                        for _, legacy_info in native[len(expected) :]:
                            self.assertNotIn("deterministic_level", legacy_info)
                            self.assertTrue(legacy_info["_deterministic_declared_legacy"])
                        # Splitting no longer writes legacy bookkeeping into the
                        # grade descriptors; each artifact only describes itself.
                        for name, _ in native[: len(expected)]:
                            descriptor = json.loads((Path(output) / (name + ".json")).read_text())
                            self.assertEqual(descriptor["binFileName"], name)
                            self.assertNotIn("legacyCompatibilityOutputs", descriptor)
                    else:
                        self.assertEqual(len(native), 2 if requested == "all" else 1)
                    self.assertEqual(config["op_list"][0]["bin_filename"], "AddCustom_test")

    def test_auto_split_validates_allowed_and_disjoint_groups_before_native(self):
        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        tree = ast.parse((opc / "op_compilation.py").read_text())
        cls = next(n for n in tree.body if isinstance(n, ast.ClassDef) and n.name == "OpCompilation")
        cls.body = [n for n in cls.body if isinstance(n, ast.FunctionDef) and n.name == "__auto_split_levels"]
        constants = load("auto_split_test_constants", opc / "constant.py")
        ns = {"OpcOptions": constants.OpcOptions, "copy": copy}
        exec(compile(ast.Module(body=[cls], type_ignores=[]), "auto_split", "exec"), ns)
        compiler = ns["OpCompilation"]()
        # The LEGACY_SEL opt-in stays mandatory; each group must be one of the
        # allowed partitions and the groups must not overlap.
        for groups, marker in [
            ([[0], [1, 2], [3]], False),
            ([[0, 1, 2], [3]], True),
            ([[1, 2], [1]], True),
            ([[0], [0]], True),
            ([[1], [1, 2], [3]], True),
        ]:
            with self.subTest(groups=groups, marker=marker), self.assertRaises(ValueError):
                compiler._OpCompilation__auto_split_levels({}, {}, 0, groups, marker)
        self.assertFalse(compiler._OpCompilation__auto_split_levels({}, {}, 0, [], False))

        # Single-level groups and any disjoint cover are accepted partitions.
        for groups in ([[1], [2], [3]], [[3]], [[0], [1]], [[0], [2], [3]], [[1, 2]]):
            with self.subTest(groups=groups):
                normalized = [tuple(group) for group in groups]
                compiled, legacy_batch = [], []

                def compile_device(op, info, idx):
                    compiled.append((op["bin_filename"], copy.deepcopy(info)))

                compiler._OpCompilation__single_op_compile = compile_device
                compiler._OpCompilation__single_op_compile_all_deterministic = lambda op, info, idx: (
                    legacy_batch.append(copy.deepcopy(info))
                )
                compiler._OpCompilation__compile_res = True
                self.assertTrue(
                    compiler._OpCompilation__auto_split_levels(
                        {"bin_filename": "Add"}, {"op_type": "Add"}, 0, groups, True
                    )
                )
                self.assertEqual(
                    [name for name, _ in compiled], ["Add_level" + "_".join(map(str, g)) for g in normalized]
                )
                for (name, info), group in zip(compiled, normalized):
                    self.assertEqual(info["deterministic_level"], ",".join(map(str, group)))
                    self.assertEqual(info["deterministic"], "false" if group == (0,) else "true")
                    self.assertEqual(info["_deterministic_level_filter"], group)
                self.assertEqual(len(legacy_batch), 1)
                self.assertTrue(legacy_batch[0]["_deterministic_declared_legacy"])
                self.assertNotIn("deterministic_level", legacy_batch[0])

    def test_loaded_pypto_implementation_skips_discovery_before_execution(self):
        import functools
        import inspect

        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        constants = load("loaded_pypto_constants", opc / "constant.py")
        path = opc / "op_compilation.py"
        tree = ast.parse(path.read_text())
        cls = next(node for node in tree.body if isinstance(node, ast.ClassDef) and node.name == "OpCompilation")
        methods = {
            "__init__",
            "__single_op_compile",
            "__op_compile_by_json_info",
            "__auto_split_levels",
            "is_ascendc_op",
        }
        cls.body = [node for node in cls.body if isinstance(node, ast.FunctionDef) and node.name in methods]
        calls, errors, active = [], [], {}

        class Parser:
            def __init__(self, op, info, options):
                self.op, self.info = op, info

            def get_op_info(self, op_type):
                self.info.update(inputs=[], outputs=[])

            def generate_kernel_name(self):
                return self.op["bin_filename"]

        class Compile:
            def __init__(self, op, info, options):
                self.info = info

            def op_compile(self):
                active["info"] = self.info
                self.info["op_func_attr"]()
                return [] if self.info.get("_deterministic_discovery_only") else "compiled.json"

        def pypto_compile_op():
            info = active["info"]
            if info.get("_deterministic_discovery_only"):
                raise AssertionError("PyPTO must not execute under discovery-only context")
            calls.append(info.get("deterministic"))

        namespace = {
            "copy": copy,
            "inspect": inspect,
            "re": re,
            "os": os,
            "OpInfoParser": Parser,
            "SingleOpCompile": Compile,
            "OpcOptions": constants.OpcOptions,
            "CompileParam": constants.CompileParam,
            "GraphDefParam": constants.GraphDefParam,
            "check_is_dynamic": lambda _: False,
            "get_enable_vector_core_from_opstore": lambda _: False,
            "logger": types.SimpleNamespace(info=lambda *_: None, debug=lambda *_: None, error=lambda *_: None),
        }
        exec(compile(ast.Module(body=[cls], type_ignores=[]), str(path), "exec"), namespace)
        compilation = namespace["OpCompilation"]
        for name in (
            "set_flag_before_compile",
            "set_l2_mode_flag",
            "_OpCompilation__set_int64_mode",
            "_OpCompilation__set_impl_mode",
            "_OpCompilation__del_compile_res_files",
            "_OpCompilation__single_op_post_compile",
        ):
            setattr(compilation, name, lambda *args: None)
        compilation.get_l2_mode = lambda *_: "0"
        compilation._OpCompilation__create_debug_dir = lambda *_: "."
        compilation.record_compile_error_info = lambda *args: errors.append(args)

        def compile_all(compiler, op, info, idx):
            for flag in ("true", "false"):
                legacy_info = copy.deepcopy(info)
                legacy_info["deterministic"] = flag
                compiler._OpCompilation__single_op_compile(op, legacy_info, idx)

        compilation._OpCompilation__single_op_compile_all_deterministic = compile_all
        config = {"op_type": "FlashAttnGrad", "op_list": [{"bin_filename": "FlashAttnGrad_test"}]}
        for from_store in (False, True):
            for depth in (0, 2):
                for requested in (None, "false", "true", "all"):
                    with self.subTest(
                        from_store=from_store, depth=depth, requested=requested
                    ), tempfile.TemporaryDirectory() as output:
                        # The inspected loader is not the implementation selected by registration.
                        entry = Path(output) / "loader.py"
                        entry.write_text("from ascendc import compile_op\n")
                        scope = {"pypto_compile_op": pypto_compile_op}
                        exec(
                            compile("def flash_attn_grad(): return pypto_compile_op()", "registered_impl.py", "exec"),
                            scope,
                        )
                        implementation = scope["flash_attn_grad"]
                        for _ in range(depth):

                            def decorate(func):
                                @functools.wraps(func)
                                def wrapper():
                                    return func()

                                return wrapper

                            implementation = decorate(implementation)
                        compilation.get_op_func_attr = lambda *args: implementation
                        compilation._OpCompilation__get_single_op_operator = staticmethod(lambda *args: implementation)
                        options = {"op_path": str(entry), "output": output, "deterministic": requested}
                        if not from_store:
                            options["main_func"] = "flash_attn_grad"
                        compiler = compilation(options)
                        calls.clear()
                        errors.clear()
                        compiler._OpCompilation__op_compile_by_json_info(config)
                        self.assertTrue(compiler._OpCompilation__compile_res, errors)
                        self.assertEqual(errors, [])
                        self.assertEqual(calls, ["true", "false"] if requested == "all" else [requested])
                        self.assertEqual(config["op_list"][0]["bin_filename"], "FlashAttnGrad_test")

        # Importing PyPTO in the same module must not disable a native wrapper.
        native_calls = []
        scope = {"pypto_compile_op": pypto_compile_op, "compile_op": lambda: native_calls.append("native")}
        exec("def native_op(): return compile_op()", scope)
        compiler = compilation({})
        result = compiler._OpCompilation__single_op_compile(
            {"bin_filename": "Native_test_level_discovery"},
            {"op_type": "Native", "op_func_attr": scope["native_op"], "_deterministic_discovery_only": True},
            0,
        )
        self.assertTrue(compiler._OpCompilation__compile_res, errors)
        self.assertEqual(result, [])
        self.assertEqual(native_calls, ["native"])

    def test_single_task_switch_cleans_dirty_options_without_touching_public_api(self):
        dirty = [
            "-O2",
            "-DASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL=1",
            "-UASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL",
            "-D",
            "ASCENDC_TPL_COMPILER_HAS_DETERMINISTIC_LEVEL=1",
            "-U",
            "ASCENDC_TPL_DISABLE_DETERMINISTIC_LEVEL",
            "-D ASCENDC_TPL_DISABLE_DETERMINISTIC_LEVEL=0",
            "-DASCENDC_TPL_HAS_DETERMINISTIC_LEVEL=1",
            "-DASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL_SIMILAR=1",
            "-DKEEP=7",
            "-DASCENDC_TPL_DETERMINISTIC_LEVEL_SEL(...)=external_override",
        ]
        expected = [
            "-O2",
            "-D",
            "ASCENDC_TPL_COMPILER_HAS_DETERMINISTIC_LEVEL=1",
            "-U",
            "ASCENDC_TPL_DISABLE_DETERMINISTIC_LEVEL",
            "-D ASCENDC_TPL_DISABLE_DETERMINISTIC_LEVEL=0",
            "-DASCENDC_TPL_HAS_DETERMINISTIC_LEVEL=1",
            "-DASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL_SIMILAR=1",
            "-DKEEP=7",
            "-DASCENDC_TPL_DETERMINISTIC_LEVEL_SEL(...)=external_override",
        ]
        original = list(dirty)
        for helper in (self.helper, sys.modules["asc_op_compile_base.adapter.deterministic"]):
            self.assertEqual(helper.deterministic_compile_options(dirty), expected)
            self.assertEqual(dirty, original)
            for levels in (None, "0", "3"):
                self.selected = levels
                self.assertEqual(helper.deterministic_compile_options(dirty), expected)
            self.selected = None

    def _single_switch_infer_fixture(self, family, probe_content):
        path = BUILD / family / "kernel_info_infer.py"
        tree = ast.parse(path.read_text())
        cls = next(node for node in tree.body if isinstance(node, ast.ClassDef) and node.name == "KernelInfoInfer")
        cls.body = [
            node
            for node in cls.body
            if isinstance(node, ast.FunctionDef)
            and node.name in {"_preprocess_source", "get_tiling_key_list_and_simple_infer_code_channel"}
        ]
        calls, parsed = [], []

        def command(source, destination, options, *rest):
            return dict(source=source, destination=destination, options=list(options.compile_options))

        def execute(spec, *unused):
            calls.append(spec)
            Path(spec["destination"]).write_text(probe_content)

        namespace = {
            "__package__": "asc_op_compile_base." + family,
            "os": os,
            "stat": __import__("stat"),
            "copy": copy,
            "OpInfo": object,
            "CompileOptionTuple": object,
            "CompileStage": types.SimpleNamespace(PRECOMPILE=0),
            "CommonUtility": types.SimpleNamespace(
                get_chip_version=lambda: "c310",
                is_v220=lambda: False,
                is_c310=lambda: False,
                is_920r1=lambda: False,
                is_920r2=lambda: False,
                is_m510=lambda: False,
                is_516r2=lambda: False,
                run_cmd_inner=execute,
            ),
            "gen_compile_cmd_v220": command,
            "_gen_compile_cmd": command,
        }
        exec(compile(ast.Module(body=[cls], type_ignores=[]), str(path), "exec"), namespace)
        infer = namespace["KernelInfoInfer"]

        def parse(*args):
            parsed.append(Path(args[1]).read_text())
            return parsed[-1]

        infer.infer_info_from_ifile = staticmethod(parse)
        return infer, calls, parsed

    def test_single_switch_injects_enable_only_for_level_tasks(self):
        enable = "-DASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL=1"
        dirty = ["-O2", enable, "-D", enable, "-DASCENDC_TPL_COMPILER_HAS_DETERMINISTIC_LEVEL=1"]
        for family in ("asc_op_compiler", "adapter"):
            for levels, discovery in [("0", False), ("3", False), (None, True), (None, False)]:
                with self.subTest(family=family, levels=levels, discovery=discovery), tempfile.TemporaryDirectory() as (
                    directory
                ):
                    self.selected = levels
                    self.additions.clear()
                    if discovery:
                        self.additions["deterministic_discovery_only"] = True
                    infer, calls, parsed = self._single_switch_infer_fixture(
                        family, "ordinary native preprocessed selectors"
                    )
                    options = types.SimpleNamespace(compile_options=list(dirty))
                    infer.get_tiling_key_list_and_simple_infer_code_channel(
                        None, "source.cpp", str(Path(directory) / "kernel.i"), options, None, "kernel"
                    )
                    expected = discovery or levels is not None
                    # The managed switch is cleaned first, then injected exactly
                    # once for grade tasks and never for boolean-only tasks.
                    self.assertEqual(options.compile_options.count(enable), 1 if expected else 0)
                    self.assertEqual(len(calls), 1)
                    self.assertEqual(calls[0]["options"].count(enable), 1 if expected else 0)
                    self.assertEqual(self.additions["deterministic_level_enabled"], expected)
                    self.assertIn("-E", calls[0]["options"])
                    self.assertIn("-DASCENDC_TPL_PRE", calls[0]["options"])
                    self.assertEqual(parsed, ["ordinary native preprocessed selectors"])
            self.selected = None
            self.additions.clear()

    def test_level_request_distinguishes_discovery_explicit_zero_and_boolean(self):
        for helper in (self.helper, sys.modules["asc_op_compile_base.adapter.deterministic"]):
            self.selected = None
            self.additions.clear()
            self.assertFalse(helper.deterministic_level_request()[1])
            self.additions["deterministic_discovery_only"] = True
            self.assertTrue(helper.deterministic_level_request()[1])
            self.additions.clear()
            for value in ("0", "1", "3"):
                self.selected = value
                self.assertEqual(helper.deterministic_level_request()[1:], (True, (int(value),)))
            self.selected = None
            self.additions["deterministic_declared_legacy"] = True
            self.assertTrue(helper.deterministic_level_request()[1])
            self.additions.clear()

    def test_adapter_single_switch_uses_explicit_modern_context(self):
        helper = sys.modules["asc_op_compile_base.adapter.deterministic"]
        empty_legacy_context = types.SimpleNamespace(get_addition=lambda _: None)
        self.selected = "0"
        self.assertEqual(helper.deterministic_level_request(empty_legacy_context)[1:], (True, (0,)))
        self.selected = "3"
        self.assertEqual(helper.deterministic_level_request(empty_legacy_context)[1:], (True, (3,)))
        self.selected = None
        self.additions["deterministic_discovery_only"] = True
        self.assertTrue(helper.deterministic_level_request(empty_legacy_context)[1])
        self.additions.clear()
        self.assertFalse(helper.deterministic_level_request(empty_legacy_context)[1])

    def test_tbe_only_discovery_resolves_and_publishes_real_infer_results(self):
        helper = sys.modules["asc_op_compile_base.adapter.deterministic"]
        additions = {"deterministic_discovery_only": True}
        context = types.SimpleNamespace(
            get_addition=lambda name: additions.get(name),
            add_addition=lambda name, value: additions.update({name: value}),
        )
        with patch.object(sys.modules["tbe.common.context"], "get_context", lambda: context), patch.object(
            sys.modules["asc_op_compile_base.common.context"], "get_context", lambda: None
        ):
            self.assertIs(helper.deterministic_request_context(), context)
            self.assertIs(helper.get_level_discovery_context(), context)
            self.assertTrue(helper.deterministic_level_request()[1])
            path = BUILD / "adapter/kernel_info_infer.py"
            method = next(
                node
                for node in ast.walk(ast.parse(path.read_text()))
                if isinstance(node, ast.FunctionDef) and node.name == "infer_info_from_ifile"
            )
            method.decorator_list = []
            namespace = {
                "__package__": "asc_op_compile_base.adapter",
                "OpInfo": object,
                "CORE_TYPE_MIX": 0,
                "CommonUtility": types.SimpleNamespace(
                    is_v220=lambda: False, is_c310=lambda: False, is_920r1=lambda: False, is_920r2=lambda: False
                ),
                "global_var_storage": types.SimpleNamespace(get_variable=lambda _: False),
                "get_level_discovery_context": helper.get_level_discovery_context,
                "discover_deterministic_groups": self.parsers[1].discover_deterministic_groups,
            }
            exec(compile(ast.Module(body=[method], type_ignores=[]), str(path), "exec"), namespace)
            groups = self.group(0, False, "0") + "," + self.group(0, True, "1,2") + "," + self.group(1, True, "3")
            with tempfile.TemporaryDirectory() as directory:
                preprocessed = Path(directory) / "native.i"
                preprocessed.write_text(self.declaration() + "\n@@ASCENDC_TPL_LISTS@@ = {" + groups + "}\n")
                self.assertIsNone(
                    namespace["infer_info_from_ifile"](None, str(preprocessed), None, "source.cpp", "kernel")
                )
            self.assertTrue(additions["deterministic_discovery_complete"])
            self.assertEqual(additions["declared_deterministic_level_groups"], [[0], [1, 2], [3]])

    def test_non_template_entries_skip_native_only_for_discovery_probe(self):
        for family in ("asc_op_compiler", "adapter"):
            for filename, function, arguments in [
                ("compile_op.py", "compile_op_with_customized_config", (None, None, None)),
                ("super_kernel_op_compile.py", "compile_super_kernel", ({}, None)),
            ]:
                path = BUILD / family / filename
                tree = ast.parse(path.read_text())
                entry = next(n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name == function)
                # Execute the real leading guard, stopping before unrelated
                # native/device setup. A discovery-only task returns before the
                # body; anything else must reach the sentinel.
                guard_end = (
                    next(
                        i
                        for i, n in enumerate(entry.body)
                        if isinstance(n, ast.If)
                        and any(isinstance(child, ast.Return) for child in n.body)
                        and "discovery" in ast.unparse(n.test)
                    )
                    + 1
                )
                entry.body = entry.body[:guard_end]
                entry.body.append(
                    ast.Raise(
                        exc=ast.Call(
                            func=ast.Name(id="AssertionError", ctx=ast.Load()),
                            args=[ast.Constant(value="legacy reached existing entry body")],
                            keywords=[],
                        ),
                        cause=None,
                    )
                )
                module = ast.fix_missing_locations(ast.Module(body=[entry], type_ignores=[]))
                namespace = {
                    "__package__": "asc_op_compile_base." + family,
                    "OpInfo": object,
                    "CustomizedConfig": object,
                }
                exec(compile(module, str(path), "exec"), namespace)
                for levels in ("0", "3", None):
                    self.selected = levels
                    self.additions.clear()
                    with self.assertRaisesRegex(AssertionError, "legacy reached existing entry body"):
                        namespace[function](*arguments)
                    self.assertNotIn("deterministic_discovery_complete", self.additions)
                self.selected = None
                self.additions.clear()
                self.additions["deterministic_discovery_only"] = True
                namespace[function](*arguments)
                self.assertTrue(self.additions["deterministic_discovery_complete"])
                self.additions.clear()

    def test_preprocess_and_device_compilation_share_capability_options(self):
        # Exercise the real production option preparation for both Python entry families.
        mapping = types.ModuleType("asc_op_compile_base.common.buildcfg.buildcfg_mapping")
        mapping.op_debug_config = "op_debug_config"
        sys.modules[mapping.__name__] = mapping
        for package in ("asc_op_compiler", "adapter"):
            path = BUILD / package / "ascendc_compile_base.py"
            tree = ast.parse(path.read_text())
            method = next(n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name == "compile_pre_process")
            if package == "adapter":
                mapping = types.ModuleType("tbe.common.buildcfg.buildcfg_mapping")
                mapping.op_debug_config = "op_debug_config"
                sys.modules[mapping.__name__] = mapping
            namespace = {
                "__package__": "asc_op_compile_base." + package,
                "OpInfo": object,
                "cce_runtime": types.SimpleNamespace(
                    **{
                        n: types.SimpleNamespace(local_list=[])
                        for n in (
                            "TBE_WORKSPACE_SIZE_LIST",
                            "TBE_WORKSPACE_IND_LIST",
                            "MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST",
                            "TBE_ATUO_ATOMIC_IND_LIST",
                        )
                    }
                ),
                "CommonUtility": types.SimpleNamespace(
                    get_ascendc_compiler_path=lambda: None, is_support_super_kernel=lambda: False
                ),
                "global_var_storage": types.SimpleNamespace(get_variable=lambda _: False, set_variable=lambda *_: None),
                "get_current_build_config": lambda name: [] if name == "op_debug_config" else False,
                "is_enable_sanitizer": lambda _: False,
                "is_enable_build_log": lambda: False,
                "is_enable_ascendc_cov": lambda: False,
                "global_ascendc_kernel_feature_manager": types.SimpleNamespace(
                    init_available_and_enable_features=lambda: None
                ),
            }
            exec(compile(ast.Module(body=[method], type_ignores=[]), str(path), "exec"), namespace)
            options = namespace["compile_pre_process"](None, [])
            self.assertNotIn("-DASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL=1", options)
            self.assertFalse(any("COMPILER_HAS" in option or "DISABLE_DETERMINISTIC" in option for option in options))
            # KernelInfoInfer deep-copies this exact option tuple for -E; the
            # original remains the device compile tuple, so both see the flags.
            infer_source = (BUILD / package / "kernel_info_infer.py").read_text()
            self.assertIn("copy.deepcopy(compile_option_tuple)", infer_source)

    def test_real_elf_metadata_generator_uses_each_compile_boolean(self):
        for package in ("asc_op_compiler", "adapter"):
            path = BUILD / package / "ascendc_compile_dfx.py"
            tree = ast.parse(path.read_text())
            method = next(
                node
                for node in ast.walk(tree)
                if isinstance(node, ast.FunctionDef) and node.name == "generate_meta_info_func_section"
            )
            # The production method also consults the global storage (e.g. for
            # compatibility mode on newer devkits); keep it inert for the test.
            namespace = {
                "get_current_build_config": lambda _: self.flag,
                "global_var_storage": types.SimpleNamespace(get_variable=lambda _: False),
            }
            exec(compile(ast.Module(body=[method], type_ignores=[]), str(path), "exec"), namespace)
            for flag in (False, True):
                self.flag = flag
                section = namespace["generate_meta_info_func_section"](None, "134228013", None, "AddCustom")
                self.assertIn("{{F_TYPE_DETERMINISTIC_INFO, 4}, " + str(int(flag)) + "}", section)
                self.assertIn('section (".ascend.meta.AddCustom")', section)

    def test_impl_mode_normalization_keeps_optional_only_for_same_task(self):
        import inspect

        opc = ROOT / "tools/build/asc_opc/python/asc_opc_tool"
        constants = load("optional_test_constants", opc / "constant.py")
        tree = ast.parse((opc / "op_compilation.py").read_text())
        cls = next(node for node in tree.body if isinstance(node, ast.ClassDef) and node.name == "OpCompilation")
        cls.body = [
            node
            for node in cls.body
            if isinstance(node, ast.FunctionDef) and node.name in {"__set_impl_mode", "__check_update_impl_mode"}
        ]
        logger = types.SimpleNamespace(info=lambda *_: None, debug=lambda *_: None)
        namespace = {
            "inspect": inspect,
            "OpcOptions": constants.OpcOptions,
            "OpImplType": constants.OpImplType,
            "CompileParam": constants.CompileParam,
            "logger": logger,
            "check_and_normalize_impl_mode": lambda mode: (True, mode),
            "normalize_optional_impl_mode": lambda mode: ",".join(
                item for item in mode.split(",") if item != "optional"
            ),
        }
        exec(compile(ast.Module(body=[cls], type_ignores=[]), str(opc / "op_compilation.py"), "exec"), namespace)
        compiler = namespace["OpCompilation"]()
        compiler._OpCompilation__opc_compile_args = {}
        compiler._OpCompilation__is_ascendc = True

        def operator(impl_mode="high_precision"):
            pass

        info = {"op_func_attr": operator, "impl_mode": "high_performance,optional"}
        compiler._OpCompilation__set_impl_mode(info)
        self.assertTrue(info["optional_impl_mode_flag"])
        self.assertEqual(info["impl_mode"], "high_performance")
        repeated = copy.deepcopy(info)
        compiler._OpCompilation__set_impl_mode(repeated)
        self.assertTrue(repeated["optional_impl_mode_flag"])
        repeated["impl_mode"] = "high_precision,high_performance"
        compiler._OpCompilation__set_impl_mode(repeated)
        self.assertFalse(repeated["optional_impl_mode_flag"])
        other = {"op_func_attr": operator, "impl_mode": "high_performance"}
        compiler._OpCompilation__set_impl_mode(other)
        self.assertFalse(other["optional_impl_mode_flag"])

    def test_native_host_encoder_ignores_metadata(self):
        compiler = shutil.which("clang++") or shutil.which("g++")
        if not compiler:
            self.skipTest("C++ compiler unavailable")
        source = r"""#define ASCENDC_DEBUG
#include "utils/tiling/template_argument.h"
ASCENDC_TPL_ARGS_DECL(AddCustom, ASCENDC_TPL_BOOL_DECL(B, 0, 1));
ASCENDC_TPL_SEL(ASCENDC_TPL_ARGS_SEL(ASCENDC_TPL_BOOL_SEL(B, 0, 1),
ASCENDC_TPL_DETERMINISTIC_SEL(true), ASCENDC_TPL_DETERMINISTIC_LEVEL_SEL(1,2)));
int main() { return GET_TPL_TILING_KEY(0) != 0 || GET_TPL_TILING_KEY(1) != 1; }
"""
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / "encoder.cpp"
            program.write_text(source)
            executable = Path(directory) / "encoder"
            subprocess.run(
                [
                    compiler,
                    "-std=c++17",
                    "-D_GLIBCXX_USE_CXX11_ABI=0",
                    "-I" + str(ROOT / "include"),
                    str(program),
                    str(ROOT / "impl/utils/tiling/template_argument.cpp"),
                    "-o",
                    str(executable),
                ],
                check=True,
                capture_output=True,
                text=True,
            )
            subprocess.run([str(executable)], check=True, capture_output=True, text=True)

    def test_framework_fallback_keys_keep_legacy_and_emit_canonical_dt(self):
        utility = load("deterministic_framework_insert", ROOT / "cmake/asc/fwk_modules/util/insert_simplified_keys.py")
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "bin.json"
            descriptor = {
                "binFileName": "AddCustom_FP32",
                "supportInfo": {
                    "staticKey": "old",
                    "deterministic": "true",
                    "implMode": "high_performance",
                    "deterministicLevels": [1, 2],
                    "inputs": [{"dtype": "float32", "format": "ND"}],
                },
            }
            path.write_text(json.dumps(descriptor, indent=2))
            utility.insert_simplified_keys(str(path))
            generated = json.loads(path.read_text())
            # The levels branch keeps the overflow field in canonical keys.
            self.assertEqual(
                generated["supportInfo"]["simplifiedKey"],
                ["AddCustom/d=1,p=1,o=0,dt=1/0,2/", "AddCustom/d=1,p=1,o=0,dt=2/0,2/"],
            )
            utility.insert_simplified_keys(str(path))
            self.assertEqual(json.loads(path.read_text()), generated)
            descriptor["supportInfo"]["deterministicLevels"] = [0, 1, 2]
            path.write_text(json.dumps(descriptor, indent=2))
            with self.assertRaises(ValueError):
                utility.insert_simplified_keys(str(path))
            descriptor["supportInfo"].pop("deterministicLevels")
            path.write_text(json.dumps(descriptor, indent=2))
            utility.insert_simplified_keys(str(path))
            self.assertEqual(json.loads(path.read_text())["supportInfo"]["simplifiedKey"], "AddCustom/d=1,p=1,o=0/0,2/")
            descriptor["supportInfo"]["deterministicLevels"] = [0]
            path.write_text(json.dumps(descriptor, indent=2))
            with self.assertRaises(ValueError):
                utility.insert_simplified_keys(str(path))

    def test_real_header_preprocessing(self):
        compiler = shutil.which("clang++") or shutil.which("g++")
        if not compiler:
            self.skipTest("C++ preprocessor unavailable")
        source = """#define ASCENDC_TPL_PRE
#include "template_argument.h"
ASCENDC_TPL_ARGS_DECL(AddCustom, ASCENDC_TPL_BOOL_DECL(B, 0, 1));
ASCENDC_TPL_SEL(ASCENDC_TPL_ARGS_SEL(ASCENDC_TPL_BOOL_SEL(B, 0),
ASCENDC_TPL_DETERMINISTIC_SEL(true), ASCENDC_TPL_DETERMINISTIC_LEVEL_SEL(1,2)));
"""
        result = subprocess.run(
            [compiler, "-std=c++17", "-E", "-P", "-x", "c++", "-I" + str(ROOT / "include/utils/tiling"), "-"],
            input=source,
            text=True,
            capture_output=True,
            check=True,
        )
        start = result.stdout.index("@@ASCENDC_TPL_ARGS_DECL_AddCustom@@")
        split = result.stdout.index("@@ASCENDC_TPL_LISTS@@", start)
        self.flag, self.selected = True, "1,2"
        for parser in self.parsers:
            parser.extract_template_tiling_info(result.stdout[start:split], result.stdout[split:])
            decoded = parser.decode_tiling()
            self.assertEqual(set(decoded), {0})
            self.assertEqual(decoded[0]["deterministicLevels"], [1, 2])
            self.assertEqual(decoded[0]["paramArgs"], ["0"])

    def test_legacy_sel_alone_opts_untagged_selectors_into_legacy_domain(self):
        # ASCENDC_TPL_LEGACY_SEL alone now owns the domain split; the retired
        # ASCENDC_TPL_LEGACY_DECL marker is neither required nor consulted.
        for parser in self.parsers:
            self.additions.clear()
            self.selected, self.flag = None, False
            self.initialize(parser, self.group(0, legacy=True))
            decoded = parser.decode_tiling()
            self.assertEqual(set(decoded), {0})
            self.assertTrue(all("LEGACY" not in value for value in decoded.values()))

    def test_dual_declarations_select_only_grade_domain(self):
        for parser in self.parsers:
            self.initialize(
                parser,
                self.group(0, False, "0"),
                self.group(0, True, "1,2"),
                self.group(1, True, "3"),
                self.group(0, legacy=True),
                self.group(1, legacy=True),
            )
            for levels, flag, key in [("0", False, 0), ("1,2", True, 0), ("3", True, 1)]:
                self.selected, self.flag = levels, flag
                decoded = parser.decode_tiling()
                self.assertEqual(set(decoded), {key})
                self.assertEqual(decoded[key]["deterministicLevels"], list(map(int, levels.split(","))))

    def test_dual_declarations_legacy_keeps_both_batch_instances(self):
        for parser in self.parsers:
            self.initialize(
                parser,
                self.group(0, False, "0"),
                self.group(0, True, "1,2"),
                self.group(1, True, "3"),
                self.group(0, legacy=True),
                self.group(1, legacy=True),
            )
            self.additions["deterministic_declared_legacy"] = True
            self.selected = None
            for self.flag in [False, True]:
                decoded = parser.decode_tiling()
                self.assertEqual(set(decoded), {0, 1})
                self.assertTrue(all("deterministicLevels" not in v for v in decoded.values()))
                self.assertEqual(self.additions["declared_deterministic_level_groups"], [])

    def test_declared_legacy_task_requires_explicit_legacy_selector(self):
        for parser in self.parsers:
            self.additions.clear()
            self.additions["deterministic_declared_legacy"] = True
            self.selected = None
            for self.flag in (False, True):
                self.initialize(parser, self.group(1, True, "3"))
                with self.assertRaisesRegex(RuntimeError, "requires an ASCENDC_TPL_LEGACY_SEL selector"):
                    parser.decode_tiling()

    def test_legacy_flag_only_in_outer_context_still_marks_legacy_only(self):
        # The outer asc_opc flow publishes deterministic_declared_legacy on the
        # modern context only; the adapter must resolve both layers, or a legacy
        # batch task keeps only the grade instances and the legacy binaries that
        # old consumers need go missing.
        parser = self.parsers[1]
        additions = {}
        outer = types.SimpleNamespace(
            get_addition=lambda name: True if name == "deterministic_declared_legacy" else additions.get(name),
            add_addition=lambda name, value: additions.update({name: value}),
        )
        idle = types.SimpleNamespace(get_addition=lambda _: None, add_addition=lambda *_: None)
        with patch.object(parser, "get_context", lambda: idle), patch.object(
            sys.modules["asc_op_compile_base.common.context"], "get_context", lambda: outer
        ):
            self.initialize(parser, self.group(0, False, "0"), self.group(1, legacy=True))
            decoded = parser.decode_tiling()
            self.assertEqual(set(decoded), {1})
            self.assertTrue(all("deterministicLevels" not in value for value in decoded.values()))
            self.assertEqual(additions["declared_deterministic_level_groups"], [])
            # Without a declared LEGACY_SEL selector the legacy task still fails.
            self.initialize(parser, self.group(1, True, "3"))
            with self.assertRaisesRegex(RuntimeError, "requires an ASCENDC_TPL_LEGACY_SEL selector"):
                parser.decode_tiling()

    def test_legacy_selector_payload_and_combination_rejected(self):
        for parser in self.parsers:
            for group in [
                self.group(0, False, "0", legacy=True),
                self.group(0, legacy=True).replace("= {true}", "= {true},@@ASCENDC_TPL_LEGACY_SEL@@ = {true}"),
            ]:
                with self.assertRaisesRegex(ValueError, "duplicate LEGACY_SEL or combine"):
                    self.initialize(parser, group)
            for payload in ["{}", "{false}", "{1}", "{true,false}", "{true,true}"]:
                with self.subTest(family=parser.__name__, payload=payload):
                    with self.assertRaisesRegex(ValueError, "must be"):
                        self.initialize(parser, self.group(0, legacy=True).replace("{true}", payload))

    def test_explicit_legacy_family_real_header(self):
        compiler = shutil.which("clang++") or shutil.which("g++")
        if not compiler:
            self.skipTest("C++ preprocessor unavailable")
        source = """#define ASCENDC_TPL_PRE
#include "template_argument.h"
ASCENDC_TPL_ARGS_DECL(AddCustom, ASCENDC_TPL_BOOL_DECL(B, 0, 1));
ASCENDC_TPL_SEL(
 ASCENDC_TPL_ARGS_SEL(ASCENDC_TPL_BOOL_SEL(B,0), ASCENDC_TPL_DETERMINISTIC_LEVEL_SEL(0)),
 ASCENDC_TPL_ARGS_SEL(ASCENDC_TPL_BOOL_SEL(B,0,1), ASCENDC_TPL_LEGACY_SEL()));
"""
        result = subprocess.run(
            [compiler, "-std=c++17", "-E", "-P", "-x", "c++", "-I" + str(ROOT / "include/utils/tiling"), "-"],
            input=source,
            text=True,
            capture_output=True,
            check=True,
        )
        self.assertIn("@@ASCENDC_TPL_LEGACY_SEL@@ = {true}", result.stdout)
        self.additions.update(deterministic_declared_legacy=True)
        self.selected = None
        declaration = next(line for line in result.stdout.splitlines() if "@@ASCENDC_TPL_ARGS_DECL" in line)
        selection = next(line for line in result.stdout.splitlines() if "@@ASCENDC_TPL_LISTS" in line)
        for parser in self.parsers:
            parser.extract_template_tiling_info(declaration, selection)
            decoded = parser.decode_tiling()
            self.assertEqual(set(decoded), {0, 1})
            self.assertEqual(decoded[0]["paramArgs"], ["0"])
            self.assertEqual(decoded[1]["paramArgs"], ["1"])
            self.assertTrue(all("LEGACY" not in value for value in decoded.values()))

    def test_declared_legacy_task_requests_real_header_probe(self):
        self.additions.update(deterministic_declared_legacy=True)
        _, requested, levels = self.helper.deterministic_level_request()
        self.assertTrue(requested)
        self.assertIsNone(levels)

    def test_complete_groups_without_dual_marker_keep_boolean_defaults(self):
        for parser in self.parsers:
            self.initialize(
                parser,
                self.group(0, False, "0"),
                self.group(1, True, "1,2"),
                self.group(0, True, "3"),
                self.group(0, True),
            )
            self.selected, self.flag = "1", True
            self.assertEqual(set(parser.decode_tiling()), {0, 1})

    def test_declared_legacy_task_rejects_simultaneous_grade_filter(self):
        self.additions["deterministic_declared_legacy"] = True
        self.selected, self.flag = "3", True
        for parser in self.parsers:
            self.initialize(parser, self.group(1, True, "3"))
            with self.assertRaisesRegex(RuntimeError, "cannot request"):
                parser.decode_tiling()

    def test_adapter_resolves_declared_legacy_context(self):
        helper = sys.modules["asc_op_compile_base.adapter.deterministic"]
        idle = types.SimpleNamespace(get_addition=lambda _: None)
        active = types.SimpleNamespace(
            get_addition=lambda name: True if name == "deterministic_declared_legacy" else None
        )
        with patch.object(sys.modules["tbe.common.context"], "get_context", lambda: idle), patch.object(
            sys.modules["asc_op_compile_base.common.context"], "get_context", lambda: active
        ):
            self.assertIs(helper.deterministic_request_context(), active)
            self.assertTrue(helper.deterministic_level_request()[1])


if __name__ == "__main__":
    unittest.main()
