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
"""Calculate a stable Resource ID and atomically attach it to an ELF object."""

from contextlib import suppress
import hashlib
import os
import re
import shutil
import subprocess
import tempfile


_RESOURCE_ID_SECTION = ".ascend.resource_id"
_RESOURCE_ID_PATTERN = re.compile(r"^[0-9a-f]{64}$")


class ResourceIdError(RuntimeError):
    pass


def _validate_elf(dynamic_object_path):
    if not os.path.isfile(dynamic_object_path) or os.path.islink(dynamic_object_path):
        raise ResourceIdError(
            f"ELF object is not a regular file: {dynamic_object_path}"
        )
    try:
        with open(dynamic_object_path, "rb") as file_obj:
            magic = file_obj.read(4)
    except OSError as error:
        raise ResourceIdError(
            f"failed to read ELF object: {dynamic_object_path}"
        ) from error
    if magic != b"\x7fELF":
        raise ResourceIdError(f"input is not an ELF object: {dynamic_object_path}")


def _find_tool(*names):
    cann_root = os.environ.get("ASCEND_HOME_PATH")
    for name in names:
        if cann_root:
            candidate = os.path.join(cann_root, "bin", name)
            if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
                return candidate
        candidate = shutil.which(name)
        if candidate:
            return candidate
    raise ResourceIdError(f"required ELF tool is unavailable: {', '.join(names)}")


def _run_tool(command):
    try:
        result = subprocess.run(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            check=False,
            text=True,
        )
    except OSError as error:
        raise ResourceIdError(f"failed to execute ELF tool: {command[0]}") from error
    if result.returncode != 0:
        output = result.stdout.strip()
        raise ResourceIdError(f"ELF tool failed ({command[0]}): {output}")
    return result.stdout


def calculate_resource_id(dynamic_object_path: str) -> str:
    # Calculate the digest before the Resource ID section changes the ELF.
    _validate_elf(dynamic_object_path)
    digest = hashlib.sha256()
    try:
        with open(dynamic_object_path, "rb") as file_obj:
            for chunk in iter(lambda: file_obj.read(1024 * 1024), b""):
                digest.update(chunk)
    except OSError as error:
        raise ResourceIdError(
            f"failed to hash ELF object: {dynamic_object_path}"
        ) from error
    return digest.hexdigest()


def write_resource_id_section(dynamic_object_path: str, resource_id: str) -> None:
    if not isinstance(resource_id, str) or not _RESOURCE_ID_PATTERN.fullmatch(
        resource_id
    ):
        raise ResourceIdError("resource_id must be 64 lowercase hex characters")
    objcopy = _find_tool("llvm-objcopy")
    directory = os.path.dirname(os.path.realpath(dynamic_object_path))
    object_fd = None
    payload_fd = None
    temporary_object = None
    payload_path = None
    try:
        # Keep temporary files beside the ELF so the final replacement is atomic.
        object_fd, temporary_object = tempfile.mkstemp(
            prefix=".resource_id.object.", suffix=".o", dir=directory
        )
        os.close(object_fd)
        object_fd = None
        payload_fd, payload_path = tempfile.mkstemp(
            prefix=".resource_id.payload.", dir=directory
        )
        payload_file = os.fdopen(payload_fd, "wb")
        payload_fd = None
        with payload_file:
            payload_file.write(resource_id.encode("ascii"))
            payload_file.flush()
            os.fsync(payload_file.fileno())
        # Mutate a private copy and publish it only after objcopy succeeds.
        shutil.copy2(dynamic_object_path, temporary_object)
        _run_tool(
            [
                objcopy,
                "--add-section",
                f"{_RESOURCE_ID_SECTION}={payload_path}",
                "--set-section-flags",
                f"{_RESOURCE_ID_SECTION}=readonly",
                temporary_object,
            ]
        )
        with open(temporary_object, "rb") as temporary_file:
            os.fsync(temporary_file.fileno())
        os.replace(temporary_object, dynamic_object_path)
    finally:
        # Cover failures before descriptors are transferred to file objects.
        for file_descriptor in (object_fd, payload_fd):
            if file_descriptor is None:
                continue
            with suppress(OSError):
                os.close(file_descriptor)
        for path in (temporary_object, payload_path):
            if path is None:
                continue
            try:
                os.unlink(path)
            except FileNotFoundError:
                pass


def generate_and_write_resource_id(dynamic_object_path: str) -> str:
    resource_id = calculate_resource_id(dynamic_object_path)
    write_resource_id_section(dynamic_object_path, resource_id)
    return resource_id
