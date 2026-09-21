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

"""
AscendC API UT 代码生成器

该模块提供 UT 代码生成的核心功能，支持：
- AIV (Vector 核心) 基础 API 测试
- AIC (Cube 核心) 基础 API 测试
- C API 测试
- 高阶 API 测试
- SIMT API 测试
- Reg Compute API 测试

库使用方式:
    from ut_generator import ApiType, ChipArch, TestCase, UTConfig, create_generator

    config = UTConfig(api_type=ApiType.AIV, api_name="Add", chip=ChipArch.ASCEND910B1)
    generator = create_generator(config)
    code = generator.generate()

命令行调用:
    python ut_generator_cli.py --type aiv --api Add --chip ascend910b1
"""

import json
import re
from dataclasses import dataclass, field
from enum import Enum
from functools import lru_cache
from pathlib import Path
from string import Template
from typing import List, Optional, Dict, Any


SCRIPT_DIR = Path(__file__).resolve().parent
SKILL_DIR = SCRIPT_DIR.parent
SKILLS_DIR = SKILL_DIR.parent
REPO_ROOT = SKILL_DIR.parents[2]
NPU_ARCH_FACTS_PATH = SKILLS_DIR / "asc-npu-arch" / "references" / "npu-arch-facts.json"
GENERATION_CONSTRAINTS_PATH = SKILL_DIR / "references" / "foundations" / "generation-constraints.json"
REFERENCE_CONSTRAINT_FILES = {
    "npu_arch_facts": str(NPU_ARCH_FACTS_PATH),
    "generation_constraints": str(GENERATION_CONSTRAINTS_PATH),
}


def _load_reference_json(path: Path) -> Dict[str, Any]:
    """Load a structured reference file used as a generator constraint source."""
    if not path.is_file():
        raise FileNotFoundError(f"missing structured reference: {path}")
    with path.open("r", encoding="utf-8") as f:
        return json.load(f)


_REFERENCE_LOAD_ERRORS: Dict[str, Exception] = {}


@lru_cache(maxsize=None)
def _load_reference_json_safe(name: str, path: Path) -> Dict[str, Any]:
    """Load a structured reference without making module import fail."""
    try:
        data = _load_reference_json(path)
    except Exception as exc:
        _REFERENCE_LOAD_ERRORS[name] = exc
        return {}

    _REFERENCE_LOAD_ERRORS.pop(name, None)
    return data


def get_npu_arch_facts() -> Dict[str, Any]:
    """Return NPU architecture facts from the structured reference."""
    return _load_reference_json_safe("npu_arch_facts", NPU_ARCH_FACTS_PATH)


def get_generation_constraints() -> Dict[str, Any]:
    """Return UT generation constraints from the structured reference."""
    return _load_reference_json_safe("generation_constraints", GENERATION_CONSTRAINTS_PATH)


def ensure_reference_constraints_loaded() -> None:
    """Raise a CLI-friendly error if structured references were unavailable."""
    if not _REFERENCE_LOAD_ERRORS:
        return

    details = "; ".join(f"{name}: {exc}" for name, exc in sorted(_REFERENCE_LOAD_ERRORS.items()))
    raise RuntimeError(f"failed to load structured references: {details}")


def _enum_member_name(value: str) -> str:
    """Convert a reference value to a stable enum member name."""
    return re.sub(r"[^A-Z0-9]+", "_", value.upper()).strip("_")


def _enum_members_from_reference(facts: Dict[str, Any], fallback: Dict[str, str]) -> Dict[str, str]:
    """Build Enum members from a reference object, keeping import usable on failure."""
    members = {
        fact.get("enum_name", _enum_member_name(item_name)): item_name
        for item_name, fact in facts.items()
        if isinstance(fact, dict)
    }
    return members or fallback


NPU_ARCH_FACTS = get_npu_arch_facts()
GENERATION_CONSTRAINTS = get_generation_constraints()
CHIP_FACTS = NPU_ARCH_FACTS.get("chips", {})
DTYPE_FACTS = NPU_ARCH_FACTS.get("dtypes", {})
API_TYPE_FACTS = GENERATION_CONSTRAINTS.get("api_types", {})
AIV_GENERATION_FACTS = GENERATION_CONSTRAINTS.get("aiv_generation", {})
AIC_GENERATION_FACTS = GENERATION_CONSTRAINTS.get("aic_generation", {})
REG_GENERATION_FACTS = GENERATION_CONSTRAINTS.get("reg_generation", {})
SIMT_GENERATION_FACTS = GENERATION_CONSTRAINTS.get("simt_generation", {})
C_API_GENERATION_FACTS = GENERATION_CONSTRAINTS.get("c_api_generation", {})
UTILS_GENERATION_FACTS = GENERATION_CONSTRAINTS.get("utils_generation", {})


ApiType = Enum("ApiType", _enum_members_from_reference(API_TYPE_FACTS, {"UNAVAILABLE": "unavailable"}))


API_TYPE_MAP = {api_type.value: api_type for api_type in ApiType}
API_TYPE_CHOICES = list(API_TYPE_MAP.keys())
API_RESTRICTIONS = GENERATION_CONSTRAINTS.get("api_restrictions", {})
AIV_GENERIC_BINARY_APIS = frozenset(name.lower() for name in AIV_GENERATION_FACTS.get("generic_binary_apis", []))
AIV_GENERIC_SCALAR_TENSOR_DISPATCH_APIS = frozenset(
    name.lower() for name in AIV_GENERATION_FACTS.get("generic_scalar_tensor_dispatch_apis", [])
)
AIV_EXPLICIT_TEMPLATES = frozenset(AIV_GENERATION_FACTS.get("explicit_templates", []))
AIC_GENERIC_MMAD_LIKE_APIS = frozenset(name.lower() for name in AIC_GENERATION_FACTS.get("generic_mmad_like_apis", []))
AIC_EXPLICIT_TEMPLATES = frozenset(AIC_GENERATION_FACTS.get("explicit_templates", []))
REG_GENERIC_TEMPLATES = frozenset(REG_GENERATION_FACTS.get("generic_templates", []))
SIMT_GENERIC_TEMPLATES = frozenset(SIMT_GENERATION_FACTS.get("generic_templates", []))
C_API_GENERIC_TEMPLATES = frozenset(C_API_GENERATION_FACTS.get("generic_templates", []))
UTILS_GENERIC_TEMPLATES = frozenset(UTILS_GENERATION_FACTS.get("generic_templates", []))


