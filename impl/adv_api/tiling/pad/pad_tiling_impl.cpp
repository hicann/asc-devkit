/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file pad_tiling_impl.cpp
 * \brief
 */

#include "adv_api/utils/types.h"
#include "adv_api/pad/pad_tiling.h"
#include "../../detail/host_log.h"

namespace optiling {
REGISTER_TILING_DATA_CLASS(UnPadTilingOpApi, UnPadTiling);
REGISTER_TILING_DATA_CLASS(PadTilingOpApi, PadTiling);
} // namespace optiling

namespace AscendC {
namespace {
const uint32_t PAD_ONE_BLK_SIZE = 32;
const uint32_t PAD_MAX_REPEAT_TIMES = 255;
const uint32_t PAD_NCHW_CONV_ADDR_LIST_SIZE = 16;
const uint32_t TMP_BUFFER_NUM = 2;
constexpr uint32_t PAD_DIM_TWO = 2;
constexpr uint32_t PAD_TYPE_TWO = 2;
constexpr uint32_t PAD_TYPE_FOUR = 4;

void CheckPadMaxMinTmpSizeParams(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const char* funcName)
{
    ASCENDC_HOST_ASSERT(
        typeSize == PAD_TYPE_TWO || typeSize == PAD_TYPE_FOUR, continue,
        "[Pad][%s] The value of typeSize is %u, should be 2 or 4.", funcName, typeSize);

    ASCENDC_HOST_ASSERT(
        srcShape.GetDimNum() == PAD_DIM_TWO, continue, "[Pad][%s] The dims of srcShape is %zu, should be 2.", funcName,
        srcShape.GetDimNum());
}

void CheckPadTilingFuncParams(
    const AscendC::TensorShape srcShape, const AscendC::TensorShape oriSrcShape, const uint32_t stackBufferSize,
    const uint32_t typeSize, const char* funcName)
{
    (void)stackBufferSize;
    CheckPadMaxMinTmpSizeParams(srcShape, typeSize, funcName);

    ASCENDC_HOST_ASSERT(
        oriSrcShape.GetDimNum() == PAD_DIM_TWO, continue, "[Pad][%s] The dims of oriSrcShape is %zu, should be 2.",
        funcName, oriSrcShape.GetDimNum());
    ASCENDC_HOST_ASSERT(
        oriSrcShape.GetShapeSize() <= srcShape.GetShapeSize(), continue,
        "[Pad][%s] The size of oriSrcShape is %li, should be less than or equal to srcShape size %li.", funcName,
        oriSrcShape.GetShapeSize(), srcShape.GetShapeSize());
}

void CheckUnPadParams(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const char* funcName)
{
    ASCENDC_HOST_ASSERT(
        typeSize == PAD_TYPE_TWO || typeSize == PAD_TYPE_FOUR, continue,
        "[UnPad][%s] The value of typeSize is %u, should be 2 or 4.", funcName, typeSize);

    ASCENDC_HOST_ASSERT(
        srcShape.GetDimNum() == PAD_DIM_TWO, continue, "[UnPad][%s] The dims of srcShape is %zu, should be 2.",
        funcName, srcShape.GetDimNum());
}

void CheckGetUnPadMaxMinTmpSizeParams(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const char* funcName)
{
    CheckUnPadParams(srcShape, typeSize, funcName);
}

} // namespace

void GetPadMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
{
    CheckPadMaxMinTmpSizeParams(srcShape, typeSize, "GetPadMaxMinTmpSize");
    ASCENDC_HOST_ASSERT(typeSize > 0, return, "typeSize must be greater than 0.");

    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t srcHeight = shapeDims[0];
    uint32_t srcWidth = shapeDims[1];

    maxValue = ((srcHeight * srcWidth - 1) / PAD_NCHW_CONV_ADDR_LIST_SIZE + 1) * PAD_NCHW_CONV_ADDR_LIST_SIZE *
               (PAD_ONE_BLK_SIZE / typeSize) * TMP_BUFFER_NUM * typeSize / sizeof(uint8_t);
    minValue =
        PAD_NCHW_CONV_ADDR_LIST_SIZE * (PAD_ONE_BLK_SIZE / typeSize) * TMP_BUFFER_NUM * typeSize / sizeof(uint8_t);
}

void PadTilingFunc(
    const AscendC::TensorShape srcShape, const AscendC::TensorShape oriSrcShape, const uint32_t stackBufferSize,
    const uint32_t typeSize, optiling::PadTiling& tiling)
{
    CheckPadTilingFuncParams(srcShape, oriSrcShape, stackBufferSize, typeSize, "PadTilingFunc");
    ASCENDC_HOST_ASSERT(typeSize > 0, return, "typeSize must be greater than 0.");
    // common
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    std::vector<int64_t> oriShapeDims = oriSrcShape.GetDims();

    uint32_t srcHeight = shapeDims[0];
    uint32_t srcWidth = shapeDims[1];
    uint32_t srcOriWidth = oriShapeDims[1];
    tiling.set_srcHeight(srcHeight);
    tiling.set_srcWidth(srcWidth);
    tiling.set_srcOriWidth(srcOriWidth);

    // width 32B aligned
    uint32_t widthWithoutLastBlock = srcWidth - PAD_ONE_BLK_SIZE / typeSize;
    tiling.set_widthWithoutLastBlock(widthWithoutLastBlock);

    uint32_t blocksPerRow = srcWidth * typeSize / PAD_ONE_BLK_SIZE;
    tiling.set_blocksPerRow(blocksPerRow);

    uint32_t heightTiling = PAD_MAX_REPEAT_TIMES;
    uint32_t heightFractal = srcHeight / heightTiling;
    uint32_t heightFractalTail = srcHeight % heightTiling;
    tiling.set_heightTiling(heightTiling);
    tiling.set_heightFractal(heightFractal);
    tiling.set_heightFractalTail(heightFractalTail);

    uint32_t mainLoopOffset = heightTiling * srcWidth;
    uint32_t tailBlockOffset = heightFractal * heightTiling * srcWidth + widthWithoutLastBlock;
    tiling.set_mainLoopOffset(mainLoopOffset);
    tiling.set_tailBlockOffset(tailBlockOffset);

    // no need tmp
    if (stackBufferSize == 0) {
        return;
    }

    // width 32B unaligned
    uint32_t baseBlockLen = PAD_NCHW_CONV_ADDR_LIST_SIZE * PAD_ONE_BLK_SIZE; // Byte; x multiply 32B, for brcb
    uint32_t baseBlockSize = baseBlockLen / typeSize;                        // ele
    uint32_t tmpBuffer1BlockNum = stackBufferSize * sizeof(uint8_t) / typeSize / baseBlockSize / 2;
    uint32_t tmpBuffer2Offset = tmpBuffer1BlockNum * baseBlockSize;
    tiling.set_tmpBuffer1RowNum(PAD_NCHW_CONV_ADDR_LIST_SIZE * tmpBuffer1BlockNum);
    tiling.set_tmpBuffer1BlockNum(tmpBuffer1BlockNum);
    tiling.set_tmpBuffer2Offset(tmpBuffer2Offset);

    uint32_t widthTiling = PAD_NCHW_CONV_ADDR_LIST_SIZE * tmpBuffer1BlockNum; // elements
    uint32_t widthFractalTail = srcWidth % widthTiling;
    uint32_t widthFractal = srcWidth / widthTiling;
    // aligned to 8 or 16
    uint32_t widthFractalTailAlingned =
        ((widthFractalTail - 1) / (PAD_ONE_BLK_SIZE / typeSize) + 1) * (PAD_ONE_BLK_SIZE / typeSize);

    tiling.set_widthTiling(widthTiling);
    tiling.set_widthFractal(widthFractal);
    tiling.set_widthFractalTail(widthFractalTail);
    tiling.set_widthFractalTailAlingned(widthFractalTailAlingned);

    uint32_t brcbTiling = PAD_NCHW_CONV_ADDR_LIST_SIZE * tmpBuffer1BlockNum; // elements
    uint32_t brcbFractal = srcHeight * srcWidth / brcbTiling;
    uint32_t brcbFractalTail = srcHeight * srcWidth % brcbTiling;
    tiling.set_brcbTiling(brcbTiling);
    tiling.set_brcbFractal(brcbFractal);
    tiling.set_brcbFractalTail(brcbFractalTail);

    uint32_t maxRepeatTimes = 254; // 255*8 not 32B aligned, so 254
    uint32_t brcbTilingRepeatTimes = brcbTiling / 8 / maxRepeatTimes;
    uint32_t brcbTilingRepeatTimesTail = brcbTiling / 8 % maxRepeatTimes;
    uint32_t brcbFractalTailBrcbRepeatTimes = (brcbFractalTail + 7) / 8;
    uint32_t brcbFractalTailRepeatTimes = brcbFractalTailBrcbRepeatTimes / maxRepeatTimes;
    uint32_t brcbFractalTailRepeatTimesTail = brcbFractalTailBrcbRepeatTimes % maxRepeatTimes;
    tiling.set_maxRepeatTimes(maxRepeatTimes);
    tiling.set_brcbTilingRepeatTimes(brcbTilingRepeatTimes);
    tiling.set_brcbTilingRepeatTimesTail(brcbTilingRepeatTimesTail);
    tiling.set_brcbFractalTailRepeatTimes(brcbFractalTailRepeatTimes);
    tiling.set_brcbFractalTailRepeatTimesTail(brcbFractalTailRepeatTimesTail);
}

void PadTilingFunc(
    const AscendC::TensorShape srcShape, const AscendC::TensorShape oriSrcShape, const uint32_t stackBufferSize,
    const uint32_t typeSize, AscendC::tiling::PadTiling& tiling)
{
    optiling::PadTiling tilingData;
    PadTilingFunc(srcShape, oriSrcShape, stackBufferSize, typeSize, tilingData);
    tilingData.SaveToBuffer(&tiling, sizeof(PadTiling));
}

void GetUnPadMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,
    const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
{
    CheckGetUnPadMaxMinTmpSizeParams(srcShape, typeSize, "GetUnPadMaxMinTmpSize");
    ASCENDC_HOST_ASSERT(typeSize > 0, return, "typeSize must be greater than 0.");

    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t srcHeight = shapeDims[0];
    uint32_t srcWidth = shapeDims[1];

    auto npuArch = ascendcPlatform.GetCurNpuArch();
    if (npuArch == NpuArch::DAV_2201 || npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102) {
        maxValue = 0;
        minValue = 0;
    } else {
        maxValue = ((srcHeight * srcWidth - 1) / PAD_NCHW_CONV_ADDR_LIST_SIZE + 1) * PAD_NCHW_CONV_ADDR_LIST_SIZE *
                   (PAD_ONE_BLK_SIZE / typeSize) * TMP_BUFFER_NUM * typeSize / sizeof(uint8_t);
        minValue =
            PAD_NCHW_CONV_ADDR_LIST_SIZE * (PAD_ONE_BLK_SIZE / typeSize) * TMP_BUFFER_NUM * typeSize / sizeof(uint8_t);
    }
}

void UnPadTilingFunc(
    const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    optiling::UnPadTiling& tiling)
{
    CheckGetUnPadMaxMinTmpSizeParams(srcShape, typeSize, "UnPadTilingFunc");
    ASCENDC_HOST_ASSERT(typeSize > 0, return, "typeSize must be greater than 0.");

    std::vector<int64_t> shapeDims = srcShape.GetDims();

    uint32_t srcHeight = shapeDims[0];
    uint32_t srcWidth = shapeDims[1];

    tiling.set_srcHeight(srcHeight);
    tiling.set_srcWidth(srcWidth);

    // 220 no need tmp
    if (stackBufferSize == 0) {
        return;
    }

    uint32_t baseBlockLen = PAD_NCHW_CONV_ADDR_LIST_SIZE * PAD_ONE_BLK_SIZE; // Byte; x multiply 32B, for brcb
    uint32_t baseBlockSize = baseBlockLen / typeSize;                        // ele
    uint32_t tmpBuffer1BlockNum = stackBufferSize * sizeof(uint8_t) / typeSize / baseBlockSize / 2;
    uint32_t tmpBuffer2Offset = tmpBuffer1BlockNum * baseBlockSize;

    tiling.set_tmpBuffer1BlockNum(tmpBuffer1BlockNum);
    tiling.set_tmpBuffer1RowNum(PAD_NCHW_CONV_ADDR_LIST_SIZE * tmpBuffer1BlockNum);
    tiling.set_tmpBuffer2Offset(tmpBuffer2Offset);

    uint32_t widthTiling = PAD_NCHW_CONV_ADDR_LIST_SIZE * tmpBuffer1BlockNum; // elements
    uint32_t widthFractal = srcWidth / widthTiling;
    uint32_t widthFractalTail = srcWidth % widthTiling;
    if (widthFractalTail != 0) {
        widthFractal += 1;
    } else {
        widthFractalTail = widthTiling;
    }

    tiling.set_widthTiling(widthTiling);
    tiling.set_widthFractal(widthFractal);
    tiling.set_widthFractalTail(widthFractalTail);
}

void UnPadTilingFunc(
    const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    AscendC::tiling::UnPadTiling& tiling)
{
    optiling::UnPadTiling tilingData;
    UnPadTilingFunc(srcShape, stackBufferSize, typeSize, tilingData);
    tilingData.SaveToBuffer(&tiling, sizeof(UnPadTiling));
}
} // namespace AscendC
