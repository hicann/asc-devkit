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

import json
import tempfile
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path

from presmoke.orchestrate_report import write_final_report


class OrchestrateJunitTest(unittest.TestCase):
    def test_write_final_report_generates_junit_for_pass_fail_and_skip(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            run_dir = root / "full_card0"
            results_dir = run_dir / "results"
            logs_dir = results_dir / "logs"
            stage_dir = logs_dir / "stages"
            stage_dir.mkdir(parents=True)
            case_log = logs_dir / "pass.log"
            fail_stage_log = stage_dir / "fail_run.log"
            case_log.write_text("case log with invalid xml char \x00 and pass details\n", encoding="utf-8")
            fail_stage_log.write_text("stage fail log\n", encoding="utf-8")
            (run_dir / "meta.txt").write_text(
                "\n".join(
                    [
                        "card=0",
                        "arch=dav-3510",
                        "modes=npu",
                        "schedule=fixed",
                        "jobs=8",
                        "npu_slots=1",
                        "cpu_run_slots=88",
                        "make_jobs=11",
                        "timeout=120",
                        "cpu_run_timeout=300",
                        "elapsed_sec=12",
                    ]
                )
                + "\n",
                encoding="utf-8",
            )
            (results_dir / "report.json").write_text(
                json.dumps(
                    {
                        "host_arch": "dav-3510",
                        "modes": ["npu"],
                        "summary": {"PASS": 1, "FAIL": 1, "SKIP": 1},
                        "npu_stats": {"busy_s": 10, "idle_s": 1, "utilization": 0.9},
                        "parallel_config": {"jobs": 8, "npu_slots": 1},
                        "results": [
                            {
                                "example": "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_ibshareAB",
                                "arch": "dav-3510",
                                "mode": "npu",
                                "status": "PASS",
                                "reason": "",
                                "rc": 0,
                                "duration_s": 1.25,
                                "log_file": str(case_log),
                                "stage_log_files": [str(stage_dir / "missing.log")],
                                "steps": [
                                    {"kind": "build", "command": "bash run.sh build", "rc": 0, "duration_s": 1.0},
                                    {
                                        "kind": "run",
                                        "command": "bash run.sh run",
                                        "rc": 0,
                                        "duration_s": 0.2,
                                        "wait_s": 0.1,
                                        "npu_slot": True,
                                    },
                                ],
                            },
                            {
                                "example": "03_simt_api/02_features/00_framework/00_pytorch/torch_library_roll",
                                "arch": "dav-3510",
                                "mode": "npu",
                                "status": "FAIL",
                                "reason": "nonzero rc",
                                "failing_step": "bash run.sh build",
                                "rc": 2,
                                "duration_s": 2.5,
                                "log_file": str(logs_dir / "missing_fail.log"),
                                "stage_log_files": [str(fail_stage_log)],
                                "steps": [
                                    {"kind": "build", "command": "bash run.sh build", "rc": 2, "duration_s": 2.5}
                                ],
                            },
                            {
                                "example": "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin",
                                "arch": "dav-3510",
                                "mode": "npu",
                                "status": "SKIP",
                                "reason": "requires TensorFlow 2.6.5 environment; skipped by presmoke",
                                "rc": None,
                                "duration_s": 0,
                                "log_file": "",
                                "stage_log_files": [],
                                "steps": [],
                            },
                        ],
                    },
                    ensure_ascii=False,
                ),
                encoding="utf-8",
            )

            write_final_report(root)

            junit_path = root / "junit.xml"
            self.assertTrue(junit_path.exists())
            tree = ET.parse(junit_path)
            suite = tree.getroot().find("testsuite")
            self.assertIsNotNone(suite)
            assert suite is not None
            self.assertEqual(suite.attrib["name"], "presmoke.dav-3510.npu")
            self.assertEqual(suite.attrib["tests"], "3")
            self.assertEqual(suite.attrib["failures"], "1")
            self.assertEqual(suite.attrib["skipped"], "1")

            cases = {case.attrib["name"]: case for case in suite.findall("testcase")}
            pass_case = cases["matmul_ibshareAB [npu]"]
            self.assertEqual(pass_case.attrib["classname"], "01_simd_cpp_api.04_advanced_api.00_matmul")
            self.assertIsNone(pass_case.find("failure"))
            self.assertIsNone(pass_case.find("skipped"))
            pass_out = pass_case.findtext("system-out") or ""
            self.assertIn("bash run.sh build", pass_out)
            self.assertIn("case log with invalid xml char", pass_out)
            self.assertNotIn("\x00", pass_out)
            self.assertIn("missing.log (missing)", pass_out)

            fail_case = cases["torch_library_roll [npu]"]
            failure = fail_case.find("failure")
            self.assertIsNotNone(failure)
            assert failure is not None
            self.assertIn("nonzero rc", failure.attrib["message"])
            self.assertIn("rc=2", failure.attrib["message"])
            self.assertIn("bash run.sh build", failure.text or "")
            self.assertIn("stage fail log", fail_case.findtext("system-out") or "")

            skip_case = cases["tensorflow_builtin [npu]"]
            skipped = skip_case.find("skipped")
            self.assertIsNotNone(skipped)
            assert skipped is not None
            self.assertIn("TensorFlow 2.6.5", skipped.attrib["message"])


if __name__ == "__main__":
    unittest.main()
