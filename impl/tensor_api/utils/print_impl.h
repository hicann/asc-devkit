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
#warning                                                                                                               \
    "impl/tensor_api/utils/print_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/utils/print.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file print_impl.h
 * \brief Tensor API debug print helpers implementation.
 */
#ifndef IMPL_TENSOR_API_UTILS_PRINT_IMPL_H
#define IMPL_TENSOR_API_UTILS_PRINT_IMPL_H

#include "impl/tensor_api/utils/print_value_impl.h"
#include "impl/tensor_api/utils/print_tensor_debug_bus_impl.h"

namespace asc {
namespace te {
namespace detail {

template <typename Data, typename Tensor>
struct addressable_tensor_reader {
    const Tensor& tensor;

    template <typename Coord>
    __aicore__ inline void emit(print_session& session, const Coord& coord)
    {
        emit_element_as<Data>(session, tensor[coord]);
    }
};

template <typename T>
__aicore__ inline void emit_layout_table(print_session& session, const T& layout)
{
    static_assert(is_layout_v<T>, "print_layout expects a Tensor API layout.");
    static_assert(Std::remove_cvref_t<T>::rank_size == 2, "print_layout only supports rank-2 layouts.");

    int idx_width = num_digits(static_cast<long long>(cosize(layout))) + 2;
    emit_value(session, layout);
    print_fragment(session, (__gm__ const char*)"\n");

    print_fragment(session, (__gm__ const char*)"    ");
    for (int n = 0; n < static_cast<int>(size<1>(layout)); ++n) {
        print_fragment(session, (__gm__ const char*)"  ");
        emit_padded_int(session, n, idx_width - 2);
        print_fragment(session, (__gm__ const char*)" ");
    }
    print_fragment(session, (__gm__ const char*)"\n");

    for (int m = 0; m < static_cast<int>(size<0>(layout)); ++m) {
        print_fragment(session, (__gm__ const char*)"    ");
        for (int n = 0; n < static_cast<int>(size<1>(layout)); ++n) {
            print_fragment(session, (__gm__ const char*)"+");
            for (int i = 0; i < idx_width; ++i) {
                print_fragment(session, (__gm__ const char*)"-");
            }
        }
        print_fragment(session, (__gm__ const char*)"+\n");

        emit_padded_int(session, m, 2);
        print_fragment(session, (__gm__ const char*)"  ");
        for (int n = 0; n < static_cast<int>(size<1>(layout)); ++n) {
            print_fragment(session, (__gm__ const char*)"| ");
            emit_padded_int(session, static_cast<long long>(layout(make_coord(m, n))), idx_width - 2);
            print_fragment(session, (__gm__ const char*)" ");
        }
        print_fragment(session, (__gm__ const char*)"|\n");
    }

    print_fragment(session, (__gm__ const char*)"    ");
    for (int n = 0; n < static_cast<int>(size<1>(layout)); ++n) {
        print_fragment(session, (__gm__ const char*)"+");
        for (int i = 0; i < idx_width; ++i) {
            print_fragment(session, (__gm__ const char*)"-");
        }
    }
    print_fragment(session, (__gm__ const char*)"+\n");
}

__aicore__ inline void emit_tensor_separator(print_session& session, int width, __gm__ const char* separator)
{
    for (int i = 0; i < width; ++i) {
        print_fragment(session, separator);
    }
    print_fragment(session, (__gm__ const char*)"\n");
}

template <typename Reader, typename T>
__aicore__ inline void emit_rank_one_tensor(print_session& session, const T& tensor, Reader& reader)
{
    for (int m = 0; m < static_cast<int>(size<0>(tensor.layout())); ++m) {
        reader.emit(session, make_coord(m));
        print_fragment(session, (__gm__ const char*)"\n");
    }
}

template <typename Reader, typename T>
__aicore__ inline void emit_rank_two_tensor(print_session& session, const T& tensor, Reader& reader)
{
    for (int m = 0; m < static_cast<int>(size<0>(tensor.layout())); ++m) {
        for (int n = 0; n < static_cast<int>(size<1>(tensor.layout())); ++n) {
            reader.emit(session, make_coord(m, n));
        }
        print_fragment(session, (__gm__ const char*)"\n");
    }
}

template <typename Reader, typename T>
__aicore__ inline void emit_rank_three_plane(print_session& session, const T& tensor, Reader& reader, int k)
{
    for (int m = 0; m < static_cast<int>(size<0>(tensor.layout())); ++m) {
        for (int n = 0; n < static_cast<int>(size<1>(tensor.layout())); ++n) {
            reader.emit(session, make_coord(m, n, k));
        }
        print_fragment(session, (__gm__ const char*)"\n");
    }
}

template <typename Reader, typename T>
__aicore__ inline void emit_rank_three_tensor(print_session& session, const T& tensor, Reader& reader)
{
    int separator_width = 5 * static_cast<int>(size<1>(tensor.layout()));
    for (int k = 0; k < static_cast<int>(size<2>(tensor.layout())); ++k) {
        if (k != 0) {
            emit_tensor_separator(session, separator_width, (__gm__ const char*)"-");
        }
        emit_rank_three_plane(session, tensor, reader, k);
    }
}

template <typename Reader, typename T>
__aicore__ inline void emit_rank_four_plane(print_session& session, const T& tensor, Reader& reader, int k, int p)
{
    for (int m = 0; m < static_cast<int>(size<0>(tensor.layout())); ++m) {
        for (int n = 0; n < static_cast<int>(size<1>(tensor.layout())); ++n) {
            reader.emit(session, make_coord(m, n, k, p));
        }
        print_fragment(session, (__gm__ const char*)"\n");
    }
}

template <typename Reader, typename T>
__aicore__ inline void emit_rank_four_tensor(print_session& session, const T& tensor, Reader& reader)
{
    int separator_width = 5 * static_cast<int>(size<1>(tensor.layout()));
    for (int p = 0; p < static_cast<int>(size<3>(tensor.layout())); ++p) {
        if (p != 0) {
            emit_tensor_separator(session, separator_width, (__gm__ const char*)"=");
        }
        for (int k = 0; k < static_cast<int>(size<2>(tensor.layout())); ++k) {
            if (k != 0) {
                emit_tensor_separator(session, separator_width, (__gm__ const char*)"-");
            }
            emit_rank_four_plane(session, tensor, reader, k, p);
        }
    }
}

template <typename Location>
__aicore__ inline void emit_unsupported_tensor_location(print_session& session)
{
    print_fragment(session, (__gm__ const char*)"[print_tensor] tensor elements cannot be printed from ");
    if constexpr (Std::is_same_v<Location, location::l1>) {
        print_fragment(session, (__gm__ const char*)"L1 (__cbuf__)");
    } else if constexpr (Std::is_same_v<Location, location::l0a>) {
        print_fragment(session, (__gm__ const char*)"L0A (__ca__)");
    } else if constexpr (Std::is_same_v<Location, location::l0b>) {
        print_fragment(session, (__gm__ const char*)"L0B (__cb__)");
    } else if constexpr (Std::is_same_v<Location, location::l0c>) {
        print_fragment(session, (__gm__ const char*)"L0C (__cc__)");
    } else if constexpr (Std::is_same_v<Location, location::l0scalea>) {
        print_fragment(session, (__gm__ const char*)"L0ScaleA");
    } else if constexpr (Std::is_same_v<Location, location::l0scaleb>) {
        print_fragment(session, (__gm__ const char*)"L0ScaleB");
    } else if constexpr (Std::is_same_v<Location, location::bias>) {
        print_fragment(session, (__gm__ const char*)"BiasTable (__biasbuf__)");
    } else if constexpr (Std::is_same_v<Location, location::fixbuf>) {
        print_fragment(session, (__gm__ const char*)"FixBuf (__fbuf__)");
    } else if constexpr (Std::is_same_v<Location, location::ssbuf>) {
        print_fragment(session, (__gm__ const char*)"SSBuf (__ssbuf__)");
    } else if constexpr (Std::is_same_v<Location, Std::ignore_t>) {
        print_fragment(session, (__gm__ const char*)"ordinary local memory");
    } else {
        print_fragment(session, (__gm__ const char*)"this memory location");
    }
#if defined(__DAV_CUBE__) && (__NPU_ARCH__ == 3510)
    print_fragment(session, (__gm__ const char*)"; only GM, UB, L1, L0C, and BiasTable are supported.\n");
#else
    print_fragment(session, (__gm__ const char*)"; only GM and UB are supported.\n");
#endif
}

template <typename Reader, typename T>
__aicore__ inline void emit_tensor_with_reader(print_session& session, const T& tensor, Reader& reader)
{
    using layout_type = typename Std::remove_cvref_t<T>::layout_type;
    if constexpr (layout_type::rank_size == 1) {
        emit_rank_one_tensor(session, tensor, reader);
    } else if constexpr (layout_type::rank_size == 2) {
        emit_rank_two_tensor(session, tensor, reader);
    } else if constexpr (layout_type::rank_size == 3) {
        emit_rank_three_tensor(session, tensor, reader);
    } else if constexpr (layout_type::rank_size == 4) {
        emit_rank_four_tensor(session, tensor, reader);
    }
}

template <typename Data, typename T>
__aicore__ inline void emit_addressable_tensor_content(print_session& session, const T& tensor)
{
    addressable_tensor_reader<Data, T> reader{tensor};
    emit_tensor_with_reader(session, tensor, reader);
}

#if defined(__DAV_CUBE__) && (__NPU_ARCH__ == 3510)
template <typename Data, AscendC::Hardware Hardware, typename T>
__aicore__ inline void emit_debug_bus_tensor_content(print_session& session, const T& tensor)
{
    debug_bus_tensor_reader<Data, T, Hardware> reader{tensor};
    emit_tensor_with_reader(session, tensor, reader);
}
#endif

template <typename T>
__aicore__ inline void emit_tensor_content(print_session& session, const T& tensor, bool print_type)
{
    static_assert(is_attr_tensor_v<T>, "print_tensor expects a Tensor API tensor created by make_tensor.");
    using tensor_type = Std::remove_cvref_t<T>;
    using layout_type = typename tensor_type::layout_type;
    using element_type = typename tensor_type::element_type;
    using data_type = get_attribute_element_type<element_type*>;
    using tensor_location = get_mem_location<tensor_type>;
    static_assert(layout_type::rank_size >= 1 && layout_type::rank_size <= 4,
                  "print_tensor only supports rank 1 to 4 tensors.");

    if (print_type) {
        emit_value(session, tensor);
        print_fragment(session, (__gm__ const char*)":\n");
    }
    if constexpr (Std::is_same_v<tensor_location, location::gm> || Std::is_same_v<tensor_location, location::ub>) {
        emit_addressable_tensor_content<data_type>(session, tensor);
#if defined(__DAV_CUBE__) && (__NPU_ARCH__ == 3510)
    } else if constexpr (Std::is_same_v<tensor_location, location::l1>) {
        emit_debug_bus_tensor_content<data_type, AscendC::Hardware::L1>(session, tensor);
    } else if constexpr (Std::is_same_v<tensor_location, location::l0c>) {
        emit_debug_bus_tensor_content<data_type, AscendC::Hardware::L0C>(session, tensor);
    } else if constexpr (Std::is_same_v<tensor_location, location::bias>) {
        emit_debug_bus_tensor_content<data_type, AscendC::Hardware::BIAS>(session, tensor);
#endif
    } else {
        emit_unsupported_tensor_location<tensor_location>(session);
    }
}

} // namespace detail

__aicore__ inline void print(__gm__ const char* fmt)
{
    detail::print_format(fmt);
}

template <typename Arg0, typename... Args>
__aicore__ inline void print(__gm__ const char* fmt, Arg0&& arg0, Args&&... args)
{
    detail::print_format(fmt, static_cast<Arg0&&>(arg0), static_cast<Args&&>(args)...);
}

template <typename Value, typename Std::enable_if_t<!detail::is_print_string_v<Value>, int> = 0>
__aicore__ inline void print(const Value& value)
{
#if !(defined(ASCENDC_DUMP) && (ASCENDC_DUMP == 0))
    detail::print_session session;
    detail::emit_value(session, value);
    detail::print_fragment(session, (__gm__ const char*)"\n");
#else
    (void)value;
#endif
}

template <typename Layout>
__aicore__ inline void print_layout(const Layout& layout)
{
#if !(defined(ASCENDC_DUMP) && (ASCENDC_DUMP == 0))
    detail::print_session session;
    detail::emit_layout_table(session, layout);
#else
    (void)layout;
#endif
}

template <typename Tensor>
__aicore__ inline void print_tensor(const Tensor& tensor, bool print_type = true)
{
#if !(defined(ASCENDC_DUMP) && (ASCENDC_DUMP == 0))
    detail::print_session session;
    detail::emit_tensor_content(session, tensor, print_type);
#else
    (void)tensor;
    (void)print_type;
#endif
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_UTILS_PRINT_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
