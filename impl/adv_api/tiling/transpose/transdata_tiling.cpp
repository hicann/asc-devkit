/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "adv_api/utils/types.h"
#include "adv_api/transpose/transdata_tiling.h"

#include <cstdint>
#include <algorithm>

#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"
namespace AscendC {
namespace {
constexpr int32_t PAD_ELE_FOR_HALF = 16;
constexpr int32_t N_INDEX = 0;
constexpr int32_t C_INDEX = 1;
constexpr int32_t D_INDEX = 2;
constexpr int32_t H_INDEX = 3;
constexpr int32_t W_INDEX = 4;

struct TmpTransDataParams {
    int32_t n = 0;
    int32_t c = 0;
    int32_t d = 0;
    int32_t h = 0;
    int32_t w = 0;
};

int32_t DivCeil(int32_t a, int32_t b)
{
    if (b == 0) {
        return a;
    }
    return (a + b - 1) / b;
}

int32_t AlignUp(int32_t a, int32_t b) { return DivCeil(a, b) * b; }

bool GenerateFractalZ3DToNcdhwShapeInfo(
    const std::vector<int64_t>& dstDims, const std::vector<int64_t>& srcDims, TmpTransDataParams& param,
    const int32_t c0, const int32_t n0)
{
    ASCENDC_HOST_ASSERT(
        srcDims.size() == 7 && dstDims.size() == 5, return false,
        "[TransData][GetTransDataMaxMinTmpSize] input shapes are not matched with DataFormat.");
    param.n = dstDims[N_INDEX];
    param.c = dstDims[C_INDEX];
    param.d = dstDims[D_INDEX];
    param.h = dstDims[H_INDEX];
    param.w = dstDims[W_INDEX];
    // validate d, h, w
    ASCENDC_HOST_ASSERT(
        param.d == srcDims[0] && param.h == srcDims[2] && param.w == srcDims[3], return false,
        "[TransData][GetTransDataMaxMinTmpSize] shapeInfo d,h,w is not matched.");
    ASCENDC_HOST_ASSERT(
        srcDims[6] == c0 && srcDims[1] * c0 == AlignUp(param.c, c0), return false,
        "[TransData][GetTransDataMaxMinTmpSize] src c0, c1 is not able to be converted to c.");
    ASCENDC_HOST_ASSERT(
        srcDims[5] == n0 && srcDims[4] * n0 == AlignUp(param.n, n0), return false,
        "[TransData][GetTransDataMaxMinTmpSize] src n0, n1 is not able to be converted to n.");
    return true;
}

bool GenerateNcdhwToFractalZ3DShapeInfo(
    const std::vector<int64_t>& dstDims, const std::vector<int64_t>& srcDims, TmpTransDataParams& param,
    const int32_t c0, const int32_t n0)
{
    ASCENDC_HOST_ASSERT(
        srcDims.size() == 5 && dstDims.size() == 7, return false,
        "[TransData][GetTransDataMaxMinTmpSize] input shapes are not matched with DataFormat.");
    param.n = srcDims[N_INDEX];
    param.c = srcDims[C_INDEX];
    param.d = srcDims[D_INDEX];
    param.h = srcDims[H_INDEX];
    param.w = srcDims[W_INDEX];
    // validate d, h, w
    ASCENDC_HOST_ASSERT(
        param.d == dstDims[0] && param.h == dstDims[2] && param.w == dstDims[3], return false,
        "[TransData][GetTransDataMaxMinTmpSize] shapeInfo d,h,w is not matched.");
    ASCENDC_HOST_ASSERT(
        dstDims[6] == c0 && dstDims[1] * c0 == AlignUp(param.c, c0), return false,
        "[TransData][GetTransDataMaxMinTmpSize] dst c0, c1 is not able to be converted to c.");
    ASCENDC_HOST_ASSERT(
        dstDims[5] == n0 && dstDims[4] * n0 == AlignUp(param.n, n0), return false,
        "[TransData][GetTransDataMaxMinTmpSize] dst n0, n1 is not able to be converted to n.");
    return true;
}

bool GenerateShapeInfo(
    const TransDataConfig& config, const AscendC::TensorShape& srcShape, const AscendC::TensorShape& dstShape,
    AscendC::TensorDataType type, TmpTransDataParams& param)
{
    (void)type;
    constexpr int32_t c0 = 16, n0 = 16;
    std::vector<int64_t> srcDims = srcShape.GetDims();
    std::vector<int64_t> dstDims = dstShape.GetDims();
    if (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::NDC1HWC0) {
        ASCENDC_HOST_ASSERT(
            srcDims.size() == 5 && dstDims.size() == 6, return false,
            "[TransData][GetTransDataMaxMinTmpSize] input shapes are not matched with DataFormat.");
        param.n = srcDims[N_INDEX];
        param.c = srcDims[C_INDEX];
        param.d = srcDims[D_INDEX];
        param.h = srcDims[H_INDEX];
        param.w = srcDims[W_INDEX];
        // validate n, d, h, w
        ASCENDC_HOST_ASSERT(
            param.n == dstDims[0] && param.d == dstDims[1] && param.h == dstDims[3] && param.w == dstDims[4],
            return false, "[TransData][GetTransDataMaxMinTmpSize] shapeInfo n,d,h,w is not matched.");
        ASCENDC_HOST_ASSERT(
            dstDims[5] == c0 && dstDims[2] * c0 == AlignUp(param.c, c0), return false,
            "[TransData][GetTransDataMaxMinTmpSize] dst c0, c1 is not able to be converted to c.");
        return true;
    }
    if (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::FRACTAL_Z_3D) {
        return GenerateNcdhwToFractalZ3DShapeInfo(dstDims, srcDims, param, c0, n0);
    }
    if (config.srcFormat == DataFormat::FRACTAL_Z_3D && config.dstFormat == DataFormat::NCDHW) {
        return GenerateFractalZ3DToNcdhwShapeInfo(dstDims, srcDims, param, c0, n0);
    }
    if (config.srcFormat == DataFormat::NDC1HWC0 && config.dstFormat == DataFormat::NCDHW) {
        ASCENDC_HOST_ASSERT(
            srcDims.size() == 6 && dstDims.size() == 5, return false,
            "[TransData][GetTransDataMaxMinTmpSize] input shapes are not matched with DataFormat.");
        param.n = dstDims[N_INDEX];
        param.c = dstDims[C_INDEX];
        param.d = dstDims[D_INDEX];
        param.h = dstDims[H_INDEX];
        param.w = dstDims[W_INDEX];
        // validate n, d, h, w
        ASCENDC_HOST_ASSERT(
            param.n == srcDims[0] && param.d == srcDims[1] && param.h == srcDims[3] && param.w == srcDims[4],
            return false, "[TransData][GetTransDataMaxMinTmpSize] shapeInfo n,d,h,w is not matched.");
        ASCENDC_HOST_ASSERT(
            srcDims[5] == c0 && srcDims[2] * c0 == AlignUp(param.c, c0), return false,
            "[TransData][GetTransDataMaxMinTmpSize]  src c0, c1 is not able to be converted to c.");
        return true;
    }
    return false;
}

int32_t GetTmpBufferSize(const TransDataConfig& config, const TmpTransDataParams& param)
{
    constexpr int32_t dataSize = 2;
    int32_t n = param.n, c = param.c, d = param.d, h = param.h, w = param.w;
    constexpr int32_t c0 = 16, n0 = 16;
    int32_t c1 = DivCeil(c, c0), n1 = DivCeil(n, n0);
    int32_t padHw = AlignUp(h * w, 16);
    if (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::NDC1HWC0) {
        return d * padHw * dataSize + d * c1 * c0 * padHw * dataSize;
    }
    if (config.srcFormat == DataFormat::NDC1HWC0 && config.dstFormat == DataFormat::NCDHW) {
        constexpr int32_t redundantDataBuffer = 512;
        return d * c1 * c0 * padHw * dataSize + redundantDataBuffer;
    }
    if (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::FRACTAL_Z_3D) {
        auto tmpDupBufferSize = std::max(c * d * padHw * dataSize, d * h * w * n1 * n0 * dataSize);
        return tmpDupBufferSize + n1 * n0 * d * c1 * c0 * padHw * dataSize;
    }
    if (config.srcFormat == DataFormat::FRACTAL_Z_3D && config.dstFormat == DataFormat::NCDHW) {
        constexpr int32_t doubleTmpSize = 2;
        if (n == n0 * n1 && c == c0 * c1) {
            return n1 * n0 * c1 * c0 * d * padHw * dataSize;
        }
        return n1 * n0 * c1 * c0 * d * padHw * dataSize * doubleTmpSize;
    }
    return 0;
}
} // namespace

bool GetTransDataMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& platform, const AscendC::TensorShape& srcShape,
    const AscendC::TensorShape& dstShape, const AscendC::TensorDataType dataType, const TransDataConfig& config,
    uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(
        dataType == AscendC::TensorDataType::DT_FLOAT16 || dataType == AscendC::TensorDataType::DT_BF16 ||
            dataType == AscendC::TensorDataType::DT_UINT16 || dataType == AscendC::TensorDataType::DT_INT16,
        return false,
        "[TransData][GetTransDataMaxMinTmpSize] it only supports DT_FLOAT16/DT_BF16/DT_UINT16/DT_INT16 data type");

