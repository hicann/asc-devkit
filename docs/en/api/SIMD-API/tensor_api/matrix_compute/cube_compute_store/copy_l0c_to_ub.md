# copy (L0C Buffer to Unified Buffer Data Movement)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-11-19T02:32:06.780Z pushedAt=2026-11-19T05:54:06.640Z nl=2 -->

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

The Tensor API uses the `copy` API to uniformly execute data movement over different paths. This API is used to move the cube computation result in the L0C Buffer to the Unified Buffer (UB). The data in the L0C Buffer is usually the output of `mmad`, and the data format is `NZ`. When moving to the UB, the API automatically selects the real-time format conversion from `NZ` to `ND`, `NZ` to `DN`, or `NZ` to `NZ` based on the destination tensor format.

The API supports complete tensor movement. It also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`. Both non-quantized movement and quantized movement support this form.

L0C Buffer to UB movement supports non-quantized output, direct conversion output from `float` to `half` or `bfloat16_t`, and real-time quantized output that cooperates with scalar or tensor quantization parameters. Real-time ReLU, dual-target mode, and rounding mode are configured through `l0c_to_ub_trait`. The `unit_flag` related to fine-grained parallelism between `mmad` and `Fixpipe` is configured through `l0c_to_ub_params`.

The API supports batch mode. Batch mode is used to complete the movement of multiple cube computation results at one time. The layout adds a Batch dimension to the outermost layer of the original cube layout. The source tensor is in `NZ` format, with the fractal fixed to 16×16, and can be constructed through `make_frame_layout<nz_layout_ptn>(batch, m, n)`. The destination tensor can be constructed through `make_frame_layout<nd_layout_ptn>(batch, m, n)`, `make_frame_layout<dn_layout_ptn>(batch, m, n)`, `make_frame_layout<nd_ext_layout_ptn>(batch, m, n)`, `make_frame_layout<dn_ext_layout_ptn>(batch, m, n)`, or `make_frame_layout<nz_layout_ptn, DstType>(batch, m, n)`. For the `NZ` format, the destination data type can be specified through the template parameter `DstType`. `C0` indicates the column fractal size of the NZ format, and the default is 16.

The valid combinations, intermediate data types, and data paths of real-time quantization, real-time ReLU, real-time format conversion, real-time channel split, and real-time channel merge are shown in the following figure. In the figure, F32 to F16 and F32 to BF16 are non-quantized modes that only perform cast. The remaining paths are non-quantized, real-time scalar, or tensor quantization modes. For Ascend 950PR&950DT products, NZ2DN real-time format conversion is also supported.

**Figure 1** L0C2UB flowchart

![L0C2UB flowchart](../../../../figures/L0C2UB_Function_Combination_950.png)

## Prototype

- Executes non-quantized movement from the L0C Buffer to the UB.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and uses the [default trait](../../algorithm.md#copy_traits) to execute non-quantized movement from the L0C Buffer to the UB.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Executes quantized movement from the L0C Buffer to the UB.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src,
        const Quant& quant)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and uses the default trait to execute quantized movement from the L0C Buffer to the UB.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src, const Quant& quant)
    ```

- Executes non-quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and uses the default trait to execute non-quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Executes quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and uses the default trait to execute quantized movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- Constructs a movement atomic object using the default trait.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- Constructs a movement atomic object using the specified trait.

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

|Parameter|Input/Output|Description|
|--------|--------|--------|
|atom|Input|Movement atomic object that encapsulates the Operation, Trait, and optional runtime Params combination of a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom).|
|dst|Output|Destination tensor, whose storage location is `location::ub`. The supported data formats are `ND`, `DN`, and `NZ`.|
|src|Input|Source tensor, whose storage location is `location::l0c` and data format is `NZ`. It is usually the computation result of `mmad`.|
|quant|Input|Optional quantization parameter. When `uint64_t` is passed in, it indicates a scalar quantization parameter. When a tensor is passed in, it indicates a tensor quantization parameter. The tensor is located in the L1 Buffer, and its element type is `uint64_t`.|
|dst_coord|Input|Start coordinate of the movement region in the destination tensor. `zero_coord` can also be passed in.|
|src_coord|Input|Start coordinate of the movement region in the source tensor. `zero_coord` can also be passed in.|
|copy_shape|Input|Shape of the movement region, used to specify the size of the movement region.|

To configure runtime movement parameters, first bind the `l0c_to_ub_params` object to the movement atomic object through `atom.with(params)`. When it is not bound, the default value is used.

**Table 3** `make_copy` API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. For L0C Buffer to UB movement, use `copy_l0c_to_ub{}`. |
| trait | Input | Movement trait object. For L0C Buffer to UB movement, the default is `l0c_to_ub_trait_default{}`. |


### copy_l0c_to_ub Description

`copy_l0c_to_ub` is used to identify the L0C Buffer to UB data movement path. Only its public content is listed:

