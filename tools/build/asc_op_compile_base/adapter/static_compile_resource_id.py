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
"""Build a Resource ID metadata object from the linked object inputs."""

from contextlib import suppress
import hashlib
import os
import platform
import shutil
import tempfile

from .ascendc_common_utility import CommonUtility, CompileStage
from .ascendc_compile_base import COMPILER_ARCH, get_soc_spec, link_resource_id_obj
from .global_storage import global_var_storage


_RESOURCE_ID_TYPE = 6
_RESOURCE_ID_VALUE_SIZE = 64
_HASH_CHUNK_SIZE = 1024 * 1024


class ResourceIdError(RuntimeError):
    pass


def calculate_resource_id(object_path):
    """SHA256 of the kernel object as it stands before the Resource ID is embedded."""
    if os.path.islink(object_path):
        raise ResourceIdError(f"link input must not be a symlink: {object_path}")
    digest = hashlib.sha256()
    try:
        with open(object_path, "rb") as file_obj:
            chunk = file_obj.read(_HASH_CHUNK_SIZE)
            while chunk:
                digest.update(chunk)
                chunk = file_obj.read(_HASH_CHUNK_SIZE)
    except OSError as error:
        raise ResourceIdError(f"failed to read link input: {object_path}") from error
    return digest.hexdigest()


def _asc_include_options():
    """Include roots for the ASC headers, resolved the way SuperKernel resolves them."""
    ascend_home_path = os.environ.get("ASCEND_HOME_PATH")
    if not ascend_home_path:
        asc_opc_path = shutil.which("asc_opc")
        if asc_opc_path is None:
            ascend_home_path = "/usr/local/Ascend/cann"
        else:
            asc_opc_dir = os.path.realpath(os.path.dirname(asc_opc_path))
            ascend_home_path = os.path.realpath(os.path.join(asc_opc_dir, "..", ".."))
    arch_dir = "x86_64-linux" if "x86" in platform.machine() else "aarch64-linux"
    asc_path = os.path.realpath(os.path.join(ascend_home_path, arch_dir, "asc"))
    return ["-I" + asc_path, "-I" + os.path.join(asc_path, "include")]


def _render_resource_id_source(resource_id):
    """Render the C++ source that appends the Resource ID entry to .ascend.meta."""
    # One character at a time: a string literal of exactly 64 chars would carry an
    # implicit NUL and overflow the value.
    value = ", ".join(f"'{char}'" for char in resource_id)
    source = '#include "basic_api/kernel_tensor.h"\n'
    source += '__attribute__((used, section(".ascend.meta")))\n'
    source += "static const BinaryMetaSpecializationResourceId "
    source += "g_ascend_resource_id_section = "
    source += "{{B_TYPE_SPECIALIZATION_RESOURCE_ID, %d}, {" % _RESOURCE_ID_VALUE_SIZE
    source += value + "}};\n"
    return source


def generate_resource_id_object(object_path, output_path, compile_command, cce_arch, compile_log_path=None):
    """Compile the Resource ID into a .ascend.meta metadata object."""
    resource_id = calculate_resource_id(object_path)
    directory = os.path.dirname(os.path.realpath(output_path))
    source_fd = None
    source_path = None
    try:
        source_fd, source_path = tempfile.mkstemp(prefix=".resource_id.source.", suffix=".cpp", dir=directory)
        source_file = os.fdopen(source_fd, "w")
        source_fd = None
        with source_file:
            source_file.write(_render_resource_id_source(resource_id))
        with suppress(FileNotFoundError):
            os.unlink(output_path)
        CommonUtility.run_cmd_inner(
            list(compile_command)
            + ["-c", "-O3", "-xcce", source_path]
            + _asc_include_options()
            + ["--cce-aicore-arch=" + cce_arch, "--cce-aicore-only", "-std=c++17", "-o", output_path],
            CompileStage.SPECIALIZATION,
            compile_log_path,
        )
        return resource_id
    finally:
        if source_fd is not None:
            with suppress(OSError):
                os.close(source_fd)
        if source_path is not None:
            with suppress(FileNotFoundError):
                os.unlink(source_path)


def embed_resource_id(dst_file, is_debug, compile_log_path=None):
    """Compile the Resource ID of dst_file and merge it back into dst_file."""
    resource_object = "%s.resource_id.o" % dst_file
    compile_command = [global_var_storage.get_variable("ascendc_compiler_path")]
    if global_var_storage.get_variable("ascendc_enable_ccache"):
        compile_command.insert(0, os.environ.get("ASCENDC_CCACHE_EXECUTABLE"))
    resource_id = generate_resource_id_object(
        dst_file, resource_object, compile_command, get_soc_spec(COMPILER_ARCH), compile_log_path
    )
    link_resource_id_obj(dst_file, resource_object, is_debug, compile_log_path)
    return resource_id
