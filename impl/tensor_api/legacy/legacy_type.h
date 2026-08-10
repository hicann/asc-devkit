/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_TENSOR_API_LEGACY_LEGACY_TYPE_H
#define IMPL_TENSOR_API_LEGACY_LEGACY_TYPE_H

#include "impl/tensor_api/legacy/legacy_utils.h"

namespace asc {
namespace te {

namespace location {
using INVALID = invalid;
using GM = gm;
using UB = ub;
using L1 = l1;
using L0A = l0a;
using L0B = l0b;
using L0ScaleA = l0scalea;
using L0ScaleB = l0scaleb;
using L0C = l0c;
using BIAS = bias;
using FIXBUF = fixbuf;
using SSBUF = ssbuf;
} // namespace location
namespace Location = location;

template <typename... Shapes>
using Shape = shape_type<Shapes...>;

template <typename... Strides>
using Stride = stride_type<Strides...>;

template <typename... Coords>
using Coord = coord_type<Coords...>;

template <typename T, typename U, typename Info = Std::ignore_t>
using Layout = layout_type<T, U, Info>;

template <typename T = uint16_t, typename C0 = Std::Int<C0_ELEMENT<T>>>
using LayoutTraitDefault = layout_trait_default<T, C0>;

template <typename T>
using GetLayoutPattern = get_layout_pattern<T>;

using ZNLayoutPtn = zn_layout_ptn;
using ZZLayoutPtn = zz_layout_ptn;
using NNLayoutPtn = nn_layout_ptn;
using NZLayoutPtn = nz_layout_ptn;
using NDLayoutPtn = nd_layout_ptn;
using DNLayoutPtn = dn_layout_ptn;
using NDExtLayoutPtn = nd_ext_layout_ptn;
using DNExtLayoutPtn = dn_ext_layout_ptn;
using ScaleANDLayoutPtn = scalea_nd_layout_ptn;
using ScaleADNLayoutPtn = scalea_dn_layout_ptn;
using ScaleBNDLayoutPtn = scaleb_nd_layout_ptn;
using ScaleBDNLayoutPtn = scaleb_dn_layout_ptn;
using NCHWLayoutPtn = nchw_layout_ptn;
using NHWCLayoutPtn = nhwc_layout_ptn;
using NC1HWC0LayoutPtn = nc1hwc0_layout_ptn;
using NCDHWLayoutPtn = ncdhw_layout_ptn;
using NDC1HWC0LayoutPtn = ndc1hwc0_layout_ptn;

template <typename Iterator>
using ViewEngine = view_engine<Iterator>;

using CopyGM2L1 = copy_gm_to_l1;
using CopyL12UB = copy_l1_to_ub;
using CopyL12L0B = copy_l1_to_l0b;
using CopyL12L0A = copy_l1_to_l0a;
using CopyL12L0ScaleA = copy_l1_to_l0scalea;
using CopyL12L0ScaleB = copy_l1_to_l0scaleb;
using CopyL12FB = copy_l1_to_fixbuf;
using CopyL12BT = copy_l1_to_biastable;
using CopyL0C2GM = copy_l0c_to_gm;
using CopyL0C2UB = copy_l0c_to_ub;
using CopyL0C2L1 = copy_l0c_to_l1;
using CopyGM2L1Trait = copy_gm_to_l1_trait;
using CopyGM2L1TraitDefault = copy_gm_to_l1_trait_default;
using CopyL12UBTrait = copy_l1_to_ub_trait;
using CopyL12UBTraitDefault = copy_l1_to_ub_trait_default;
using CopyL12L0ATrait = copy_l1_to_l0a_trait;
using CopyL12L0ATraitDefault = copy_l1_to_l0a_trait_default;
using CopyL12L0BTrait = copy_l1_to_l0b_trait;
using CopyL12L0BTraitDefault = copy_l1_to_l0b_trait_default;
using CopyL12L0ScaleATrait = copy_l1_to_l0scalea_trait;
using CopyL12L0ScaleATraitDefault = copy_l1_to_l0scalea_trait_default;
using CopyL12L0ScaleBTrait = copy_l1_to_l0scaleb_trait;
using CopyL12L0ScaleBTraitDefault = copy_l1_to_l0scaleb_trait_default;
using CopyL12FBTrait = copy_l1_to_fixbuf_trait;
using CopyL12FBTraitDefault = copy_l1_to_fixbuf_trait_default;
using CopyL12BTTrait = copy_l1_to_biastable_trait;
using CopyL12BTTraitDefault = copy_l1_to_biastable_trait_default;
using CopyL0C2GMTraitDefault = copy_l0c_to_gm_trait_default;
using CopyL0C2UBTraitDefault = copy_l0c_to_ub_trait_default;
using CopyL0C2L1TraitDefault = copy_l0c_to_l1_trait_default;

using CopyGM2UB = copy_gm_to_ub;
using CopyUB2L1 = copy_ub_to_l1;
using CopyUB2GM = copy_ub_to_gm;
using CopyUB2UB = copy_ub_to_ub;
using CopyGM2UBTrait = copy_gm_to_ub_trait;
using CopyGM2UBTraitDefault = copy_gm_to_ub_trait_default;
using CopyUB2L1Trait = copy_ub_to_l1_trait;
using CopyUB2L1TraitDefault = copy_ub_to_l1_trait_default;
using CopyUB2GMTrait = copy_ub_to_gm_trait;
using CopyUB2GMTraitDefault = copy_ub_to_gm_trait_default;
using CopyUB2UBTrait = copy_ub_to_ub_trait;
using CopyUB2UBTraitDefault = copy_ub_to_ub_trait_default;

using MmadTraitDefault = mmad_trait_default;
using MmadOperation = mmad_operation;

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_LEGACY_LEGACY_TYPE_H
