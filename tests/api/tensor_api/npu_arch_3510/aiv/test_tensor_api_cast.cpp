/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include "c_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

class Tensor_Api_Vector_Cast_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}

    void TearDown() override {}
};

template <typename DstDataType, typename SrcDataType, typename Func, typename TraitType = AscendC::Std::ignore_t>
__aicore__ inline void TestTransformBinary(
    __gm__ DstDataType* z, __gm__ SrcDataType* x, __ubuf__ DstDataType zUB[2048], __ubuf__ SrcDataType xUB[2048])
{
    constexpr uint32_t TILE_LENGTH = 2048;
    constexpr uint32_t BLK_NUM = 1;

    using namespace AscendC::Te;
    asc_init();

    constexpr uint8_t cacheMode = 0;
    constexpr uint32_t burstLength = 0;
    constexpr uint64_t srcStride = 0;
    constexpr uint32_t dstStride = 0;

    auto gmPtrX = MakeMemPtr<Location::GM>(x);
    auto gmPtrZ = MakeMemPtr<Location::GM>(z);

    auto xGm = MakeTensor(gmPtrX, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(gmPtrZ, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    asc_copy_gm2ub_align(
        xLocal.Data().Get(), xGm.Data().Get(), BLK_NUM, TILE_LENGTH * sizeof(SrcDataType), 0, 0, true, cacheMode,
        srcStride, dstStride);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    Transform<Func, TraitType>(zLocal, xLocal);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(
        zGm.Data().Get(), zLocal.Data().Get(), BLK_NUM, TILE_LENGTH * sizeof(DstDataType), cacheMode, srcStride,
        dstStride);
}

template <typename RoundMode, typename SatMode, typename IndexPos>
struct CastTrait {
    using roundMode = RoundMode;
    using satMode = SatMode;
    using indexPos = IndexPos;
};

#define VECTOR_CAST_3510(Function, DstDataType, SrcDataType)                                        \
    TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_##Function##_##DstDataType##_##SrcDataType)          \
    {                                                                                               \
        constexpr uint32_t TILE_LENGTH = 2048;                                                      \
                                                                                                    \
        __gm__ SrcDataType x[TILE_LENGTH] = {0};                                                    \
        __gm__ DstDataType z[TILE_LENGTH] = {0};                                                    \
                                                                                                    \
        __ubuf__ SrcDataType xUB[TILE_LENGTH] = {0};                                                \
        __ubuf__ DstDataType zUB[TILE_LENGTH] = {0};                                                \
                                                                                                    \
        TestTransformBinary<DstDataType, SrcDataType, AscendC::Te::Inst::Function>(z, x, zUB, xUB); \
        EXPECT_EQ(z[0], zUB[0]);                                                                    \
    }

// ===================================================================
// 1. 同类型舍入操作：Rint / Round / Trunc
// ===================================================================
VECTOR_CAST_3510(Ceil, half, half)
VECTOR_CAST_3510(Ceil, float, float)
VECTOR_CAST_3510(Ceil, bfloat16_t, bfloat16_t)

VECTOR_CAST_3510(Rint, half, half)
VECTOR_CAST_3510(Rint, float, float)
VECTOR_CAST_3510(Rint, bfloat16_t, bfloat16_t)

VECTOR_CAST_3510(Round, half, half)
VECTOR_CAST_3510(Round, float, float)
VECTOR_CAST_3510(Round, bfloat16_t, bfloat16_t)

VECTOR_CAST_3510(Trunc, half, half)
VECTOR_CAST_3510(Trunc, float, float)
VECTOR_CAST_3510(Trunc, bfloat16_t, bfloat16_t)

// ===================================================================
// 2. float → half
// ===================================================================

// 2a. float → half, RD + NoSat + Even（偶数位，不饱和）
using CastTraitFloat2HalfRDNoSatEven = CastTrait<
    AscendC::Te::CastRoundMode::RD, AscendC::Te::CastSatMode::NoSat,
    AscendC::Std::ignore_t>; // Even 默认

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Half_RD_NoSat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ half z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ half zUB[TILE_LENGTH] = {0};

    TestTransformBinary<half, float, AscendC::Te::Inst::Cast, CastTraitFloat2HalfRDNoSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], static_cast<half>(x[0]));
    EXPECT_EQ(z[0], zUB[0]);
}

