#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Run focused 9201 synchronization checks; requires ASCEND_HOME_PATH.

Resource fixtures only enable compilation. They must never be used for execution
or interpreted as 9201 resource specifications.
"""

import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = """#include <cstdint>
namespace AscendC {
constexpr uint32_t TOTAL_UB_SIZE=0, TMP_UB_OFFSET=0, TOTAL_L1_SIZE=0, TOTAL_L0C_SIZE=0;
constexpr uint32_t TOTAL_VEC_LOCAL_SIZE=0, SINGLE_MSG_SIZE=0, CACHE_LINE_SIZE=0;
constexpr uint32_t RESERVED_WORKSPACE=0;
constexpr uint32_t ONE_BLOCK_SIZE=32, VECTOR_REG_WIDTH_2XVL=512, B64_DATA_NUM_PER_REPEAT=32;
}
"""


def run(command, cwd=ROOT):
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if result.returncode:
        print(result.stdout + result.stderr, file=sys.stderr)
        raise subprocess.CalledProcessError(result.returncode, command)
    return result.stdout + result.stderr


def main():
    cann = os.environ.get("ASCEND_HOME_PATH")
    if not cann:
        raise SystemExit("Set ASCEND_HOME_PATH to the CANN package containing bin/bisheng.")
    compiler = Path(cann).resolve() / "bin/bisheng"
    if not compiler.is_file():
        raise SystemExit(f"Compiler not found: {compiler}")
    with tempfile.TemporaryDirectory(prefix="asc-9201-sync-") as td:
        tmp = Path(td)
        generated = tmp / "generated"
        run(
            [
                sys.executable,
                str(ROOT / "impl/adv_api/cmake/scripts/gen_kernel_tiling_data_def.py"),
                str(ROOT / "include/adv_api"),
                str(generated / "kernel_tiling/kernel_tiling.h"),
            ]
        )
        includes = []
        for path in [ROOT, ROOT / "include", ROOT / "include/basic_api", ROOT / "include/utils", generated]:
            includes += ["-iquote", str(path)]
        flags = [str(compiler), "-x", "cce", "--npu-arch=dav-9201", "-std=c++17", *includes]
        units = [("reg_reduce", []), ("reg_cast", []), ("reg_datacopy", ["-DASCENDC_REG_SYNC_TEST_UB_SIZE_FIXTURE"])]
        units += [
            (unit, [f"--cce-ftz={ftz}"])
            for unit in ["unary_reg", "unary_membase", "div_reg"]
            for ftz in ["true", "false"]
        ]
        for unit, extra in units:
            src = ROOT / f"tests/compile/9201_sync_{unit}.cpp"
            output = run([*flags, "--cce-aicore-only", "-fsyntax-only", *extra, str(src)], cwd=tmp)
            warnings = len(re.findall(r"\bwarning:", output))
            print(f"PASS {src.name} {' '.join(extra)} ({warnings} compiler warnings)", flush=True)
        fixture = tmp / "fixture.h"
        fixture.write_text(FIXTURE)
        src = ROOT / "tests/python/asc_op_compiler/9201_sync_membase.asc"
        for preload in [True, False]:
            name = "preload" if preload else "no_preload"
            probe = tmp / (name + ".asc")
            text = src.read_text()
            probe.write_text(text if preload else text.replace("AscendC::ICachePreLoad(2);", ""))
            run(
                [*flags, "-include", str(fixture), "-c", "-save-temps", str(probe), "-o", str(tmp / (name + ".o"))],
                cwd=tmp,
            )
            devices = list(tmp.glob(name + "-cce-*-vec.o")) + list(tmp.glob(name + "-cce-*-cube.o"))
            assert devices, "No device ELF emitted"
            found = False
            for obj in devices:
                result = subprocess.run(["readelf", "-x", ".ascend.meta", str(obj)], capture_output=True, text=True)
                assert result.returncode == 0, result.stderr
                found |= bool(re.search(r"07000400\s+01000000", result.stdout))
            assert found == preload, (name, found)
            print(f"PASS {name} device ELF: icache type7 TLV {'present' if found else 'absent'}", flush=True)
        for script in [
            "tests/compile/9201_sync_shared.py",
            "tests/compile/9201_sync_unary_precision.py",
            "tests/python/asc_op_compiler/9201_sync_membase.py",
        ]:
            args = ["--cxx", str(compiler)] if script.endswith("unary_precision.py") else []
            print(run([sys.executable, str(ROOT / script), *args]).strip(), flush=True)
        print(f"PASS {len(units)} syntax checks, 2 device builds/TLV controls, 3 host regression scripts")


if __name__ == "__main__":
    main()
