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

import json
import os
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest import mock


class _RecordingMode(Enum):
    EXECUTE_AND_RECORD = "execute_and_record"
    RECORD_ONLY = "record_only"


@dataclass(frozen=True)
class _KernelCompileCommand:
    tiling_key: str
    compiled_symbol: str
    core_type: str
    source_path: str
    argv: tuple
    output_path: str
    compile_tiling_key: str = None
    compile_symbol: str = None
    object_type: str = None
    constant_info_files: tuple = ()


@dataclass(frozen=True)
class _SkBindCommand:
    source_path: str
    argv: tuple
    output_path: str


class _CommandSession:
    def __init__(self, mode):
        self._mode = mode
        self._records = []
        self._sk_bind_records = []

    @property
    def records(self):
        return tuple(self._records)

    @property
    def sk_bind_records(self):
        return tuple(self._sk_bind_records)

    def submit(self, command):
        self._records.append(command)

    def submit_sk_bind(self, command):
        self._sk_bind_records.append(command)


MODULE_DIR = os.path.realpath(
    os.path.join(
        os.path.dirname(__file__),
        "../../../tools/build/asc_op_compile_base/asc_op_compiler",
    )
)
sys.path.insert(0, MODULE_DIR)
import static_compile_resource_generator as generator


class TestStaticCompileResourceGenerator(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_dir.cleanup)
        self.root = self.temp_dir.name
        self.input_root = os.path.join(self.root, "inputs")
        self.output_root = os.path.join(self.root, "output")
        self.output_dir = os.path.join(self.output_root, "add_custom")
        self.manifest_name = "add_custom_manifest.json"
        self.cann_root = os.path.join(self.root, "cann")
        os.makedirs(os.path.join(self.cann_root, "bin"))
        os.makedirs(os.path.join(self.cann_root, "include"))
        self.bisheng = os.path.join(self.cann_root, "bin", "bisheng")
        Path(self.bisheng).touch()
        self.env_patch = mock.patch.dict(
            os.environ, {"ASCEND_HOME_PATH": self.cann_root}, clear=False
        )
        self.env_patch.start()
        self.addCleanup(self.env_patch.stop)

        os.makedirs(self.input_root)
        self.source_path = os.path.join(self.input_root, "add_custom.cpp")
        Path(self.source_path).write_text(
            'extern "C" void add_custom() {}\n', encoding="utf-8"
        )
        self.wrapper_path = os.path.join(self.input_root, "add_custom_kernel.cpp")
        Path(self.wrapper_path).write_text(
            "#ifndef ADD_CUSTOM_KERNEL\n"
            "#define ADD_CUSTOM_KERNEL\n"
            f'#include "{self.source_path}"\n'
            'extern "C" void auto_gen_add_custom_kernel() { add_custom(); }\n'
            "#endif\n",
            encoding="utf-8",
        )
        self.sk_wrapper_path = os.path.join(self.input_root, "add_custom_sk_kernel.cpp")
        Path(self.sk_wrapper_path).write_text(
            "#ifndef ADD_CUSTOM_SK_KERNEL\n"
            "#define ADD_CUSTOM_SK_KERNEL\n"
            f'#include "{self.source_path}"\n'
            'extern "C" void auto_gen_add_custom_kernel() { add_custom(); }\n'
            "#endif\n",
            encoding="utf-8",
        )
        self.sk_bind_path = os.path.join(self.input_root, "sk_bind.cpp")
        Path(self.sk_bind_path).write_text(
            '#include "kernel_operator.h"\n', encoding="utf-8"
        )
        self.tiling_path = os.path.join(self.input_root, "add_custom_tiling_data.h")
        Path(self.tiling_path).write_text(
            "#ifndef ADD_CUSTOM_TILING_DATA_H\n"
            "#define ADD_CUSTOM_TILING_DATA_H\n"
            "#define GET_TILING_DATA(tiling_data, tiling_arg) int tiling_data = 0\n"
            "#endif\n",
            encoding="utf-8",
        )
        self.dynamic_object = os.path.join(self.input_root, "AddCustom.o")
        Path(self.dynamic_object).write_bytes(b"\x7fELFtest-object")
        self.metadata_path = os.path.join(self.input_root, "add_custom.json")
        Path(self.metadata_path).write_text(
            json.dumps({"kernelList": [{"kernelName": "add_custom_100000"}]}),
            encoding="utf-8",
        )
        self.resource_id = "a" * 64

    def make_compile_context(
        self,
        *,
        kernel_name="add_custom",
        code_channel=2,
        kernel_types=None,
        wrapper_path=None,
        symbols=None,
    ):
        return SimpleNamespace(
            kernel_name=kernel_name,
            origin_func_name="add_custom",
            code_channel=code_channel,
            hard_sync=False,
            no_set_kernel_type=kernel_types is None,
            tiling_key_list=["100000"],
            tiling_key_kernel_type=kernel_types or {},
            raw_tiling_key_kernel_type=kernel_types or {},
            tiling_key_group_map={},
            src_file=self.source_path,
            gen_kernel_func_file=wrapper_path or self.wrapper_path,
            dst_file=self.dynamic_object,
            global_kernel_symbols=list(symbols or []),
            global_kernel_attribute="__global__ [aicore]",
            max_tiling_size=0,
            compiled_tiling_key_data_size_map={},
            compile_command_session=_CommandSession(_RecordingMode.EXECUTE_AND_RECORD),
        )

    def make_recorded_commands(
        self, compile_info, options, constant_info_files, chip_version
    ):
        if compile_info.no_set_kernel_type:
            core_types = (
                ("cube", "vec")
                if compile_info.code_channel == 0
                else ("cube" if compile_info.code_channel == 1 else "vec",)
            )
        else:
            kernel_type = compile_info.tiling_key_kernel_type["100000"].name
            core_types = ("cube",) if "AIC_ONLY" in kernel_type else ("vec",)
        commands = []
        for index, core_type in enumerate(core_types):
            suffix = ""
            if len(core_types) > 1:
                suffix = "_mix_aic" if core_type == "cube" else "_mix_aiv"
            compiled_symbol = (
                compile_info.global_kernel_symbols[index]
                if compile_info.global_kernel_symbols
                else f"{compile_info.kernel_name}_100000{suffix}"
            )
            output_path = os.path.join(self.input_root, f"static_{core_type}.o")
            argv = [
                self.bisheng,
                "-c",
                "-O3",
                *options.compile_options,
                "-DCONST_TILING",
                compile_info.gen_kernel_func_file,
                f"--cce-aicore-arch=dav-{chip_version}-{core_type}",
                "--cce-aicore-only",
                f"-Dauto_gen_add_custom_kernel={compiled_symbol}",
                "-DTILING_KEY_VAR=100000UL",
                "-Dadd_custom=add_custom_100000_tilingkey",
                "-o",
                output_path,
            ]
            for constant_info_file in constant_info_files:
                argv.extend(["-include", constant_info_file])
            raw_kernel_type = compile_info.raw_tiling_key_kernel_type.get(
                "100000",
                compile_info.tiling_key_kernel_type.get("100000"),
            )
            if (
                chip_version == "c310"
                and getattr(raw_kernel_type, "name", "") == "KERNEL_TYPE_AIC_ONLY"
            ):
                argv.append("-DRAW_AIC_ONLY_DUMP_TENSOR")
            commands.append(
                _KernelCompileCommand(
                    tiling_key="100000",
                    compiled_symbol=compiled_symbol,
                    core_type=core_type,
                    source_path=compile_info.gen_kernel_func_file,
                    argv=tuple(argv),
                    output_path=output_path,
                )
            )
        return tuple(commands)

    def make_options(self, *, source_include_style=None):
        source_include = []
        if source_include_style == "joined":
            source_include = ["-I" + self.input_root]
        elif source_include_style == "split":
            source_include = ["-I", self.input_root]
        return SimpleNamespace(
            compile_options=[
                "-I" + os.path.join(self.cann_root, "include"),
                *source_include,
                "-I" + os.path.join(self.input_root, "missing_joined"),
                "-I",
                os.path.join(self.input_root, "missing_split"),
            ],
            mllvm_options=[],
        )

    def make_records(
        self,
        *,
        basic_context=None,
        basic_options=None,
        sk_context=None,
        sk_options=None,
        chip_version="c220",
        tiling_key_data_size=None,
        default_tiling_size=16,
        constant_infos=None,
        constant_info_size_by_tiling_key=None,
    ):
        constant_infos = constant_infos or (
            generator.ConstantInfoRecord(
                name="tiling_data",
                parameter_index=4,
                byte_size=default_tiling_size,
                template_path=self.tiling_path,
                template="@@STATIC_VALUE_tiling_data@@",
            ),
        )
        basic_context = basic_context or self.make_compile_context()
        basic_options = basic_options or self.make_options()
        constant_info_files = tuple(
            dict.fromkeys(item.template_path for item in constant_infos)
        )
        basic_context.compile_command_session = _CommandSession(
            _RecordingMode.EXECUTE_AND_RECORD
        )
        for command in self.make_recorded_commands(
            basic_context, basic_options, constant_info_files, chip_version
        ):
            basic_context.compile_command_session.submit(command)
        if sk_context is not None:
            sk_context.compile_command_session = _CommandSession(
                _RecordingMode.RECORD_ONLY
            )
            for command in self.make_recorded_commands(
                sk_context,
                sk_options or self.make_options(),
                constant_info_files,
                chip_version,
            ):
                sk_context.compile_command_session.submit(command)
        size_by_tiling_key = (
            {
                "tiling_data": (
                    {"100000": 8}
                    if tiling_key_data_size is None
                    else tiling_key_data_size
                )
            }
            if constant_info_size_by_tiling_key is None
            else constant_info_size_by_tiling_key
        )
        tiling_info = SimpleNamespace(
            default_tiling_size=default_tiling_size,
            tiling_key_data_size=(
                {"100000": 8} if tiling_key_data_size is None else tiling_key_data_size
            ),
            tiling_data_file_path=self.tiling_path,
        )
        self._last_snapshot = generator.ManifestInputSnapshot(
            soc_version="Ascend910B",
            kernel_spec_dir=self.output_root,
            resource_id=self.resource_id,
            compile_info=basic_context,
            basic_compile_info=basic_context,
            sk_compile_info=sk_context,
            tiling_info=tiling_info,
            workspace_idx=3,
            constant_infos=constant_infos,
            constant_info_size_by_tiling_key=size_by_tiling_key,
        )
        return generator._KernelCompileRecordBuilder(
            self._last_snapshot,
            constant_infos,
            size_by_tiling_key,
        ).build()

    def make_request(
        self,
        records,
        *,
        sk_cap_bitmap=None,
        sk_kernel_attribute="",
        basic_kernel_attribute="",
        sk_bind_compile_options=(),
        sk_bind_command=None,
    ):
        if sk_bind_command is None and sk_cap_bitmap is not None:
            sk_bind_output = os.path.join(self.input_root, "sk_bind.o")
            sk_bind_command = _SkBindCommand(
                source_path=self.sk_bind_path,
                argv=(
                    self.bisheng,
                    "-c",
                    "-O3",
                    *tuple(sk_bind_compile_options),
                    "-xcce",
                    self.sk_bind_path,
                    "--cce-aicore-arch=dav-c220-cube",
                    "--cce-aicore-only",
                    "-std=c++17",
                    "-DTILING_KEY_VAR=0",
                    "-o",
                    sk_bind_output,
                ),
                output_path=sk_bind_output,
            )
        snapshot = self._last_snapshot
        if snapshot.sk_compile_info is not None:
            snapshot.sk_compile_info.global_kernel_attribute = sk_kernel_attribute
            snapshot.basic_compile_info.global_kernel_attribute = basic_kernel_attribute
            if sk_bind_command is not None:
                snapshot.sk_compile_info.compile_command_session.submit_sk_bind(
                    sk_bind_command
                )
        return generator.ManifestInputSnapshot(
            soc_version=snapshot.soc_version,
            kernel_spec_dir=snapshot.kernel_spec_dir,
            resource_id=snapshot.resource_id,
            compile_info=snapshot.compile_info,
            basic_compile_info=snapshot.basic_compile_info,
            tiling_info=snapshot.tiling_info,
            workspace_idx=snapshot.workspace_idx,
            sk_compile_info=snapshot.sk_compile_info,
            sk_cap_bitmap=sk_cap_bitmap,
            constant_infos=snapshot.constant_infos,
            constant_info_size_by_tiling_key=(
                snapshot.constant_info_size_by_tiling_key
            ),
        )

    def generate(self, snapshot):
        return generator.ManifestPackageWriter(snapshot).write()

    def resource_logical_path(self, manifest, reference):
        prefix = "${resource}/" + manifest["resource_path"] + "/"
        self.assertTrue(reference.startswith(prefix))
        return reference[len(prefix) :]

    def test_normalize_source_include_options(self):
        replacement = "${source_file_path}"
        source_argument = self.wrapper_path
        missing_include = os.path.join(self.input_root, "missing")
        cases = (
            ("split", ["-I", self.input_root]),
            ("joined", ["-I" + self.input_root]),
            ("missing split", ["-I", missing_include]),
            ("missing joined", ["-I" + missing_include]),
        )
        for name, include_options in cases:
            with self.subTest(name=name):
                argv = [self.bisheng, *include_options, source_argument]
                self.assertEqual(
                    generator._normalize_source_include_options(
                        argv,
                        self.source_path,
                        replacement,
                        insertion_before=source_argument,
                    ),
                    [self.bisheng, "-I", replacement, source_argument],
                )

    def test_manifest_package_writer_writes_snapshot(self):
        compile_info = self.make_compile_context()
        compile_info.dst_file = os.path.join(self.input_root, "add_custom.o")
        compile_info.src_file = self.source_path
        compile_info.max_tiling_size = 32
        compile_info.compiled_tiling_key_data_size_map = {"100000": 24}
        Path(compile_info.dst_file).write_bytes(b"\x7fELFtest-object")
        metadata_path = os.path.join(self.input_root, "add_custom.json")
        Path(metadata_path).write_text(
            json.dumps({"kernelList": [{"kernelName": "add_custom_100000"}]}),
            encoding="utf-8",
        )
        options = self.make_options()
        for command in self.make_recorded_commands(
            compile_info, options, (self.tiling_path,), "c220"
        ):
            compile_info.compile_command_session.submit(command)
        tiling_info = SimpleNamespace(
            default_tiling_size=0,
            tiling_key_data_size={"100000": 16},
            tiling_data_file_path=self.tiling_path,
        )
        snapshot = generator.ManifestInputSnapshot(
            soc_version="Ascend910B",
            kernel_spec_dir=self.output_root,
            resource_id=self.resource_id,
            compile_info=compile_info,
            basic_compile_info=compile_info,
            tiling_info=tiling_info,
            workspace_idx=3,
        )

        manifest_path = generator.ManifestPackageWriter(snapshot).write()

        self.assertEqual(
            manifest_path,
            os.path.join(self.output_root, "add_custom", self.manifest_name),
        )
        manifest = json.loads(Path(manifest_path).read_text(encoding="utf-8"))
        self.assertEqual(manifest["resource_id"], self.resource_id)
        self.assertEqual(manifest["kernels"][0]["kernel_name"], "add_custom_100000")
        self.assertEqual(manifest["kernels"][0]["constant_infos"][0]["byte_size"], 24)
        self.assertTrue(os.path.isdir(os.path.join(self.output_dir, "resources")))

    def test_basic_mix_manifest_uses_static_wrapper_and_tiling_template(self):
        records = self.make_records(
            basic_context=self.make_compile_context(code_channel=0),
        )
        Path(self.wrapper_path).write_text(
            Path(self.wrapper_path).read_text(encoding="utf-8")
            + "// wrapper content read during Manifest generation\n",
            encoding="utf-8",
        )
        result = self.generate(self.make_request(records))

        manifest = json.loads(Path(result).read_text(encoding="utf-8"))
        self.assertEqual(
            set(manifest),
            {
                "soc_version",
                "resource_id",
                "source_file",
                "resource_path",
                "options",
                "kernels",
            },
        )
        self.assertEqual(manifest["source_file"], "add_custom.cpp")
        self.assertEqual(manifest["resource_path"], "resources")
        self.assertEqual(manifest["options"]["common_compile"][:2], ["-c", "-O3"])

        kernel = manifest["kernels"][0]
        self.assertEqual(
            kernel["link_options"],
            ["-m", "aicorelinux", "-Ttext=0", "-q", "-x"],
        )
        constant_info = kernel["constant_infos"][0]
        self.assertEqual(constant_info["template"], "@@STATIC_VALUE_tiling_data@@")
        self.assertEqual(
            constant_info["file"],
            "${resource}/resources/include/add_custom_tiling_data.h",
        )
        template_path = os.path.join(
            self.output_dir,
            "resources",
            self.resource_logical_path(manifest, constant_info["file"]),
        )
        template_text = Path(template_path).read_text(encoding="utf-8")
        self.assertEqual(template_text.count("@@STATIC_VALUE_tiling_data@@"), 1)

        basic = kernel["objects"][0]
        self.assertEqual(basic["object_name"], "basic")
        self.assertEqual(basic["object_type"], "basic")
        self.assertEqual(
            basic["outputs"],
            [
                "${output}/add_custom_100000.aic.o",
                "${output}/add_custom_100000.aiv.o",
            ],
        )
        serialized = json.dumps(manifest)
        self.assertNotIn("intermediates", serialized)
        for old_name in (
            "static_inputs",
            "token",
            "obj_name",
            "obj_type",
            "option",
            "link_option",
        ):
            self.assertNotIn(f'"{old_name}"', serialized)
        self.assertNotIn(self.root, serialized)
        resource_root = os.path.join(self.output_dir, "resources")
        self.assertEqual(
            {
                os.path.relpath(path, resource_root)
                for path in Path(resource_root).rglob("*")
                if path.is_file()
            },
            {
                "src/add_custom_kernel.cpp",
                "include/add_custom_tiling_data.h",
            },
        )
        self.assertFalse(
            os.path.exists(os.path.join(resource_root, "src", "add_custom.cpp"))
        )

        for command in basic["commands"]:
            self.assertEqual(command["type"], "compile")
            self.assertNotIn("stage", command)
            expanded_cmd = list(command["cmd"])
            option_index = expanded_cmd.index("${options:common_compile}")
            expanded_cmd[option_index : option_index + 1] = manifest["options"][
                "common_compile"
            ]
            self.assertIn("-DCONST_TILING", expanded_cmd)
            self.assertTrue(
                any(
                    expanded_cmd[index : index + 2] == ["-I", "${source_file_path}"]
                    for index in range(len(expanded_cmd) - 1)
                )
            )
            self.assertNotIn("-I${source_file_path}", expanded_cmd)
            self.assertLess(
                expanded_cmd.index("${source_file_path}"),
                next(
                    index
                    for index, value in enumerate(expanded_cmd)
                    if value.startswith("${resource}/resources/src/")
                ),
            )
            self.assertFalse(
                any(
                    "missing_joined" in value or "missing_split" in value
                    for value in expanded_cmd
                )
            )
            include_index = expanded_cmd.index("-include")
            self.assertEqual(
                expanded_cmd[include_index + 1],
                constant_info["file"],
            )

        wrapper_reference = next(
            arg
            for arg in basic["commands"][0]["cmd"]
            if arg.startswith("${resource}/resources/src/")
        )
        wrapper_path = os.path.join(
            self.output_dir,
            "resources",
            self.resource_logical_path(manifest, wrapper_reference),
        )
        wrapper_text = Path(wrapper_path).read_text(encoding="utf-8")
        self.assertNotIn("@@STATIC_VALUE_tiling_data@@", wrapper_text)
        self.assertNotIn(self.source_path, wrapper_text)
        self.assertIn('#include "add_custom.cpp"', wrapper_text)
        self.assertIn("wrapper content read during Manifest generation", wrapper_text)

    def test_static_tiling_template_overrides_pointer_access(self):
        dynamic_header = (
            "#ifndef ADD_CUSTOM_TILING_DATA_H\n"
            "#define ADD_CUSTOM_TILING_DATA_H\n"
            "#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr) "
            "tiling_struct *dst_ptr = (tiling_struct *)tiling_ptr\n"
            "#endif\n"
        )

        template_text = generator.static_tiling_template_text(dynamic_header)

        self.assertIn("#undef GET_TILING_DATA_PTR_WITH_STRUCT", template_text)
        static_definition = template_text[
            template_text.rfind("#define GET_TILING_DATA_PTR_WITH_STRUCT") :
        ]
        self.assertIn("__ascendc_static_tiling_data", static_definition)
        self.assertIn("__ascendc_static_full_##dst_ptr", static_definition)
        self.assertNotIn("(tiling_struct *)tiling_ptr", static_definition)

    def test_static_tiling_template_ignores_generic_class_name(self):
        dynamic_header = (
            "InitTilingData<T>();\nInitTilingData<ValidTiling>();\n#endif\n"
        )

        template_text = generator.static_tiling_template_text(dynamic_header)

        self.assertIn("REGISTER_TILINGDATA_SIZE(ValidTiling", template_text)
        self.assertNotIn("REGISTER_TILINGDATA_SIZE(T,", template_text)

    def test_multiple_constant_infos_share_one_template(self):
        block_dim_template = "@@STATIC_VALUE_block_dim@@"
        tiling_header = Path(self.tiling_path).read_text(encoding="utf-8")
        Path(self.tiling_path).write_text(
            tiling_header.replace(
                "#endif\n",
                "#ifdef CONST_TILING\n"
                "static constexpr uint32_t __ascendc_static_block_dim = "
                f"{block_dim_template};\n"
                "#endif\n"
                "#endif\n",
            ),
            encoding="utf-8",
        )
        constant_infos = (
            generator.ConstantInfoRecord(
                name="tiling_data",
                parameter_index=4,
                byte_size=8,
                template_path=self.tiling_path,
                template="@@STATIC_VALUE_tiling_data@@",
            ),
            generator.ConstantInfoRecord(
                name="block_dim",
                parameter_index=5,
                byte_size=4,
                template_path=self.tiling_path,
                template=block_dim_template,
            ),
        )
        records = self.make_records(
            constant_infos=constant_infos,
            constant_info_size_by_tiling_key={
                "tiling_data": {"100000": 8},
                "block_dim": {"100000": 4},
            },
        )

        result = self.generate(self.make_request(records))

        manifest = json.loads(Path(result).read_text(encoding="utf-8"))
        kernel = manifest["kernels"][0]
        self.assertEqual(
            kernel["constant_infos"],
            [
                {
                    "name": "tiling_data",
                    "parameter_index": 4,
                    "byte_size": 8,
                    "file": "${resource}/resources/include/add_custom_tiling_data.h",
                    "template": "@@STATIC_VALUE_tiling_data@@",
                },
                {
                    "name": "block_dim",
                    "parameter_index": 5,
                    "byte_size": 4,
                    "file": "${resource}/resources/include/add_custom_tiling_data.h",
                    "template": block_dim_template,
                },
            ],
        )
        template_path = os.path.join(
            self.output_dir,
            "resources",
            "include",
            "add_custom_tiling_data.h",
        )
        template_text = Path(template_path).read_text(encoding="utf-8")
        for constant_info in kernel["constant_infos"]:
            self.assertEqual(template_text.count(constant_info["template"]), 1)
        for command in kernel["objects"][0]["commands"]:
            expanded_cmd = list(command["cmd"])
            option_index = expanded_cmd.index("${options:common_compile}")
            expanded_cmd[option_index : option_index + 1] = manifest["options"][
                "common_compile"
            ]
            self.assertEqual(
                [
                    expanded_cmd[index + 1]
                    for index, value in enumerate(expanded_cmd[:-1])
                    if value == "-include"
                ].count(kernel["constant_infos"][0]["file"]),
                1,
            )

    def test_unused_grouped_tiling_keys_do_not_block_records(self):
        compile_context = self.make_compile_context()
        compile_context.tiling_key_group_map = {"100000": ["100001"]}

        records = self.make_records(basic_context=compile_context)
        self.assertEqual(len(records), 1)
        self.assertEqual(records[0].tiling_key, "100000")

    def test_missing_exact_tiling_size_uses_default_size(self):
        records = self.make_records(
            default_tiling_size=16,
            tiling_key_data_size={"200000": 8},
        )

        self.assertEqual(records[0].constant_infos[0].byte_size, 16)

    def test_grouped_tiling_key_reuses_master_compile_identity(self):
        Path(self.metadata_path).write_text(
            json.dumps(
                {
                    "kernelList": [
                        {
                            "kernelName": "add_custom_100000",
                        },
                        {
                            "kernelName": "add_custom_100001",
                        },
                    ]
                }
            ),
            encoding="utf-8",
        )
        compile_context = self.make_compile_context(code_channel=0)
        compile_context.tiling_key_group_map = {"100000": ["100001"]}

        records = self.make_records(
            basic_context=compile_context,
            tiling_key_data_size={"100000": 8, "100001": 12},
        )

        self.assertEqual(
            [(record.tiling_key, record.compile_tiling_key) for record in records],
            [("100000", "100000"), ("100001", "100000")],
        )
        self.assertEqual(
            [record.constant_infos[0].byte_size for record in records],
            [8, 12],
        )
        master_commands, slave_commands = (
            records[0].commands,
            records[1].commands,
        )
        self.assertEqual(
            [command.compile_symbol for command in slave_commands],
            [command.compiled_symbol for command in master_commands],
        )
        self.assertEqual(
            [command.compiled_symbol for command in slave_commands],
            [
                "add_custom_100001_mix_aic",
                "add_custom_100001_mix_aiv",
            ],
        )
        for command in slave_commands:
            self.assertIn("-DTILING_KEY_VAR=100000UL", command.argv)
            self.assertIn(
                f"-Dauto_gen_add_custom_kernel={command.compile_symbol}",
                command.argv,
            )
            self.assertIn(
                "-Dadd_custom=add_custom_100000_tilingkey",
                command.argv,
            )

        result = self.generate(self.make_request(records))
        manifest = json.loads(Path(result).read_text(encoding="utf-8"))
        self.assertEqual(
            [kernel["kernel_name"] for kernel in manifest["kernels"]],
            ["add_custom_100000", "add_custom_100001"],
        )
        slave_basic = manifest["kernels"][1]["objects"][0]
        self.assertEqual(
            slave_basic["outputs"],
            [
                "${output}/add_custom_100001.aic.o",
                "${output}/add_custom_100001.aiv.o",
            ],
        )
        for command in slave_basic["commands"]:
            self.assertIn("-DTILING_KEY_VAR=100000UL", command["cmd"])
            self.assertTrue(
                any(
                    value.startswith(
                        "-Dauto_gen_add_custom_kernel=add_custom_100000_mix_ai"
                    )
                    for value in command["cmd"]
                )
            )

        Path(self.metadata_path).write_text(
            json.dumps(
                {
                    "kernelList": [
                        {
                            "kernelName": "add_custom_100001",
                        }
                    ]
                }
            ),
            encoding="utf-8",
        )
        exact_slave_records = self.make_records(
            basic_context=compile_context,
            tiling_key_data_size={"100001": 12},
        )
        self.assertEqual(exact_slave_records[0].constant_infos[0].byte_size, 12)

        c310_records = self.make_records(
            basic_context=compile_context,
            chip_version="c310",
            tiling_key_data_size={"100000": 8, "100001": 12},
        )
        c310_slave = c310_records[0]
        self.assertEqual(c310_slave.compile_tiling_key, "100000")
        self.assertTrue(
            all(
                "--cce-aicore-arch=dav-c310-" in " ".join(command.argv)
                for command in c310_slave.commands
            )
        )

    def test_logical_symbol_rewrites_only_trailing_tiling_key(self):
        cases = (
            (
                "op100000_add_100000",
                "op100000_add_100001",
            ),
            (
                "op100000_add_100000_mix_aic",
                "op100000_add_100001_mix_aic",
            ),
            (
                "op100000_add_100000_mix_aiv",
                "op100000_add_100001_mix_aiv",
            ),
        )
        for compile_symbol, expected in cases:
            with self.subTest(compile_symbol=compile_symbol):
                self.assertEqual(
                    generator._logical_symbol(
                        compile_symbol,
                        "100000",
                        "100001",
                    ),
                    expected,
                )

        with self.assertRaisesRegex(
            generator.ManifestCommandError,
            "does not end with master tiling key 100000",
        ):
            generator._logical_symbol(
                "op100000_add_200000_mix_aic",
                "100000",
                "100001",
            )

    def test_grouped_tiling_key_rejects_ambiguous_or_missing_master(self):
        first_context = self.make_compile_context()
        first_context.tiling_key_group_map = {"100000": ["100001"]}
        second_context = self.make_compile_context()
        second_context.tiling_key_group_map = {"200000": ["100001"]}
        with self.assertRaisesRegex(
            generator.ManifestCommandError,
            "tiling key 100001 belongs to multiple masters",
        ):
            generator.build_tiling_key_group_index(
                first_context,
                second_context,
            )

        Path(self.metadata_path).write_text(
            json.dumps({"kernelList": [{"kernelName": "add_custom_100001"}]}),
            encoding="utf-8",
        )
        first_context.tiling_key_group_map = {"200000": ["100001"]}
        with self.assertRaisesRegex(
            generator.ManifestCommandError,
            "master tiling key 200000 has no physical compile command",
        ):
            self.make_records(basic_context=first_context)

        Path(self.metadata_path).write_text(
            json.dumps({"kernelList": [{"kernelName": "add_custom_unknown"}]}),
            encoding="utf-8",
        )
        with self.assertRaisesRegex(
            generator.ManifestCommandError,
            "cannot determine tiling key for kernel add_custom_unknown",
        ):
            self.make_records()

    def test_grouped_tiling_key_sk_renames_slave_symbol_after_master_compile(self):
        Path(self.metadata_path).write_text(
            json.dumps(
                {
                    "kernelList": [
                        {"kernelName": "add_custom_100001"},
                    ]
                }
            ),
            encoding="utf-8",
        )
        basic_context = self.make_compile_context(
            code_channel=0,
            symbols=[
                "add_custom_100000_mix_aic",
                "add_custom_100000_mix_aiv",
            ],
        )
        sk_context = self.make_compile_context(
            kernel_name="add_custom_sk",
            code_channel=0,
            wrapper_path=self.sk_wrapper_path,
            symbols=[
                "add_custom_sk_100000_mix_aic",
                "add_custom_sk_100000_mix_aiv",
            ],
        )
        basic_context.tiling_key_group_map = {"100000": ["100001"]}
        sk_context.tiling_key_group_map = {"100000": ["100001"]}
        Path(self.sk_wrapper_path).write_text(
            "#ifndef ADD_CUSTOM_SK_KERNEL\n"
            "#define ADD_CUSTOM_SK_KERNEL\n"
            f'#include "{self.source_path}"\n'
            "namespace sk { class SkSystemArgs; }\n"
            "#define __gm__\n"
            'extern "C" void auto_gen_add_custom_kernel(\n'
            "    __gm__ uint64_t *param, sk::SkSystemArgs *sargs) {}\n"
            'extern "C" __sk__ void add_custom_sk_100001_mix_aic(\n'
            "    uint64_t args_offset) {\n"
            "    auto_gen_add_custom_kernel(param, sargs);\n"
            "}\n"
            "#endif\n",
            encoding="utf-8",
        )
        options = self.make_options(source_include_style="joined")
        sk_options = self.make_options(source_include_style="joined")
        sk_options.compile_options.append("-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__=4")
        records = self.make_records(
            basic_context=basic_context,
            basic_options=options,
            sk_context=sk_context,
            sk_options=sk_options,
            tiling_key_data_size={"100001": 12},
        )

        result = self.generate(
            self.make_request(
                records,
                sk_cap_bitmap=3,
                sk_kernel_attribute="__global__ [aicore]",
                basic_kernel_attribute="__global__ [aicore]",
                sk_bind_compile_options=[*options.compile_options, "-DREAL_BIND_ARGV"],
            )
        )

        manifest = json.loads(Path(result).read_text(encoding="utf-8"))
        self.assertNotIn("-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__=4", json.dumps(manifest))
        sk_object = manifest["kernels"][0]["objects"][1]
        self.assertIn("-DREAL_BIND_ARGV", sk_object["commands"][2]["cmd"])
        self.assertTrue(
            all("stage" not in command for command in sk_object["commands"])
        )
        sk_compiles = sk_object["commands"][:2]
        for core_suffix, command in zip(("aic", "aiv"), sk_compiles):
            self.assertIn("-DTILING_KEY_VAR=100000UL", command["cmd"])
            self.assertIn(
                f"-Dauto_gen_add_custom_kernel="
                f"add_custom_100000_mix_{core_suffix}_static_sk",
                command["cmd"],
            )
            self.assertIn(
                "-Dadd_custom=add_custom_100000_tilingkey",
                command["cmd"],
            )
            self.assertIn(
                f"${{output}}/add_custom_100001.{core_suffix}.sk_source.o",
                command["cmd"],
            )

        logical_renames = sk_object["commands"][3:5]
        for core_suffix, command in zip(("aic", "aiv"), logical_renames):
            self.assertEqual(command["type"], "objcopy")
            self.assertIn(
                f"--redefine-sym=add_custom_sk_100001_mix_{core_suffix}="
                f"add_custom_100001_mix_{core_suffix}_static_sk",
                command["cmd"],
            )
            self.assertIn(
                f"--strip-symbol=add_custom_100000_mix_{core_suffix}_static_sk",
                command["cmd"],
            )
            self.assertEqual(
                command["cmd"][-1],
                f"${{output}}/add_custom_100001.{core_suffix}.sk.o",
            )

        self.assertEqual(
            sk_object["outputs"],
            [
                "${output}/add_custom_100001.aic.sk.o",
                "${output}/add_custom_100001.aiv.sk.o",
                "${output}/add_custom_100001.aic.sk_split1.o",
                "${output}/add_custom_100001.aic.sk_split2.o",
                "${output}/add_custom_100001.aic.sk_split3.o",
                "${output}/add_custom_100001.aiv.sk_split1.o",
                "${output}/add_custom_100001.aiv.sk_split2.o",
                "${output}/add_custom_100001.aiv.sk_split3.o",
                "${output}/add_custom_100001.sk_bind.o",
            ],
        )
        bind_reference = next(
            value
            for value in sk_object["commands"][2]["cmd"]
            if value.startswith("${resource}/resources/src/sk_bind_")
        )
        bind_source = Path(
            self.output_dir,
            "resources",
            self.resource_logical_path(manifest, bind_reference),
        ).read_text(encoding="utf-8")
        for core_suffix in ("aic", "aiv"):
            self.assertIn(
                f"SK_BIND(add_custom_100001_mix_{core_suffix}, 3, "
                f"add_custom_100001_mix_{core_suffix}_static_sk,",
                bind_source,
            )
        sk_wrapper_reference = next(
            value
            for value in sk_compiles[0]["cmd"]
            if value.startswith("${resource}/resources/src/")
        )
        static_sk_wrapper = Path(
            self.output_dir,
            "resources",
            self.resource_logical_path(manifest, sk_wrapper_reference),
        ).read_text(encoding="utf-8")
        self.assertIn(
            "add_custom_sk_100001_mix_aic(\n"
            "    __gm__ uint64_t *param, sk::SkSystemArgs *sargs)",
            static_sk_wrapper,
        )
        self.assertNotIn("uint64_t args_offset", static_sk_wrapper)

    def test_sk_object_uses_static_symbol_bind_and_output_namespace(self):
        dynamic_sk_symbol = "add_custom_abcd1234_100000_tilingkey"
        basic_context = self.make_compile_context(symbols=["add_custom_100000"])
        sk_context = self.make_compile_context(
            kernel_name="add_custom_abcd1234",
            wrapper_path=self.sk_wrapper_path,
            symbols=[dynamic_sk_symbol],
        )
        options = self.make_options(source_include_style="joined")
        records = self.make_records(
            basic_context=basic_context,
            basic_options=options,
            sk_context=sk_context,
            sk_options=options,
        )

        result = self.generate(
            self.make_request(
                records,
                sk_cap_bitmap=3,
                sk_kernel_attribute="__global__ [aicore]",
                basic_kernel_attribute="__global__ [aicore]",
                sk_bind_compile_options=options.compile_options,
            )
        )
        manifest = json.loads(Path(result).read_text(encoding="utf-8"))
        sk_object = manifest["kernels"][0]["objects"][1]
        self.assertEqual(sk_object["object_name"], "sk")
        self.assertEqual(sk_object["object_type"], "sk")
        self.assertTrue(
            all("stage" not in command for command in sk_object["commands"])
        )
        self.assertEqual(
            sk_object["outputs"],
            [
                "${output}/add_custom_100000.sk.o",
                "${output}/add_custom_100000.sk_split1.o",
                "${output}/add_custom_100000.sk_split2.o",
                "${output}/add_custom_100000.sk_split3.o",
                "${output}/add_custom_100000.sk_bind.o",
            ],
        )
        sk_compile = sk_object["commands"][0]["cmd"]
        expanded_sk_compile = list(sk_compile)
        option_index = expanded_sk_compile.index("${options:common_compile}")
        expanded_sk_compile[option_index : option_index + 1] = manifest["options"][
            "common_compile"
        ]
        self.assertTrue(
            any(
                expanded_sk_compile[index : index + 2] == ["-I", "${source_file_path}"]
                for index in range(len(expanded_sk_compile) - 1)
            )
        )
        self.assertIn(
            "-Dauto_gen_add_custom_kernel=add_custom_100000_static_sk",
            sk_compile,
        )
        self.assertIn(
            "-Dadd_custom=add_custom_100000_tilingkey",
            sk_compile,
        )
        self.assertEqual(
            sk_object["commands"][2]["cmd"][0],
            "${env:ASCEND_HOME_PATH}/bin/llvm-objcopy",
        )
        bind_compile = sk_object["commands"][1]["cmd"]
        self.assertNotIn("${source_file_path}", bind_compile)
        self.assertNotIn(self.input_root, json.dumps(bind_compile))
        bind_reference = next(
            value
            for value in sk_object["commands"][1]["cmd"]
            if value.startswith("${resource}/resources/src/sk_bind_")
        )
        bind_path = os.path.join(
            self.output_dir,
            "resources",
            self.resource_logical_path(manifest, bind_reference),
        )
        bind_text = Path(bind_path).read_text(encoding="utf-8")
        self.assertIn("add_custom_100000_static_sk", bind_text)
        self.assertNotIn(dynamic_sk_symbol, bind_text)

    def test_publish_failure_restores_previous_kernel(self):
        request = self.make_request(self.make_records())
        os.makedirs(self.output_dir)
        marker = os.path.join(self.output_dir, "previous.txt")
        Path(marker).write_text("previous\n", encoding="utf-8")
        real_replace = os.replace

        def fail_stage_publish(source, destination):
            if destination == self.output_dir and os.path.basename(source).startswith(
                ".add_custom.stage."
            ):
                raise OSError("publish failed")
            return real_replace(source, destination)

        with mock.patch.object(generator.os, "replace", side_effect=fail_stage_publish):
            with self.assertRaisesRegex(
                generator.ManifestPublishError, "failed to publish"
            ):
                self.generate(request)

        self.assertEqual(Path(marker).read_text(encoding="utf-8"), "previous\n")
        self.assertFalse(
            any(
                name.startswith(".add_custom.stage.")
                for name in os.listdir(self.output_root)
            )
        )

        with mock.patch.object(
            generator,
            "_fsync_directory",
            side_effect=OSError("fsync failed"),
        ):
            with self.assertRaisesRegex(
                generator.ManifestPublishError, "failed to publish"
            ):
                self.generate(request)

        self.assertEqual(Path(marker).read_text(encoding="utf-8"), "previous\n")


if __name__ == "__main__":
    unittest.main()
