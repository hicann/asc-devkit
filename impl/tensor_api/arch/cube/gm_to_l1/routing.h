/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cube/gm_to_l1/routing.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file routing.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_ROUTING_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_ROUTING_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/dn2nz.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/dn2zn.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nd2nd.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nd2nz.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nd2zn.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nc1hwc02nc1hwc0.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nchw2nc1hwc0.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/ncdhw2ndc1hwc0.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nhwc2nc1hwc0.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nz2nz.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/zn2zn.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scalea_nd2zz.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scalea_dn2zz.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scalea_zz2zz.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scaleb_nd2nn.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scaleb_dn2nn.h"
#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scaleb_nn2nn.h"

namespace AscendC {
namespace Te {

class CopyGM2L1Ignore {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        static_assert(!Std::is_same_v<T, T>, "CopyGM2L1: unsupported layout pattern combination.");
    }
};

template <uint32_t Version, typename DstLayoutPtn, typename SrcLayoutPtn>
struct CopyGM2L1Routing {
    using type = CopyGM2L1Ignore;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NDExtLayoutPtn, NDExtLayoutPtn> {
    using type = CopyGmToCbufAlignV2ND;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NDLayoutPtn, NDLayoutPtn> {
    using type = CopyGmToCbufAlignV2ND;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NZLayoutPtn, NDExtLayoutPtn> {
    using type = CopyGmToCbufMultiND2Nz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NZLayoutPtn, NDLayoutPtn> {
    using type = CopyGmToCbufMultiND2Nz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZNLayoutPtn, NDExtLayoutPtn> {
    using type = CopyGmToCbufMultiND2Zn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZNLayoutPtn, NDLayoutPtn> {
    using type = CopyGmToCbufMultiND2Zn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NZLayoutPtn, DNExtLayoutPtn> {
    using type = CopyGmToCbufMultiDN2Nz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NZLayoutPtn, DNLayoutPtn> {
    using type = CopyGmToCbufMultiDN2Nz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZNLayoutPtn, DNExtLayoutPtn> {
    using type = CopyGmToCbufMultiDN2Zn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZNLayoutPtn, DNLayoutPtn> {
    using type = CopyGmToCbufMultiDN2Zn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NZLayoutPtn, NZLayoutPtn> {
    using type = CopyGmToCbufAlignV2NZ;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZNLayoutPtn, ZNLayoutPtn> {
    using type = CopyGmToCbufAlignV2ZN;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZZLayoutPtn, ScaleANDLayoutPtn> {
    using type = CopyGmToCbufScaleAND2Zz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZZLayoutPtn, ScaleADNLayoutPtn> {
    using type = CopyGmToCbufScaleADN2Zz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, ZZLayoutPtn, ZZLayoutPtn> {
    using type = CopyGmToCbufScaleAZz2Zz;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NNLayoutPtn, ScaleBNDLayoutPtn> {
    using type = CopyGmToCbufScaleBND2Nn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NNLayoutPtn, ScaleBDNLayoutPtn> {
    using type = CopyGmToCbufScaleBDN2Nn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NNLayoutPtn, NNLayoutPtn> {
    using type = CopyGmToCbufScaleBNn2Nn;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NC1HWC0LayoutPtn, NC1HWC0LayoutPtn> {
    using type = CopyGmToCbufNC1HWC02NC1HWC0;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NC1HWC0LayoutPtn, NHWCLayoutPtn> {
    using type = CopyGmToCbufNHWC2NC1HWC0;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NC1HWC0LayoutPtn, NCHWLayoutPtn> {
    using type = CopyGmToCbufNCHW2NC1HWC0;
};

template <uint32_t Version>
struct CopyGM2L1Routing<Version, NDC1HWC0LayoutPtn, NCDHWLayoutPtn> {
    using type = CopyGmToCbufNCDHW2NDC1HWC0;
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_ROUTING_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
