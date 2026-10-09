#!/usr/bin/python3
# coding=utf-8
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

"""Compile generated bindings to check specialization option semantics."""

import ast
import importlib.util
from itertools import product
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from types import SimpleNamespace


class TestSkBindOptions(unittest.TestCase):
    @unittest.skipUnless(shutil.which("g++"), "requires a host C++ preprocessor")
    def test_debug_checks_require_sk_sub_combine_context(self):
        root = Path(__file__).resolve().parents[3]
        for variant in ("asc_op_compiler", "adapter"):
            path = root / "tools/build/asc_op_compile_base" / variant / "compile_op.py"
            tree = ast.parse(path.read_text())
            function = next(
                node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name == "gen_kernel_fun"
            )
            # Exercise the actual wrapper call/check generation without importing
            # platform libraries or invoking a device compiler.
            start = next(
                i
                for i, node in enumerate(function.body)
                if isinstance(node, ast.Assign)
                and isinstance(node.targets[0], ast.Name)
                and node.targets[0].id == "need_ffts"
            )
            end = next(
                i
                for i in range(start, len(function.body))
                if isinstance(function.body[i], ast.If)
                and "tiling_key_struct_map" in ast.unparse(function.body[i].test)
            )
            snippet = compile(ast.Module(body=function.body[start:end], type_ignores=[]), str(path), "exec")
            for sk_enabled, sub_combine, option, macro in product(
                (False, True), (None, False, True), ("0", "1"), (False, True)
            ):
                with self.subTest(variant=variant, sk=sk_enabled, context=sub_combine, option=option, macro=macro):
                    context = None if sub_combine is None else SimpleNamespace(get_addition=lambda key: sub_combine)
                    env = {
                        "source": "",
                        "is_mix": False,
                        "is_single_and_using_hard_sync": False,
                        "get_context": lambda: context,
                        "global_var_storage": SimpleNamespace(get_variable=lambda key: sk_enabled),
                        "compile_info": SimpleNamespace(
                            super_kernel_info={"sp_options": {"debug-per-op-max-core-num": option}}
                        ),
                        "func_name": "kernel",
                        "opinfo": None,
                        "tiling_info": None,
                        "gen_usr_origin_kernel_function_call": lambda *a, **kw: "user_kernel();\n",
                    }
                    exec(snippet, env)
                    eligible = True
                    self.assertEqual("#ifdef __ASCENDC_SUPER_KERNEL_DEBUG__" in env["source"], eligible)
                    command = [shutil.which("g++"), "-E", "-P", "-x", "c++", "-"]
                    if macro:
                        command.insert(1, "-D__ASCENDC_SUPER_KERNEL_DEBUG__")
                    result = subprocess.run(
                        command, input=env["source"], text=True, capture_output=True, check=True
                    ).stdout
                    self.assertIn("user_kernel();", result)
                    self.assertEqual("g_superKernelSetWaitFlagCountDifference" in result, eligible and macro)
                    self.assertEqual("assert(false" in result, eligible and macro)

    @unittest.skipUnless(shutil.which("g++"), "requires a host C++ compiler")
    def test_dcci_option_only_adds_capability(self):
        root = Path(__file__).resolve().parents[3]
        for variant in ("asc_op_compiler", "adapter"):
            module_dir = root / "tools/build/asc_op_compile_base" / variant
            source_path = module_dir / "static_compile_resource_generator.py"
            tree = ast.parse(source_path.read_text())
            function = next(
                node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name == "gen_sk_bind_source"
            )
            module = SimpleNamespace()
            exec(compile(ast.Module(body=[function], type_ignores=[]), str(source_path), "exec"), module.__dict__)
            with self.subTest(variant=variant), tempfile.TemporaryDirectory() as directory:
                temp = Path(directory)
                # Match the real binding's integral template argument contract.
                (temp / "kernel_operator.h").write_text(
                    "#pragma once\n#include <cstdint>\n"
                    "template<auto GF, uint64_t cap, auto... SK> struct Binding {\n"
                    "  static constexpr uint64_t capability = cap;\n};\n"
                    "#define SK_BIND(...) "
                    "static_assert(Binding<__VA_ARGS__>::capability == EXPECTED)\n"
                )
                cases = []
                for override, gm_dcci, wait_flag, set_flag in product(
                    (None, "", 0, 1), (False, True), (False, True), (False, True)
                ):
                    cases.extend(
                        [
                            "#undef __ASCENDC_SUPER_KERNEL_DISABLE_DCCI__",
                            "#undef __ASCENDC_ENABLE_WAIT_PRE_TASK_END",
                            "#undef __ASCENDC_ENABLE_SET_NEXT_TASK_START",
                            "#undef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__",
                            "#undef EXPECTED",
                        ]
                    )
                    expected = 0
                    if override is not None:
                        cases.append(f"#define __ASCENDC_SUPER_KERNEL_DISABLE_DCCI__ {override}")
                        expected = 4
                    if wait_flag:
                        cases.append("#define __ASCENDC_ENABLE_WAIT_PRE_TASK_END")
                        expected |= 1
                    if set_flag:
                        cases.append("#define __ASCENDC_ENABLE_SET_NEXT_TASK_START")
                        expected |= 2
                    if gm_dcci:
                        cases.append("#define __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__")
                    cases.append(f"#define EXPECTED {expected}")
                    cases.append(
                        module.gen_sk_bind_source([("basic_a", "sk_a"), ("basic_b", "sk_b")], "", "").decode("utf-8")
                    )
                source = temp / "binding.cpp"
                source.write_text("\n".join(cases))
                result = subprocess.run(
                    [shutil.which("g++"), "-std=c++17", "-fsyntax-only", str(source)],
                    capture_output=True,
                    text=True,
                    check=False,
                )
                self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