ChipArch = Enum("ChipArch", _enum_members_from_reference(CHIP_FACTS, {"UNAVAILABLE": "unavailable"}))
CHIP_ARCH_BY_NAME = {chip.value: chip for chip in ChipArch}
ARCH_DIR_MAP = dict((CHIP_ARCH_BY_NAME[chip_name], fact["arch_dir"]) for chip_name, fact in CHIP_FACTS.items())
NPU_ARCH_MAP = dict((CHIP_ARCH_BY_NAME[chip_name], str(fact["npu_arch"])) for chip_name, fact in CHIP_FACTS.items())
SOC_VERSION_MAP = dict((CHIP_ARCH_BY_NAME[chip_name], fact["soc_version"]) for chip_name, fact in CHIP_FACTS.items())
DTYPE_MAP = dict(
    (
        dtype_name,
        {
            **fact,
            "cpp_type": fact.get("cpp_type", dtype_name),
            "size": int(fact["size"]),
            "align32_elements": int(fact["align32_elements"]),
        },
    )
    for dtype_name, fact in DTYPE_FACTS.items()
)
GENERATOR_DTYPE_MAP = dict(
    (dtype_name, fact) for dtype_name, fact in DTYPE_MAP.items() if fact.get("generic_ut_generation", False)
)


@dataclass
class TestCase:
    """测试用例配置"""

    name: str  # 测试用例名称
    data_size: int = 256  # 数据大小
    dtype: str = "half"  # 数据类型
    input_count: int = 1  # 输入数量
    has_mask: bool = False  # 是否有 mask
    additional_params: Dict[str, Any] = field(default_factory=dict)


@dataclass
class UTConfig:
    """UT 生成配置"""

    api_type: ApiType
    api_name: str
    chip: ChipArch
    test_cases: List[TestCase] = field(default_factory=list)
    custom_includes: List[str] = field(default_factory=list)
    kernel_params: Dict[str, Any] = field(default_factory=dict)
    output_dir: Optional[str] = None


@dataclass(frozen=True)
class AdvApiProfile:
    """Executable high-level API UT profile backed by an existing source UT."""

    source: str
    output: str


def normalize_adv_api_name(api_name: str) -> str:
    """Normalize high-level API names for executable profile lookup."""
    return re.sub(r"[^a-z0-9]+", "", api_name.lower())


def _load_adv_profile_entry(api_name: str, raw_profile: Any) -> AdvApiProfile:
    """Create an executable Adv profile from the explicit per-run config."""
    if not isinstance(raw_profile, dict):
        raise ValueError(f"invalid ADV profile for {api_name!r}: expected object")

    source = raw_profile.get("source")
    output = raw_profile.get("output", source)
    if not isinstance(source, str) or not source:
        raise ValueError(f"invalid ADV profile for {api_name!r}: missing source")
    if not isinstance(output, str) or not output:
        raise ValueError(f"invalid ADV profile for {api_name!r}: missing output")

    return AdvApiProfile(source=source, output=output)


def get_adv_api_profile(api_name: str, kernel_params: Dict[str, Any]) -> Optional[AdvApiProfile]:
    """Return an executable high-level API profile explicitly supplied by this run."""
    kernel_params = kernel_params or {}
    profile = kernel_params.get("adv_profile", {})
    if not profile:
        return None
    return _load_adv_profile_entry(normalize_adv_api_name(api_name), profile)


def get_adv_profile_output_path(api_name: str, kernel_params: Dict[str, Any]) -> Optional[str]:
    """Return the output path for an explicitly supplied high-level API profile."""
    profile = get_adv_api_profile(api_name, kernel_params)
    return profile.output if profile is not None else None


def normalize_generated_cpp(code: str) -> str:
    """Normalize generated C++ text without changing semantic content."""
    normalized = "\n".join(line.rstrip() for line in code.splitlines())
    if code.endswith("\n"):
        normalized += "\n"
    header_match = re.match(r"\A/\*\*.*?\*/\n*", normalized, re.DOTALL)
    header = header_match.group(0) if header_match else ""
    if "Copyright (c)" in header and "Huawei Technologies Co., Ltd." in header:
        normalized = TEMPLATES["copyright"] + "\n" + normalized[header_match.end() :]
    return normalized


def render_gtest_values_instantiation(api_name: str, api_name_upper: str, test_params: List[str]) -> str:
    """Render gtest value instantiation in the repository clang-format style."""
    testsuite = f"{api_name}Testsuite"
    if len(test_params) == 1:
        return (
            f"INSTANTIATE_TEST_CASE_P(\n    TEST_{api_name_upper}, {testsuite}, ::testing::Values({test_params[0]}));"
        )

    values = ",\n        ".join(test_params)
    return (
        "INSTANTIATE_TEST_CASE_P(\n"
        f"    TEST_{api_name_upper}, {testsuite},\n"
        f"    ::testing::Values(\n"
        f"        {values}));"
    )


# =============================================================================
# 内置模板定义
# =============================================================================