    ASCENDC_HOST_ASSERT(
        ((config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::FRACTAL_Z_3D) ||
         (config.srcFormat == DataFormat::FRACTAL_Z_3D && config.dstFormat == DataFormat::NCDHW) ||
         (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::NDC1HWC0) ||
         (config.srcFormat == DataFormat::NDC1HWC0 && config.dstFormat == DataFormat::NCDHW)),
        return false,
        "[TransData][GetTransDataMaxMinTmpSize] The parameter config srcFormat/dstFormat only supports "
        "(NCDHW, FRACTAL_Z_3D)/(FRACTAL_Z_3D, NCDHW)/(NCDHW, NDC1HWC0)/(NDC1HWC0, NCDHW)!");

    const auto npuArch = platform.GetCurNpuArch();
    ;
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_2201 || npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102 ||
         npuArch == NpuArch::DAV_3003 || npuArch == NpuArch::DAV_3113),
        return false, "[TransData][GetTransDataMaxMinTmpSize] Unsupported NpuArch for TransData API.");

    TmpTransDataParams tmpParam;

    ASCENDC_HOST_ASSERT(
        GenerateShapeInfo(config, srcShape, dstShape, dataType, tmpParam), return false,
        "[TransData][GetTransDataMaxMinTmpSize] failed to validate inputs informations.");
    maxValue = GetTmpBufferSize(config, tmpParam);
    minValue = maxValue;
    return true;
}
} // namespace AscendC
