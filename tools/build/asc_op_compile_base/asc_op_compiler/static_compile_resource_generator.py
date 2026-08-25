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
"""Build and publish static compile Manifests with rollback support."""

import copy
import fcntl
import json
import os
from dataclasses import dataclass, replace
from pathlib import Path
import re
import shutil
import tempfile
from typing import Optional, Tuple

if __package__:
    from .static_compile_resource_id import (
        ResourceIdError,
        generate_and_write_resource_id,
    )
else:
    from static_compile_resource_id import (
        ResourceIdError,
        generate_and_write_resource_id,
    )

_TILING_DATA_TEMPLATE = "@@STATIC_VALUE_tiling_data@@"
_LINK_OPTION = ("-m", "aicorelinux", "-Ttext=0", "-q", "-x")


class ManifestGenerationError(RuntimeError):
    pass


class ManifestResourceError(ManifestGenerationError):
    pass


class ManifestCommandError(ManifestGenerationError):
    pass


class ManifestPublishError(ManifestGenerationError):
    pass


def _kernel_spec_runtime():
    # Keep the Manifest writer importable without loading the compiler runtime.
    from asc_op_compile_base.common import context as context_module
    from asc_op_compile_base.common import error_mgr
    from asc_op_compile_base.asc_op_compiler import ascendc_common_utility
    from asc_op_compile_base.asc_op_compiler import global_storage

    return context_module, error_mgr, ascendc_common_utility, global_storage


@dataclass(frozen=True)
class ConstantInfoRecord:
    """Describe one constant resource referenced by a Manifest Kernel."""

    name: str
    parameter_index: int
    byte_size: int
    template_path: str
    template: str


@dataclass(frozen=True)
class KernelCompileRecord:
    """Bind one logical Tiling Key to its physical compile commands."""

    kernel_name: str
    tiling_key: str
    compile_tiling_key: str
    constant_infos: Tuple[ConstantInfoRecord, ...]
    commands: Tuple[object, ...]
    link_options: Tuple[str, ...] = _LINK_OPTION


@dataclass(frozen=True)
class ManifestInputSnapshot:
    """Bundle compiler outputs required to build one Kernel Manifest package."""

    soc_version: str
    kernel_spec_dir: str
    resource_id: str
    compile_info: object
    basic_compile_info: object
    tiling_info: object
    workspace_idx: int
    sk_compile_info: Optional[object] = None
    sk_cap_bitmap: Optional[int] = None
    constant_infos: Optional[Tuple[ConstantInfoRecord, ...]] = None
    constant_info_size_by_tiling_key: Optional[dict] = None


def _read_regular_file(path, description):
    try:
        return Path(path).read_bytes()
    except OSError as error:
        raise ManifestResourceError(f"failed to read {description}: {path}") from error


def build_tiling_key_group_index(*compile_contexts):
    # Grouped keys reuse the master's physical command but keep logical identities.
    master_to_slaves = {}
    slave_to_master = {}
    for context in compile_contexts:
        if context is None:
            continue
        group_map = getattr(context, "tiling_key_group_map", {}) or {}
        for master, slaves in group_map.items():
            master = str(master)
            known_slaves = list(master_to_slaves.setdefault(master, ()))
            for slave in slaves:
                slave = str(slave)
                if slave == master:
                    raise ManifestCommandError(
                        f"tiling key {master} cannot be both master and slave"
                    )
                previous_master = slave_to_master.get(slave)
                if previous_master is not None and previous_master != master:
                    raise ManifestCommandError(
                        f"tiling key {slave} belongs to multiple masters"
                    )
                slave_to_master[slave] = master
                if slave not in known_slaves:
                    known_slaves.append(slave)
            master_to_slaves[master] = tuple(known_slaves)
    for master in master_to_slaves:
        if master in slave_to_master:
            raise ManifestCommandError(
                f"tiling key {master} cannot be both master and slave"
            )
    return master_to_slaves, slave_to_master


def _logical_symbol(compile_symbol, compile_tiling_key, logical_tiling_key):
    # Replace only the trailing physical key so user symbol prefixes remain intact.
    if logical_tiling_key == compile_tiling_key:
        return compile_symbol
    for core_suffix in ("", "_mix_aic", "_mix_aiv"):
        master_suffix = f"_{compile_tiling_key}{core_suffix}"
        if compile_symbol.endswith(master_suffix):
            return (
                compile_symbol[: -len(master_suffix)]
                + f"_{logical_tiling_key}{core_suffix}"
            )
    raise ManifestCommandError(
        f"compile symbol {compile_symbol} does not end with master tiling key "
        f"{compile_tiling_key}"
    )


