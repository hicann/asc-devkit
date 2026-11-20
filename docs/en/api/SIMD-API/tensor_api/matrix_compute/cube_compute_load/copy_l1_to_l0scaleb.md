# copy (L1 Buffer to L0B_MX Buffer Movement)

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-27T02:46:54.431Z pushedAt=2026-10-27T13:03:35.331Z nl=2 -->

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

The Tensor API uniformly performs movement over different paths through the `copy` API. This API moves the right cube scaling data in the L1 Buffer to the L0B_MX Buffer. The starting address of the right cube scaling data in the L0B_MX Buffer is derived from 1/16 of the starting address of the right cube in the L0B Buffer.

The API supports moving a complete tensor, and also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`.

The data type of this path is fixed to `fp8_e8m0_t`. The logical cube shape of ScaleB is `(K / 32, N)`, and the physical storage uses the `NN` fractal layout, with a single fractal size of 2 * 16. When constructing the layout, the logical shape must be padded according to the physical fractal. Refer to the following figure for the fractal arrangement and scaling computation relationship of the scaling data of the left and right cubes, where the right cube scaling data is in the `NN` data format:

**Figure 1**  Scaling diagram of the left cube and right cube scaling data

![Scaling diagram of the left cube and right cube scaling data](../../../../figures/image_0000002549011155.png)

## Function Prototype

- Performs movement from the L1 Buffer to the L0B_MX Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and uses the [default trait](../../algorithm.md#copy_traits) to perform movement from the L1 Buffer to the L0B_MX Buffer.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs movement from the L1 Buffer to the L0B_MX Buffer according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and the destination tensor, and uses the default trait to perform movement from the L1 Buffer to the L0B_MX Buffer according to the specified source coordinate, destination coordinate, and movement shape.

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

**Table 1** Template parameter description of the `copy` API

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type, which must be a Tensor API Tensor type. |
| SrcTensor | Source tensor type, which must be a Tensor API Tensor type. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |

**Table 2** Template parameter description of the `make_copy` API

| Parameter | Description |
| :--- | :--- |
| CopyOperation | Type of the movement operation object. |
| CopyTrait | Type of the movement trait object. |

**Table 3** Parameter description of the `copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, stored at `location::l0scaleb` with the `NN` data format. |
| src | Input | Source tensor, stored at `location::l1` with the `NN` data format. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor; can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor; can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 4** Parameter description of the `make_copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. The L1 Buffer to L0B_MX Buffer path uses `copy_l1_to_l0scaleb{}`. |
| trait | Input | Movement trait object, used to specify the movement characteristics. The default trait for the L1 Buffer to L0B_MX Buffer path uses `l1_to_l0scaleb_trait_default{}`. |

### copy_l1_to_l0scaleb Description

`copy_l1_to_l0scaleb` identifies the movement path from L1 Buffer to L0B_MX Buffer. Only its public members are listed:

```cpp
struct copy_l1_to_l0scaleb {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and movement parameters, and dispatches the movement from L1 Buffer to L0B_MX Buffer.

### `l1_to_l0scaleb_trait` Description

`l1_to_l0scaleb_trait` represents the static configuration of this path. It currently has no configurable members and is defined as follows:

```cpp
struct l1_to_l0scaleb_trait {};
```

`l1_to_l0scaleb_trait_default` provides the default trait type and default value for this path:

```cpp
struct l1_to_l0scaleb_trait_default {
    using trait_type = l1_to_l0scaleb_trait;
    static constexpr const trait_type value = {};
};
```


## Data Type

Only the `fp8_e8m0_t` data type is supported.

The base data types of the source tensor and the destination tensor must be consistent.

## Return Value

- `copy` has no return value.
- `make_copy` returns a `copy_atom` object.

## Constraints

- The data format of the source tensor and the destination tensor is `NN`, which is constructed using `make_frame_layout<nn_layout_ptn, _2>(...)`.
- The source address and the destination address must meet the 32-byte alignment requirement.
- The logical shape of ScaleB is `(K / 32, N)`; the row and column parameters passed to `make_frame_layout` must meet the physical fractal requirement of the `NN` layout, and the physical storage space may contain padding data.
- The destination address of ScaleB is derived from the corresponding L0B cube address according to the 1/16 address encoding relationship. A regular L0B address cannot be used directly as an L0ScaleB address.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0scaleb_example()
{
    constexpr uint32_t k = 32;
    constexpr uint32_t n = 16;
    constexpr uint32_t scale_k = k / 16;

    __cbuf__ fp8_e8m0_t l1_buf[scale_k * n];
    __cb__ fp8_e5m2_t l0b_buf[k * n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nn_layout_ptn, _2>(scale_k, n));

    // The ScaleB address is derived from the L0B Buffer address of the right cube by 1/16 address encoding.
    auto l0scaleb_ptr = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>(reinterpret_cast<uint64_t>(l0b_buf) / 16);
    auto l0scaleb_tensor = make_tensor(l0scaleb_ptr, make_frame_layout<nn_layout_ptn, _2>(scale_k, n));

    auto atom = make_copy(copy_l1_to_l0scaleb{}, l1_to_l0scaleb_trait_default{});
    copy(atom, l0scaleb_tensor, l1_tensor);
}
```
