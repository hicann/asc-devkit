#!/usr/bin/env python3
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

"""Resolve Kernel Spec source ownership and installed source paths."""

import os
from pathlib import Path, PurePosixPath


class BundleError(RuntimeError):
    pass


def _checked_path(value):
    path = Path(os.path.abspath(value))
    if any(parent.is_symlink() for parent in (path, *path.parents)):
        raise BundleError(f"symlink is not permitted: {path}")
    return path


def _relative(value):
    if not isinstance(value, str) or not value or "\\" in value or any(c in value for c in "\n\r\0"):
        raise BundleError(f"invalid relative path: {value!r}")
    path = PurePosixPath(value)
    if path.is_absolute() or ".." in path.parts or path == PurePosixPath("."):
        raise BundleError(f"path must stay below its root: {value}")
    return path


class KernelSpecSourceMapper:
    """Resolve source candidates to their installed paths."""

    def __init__(self, source_map, source_file=()):
        self.source_files = source_file
        maps = {}
        for source, installed in source_map:
            root = _checked_path(source)
            target = _relative(installed).as_posix()
            if root in maps and maps[root] != target:
                raise BundleError(f"conflicting source-map: {root}")
            maps[root] = target
        self.maps = sorted(maps.items(), key=lambda item: (-len(item[0].parts), str(item[0])))

    def map_path(self, path):
        for root, installed in self.maps:
            try:
                tail = path.relative_to(root)
            except ValueError:
                continue
            return str(PurePosixPath(installed) / PurePosixPath(tail.as_posix()))
        return None

    def resolve(self, value, bound_source=None):
        """Resolve only the referenced source; unrelated files do not need installation maps."""
        relative = _relative(value)

        def matches(path):
            if len(relative.parts) == 1:
                return path.name == value
            return self.map_path(path) == relative.as_posix()

        if bound_source is not None:
            bound = _checked_path(bound_source)
            candidates = {path for path in bound.rglob(relative.name) if matches(path)} if bound.is_dir() else {bound}
            if not bound.is_dir() and not matches(bound):
                raise BundleError(f"source_file does not match producer source: {value}: {bound_source}")
        else:
            candidates = set()
            for source_value in self.source_files:
                path = Path(os.path.abspath(source_value))
                if matches(path):
                    candidates.add(path)
        if len(candidates) != 1:
            raise BundleError(f"missing or ambiguous source_file {relative}: {len(candidates)} candidates")
        source = _checked_path(candidates.pop())
        if not source.is_file():
            raise BundleError(f"source file is missing: {source}")
        installed = self.map_path(source)
        if installed is None:
            raise BundleError(f"source file has no source-map: {source}")
        return source, installed