class _KernelCompileRecordBuilder:
    """Join compiler metadata with recorded commands for every logical Kernel."""

    def __init__(self, snapshot, constant_infos, constant_info_size_by_tiling_key):
        self._snapshot = snapshot
        self._constant_infos = tuple(constant_infos)
        self._constant_info_size_by_tiling_key = constant_info_size_by_tiling_key
        self._constant_info_files = tuple(
            dict.fromkeys(item.template_path for item in self._constant_infos)
        )

    @staticmethod
    def _resolve_tiling_key(item, logical_keys):
        tiling_key_value = item.get("tilingKey")
        if tiling_key_value is not None:
            return str(tiling_key_value)
        try:
            return next(
                key
                for key in sorted(logical_keys, key=len, reverse=True)
                if item["kernelName"].endswith("_" + key)
            )
        except StopIteration as error:
            raise ManifestCommandError(
                f"cannot determine tiling key for kernel {item['kernelName']}"
            ) from error

    def build(self) -> Tuple[KernelCompileRecord, ...]:
        compile_info = self._snapshot.compile_info
        metadata_json_path = os.path.join(
            os.path.dirname(compile_info.dst_file), compile_info.kernel_name + ".json"
        )
        metadata = json.loads(Path(metadata_json_path).read_text(encoding="utf-8"))
        _, slave_to_master = build_tiling_key_group_index(
            self._snapshot.basic_compile_info,
            self._snapshot.sk_compile_info,
        )
        # Physical commands are indexed before slave keys are expanded.
        commands_by_key = self._group_commands_by_tiling_key()
        exact_sizes = {
            name: {str(key): value for key, value in sizes.items()}
            for name, sizes in (self._constant_info_size_by_tiling_key or {}).items()
        }
        return tuple(
            self._build_record(
                item,
                commands_by_key,
                slave_to_master,
                exact_sizes,
            )
            for item in metadata["kernelList"]
        )

    def _group_commands_by_tiling_key(self):
        snapshot = self._snapshot
        all_commands = list(
            self._build_context_commands(snapshot.basic_compile_info, "basic")
        )
        if snapshot.sk_compile_info is not None:
            all_commands.extend(
                self._build_context_commands(snapshot.sk_compile_info, "sk")
            )
        commands_by_key = {}
        for command in all_commands:
            commands_by_key.setdefault(command.tiling_key, []).append(command)
        return commands_by_key

    def _build_record(self, item, commands_by_key, slave_to_master, exact_sizes):
        tiling_key = self._resolve_tiling_key(
            item,
            set(commands_by_key) | set(slave_to_master),
        )
        compile_tiling_key = slave_to_master.get(tiling_key, tiling_key)
        try:
            physical_commands = commands_by_key[compile_tiling_key]
        except KeyError as error:
            raise ManifestCommandError(
                f"master tiling key {compile_tiling_key} has no physical "
                "compile command"
            ) from error
        commands = tuple(
            replace(
                command,
                tiling_key=tiling_key,
                compile_tiling_key=compile_tiling_key,
                compiled_symbol=_logical_symbol(
                    command.compile_symbol,
                    compile_tiling_key,
                    tiling_key,
                ),
            )
            for command in physical_commands
        )
        return KernelCompileRecord(
            kernel_name=item["kernelName"],
            tiling_key=tiling_key,
            compile_tiling_key=compile_tiling_key,
            constant_infos=self._resolve_constant_infos(
                tiling_key, compile_tiling_key, exact_sizes
            ),
            commands=commands,
            link_options=_LINK_OPTION,
        )

    def _resolve_constant_infos(self, tiling_key, compile_tiling_key, exact_sizes):
        resolved = []
        for constant_info in self._constant_infos:
            sizes = exact_sizes.get(constant_info.name, {})
            byte_size = sizes.get(
                tiling_key,
                sizes.get(compile_tiling_key, constant_info.byte_size),
            )
            resolved.append(replace(constant_info, byte_size=byte_size))
        return tuple(resolved)

    def _build_context_commands(self, compile_info, object_type):
        recorded_commands = compile_info.compile_command_session.records
        if not recorded_commands:
            raise ManifestCommandError(
                f"{object_type} Manifest generation requires recorded "
                "dynamic compile commands"
            )
        return tuple(
            replace(
                spec,
                compile_tiling_key=str(spec.tiling_key),
                compile_symbol=spec.compiled_symbol,
                object_type=object_type,
                constant_info_files=self._constant_info_files,
            )
            for spec in recorded_commands
        )


