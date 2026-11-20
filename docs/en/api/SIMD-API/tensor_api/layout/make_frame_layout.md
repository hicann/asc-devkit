# make_frame_layout

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:30:45.179Z pushedAt=2026-10-27T13:03:35.308Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/tensor.h`.

make_frame_layout constructs a layout object with type information based on a standard fractal layout pattern. This API computes the shape and stride based on the constructor parameters, combined with the specified LayoutPattern (layout pattern) and layout trait (layout feature parameters), to construct the layout object. In addition to cube layouts, this API also supports constructing layouts in the NCHW, NHWC, NC1HWC0, NCDHW, and NDC1HWC0 formats for convolution scenarios. It also supports passing a Batch dimension before the cube dimensions to construct layouts for multi-batch cubes.

LayoutPattern determines the fractal arrangement of data in memory. Movement and computation operations between different storage locations require the source/destination tensors to satisfy a specific LayoutPattern combination.

## Function Prototype

```cpp
template <typename LayoutPattern, typename Trait = Std::ignore_t, typename... Args>
__aicore__ inline decltype(auto) make_frame_layout(const Args&... args)

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline decltype(auto) make_frame_layout(const Args&... args)

template <typename LayoutPattern, typename Trait = layout_trait_default<>>
struct frame_layout_format {
    template <typename... Args>
    __aicore__ inline decltype(auto) operator()(const Args&... args);
};
```

The public definition of the layout trait is as follows.

```cpp
template <typename DataType, typename C0>
struct layout_trait {
    static constexpr auto c0_element = C0{};
};

template <typename DataType = uint16_t,
          typename C0 = Std::Int<c0_element<DataType>>>
struct layout_trait_default : layout_trait<DataType, C0> {};
```

The supported LayoutPattern type definitions are as follows.

```cpp
struct zn_layout_ptn {};
struct zz_layout_ptn {};
struct nn_layout_ptn {};
struct nz_layout_ptn {};
struct nd_layout_ptn {};
struct dn_layout_ptn {};
struct nd_ext_layout_ptn {};
struct dn_ext_layout_ptn {};
struct scalea_nd_layout_ptn {};
struct scalea_dn_layout_ptn {};
struct scaleb_nd_layout_ptn {};
struct scaleb_dn_layout_ptn {};
struct nchw_layout_ptn {};
struct nhwc_layout_ptn {};
struct nc1hwc0_layout_ptn {};
struct ncdhw_layout_ptn {};
struct ndc1hwc0_layout_ptn {};
```

Common invocation forms are as follows.

```cpp
// Construct a single cube layout, where m is the number of rows and n is the number of columns.
auto layout = make_frame_layout<LayoutPattern, Trait>(m, n);

// Construct a batch cube layout, where batch is the number of cubes, m is the number of rows of a single cube, and n is the number of columns of a single cube.
auto batch_layout = make_frame_layout<LayoutPattern, Trait>(batch, m, n);

