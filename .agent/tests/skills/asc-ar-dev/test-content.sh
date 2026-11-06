#!/usr/bin/env bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set -euo pipefail

TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${TEST_DIR}/../../../.." && pwd)"
SKILL_DIR="${REPO_ROOT}/.agent/skills/asc-ar-dev"
SKILL_FILE="${SKILL_DIR}/SKILL.md"
REFERENCES_DIR="${SKILL_DIR}/references"
AGENT_README="${REPO_ROOT}/.agent/README.md"

fail() {
    echo "[FAIL] $*" >&2
    exit 1
}

expect_fixed() {
    local file="$1"
    local text="$2"
    local label="$3"
    grep -Fq -- "$text" "$file" || fail "missing content: ${label}"
}

expect_pattern() {
    local file="$1"
    local pattern="$2"
    local label="$3"
    grep -Eq "$pattern" "$file" || fail "missing content: ${label}"
}

reject_tree_pattern() {
    local path="$1"
    local pattern="$2"
    local label="$3"
    if grep -REnI \
        --include='*.md' \
        --include='*.py' \
        --include='*.sh' \
        --include='*.json' \
        --exclude-dir='__pycache__' \
        -- "$pattern" "$path"; then
        fail "forbidden content: ${label}"
    fi
}

reject_tree_pattern "$SKILL_DIR" 'z[0-9]{8}|/home/[^/]+/|commit [0-9a-f]{7,40}|067fefc|0903eaa|references/09_aicore_printf_ringbuf\.md|TLV\(4,4,6\)|ssh -|git@gitcode|AKIA|PRIVATE KEY' \
    "personal path, commit record, missing reference, or private credential pattern"

expect_pattern "$SKILL_FILE" '^description:.*(当用户需要|触发).*AscendC' "description trigger condition"
expect_fixed "$SKILL_FILE" "../asc-npu-arch/SKILL.md" "asc-npu-arch boundary"
expect_fixed "$SKILL_FILE" "../asc-api-ut-gen/SKILL.md" "asc-api-ut-gen boundary"
expect_fixed "$SKILL_FILE" "must not maintain a duplicated full chip or dtype table" "no duplicated chip dtype table"
expect_fixed "$SKILL_FILE" "DEVKIT_PATH" "DEVKIT_PATH input"
expect_fixed "$SKILL_FILE" "CANN_PATH" "CANN_PATH input"
expect_fixed "$SKILL_FILE" "SOC_ARCH" "SOC_ARCH input"
expect_fixed "$SKILL_FILE" "TASK_GOAL" "TASK_GOAL input"
expect_fixed "$SKILL_FILE" "current workspace or this skill directory" "repository path derivation"
expect_fixed "$SKILL_FILE" "ASCEND_HOME_PATH" "ASCEND_HOME_PATH resolution"
expect_fixed "$SKILL_FILE" "ASCEND_TOOLKIT_HOME" "ASCEND_TOOLKIT_HOME resolution"
expect_fixed "$SKILL_FILE" "Never fall back to a hard-coded personal path" "hard-coded path ban"
expect_fixed "$SKILL_FILE" "New API Documentation Gate" "new API documentation gate"
expect_fixed "$SKILL_FILE" "Before writing or editing kernel code" "pre-code API lookup timing"
expect_fixed "$SKILL_FILE" "docs/zh/api/README.md" "API documentation root"
expect_fixed "$SKILL_FILE" "docs/zh/api/Ascend-C-API列表.md" "API category index"
expect_fixed "$SKILL_FILE" "Do not write the API call" "unverified API stop rule"
expect_fixed "$SKILL_FILE" "API evidence log" "API evidence log requirement"
expect_fixed "$SKILL_FILE" "Kernel API Scope" "kernel API scope definition"
expect_fixed "$SKILL_FILE" "call-site based" "kernel API call-site classification"
expect_fixed "$SKILL_FILE" "public devkit symbol" "kernel API public symbol definition"

for heading in "## 需求" "## 背景简述" "## 涉及领域" "## 方案详述"; do
    expect_fixed "$SKILL_FILE" "$heading" "output heading ${heading}"
done

expect_fixed "$SKILL_FILE" 'launch kernels only through `<<<>>>`' "host launch rule"
expect_fixed "$SKILL_FILE" 'must not call `__aicore__` functions directly' "host cannot call aicore"
expect_fixed "$SKILL_FILE" 'must not call another `__global__` function' "global cannot call global"
expect_fixed "$SKILL_FILE" 'must not call `__global__` functions' "aicore cannot call global"
expect_fixed "$SKILL_FILE" "basic API or high-level API" "API route"
expect_fixed "$SKILL_FILE" "compile-engineering" "compile engineering route"
expect_fixed "$SKILL_FILE" "vector pipeline" "vector pipeline route"
expect_fixed "$SKILL_FILE" "tiling or matmul" "tiling matmul route"
expect_fixed "$SKILL_FILE" "debug/dump/log" "debug dump route"
expect_fixed "$SKILL_FILE" ".ascend.meta" "ascend meta route"
expect_fixed "$SKILL_FILE" "TLV" "TLV route"
expect_fixed "$SKILL_FILE" "DFX" "DFX route"

