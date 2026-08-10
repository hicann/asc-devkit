/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_TENSOR_API_LEGACY_LEGACY_ATOM_H
#define IMPL_TENSOR_API_LEGACY_LEGACY_ATOM_H

#include "impl/tensor_api/legacy/legacy_type.h"

namespace asc {
namespace te {

// MmadTraits: 独立的 PascalCase 模板，不依赖 mmad_traits
template <typename MmadOperationType, typename... MmadOpArgs>
struct MmadTraits {};

template <typename MmadOp, typename MmadTraitsType, typename MmadOpWith, typename MmadTraitsWith>
struct MmadTraits<MmadOp, MmadTraitsType, MmadOpWith, MmadTraitsWith> {
    using TraitType = get_trait_member_type_t<MmadTraitsType>;
    using trait_type = TraitType;
    static constexpr const TraitType defaultTrait = MmadTraitsType::value;

    mmad_params params;

    __aicore__ inline constexpr MmadTraits() = default;
    __aicore__ inline constexpr MmadTraits(mmad_params p) : params(p) {}

    __aicore__ inline constexpr MmadTraits<MmadOpWith, MmadTraitsWith> with() const
    {
        return {};
    }

    template <typename Params>
    __aicore__ inline constexpr MmadTraits<MmadOpWith, MmadTraitsWith> with(const Params& p) const
    {
        return {normalize_mmad_params(p)};
    }

    template <const TraitType& trait = defaultTrait, typename... Args>
    __aicore__ inline void mmad_unpack(const Args&... args) const
    {
        using normalized_t = Std::remove_cvref_t<decltype(normalize_mmad_trait(TraitType{}))>;
        if constexpr (Std::is_same_v<TraitType, normalized_t>) {
            MmadOp::template mmad<TraitType, trait, Args...>(args..., params);
        } else {
            constexpr static normalized_t normalized_trait = normalize_mmad_trait(trait);
            MmadOp::template mmad<normalized_t, normalized_trait, Args...>(args..., params);
        }
    }

    template <const TraitType& trait = defaultTrait, typename... Args>
    __aicore__ inline void call(const Args&... args) const
    {
        mmad_unpack<trait>(args...);
    }
};

// MmadTraits 2参数特化: 继承4参数
template <typename MmadOp, typename MmadTraitsType>
struct MmadTraits<MmadOp, MmadTraitsType> : public MmadTraits<MmadOp, MmadTraitsType, mmad_op_with, MmadTraitsType> {};

// MmadTraits 1参数特化: 标准操作绑定默认 trait
template <> struct MmadTraits<mmad_operation> : public MmadTraits<mmad_operation, mmad_trait_default> {};

// MmadAtom: 独立的 PascalCase 模板，不依赖 mmad_atom
template <typename... Args>
struct MmadAtom;

template <typename MmadOperationType>
struct MmadAtom<MmadOperationType> : public MmadTraits<MmadOperationType> {
    using mmad_trait_type = MmadTraits<MmadOperationType>;
    using TraitType = get_trait_member_type_t<mmad_trait_type>;
    using trait_type = TraitType;
    static constexpr const TraitType defaultTrait = mmad_trait_type::defaultTrait;

    template <const TraitType& trait = defaultTrait, typename... Params>
    __aicore__ inline void call(const Params&... params) const
    {
        mmad_trait_type::template call<trait>(params...);
    }

    template <typename... TraitsArgs>
    __aicore__ inline auto with(TraitsArgs&&... args) const
    {
        auto traits = mmad_trait_type::with(static_cast<TraitsArgs&&>(args)...);
        return MmadAtom<decltype(traits)>{traits};
    }
};

template <typename... Args>
struct MmadAtom<MmadTraits<Args...>> : public MmadTraits<Args...> {
    using mmad_trait_type = MmadTraits<Args...>;
    using TraitType = get_trait_member_type_t<mmad_trait_type>;
    using trait_type = TraitType;
    static constexpr const TraitType defaultTrait = mmad_trait_type::defaultTrait;

    template <const TraitType& trait = defaultTrait, typename... Params>
    __aicore__ inline void call(const Params&... params) const
    {
        mmad_trait_type::template call<trait>(params...);
    }

