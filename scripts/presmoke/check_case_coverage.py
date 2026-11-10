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
"""校验 examples/ 与 scripts/presmoke/cases/ 两边对齐。

presmoke 的事实源是 scripts/presmoke/cases/**/run.sh：strict-schedule 比对的是
「清单 ↔ 已存在的 run.sh」，而 run.sh 由 generate_case_runners.py 从 examples 的
README 生成。两者之间原先没有任何检查，于是一个样例只要没被生成出 run.sh，它对所有
门禁都不存在 —— 曾因此让 63 个声明支持 Ascend 950PR&950DT 的样例从未被看护过。
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

from .discover import discover_examples
from .generate_case_runners import HAND_WRITTEN_CASES

SCENARIO_SUFFIX = re.compile(r"__scenario_\d+$")


def main() -> int:
    parser = argparse.ArgumentParser(description="Check examples/ vs scripts/presmoke/cases/ consistency")
    parser.add_argument("--project-root", default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    root = Path(args.project_root).resolve()
    examples_root = root / "examples"
    cases_root = root / "scripts" / "presmoke" / "cases"

    examples = {path.relative_to(examples_root).as_posix() for path in discover_examples(examples_root)}
    cases = {path.parent.relative_to(cases_root).as_posix() for path in cases_root.rglob("run.sh")}
    # 一个样例可以被展开成多个 __scenario_N case，按去掉后缀后的路径归并。
    case_sources = {SCENARIO_SUFFIX.sub("", case) for case in cases}

    missing_runner = sorted(examples - case_sources)
    orphan_case = sorted(c for c in case_sources if c not in examples and c not in HAND_WRITTEN_CASES)

    errors = []
    if missing_runner:
        errors.append(
            f"examples 有「编译运行」段但没有 run.sh（{len(missing_runner)} 个），"
            f"跑一次 python3 -m presmoke.generate_case_runners 即可生成：\n  " + "\n  ".join(missing_runner)
        )
    if orphan_case:
        errors.append(
            f"run.sh 在 examples 下找不到对应样例（{len(orphan_case)} 个）。手工维护的 case "
            f"请加进 generate_case_runners.py 的 HAND_WRITTEN_CASES：\n  " + "\n  ".join(orphan_case)
        )

    if errors:
        print("case coverage mismatch:", file=sys.stderr)
        for item in errors:
            print(f"- {item}", file=sys.stderr)
        return 1
    print(f"case coverage ok: examples={len(examples)} cases={len(cases)} sources={len(case_sources)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
