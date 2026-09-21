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

from presmoke.case_runners import CaseRunnerOptions, build_case_runner_cells_with_skips


class WerrorTest(unittest.TestCase):
    def test_case_runner_sets_werror_env_when_enabled(self) -> None:
        project_root = Path(__file__).resolve().parents[3]

        cells, _, _ = build_case_runner_cells_with_skips(
            project_root,
            CaseRunnerOptions(
                arch="dav-2201",
                modes=["npu"],
                includes=["01_simd_cpp_api/00_introduction/00_quickstart/hello_world"],
                excludes=[],
                werror=True,
            ),
        )

        self.assertEqual(len(cells), 1)
        for command in cells[0].commands:
            self.assertEqual(command.env.get("PRESMOKE_WERROR"), "1")

    def test_case_common_werror_cmake_wrapper_reaches_bash_lc(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_common = project_root / "scripts/presmoke/case_common.sh"

        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            fake_bin = tmp_path / "bin"
            fake_bin.mkdir()
            cmake_log = tmp_path / "cmake_args.log"
            cmake = fake_bin / "cmake"
            cmake.write_text(
                "#!/usr/bin/env bash\n"
                'printf \'args=%s\\n\' "$*" >> "$PRESMOKE_CMAKE_LOG"\n'
                'printf \'env=%s|%s\\n\' "${CFLAGS:-}" "${CXXFLAGS:-}" >> "$PRESMOKE_CMAKE_LOG"\n',
                encoding="utf-8",
            )
            cmake.chmod(0o755)

            script = f"""
set -euo pipefail
source {case_common}
export PATH={fake_bin}:$PATH
export PRESMOKE_WERROR=1
export PRESMOKE_CMAKE_LOG={cmake_log}
bash -lc 'cmake ..'
bash -lc 'cmake --build build -j'
"""
            env = os.environ.copy()
            env.pop("PRESMOKE_WERROR", None)
            result = subprocess.run(["bash", "-c", script], env=env, text=True, capture_output=True, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            lines = cmake_log.read_text(encoding="utf-8").splitlines()
            self.assertEqual(len(lines), 4)
            self.assertNotIn("CMAKE_C_FLAGS", lines[0])
            self.assertNotIn("CMAKE_CXX_FLAGS", lines[0])
            self.assertIn("-DCMAKE_ASC_FLAGS=-Werror", lines[0])
            self.assertEqual(lines[1], "env=-Werror|-Werror")
            self.assertEqual(lines[2], "args=--build build -j")
            self.assertEqual(lines[3], "env=|")


if __name__ == "__main__":
    unittest.main()