// Construct a convolution input or output layout.
auto nchw_layout = make_frame_layout<nchw_layout_ptn>(n, c, h, w);
auto nhwc_layout = make_frame_layout<nhwc_layout_ptn>(n, h, w, c);
auto nc1hwc0_layout = make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0);
auto ncdhw_layout = make_frame_layout<ncdhw_layout_ptn>(n, c, d, h, w);
auto ndc1hwc0_layout = make_frame_layout<ndc1hwc0_layout_ptn>(n, d, c1, h, w, c0);
```

### Convolution LayoutPattern

**Table 1**  Convolution LayoutPattern description

| LayoutPattern | Constructor Parameter | shape | stride | Description |
| :--- | :--- | :--- | :--- | :--- |
| `nchw_layout_ptn` | `n, c, h, w` | `(N, C, H, W)` | `(C * H * W, H * W, W, 1)` | The channel dimension C precedes the height dimension H and the width dimension W. |
| `nhwc_layout_ptn` | `n, h, w, c` | `(N, H, W, C)` | `(H * W * C, W * C, C, 1)` | The channel dimension C is in the innermost position. |
| `nc1hwc0_layout_ptn` | `n, c1, h, w, c0` | `(N, C1, H, W, C0)` | `(C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` | Splits the channel dimension C into C1 and C0, where `C = C1 * C0`. |
| `ncdhw_layout_ptn` | `n, c, d, h, w` | `(N, C, D, H, W)` | `(C * D * H * W, D * H * W, H * W, W, 1)` | Contiguous NCDHW layout of a 3D convolution feature map. |
| `ndc1hwc0_layout_ptn` | `n, d, c1, h, w, c0` | `(N, D, C1, H, W, C0)` | `(D * C1 * H * W * C0, C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` | Fractal layout of a 3D convolution feature map, splitting the channel dimension C into C1 and C0. |

All the layouts above are contiguous row-major layouts, and the unit of stride is elements. The C0 of `nc1hwc0_layout_ptn` and `ndc1hwc0_layout_ptn` is explicitly specified by function parameters, rather than derived through layout_trait.

### Specifying layout_trait

layout_trait is used to specify the cardinality of the C0 dimension. make_frame_layout supports the following four ways of passing it:

1. **`layout_trait_default<T>`**: Automatically computes C0 based on the data type T. T is used only for C0 deduction and is not stored as data type information in layout_trait.

   ```cpp
   make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n)
   ```

2. **C0 constant type**: Directly specifies the number of elements in the C0 dimension. Types such as `_8` and `_16` are shorthand forms of [Std::Int](../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md). For example, `_16` represents `Std::Int<16>`.

   ```cpp
   make_frame_layout<nz_layout_ptn, _16>(m, n)
   ```

3. **Omitting the second template parameter**: In scenarios where the fractal construction does not require the user to specify C0, or where C0 is fixed by the layout pattern, only LayoutPattern can be passed. In this case, the API uses the built-in C0 value according to LayoutPattern. Such fractals include nd_layout_ptn, dn_layout_ptn, dn_ext_layout_ptn, nd_ext_layout_ptn, scalea_nd_layout_ptn, scalea_dn_layout_ptn, scaleb_nd_layout_ptn, and scaleb_dn_layout_ptn.

   ```cpp
   make_frame_layout<nd_layout_ptn>(m, n)
   ```

4. **Custom Trait type**: When the three methods of deducing C0 from the data type, directly passing a C0 constant type, and omitting the second template parameter cannot express the target layout, a custom Trait type can be passed. The custom Trait must provide a `c0_element` member to represent the number of elements in the C0 dimension.

   ```cpp
   struct my_layout_trait {
       static constexpr auto c0_element = Std::Int<16>{};
   };

   make_frame_layout<nz_layout_ptn, my_layout_trait>(m, n)
   ```

For compatibility with existing code, the `layout_trait<T, C0>` form is also supported, where T is used only to maintain source compatibility, and layout_trait actually uses only C0 as the layout feature.

## Parameters

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| LayoutPattern | Input | Layout pattern template parameter. For supported LayoutPattern values and data layout formats, see [layout and hierarchical representation](layout_hierarchical_representation.md). |
| Trait | Input | The second template parameter, used to describe the C0 value. It can be `layout_trait_default<T>`, a C0 constant type, a custom Trait type with a `c0_element` member, or a concrete data type. When omitted, the API uses the built-in C0 value according to LayoutPattern. |
| C0Element | Input | Directly specifies the number of C0 elements using a non-type template parameter. |
| Args | Input | Type parameter pack of constructor parameters. |
| DataType | Input | Element type used by the layout trait to deduce C0. |
| C0 | Input | Compile-time integer constant type representing the number of C0 elements. |
| args... | Input | Constructor parameters passed to `make_frame_layout` or `frame_layout_format::operator()`. In cube scenarios, these are commonly m and n; in Batch cube scenarios, batch, m, and n. In 2D convolution scenarios, n, c, h, w or n, c1, h, w, c0 are passed according to LayoutPattern; in 3D convolution scenarios, n, c, d, h, w or n, d, c1, h, w, c0 are passed. |

## Return Value

Returns an object of type `layout<shape, stride, Std::tuple<LayoutPattern, Trait>>`, where the shape and stride are deduced at compile time from the Layout Pattern and constructor parameters.

## Constraints

- LayoutPattern must be one of the supported layout patterns; unsupported patterns trigger a compile-time error.
- The number and meaning of constructor parameters are determined by LayoutPattern. For common 2D scenarios, passing (m, n) is sufficient.
- When using `nc1hwc0_layout_ptn`, the user must explicitly pass in C0 based on the data type and subsequent operation requirements, and ensure that the C-axis data satisfies `C = C1 * C0`.
- When using `ndc1hwc0_layout_ptn`, the user must explicitly pass in C0 and ensure that the C-axis data satisfies `C = C1 * C0`.
- In Batch mode, pass (batch, m, n), where m and n represent the number of rows and columns of a single cube, and batch represents the number of consecutively arranged cubes.
- Trait must be an integer constant, a Trait type with a `c0_element` member, or a concrete data type; unsupported types trigger a compile-time error. When a concrete data type is passed, that type is used only to deduce C0; the actual data type of the Tensor is determined by the memory pointer type bound by `make_tensor`.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Construct an ND layout tensor on the GM side.
auto layout_gm = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n);
auto gm_tensor = make_tensor(make_mem_ptr<location::gm>(gm_addr), layout_gm);

// Construct an NZ layout tensor on the L1 side (the standard format for L0A/L0B movement).
auto layout_l1 = make_frame_layout<nz_layout_ptn, float>(m, n);
auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_l1);

// Construct an NZ layout by omitting the second template parameter.
auto layout_nz = make_frame_layout<nz_layout_ptn>(m, n);
auto l1_tensor_nz = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_nz);

// Construct by specifying the number of C0 elements.
auto layout_c0 = make_frame_layout<nz_layout_ptn, _16>(m, n);
auto l1_tensor_c0 = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_c0);

// Construct with a custom trait.
struct my_layout_trait {
    static constexpr auto c0_element = Std::Int<16>{};
};
auto layout_custom = make_frame_layout<nz_layout_ptn, my_layout_trait>(m, n);

// Construct a batch cube layout.
auto layout_batch_nz = make_frame_layout<nz_layout_ptn, float>(batch, m, n);
auto l1_batch_tensor = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_batch_nz);

// Construct the scale Batch layout for the MX scenario.
auto layout_batch_scalea = make_frame_layout<scalea_nd_layout_ptn>(batch, scale_m, scale_k);
auto gm_scale_tensor = make_tensor(make_mem_ptr<location::gm>(scale_addr), layout_batch_scalea);

// Construct the convolution feature map layout.
auto layout_nchw = make_frame_layout<nchw_layout_ptn>(n, c, h, w);
auto layout_nhwc = make_frame_layout<nhwc_layout_ptn>(n, h, w, c);
auto layout_nc1hwc0 = make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0);
```
