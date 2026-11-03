#!/usr/bin/env python3
# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""Lazy read-only environment detection; explicit task targets always win."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

from search import PLATFORMS
from derive.gate import version_key

# Full device identifiers only. Shared runtime enums such as ASCEND910B are ambiguous.
DEVICE_PATTERNS = (
    (r"(?i)\b(?:ascend)?950(?:PR|DT)?(?:[_-]\w+)?\b", "950"),
    (r"(?i)\b(?:ascend)?910_93\w*\b", "a3"),
    (r"(?i)\b(?:ascend)?910B[1-4](?:C)?\b", "a2"),
    (r"(?i)\b(?:ascend)?310B[1-4]\b", "310b"),
    (r"(?i)\b(?:ascend)?310P[1-9]\b", "310p"),
)


def detect(platform=None, cann_version=None, package_root=None):
    observations = []
    candidates = set()
    detected_component_version = None
    if not platform:
        tool = shutil.which("npu-smi")
        if tool:
            try:
                probe = subprocess.run([tool, "info"], capture_output=True, text=True, timeout=5)
                observations.append(
                    {"source": "npu-smi info", "output": probe.stdout[:16000], "returncode": probe.returncode}
                )
                if probe.returncode == 0:
                    for pattern, value in DEVICE_PATTERNS:
                        if re.search(pattern, probe.stdout):
                            candidates.add(value)
                    # Overview output may only expose a shared compatibility name.
                    # Enumerate bounded device IDs and request board identity rather
                    # than defaulting to logical device zero or guessing from arch.
                    if not candidates:
                        listed = subprocess.run([tool, "info", "-l"], capture_output=True, text=True, timeout=5)
                        ids = (
                            re.findall(r"(?im)\bNPU\s+ID\s*:\s*(\d+)\b", listed.stdout)
                            if listed.returncode == 0
                            else []
                        )
                        for device in dict.fromkeys(ids[:8]):
                            board = subprocess.run(
                                [tool, "info", "-t", "board", "-i", device], capture_output=True, text=True, timeout=5
                            )
                            observations.append(
                                {
                                    "source": f"npu-smi board {device}",
                                    "output": board.stdout[:8000],
                                    "returncode": board.returncode,
                                }
                            )
                            if board.returncode == 0:
                                for pattern, value in DEVICE_PATTERNS:
                                    if re.search(pattern, board.stdout):
                                        candidates.add(value)
            except (OSError, subprocess.TimeoutExpired) as exc:
                observations.append({"source": "npu-smi info", "error": str(exc)})
        if len(candidates) == 1:
            platform = next(iter(candidates))
    roots = []
    if package_root:
        roots.append(Path(package_root).expanduser())
    else:
        for key in ("ASCEND_HOME_PATH", "ASCEND_TOOLKIT_HOME"):
            if os.environ.get(key):
                roots.append(Path(os.environ[key]).expanduser())
    if not cann_version:
        versions = set()
        for root in dict.fromkeys(roots):
            # Read installation metadata, never infer from directory names or run installers.
            for rel in ("share/info/asc-devkit/version.info", "version.info"):
                path = root / rel
                if path.is_file():
                    match = re.search(r"(?mi)^\s*version\s*=\s*[\"']?([\w.-]+)", path.read_text())
                    if match:
                        version_key(match[1])
                        versions.add(match[1])
                        observations.append({"source": str(path.resolve()), "version": match[1]})
        if len(versions) == 1:
            cann_version = next(iter(versions))
            detected_component_version = cann_version
    if cann_version:
        version_key(cann_version)
    unknown = [key for key, value in (("platform", platform), ("cann_version", cann_version)) if not value]
    return {
        "platform": platform,
        "arch": PLATFORMS[platform][0] if platform else None,
        "cann_version": cann_version,
        "component_version": detected_component_version,
        "platform_candidates": sorted(candidates),
        "evidence": observations,
        "unknown": unknown,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--platform", choices=sorted(PLATFORMS))
    parser.add_argument("--cann-version")
    parser.add_argument("--package-root")
    args = parser.parse_args()
    try:
        print(json.dumps(detect(args.platform, args.cann_version, args.package_root), ensure_ascii=False))
        return 0
    except (OSError, ValueError) as exc:
        print(json.dumps({"error": str(exc)}))
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