TEMPLATES = {
    # -------------------------------------------------------------------------
    # 版权声明
    # -------------------------------------------------------------------------
    "copyright": """/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
""",
    # -------------------------------------------------------------------------
    # AIV (Vector 核心) API 模板
    # -------------------------------------------------------------------------
    "aiv_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include <vector>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

${KERNEL_CLASS}

${MAIN_FUNCTION}

${INIT_FUNCTION}

${PARAM_STRUCT}

${TEST_CLASS}

${INSTANTIATION}

${TEST_CASE}
""",
    "aiv_kernel_class": """template <${TEMPLATE_PARAMS}>
class Kernel${API_NAME} {
public:
    __aicore__ inline Kernel${API_NAME}() {}

    __aicore__ inline void Init(
        __gm__ uint8_t* dstGm, __gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, uint32_t dataSize)
    {
        this->dataSize = dataSize;

        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);
        src0Global.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(src0Gm), dataSize);
        src1Global.SetGlobalBuffer(reinterpret_cast<__gm__ Src1T*>(src1Gm), dataSize);

        pipe.InitBuffer(inQueueSrc0, 1, dataSize * sizeof(T));
        pipe.InitBuffer(inQueueSrc1, 1, dataSize * sizeof(Src1T));
        pipe.InitBuffer(outQueueDst, 1, dataSize * sizeof(T));
    }

    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<T> src0Local = inQueueSrc0.AllocTensor<T>();
        LocalTensor<Src1T> src1Local = inQueueSrc1.AllocTensor<Src1T>();
        DataCopy(src0Local, src0Global, dataSize);
        DataCopy(src1Local, src1Global, dataSize);
        inQueueSrc0.EnQue(src0Local);
        inQueueSrc1.EnQue(src1Local);
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<T> src0Local = inQueueSrc0.DeQue<T>();
        LocalTensor<Src1T> src1Local = inQueueSrc1.DeQue<Src1T>();
        LocalTensor<T> dstLocal = outQueueDst.AllocTensor<T>();

        ${API_CALL}

        outQueueDst.EnQue<T>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
        inQueueSrc1.FreeTensor(src1Local);
    }

    __aicore__ inline void CopyOut()
    {
        LocalTensor<T> dstLocal = outQueueDst.DeQue<T>();
        DataCopy(dstGlobal, dstLocal, dataSize);
        outQueueDst.FreeTensor(dstLocal);
    }

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueSrc0;
    TQue<TPosition::VECIN, 1> inQueueSrc1;
    TQue<TPosition::VECOUT, 1> outQueueDst;
    GlobalTensor<T> dstGlobal;
    GlobalTensor<T> src0Global;
    GlobalTensor<Src1T> src1Global;
    uint32_t dataSize;
};""",
    "aiv_main_function": """template <typename T, typename Src1T = T>
__aicore__ inline void main_${API_NAME}(uint8_t* dstGm, uint8_t* src0Gm, uint8_t* src1Gm, uint32_t dataSize)
{
    Kernel${API_NAME}<T, Src1T> op;
    op.Init(dstGm, src0Gm, src1Gm, dataSize);
    op.Process();
}""",
    "aiv_init_function": """template <typename T, typename Src1T = T>
void Init${API_NAME}Inputs(uint8_t* src0Gm, uint8_t* src1Gm, uint32_t dataSize)
{
    T* src0 = reinterpret_cast<T*>(src0Gm);
    Src1T* src1 = reinterpret_cast<Src1T*>(src1Gm);
    for (uint32_t i = 0; i < dataSize; i++) {
        src0[i] = static_cast<T>(i % 256);
        src1[i] = static_cast<Src1T>((i + 1) % 256);
    }
}""",
    "aiv_param_struct": """struct ${API_NAME}TestParams {
    uint32_t data_size;
    uint32_t data_bit_size;
    void (*cal_func)(uint8_t*, uint8_t*, uint8_t*, uint32_t);
    void (*init_func)(uint8_t*, uint8_t*, uint32_t);
};""",
    "aiv_test_class": (
        """class ${API_NAME}Testsuite : public testing::Test, """
        """public testing::WithParamInterface<${API_NAME}TestParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};"""
    ),
    "aiv_instantiation": """${INSTANTIATION}""",
    "aiv_test_case": """TEST_P(${API_NAME}Testsuite, ${API_NAME}TestCase)
{
    auto param = GetParam();

    // 分配 GM 内存
    std::vector<uint8_t> dstGm(param.data_size * param.data_bit_size, 0);
    std::vector<uint8_t> src0Gm(param.data_size * param.data_bit_size, 0);
    std::vector<uint8_t> src1Gm(param.data_size * param.data_bit_size, 0);

    // 初始化输入数据
    param.init_func(src0Gm.data(), src1Gm.data(), param.data_size);

    // 执行测试
    param.cal_func(dstGm.data(), src0Gm.data(), src1Gm.data(), param.data_size);

    // 验证结果
    for (uint32_t i = 0; i < param.data_size; i++) {
        // TODO: 添加实际验证逻辑
        // EXPECT_NEAR(...);
    }
}""",
    "aiv_scalar_tensor_dispatch_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include <vector>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

${KERNEL_CLASS}

${MAIN_FUNCTION}

${PARAM_STRUCT}

${TEST_CLASS}

${INSTANTIATION}

${TEST_CASE}
""",
    "aiv_scalar_tensor_dispatch_kernel_class": """template <typename T>
class Kernel${API_NAME} {
public:
    __aicore__ inline void Init(__gm__ uint8_t* srcGm, __gm__ uint8_t* dstGm, uint32_t dataSize)
    {
        this->dataSize = dataSize;

        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ PrimT<T>*>(srcGm), dataSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ PrimT<T>*>(dstGm), dataSize);

        pipe.InitBuffer(inQueueSrc, 1, dataSize * sizeof(PrimT<T>));
        pipe.InitBuffer(outQueueDst, 1, dataSize * sizeof(PrimT<T>));
    }

    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<T> srcLocal = inQueueSrc.AllocTensor<T>();
        DataCopy(srcLocal, srcGlobal, dataSize);
        inQueueSrc.EnQue(srcLocal);
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<T> srcLocal = inQueueSrc.DeQue<T>();
        LocalTensor<T> dstLocal = outQueueDst.AllocTensor<T>();
        PrimT<T> scalarValue = srcLocal.GetValue(0);

        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);

        if constexpr (IsSameType<PrimT<T>, T>::value) {
            ${RAW_API_CALL}
        } else {
            ${TENSOR_API_CALL}
        }

        outQueueDst.EnQue<T>(dstLocal);
        inQueueSrc.FreeTensor(srcLocal);
    }

    __aicore__ inline void CopyOut()
    {
        LocalTensor<T> dstLocal = outQueueDst.DeQue<T>();
        DataCopy(dstGlobal, dstLocal, dataSize);
        outQueueDst.FreeTensor(dstLocal);
    }

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueSrc;
    TQue<TPosition::VECOUT, 1> outQueueDst;
    GlobalTensor<T> srcGlobal;
    GlobalTensor<T> dstGlobal;
    uint32_t dataSize;
};""",
    "aiv_scalar_tensor_dispatch_main_function": """template <typename T>
__aicore__ inline void main_${API_NAME}(uint8_t* srcGm, uint8_t* dstGm, uint32_t dataSize)
{
    Kernel${API_NAME}<T> op;
    op.Init(srcGm, dstGm, dataSize);
    op.Process();
}""",
    "aiv_scalar_tensor_dispatch_param_struct": """struct ${API_NAME}TestParams {
    uint32_t data_size;
    uint32_t data_bit_size;
    void (*cal_func)(uint8_t*, uint8_t*, uint32_t);
};""",
    "aiv_scalar_tensor_dispatch_test_class": (
        """class ${API_NAME}Testsuite : public testing::Test, """
        """public testing::WithParamInterface<${API_NAME}TestParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};"""
    ),
    "aiv_scalar_tensor_dispatch_instantiation": """${INSTANTIATION}""",
    "aiv_scalar_tensor_dispatch_test_case": """TEST_P(${API_NAME}Testsuite, ${API_NAME}TestCase)
{
    auto param = GetParam();

    std::vector<uint8_t> srcGm(param.data_size * param.data_bit_size, 0);
    std::vector<uint8_t> dstGm(param.data_size * param.data_bit_size, 0);

    param.cal_func(srcGm.data(), dstGm.data(), param.data_size);

    EXPECT_EQ(dstGm[0], 0x00);
}""",
    # -------------------------------------------------------------------------
    # AIC (Cube 核心) API 模板
    # -------------------------------------------------------------------------
    "aic_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

