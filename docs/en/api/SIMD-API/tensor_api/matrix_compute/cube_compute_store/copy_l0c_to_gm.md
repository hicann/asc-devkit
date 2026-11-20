# copy (L0C Buffer to Global Memory Data Movement)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-11-19T02:32:13.696Z pushedAt=2026-11-19T05:54:06.641Z nl=2 -->

## Supported Products

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

The Tensor API uses the `copy` API to uniformly execute data movement on different paths. This API moves the cube computation result in the L0C Buffer to Global Memory. The data in the L0C Buffer is usually the output of `mmad`, and the data format is `NZ`. When moving to Global Memory, the API automatically selects real-time format conversion from `NZ` to `ND`, `DN`, `NZ`, `NC1HWC0`, `NHWC`, or `NCHW` based on the destination tensor layout. Among them, `NC1HWC0`, `NHWC`, and `NCHW` are used for convolution output feature maps.

The API supports full tensor movement and also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`. Both non-quantized movement and quantized movement support this form.

L0C Buffer to Global Memory movement supports non-quantized output, direct conversion output from `float` to `half` or `bfloat16_t`, and real-time quantized output with scalar or tensor quantization parameters. Real-time ReLU, channel split, and quantization rounding mode are configured through `l0c_to_gm_trait`. The `unit_flag` related to fine-grained parallelism between `mmad` and `Fixpipe` is configured through `l0c_to_gm_params`.

The API supports batch mode. Batch mode is used to complete the movement of multiple cube computation results at once. The layout adds a Batch dimension to the outermost layer of the original cube layout. The source tensor is in `NZ` format with the fractal fixed to 16×16, and can be constructed through `make_frame_layout<nz_layout_ptn>(batch, m, n)`. The destination tensor can be constructed through `make_frame_layout<nd_layout_ptn>(batch, m, n)`, `make_frame_layout<dn_layout_ptn>(batch, m, n)`, `make_frame_layout<nd_ext_layout_ptn>(batch, m, n)`, `make_frame_layout<dn_ext_layout_ptn>(batch, m, n)`, or `make_frame_layout<nz_layout_ptn, DstType>(batch, m, n)`. For the `NZ` format, the destination data type can be specified through the template parameter `DstType`. `C0` represents the column fractal size of the NZ format, which defaults to 16. The convolution output path currently supports only N = 1 and does not support batch mode.

The valid combinations, intermediate data types, and data paths of real-time quantization, real-time ReLU, real-time format conversion, real-time channel split, and real-time channel merge are shown in the following figure. In the figure, F32 to F16 and F32 to BF16 are non-quantized modes that only perform cast. The remaining paths are non-quantized mode, real-time scalar quantization mode, or real-time tensor quantization mode.

For Ascend 950PR&950DT products:

**Figure 1** L0C2GM flowchart

![L0C2GM flowchart](../../../../figures/L0C2GM_Function_Combination_950.png)

## Prototype

- Execute non-quantized movement from L0C Buffer to Global Memory.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derive the movement path based on the storage locations of the source and destination tensors, and use the [default trait](../../algorithm.md#copy_traits) to execute non-quantized movement from L0C Buffer to Global Memory.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Execute quantized movement from L0C Buffer to Global Memory.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src,
        const Quant& quant)
    ```

- Automatically derive the movement path based on the storage locations of the source and destination tensors, and use the default trait to execute quantized movement from L0C Buffer to Global Memory.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src, const Quant& quant)
    ```

- Execute non-quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derive the movement path based on the storage locations of the source and destination tensors, and use the default trait to execute non-quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Execute quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- Automatically derive the movement path based on the storage locations of the source and destination tensors, and use the default trait to execute quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- Construct a movement atomic object using the default trait.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- Construct a movement atomic object using the specified trait.

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation, const CopyTrait& trait)
    ```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type, which must be a Tensor API Tensor type. |
| SrcTensor | Source tensor type, which must be a Tensor API Tensor type. |
| Quant | Quantization parameter type, which can be `uint64_t` or a Tensor API tensor type that meets the requirements. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |
| CopyOperation | Movement operation object type. |
| CopyTrait | Movement trait object type. |

