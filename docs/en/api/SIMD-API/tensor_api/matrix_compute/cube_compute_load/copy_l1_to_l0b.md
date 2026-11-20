# copy (Movement from L1 Buffer to L0B Buffer)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:44:28.440Z pushedAt=2026-10-27T13:03:35.326Z nl=2 -->

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

The Tensor API uses the `copy` API to uniformly perform movement across different paths. This API is used to move the right cube data in the L1 Buffer to the L0B Buffer.

The API supports complete tensor movement. It also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`.

This path uses a 512-byte data fractal as the basic movement unit. The fractal cube shapes corresponding to different data types are as follows:

**Table 1**  Data bit width and fractal cube shape

| Data Bit Width | Fractal Cube Shape |
| :--- | :--- |
| b4 | 64 * 16 |
| b8 | 32 * 16 |
| b16 | 16 * 16 |
| b32 | 8 * 16 |

The API supports non-transpose movement and transpose movement. The layouts of the source tensor and the destination tensor must match the format combinations supported by the current path, as shown in Table 2.

**Table 2**  Copy mode and format combination

| Copy mode | Source layout -> Destination layout |
| :--- | :--- |
| Non-transpose movement | `Zn -> Zn` |
| Transpose movement | `Nz -> Zn` |

The API supports Batch mode. In Batch mode, the layouts of the source tensor and the destination tensor must add a Batch dimension before the original fractal layout, with a shape structure of `(b, single-cube shape)`, where `b` indicates the batch count. The batch counts of the source tensor and the destination tensor must be consistent. The Batch dimension stride must be consistent with the physical space occupied by a single batch of data, that is, `Batch dimension stride = [capacity()](../../layout/capacity.md) of the single-batch layout`. When calculated in bytes, `Batch dimension stride * sizeof(element type) = physical bytes occupied by a single batch of data`. You can use `make_frame_layout<zn_layout_ptn, DataType>(b, k, n)` or `make_frame_layout<nz_layout_ptn, DataType>(b, k, n)` to construct a contiguous Batch layout.

## Function Prototype

- Performs movement from L1 Buffer to L0B Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and performs the movement from L1 Buffer to L0B Buffer using the [default trait](../../algorithm.md#copy_traits).

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs L1 Buffer to L0B Buffer movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and performs L1 Buffer to L0B Buffer movement using the default trait according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Constructs a movement atomic object using the default trait.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- Constructs a movement atomic object using the specified trait.

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(
        const CopyOperation& operation, const CopyTrait& trait)
    ```

## Parameters

**Table 3** Template parameter description of the `copy` API

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type. |
| SrcTensor | Source tensor type. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |

**Table 4** Template parameter description of the `make_copy` API

| Parameter | Description |
| :--- | :--- |
| CopyOperation | Type of the movement operation object. |
| CopyTrait | Type of the movement trait object. |

**Table 5**  Parameters of the `copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, whose storage location must be `location::l0b`. |
| src | Input | Source tensor, whose storage location must be `location::l1`. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor; can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor; can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 6**  Parameters of the `make_copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. The L1 Buffer to L0B Buffer path uses `copy_l1_to_l0b{}`. |
| trait | Input | Movement trait object, used to specify the movement characteristics. The default trait for the L1 Buffer to L0B Buffer path uses `l1_to_l0b_trait_default{}`. |

### copy_l1_to_l0b Description

`copy_l1_to_l0b` identifies the L1 Buffer to L0B Buffer movement path. Only its public members are listed:

```cpp
struct copy_l1_to_l0b {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and movement parameters, and dispatches the movement from L1 Buffer to L0B Buffer.

### `l1_to_l0b_trait` Description

`l1_to_l0b_trait` represents the static configuration of this path. It currently has no configurable members and is defined as follows:

```cpp
struct l1_to_l0b_trait {};
```

`l1_to_l0b_trait_default` provides the default Trait type and default value for this path:

```cpp
struct l1_to_l0b_trait_default {
    using trait_type = l1_to_l0b_trait;
    static constexpr const trait_type value = {};
};
```


## Data Type

The supported data types include:

`fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, and `float`.

The data types of the source tensor and the destination tensor must be consistent.

## Return Value

- `copy` has no return value.
- `make_copy` returns a `copy_atom` object.

## Constraints

- The destination address is located in the L0B Buffer, and its start address must meet the 512-byte alignment requirement.
- The source address is located in the L1 Buffer, and its start address must meet the 32-byte alignment requirement.
- Special data type constraints: When transpose is enabled on the L1 Buffer-to-L0B Buffer path, that is, `NZ2ZN`, the supported data type constraints are as follows:
  - The b32 data type requires the source cube shape to be 16-aligned in the K-axis direction.
  - The b8 data type requires the source cube shape to be 32-aligned in the M-axis direction.
  - The b4 data type requires the source cube shape to be 64-aligned in the M-axis direction.
- In Batch mode, the source tensor and destination tensor must have the same Batch count, and the tensor data must be contiguously arranged in both L1 Buffer and L0B Buffer.

## Key Features

### Non-transpose movement

When both the source tensor and destination tensor use the `ZN` data format, the API copies data from the L1 Buffer to the L0B Buffer in a non-transpose manner.

**Figure 1**  Non-transpose movement schematic diagram

![Non-transpose movement schematic diagram](../../../../figures/load2dv2_start_addr_demo.png)

### Transpose movement

When the source tensor data format is `NZ` and the destination tensor data format is `ZN`, the API performs movement in transpose mode. Transpose movement transposes each fractal cube of the source tensor.

The transpose movement schematic diagrams for different data bit widths are as follows.

#### b4 Transpose Movement

**Figure 2**  b4 transpose movement schematic diagram

![b4 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b4.png)

#### b8 transpose movement

**Figure 3**  b8 transpose movement schematic diagram

![b8 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b8.png)

#### b16 Transpose Movement

**Figure 4**  b16 transpose movement schematic diagram

![b16 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b16.png)

#### b32 transpose movement

**Figure 5**  b32 transpose movement schematic diagram

![b32 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b32.png)

### Batch Copy

When both the source tensor and the destination tensor use a Layout with a Batch dimension, the `copy` API completes the movement from the L1 Buffer to the L0B Buffer according to the data range described by the Batch dimension. For non-transpose movement, both the source tensor and the destination tensor use the `ZN` format; for transpose movement, the source tensor uses the `NZ` format and the destination tensor uses the `ZN` format. The Batch dimension is the outermost dimension, the Batch counts of the source and destination must be identical, and the Batch Stride of each Batch must equal the physical space occupied by the corresponding single-Batch Layout.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0b_example()
{
    using b_type = half;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ b_type l1_buf[k * n];
    __cb__ b_type l0b_buf[k * n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<zn_layout_ptn, b_type>(k, n));

    auto l0b_tensor = make_tensor(make_mem_ptr(l0b_buf), make_frame_layout<zn_layout_ptn, b_type>(k, n));

    auto atom = make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{});
    copy(atom, l0b_tensor, l1_tensor);
}
```

The following shows an example of Batch mode.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0b_batch_example()
{
    using b_type = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ b_type l1_buf[batch * k * n];
    __cb__ b_type l0b_buf[batch * k * n];

    auto layout = make_frame_layout<zn_layout_ptn, b_type>(batch, k, n);
    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), layout);
    auto l0b_tensor = make_tensor(make_mem_ptr(l0b_buf), layout);

    auto atom = make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{});
    copy(atom, l0b_tensor, l1_tensor);
}
```