${KERNEL_CLASS}

${TEST_CLASS}

${TEST_CASE}
""",
    "aic_kernel_class": """template <typename Src0T, typename Src1T, typename DstT, typename L1OutT>
class Kernel${API_NAME} {
public:
    __aicore__ inline Kernel${API_NAME}() {}

    __aicore__ inline void Init(
        __gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* c, uint16_t mVal, uint16_t kVal, uint16_t nVal)
    {
        this->m = mVal;
        this->k = kVal;
        this->n = nVal;

        aGM.SetGlobalBuffer(reinterpret_cast<__gm__ Src0T*>(a));
        bGM.SetGlobalBuffer(reinterpret_cast<__gm__ Src1T*>(b));
        cGM.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(c));

        pipe.InitBuffer(inQueueA1, 1, m * k * sizeof(Src0T));
        pipe.InitBuffer(inQueueB1, 1, k * n * sizeof(Src1T));
        pipe.InitBuffer(outQueueCO1, 1, m * n * sizeof(L1OutT));
    }

    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<Src0T> a1Local = inQueueA1.AllocTensor<Src0T>();
        LocalTensor<Src1T> b1Local = inQueueB1.AllocTensor<Src1T>();
        DataCopy(a1Local, aGM, m * k);
        DataCopy(b1Local, bGM, k * n);
        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<Src0T> a1Local = inQueueA1.DeQue<Src0T>();
        LocalTensor<Src1T> b1Local = inQueueB1.DeQue<Src1T>();
        LocalTensor<L1OutT> co1Local = outQueueCO1.AllocTensor<L1OutT>();

        ${API_CALL}

        outQueueCO1.EnQue<L1OutT>(co1Local);
        inQueueA1.FreeTensor(a1Local);
        inQueueB1.FreeTensor(b1Local);
    }

    __aicore__ inline void CopyOut()
    {
        LocalTensor<L1OutT> co1Local = outQueueCO1.DeQue<L1OutT>();
        DataCopy(cGM, co1Local, m * n);
        outQueueCO1.FreeTensor(co1Local);
    }

    TPipe pipe;
    TQue<TPosition::A1, 1> inQueueA1;
    TQue<TPosition::B1, 1> inQueueB1;
    TQue<TPosition::CO1, 1> outQueueCO1;
    GlobalTensor<Src0T> aGM;
    GlobalTensor<Src1T> bGM;
    GlobalTensor<DstT> cGM;
    uint16_t m, k, n;
};""",
    "aic_test_class": """class TEST_${API_NAME_UPPER} : public testing::Test {
protected:
    void SetUp() { g_coreType = AscendC::AIC_TYPE; }
    void TearDown()
    {
        AscendC::CheckSyncState();
        g_coreType = AscendC::MIX_TYPE;
    }
};""",
    "aic_test_case": """TEST_F(TEST_${API_NAME_UPPER}, ${API_NAME}_Basic)
{
    uint16_t m = ${M_SIZE};
    uint16_t n = ${N_SIZE};
    uint16_t k = ${K_SIZE};

    // 跳过非目标核心
    if ASCEND_IS_AIV {
        return;
    }

    // 分配内存
    uint8_t a[m * k * sizeof(${SRC0_TYPE})] = {0};
    uint8_t b[k * n * sizeof(${SRC1_TYPE})] = {0};
    uint8_t c[m * n * sizeof(${DST_TYPE})] = {0};

    // 初始化输入数据
    for (int i = 0; i < m * k; i++) {
        reinterpret_cast<${SRC0_TYPE}*>(a)[i] = static_cast<${SRC0_TYPE}>(i % 10);
    }
    for (int i = 0; i < k * n; i++) {
        reinterpret_cast<${SRC1_TYPE}*>(b)[i] = static_cast<${SRC1_TYPE}>((i + 1) % 10);
    }

    // 执行 Kernel
    Kernel${API_NAME}<${SRC0_TYPE}, ${SRC1_TYPE}, ${DST_TYPE}, ${L1OUT_TYPE}> op;
    op.Init(a, b, c, m, k, n);
    op.Process();

    // 验证结果
    for (int32_t i = 0; i < m * n; i++) {
        // TODO: 添加实际验证逻辑
        ${DST_TYPE} val = reinterpret_cast<${DST_TYPE}*>(c)[i];
    }
}""",
    # -------------------------------------------------------------------------
    # C API 模板
    # -------------------------------------------------------------------------
    "c_api_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

${TEST_CLASS}

${STUB_FUNCTION}

${TEST_CASE}
""",
    "c_api_test_class": """
class Test${API_NAME}CAPI : public testing::Test {
protected:
    void SetUp() {
        g_coreType = C_API_AIV_TYPE;
    }
    void TearDown() {
        g_coreType = C_API_AIV_TYPE;
    }
};
""",
    "c_api_stub_function": """
namespace {
void ${API_NAME}_Stub(vector_uint8_t& dst, vector_uint8_t src0,
                       vector_uint8_t src1, vector_bool mask, Literal literal) {
    // 参数验证
    EXPECT_TRUE(true);
}
}
""",
    "c_api_test_case": """
TEST_F(Test${API_NAME}CAPI, ${API_NAME}_Succ) {
    vector_uint8_t dst;
    vector_uint8_t src0;
    vector_uint8_t src1;
    vector_bool mask;

    MOCKER_CPP(${API_NAME}, void(vector_uint8_t&, vector_uint8_t, vector_uint8_t, vector_bool, Literal))
        .times(1)
        .will(invoke(${API_NAME}_Stub));

    ${API_NAME}(dst, src0, src1, mask);
    GlobalMockObject::verify();
}
""",
    # -------------------------------------------------------------------------
    # 高阶 API 模板
    # -------------------------------------------------------------------------
    "adv_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

