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
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


class OrchestrateShellTest(unittest.TestCase):
    def base_env(self, root: Path, **overrides: str) -> dict[str, str]:
        env = {
            **os.environ,
            "PATH": f"{root / 'bin'}{os.pathsep}{os.environ.get('PATH', '')}",
            "REAL_PYTHON": shutil.which("python3") or "python3",
            "NPU_CARD_DEV_GLOB": str(root / "dev/davinci[0-9]*"),
        }
        env.update(overrides)
        return env

    def copy_orchestrate_fixture(self, root: Path) -> Path:
        project_root = Path(__file__).resolve().parents[3]
        script = root / "scripts/run_presmoke.sh"
        script.parent.mkdir(parents=True)
        shutil.copy(project_root / "scripts/run_presmoke.sh", script)
        script.chmod(0o755)

        presmoke_pkg = root / "scripts/presmoke"
        presmoke_pkg.mkdir(parents=True)
        shutil.copy(
            project_root / "scripts/presmoke/orchestrate_report.py",
            presmoke_pkg / "orchestrate_report.py",
        )
        (presmoke_pkg / "__init__.py").write_text("", encoding="utf-8")

        fake_bin = root / "bin"
        fake_bin.mkdir()
        fake_python = fake_bin / "python3"
        fake_python.write_text(
            """#!/usr/bin/env bash
set -euo pipefail
if [[ "${1:-}" != "-m" || "${2:-}" != "presmoke" ]]; then
    exec "${REAL_PYTHON}" "$@"
fi
shift 2
results=""
dry_run=0
stages="all"
filters=()
while [[ $# -gt 0 ]]; do
    case "$1" in
        --results)
            results="$2"
            shift 2
            ;;
        --dry-run)
            dry_run=1
            shift
            ;;
        --stages)
            stages="$2"
            shift 2
            ;;
        --stages=*)
            stages="${1#--stages=}"
            shift
            ;;
        --filter)
            filters+=("$2")
            shift 2
            ;;
        --filter=*)
            filters+=("${1#--filter=}")
            shift
            ;;
        --exact-filter)
            filters+=("$2")
            shift 2
            ;;
        --exact-filter=*)
            filters+=("${1#--exact-filter=}")
            shift
            ;;
        *)
            shift
            ;;
    esac
done
mkdir -p "$results"
run_file="$(dirname "$results")/run_env.txt"
{
    echo "device=${ASCEND_RT_VISIBLE_DEVICES:-}"
    printf 'filters=%s\\n' "${filters[*]:-}"
    echo "dry_run=$dry_run"
    echo "stages=$stages"
} > "$run_file"
if [[ "$dry_run" == "1" ]]; then
cat > "$results/report.json" <<'JSON'
{"summary":{"PASS":0,"FAIL":0,"SKIP":5},"results":[
{"example":"case/a","arch":"dav-2201","mode":"npu","status":"SKIP","reason":"dry-run","rc":0,"duration_s":0,"steps":[]},
{"example":"case/b","arch":"dav-2201","mode":"npu","status":"SKIP","reason":"dry-run","rc":0,"duration_s":0,"steps":[]},
{"example":"case/c","arch":"dav-2201","mode":"npu","status":"SKIP","reason":"dry-run","rc":0,"duration_s":0,"steps":[]},
{"example":"case/d","arch":"dav-2201","mode":"npu","status":"SKIP","reason":"dry-run","rc":0,"duration_s":0,"steps":[]},
{"example":"case/e","arch":"dav-2201","mode":"npu","status":"SKIP","reason":"dry-run","rc":0,"duration_s":0,"steps":[]}
],"npu_stats":{"busy_s":0,"idle_s":0,"utilization":0}}
JSON
elif [[ "${PRESMOKE_STUB_RC:-0}" == "0" ]]; then
python_args=("$results/report.json")
if [[ "${#filters[@]}" -gt 0 ]]; then
    python_args+=("${filters[@]}")
fi
"${REAL_PYTHON}" - "${python_args[@]}" <<'PY'
import json
import sys

report = sys.argv[1]
filters = sys.argv[2:]
results = [
    {
        "example": item,
        "arch": "dav-2201",
        "mode": "npu",
        "status": "PASS",
        "reason": "",
        "rc": 0,
        "duration_s": 1,
        "steps": [],
    }
    for item in filters
]
with open(report, "w", encoding="utf-8") as handle:
    json.dump(
        {
            "summary": {"PASS": len(results), "FAIL": 0, "SKIP": 0},
            "results": results,
            "npu_stats": {"busy_s": 9, "idle_s": 1, "utilization": 0.9},
        },
        handle,
    )
PY
else
cat > "$results/report.json" <<'JSON'
{
  "summary": {"PASS": 0, "FAIL": 1, "SKIP": 0},
  "results": [
    {
      "example": "stub/fail",
      "arch": "dav-2201",
      "mode": "npu",
      "status": "FAIL",
      "reason": "nonzero rc",
      "rc": 1,
      "duration_s": 1,
      "steps": []
    }
  ],
  "npu_stats": {"busy_s": 0, "idle_s": 0, "utilization": 0}
}
JSON
fi
cat > "$results/report.md" <<'MD'
# stub
MD
exit "${PRESMOKE_STUB_RC:-0}"
""",
            encoding="utf-8",
        )
        fake_python.chmod(0o755)
        fake_npu_smi = fake_bin / "npu-smi"
        fake_npu_smi.write_text(
            """#!/usr/bin/env bash
cat <<'EOF'
+----+------+------+
| 0  | 910B | OK   |
| 1  | 910B | OK   |
| 2  | 910B | OK   |
EOF
""",
            encoding="utf-8",
        )
        fake_npu_smi.chmod(0o755)
        return script

    def test_full_run_cleans_cann_vendors_before_presmoke(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            vendors = root / "opp/vendors"
            stale = vendors / "stale_vendor"
            stale.mkdir(parents=True)
            (stale / "old.txt").write_text("old", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
                NPU_CARDS="0",
            )

            result = subprocess.run(
                ["bash", str(script)],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertFalse(stale.exists())
            self.assertIn("vendors_clean_done", result.stdout)
            self.assertIn("Presmoke Summary:", result.stdout)
            self.assertIn("Elapsed:", result.stdout)
            self.assertIn("Cases: total=0 pass=0 fail=0 skip=0", result.stdout)
            self.assertIn("npu: busy=9.000s idle=1.000s util=90.000%", result.stdout)
            self.assertIn("execute samples success", result.stdout)
            self.assertTrue((root / "out/junit.xml").exists())

    def test_default_output_root_is_presmoke_reports(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                ARCH="dav-2201",
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--dry-run"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn(
                f"out_root={root / 'presmoke_reports' / 'presmoke_dav-2201_'}",
                result.stdout,
            )
            self.assertFalse((root / "presmoke_results").exists())

    def test_cpu_mode_uses_default_schedule(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                MODES="cpu",
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--dry-run"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("schedule=default", result.stdout)

    def test_mode_alias_enables_cpu_mode(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                MODE="cpu",
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--dry-run"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("modes=cpu", result.stdout)
            self.assertIn("schedule=default", result.stdout)

    def test_filtered_run_does_not_clean_cann_vendors(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            vendors = root / "opp/vendors"
            stale = vendors / "stale_vendor"
            stale.mkdir(parents=True)
            (stale / "old.txt").write_text("old", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--filter", "x"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(stale.exists())
            self.assertIn("vendors_clean_skip reason=non_full_run", result.stdout)

    def test_dry_run_does_not_clean_cann_vendors(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            vendors = root / "opp/vendors"
            stale = vendors / "stale_vendor"
            stale.mkdir(parents=True)
            (stale / "old.txt").write_text("old", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--dry-run"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(stale.exists())
            self.assertIn("vendors_clean_skip reason=non_full_run", result.stdout)

    def test_failed_presmoke_keeps_legacy_failure_marker(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
                PRESMOKE_STUB_RC="1",
                NPU_CARDS="0",
            )

            result = subprocess.run(
                ["bash", str(script)],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("Presmoke Summary:", result.stdout)
            self.assertIn("Cases: total=1 pass=0 fail=1 skip=0", result.stdout)
            self.assertIn("execute samples failed", result.stderr)

    def test_default_npu_cards_shards_cases_across_detected_cards(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script)],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue((root / "out/full_card0/results/report.json").exists())
            self.assertTrue((root / "out/full_card1/results/report.json").exists())
            self.assertTrue((root / "out/full_card2/results/report.json").exists())
            self.assertFalse((root / "out/.workspaces").exists())
            self.assertFalse((root / "out/.plan").exists())
            self.assertFalse((root / "out/.state").exists())
            self.assertIn("multi_card_start cards=0 1 2 cases=5", result.stdout)
            self.assertIn(
                f"project_root={root}",
                (root / "out/full_card0/meta.txt")
                .read_text(encoding="utf-8")
                .splitlines(),
            )
            self.assertIn(
                "device=0",
                (root / "out/full_card0/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "device=1",
                (root / "out/full_card1/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "device=2",
                (root / "out/full_card2/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "filters=case/a case/d",
                (root / "out/full_card0/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "filters=case/b case/e",
                (root / "out/full_card1/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "filters=case/c",
                (root / "out/full_card2/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn("Cases: total=5 pass=5 fail=0 skip=0", result.stdout)

    def test_matching_fixed_shards_override_dynamic_balancing(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            shards = root / "scripts/presmoke/schedules/shards/dav-2201_npu_3cards"
            shards.mkdir(parents=True)
            (shards / "card_0.txt").write_text("case/e\n", encoding="utf-8")
            (shards / "card_1.txt").write_text("case/d\ncase/c\n", encoding="utf-8")
            (shards / "card_2.txt").write_text("case/b\ncase/a\n", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script)],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("multi_card_fixed_shards", result.stdout)
            self.assertIn(
                "filters=case/e",
                (root / "out/full_card0/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "filters=case/d case/c",
                (root / "out/full_card1/run_env.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "filters=case/b case/a",
                (root / "out/full_card2/run_env.txt").read_text(encoding="utf-8"),
            )

    def test_build_only_uses_single_run_and_forwards_stage(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--stages", "build"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue((root / "out/full_card0").exists())
            self.assertFalse((root / "out/full_multi").exists())
            run_env = (root / "out/full_card0/run_env.txt").read_text(encoding="utf-8")
            self.assertIn("stages=build", run_env)

    def test_explicit_all_stages_keeps_multi_card_execution(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--stages", "all"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("multi_card_start cards=0 1 2 cases=5", result.stdout)

    def test_failed_run_cleans_transient_plan_and_state(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            (root / "out").mkdir()
            (root / "out/.plan").mkdir()
            (root / "out/.state").mkdir()
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
                PRESMOKE_STUB_RC="1",
                NPU_CARDS="0",
            )

            result = subprocess.run(
                ["bash", str(script)],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 1)
            self.assertTrue((root / "out/FINAL_REPORT.md").exists())
            self.assertFalse((root / "out/.plan").exists())
            self.assertFalse((root / "out/.state").exists())

    def test_auto_npu_cards_prefers_davinci_devices_over_npu_smi(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            dev_dir = root / "dev"
            dev_dir.mkdir()
            (dev_dir / "davinci4").touch()
            (dev_dir / "davinci6").touch()
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
                NPU_CARD_DEV_GLOB=str(dev_dir / "davinci[0-9]*"),
            )

            result = subprocess.run(
                ["bash", str(script)],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue((root / "out/full_card4/results/report.json").exists())
            self.assertTrue((root / "out/full_card6/results/report.json").exists())
            self.assertFalse((root / "out/full_card0/results/report.json").exists())
            self.assertIn("multi_card_start cards=4 6 cases=5", result.stdout)

    def test_single_detected_npu_card_is_used_for_single_run(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            dev_dir = root / "dev"
            dev_dir.mkdir()
            (dev_dir / "davinci7").touch()
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
                NPU_CARD_DEV_GLOB=str(dev_dir / "davinci[0-9]*"),
            )

            result = subprocess.run(
                ["bash", str(script), "--filter", "case/a"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue((root / "out/full_card7/results/report.json").exists())
            self.assertFalse((root / "out/full_card0/results/report.json").exists())
            self.assertIn(
                "device=7",
                (root / "out/full_card7/run_env.txt").read_text(encoding="utf-8"),
            )

    def test_make_wrapper_limits_explicit_parallel_jobs(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            fake_bin = root / "bin"
            fake_bin.mkdir()
            capture = root / "args.txt"
            fake_make = fake_bin / "make"
            fake_make.write_text(
                f"#!/usr/bin/env bash\nprintf '%s\\n' \"$@\" > {capture}\n",
                encoding="utf-8",
            )
            fake_make.chmod(0o755)
            project_root = Path(__file__).resolve().parents[3]
            wrapper = project_root / "scripts/presmoke/wrappers/make"
            env = {
                **os.environ,
                "PATH": f"{wrapper.parent}{os.pathsep}{fake_bin}{os.pathsep}{os.environ.get('PATH', '')}",
                "PRESMOKE_MAKE_JOBS": "16",
            }

            result = subprocess.run(
                [str(wrapper), "-j", "binary", "package"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(
                capture.read_text(encoding="utf-8").splitlines(),
                ["-j16", "binary", "package"],
            )

            result = subprocess.run(
                [str(wrapper), "-j", "8", "binary"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(
                capture.read_text(encoding="utf-8").splitlines(), ["-j16", "binary"]
            )


if __name__ == "__main__":
    unittest.main()