// 2b. float → half, RN + Sat + Odd（奇数位，饱和）
using CastTraitFloat2HalfRNSatOdd =
    CastTrait<AscendC::Te::CastRoundMode::RN, AscendC::Te::CastSatMode::Sat, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Half_RN_Sat_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ half z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ half zUB[TILE_LENGTH] = {0};

    TestTransformBinary<half, float, AscendC::Te::Inst::Cast, CastTraitFloat2HalfRNSatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// 2c. float → half, RU + Sat + Even
using CastTraitFloat2HalfRUSatEven =
    CastTrait<AscendC::Te::CastRoundMode::RU, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Half_RU_Sat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ half z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ half zUB[TILE_LENGTH] = {0};

    TestTransformBinary<half, float, AscendC::Te::Inst::Cast, CastTraitFloat2HalfRUSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// 2d. float → half, RZ + NoSat + Odd
using CastTraitFloat2HalfRZNoSatOdd =
    CastTrait<AscendC::Te::CastRoundMode::RZ, AscendC::Te::CastSatMode::NoSat, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Half_RZ_NoSat_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ half z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ half zUB[TILE_LENGTH] = {0};

    TestTransformBinary<half, float, AscendC::Te::Inst::Cast, CastTraitFloat2HalfRZNoSatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 3. half → int8_t：测试不同 RoundMode × SatMode × IndexPos
// ===================================================================

// 3a. half → int8_t, RD + Sat + Even（向零舍入，饱和）
using CastTraitHalf2Int8RDSatEven =
    CastTrait<AscendC::Te::CastRoundMode::RD, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Half2Int8_RD_Sat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ half x[TILE_LENGTH] = {0};
    __gm__ int8_t z[TILE_LENGTH] = {0};

    __ubuf__ half xUB[TILE_LENGTH] = {0};
    __ubuf__ int8_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int8_t, half, AscendC::Te::Inst::Cast, CastTraitHalf2Int8RDSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// 3b. half → int8_t, RN + NoSat + Odd
using CastTraitHalf2Int8RNNoSatOdd =
    CastTrait<AscendC::Te::CastRoundMode::RN, AscendC::Te::CastSatMode::NoSat, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Half2Int8_RN_NoSat_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ half x[TILE_LENGTH] = {0};
    __gm__ int8_t z[TILE_LENGTH] = {0};

    __ubuf__ half xUB[TILE_LENGTH] = {0};
    __ubuf__ int8_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int8_t, half, AscendC::Te::Inst::Cast, CastTraitHalf2Int8RNNoSatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// 3c. half → int8_t, RU + Sat + Odd
using CastTraitHalf2Int8RUSatOdd =
    CastTrait<AscendC::Te::CastRoundMode::RU, AscendC::Te::CastSatMode::Sat, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Half2Int8_RU_Sat_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ half x[TILE_LENGTH] = {0};
    __gm__ int8_t z[TILE_LENGTH] = {0};

    __ubuf__ half xUB[TILE_LENGTH] = {0};
    __ubuf__ int8_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int8_t, half, AscendC::Te::Inst::Cast, CastTraitHalf2Int8RUSatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 4. half → float：简单拓宽，默认 + Even/Odd
// ===================================================================

using CastTraitHalf2FloatDefault = CastTrait<AscendC::Std::ignore_t, AscendC::Std::ignore_t, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_Cast_Half2Float_Default)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ half x[TILE_LENGTH] = {0};
    __gm__ float z[TILE_LENGTH] = {0};

    __ubuf__ half xUB[TILE_LENGTH] = {0};
    __ubuf__ float zUB[TILE_LENGTH] = {0};

    TestTransformBinary<float, half, AscendC::Te::Inst::Cast, CastTraitHalf2FloatDefault>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], static_cast<float>(x[0]));
    EXPECT_EQ(z[0], zUB[0]);
}

using CastTraitHalf2FloatOdd =
    CastTrait<AscendC::Std::ignore_t, AscendC::Std::ignore_t, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_Cast_Half2Float_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ half x[TILE_LENGTH] = {0};
    __gm__ float z[TILE_LENGTH] = {0};

    __ubuf__ half xUB[TILE_LENGTH] = {0};
    __ubuf__ float zUB[TILE_LENGTH] = {0};

    TestTransformBinary<float, half, AscendC::Te::Inst::Cast, CastTraitHalf2FloatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 5. float → int32_t：测试不同 RoundMode × SatMode