${KERNEL_CLASS}

${PARAM_STRUCT}

${TEST_CLASS}

${INSTANTIATION}

${TEST_CASE}
""",
    "adv_kernel_class": """
template <typename T1, typename T2>
class Kernel${API_NAME} {
public:
    __aicore__ inline Kernel${API_NAME}() {}

    __aicore__ inline void Init(__gm__ uint8_t* srcGm, __gm__ uint8_t* dstGm,
                                 uint32_t height, uint32_t width) {
        this->height = height;
        this->width = width;

        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T1*>(srcGm), height * width);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T2*>(dstGm), height * width);

        pipe.InitBuffer(inQueueSrc, 1, height * width * sizeof(T1));
        pipe.InitBuffer(outQueueDst, 1, height * width * sizeof(T2));
    }

    __aicore__ inline void Process() {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn() {
        LocalTensor<T1> srcLocal = inQueueSrc.AllocTensor<T1>();
        DataCopy(srcLocal, srcGlobal, height * width);
        inQueueSrc.EnQue(srcLocal);
    }

    __aicore__ inline void Compute() {
        LocalTensor<T1> srcLocal = inQueueSrc.DeQue<T1>();
        LocalTensor<T2> dstLocal = outQueueDst.AllocTensor<T2>();

        // 调用高阶 API
        ${API_CALL}

        outQueueDst.EnQue<T2>(dstLocal);
        inQueueSrc.FreeTensor(srcLocal);
    }

    __aicore__ inline void CopyOut() {
        LocalTensor<T2> dstLocal = outQueueDst.DeQue<T2>();
        DataCopy(dstGlobal, dstLocal, height * width);
        outQueueDst.FreeTensor(dstLocal);
    }

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueSrc;
    TQue<TPosition::VECOUT, 1> outQueueDst;
    GlobalTensor<T1> srcGlobal;
    GlobalTensor<T2> dstGlobal;
    uint32_t height, width;
};
""",
    "adv_param_struct": """
struct ${API_NAME}TestParams {
    uint32_t typeSize;
    uint32_t height;
    uint32_t width;
    void (*cal_func)(uint8_t*, uint8_t*, uint32_t, uint32_t);
};
""",
    "adv_test_class": """
class ${API_NAME}Testsuite : public testing::Test,
                              public testing::WithParamInterface<${API_NAME}TestParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};
""",
    "adv_instantiation": """
INSTANTIATE_TEST_CASE_P(TEST_${API_NAME_UPPER}, ${API_NAME}Testsuite,
    ::testing::Values(
        ${TEST_PARAMS}
    ));
""",
    "adv_test_case": """
