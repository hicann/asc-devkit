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

import os
import subprocess
import tempfile
import unittest
from pathlib import Path


class RunPresmokeParallelTest(unittest.TestCase):
    """直接驱动 run_presmoke.sh 里的 resolve_multi_card_shard_parallel。

    脚本末尾是 `main "$@"`，不能直接 source，所以把这一行之前的部分当函数库用。
    顶层只有一堆 `${VAR:-default}` 赋值和一次 mkdir，指到临时目录即可。
    """

    def _resolve(self, cards: int, **env_overrides: str) -> list[str]:
        project_root = Path(__file__).resolve().parents[3]
        script = (project_root / "scripts/run_presmoke.sh").read_text(encoding="utf-8")
        library, sep, _ = script.rpartition('\nmain "$@"')
        self.assertTrue(sep, 'run_presmoke.sh 结尾不再是 main "$@"，这个测试的切法要跟着改')

        with tempfile.TemporaryDirectory() as tmp:
            env = {
                "PATH": os.environ.get("PATH", "/usr/bin:/bin"),
                "PROJECT_ROOT": tmp,
                "OUT_ROOT": str(Path(tmp) / "reports"),
                **env_overrides,
            }
            result = subprocess.run(
                ["bash", "-c", f"{library}\nresolve_multi_card_shard_parallel {cards}\n"],
                env=env,
                capture_output=True,
                text=True,
                check=False,
            )
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout.strip().split("\t")

    def test_invalid_jobs_is_clamped_in_the_emitted_value(self) -> None:
        # JOBS 来自环境。它既是 shard_make_jobs 的除数（0 会让 set -e 下整个 presmoke 中止），
        # 又是输出的第一列。兜底只兜局部变量是不够的 —— 输出那一列如果再过一遍
        # resolve_shard_parallel_value，原始 $JOBS 会被原样吐回去，兜底值就白做了。
        for jobs in ("0", "abc", "-1", ""):
            with self.subTest(jobs=jobs):
                shard_jobs, make_jobs, _run_slots, per_card_cpus = self._resolve(4, JOBS=jobs)
                self.assertGreaterEqual(int(shard_jobs), 1, f"JOBS={jobs!r} 没被兜住")
                self.assertGreaterEqual(int(make_jobs), 1)
                self.assertGreaterEqual(int(per_card_cpus), 1)

    def test_explicit_jobs_is_honoured(self) -> None:
        shard_jobs, _make_jobs, _run_slots, _per_card_cpus = self._resolve(4, JOBS="7")
        self.assertEqual(shard_jobs, "7", "合法的 JOBS 不该被兜底逻辑改掉")


if __name__ == "__main__":
    unittest.main()
