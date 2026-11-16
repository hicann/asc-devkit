#!/usr/bin/python3
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

import ast
import importlib
import importlib.util
import sys
from contextlib import ExitStack
from pathlib import Path
from types import SimpleNamespace
from unittest import TestCase, mock

sys.path.insert(0, str(Path(__file__).resolve().parents[3] / "tools/build"))

compile_base = importlib.import_module("asc_op_compile_base.asc_op_compiler.ascendc_compile_base")
gen_json = importlib.import_module("asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_json")

compile_op = importlib.import_module("asc_op_compile_base.asc_op_compiler.compile_op")
v200 = importlib.import_module("asc_op_compile_base.asc_op_compiler.ascendc_compile_v200")
dcci = importlib.import_module("asc_op_compile_base.asc_op_compiler.super_kernel_sub_op_compile")


def _load_adapter_signature_functions():
    # This standalone compiler UT uses host dependencies, not the TBE runtime.
    # Execute the actual adapter helpers and wrappers without adapter/__init__.py.
    adapter_dir = Path(__file__).resolve().parents[3] / "tools/build/asc_op_compile_base/adapter"
    spec = importlib.util.spec_from_file_location(
        "asc_op_compile_base.asc_op_compiler._adapter_signature_test", adapter_dir / "kernel_signature.py"
    )
    signature = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(signature)
    module = SimpleNamespace(**vars(compile_op))
    module.collect_kernel_arguments = signature.collect_kernel_arguments
    module.render_super_kernel_signature = signature.render_super_kernel_signature
    source_path = adapter_dir / "compile_op.py"
    tree = ast.parse(source_path.read_text())
    names = {"_gen_kernel_func_declare_head", "_gen_kernel_func_declare_head_with_workspace"}
    functions = [node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name in names]
    assert {node.name for node in functions} == names
    exec(compile(ast.Module(body=functions, type_ignores=[]), str(source_path), "exec"), vars(module))
    return module