TEST_P(${API_NAME}Testsuite, ${API_NAME}TestCase) {
    auto param = GetParam();

    uint32_t dataSize = param.height * param.width;

    // 分配内存
    uint8_t srcGm[dataSize * param.typeSize] = {0};
    uint8_t dstGm[dataSize * param.typeSize] = {0};

    // 初始化输入数据
    for (uint32_t i = 0; i < dataSize; i++) {
        srcGm[i] = static_cast<uint8_t>(i % 256);
    }

    // 执行测试
    param.cal_func(srcGm, dstGm, param.height, param.width);

    // 验证结果
    // TODO: 添加实际验证逻辑
}
""",
    # -------------------------------------------------------------------------
    # SIMT API 模板
    # -------------------------------------------------------------------------
    "simt_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include <type_traits>
#include <cmath>
#include "kernel_operator.h"
#include "simt_compiler_stub.h"

using namespace std;
using namespace AscendC;

${KERNEL_CLASS}

${TEST_CLASS}

${TEST_CASE}
""",
    "simt_kernel_class": """
template <typename T>
class Kernel${API_NAME} {
public:
    __aicore__ inline Kernel${API_NAME}() {}

    __aicore__ inline void Process(__gm__ T* out, __gm__ T* src0, __gm__ T* src1,
                                    const int mode) {
        // SIMT 计算逻辑
        // TODO: 实现 SIMT 计算
    }
};
""",
    "simt_test_class": """
class ${API_NAME}Testsuite : public testing::Test,
                              public testing::WithParamInterface<int> {
protected:
    void SetUp() {}
    void TearDown() {}
};
""",
    "simt_test_case": """
TEST_P(${API_NAME}Testsuite, ${API_NAME}TestCase) {
    int mode = GetParam();
    int fpByteSize = 4;
    int shapeSize = 128;

    uint8_t dstGm[shapeSize * fpByteSize] = {0};
    uint8_t src0Gm[shapeSize * fpByteSize] = {0};
    uint8_t src1Gm[shapeSize * fpByteSize] = {0};

    // 初始化输入数据
    for (int i = 0; i < shapeSize; i++) {
        reinterpret_cast<float*>(src0Gm)[i] = static_cast<float>(i);
        reinterpret_cast<float*>(src1Gm)[i] = static_cast<float>(i + 1);
    }

    // 执行 SIMT Kernel
    // TODO: 实现 SIMT 调用

    // 验证结果
    // TODO: 添加验证逻辑
}
""",
    # -------------------------------------------------------------------------
    # Register Compute API 模板
    # -------------------------------------------------------------------------
    "reg_basic": """${COPYRIGHT}
#include <gtest/gtest.h>
#include <type_traits>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

${KERNEL_CLASS}

${TEST_CLASS}

${TEST_CASE}
""",
    "reg_kernel_class": """
template <typename T, typename Src1T, int32_t mD>
class Kernel${API_NAME} {
public:
    __aicore__ inline Kernel${API_NAME}() {}

    __aicore__ inline void Init(GM_ADDR dstGm, GM_ADDR src0Gm, GM_ADDR src1Gm,
                                 uint32_t nums, uint32_t vecMask) {
        this->nums = nums;
        this->vecMask = vecMask;

        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), nums);
        src0Global.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(src0Gm), nums);
        src1Global.SetGlobalBuffer(reinterpret_cast<__gm__ Src1T*>(src1Gm), nums);
    }

    __aicore__ inline void Process() {
        __VEC_SCOPE__ {
            ${REG_COMPUTE_BODY}
        }
        CopyOut();
    }

private:
    __aicore__ inline void CopyOut() {
        for (uint32_t i = 0; i < nums; i++) {
            dstGlobal.SetValue(i, src0Global.GetValue(i));
        }
    }

    GlobalTensor<T> dstGlobal;
    GlobalTensor<T> src0Global;
    GlobalTensor<Src1T> src1Global;
    uint32_t nums;
    uint32_t vecMask;
};
""",
    "reg_test_class": """
class TEST_${API_NAME_UPPER} : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};
""",
    "reg_test_case": """
TEST_F(TEST_${API_NAME_UPPER}, ${API_NAME}_Basic) {
    constexpr int32_t dataSize = 256;

    uint8_t dstGm[dataSize * sizeof(half)] = {0};
    uint8_t src0Gm[dataSize * sizeof(half)] = {0};
    uint8_t src1Gm[dataSize * sizeof(half)] = {0};

    // 初始化输入数据
    for (int i = 0; i < dataSize; i++) {
        reinterpret_cast<half*>(src0Gm)[i] = static_cast<half>(i % 100);
        reinterpret_cast<half*>(src1Gm)[i] = static_cast<half>((i + 1) % 100);
    }

    // 执行 Kernel
    Kernel${API_NAME}<half, half, 1> op;
    op.Init(dstGm, src0Gm, src1Gm, dataSize, 256);
    op.Process();

    // 验证结果
    // TODO: 添加验证逻辑
}
""",
}


# =============================================================================
# UT 生成器类
# =============================================================================


class UTGenerator:
    """UT 代码生成器基类 - 使用模板系统"""

    def __init__(self, config: UTConfig):
        self.config = config

    @staticmethod
    def render(template_name: str, variables: Dict[str, Any]) -> str:
        """渲染模板"""
        template = TEMPLATES.get(template_name, "")
        if not template:
            raise ValueError(f"Template not found: {template_name}")

        substitutions = {key: str(value) for key, value in variables.items()}
        return Template(template).safe_substitute(substitutions)

    def get_test_dir(self) -> str:
        """获取测试目录路径"""
        base_dir = ARCH_DIR_MAP[self.config.chip]

        if self.config.api_type == ApiType.AIV:
            return f"tests/api/basic_api/{base_dir}/{base_dir}_aiv"
        elif self.config.api_type == ApiType.AIC:
            return f"tests/api/basic_api/{base_dir}/{base_dir}_aic"
        elif self.config.api_type == ApiType.C_API:
            npu_arch = NPU_ARCH_MAP[self.config.chip]
            return f"tests/api/c_api/npu_arch_{npu_arch}/vector_compute"
        elif self.config.api_type == ApiType.ADV:
            return f"tests/api/adv_api/{self.config.api_name.lower()}"
        elif self.config.api_type == ApiType.SIMT:
            return f"tests/api/simt_api/{base_dir}_simt"
        elif self.config.api_type == ApiType.REG_COMPUTE:
            return f"tests/api/reg_compute_api/{base_dir}_reg_compute"
        elif self.config.api_type == ApiType.UTILS:
            return "tests/api/utils"
        else:
            return f"tests/api/basic_api/{base_dir}"

    def get_test_filename(self) -> str:
        """获取测试文件名"""
        api_name = self.config.api_name.lower()
        if self.config.api_type == ApiType.C_API:
            return f"test_{api_name}.cpp"
        return f"test_operator_{api_name}.cpp"

    def generate(self) -> str:
        """生成完整的测试文件内容 - 子类实现"""
        raise NotImplementedError("Subclasses must implement generate()")