```cpp
struct copy_l0c_to_ub {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function is used to receive the Trait and movement parameters and dispatch the L0C Buffer to UB data movement.

### l0c_to_ub_trait Description

`l0c_to_ub_trait` is used to configure the static features of L0C Buffer to UB movement.

```cpp
enum class round_mode : uint8_t { default_round = 0, hybrid = 1 };

enum class dual_dst_mode : uint8_t { disable = 0, split_m = 1, split_n = 2 };

struct l0c_to_ub_trait {
    __aicore__ constexpr l0c_to_ub_trait() = default;
    __aicore__ constexpr l0c_to_ub_trait(asc::te::round_mode round_mode,
        bool enable_relu, bool enable_channel_split, dual_dst_mode dual_dst_ctl);

    asc::te::round_mode round_mode = asc::te::round_mode::default_round;
    bool enable_relu = false;
    bool enable_channel_split = false;
    dual_dst_mode dual_dst_ctl = dual_dst_mode::disable;
};
```

**Table 4** `l0c_to_ub_trait` members

|Member|Default Value|Description|
|--------|--------|--------|
|round_mode|`round_mode::default_round`|Rounding mode. `round_mode::hybrid` is supported only in the quantized output scenario where the source type is `float` and the destination type is `hifloat8_t`.|
|enable_relu|`false`|Whether to enable real-time ReLU.|
|enable_channel_split|`false`|Whether to enable output data channel split.|
|dual_dst_ctl|`dual_dst_mode::disable`|Dual-target mode control. The values include `dual_dst_mode::disable`, `dual_dst_mode::split_m`, and `dual_dst_mode::split_n`. For details, see [L0C Buffer to Unified Buffer Dual-Target Mode](../cube_store_key_features/l0c_to_ub_dual_dst.md).|

Example of using a custom trait:

```cpp
constexpr l0c_to_ub_trait l0c_to_ub_trait_value = {
    round_mode::default_round,
    false,
    false,
    dual_dst_mode::disable
};

struct copy_l0c_to_ub_trait_custom {
    using trait_type = l0c_to_ub_trait;
    static constexpr const trait_type value = l0c_to_ub_trait_value;
};
```

`l0c_to_ub_trait_default` provides the default Trait type and default value for this path:

```cpp
struct l0c_to_ub_trait_default {
    using trait_type = l0c_to_ub_trait;
    static constexpr const trait_type value = {};
};
```

### l0c_to_ub_params Description

`l0c_to_ub_params` is used to configure runtime movement parameters.

```cpp
enum class unit_flag_mode : uint8_t { disable = 0, enable_keep = 2, enable_update = 3 };

struct l0c_to_ub_params {
    __aicore__ constexpr l0c_to_ub_params() = default;
    __aicore__ constexpr l0c_to_ub_params(
        unit_flag_mode unit_flag, uint8_t sub_block_id = 0) :
        unit_flag(unit_flag), sub_block_id(sub_block_id)
    {}

    unit_flag_mode unit_flag = unit_flag_mode::disable;
    uint8_t sub_block_id = 0;
};
```

**Table 5** `l0c_to_ub_params` members

|Member|Default Value|Description|
|--------|--------|--------|
|unit_flag|`unit_flag_mode::disable`|Controls the fine-grained parallelism between the `mmad` instruction and the `Fixpipe` instruction. `unit_flag_mode::disable` indicates that it is not enabled. `unit_flag_mode::enable_keep` indicates that it is enabled and the unit flag bit is not reset after execution. `unit_flag_mode::enable_update` indicates that it is enabled and the unit flag bit is reset after execution.|
|sub_block_id|`0`|Indicates the number of the target UB when single-target mode is enabled.|

## Data Type

L0C Buffer to UB movement automatically selects the quantization mode based on whether a quantization parameter is passed in.

**Table 6** Data types

|Source Tensor Type (L0C Buffer)|Destination Tensor Type (UB)|Call form|Description|
|--------|--------|--------|--------|
|`int32_t`|`int8_t`, `uint8_t`|`copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`int8_t`, `uint8_t`|`copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`fp8_e4m3fn_t`, `hifloat8_t`|`copy(atom, dst, src, quant)`|Scalar or tensor quantized output. `hifloat8_t` supports `round_mode::default_round` and `round_mode::hybrid`.|
|`int32_t`|`half`, `bfloat16_t`|`copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`half`|`copy(atom, dst, src)`|Direct conversion output, corresponding to F32 to F16.|
|`float`|`bfloat16_t`|`copy(atom, dst, src)`|Direct conversion output, corresponding to F32 to BF16.|
|`float`|`half`, `bfloat16_t`|`copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`int32_t`|`int32_t`|`copy(atom, dst, src)`|Non-quantized output.|
|`float`|`float`|`copy(atom, dst, src)`|Non-quantized output.|
|`float`|`float`|`copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|