// ===================================================================

using CastTraitFloat2Int32RNNoSatEven =
    CastTrait<AscendC::Te::CastRoundMode::RN, AscendC::Te::CastSatMode::NoSat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Int32_RN_NoSat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ int32_t z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ int32_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int32_t, float, AscendC::Te::Inst::Cast, CastTraitFloat2Int32RNNoSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

using CastTraitFloat2Int32RDSatEven =
    CastTrait<AscendC::Te::CastRoundMode::RD, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Int32_RD_Sat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ int32_t z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ int32_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int32_t, float, AscendC::Te::Inst::Cast, CastTraitFloat2Int32RDSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

using CastTraitFloat2Int32RUSatEven =
    CastTrait<AscendC::Te::CastRoundMode::RU, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Int32_RU_Sat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ int32_t z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ int32_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int32_t, float, AscendC::Te::Inst::Cast, CastTraitFloat2Int32RUSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 6. int32_t → int16_t：测试 Sat（收缩 + 饱和）
// ===================================================================

using CastTraitInt322Int16SatEven =
    CastTrait<AscendC::Std::ignore_t, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Int322Int16_Sat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ int32_t x[TILE_LENGTH] = {0};
    __gm__ int16_t z[TILE_LENGTH] = {0};

    __ubuf__ int32_t xUB[TILE_LENGTH] = {0};
    __ubuf__ int16_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int16_t, int32_t, AscendC::Te::Inst::Cast, CastTraitInt322Int16SatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

using CastTraitInt322Int16NoSatOdd =
    CastTrait<AscendC::Std::ignore_t, AscendC::Te::CastSatMode::NoSat, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Int322Int16_NoSat_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ int32_t x[TILE_LENGTH] = {0};
    __gm__ int16_t z[TILE_LENGTH] = {0};

    __ubuf__ int32_t xUB[TILE_LENGTH] = {0};
    __ubuf__ int16_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int16_t, int32_t, AscendC::Te::Inst::Cast, CastTraitInt322Int16NoSatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 7. int16_t → half：测试不同 RoundMode
// ===================================================================

using CastTraitInt162HalfRN = CastTrait<AscendC::Te::CastRoundMode::RN, AscendC::Std::ignore_t, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Int162Half_RN)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ int16_t x[TILE_LENGTH] = {0};
    __gm__ half z[TILE_LENGTH] = {0};

    __ubuf__ int16_t xUB[TILE_LENGTH] = {0};
    __ubuf__ half zUB[TILE_LENGTH] = {0};

    TestTransformBinary<half, int16_t, AscendC::Te::Inst::Cast, CastTraitInt162HalfRN>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

using CastTraitInt162HalfRZ = CastTrait<AscendC::Te::CastRoundMode::RZ, AscendC::Std::ignore_t, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Int162Half_RZ)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ int16_t x[TILE_LENGTH] = {0};
    __gm__ half z[TILE_LENGTH] = {0};

    __ubuf__ int16_t xUB[TILE_LENGTH] = {0};
    __ubuf__ half zUB[TILE_LENGTH] = {0};

    TestTransformBinary<half, int16_t, AscendC::Te::Inst::Cast, CastTraitInt162HalfRZ>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 8. uint16_t → uint8_t
// ===================================================================

using CastTraitU162UInt8SatOdd =
    CastTrait<AscendC::Std::ignore_t, AscendC::Te::CastSatMode::Sat, AscendC::Te::CastIndexPos::Odd>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_U162UInt8_Sat_Odd)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ uint16_t x[TILE_LENGTH] = {0};
    __gm__ uint8_t z[TILE_LENGTH] = {0};

    __ubuf__ uint16_t xUB[TILE_LENGTH] = {0};
    __ubuf__ uint8_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<uint8_t, uint16_t, AscendC::Te::Inst::Cast, CastTraitU162UInt8SatOdd>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 9. uint32_t → uint16_t：测试收缩 + Sat
// ===================================================================

using CastTraitU322UInt16SatEven =
    CastTrait<AscendC::Std::ignore_t, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_U322UInt16_Sat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ uint32_t x[TILE_LENGTH] = {0};
    __gm__ uint16_t z[TILE_LENGTH] = {0};

    __ubuf__ uint32_t xUB[TILE_LENGTH] = {0};
    __ubuf__ uint16_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<uint16_t, uint32_t, AscendC::Te::Inst::Cast, CastTraitU322UInt16SatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 10. half → int32_t (1:2 扩宽)