class AIVUTGenerator(UTGenerator):
    """AIV (Vector 核心) API 测试生成器"""

    def generate(self) -> str:
        template_kind = self._get_template_kind()
        if template_kind == "scalar_tensor_dispatch":
            return self._generate_scalar_tensor_dispatch()
        if template_kind != "binary":
            raise ValueError(f"unsupported AIV template kind: {template_kind}")
        return self._generate_binary()

    def _get_template_kind(self) -> str:
        explicit_template = self.config.kernel_params.get("aiv_template")
        if explicit_template is not None:
            if explicit_template not in AIV_EXPLICIT_TEMPLATES:
                raise ValueError(
                    f"unsupported explicit AIV template {explicit_template!r}; "
                    f"supported values: {sorted(AIV_EXPLICIT_TEMPLATES)}"
                )
            return explicit_template

        if self.config.api_name.lower() in AIV_GENERIC_BINARY_APIS:
            return "binary"
        if self.config.api_name.lower() in AIV_GENERIC_SCALAR_TENSOR_DISPATCH_APIS:
            return "scalar_tensor_dispatch"

        raise ValueError(
            "AIVUTGenerator cannot infer an executable AIV UT signature for "
            f"{self.config.api_name!r}. Generic AIV skeleton generation is "
            "limited to verified binary APIs. Read the target impl and same-category "
            "UT first, then pass kernel_params.aiv_template explicitly or write an "
            "API-specific UT."
        )

    def _validate_binary_cases(self) -> None:
        non_binary_cases = [tc.name for tc in self.config.test_cases if tc.input_count != 2]
        if non_binary_cases:
            raise ValueError(
                f"binary AIV template requires input_count=2 for every test case; non-binary cases: {non_binary_cases}"
            )

    def _validate_scalar_tensor_dispatch_cases(self) -> None:
        invalid_cases = [tc.name for tc in self.config.test_cases if tc.input_count != 1]
        if invalid_cases:
            raise ValueError(
                "scalar_tensor_dispatch AIV template requires input_count=1 for every test case; "
                f"invalid cases: {invalid_cases}"
            )

    def _generate_binary(self) -> str:
        self._validate_binary_cases()
        variables = {
            "COPYRIGHT": TEMPLATES["copyright"],
            "API_NAME": self.config.api_name,
            "API_NAME_UPPER": self.config.api_name.upper(),
            "TEMPLATE_PARAMS": "typename T, typename Src1T",
            "API_CALL": f"{self.config.api_name}(dstLocal, src0Local, src1Local, dataSize);",
        }

        # 生成测试参数
        test_params = []
        for tc in self.config.test_cases:
            dtype_info = DTYPE_MAP.get(tc.dtype, DTYPE_MAP["half"])
            func_name = f"main_{self.config.api_name}<{tc.dtype}>"
            init_func_name = f"Init{self.config.api_name}Inputs<{tc.dtype}>"
            test_params.append(
                f"{self.config.api_name}TestParams{{{tc.data_size}, "
                f"{dtype_info['size']}, {func_name}, {init_func_name}}}"
            )
        variables["INSTANTIATION"] = render_gtest_values_instantiation(
            self.config.api_name, self.config.api_name.upper(), test_params
        )

        # 渲染各部分
        parts = [
            self.render(
                "aiv_basic",
                {
                    **variables,
                    "KERNEL_CLASS": self.render("aiv_kernel_class", variables),
                    "MAIN_FUNCTION": self.render("aiv_main_function", variables),
                    "INIT_FUNCTION": self.render("aiv_init_function", variables),
                    "PARAM_STRUCT": self.render("aiv_param_struct", variables),
                    "TEST_CLASS": self.render("aiv_test_class", variables),
                    "INSTANTIATION": self.render("aiv_instantiation", variables),
                    "TEST_CASE": self.render("aiv_test_case", variables),
                },
            )
        ]
        return "\n".join(parts)

    def _generate_scalar_tensor_dispatch(self) -> str:
        self._validate_scalar_tensor_dispatch_cases()
        variables = {
            "COPYRIGHT": TEMPLATES["copyright"],
            "API_NAME": self.config.api_name,
            "API_NAME_UPPER": self.config.api_name.upper(),
            "RAW_API_CALL": f"{self.config.api_name}(dstLocal, scalarValue, dataSize);",
            "TENSOR_API_CALL": f"{self.config.api_name}(dstLocal, srcLocal, dataSize);",
        }

        test_params = []
        for tc in self.config.test_cases:
            dtype_info = DTYPE_MAP.get(tc.dtype, DTYPE_MAP["half"])
            dtype_name = tc.dtype
            if tc.additional_params.get("tensor_trait", False):
                dtype_name = f"TensorTrait<{tc.dtype}>"
            func_name = f"main_{self.config.api_name}<{dtype_name}>"
            test_params.append(f"{self.config.api_name}TestParams{{{tc.data_size}, {dtype_info['size']}, {func_name}}}")
        variables["INSTANTIATION"] = render_gtest_values_instantiation(
            self.config.api_name, self.config.api_name.upper(), test_params
        )

        return (
            self.render(
                "aiv_scalar_tensor_dispatch_basic",
                {
                    **variables,
                    "KERNEL_CLASS": self.render("aiv_scalar_tensor_dispatch_kernel_class", variables),
                    "MAIN_FUNCTION": self.render("aiv_scalar_tensor_dispatch_main_function", variables),
                    "PARAM_STRUCT": self.render("aiv_scalar_tensor_dispatch_param_struct", variables),
                    "TEST_CLASS": self.render("aiv_scalar_tensor_dispatch_test_class", variables),
                    "INSTANTIATION": self.render("aiv_scalar_tensor_dispatch_instantiation", variables),
                    "TEST_CASE": self.render("aiv_scalar_tensor_dispatch_test_case", variables),
                },
            ).rstrip()
            + "\n"
        )