def static_tiling_template_text(dynamic_header):
    # Replace runtime Tiling access macros with compile-time constant decoding.
    final_endif = dynamic_header.rfind("#endif")

    class_names = []
    class_name_pattern = r"InitTilingData<([A-Za-z_][A-Za-z0-9_:]*)>"
    ignored_class_names = {"T", "tiling_struct", "tiling_type", "type", "Type"}
    for name in re.findall(class_name_pattern, dynamic_header):
        if name not in ignored_class_names:
            class_names.append(name)
    class_name = class_names[0] if class_names else None
    slash = "\\"
    lines = [
        "#ifdef CONST_TILING",
        f"static constexpr uint8_t __ascendc_static_tiling_data[] = {_TILING_DATA_TEMPLATE};",
    ]
    for macro in (
        "GET_TILING_DATA",
        "GET_TILING_DATA_WITH_STRUCT",
        "GET_TILING_DATA_MEMBER",
        "GET_TILING_DATA_PTR_WITH_STRUCT",
    ):
        lines.extend([f"#ifdef {macro}", f"#undef {macro}", "#endif"])
    if class_name is None:
        lines.append("#define GET_TILING_DATA(tiling_data, tiling_arg)")
    else:
        lines.extend(
            [
                f"#define GET_TILING_DATA(tiling_data, tiling_arg) {slash}",
                f"    REGISTER_TILINGDATA_SIZE({class_name}, __COUNTER__); {slash}",
                f"    static constexpr {class_name} tiling_data = {slash}",
                f"        convert_from_bytes<{class_name}>(__ascendc_static_tiling_data);",
            ]
        )
    lines.extend(
        [
            f"#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg) {slash}",
            f"    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__); {slash}",
            f"    static constexpr tiling_struct tiling_data = {slash}",
            "        convert_from_bytes<tiling_struct>(__ascendc_static_tiling_data);",
            f"#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling) {slash}",
            f"    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__); {slash}",
            f"    static constexpr tiling_type __ascendc_static_full_##var = {slash}",
            f"        convert_from_bytes<tiling_type>(__ascendc_static_tiling_data); {slash}",
            "    auto& var = __ascendc_static_full_##var.member;",
            f"#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr) {slash}",
            f"    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__); {slash}",
            f"    static constexpr tiling_struct __ascendc_static_full_##dst_ptr = {slash}",
            f"        convert_from_bytes<tiling_struct>(__ascendc_static_tiling_data); {slash}",
            "    const tiling_struct *dst_ptr = &__ascendc_static_full_##dst_ptr;",
            "#endif",
            "",
        ]
    )
    result = dynamic_header[:final_endif] + "\n" + "\n".join(lines)
    result += dynamic_header[final_endif:]
    return result


def _portable_wrapper_text(source_content, original_source_path, sk_slave_symbols=()):
    # Remove host-specific paths and restore the ABI expected by logical SK slaves.
    wrapper_text = source_content.decode("utf-8")
    absolute_include = f'#include "{original_source_path}"'
    wrapper_text = wrapper_text.replace(
        absolute_include,
        f'#include "{os.path.basename(original_source_path)}"',
        1,
    )
    for symbol in sk_slave_symbols:
        pattern = (
            rf"(\bvoid\s+{re.escape(symbol)}\s*\(\s*)"
            r"uint64_t\s+args_offset(\s*\))"
        )
        replacement = r"\1__gm__ uint64_t *param, sk::SkSystemArgs *sargs\2"
        wrapper_text = re.sub(pattern, replacement, wrapper_text, count=1)
    return wrapper_text


def _add_resource(resources, logical_path, content):
    resources[logical_path] = content
    return logical_path


def _core_suffix(core_type):
    return "aic" if core_type == "cube" else "aiv"


def _output_placeholder(name):
    return "${output}/" + name


def _resource_placeholder(resource_path, logical_path):
    return "${resource}/" + resource_path + "/" + logical_path


def _replace_path_token(value, source, replacement):
    if value == source:
        return replacement
    for prefix in ("-I", "-iquote", "-include", "-isystem"):
        path_value = value[len(prefix) :] if value.startswith(prefix) else value
        if value.startswith(prefix) and path_value == source:
            return prefix + replacement
    return value


def _parse_include_option(argv, index):
    value = argv[index]
    if value == "-I" and index + 1 < len(argv):
        return argv[index + 1], 2
    if value.startswith("-I") and value != "-I":
        return value[2:], 1
    return None, 1


def _normalize_source_include_options(
    argv, source_file_path, replacement, insertion_before=None
):
    source_dir = os.path.dirname(source_file_path)
    normalized = []
    source_include_added = False
    index = 0
    while index < len(argv):
        value = argv[index]
        include_path, option_size = _parse_include_option(argv, index)
        if include_path is None:
            normalized.append(value)
            index += option_size
            continue
        if include_path == source_dir:
            if replacement is not None and not source_include_added:
                normalized.extend(["-I", replacement])
                source_include_added = True
            index += option_size
            continue
        if os.path.isabs(include_path) and not os.path.exists(include_path):
            index += option_size
            continue
        normalized.extend(argv[index : index + option_size])
        index += option_size
    if replacement is not None and not source_include_added:
        insertion_index = next(
            (
                index
                for index, value in enumerate(normalized)
                if insertion_before is not None and value == insertion_before
            ),
            len(normalized),
        )
        normalized[insertion_index:insertion_index] = ["-I", replacement]
    return normalized


def _static_bind_source(pairs, basic_attribute, sk_attribute, cap_bitmap):
    lines = ['#include "kernel_operator.h"']
    for basic_symbol, sk_symbol in pairs:
        lines.extend(
            [
                f'extern "C" {basic_attribute} void {basic_symbol}();',
                f'extern "C" {sk_attribute} void {sk_symbol}();',
                f'extern "C" {sk_attribute} void {sk_symbol}_split1();',
                f'extern "C" {sk_attribute} void {sk_symbol}_split2();',
                f'extern "C" {sk_attribute} void {sk_symbol}_split3();',
                f"SK_BIND({basic_symbol}, {cap_bitmap}, {sk_symbol}, "
                f"{sk_symbol}_split1, {sk_symbol}_split2, {sk_symbol}_split3);",
            ]
        )
    return ("\n".join(lines) + "\n").encode("utf-8")