    template <typename... TraitsArgs>
    __aicore__ inline auto with(TraitsArgs&&... args) const
    {
        auto traits = mmad_trait_type::with(static_cast<TraitsArgs&&>(args)...);
        return MmadAtom<decltype(traits)>{traits};
    }
};

// normalize_copy_trait: PascalCase trait → snake_case trait 转换
template <typename Trait>
__aicore__ inline constexpr Trait normalize_copy_trait(const Trait& trait)
{
    return trait;
}

__aicore__ inline constexpr copy_l0c_to_gm_trait normalize_copy_trait(const CopyL0C2GMTrait& trait)
{
    return {normalize_round_mode(trait.roundMode), trait.enableRelu, trait.enableChannelSplit};
}

__aicore__ inline constexpr copy_l0c_to_ub_trait normalize_copy_trait(const CopyL0C2UBTrait& trait)
{
    return {normalize_round_mode(trait.roundMode), trait.enableRelu, trait.enableChannelSplit,
            normalize_dual_dst_mode(trait.dualDstCtl)};
}

__aicore__ inline constexpr copy_l0c_to_l1_trait normalize_copy_trait(const CopyL0C2L1Trait& trait)
{
    return {normalize_round_mode(trait.roundMode), trait.enableRelu, trait.enableChannelSplit};
}

// 检测 CopyOp 是否有 snake_case copy 方法
template <typename AlwaysVoid, typename Op, typename Trait, const Trait& T, typename... Args>
struct has_copy_method : Std::false_type {};

template <typename Op, typename Trait, const Trait& T, typename... Args>
struct has_copy_method<void_t<decltype(Op::template copy<Trait, T, Args...>(std::declval<Args>()...))>,
                       Op, Trait, T, Args...> : Std::true_type {};

// CopyTraits: 独立的 PascalCase 模板，不依赖 copy_traits
template <typename CopyOperation, typename... CopyOpArgs>
struct CopyTraits {};

template <typename CopyOp, typename Traits, typename CopyOpWith, typename TraitsWith>
struct CopyTraits<CopyOp, Traits, CopyOpWith, TraitsWith> {
    using TraitType = get_trait_member_type_t<Traits>;
    using trait_type = TraitType;
    static constexpr const TraitType defaultTrait = Traits::value;

    fixpipe_params params;

    __aicore__ inline constexpr CopyTraits() = default;
    __aicore__ inline constexpr CopyTraits(fixpipe_params p) : params(p) {}

    __aicore__ inline constexpr CopyTraits<CopyOpWith, TraitsWith> with() const
    {
        return {};
    }

    template <typename Params>
    __aicore__ inline constexpr CopyTraits<CopyOpWith, TraitsWith> with(const Params& p) const
    {
        using params_type = Std::remove_cvref_t<Params>;
        if constexpr (Std::is_same_v<params_type, fixpipe_params> || Std::is_same_v<params_type, FixpipeParams>) {
            return {normalize_fixpipe_params(p)};
        } else if constexpr (Std::is_same_v<params_type, copy_gm_to_ub_params>
                             || Std::is_same_v<params_type, CopyGM2UBParams>) {
            return {normalize_copy_gm_to_ub_params(p)};
        } else {
            return {p};
        }
    }

    template <const TraitType& trait = defaultTrait, typename... Args>
    __aicore__ inline void copy_unpack(const Args&... args) const
    {
        using normalized_t = Std::remove_cvref_t<decltype(normalize_copy_trait(Traits::value))>;
        if constexpr (Std::is_same_v<TraitType, normalized_t>) {
            if constexpr (has_copy_method<void, CopyOp, normalized_t, trait, Args...>::value) {
                CopyOp::template copy<normalized_t, trait, Args...>(args...);
            } else {
                CopyOp::template Copy<normalized_t, trait, Args...>(args...);
            }
        } else {
            constexpr static normalized_t normalized_trait = normalize_copy_trait(trait);
            if constexpr (has_copy_method<void, CopyOp, normalized_t, normalized_trait, Args...>::value) {
                CopyOp::template copy<normalized_t, normalized_trait, Args...>(args...);
            } else {
                CopyOp::template Copy<normalized_t, normalized_trait, Args...>(args...);
            }
        }
    }