class AICUTGenerator(UTGenerator):
    """AIC (Cube 核心) API 测试生成器"""

    def generate(self) -> str:
        template_kind = self._get_template_kind()
        if template_kind != "mmad":
            raise ValueError(f"unsupported AIC template kind: {template_kind}")

        kernel_params = self.config.kernel_params
        m = kernel_params.get("m", 16)
        k = kernel_params.get("k", 64)
        n = kernel_params.get("n", 16)

        variables = {
            "COPYRIGHT": TEMPLATES["copyright"],
            "API_NAME": self.config.api_name,
            "API_NAME_UPPER": self.config.api_name.upper(),
            "API_CALL": "Mmad(co1Local, a1Local, b1Local, {m, k, n}, {1, 1, 1});",
            "M_SIZE": str(m),
            "N_SIZE": str(n),
            "K_SIZE": str(k),
            "SRC0_TYPE": "half",
            "SRC1_TYPE": "half",
            "DST_TYPE": "float",
            "L1OUT_TYPE": "float",
        }

        parts = [
            self.render(
                "aic_basic",
                {
                    **variables,
                    "KERNEL_CLASS": self.render("aic_kernel_class", variables),
                    "TEST_CLASS": self.render("aic_test_class", variables),
                    "TEST_CASE": self.render("aic_test_case", variables),
                },
            )
        ]
        return "\n".join(parts)

    def _get_template_kind(self) -> str:
        explicit_template = self.config.kernel_params.get("aic_template")
        if explicit_template:
            if explicit_template not in AIC_EXPLICIT_TEMPLATES:
                raise ValueError(
                    f"unsupported explicit AIC template {explicit_template!r}; "
                    f"supported templates: {sorted(AIC_EXPLICIT_TEMPLATES)}"
                )
            return explicit_template

        if self.config.api_name.lower() in AIC_GENERIC_MMAD_LIKE_APIS:
            return "mmad"

        raise ValueError(
            "AIC automatic generation requires a verified template family; "
            f"{self.config.api_name!r} is not in the generic MMAD-like allowlist. "
            "Read the implementation and same-category UT first, then write an "
            "API-specific UT or pass an explicit verified template."
        )


class CAPITestGenerator(UTGenerator):
    """C API 测试生成器"""

    def generate(self) -> str:
        requested_template = self.config.kernel_params.get("c_api_template")
        if requested_template:
            if requested_template not in C_API_GENERIC_TEMPLATES:
                raise ValueError(
                    f"unsupported explicit C API template {requested_template!r}; "
                    f"supported templates: {sorted(C_API_GENERIC_TEMPLATES)}"
                )

        raise ValueError(
            "C API automatic generation is disabled until an executable "
            "signature-family template is verified for the target architecture. "
            "Read the declaration, impl, stub signature, and same-category UT "
            "first, then write an API-specific UT instead of reusing a generic "
            "mock shape."
        )


class ADVUTGenerator(UTGenerator):
    """高阶 API 测试生成器"""

    def generate(self) -> str:
        profile = get_adv_api_profile(self.config.api_name, self.config.kernel_params)
        if profile is None:
            raise ValueError(
                "AdvUTGenerator cannot infer an executable high-level API UT "
                f"signature for {self.config.api_name!r}. Generic ADV skeleton "
                "generation is disabled because high-level APIs require "
                "API-specific params, workspace, multi-output handling, and "
                "validation. Pass kernel_params.adv_profile with an explicit "
                "existing executable source UT or write an API-specific UT."
            )

        source_path = REPO_ROOT / profile.source
        if not source_path.is_file():
            raise FileNotFoundError(
                f"ADV executable profile for {self.config.api_name!r} points to a missing reference UT: {source_path}"
            )

        return normalize_generated_cpp(source_path.read_text(encoding="utf-8"))


class SIMTUTGenerator(UTGenerator):
    """SIMT API 测试生成器"""

    def generate(self) -> str:
        requested_template = self.config.kernel_params.get("simt_template")
        if requested_template:
            if requested_template not in SIMT_GENERIC_TEMPLATES:
                raise ValueError(
                    f"unsupported explicit SIMT template {requested_template!r}; "
                    f"supported templates: {sorted(SIMT_GENERIC_TEMPLATES)}"
                )

        raise ValueError(
            "SIMT automatic generation is disabled until an executable "
            "SIMT-family template is verified. Read the declaration, impl, and "
            "same-category UT first, then write an API-specific UT instead of a "
            "placeholder kernel."
        )


class RegComputeUTGenerator(UTGenerator):
    """Register Compute API 测试生成器"""

    def generate(self) -> str:
        requested_template = self.config.kernel_params.get("reg_template")
        if requested_template:
            if requested_template not in REG_GENERIC_TEMPLATES:
                raise ValueError(
                    f"unsupported explicit regbase template {requested_template!r}; "
                    f"supported templates: {sorted(REG_GENERIC_TEMPLATES)}"
                )

        raise ValueError(
            "regbase automatic generation is disabled until an executable "
            "register-family template is verified. Read the implementation and "
            "same-category UT first, then write an API-specific UT instead of a "
            "placeholder skeleton."
        )


class UtilsUTGenerator(UTGenerator):
    """Utils API 测试生成器"""

    def generate(self) -> str:
        requested_template = self.config.kernel_params.get("utils_template")
        if requested_template:
            if requested_template not in UTILS_GENERIC_TEMPLATES:
                raise ValueError(
                    f"unsupported explicit utils template {requested_template!r}; "
                    f"supported templates: {sorted(UTILS_GENERIC_TEMPLATES)}"
                )

        raise ValueError(
            "utils automatic generation is disabled until an executable "
            "utils-family template is verified. Read the declaration, impl, "
            "and same-category UT first, then write an API-specific UT instead "
            "of reusing one skeleton across runtime, compile-time, std, tiling, "
            "and context utilities."
        )


def create_generator(config: UTConfig) -> UTGenerator:
    """根据 API 类型创建对应的生成器"""
    ensure_reference_constraints_loaded()

    generators = {
        ApiType.AIV: AIVUTGenerator,
        ApiType.AIC: AICUTGenerator,
        ApiType.C_API: CAPITestGenerator,
        ApiType.ADV: ADVUTGenerator,
        ApiType.SIMT: SIMTUTGenerator,
        ApiType.REG_COMPUTE: RegComputeUTGenerator,
        ApiType.UTILS: UtilsUTGenerator,
    }

    generator_class = generators.get(config.api_type)
    if not generator_class:
        raise ValueError(f"Unsupported API type: {config.api_type}")

    return generator_class(config)