**Table 2** `copy` API parameters

| Parameter | Input/Output | Description |
|--------|--------|--------|
| atom | Input | Movement atomic object, which encapsulates the Operation, Trait, and optional runtime Params combination of a specific movement path. It can be constructed by the `make_copy` API. See [Algorithm](../../algorithm.md#copy_atom) for details. |
| dst | Output | Destination tensor, whose storage location is `location::gm`. Supports `ND`, `DN`, `NZ`, `NC1HWC0`, `NHWC`, and `NCHW` data formats. |
| src | Input | Source tensor, whose storage location is `location::l0c` and data format is `NZ`. It is usually the computation result of `mmad`. |
| quant | Input | Optional quantization parameter. When `uint64_t` is passed in, it represents a scalar quantization parameter. When a tensor is passed in, it represents a tensor quantization parameter. The tensor is located in the L1 Buffer and its element type is `uint64_t`. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor, which can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor, which can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

To configure runtime movement parameters, first bind the `l0c_to_gm_params` object to the movement atomic object through `atom.with(params)`. The default value is used when it is not bound.

**Table 3** `make_copy` API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. L0C Buffer to Global Memory movement uses `copy_l0c_to_gm{}`. |
| trait | Input | Movement static feature object. L0C Buffer to Global Memory movement uses `l0c_to_gm_trait_default{}` by default. |



### copy_l0c_to_gm Description

`copy_l0c_to_gm` is used to identify the L0C Buffer to Global Memory data movement path. Only its public content is listed:

```cpp
struct copy_l0c_to_gm {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function is used to receive the Trait and movement parameters and dispatch L0C Buffer to Global Memory data movement.

### l0c_to_gm_trait Description

`l0c_to_gm_trait` is used to configure the static features of L0C Buffer to Global Memory movement.

```cpp
enum class round_mode : uint8_t { default_round = 0, hybrid = 1 };

struct l0c_to_gm_trait {
    __aicore__ constexpr l0c_to_gm_trait() = default;
    __aicore__ constexpr l0c_to_gm_trait(asc::te::round_mode round_mode,
        bool enable_relu, bool enable_channel_split);

    asc::te::round_mode round_mode = asc::te::round_mode::default_round;
    bool enable_relu = false;
    bool enable_channel_split = false;
};
```

**Table 4** `l0c_to_gm_trait` members

| Member | Default Value | Description |
|--------|--------|--------|
| round_mode | `round_mode::default_round` | Rounding mode. `round_mode::hybrid` is supported only in the quantized output scenario where the source type is `float` and the destination type is `hifloat8_t`. |
| enable_relu | `false` | Whether to enable real-time ReLU. |
| enable_channel_split | `false` | Whether to enable output data channel split. Only the scenario where the format is NZ and both the source type and destination type are `float` is supported. |

Example of using a custom trait:

```cpp
constexpr l0c_to_gm_trait l0c_to_gm_trait_value = {
    round_mode::default_round,
    false,
    true
};

struct copy_l0c_to_gm_trait_custom {
    using trait_type = l0c_to_gm_trait;
    static constexpr const trait_type value = l0c_to_gm_trait_value;
};
```

`l0c_to_gm_trait_default` provides the default Trait type and default value for this path:

```cpp
struct l0c_to_gm_trait_default {
    using trait_type = l0c_to_gm_trait;
    static constexpr const trait_type value = {};
};
```

### l0c_to_gm_params Description

`l0c_to_gm_params` is used to configure runtime movement parameters.

```cpp
enum class unit_flag_mode : uint8_t { disable = 0, enable_keep = 2, enable_update = 3 };

struct l0c_to_gm_params {
    __aicore__ constexpr l0c_to_gm_params() = default;
    __aicore__ constexpr l0c_to_gm_params(unit_flag_mode unit_flag) : unit_flag(unit_flag) {}

    unit_flag_mode unit_flag = unit_flag_mode::disable;
};
```

**Table 5** `l0c_to_gm_params` members

| Member | Default Value | Description |
|--------|--------|--------|
| unit_flag | `unit_flag_mode::disable` | Controls fine-grained parallelism between mmad instructions and Fixpipe instructions. `unit_flag_mode::disable` means not enabled, `unit_flag_mode::enable_keep` means enabled and the unit flag bit is not reset after execution, and `unit_flag_mode::enable_update` means enabled and the unit flag bit is reset after execution. |

## Data Type

L0C Buffer to Global Memory movement automatically selects the quantization mode based on whether a quantization parameter is passed in.

**Table 6** Data types

| Source tensor type (L0C Buffer) | Destination tensor type (Global Memory) | Call form | Description |
|--------|--------|--------|--------|
| `int32_t` | `int8_t`, `uint8_t` | `copy(atom, dst, src, quant)` | Scalar or tensor quantized output. |
| `float` | `int8_t`, `uint8_t` | `copy(atom, dst, src, quant)` | Scalar or tensor quantized output. |
| `float` | `fp8_e4m3fn_t`, `hifloat8_t` | `copy(atom, dst, src, quant)` | Scalar or tensor quantized output. `hifloat8_t` supports `round_mode::default_round` and `round_mode::hybrid`. |
| `int32_t` | `half`, `bfloat16_t` | `copy(atom, dst, src, quant)` | Scalar or tensor quantized output. |
| `float` | `half` | `copy(atom, dst, src)` | Direct conversion output. |
| `float` | `bfloat16_t` | `copy(atom, dst, src)` | Direct conversion output. |
| `float` | `half`, `bfloat16_t` | `copy(atom, dst, src, quant)` | Scalar or tensor quantized output. |
| `int32_t` | `int32_t` | `copy(atom, dst, src)` | Non-quantized output. |
| `float` | `float` | `copy(atom, dst, src)` | Non-quantized output. |
| `float` | `float` | `copy(atom, dst, src, quant)` | Scalar or tensor quantized output. |

When `quant` is not passed in, the API selects the non-quantized or direct conversion mode. When `uint64_t` is passed in, it selects the scalar quantization mode. When a tensor is passed in, it selects the tensor quantization mode.

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The quantization and ReLU parameters cannot be inf/nan or denormalized numbers.
- The source cube is in `NZ` format and requires 64-byte address alignment. When the destination cube is in `DN` or `ND` format, 1-byte address alignment is required. When it is in `NZ` format, 32-byte address alignment is required.
- When the destination cube is output in `NZ` format, the size in the N direction must be a multiple of 16. For the `b8` type channel merge scenario, it must be a multiple of 32. For the `float` scenario with `enable_channel_split` channel split enabled, it must be a multiple of 8.
- The tensor quantization parameter tensor should be located in the L1 Buffer, with the element type `uint64_t` and 32-byte address alignment. See [real-time quantization](../cube_store_key_features/quant_pre.md) for details.
- When `unit_flag` is enabled, it must be enabled together with `mmad`.
- When `round_mode::hybrid` is used, the source type must be `float` and the destination type must be `hifloat8_t`.
- `enable_channel_split` takes effect only when both the source type and destination type are `float` and the destination format is `NZ`. See [F32 Channel Split](../cube_store_key_features/f32_channel_split.md) for details.
- The channel merge feature is automatically enabled by hardware and cannot be configured through parameters. See [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md) for details.
- The convolution output path from `NZ` to `NC1HWC0`, `NHWC`, or `NCHW` currently supports only N = 1. `NZ` to `NHWC` or `NCHW` supports only the call form without passing in `quant`.
- In the convolution output path, the logical shape of the source `NZ` tensor must be `(H * W, C)`. When the destination tensor is `NC1HWC0`, `C = C1 * C0` must be satisfied.

## Key Features

L0C Buffer to Global Memory movement involves key features such as [real-time quantization](../cube_store_key_features/quant_pre.md), [real-time ReLU](../cube_store_key_features/relu_pre.md), [F32 Channel Split](../cube_store_key_features/f32_channel_split.md), [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md), and [batch movement](../cube_store_key_features/batch_copy.md).

## Example

`make_copy` has two parameter-passing forms: `make_copy(operation, trait)` directly specifies the static trait; `make_copy(operation)` constructs using the default trait of the path and can further bind the `l0c_to_gm_params` runtime parameters through `.with(params)`. The default value is used when params is not bound.

### Convolution Output

The following example moves the NZ cube with the shape `(H * W, C)` in the L0C Buffer to Global Memory and converts it to NHWC format. Replace the destination layout with `nchw_layout_ptn` or `nc1hwc0_layout_ptn` to output in NCHW or NC1HWC0 format, respectively.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t h = 8;
constexpr uint32_t w = 8;
constexpr uint32_t c = 16;

__aicore__ inline void copy_conv_output_l0c_to_gm(__gm__ float* gm_addr)
{
    // __cc__ represents the L0C Buffer address space, and __gm__ represents the Global Memory address space.
    __cc__ float l0c_buf[h * w * c];

    // Source tensor: NZ format, with C0 set to 16 (column fractal size of the NZ format, defaulting to 16).
    auto l0c = make_tensor(
        make_mem_ptr(l0c_buf),
        make_frame_layout<nz_layout_ptn, 16>(h * w, c));
    // Destination tensor: NHWC format (select the layout pattern on the GM side based on the target format).
    auto gm = make_tensor(
        make_mem_ptr(gm_addr), make_frame_layout<nhwc_layout_ptn>(1, h, w, c));

    auto copy_l0c_to_gm_atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    copy(copy_l0c_to_gm_atom, gm, l0c);
}
```

### Non-quantized Output

In non-quantized mode, the data in L0C is directly output to Global Memory (when the source and destination types are the same), or directly converted between `float` and `half`/`bfloat16_t`.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_gm_example(__cc__ float* l0c_addr, __gm__ float* gm_addr, uint32_t m, uint32_t n)
{
    // Source tensor: mmad output in L0C, fixed to NZ format, with C0 set to 16.
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    // Destination tensor: nd_ext layout (row-major extended layout) on the GM side.
    auto gm = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));

    l0c_to_gm_params params;
    auto atom = make_copy(copy_l0c_to_gm{}).with(params);
    copy(atom, gm, l0c);
}
```

### Scalar Quantized Output

In scalar quantization mode, the entire C cube shares the same quantization parameter. The quantization parameter is passed in as a `uint64_t` scalar.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_gm_example(__cc__ float* l0c_addr, __gm__ half* gm_addr, uint32_t m, uint32_t n)
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto gm = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));

    uint64_t quant = 1;
    auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
    copy(atom, gm, l0c, quant);
}
```