def _factor_common_compile(commands, resource_path):
    # Factor only the common prefix after the compiler executable.
    prefixes = []
    for command in commands:
        argv = command["cmd"]
        source_index = next(
            index
            for index, value in enumerate(argv)
            if value.startswith("${resource}/" + resource_path + "/src/")
        )
        prefixes.append(argv[1:source_index])
    common = list(prefixes[0])
    for prefix in prefixes[1:]:
        length = 0
        while (
            length < min(len(common), len(prefix)) and common[length] == prefix[length]
        ):
            length += 1
        common = common[:length]
    for command in commands:
        command["cmd"][1 : 1 + len(common)] = ["${options:common_compile}"]
    return common


def _write_resources(stage_dir, resource_path, resources):
    resource_root = os.path.join(stage_dir, resource_path)
    for logical_path, content in sorted(resources.items()):
        destination = os.path.join(resource_root, *logical_path.split("/"))
        os.makedirs(os.path.dirname(destination), exist_ok=True)
        Path(destination).write_bytes(content)


def _fsync_tree(root):
    for directory, _, files in os.walk(root, topdown=False):
        for name in files:
            with open(os.path.join(directory, name), "rb") as file_obj:
                os.fsync(file_obj.fileno())
        descriptor = os.open(directory, os.O_RDONLY)
        try:
            os.fsync(descriptor)
        finally:
            os.close(descriptor)


def _fsync_directory(path):
    descriptor = os.open(path, os.O_RDONLY)
    try:
        os.fsync(descriptor)
    finally:
        os.close(descriptor)


def _publish_directory(stage_dir, target_dir):
    # Move the previous package aside so it can be restored if publication fails.
    parent = os.path.dirname(target_dir)
    backup_dir = tempfile.mkdtemp(prefix=".manifest.backup.", dir=parent)
    os.rmdir(backup_dir)
    moved_previous = False
    published = False
    try:
        if os.path.lexists(target_dir):
            os.replace(target_dir, backup_dir)
            moved_previous = True
        os.replace(stage_dir, target_dir)
        published = True
        _fsync_directory(parent)
    except Exception as error:
        if published and os.path.lexists(target_dir):
            os.replace(target_dir, stage_dir)
        if moved_previous and not os.path.lexists(target_dir):
            os.replace(backup_dir, target_dir)
        raise ManifestPublishError("failed to publish Manifest directory") from error
    if moved_previous:
        shutil.rmtree(backup_dir)