class TestCompileSourceRefactor(TestCase):
    def test_static_and_dynamic_cce_preserve_source_lines_and_dfx(self):
        for static in (True, False):
            with self.subTest(static=static), ExitStack() as stack:
                info = SimpleNamespace(
                    gen_kernel_func_file="generated.cpp",
                    dst_file="kernel.o",
                    kernel_name="kernel",
                    origin_func_name="origin",
                    get_kernel_func_name=lambda: "kernel",
                    compile_log_path="compile.log",
                    tiling_key_list=["3", "8"],
                    is_debug=False,
                    enable_deterministic=False,
                    tiling_key_deterministic={},
                )
                tiling = SimpleNamespace(
                    static_shape_flag=static, tiling_key=3, tiling_data_file_path="tiling.h", block_num=1
                )
                utility = compile_base.CommonUtility
                stack.enter_context(
                    mock.patch.object(utility, "ascendc_read_file", return_value=["body\n", "#endif\n"])
                )
                write = stack.enter_context(mock.patch.object(utility, "ascendc_write_file"))
                run = stack.enter_context(mock.patch.object(utility, "run_cmd_inner"))
                stack.enter_context(mock.patch.object(compile_base, "_gen_compile_cmd", return_value=["compiler"]))
                stack.enter_context(
                    mock.patch.object(compile_base.global_var_storage, "get_variable", return_value=False)
                )
                stack.enter_context(
                    mock.patch.object(
                        compile_base.DFXSectionGenerator(), "generate_dfx_section", return_value="static DFX\n"
                    )
                )
                stack.enter_context(
                    mock.patch.object(
                        compile_base,
                        "_compile_single_tiling",
                        side_effect=[(["compile 3"], "DFX 3\n"), (["compile 8"], "DFX 8\n")],
                    )
                )
                batch = stack.enter_context(mock.patch.object(compile_base, "compile_multi_tilingkey"))
                stack.enter_context(mock.patch.object(compile_base, "fatbin_objs"))
                stack.enter_context(mock.patch.object(compile_base, "tvm_callback_cce_postproc"))
                stack.enter_context(mock.patch.object(gen_json, "_dynamic_kernel_list_to_json"))
                add_config = stack.enter_context(mock.patch.object(compile_base, "add_op_system_run_cfg_option"))
                compile_base._compile_ascendc_cce(info, SimpleNamespace(), tiling)
                self.assertEqual(add_config.call_count, 1 if static else 2)
                expected = "static DFX\n" if static else "DFX 3\nDFX 8\n"
                self.assertEqual(write.call_args[0][0], "generated.cpp")
                self.assertEqual("".join(write.call_args[0][1]), "body\n" + expected + "#endif\n")
                self.assertEqual(run.call_count, int(static))
                self.assertEqual(batch.call_count, int(not static))
                if not static:
                    self.assertEqual(batch.call_args[0][1], [["compile 3"], ["compile 8"]])

    def test_v200_static_preserves_source_lines_and_final_macro(self):
        info = SimpleNamespace(
            gen_kernel_func_file="generated.cpp",
            dst_file="kernel.o",
            kernel_name="kernel",
            origin_func_name="origin",
            get_kernel_func_name=lambda: "kernel",
            compile_log_path="compile.log",
        )
        tiling = SimpleNamespace(tiling_key=3, tiling_data_file_path="tiling.h")
        with ExitStack() as stack:
            stack.enter_context(
                mock.patch.object(v200.CommonUtility, "ascendc_read_file", return_value=["body\n", "#endif\n"])
            )
            write = stack.enter_context(mock.patch.object(v200.CommonUtility, "ascendc_write_file"))
            run = stack.enter_context(mock.patch.object(v200.CommonUtility, "run_cmd_inner"))
            stack.enter_context(mock.patch.object(v200, "gen_compile_cmd_v200", return_value=["compiler"]))
            stack.enter_context(mock.patch.object(v200.global_var_storage, "get_variable", return_value=False))
            stack.enter_context(
                mock.patch.object(v200.DFXSectionGenerator(), "generate_dfx_section", return_value="DFX\n")
            )
            add_config = stack.enter_context(mock.patch.object(v200, "add_op_system_run_cfg_option"))
            v200.call_bisheng_v200_static(
                info, SimpleNamespace(), tiling, "dav-m200", v200.KernelMetaType.KERNEL_TYPE_AICORE
            )
            add_config.assert_called_once()
            self.assertEqual("".join(write.call_args[0][1]), "body\nDFX\n#endif\n")
            self.assertEqual(run.call_args[0][0][-1], "-D" + v200.TILING_KEY_MACRO + "=3UL")

    def test_sub_combine_retains_nonempty_disable_option_semantics(self):
        for disable, expected in (("", False), (" ", True), (",", True), ("Other", True)):
            with self.subTest(disable=disable), ExitStack() as stack:
                stack.enter_context(
                    mock.patch.object(dcci, "get_context", return_value=SimpleNamespace(get_addition=lambda _: True))
                )
                stack.enter_context(mock.patch.object(dcci.global_var_storage, "set_variable"))
                options = SimpleNamespace(compile_options=[])
                info = SimpleNamespace(
                    op_type="Add",
                    kernel_name="kernel",
                    super_kernel_info={"sp_options": {"dcci-disable-on-kernel": disable}},
                )
                dcci.gen_gm_get_set_value_dcci_compile_options(options, info, True)
                self.assertEqual(bool(options.compile_options), expected)

    def test_kernel_signatures_preserve_argument_and_dfx_order(self):
        names = ["ffts_addr", "ctx", "x", "y", "__ascendc_output_shape", "workspace", "tiling"]
        for mirror in ("adapter", "asc_op_compiler"):
            module = _load_adapter_signature_functions() if mirror == "adapter" else compile_op
            for mode in ("normal", "super", "combine"):
                with self.subTest(mirror=mirror, mode=mode), ExitStack() as stack:
                    for arch in ("is_c310", "is_m510", "is_v100", "is_v200", "is_920r1", "is_920r2", "is_516r2"):
                        stack.enter_context(mock.patch.object(module.CommonUtility, arch, return_value=False))
                    stack.enter_context(
                        mock.patch.object(module.global_var_storage, "get_variable", return_value=mode != "normal")
                    )
                    save_params = stack.enter_context(mock.patch.object(module.global_var_storage, "set_variable"))
                    stack.enter_context(
                        mock.patch.object(
                            module,
                            "get_context",
                            return_value=SimpleNamespace(get_addition=lambda _: mode == "combine"),
                        )
                    )
                    op = SimpleNamespace(
                        mc2_ctx=["ctx"],
                        inputs=[None, {"param_name": "x"}],
                        outputs=[{"param_name": "y"}],
                        output_shape_depend_on_compute=[0],
                        param_type_list=[],
                    )
                    tiling = SimpleNamespace(static_shape_flag=False, static_workspace_size=-1)
                    dfx = module.DFXSectionGenerator()
                    stack.enter_context(mock.patch.dict(dfx.__dict__))
                    dfx.dfx_info_reset(op)
                    source, index, called, types, metadata = module._gen_kernel_func_declare_head(
                        True, True, op, tiling
                    )
                    self.assertEqual(index, 5)
                    self.assertEqual(
                        [p.param_name for p in dfx.params_base],
                        ["ffts", "ctx", "x", "y", "shape_tensor", "workspace", "tiling"],
                    )
                    self.assertEqual(dfx.get_param("y").point_type, module.DFXPointType.LEVEL_1_FOR_SHAPE_TENSOR)
                    if mode == "normal":
                        self.assertEqual(source, ", ".join("GM_ADDR " + n for n in names) + ") {\n")
                        self.assertEqual(called, ", ".join(names))
                        self.assertEqual(types, ", ".join("GM_ADDR " + n for n in names))
                        save_params.assert_not_called()
                    else:
                        save_params.assert_called_once_with("ascendc_sub_super_kernel_params", names)
                        self.assertEqual(called, "param, sargs" if mode == "combine" else "args_offset")
                        expected = (
                            "__gm__ uint64_t *param, sk::SkSystemArgs *sargs"
                            if mode == "combine"
                            else "uint64_t args_offset"
                        )
                        self.assertEqual(types, expected)
                        self.assertTrue(source.startswith(expected + ") {\n"))
                        statements = [
                            f"    GM_ADDR {name} = (GM_ADDR)param[__asc_index++];\n"
                            if mode == "combine"
                            else f"    GM_ADDR {name} = param_base[args_offset++];\n"
                            for name in names
                        ]
                        self.assertTrue(source.endswith("".join(statements)))