// ===================================================================

using CastTraitHalf2Int32RDNoSat =
    CastTrait<AscendC::Te::CastRoundMode::RD, AscendC::Te::CastSatMode::NoSat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Half2Int32_RD_NoSat)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ half x[TILE_LENGTH] = {0};
    __gm__ int32_t z[TILE_LENGTH] = {0};

    __ubuf__ half xUB[TILE_LENGTH] = {0};
    __ubuf__ int32_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int32_t, half, AscendC::Te::Inst::Cast, CastTraitHalf2Int32RDNoSat>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 11. float → int16_t (2:1 收缩)
// ===================================================================

using CastTraitFloat2Int16RNSat =
    CastTrait<AscendC::Te::CastRoundMode::RN, AscendC::Te::CastSatMode::Sat, AscendC::Std::ignore_t>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Int16_RN_Sat)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ int16_t z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ int16_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int16_t, float, AscendC::Te::Inst::Cast, CastTraitFloat2Int16RNSat>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 12. int8_t → int32_t (1:4 扩宽)
// ===================================================================

using CastTraitInt82Int32RDNoSatPartP0 =
    CastTrait<AscendC::Std::ignore_t, AscendC::Te::CastSatMode::NoSat, AscendC::Te::CastIndexPos::PartP0>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Int82Int32_RD_NoSat_PartP0)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ int8_t x[TILE_LENGTH] = {0};
    __gm__ int32_t z[TILE_LENGTH] = {0};

    __ubuf__ int8_t xUB[TILE_LENGTH] = {0};
    __ubuf__ int32_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<int32_t, int8_t, AscendC::Te::Inst::Cast, CastTraitInt82Int32RDNoSatPartP0>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 13. int32_t → uint8_t (4:1 收缩)
// ===================================================================

using CastTraitInt322UInt8SatPartP0 =
    CastTrait<AscendC::Std::ignore_t, AscendC::Te::CastSatMode::Sat, AscendC::Te::CastIndexPos::PartP0>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Int322UInt8_Sat_PartP0)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ int32_t x[TILE_LENGTH] = {0};
    __gm__ uint8_t z[TILE_LENGTH] = {0};

    __ubuf__ int32_t xUB[TILE_LENGTH] = {0};
    __ubuf__ uint8_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<uint8_t, int32_t, AscendC::Te::Inst::Cast, CastTraitInt322UInt8SatPartP0>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 14. bfloat16_t → float (1:2 扩宽)
// ===================================================================

using CastTraitBf162FloatEven =
    CastTrait<AscendC::Std::ignore_t, AscendC::Std::ignore_t, AscendC::Te::CastIndexPos::Even>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Bf162Float_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ bfloat16_t x[TILE_LENGTH] = {0};
    __gm__ float z[TILE_LENGTH] = {0};

    __ubuf__ bfloat16_t xUB[TILE_LENGTH] = {0};
    __ubuf__ float zUB[TILE_LENGTH] = {0};

    TestTransformBinary<float, bfloat16_t, AscendC::Te::Inst::Cast, CastTraitBf162FloatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}

// ===================================================================
// 15. float → bfloat16_t (2:1 收缩)
// ===================================================================

using CastTraitFloat2Bf16RNNoSatEven =
    CastTrait<AscendC::Te::CastRoundMode::RN, AscendC::Te::CastSatMode::NoSat, AscendC::Te::CastIndexPos::Even>;

TEST_F(Tensor_Api_Vector_Cast_3510, VECTOR_CastTrait_Float2Bf16_RN_NoSat_Even)
{
    constexpr uint32_t TILE_LENGTH = 2048;

    __gm__ float x[TILE_LENGTH] = {0};
    __gm__ bfloat16_t z[TILE_LENGTH] = {0};

    __ubuf__ float xUB[TILE_LENGTH] = {0};
    __ubuf__ bfloat16_t zUB[TILE_LENGTH] = {0};

    TestTransformBinary<bfloat16_t, float, AscendC::Te::Inst::Cast, CastTraitFloat2Bf16RNNoSatEven>(z, x, zUB, xUB);

    EXPECT_EQ(z[0], zUB[0]);
}