class ManifestPackageWriter:
    """Collect packaged Manifest resources and publish the Manifest."""

    def __init__(self, snapshot: ManifestInputSnapshot):
        self._snapshot = snapshot
        compile_info = snapshot.compile_info
        tiling_info = snapshot.tiling_info
        manifest_tiling_size = tiling_info.default_tiling_size
        if manifest_tiling_size <= 0:
            manifest_tiling_size = compile_info.max_tiling_size
        manifest_tiling_sizes = dict(tiling_info.tiling_key_data_size)
        manifest_tiling_sizes.update(compile_info.compiled_tiling_key_data_size_map)
        constant_infos = snapshot.constant_infos or (
            ConstantInfoRecord(
                name="tiling_data",
                parameter_index=snapshot.workspace_idx + 1,
                byte_size=manifest_tiling_size,
                template_path=tiling_info.tiling_data_file_path,
                template=_TILING_DATA_TEMPLATE,
            ),
        )
        constant_info_sizes = snapshot.constant_info_size_by_tiling_key or {
            "tiling_data": manifest_tiling_sizes
        }
        self._kernels = _KernelCompileRecordBuilder(
            snapshot, constant_infos, constant_info_sizes
        ).build()
        self._source_file_path = compile_info.src_file
        self._origin_func_name = compile_info.origin_func_name
        self._manifest_name = f"{compile_info.kernel_name}_manifest.json"
        output_dir = os.path.join(snapshot.kernel_spec_dir, compile_info.kernel_name)
        self._target_dir = os.path.abspath(output_dir)
        self._parent_dir = os.path.dirname(self._target_dir)
        self._kernel_name = os.path.basename(self._target_dir)
        self._resource_path = "resources"
        self._stage_dir = ""
        self._resources = {}
        self._wrapper_paths = {}
        self._constant_info_logicals = {}
        self._wrapper_commands = []

    @staticmethod
    def _append_sk_objcopy_commands(commands, rename_specs, split_specs):
        for (
            dynamic_symbol,
            static_symbol,
            compile_symbol,
            source,
            output,
        ) in rename_specs:
            commands.append(
                {
                    "type": "objcopy",
                    "cmd": [
                        "${env:ASCEND_HOME_PATH}/bin/llvm-objcopy",
                        f"--redefine-sym={dynamic_symbol}={static_symbol}",
                        f"--strip-symbol={compile_symbol}",
                        source,
                        output,
                    ],
                }
            )
        for static_symbol, sk_output, split_index, split_name in split_specs:
            commands.append(
                {
                    "type": "objcopy",
                    "cmd": [
                        "${env:ASCEND_HOME_PATH}/bin/llvm-objcopy",
                        f"--redefine-sym={static_symbol}="
                        f"{static_symbol}_split{split_index}",
                        sk_output,
                        _output_placeholder(split_name),
                    ],
                }
            )

    @staticmethod
    def _build_sk_split_specs(
        kernel, core_suffix, static_symbol, final_output, multiple_cores
    ):
        specs = []
        for split_index in range(1, 4):
            split_name = f"{kernel.kernel_name}.sk_split{split_index}.o"
            if multiple_cores:
                split_name = (
                    f"{kernel.kernel_name}.{core_suffix}.sk_split{split_index}.o"
                )
            specs.append((static_symbol, final_output, split_index, split_name))
        return tuple(specs)

    def write(self) -> str:
        os.makedirs(self._parent_dir, exist_ok=True)
        lock_path = os.path.join(
            self._parent_dir, f".{self._kernel_name}.manifest.lock"
        )
        with open(lock_path, "a+", encoding="utf-8") as lock_file:
            fcntl.flock(lock_file.fileno(), fcntl.LOCK_EX)
            try:
                # Build the complete package outside the visible target directory.
                self._stage_dir = tempfile.mkdtemp(
                    prefix=f".{self._kernel_name}.stage.", dir=self._parent_dir
                )
                self._collect_resources()
                manifest = self._build_manifest()
                _write_resources(self._stage_dir, self._resource_path, self._resources)
                manifest_path = os.path.join(self._stage_dir, self._manifest_name)
                with open(manifest_path, "w", encoding="utf-8") as file_obj:
                    json.dump(manifest, file_obj, indent=2, ensure_ascii=False)
                    file_obj.write("\n")
                # Persist the package before exposing it through the final rename.
                _fsync_tree(self._stage_dir)
                _publish_directory(self._stage_dir, self._target_dir)
                self._stage_dir = ""
                return os.path.join(
                    self._target_dir,
                    self._manifest_name,
                )
            finally:
                if self._stage_dir and os.path.isdir(self._stage_dir):
                    shutil.rmtree(self._stage_dir)
                fcntl.flock(lock_file.fileno(), fcntl.LOCK_UN)

    def _collect_resources(self):
        # Copy each generated wrapper once even when several logical keys reuse it.
        all_records = []
        for kernel in self._kernels:
            all_records.extend(kernel.commands)
        for command in all_records:
            source_path = command.source_path
            if source_path in self._wrapper_paths:
                continue
            logical = "src/" + os.path.basename(source_path)
            source_content = _read_regular_file(source_path, "generated wrapper")
            sk_slave_symbols = []
            for record in all_records:
                if record.source_path != source_path:
                    continue
                if record.object_type != "sk":
                    continue
                if record.tiling_key == record.compile_tiling_key:
                    continue
                sk_slave_symbols.append(record.compiled_symbol)
            wrapper = _portable_wrapper_text(
                source_content,
                self._source_file_path,
                tuple(sk_slave_symbols),
            ).encode("utf-8")
            self._wrapper_paths[source_path] = _add_resource(
                self._resources, logical, content=wrapper
            )

        # Constant templates are shared resources with per-Kernel logical values.
        templates_by_path = {}
        for kernel in self._kernels:
            for constant_info in kernel.constant_infos:
                templates_by_path.setdefault(constant_info.template_path, set()).add(
                    constant_info.template
                )
        for path, template_markers in templates_by_path.items():
            source_text = _read_regular_file(path, "constant info template").decode(
                "utf-8"
            )
            logical = "include/" + os.path.basename(path)
            if _TILING_DATA_TEMPLATE in template_markers:
                source_text = static_tiling_template_text(source_text)
            self._constant_info_logicals[path] = _add_resource(
                self._resources, logical, content=source_text.encode("utf-8")
            )

    def _build_manifest(self):
        # Preserve cube-before-vector ordering for deterministic Manifest output.
        manifest_kernels = []
        for kernel in self._kernels:
            basic_records = sorted(
                (record for record in kernel.commands if record.object_type == "basic"),
                key=lambda record: (0 if record.core_type == "cube" else 1),
            )
            objects = [self._build_basic_object(kernel, basic_records)]
            sk_records = sorted(
                (record for record in kernel.commands if record.object_type == "sk"),
                key=lambda record: (0 if record.core_type == "cube" else 1),
            )
            if sk_records:
                objects.append(self._build_sk_object(kernel, basic_records, sk_records))
            manifest_kernels.append(
                {
                    "kernel_name": kernel.kernel_name,
                    "constant_infos": [
                        {
                            "name": constant_info.name,
                            "parameter_index": constant_info.parameter_index,
                            "byte_size": constant_info.byte_size,
                            "file": _resource_placeholder(
                                self._resource_path,
                                self._constant_info_logicals[
                                    constant_info.template_path
                                ],
                            ),
                            "template": constant_info.template,
                        }
                        for constant_info in kernel.constant_infos
                    ],
                    "link_options": list(kernel.link_options),
                    "objects": objects,
                }
            )

        common_compile = _factor_common_compile(
            self._wrapper_commands, self._resource_path
        )
        return {
            "soc_version": self._snapshot.soc_version,
            "resource_id": self._snapshot.resource_id,
            "source_file": os.path.basename(self._source_file_path),
            "resource_path": self._resource_path,
            "options": {"common_compile": common_compile},
            "kernels": manifest_kernels,
        }

    def _build_basic_object(self, kernel, records):
        commands = []
        outputs = []
        for record in records:
            output_name = f"{kernel.kernel_name}.{_core_suffix(record.core_type)}.o"
            command = {
                "type": "compile",
                "cmd": self._normalize_compile_command(record, output_name),
            }
            commands.append(command)
            self._wrapper_commands.append(command)
            outputs.append(_output_placeholder(output_name))
        return {
            "object_name": "basic",
            "object_type": "basic",
            "commands": commands,
            "outputs": outputs,
        }

    def _build_sk_object(self, kernel, basic_records, sk_records):
        basic_by_core = {record.core_type: record for record in basic_records}
        commands = []
        outputs = []
        logical_rename_specs = []
        split_specs = []
        bind_pairs = []
        multiple_cores = len(sk_records) > 1
        for record in sk_records:
            basic_record = basic_by_core[record.core_type]
            command, final_output, rename_spec, record_splits, bind_pair = (
                self._plan_sk_compile(
                    kernel,
                    basic_record,
                    record,
                    multiple_cores,
                )
            )
            commands.append(command)
            self._wrapper_commands.append(command)
            outputs.append(final_output)
            if rename_spec is not None:
                logical_rename_specs.append(rename_spec)
            split_specs.extend(record_splits)
            bind_pairs.append(bind_pair)

        # SK compile outputs are followed by bind and symbol-rewrite commands.
        bind_output_name = self._append_sk_bind_command(
            kernel,
            bind_pairs,
            commands,
        )
        self._append_sk_objcopy_commands(
            commands,
            logical_rename_specs,
            split_specs,
        )
        outputs.extend(
            _output_placeholder(split_name) for _, _, _, split_name in split_specs
        )
        outputs.append(_output_placeholder(bind_output_name))
        return {
            "object_name": "sk",
            "object_type": "sk",
            "commands": commands,
            "outputs": outputs,
        }

    def _plan_sk_compile(self, kernel, basic_record, record, multiple_cores):
        static_sk_symbol = basic_record.compiled_symbol + "_static_sk"
        compile_static_sk_symbol = basic_record.compile_symbol + "_static_sk"
        core_suffix = _core_suffix(record.core_type)
        final_output_name = (
            f"{kernel.kernel_name}.{core_suffix}.sk.o"
            if multiple_cores
            else f"{kernel.kernel_name}.sk.o"
        )
        # A slave compiles the master key and then renames the resulting symbols.
        is_slave = kernel.tiling_key != kernel.compile_tiling_key
        compile_output_name = (
            final_output_name.replace(".sk.o", ".sk_source.o")
            if is_slave
            else final_output_name
        )
        command = {
            "type": "compile",
            "cmd": self._normalize_compile_command(
                record,
                compile_output_name,
                target_symbol=compile_static_sk_symbol,
                tiling_key=kernel.compile_tiling_key,
            ),
        }
        compile_output = _output_placeholder(compile_output_name)
        final_output = _output_placeholder(final_output_name)
        rename_spec = None
        if is_slave:
            rename_spec = (
                record.compiled_symbol,
                static_sk_symbol,
                compile_static_sk_symbol,
                compile_output,
                final_output,
            )
        split_specs = self._build_sk_split_specs(
            kernel,
            core_suffix,
            static_sk_symbol,
            final_output,
            multiple_cores,
        )
        bind_pair = (basic_record.compiled_symbol, static_sk_symbol)
        return command, final_output, rename_spec, split_specs, bind_pair

    def _append_sk_bind_command(self, kernel, bind_pairs, commands):
        sk_attribute = self._snapshot.sk_compile_info.global_kernel_attribute
        basic_attribute = self._snapshot.basic_compile_info.global_kernel_attribute
        bind_name = f"sk_bind_{kernel.kernel_name}.cpp"
        bind_logical = _add_resource(
            self._resources,
            "src/" + bind_name,
            content=_static_bind_source(
                bind_pairs,
                basic_attribute or sk_attribute,
                sk_attribute,
                self._snapshot.sk_cap_bitmap,
            ),
        )
        output_name = f"{kernel.kernel_name}.sk_bind.o"
        commands.append(
            {
                "type": "compile",
                "cmd": self._build_bind_command(bind_logical, output_name),
            }
        )
        return output_name

    def _normalize_compile_command(
        self,
        record,
        output_name,
        target_symbol=None,
        tiling_key=None,
    ):
        # Convert a recorded host compile command into a relocatable Manifest command.
        argv = self._prepare_compile_argv(record)
        source_reference = _resource_placeholder(
            self._resource_path, self._wrapper_paths[record.source_path]
        )
        normalized = self._replace_compile_paths(
            argv,
            record,
            output_name,
            source_reference,
        )
        self._rewrite_static_sk_symbols(normalized, target_symbol, tiling_key)
        source_index = normalized.index(source_reference)
        if "-DCONST_TILING" not in normalized:
            normalized.insert(source_index, "-DCONST_TILING")
        return normalized

    def _prepare_compile_argv(self, record):
        argv = _normalize_source_include_options(
            list(record.argv),
            self._source_file_path,
            "${source_file_path}",
            insertion_before=record.source_path,
        )
        argv = [
            value
            for value in argv
            if not value.startswith("-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__")
        ]
        compiler_index = next(
            index
            for index, value in enumerate(argv)
            if os.path.basename(value) == "bisheng"
        )
        argv = argv[compiler_index:]
        argv[0] = "${env:ASCEND_HOME_PATH}/bin/bisheng"
        return argv

    def _replace_compile_paths(self, argv, record, output_name, source_reference):
        constant_info_references = {
            path: _resource_placeholder(
                self._resource_path, self._constant_info_logicals[path]
            )
            for path in record.constant_info_files
        }
        output_reference = _output_placeholder(output_name)
        cann_root = os.environ.get("ASCEND_HOME_PATH", "")
        normalized = []
        for value in argv:
            if value == record.source_path:
                value = source_reference
            else:
                value = _replace_path_token(value, record.source_path, source_reference)
            value = _replace_path_token(value, record.output_path, output_reference)
            for constant_info_file, reference in constant_info_references.items():
                value = _replace_path_token(value, constant_info_file, reference)
            if cann_root and cann_root in value:
                value = value.replace(cann_root, "${env:ASCEND_HOME_PATH}")
            normalized.append(value)

        for index, value in enumerate(normalized):
            if value == record.source_path:
                normalized[index] = source_reference
            if value.startswith("-include") and record.source_path in value:
                normalized[index] = value.replace(record.source_path, source_reference)
        return normalized

    def _rewrite_static_sk_symbols(self, argv, target_symbol, tiling_key):
        if target_symbol is None:
            return
        origin_func_name = self._origin_func_name
        auto_gen_prefix = f"-Dauto_gen_{origin_func_name}_kernel="
        origin_prefix = f"-D{origin_func_name}="
        for index, value in enumerate(argv):
            if value.startswith(auto_gen_prefix):
                argv[index] = auto_gen_prefix + target_symbol
            elif value.startswith(origin_prefix):
                argv[index] = (
                    origin_prefix + f"{origin_func_name}_{tiling_key}_tilingkey"
                )

    def _build_bind_command(self, source_logical, output_name):
        sk_compile_info = self._snapshot.sk_compile_info
        recorded_commands = sk_compile_info.compile_command_session.sk_bind_records
        recorded = recorded_commands[0] if recorded_commands else None
        if recorded is None:
            raise ManifestCommandError(
                "SK Manifest generation requires a recorded sk-bind command"
            )
        source_reference = _resource_placeholder(self._resource_path, source_logical)
        output_reference = _output_placeholder(output_name)
        argv = list(recorded.argv)
        compiler_index = next(
            index
            for index, value in enumerate(argv)
            if os.path.basename(value) == "bisheng"
        )
        argv = argv[compiler_index:]
        argv[0] = "${env:ASCEND_HOME_PATH}/bin/bisheng"
        argv = _normalize_source_include_options(argv, self._source_file_path, None)
        cann_root = os.environ.get("ASCEND_HOME_PATH", "")
        normalized = []
        for value in argv:
            if value == recorded.source_path:
                value = source_reference
            else:
                value = _replace_path_token(
                    value, recorded.source_path, source_reference
                )
            value = _replace_path_token(value, recorded.output_path, output_reference)
            if value == "-DCONST_TILING" or value.startswith(
                "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__"
            ):
                continue
            if cann_root and cann_root in value:
                value = value.replace(cann_root, "${env:ASCEND_HOME_PATH}")
            normalized.append(value)
        return normalized