When `quant` is not passed in, the API selects the non-quantized or direct conversion mode. When `uint64_t` is passed in, it selects the scalar quantization mode. When a Tensor API tensor is passed in, it selects the tensor quantization mode.

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The quantization and ReLU parameters cannot be inf/nan or denormalized numbers.
- The source cube is in `NZ` format, and the address requires 64-byte alignment. The destination cube address requires 32-byte alignment.
- When the destination cube is `NZ` output, the size in the N direction must be a multiple of 16. For the `b8` type channel merge scenario, it must be a multiple of 32. For the `float` scenario with `enable_channel_split` channel split enabled, it must be a multiple of 8.
- The tensor quantization parameter tensor should be located in the L1 Buffer, with an element type of `uint64_t` and an address requiring 32-byte alignment. For detailed constraints, see [Real-Time Quantization](../cube_store_key_features/quant_pre.md).
- When `unit_flag` is enabled, it must be enabled together with `mmad`.
- When `round_mode::hybrid` is used, the source type must be `float` and the destination type must be `hifloat8_t`.
- When the destination layout is `NZ`, setting `dual_dst_ctl` to `dual_dst_mode::split_n` is not supported. For detailed constraints on dual-target mode, see [L0C Buffer to Unified Buffer Dual-Target Mode](../cube_store_key_features/l0c_to_ub_dual_dst.md).
- The channel merge feature is automatically enabled by hardware and cannot be configured through parameters. For detailed constraints, see [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md).

## Key Features

L0C Buffer to UB movement involves key features such as [Real-Time Quantization](../cube_store_key_features/quant_pre.md), [Real-Time ReLU](../cube_store_key_features/relu_pre.md), [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md), [L0C Buffer to Unified Buffer Dual-Target Mode](../cube_store_key_features/l0c_to_ub_dual_dst.md), and [Batch Movement](../cube_store_key_features/batch_copy.md).

## Example

The two parameter-passing forms of `make_copy`: `make_copy(operation, trait)` directly specifies the static trait; `make_copy(operation)` constructs using the default trait of the path, and can further bind the `l0c_to_ub_params` runtime parameters through `.with(params)`. When params is not bound, the default value is used.

### Non-Quantized Output

In non-quantized mode, the data in the L0C is directly output to the UB (when the source and destination types are the same), or directly converted and output between `float` and `half`/`bfloat16_t`.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_ub_example(__cc__ float* l0c_addr, __ubuf__ float* ub_addr, uint32_t m, uint32_t n)
{
    // Source tensor: the mmad output in the L0C, fixed to the NZ format, with C0 set to 16.
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    // Destination tensor: the nd_ext layout (row-major extended layout) is used on the UB side.
    auto ub = make_tensor(make_mem_ptr(ub_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));

    l0c_to_ub_params params;
    auto atom = make_copy(copy_l0c_to_ub{}).with(params);
    copy(atom, ub, l0c);
}
```

### Scalar Quantized Output

In scalar quantization mode, the entire C cube shares the same quantization parameter. The quantization parameter is passed in through a `uint64_t` scalar.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_ub_example(__cc__ float* l0c_addr, __ubuf__ half* ub_addr, uint32_t m, uint32_t n)
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto ub = make_tensor(make_mem_ptr(ub_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));

    uint64_t quant = 1;
    auto atom = make_copy(copy_l0c_to_ub{}).with(l0c_to_ub_params{});
    copy(atom, ub, l0c, quant);
}
```

### Tensor Quantized Output

In tensor quantization mode, the quantization parameter is a tensor located in the L1 Buffer, and each channel dimension corresponds to one quantization parameter. The quantization tensor element type is `uint64_t`, and the shape is `(1, n)`.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_ub_example(__cc__ float* l0c_addr, __ubuf__ half* ub_addr, __cbuf__ uint64_t* l1_quant_addr, uint32_t m, uint32_t n)
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto ub = make_tensor(make_mem_ptr(ub_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));
    auto quant = make_tensor(make_mem_ptr(l1_quant_addr), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto atom = make_copy(copy_l0c_to_ub{}).with(l0c_to_ub_params{});
    copy(atom, ub, l0c, quant);
}
```

### Batch Mode Output

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_batch_l0c_to_ub()
{
    constexpr uint32_t batch = 3;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cc__ float l0c_buf[batch * m * n];
    __ubuf__ half ub_buf[batch * m * n];

    auto l0c = make_tensor(make_mem_ptr(l0c_buf), make_frame_layout<nz_layout_ptn>(batch, m, n));
    auto ub = make_tensor(make_mem_ptr(ub_buf), make_frame_layout<nd_ext_layout_ptn>(batch, m, n));

    auto atom = make_copy(copy_l0c_to_ub{}).with(l0c_to_ub_params{});
    copy(atom, ub, l0c);
}
```