expect_fixed "${REFERENCES_DIR}/README.md" "asc-ar-dev reference 导航" "reference navigation title"
expect_fixed "${REFERENCES_DIR}/README.md" "../../asc-npu-arch/SKILL.md" "reference README architecture boundary"
expect_fixed "${REFERENCES_DIR}/README.md" "../../asc-api-ut-gen/SKILL.md" "reference README UT boundary"
expect_fixed "${REFERENCES_DIR}/02_local_environment.md" "walking upward from the current workspace or this skill directory" "local environment devkit derivation"
expect_fixed "${REFERENCES_DIR}/02_local_environment.md" "build.sh" "build script discovery"
expect_fixed "${REFERENCES_DIR}/02_local_environment.md" "tests/test_parts.sh" "test parts discovery"
expect_fixed "${REFERENCES_DIR}/02_local_environment.md" "tests/**/CMakeLists.txt" "cmake discovery"
expect_fixed "${REFERENCES_DIR}/04_debug_dump.md" "AICore printf Ring-Buffer Review Notes" "printf ring buffer section"
expect_fixed "${REFERENCES_DIR}/04_debug_dump.md" 'Do not claim a new `.ascend.meta` TLV exists unless the current compiler code emits it' "no invented TLV rule"
expect_fixed "${REFERENCES_DIR}/06_task_goal_example.md" '中性的 `TASK_GOAL` 四段式输入样例' "neutral task goal example"
expect_fixed "${REFERENCES_DIR}/06_task_goal_example.md" "向量基础 API" "neutral vector API example"
expect_fixed "${REFERENCES_DIR}/07_requirement_type_routing.md" "Basic And High-Level API Requirements" "API routing reference"
expect_fixed "${REFERENCES_DIR}/07_requirement_type_routing.md" "AscendC Compile-Engineering Requirements" "compile routing reference"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "API Documentation Lookup Workflow" "API lookup workflow title"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "docs/zh/api/README.md" "API lookup documentation root"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "docs/zh/api/Ascend-C-API列表.md" "API lookup category index"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "产品支持情况" "API product support check"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "函数原型" "API prototype check"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "参数说明" "API parameter check"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "数据类型" "API dtype check"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "约束说明" "API constraints check"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "调用示例" "API example check"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "declaration, implementation, and non-test examples" "missing documentation fallback"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" 'do not use `test/` or `tests/` as product implementation evidence' "exclude test sources from API evidence"
expect_fixed "$SKILL_FILE" '../asc-code-dev-standards/SKILL.md' "product development standards integration"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "What Counts As A Kernel API" "kernel API scope section"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "include/basic_api" "basic kernel API scope"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "include/adv_api" "high-level kernel API scope"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "include/c_api" "C kernel API scope"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "include/simt_api" "SIMT kernel API scope"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "include/tensor_api" "Tensor kernel API scope"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "Host-side runtime APIs" "host API exclusion"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" 'Internal helpers under `impl/`' "internal implementation exclusion"
expect_fixed "${REFERENCES_DIR}/08_api_lookup.md" "treat it as a kernel API until classified" "ambiguous symbol rule"
expect_fixed "${REFERENCES_DIR}/03_devkit_snippets.md" "verify every newly introduced API" "snippet verification warning"

expect_fixed "$AGENT_README" '`asc-ar-dev`' "README skill listing"
expect_fixed "$AGENT_README" "bash .agent/tests/skills/asc-ar-dev/test.sh" "README single test command"
expect_fixed "$AGENT_README" "asc-ar-dev/references" "README fact source entry"

if grep -Fq '06_task_goal_debug_bus_example.md' "$SKILL_FILE" "${REFERENCES_DIR}/README.md"; then
    fail "skill must use neutral 06_task_goal_example.md instead of old debug-bus example name"
fi

if grep -Fq '**芯片架构：** `ascend910`, `ascend910b1`, `ascend310p`, `ascend610`, `ascend310b1`, `ascend950pr_9599`' "$SKILL_FILE"; then
    fail "asc-ar-dev must not keep a duplicated full chip list"
fi

echo "[PASS] asc-ar-dev skill content validation passed"
