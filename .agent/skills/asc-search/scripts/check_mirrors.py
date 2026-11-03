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

"""Check that asc-search has byte-identical files in devkit and knowledge checkouts."""

import argparse
import hashlib
import json
from pathlib import Path


def files(root):
    if not root.is_dir() or not (root / "SKILL.md").is_file():
        raise ValueError(f"missing skill directory or manifest: {root}")
    return {
        p.relative_to(root).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
        for p in root.rglob("*")
        if p.is_file()
        and "__pycache__" not in p.parts
        and p.suffix != ".pyc"
        and p.relative_to(root).parts[0] not in {"index", "tmp"}
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, help="asc-devkit checkout root; defaults to this script checkout")
    parser.add_argument("--target", type=Path, required=True, help="cannbot-knowledge checkout root")
    args = parser.parse_args()
    source = (args.source or Path(__file__).resolve().parents[4]) / ".agent/skills/asc-search"
    target = args.target / ".agents/skills/asc-search"
    try:
        if source.resolve() == target.resolve():
            raise ValueError("source and target must be different skill directories")
        left, right = files(source), files(target)
    except (ValueError, OSError) as exc:
        print(json.dumps({"status": "error", "error": str(exc)}))
        return 2
    delta = sorted(p for p in left.keys() | right.keys() if left.get(p) != right.get(p))
    errors = {"asc-search": delta} if delta else {}
    print(json.dumps({"status": "different" if errors else "identical", "differences": errors}))
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