### Tensor Quantized Output

In tensor quantization mode, the quantization parameter is a tensor located in the L1 Buffer, with one quantization parameter per channel dimension. The quantization tensor element type is `uint64_t` and its shape is `(1, n)`.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_gm_example(__cc__ float* l0c_addr, __gm__ half* gm_addr, __cbuf__ uint64_t* l1_quant_addr, uint32_t m, uint32_t n)
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto gm = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));
    auto quant = make_tensor(make_mem_ptr(l1_quant_addr), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
    copy(atom, gm, l0c, quant);
}
```

### Batch Mode Output

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_batch_l0c_to_gm(__gm__ half* gm_addr)
{
    constexpr uint32_t src_batch = 3;
    constexpr uint32_t dst_batch = 9;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cc__ float l0c_buf[src_batch * m * n];

    auto l0c = make_tensor(make_mem_ptr(l0c_buf), make_frame_layout<nz_layout_ptn>(src_batch, m, n));
    auto gm = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(dst_batch, m, n));

    auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
    for (uint32_t i = 0; i < dst_batch / src_batch; ++i) {
        auto gm_slice = gm.slice(make_coord(i * src_batch, make_coord(0, 0)), make_shape(src_batch, make_shape(m, n)));
        copy(atom, gm_slice, l0c);
    }
}
```