    template <const TraitType& trait = defaultTrait, typename... Args>
    __aicore__ inline void call(const Args&... args) const
    {
        copy_unpack<trait>(args...);
    }
};

// CopyTraits 2参数特化: 继承4参数
template <typename CopyOp, typename Traits>
struct CopyTraits<CopyOp, Traits> : public CopyTraits<CopyOp, Traits, CopyOp, Traits> {};

// CopyTraits 1参数特化: 标准操作绑定默认 trait
template <> struct CopyTraits<copy_gm_to_l1> : public CopyTraits<copy_gm_to_l1, copy_gm_to_l1_trait_default> {};
template <> struct CopyTraits<copy_gm_to_ub> : public CopyTraits<copy_gm_to_ub, copy_gm_to_ub_trait_default> {};
template <> struct CopyTraits<copy_ub_to_gm> : public CopyTraits<copy_ub_to_gm, copy_ub_to_gm_trait_default> {};
template <> struct CopyTraits<copy_ub_to_l1> : public CopyTraits<copy_ub_to_l1, copy_ub_to_l1_trait_default> {};
template <> struct CopyTraits<copy_ub_to_ub> : public CopyTraits<copy_ub_to_ub, copy_ub_to_ub_trait_default> {};
template <> struct CopyTraits<copy_l1_to_ub> : public CopyTraits<copy_l1_to_ub, copy_l1_to_ub_trait_default> {};
template <> struct CopyTraits<copy_l0c_to_gm> : public CopyTraits<copy_l0c_to_gm, copy_l0c_to_gm_trait_default> {};
template <> struct CopyTraits<copy_l0c_to_ub> : public CopyTraits<copy_l0c_to_ub, copy_l0c_to_ub_trait_default> {};
template <> struct CopyTraits<copy_l0c_to_l1> : public CopyTraits<copy_l0c_to_l1, copy_l0c_to_l1_trait_default> {};
template <> struct CopyTraits<copy_l1_to_l0a> : public CopyTraits<copy_l1_to_l0a, copy_l1_to_l0a_trait_default> {};
template <> struct CopyTraits<copy_l1_to_l0b> : public CopyTraits<copy_l1_to_l0b, copy_l1_to_l0b_trait_default> {};
template <> struct CopyTraits<copy_l1_to_l0scalea> : public CopyTraits<copy_l1_to_l0scalea, copy_l1_to_l0scalea_trait_default> {};
template <> struct CopyTraits<copy_l1_to_l0scaleb> : public CopyTraits<copy_l1_to_l0scaleb, copy_l1_to_l0scaleb_trait_default> {};
template <> struct CopyTraits<copy_l1_to_fixbuf> : public CopyTraits<copy_l1_to_fixbuf, copy_l1_to_fixbuf_trait_default> {};
template <> struct CopyTraits<copy_l1_to_biastable> : public CopyTraits<copy_l1_to_biastable, copy_l1_to_biastable_trait_default> {};

// CopyAtom: 独立的 PascalCase 模板，不依赖 copy_atom
template <typename... Args>
struct CopyAtom;

template <typename CopyOperation>
struct CopyAtom<CopyOperation> : public CopyTraits<CopyOperation> {
    using copy_trait_type = CopyTraits<CopyOperation>;
    using TraitType = get_trait_member_type_t<copy_trait_type>;
    using trait_type = TraitType;
    static constexpr const TraitType defaultTrait = copy_trait_type::defaultTrait;

    template <const TraitType& trait = defaultTrait, typename... Params>
    __aicore__ inline void call(const Params&... params) const
    {
        copy_trait_type::template call<trait>(params...);
    }

    template <typename... TraitsArgs>
    __aicore__ inline auto with(TraitsArgs&&... args) const
    {
        auto traits = copy_trait_type::with(static_cast<TraitsArgs&&>(args)...);
        return CopyAtom<decltype(traits)>{traits};
    }
};

template <typename... Args>
struct CopyAtom<CopyTraits<Args...>> : public CopyTraits<Args...> {
    using copy_trait_type = CopyTraits<Args...>;
    using TraitType = get_trait_member_type_t<copy_trait_type>;
    using trait_type = TraitType;
    static constexpr const TraitType defaultTrait = copy_trait_type::defaultTrait;

    template <const TraitType& trait = defaultTrait, typename... Params>
    __aicore__ inline void call(const Params&... params) const
    {
        copy_trait_type::template call<trait>(params...);
    }

    template <typename... TraitsArgs>
    __aicore__ inline auto with(TraitsArgs&&... args) const
    {
        auto traits = copy_trait_type::with(static_cast<TraitsArgs&&>(args)...);
        return CopyAtom<decltype(traits)>{traits};
    }
};

// Copy / MakeCopy: PascalCase 接口
template <typename AtomType, typename DstTensor, typename SrcTensor>
__aicore__ inline void Copy(const AtomType& atom, const DstTensor& dst, const SrcTensor& src)
{
    atom.call(dst, src);
}

template <typename AtomType, typename DstTensor, typename SrcTensor, typename QuantParam,
          Std::enable_if_t<is_copy_quant_param_v<QuantParam>, int> = 0>
__aicore__ inline void Copy(const AtomType& atom, const DstTensor& dst, const SrcTensor& src,
                            const QuantParam& quant)
{
    atom.call(dst, src, quant);
}

template <typename CopyOperationType>
__aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copy_operation)
{
    return CopyAtom<CopyOperationType>{};
}

template <typename CopyOperationType, typename CopyTraitType>
__aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copy_operation, const CopyTraitType& copy_trait)
{
    return CopyAtom<CopyTraits<CopyOperationType, CopyTraitType>>{};
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_LEGACY_LEGACY_ATOM_H