class KernelSpecCompilation:
    """Own Kernel Spec state, Resource ID publication, and cleanup lifecycle."""

    _GLOBAL_STATE_NAMES = (
        "ascendc_enable_super_kernel",
        "ascendc_sk_double_compile",
        "ascendc_sk_sub_combine_norm_workflow",
    )

    def __init__(self, compile_info, compile_option_tuple, tiling_info):
        context_module, _, _, storage_module = _kernel_spec_runtime()
        context = context_module.get_context()
        global_var_storage = storage_module.global_var_storage
        self.compile_info = compile_info
        self.basic_compile_info = compile_info
        self.kernel_spec_dir = context.get_addition("kernel_spec_dir")
        self.resource_id = None
        self.sk_cap_bitmap = None
        self._cleaned = False
        # Save process-global SK state before the record-only replay changes it.
        self._global_state = {
            name: global_var_storage.get_variable(name)
            for name in self._GLOBAL_STATE_NAMES
        }
        is_sk_double_compile = (
            context.get_addition("super_kernel_sub_combine") is True
            and self._global_state["ascendc_enable_super_kernel"] is True
        )
        self._kernel_spec_mode = context.get_addition("kernel_spec_mode") or "None"
        self.enabled = (
            self._kernel_spec_mode in {"Normal", "SK"}
            and not tiling_info.static_shape_flag
        )
        self._validate_configuration()
        self.record_sk_commands = (
            self.enabled and self._kernel_spec_mode == "SK" and is_sk_double_compile
        )
        self.sk_compile_info = None
        self.sk_compile_option_tuple = None
        self._initialize_sk_recording(compile_option_tuple)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if not self.record_sk_commands:
            return False
        _, _, _, storage_module = _kernel_spec_runtime()
        global_var_storage = storage_module.global_var_storage
        try:
            self.cleanup()
        finally:
            # Restore the entry state even when replay or cleanup fails.
            for name, value in self._global_state.items():
                global_var_storage.set_variable(name, value)
        return False

    @classmethod
    def create(cls, compile_info, compile_option_tuple, tiling_info):
        return cls(compile_info, compile_option_tuple, tiling_info)

    def select_basic_compile(self, compile_info_origin, compile_option_tuple_origin):
        if compile_option_tuple_origin is not None:
            self.basic_compile_info = compile_info_origin
        if self.enabled:
            # Execute the Normal compile and retain its commands for the Manifest.
            _, _, utility_module, _ = _kernel_spec_runtime()
            self.basic_compile_info.compile_command_session = (
                utility_module.CompileCommandSession(
                    utility_module.CompileCommandMode.EXECUTE_AND_RECORD
                )
            )

    def attach_resource_id(self):
        if not self.enabled:
            return
        _, error_mgr, utility_module, _ = _kernel_spec_runtime()
        try:
            self.resource_id = generate_and_write_resource_id(
                self.compile_info.dst_file
            )
        except ResourceIdError as error:
            utility_module.CommonUtility.ascendc_raise_python_err(
                error_mgr.TBE_DEFAULT_PYTHON_ERROR_CODE,
                f"generate Resource ID failed, reason is: {error}",
            )

    def begin_sk_recording(self):
        if not self.record_sk_commands:
            return False
        # Restore SK state only for the record-only replay.
        _, _, _, storage_module = _kernel_spec_runtime()
        self.basic_compile_info.global_kernel_symbols = [
            command.compiled_symbol
            for command in self.basic_compile_info.compile_command_session.records
        ]
        global_var_storage = storage_module.global_var_storage
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        global_var_storage.set_variable("ascendc_sk_double_compile", True)
        global_var_storage.set_variable("ascendc_sk_sub_combine_norm_workflow", False)
        return True

    def finish_sk_recording(self, sk_cap_bitmap):
        self.sk_cap_bitmap = sk_cap_bitmap

    def publish_manifest(self, tiling_info, workspace_idx):
        if not self.enabled:
            return
        # Publish after the Resource ID and all requested commands are available.
        _, error_mgr, utility_module, storage_module = _kernel_spec_runtime()
        try:
            ManifestPackageWriter(
                ManifestInputSnapshot(
                    soc_version=storage_module.global_var_storage.get_variable(
                        "ascendc_short_soc_version"
                    ),
                    kernel_spec_dir=self.kernel_spec_dir,
                    resource_id=self.resource_id,
                    compile_info=self.compile_info,
                    basic_compile_info=self.basic_compile_info,
                    tiling_info=tiling_info,
                    workspace_idx=workspace_idx,
                    sk_compile_info=self.sk_compile_info,
                    sk_cap_bitmap=self.sk_cap_bitmap,
                )
            ).write()
        except ManifestGenerationError as error:
            utility_module.CommonUtility.ascendc_raise_python_err(
                error_mgr.TBE_DEFAULT_PYTHON_ERROR_CODE,
                f"generate Manifest failed, reason is: {error}",
            )

    def cleanup(self):
        if self._cleaned:
            return
        self._cleaned = True
        _, _, utility_module, storage_module = _kernel_spec_runtime()
        if (
            self.record_sk_commands
            and self.sk_compile_info.gen_kernel_func_file
            and not storage_module.global_var_storage.get_variable(
                "ascendc_compile_debug_config"
            )
        ):
            utility_module.CommonUtility.remove_temp_file(
                self.sk_compile_info.gen_kernel_func_file
            )

    def _validate_configuration(self):
        if not self.enabled:
            return
        _, error_mgr, utility_module, _ = _kernel_spec_runtime()
        common_utility = utility_module.CommonUtility
        if not (common_utility.is_v220() or common_utility.is_c310()):
            error_mgr.raise_tbe_python_err(
                error_mgr.TBE_DEFAULT_PYTHON_ERROR_CODE,
                "Kernel Spec only supports v220 and c310 architectures.",
            )
        if not self.kernel_spec_dir:
            error_mgr.raise_tbe_python_err(
                error_mgr.TBE_DEFAULT_PYTHON_ERROR_CODE,
                "kernel_spec_dir is required when kernel_spec_mode is Normal or SK.",
            )

    def _initialize_sk_recording(self, compile_option_tuple):
        if not self.record_sk_commands:
            return
        _, _, utility_module, storage_module = _kernel_spec_runtime()
        self.sk_compile_info = copy.deepcopy(self.compile_info)
        self.sk_compile_option_tuple = copy.deepcopy(compile_option_tuple)
        self.sk_compile_info.compile_command_session = (
            utility_module.CompileCommandSession(
                utility_module.CompileCommandMode.RECORD_ONLY
            )
        )
        # Compile the normal object first, then replay the SK workflow to
        # record commands without producing a second set of objects.
        global_var_storage = storage_module.global_var_storage
        global_var_storage.set_variable("ascendc_enable_super_kernel", False)
        global_var_storage.set_variable("ascendc_sk_double_compile", False)
        global_var_storage.set_variable("ascendc_sk_sub_combine_norm_workflow", True)
