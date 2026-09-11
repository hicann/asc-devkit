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
SKILL_DIR="${SKILL_DIR:-${REPO_ROOT}/.agent/skills/asc-api-ut-gen}"
SKILL_FILE="${SKILL_DIR}/SKILL.md"
REFERENCES_DIR="${SKILL_DIR}/references"
GENERATOR="${SKILL_DIR}/scripts/ut_generator.py"
CLI="${SKILL_DIR}/scripts/ut_generator_cli.py"

fail() {
    echo "[FAIL] $*" >&2
    exit 1
}

expect_pattern() {
    local file="$1"
    local pattern="$2"
    local label="$3"

    grep -Eq "$pattern" "$file" || fail "missing content: ${label}"
}

expect_fixed() {
    local file="$1"
    local text="$2"
    local label="$3"

    grep -Fq -- "$text" "$file" || fail "missing content: ${label}"
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

reject_tree_pattern "$SKILL_DIR" 'z[0-9]{8}|/home/[^/]+/asc-devkit' "personal workspace path or employee id"

expect_pattern "$SKILL_FILE" '^description:.*(当用户需要|触发).*' "description trigger condition"
expect_pattern "$SKILL_FILE" '^description:.*(UT|单元测试|覆盖率).*' "description keywords"
expect_fixed "$SKILL_FILE" "本技能使用 Python 脚本自动生成 UT 代码" "Python generator statement"
expect_fixed "$SKILL_FILE" "首次使用环境配置" "first-use environment gate"
expect_fixed "$SKILL_FILE" "CANN 包安装路径" "CANN package path requirement"
expect_fixed "$SKILL_FILE" "asc-devkit 仓路径不再向用户询问" "asc-devkit repository path auto detection"
expect_fixed "$SKILL_FILE" "当前 workspace 或本 skill 所在目录向上定位仓根" "asc-devkit repository root derivation"
expect_fixed "$SKILL_FILE" "推导出的 \`ASC_DEVKIT_PATH\` 必须记录到执行日志中" "asc-devkit path derivation execution log"
expect_fixed "$SKILL_FILE" "仅询问 CANN 包安装路径" "CANN-only environment question"
expect_fixed "$SKILL_FILE" "不询问用户是否允许生成或写入 UT" "no generation permission question"
expect_fixed "${REFERENCES_DIR}/workflows/coverage-scan-guide.md" "ASC_DEVKIT_PATH 已从当前 workspace 或 skill 所在仓推导" "coverage scan repo path derivation"
expect_fixed "${REFERENCES_DIR}/workflows/coverage-report-backfill-guide.md" "从当前 workspace 或 skill 所在仓推导 ASC_DEVKIT_PATH" "coverage report repo path derivation"

expect_fixed "$SKILL_FILE" "Git 扫描模式" "git scan mode"
expect_fixed "$SKILL_FILE" "精确交互模式" "precise interaction mode"
expect_fixed "$SKILL_FILE" "覆盖率扫描模式" "coverage scan mode"
expect_fixed "$SKILL_FILE" "覆盖率报告补齐模式" "coverage report backfill mode"
expect_fixed "$SKILL_FILE" '```mermaid' "workflow mermaid diagram"
expect_fixed "$SKILL_FILE" "flowchart TD" "workflow flowchart"
expect_fixed "$SKILL_FILE" "将推导出的 ASC_DEVKIT_PATH 记录到执行日志" "first-use environment mermaid log node"
expect_fixed "$SKILL_FILE" "Step 3 调用生成脚本" "UT generation mermaid step"
expect_fixed "$SKILL_FILE" "编译和测试通过？" "build/test loop decision"
expect_fixed "$SKILL_FILE" "覆盖率达到阈值？" "coverage threshold loop decision"
expect_fixed "$SKILL_FILE" "失败或覆盖率不足时必须进入图中的回路" "workflow loop requirement"
expect_fixed "${REFERENCES_DIR}/foundations/branch-coverage-guide.md" "Step 5 生成分支覆盖矩阵" "branch coverage mermaid flow"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "TestOk" "automation mermaid test decision"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "达到最大重试次数？" "automation repair retry loop"
expect_fixed "${REFERENCES_DIR}/workflows/coverage-scan-guide.md" "Step 3 跨文件内容搜索，必须执行" "coverage scan mermaid cross-file flow"
expect_fixed "${REFERENCES_DIR}/workflows/coverage-report-backfill-guide.md" "覆盖率达到 95%？" "coverage report mermaid backfill loop"
expect_fixed "$SKILL_FILE" "<芯片版本> <API类型> <API名称> [核心类型]" "shared parameter sequence"
expect_fixed "$SKILL_FILE" '$asc-api-ut-gen <芯片版本> <API类型> <API名称> [核心类型]' "codex command format"
expect_fixed "$SKILL_FILE" "OpenCode" "opencode invocation section"
expect_fixed "$SKILL_FILE" "Claude Code" "claude invocation section"
expect_fixed "$SKILL_FILE" ".claude/commands/" "claude wrapper boundary"
expect_fixed "$SKILL_FILE" "/asc-api-ut-gen <芯片版本> <API类型> <API名称> [核心类型]" "slash example format"
expect_fixed "$SKILL_FILE" "build/cov_report" "coverage report path"

expect_fixed "$SKILL_FILE" "高阶API" "advanced API coverage"
expect_fixed "$SKILL_FILE" "membase基础API" "membase API coverage"
expect_fixed "$SKILL_FILE" "regbase基础API" "regbase API coverage"
expect_fixed "$SKILL_FILE" "C API" "C API coverage"
expect_fixed "$SKILL_FILE" "SIMT API" "SIMT API coverage"
expect_fixed "$SKILL_FILE" "工具类API" "utils API coverage"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "Attribute 与结构体参数" "C API attribute struct section"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "必须读取代码中的真实结构体定义和调用链" "C API struct source rule"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "attribute、attr、params、config、mode" "C API attribute variants"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "\`__cbuf__\`" "C API cbuf pointer marker"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "\`__fbuf__\`" "C API fbuf pointer marker"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "本表不是固定全集" "C API pointer marker list not exhaustive"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "\`__aicore__\`、\`__simd_callee__\` 等函数限定符不是指针类型标记" "C API function qualifier is not pointer marker"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "impl/c_api/{memory_base_impl,reg_base_impl}/**" "C API impl struct source"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "底层 stub 签名" "C API stub signature source"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "不能只验证本 guide 中列出的常见字段" "C API no fixed attribute list"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "Stub 已逐项断言所有 attribute/结构体字段" "C API struct field assertion checklist"
expect_fixed "$SKILL_FILE" "../asc-npu-arch/SKILL.md" "npu arch source link"
expect_fixed "$SKILL_FILE" "统一事实来源" "single source statement"
expect_fixed "$SKILL_FILE" "本 skill 不再单独维护完整芯片类型表" "chip table centralization"
expect_fixed "$SKILL_FILE" "统一数据类型视图" "dtype source statement"

expect_pattern "$SKILL_FILE" 'regbase.*ascend950pr_9599' "regbase architecture restriction"
expect_pattern "$SKILL_FILE" 'SIMT API.*ascend950pr_9599' "SIMT architecture restriction"

if grep -Fq '**芯片架构：** `ascend910`, `ascend910b1`, `ascend310p`, `ascend610`, `ascend310b1`, `ascend950pr_9599`' "$SKILL_FILE"; then
    fail "asc-api-ut-gen skill must not keep a duplicated full chip list"
fi

if grep -Fq '**数据类型：** `half`, `float`, `int8_t`, `int16_t`, `int32_t`, `uint8_t`, `uint16_t`, `uint32_t`, `bfloat16_t`' "$SKILL_FILE"; then
    fail "asc-api-ut-gen skill must not keep a duplicated full dtype list"
fi

if grep -REq '^\|[[:space:]]*数据类型[[:space:]]*\|[[:space:]]*sizeof[[:space:]]*\|[[:space:]]*(32B 对齐元素数|元素对齐要求)' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen references must not keep duplicated dtype size tables"
fi

if grep -REq '^\|[[:space:]]*数据类型[[:space:]]*\|[[:space:]]*(说明|sizeof[[:space:]]*\|[[:space:]]*对齐要求)[[:space:]]*\|' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen references must not keep duplicated generic dtype tables"
fi

if grep -REq '^\|[[:space:]]*参数类型[[:space:]]*\|[[:space:]]*常见组合[[:space:]]*\|' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen references must not keep duplicated common dtype combination tables"
fi

if grep -REq '支持类型:[[:space:]]*half,[[:space:]]*float|[,{][[:space:]]*[0-9]+[[:space:]]*,[[:space:]]*kernel_func_(half|float)' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen references must not hard-code generic dtype support or dtype sizes in templates"
fi

if grep -REq --exclude='test-templates.md' '^## [0-9]+\. 测试模板示例|^### [0-9]+\.[0-9]+ (基础|参数化)测试模板' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen scenario references must link test-templates.md instead of duplicating template sections"
fi

if grep -REq --exclude='automation-guide.md' 'source \{CANN_PATH\}/set_env\.sh|bash build\.sh|--gtest_filter' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen scenario references must link automation-guide.md instead of duplicating build/test commands"
fi

if grep -REq '数据类型大小来源|float \(4 bytes\)|half \(2 bytes\)|count %[[:space:]]*[0-9]+[[:space:]]*==' "$REFERENCES_DIR"; then
    fail "asc-api-ut-gen references must not duplicate dtype size or alignment facts"
fi

expect_fixed "$SKILL_FILE" "scripts/ut_generator_cli.py" "CLI generator reference"
expect_fixed "$SKILL_FILE" "scripts/ut_generator.py" "core generator reference"
expect_fixed "$SKILL_FILE" "分支覆盖分析指南" "branch coverage guide"
expect_fixed "$SKILL_FILE" "覆盖率报告补齐指南" "coverage report backfill guide"
expect_fixed "$SKILL_FILE" "常见问题汇总" "FAQ guide"
expect_fixed "${REFERENCES_DIR}/README.md" "目录划分" "reference directory navigation"
expect_fixed "${REFERENCES_DIR}/README.md" "api-guides/" "API guide reference folder"
expect_fixed "${REFERENCES_DIR}/README.md" "workflows/" "workflow reference folder"
expect_fixed "${REFERENCES_DIR}/README.md" "foundations/" "foundation reference folder"
expect_fixed "${REFERENCES_DIR}/README.md" "troubleshooting/" "troubleshooting reference folder"
expect_fixed "${REFERENCES_DIR}/README.md" "api-directory-map.md" "API directory map reference"
expect_fixed "${REFERENCES_DIR}/README.md" "generation-constraints.json" "generation constraints reference"
expect_fixed "${REFERENCES_DIR}/foundations/api-directory-map.md" "API 目录映射表" "API directory map title"
expect_fixed "${REFERENCES_DIR}/foundations/api-directory-map.md" "tests/api/reg_compute_api/" "reg compute test mapping"
expect_fixed "${REFERENCES_DIR}/foundations/api-directory-map.md" "impl/c_api/reg_base_impl/" "C API implementation mapping"
expect_fixed "${REFERENCES_DIR}/foundations/api-directory-map.md" "ascendc_case_ascend950pr_9599_aiv_basic" "950 AIV basic test mapping"
if grep -Eq 'Tensor API|tensor_api|experimental/tensor_api|tests/api/tensor_api' "${REFERENCES_DIR}/foundations/api-directory-map.md"; then
    fail "current asc-api-ut-gen skill must ignore Tensor API mappings"
fi
expect_fixed "${REFERENCES_DIR}/foundations/test-templates.md" "只维护跨 API 可复用的 UT 骨架" "template responsibility boundary"
expect_fixed "${REFERENCES_DIR}/foundations/test-templates.md" "模板职责边界" "template ownership table"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"api_restrictions\"" "structured API restrictions"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"generic_binary_apis\"" "structured AIV generic binary allowlist"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"generic_scalar_tensor_dispatch_apis\"" "structured AIV scalar/tensor dispatch allowlist"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"generic_mmad_like_apis\"" "structured AIC MMAD-like allowlist"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"reg_generation\"" "structured regbase generation boundary"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"simt_generation\"" "structured SIMT generation boundary"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"c_api_generation\"" "structured C API generation boundary"
expect_fixed "${REFERENCES_DIR}/foundations/generation-constraints.json" "\"utils_generation\"" "structured utils generation boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/membase-api-aiv-ut-guide.md" "不要把所有 AIV API 都套成 \`src0 + src1 -> dst\` 的 binary 模板" "AIV non-binary template rule"
expect_fixed "${REFERENCES_DIR}/api-guides/membase-api-aiv-ut-guide.md" "不要直接按 \`DuplicateImpl<TensorTrait<T>>\` 注册 mock" "AIV TensorTrait mock boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/membase-api-aiv-ut-guide.md" "selector / masked multi-source" "AIV selector family boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/membase-api-aiv-ut-guide.md" "\`Select\` 是另一类常见的非 binary API" "AIV Select boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/membase-api-aic-ut-guide.md" "不要把非 MMAD-like API 仅靠改类名套进 MMAD 模板" "AIC MMAD boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/regbase-api-ut-guide.md" "通用生成器不得输出带 \`TODO\` 的" "regbase placeholder boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/simt-api-ut-guide.md" "通用生成器不得输出带 \`TODO\` 的" "SIMT placeholder boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/c-api-ut-guide.md" "不能把单一" "C API signature boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/adv-api-ut-guide.md" "\`adv_profile\` 是**精确源文件复制机制**" "ADV profile copy boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/adv-api-ut-guide.md" "如果 \`source == output\`" "ADV deleted source boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/utils-api-ut-guide.md" "\`include/basic_api/**\` 和 \`impl/basic_api/**\` 下的 helper" "utils category boundary"
expect_fixed "${REFERENCES_DIR}/api-guides/utils-api-ut-guide.md" "某个 utils family 的可执行模板" "utils template boundary"

if grep -Eq 'class Kernel\{ApiName\}|class Test\{ApiName\}CAPI|KERNEL_FIXPIPE' "${REFERENCES_DIR}/foundations/test-templates.md"; then
    fail "test-templates.md must not duplicate API-specific AIC/C API code blocks"
fi

for token in \
    "ApiType = Enum" \
    "ChipArch = Enum" \
    "class TestCase" \
    "class UTConfig" \
    "ARCH_DIR_MAP" \
    "NPU_ARCH_MAP" \
    "DTYPE_MAP" \
    "GENERATOR_DTYPE_MAP" \
    "npu-arch-facts.json" \
    "generation-constraints.json" \
    "def create_generator"; do
    expect_fixed "$GENERATOR" "$token" "generator token ${token}"
done

if grep -Eq 'DTYPE_MAP[[:space:]]*=[[:space:]]*\{|ARCH_DIR_MAP[[:space:]]*=[[:space:]]*\{|NPU_ARCH_MAP[[:space:]]*=[[:space:]]*\{' "$GENERATOR"; then
    fail "generator must load chip and dtype facts from structured references"
fi

for token in \
    "def validate_config" \
    "def parse_config" \
    "def get_output_path" \
    "def get_token_usage" \
    "def parse_coverage_report" \
    "def print_generation_report" \
    "def main" \
    "argparse.ArgumentParser" \
    "GENERATOR_DTYPE_MAP" \
    "--config" \
    "--type" \
    "--api" \
    "--chip" \
    "--coverage-report" \
    "--report-json" \
    "--list-supported"; do
    expect_fixed "$CLI" "$token" "CLI token ${token}"
done

expect_fixed "$SKILL_FILE" "token 消耗、总耗时、当前覆盖率" "post generation metrics"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "Token 消耗" "automation token report"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "当前覆盖率" "automation coverage report"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "tests/test_parts.sh" "automation test part source"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "不能作为唯一事实来源" "automation reference is not sole source"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "不能只依赖当前 reference" "automation current scripts before reference"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "执行日志必须记录已查看的 \`build.sh\`、\`tests/test_parts.sh\` 和相关 \`CMakeLists.txt\` 路径" "automation checked scripts log"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "按当前脚本/CMake 执行" "automation current script cmake priority"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "basic_test_five" "automation c/reg/simt part"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "header-only public surface" "automation header-only basic impl rule"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "impl/basic_api/**/*.h" "automation basic impl header glob"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "ascendc_run_all_header_checks" "automation header checker target"
expect_fixed "${REFERENCES_DIR}/workflows/coverage-report-backfill-guide.md" "header-only 实现" "coverage header-only validation rule"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "CMake 到 build.sh part 闭环" "automation cmake to build part closure"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "run_llt_test" "automation run_llt_test closure"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "run_python_llt_test" "automation run_python_llt_test closure"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "TEST_MOD" "automation TEST_MOD rule"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "CMake target 未纳入 build.sh 分片" "automation uncovered cmake target rule"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "Tensor API 除外" "automation tensor api exclusion"
expect_fixed "${REFERENCES_DIR}/workflows/coverage-report-backfill-guide.md" "Tensor API 除外" "coverage tensor api exclusion"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "adv_test_two" "automation adv 3510 part"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "ascendc_ut_c_api_ascend950pr_9599_AIV" "automation c api target"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "ascendc_ut_reg_compute_ascend950pr_9599" "automation regbase target"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "ascendc_ut_simt_api_ascend950pr_9599" "automation simt target"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "tests/api/aicpu_api/CMakeLists.txt" "automation aicpu cmake target"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "ascendc_ut_aicpu_api" "automation aicpu part"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "ascendc_pyut_asc_op_compiler" "automation python part"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "bash build.sh -t -j8" "automation full test fallback"
expect_fixed "${REFERENCES_DIR}/workflows/automation-guide.md" "0 tests" "automation gtest zero-test guard"
expect_fixed "$SKILL_FILE" '`build.sh`、`tests/test_parts.sh` 和 `tests/**/CMakeLists.txt`' "skill build strategy source"
expect_fixed "$SKILL_FILE" 'tests/**/CMakeLists.txt' "skill cmake strategy source"
expect_fixed "$SKILL_FILE" "Tensor API 除外" "skill tensor api exclusion"

echo "[PASS] asc-api-ut-gen skill content validation passed"
