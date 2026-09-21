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

from __future__ import annotations

import argparse
import logging
import os
import sys
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = PROJECT_ROOT / ".clangd.local"
TEMPLATE = PROJECT_ROOT / ".clangd.in"
REQUIRED_DIRS = (
    "asc/include",
    "asc/impl",
    "tools/cpudebug/lib/include",
    "include/ascendc/highlevel_api",
    "include/ascendc/host_api",
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate a clangd config from ASCEND_HOME_PATH for a local CANN installation."
    )
    parser.add_argument("--npu-arch", default="2201", choices=("2201", "3510"), help="Value for __NPU_ARCH__.")
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT), help="Output config path. Default: .clangd.local")
    parser.add_argument("-f", "--force", action="store_true", help="Overwrite the output file if it already exists.")
    return parser.parse_args()


def missing_required_dirs(ascend_home_path: Path) -> list[str]:
    return [rel_path for rel_path in REQUIRED_DIRS if not (ascend_home_path / rel_path).is_dir()]


def render_template(ascend_home_path: Path, npu_arch: str) -> str:
    template = TEMPLATE.read_text(encoding="utf-8")
    return template.replace("@ASCEND_HOME_PATH@", ascend_home_path.as_posix()).replace("@NPU_ARCH@", npu_arch)


def configure_logging() -> None:
    logging.basicConfig(level=logging.INFO, format="%(message)s")


def main() -> int:
    configure_logging()
    args = parse_args()
    ascend_home_env = os.environ.get("ASCEND_HOME_PATH")
    if not ascend_home_env:
        logging.error("ASCEND_HOME_PATH is not set. Please source CANN set_env.sh and retry.")
        return 1

    ascend_home_path = Path(ascend_home_env).expanduser().resolve()
    output = Path(args.output).expanduser()
    if not output.is_absolute():
        output = PROJECT_ROOT / output
    output = output.resolve()

    missing = missing_required_dirs(ascend_home_path)
    if missing:
        logging.error("Missing required CANN directories under ASCEND_HOME_PATH:")
        for rel_path in missing:
            logging.error("  - %s", ascend_home_path / rel_path)
        return 1

    if output.exists() and not args.force:
        logging.error("%s already exists. Use --force to overwrite it.", output)
        return 1

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(render_template(ascend_home_path, args.npu_arch), encoding="utf-8")
    logging.info("Generated %s using ASCEND_HOME_PATH=%s", output, ascend_home_path)
    logging.info("Copy it to .clangd or point clangd user config to these flags if you want clangd to load it.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
