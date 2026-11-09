#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Exercise actual shared check/message bodies on host; no NPU emulation.

--baseline reads audit baseline 2488a6f1d to demonstrate failures before this synchronization.
"""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def source(path, baseline):
    if baseline:
        return subprocess.check_output(["git", "show", f"2488a6f1d:{path}"], cwd=ROOT, text=True)
    return (ROOT / path).read_text()


def assertions(text):
    """Read two macro arguments, respecting parentheses and quoted strings."""
    for match in re.finditer(r"\bASCENDC_ASSERT\s*\(", text):
        start = pos = match.end()
        depth, quote, escaped, comma = 1, None, False, None
        while pos < len(text):
            c = text[pos]
            if quote:
                if escaped:
                    escaped = False
                elif c == "\\":
                    escaped = True
                elif c == quote:
                    quote = None
            elif c in ('"', "'"):
                quote = c
            elif c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
                if depth == 0:
                    if comma is not None:
                        yield text[comma + 1 : pos].strip()
                    break
            elif c == "," and depth == 1 and comma is None:
                comma = pos
            pos += 1


AFFECTED_MESSAGES = {
    "impl/basic_api/dav_9201/kernel_operator_data_copy_impl.h": [
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_bilinearinterpolation_impl.h": [
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_binary_scalar_impl.h": [
        ('"mask must be set when sizeof(T) is 1."',),
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_createvecindex_impl.h": [
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_duplicate_impl.h": [
        ('"mask must be set when sizeof(T) is 1."',),
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_gather_mask_impl.h": [
        ('"GatherMask Pattern can only be 1~7"',),
        ('"unsupported GetGatherMaskRemainCount on current device"',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_template_impl.h": [
        ('"mask must be set when sizeof(T) is 1."',),
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
        ('"unsupported dtype on current device!"',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_ternary_scalar_impl.h": [
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_unary_impl.h": [
        ('"mask must be set when sizeof(T) is 1."',),
        ('"maskArray must be nullptr when isMaskBitMode is false."',),
        ('"maskCount must be 0 when isMaskBitMode is true."',),
    ],
    "impl/basic_api/dav_9201/kernel_operator_vec_vpadding_impl.h": [
        ('"VectorPadding is not supported on current device"',)
    ],
}


def cpu_message_cases(baseline):
    cases = []
    for target, expected_keys in AFFECTED_MESSAGES.items():
        payloads = list(assertions(source(target, baseline)))
        keys = set(expected_keys)
        found = set()
        for payload in payloads:
            key = tuple(re.findall(r'"(?:\\.|[^"\\])*"', payload))
            if key in keys:
                cases.append(payload)
                found.add(key)
        assert found == keys, (target, keys - found)
    # Use the actual CPU UT macro; only its abort/log sinks are instrumented.
    log = source("impl/basic_api/kernel_log.h", baseline)
    macro = re.search(r"#define ASCENDC_ASSERT\(cond, behavior\).*?while \(0\)", log, re.S).group()
    prelude = """
#include <csignal>
int logs = 0, raises = 0;
namespace AscendC { struct KernelRaise {
 static KernelRaise& GetInstance() { static KernelRaise k; return k; }
 void Raise(int) { ++raises; }
}; }
#define KERNEL_LOG(...) (++logs)
"""
    body = [prelude, macro, "int main() {"]
    for i, payload in enumerate(cases):
        body += [
            f"logs=0; raises=0; ASCENDC_ASSERT(false, {payload});",
            f"if (logs != 1 || raises != 1) return {i + 1};",
        ]
    body += ["return 0; }"]
    return "\n".join(body), len(cases)


def transpose_cases(baseline):
    text = source("impl/basic_api/kernel_operator_vec_transpose_intf_impl.h", baseline)
    start = text.index("template <typename T>\n__aicore__ inline void CheckTransDataTo5HDParams")
    end = text.index("\n}\n", start) + 3
    body = text[start:end]
    prelude = """
#include <cstdint>
#include <type_traits>
#define __NPU_ARCH__ 9201
#define __aicore__
struct half {}; struct bfloat16_t {};
template<class T> using PrimT = T;
template<class T, class... U> constexpr bool SupportType() { return (std::is_same_v<T, U> || ...); }
struct TransDataTo5HDParams { bool dstHighHalf=false, srcHighHalf=false; int repeatTimes=1; };
#define KERNEL_LOG_INTERNAL(...) 0
int failures=0;
#define ASCENDC_DEBUG_ASSERT(c, ...) do { if (!(c)) ++failures; } while (0)
template<class T> void CheckValueRange(T x, T lo, T hi, const char*, const char*) {
 if (x<lo || x>hi) ++failures;
}
"""
    checks = """
int main() {
 TransDataTo5HDParams p;
 CheckTransDataTo5HDParams<bfloat16_t>(p);
 if (failures != 0) return 1;
 CheckTransDataTo5HDParams<half>(p);
 if (failures != 0) return 2;
 CheckTransDataTo5HDParams<double>(p);
 if (failures != 1) return 3;
 failures=0; p.dstHighHalf=true;
 CheckTransDataTo5HDParams<bfloat16_t>(p);
 if (failures != 1) return 4;
 failures=0; p.srcHighHalf=true; p.repeatTimes=256;
 CheckTransDataTo5HDParams<uint8_t>(p);
 return failures != 1;
}
"""
    return prelude + body + checks, 5


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", action="store_true")
    args = parser.parse_args()
    failures = 0
    with tempfile.TemporaryDirectory(prefix="asc-sync-shared-") as tmp:
        for name, make in [("cpu_messages", cpu_message_cases), ("transpose_checks", transpose_cases)]:
            code, count = make(args.baseline)
            cpp = Path(tmp) / (name + ".cpp")
            exe = Path(tmp) / name
            cpp.write_text(code)
            subprocess.run(["g++", "-std=c++17", str(cpp), "-o", str(exe)], check=True)
            result = subprocess.run([str(exe)])
            print(f"{name}: {count} cases, exit {result.returncode}", flush=True)
            failures += result.returncode != 0
    return int(failures != 0)


if __name__ == "__main__":
    raise SystemExit(main())
