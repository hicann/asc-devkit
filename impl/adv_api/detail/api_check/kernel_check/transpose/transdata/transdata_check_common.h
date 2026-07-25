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
 * \file transdata_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/transpose/transdata/transdata_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/transpose/transdata.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSDATA_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_TRANSPOSE_TRANSDATA_TRANSDATA_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_TRANSPOSE_TRANSDATA_TRANSDATA_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {

template <const TransDataConfig& config, typename T, typename U, typename S>
class CheckFuncClassTransData : public CalCountCheckFuncBasicClass,
                                public DataTypeCheckFuncBasicClass,
                                public MultipleTensorCheckFuncBasicClass,
                                public ReuseSourceCheckFuncBasicClass,
                                public SingleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassTransData(){};
    __aicore__ inline CheckFuncClassTransData(__gm__ const char* apiName)
        : CalCountCheckFuncBasicClass(apiName),
          DataTypeCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName)
    {
        this->apiName = apiName;
    };

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const TransDataParams<U, S>& params)
    {
        CheckTransDataConfig();
        CheckTransDataLayoutShapeAndSize(dstTensor, srcTensor, params);

        SingleTensorCheckFuncBasicClass::TPositionVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
    };

private:
    int32_t c0 = 16;
    int32_t n0 = 16;
    int32_t hw0 = 16;
    int32_t n = 0;
    int32_t c = 0;
    int32_t d = 0;
    int32_t h = 0;
    int32_t w = 0;
    __gm__ const char* apiName = nullptr;
    __aicore__ __gm__ const char* DataFormatToString(DataFormat format)
    {
        switch (format) {
            case DataFormat::ND:
                return "ND";
            case DataFormat::NZ:
                return "NZ";
            case DataFormat::NCHW:
                return "NCHW";
            case DataFormat::NC1HWC0:
                return "NC1HWC0";
            case DataFormat::NHWC:
                return "NHWC";
            case DataFormat::NCDHW:
                return "NCDHW";
            case DataFormat::NDC1HWC0:
                return "NDC1HWC0";
            case DataFormat::FRACTAL_Z_3D:
                return "FRACTAL_Z_3D";
            default:
                return "Unknown";
        }
    }

    __aicore__ inline void CheckTransDataConfig()
    {
        ASCENDC_ASSERT(
            (((config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::FRACTAL_Z_3D) ||
              (config.srcFormat == DataFormat::FRACTAL_Z_3D && config.dstFormat == DataFormat::NCDHW) ||
              (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::NDC1HWC0) ||
              (config.srcFormat == DataFormat::NDC1HWC0 && config.dstFormat == DataFormat::NCDHW)) ||
             HighLevelAPIParametersPrint),
            {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[%s] The parameter config srcFormat/dstFormat is (%s, %s), expect "
                    "is (NCDHW, FRACTAL_Z_3D)/(FRACTAL_Z_3D, NCDHW)/(NCDHW, NDC1HWC0)/(NDC1HWC0, NCDHW)!",
                    this->apiName, DataFormatToString(config.srcFormat), DataFormatToString(config.dstFormat));
            });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The parameter config srcFormat is %s!", apiName,
                DataFormatToString(config.srcFormat));
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The parameter config dstFormat is %s!", apiName,
                DataFormatToString(config.dstFormat));
        }
    }

    template <typename ncdhwType>
    __aicore__ inline void CheckTransDataNcdhwShapeAndSize(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const TransDataParams<U, S>& params)
    {
        ncdhwType ncdhwShape;
        uint32_t ncdhwTensorSize;
        if constexpr (config.srcFormat == DataFormat::NCDHW) {
            ncdhwShape = params.srcLayout.GetShape();
            ncdhwTensorSize = srcTensor.GetSize();
        } else {
            ncdhwShape = params.dstLayout.GetShape();
            ncdhwTensorSize = dstTensor.GetSize();
        }
        this->n = Std::get<0>(ncdhwShape);
        this->c = Std::get<1>(ncdhwShape);
        this->d = Std::get<2>(ncdhwShape);
        this->h = Std::get<3>(ncdhwShape);
        this->w = Std::get<4>(ncdhwShape);
        int32_t hw1 = (h * w + hw0 - 1) / hw0;
        uint32_t expectSrcSize = n * c * d * hw1 * hw0;
        ASCENDC_ASSERT(((ncdhwTensorSize >= expectSrcSize) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] NCDHW format Tensor size must be greater than or equal to %u, current size is %u!",
                this->apiName, expectSrcSize, ncdhwTensorSize);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The NCDHW Layout Tensor size is %u!", this->apiName, ncdhwTensorSize);
        }
    }

    template <typename fractalType>
    __aicore__ inline void CheckTransDataFractalShapeAndSize(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const TransDataParams<U, S>& params)
    {
        fractalType fractalShape;
        uint32_t fractalTensorSize;
        if constexpr (config.srcFormat == DataFormat::FRACTAL_Z_3D) {
            fractalShape = params.srcLayout.GetShape();
            fractalTensorSize = srcTensor.GetSize();
        } else {
            fractalShape = params.dstLayout.GetShape();
            fractalTensorSize = dstTensor.GetSize();
        }

        int32_t fractalD = Std::get<0>(fractalShape);
        int32_t fractalC1 = Std::get<1>(fractalShape);
        int32_t fractalH = Std::get<2>(fractalShape);
        int32_t fractalW = Std::get<3>(fractalShape);
        int32_t fractalN1 = Std::get<4>(fractalShape);
        int32_t fractalN0 = Std::get<5>(fractalShape);
        int32_t fractalC0 = Std::get<6>(fractalShape);
        ASCENDC_ASSERT((((d == fractalD) && (h == fractalH) && (w == fractalW)) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] The dims of params srcLayout/dstLayout d, h, w should be equal, but "
                "actually are (%d, %d, %d)/(%d, %d, %d)!",
                this->apiName, d, h, w, fractalD, fractalH, fractalW);
        });
        ASCENDC_ASSERT(((fractalC1 == (c + fractalC0 - 1) / fractalC0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] The dims of params srcLayout/dstLayout c0, c1, c "
                "should satisfy the formula c1 = (c + c0 - 1) / c0, but actually are c0=%d, c1=%d, c=%d!",
                this->apiName, fractalC0, fractalC1, c);
        });
        ASCENDC_ASSERT(((fractalN1 == (n + fractalN0 - 1) / fractalN0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] The dims of params srcLayout/dstLayout n0, n1, n "
                "should satisfy the formula n1 = (n + n0 - 1) / n0, but actually are n0=%d, n1=%d, n=%d!",
                this->apiName, fractalN0, fractalN1, n);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The NCDHW Layout Shape is (%d, %d, %d, %d, %d)!", this->apiName, n, c, d, h, w);
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The FRACTAL_Z_3D Layout Shape is (%d, %d, %d, %d, %d, %d, %d)!", this->apiName,
                fractalD, fractalC1, fractalH, fractalW, fractalN1, fractalN0, fractalC0);
        }
        uint32_t expectFractalSize = fractalD * fractalC1 * fractalH * fractalW * fractalN1 * fractalN0 * fractalC0;
        ASCENDC_ASSERT(((fractalTensorSize >= expectFractalSize) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] FRACTAL_Z_3D format Tensor size must be greater than or equal to %u, current size is %u!",
                this->apiName, expectFractalSize, fractalTensorSize);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The FRACTAL_Z_3D Layout Tensor size is %u!", this->apiName, fractalTensorSize);
        }
    }

    template <typename ndc1hwc0Type>
    __aicore__ inline void CheckTransDataNdc1hwc0ShapeAndSize(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const TransDataParams<U, S>& params)
    {
        ndc1hwc0Type ndc1hwc0Shape;
        uint32_t ndc1hwc0TensorSize;
        if constexpr (config.srcFormat == DataFormat::NDC1HWC0) {
            ndc1hwc0Shape = params.srcLayout.GetShape();
            ndc1hwc0TensorSize = srcTensor.GetSize();
        } else {
            ndc1hwc0Shape = params.dstLayout.GetShape();
            ndc1hwc0TensorSize = dstTensor.GetSize();
        }
        int32_t ndc1hwc0N = Std::get<0>(ndc1hwc0Shape);
        int32_t ndc1hwc0D = Std::get<1>(ndc1hwc0Shape);
        int32_t ndc1hwc0C1 = Std::get<2>(ndc1hwc0Shape);
        int32_t ndc1hwc0H = Std::get<3>(ndc1hwc0Shape);
        int32_t ndc1hwc0W = Std::get<4>(ndc1hwc0Shape);
        int32_t ndc1hwc0C0 = Std::get<5>(ndc1hwc0Shape);
        uint32_t expectNdc1hwc0Size = ndc1hwc0N * ndc1hwc0D * ndc1hwc0C1 * ndc1hwc0H * ndc1hwc0W * ndc1hwc0C0;

        ASCENDC_ASSERT(
            (((n == ndc1hwc0N) && (d == ndc1hwc0D) && (h == ndc1hwc0H) && (w == ndc1hwc0W)) ||
             HighLevelAPIParametersPrint),
            {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[%s] The dims of params srcLayout/dstLayout n, d, h, w should be equal, but "
                    "actually are (%d, %d, %d, %d)/(%d, %d, %d, %d)!",
                    this->apiName, n, d, h, w, ndc1hwc0N, ndc1hwc0D, ndc1hwc0H, ndc1hwc0W);
            });
        ASCENDC_ASSERT(((ndc1hwc0C1 == (c + ndc1hwc0C0 - 1) / ndc1hwc0C0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] The dims of params srcLayout/dstLayout c0, c1, c "
                "should satisfy the formula c1 = (c + c0 - 1) / c0, but actually are c0=%d, c1=%d, c=%d!",
                this->apiName, ndc1hwc0C0, ndc1hwc0C1, c);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The NCDHW Layout Shape is (%d, %d, %d, %d, %d)!", this->apiName, n, c, d, h, w);
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The NDC1HWC0 Layout Shape is (%d, %d, %d, %d, %d, %d)!", this->apiName, ndc1hwc0N,
                ndc1hwc0D, ndc1hwc0C1, ndc1hwc0H, ndc1hwc0W, ndc1hwc0C0);
        }

        ASCENDC_ASSERT(((ndc1hwc0TensorSize >= expectNdc1hwc0Size) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] NDC1HWC0 format Tensor size must be greater than or equal to %u, current size is %u!",
                this->apiName, expectNdc1hwc0Size, ndc1hwc0TensorSize);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The NDC1HWC0 Layout Tensor size is %u!", this->apiName, ndc1hwc0TensorSize);
        }
    }

    __aicore__ inline void CheckTransDataLayoutShapeAndSize(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const TransDataParams<U, S>& params)
    {
        auto srcShape = params.srcLayout.GetShape();
        auto dstShape = params.dstLayout.GetShape();
        using srcType = decltype(srcShape);
        using dstType = decltype(dstShape);
        using ncdhwType = Std::conditional_t<config.srcFormat == DataFormat::NCDHW, srcType, dstType>;

        CheckTransDataNcdhwShapeAndSize<ncdhwType>(dstTensor, srcTensor, params);
        if constexpr (config.srcFormat == DataFormat::FRACTAL_Z_3D || config.dstFormat == DataFormat::FRACTAL_Z_3D) {
            using fractalType = Std::conditional_t<config.srcFormat == DataFormat::FRACTAL_Z_3D, srcType, dstType>;
            CheckTransDataFractalShapeAndSize<fractalType>(dstTensor, srcTensor, params);
        } else if constexpr (config.srcFormat == DataFormat::NDC1HWC0 || config.dstFormat == DataFormat::NDC1HWC0) {
            using ndc1hwc0Type = Std::conditional_t<config.srcFormat == DataFormat::NDC1HWC0, srcType, dstType>;
            CheckTransDataNdc1hwc0ShapeAndSize<ndc1hwc0Type>(dstTensor, srcTensor, params);
        }
    }
};
} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_TRANSPOSE_TRANSDATA_TRANSDATA_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSDATA_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSDATA_CHECK_COMMON_H__
#endif
