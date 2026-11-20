# copy (Movement from L1 Buffer to Fixpipe Buffer)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:41:08.381Z pushedAt=2026-10-27T13:03:35.325Z nl=2 -->

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

The Tensor API uses the `copy` API to uniformly perform movement across different paths. This API is used to move quantized data from L1 Buffer to Fixpipe Buffer. The quantized data can be used for in-line quantization when L0C outputs to GM/Unified Buffer (UB).

The API supports moving a complete tensor, and also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`.

## Function Prototype

- Performs movement from L1 Buffer to Fixpipe Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically deduces the movement path based on the storage locations of the source and destination tensors, and uses the [default trait](../../algorithm.md#copy_traits) to perform movement from L1 Buffer to Fixpipe Buffer.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs L1 Buffer to Fixpipe Buffer movement according to the specified source coordinates, destination coordinates, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically deduces the movement path based on the storage locations of the source tensor and destination tensor, and uses the default trait to perform L1 Buffer to Fixpipe Buffer movement according to the specified source coordinates, destination coordinates, and movement shape.

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

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type, which must be a Tensor API Tensor type. |
| SrcTensor | Source tensor type, which must be a Tensor API Tensor type. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |
| CopyOperation | Type of the movement operation object. |
| CopyTrait | Type of the movement trait object. |

**Table 2**  `copy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, stored at `location::fixbuf` with the `ND` data format. |
| src | Input | Source tensor, stored at `location::l1` with the `ND` data format. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor; can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor; can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 3**  `make_copy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Data operation object. The L1 Buffer to Fixpipe Buffer path uses `copy_l1_to_fixbuf{}`. |
| trait | Input | Movement trait object, used to specify the movement characteristics. The L1 Buffer to Fixpipe Buffer path uses `l1_to_fixbuf_trait_default{}` as the default trait. |

### **copy_l1_to_fixbuf** Description

`copy_l1_to_fixbuf` identifies the L1 Buffer-to-Fixpipe Buffer movement path. Only its public members are listed:

```cpp
struct copy_l1_to_fixbuf {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and movement parameters, and dispatches the L1 Buffer-to-Fixpipe Buffer movement.

### l1_to_fixbuf_trait Description

`l1_to_fixbuf_trait` represents the static configuration of this path. It currently has no configurable members and is defined as follows:

```cpp
struct l1_to_fixbuf_trait {};
```

`l1_to_fixbuf_trait_default` provides the default trait type and default value for this path:

```cpp
struct l1_to_fixbuf_trait_default {
    using trait_type = l1_to_fixbuf_trait;
    static constexpr const trait_type value = {};
};
```


## Data Type

The data type of both the source tensor and the destination tensor must be `uint64_t`.

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- Quantization parameters must not be inf/nan or denormalized numbers.
- The memory address and size of the source tensor must meet the 32-byte alignment requirement.
- The memory address and size of the destination tensor must meet the 128-byte alignment requirement.
- Because the Fixpipe Buffer size is 4 KB, each movement can move at most 4 KB of data.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_fixbuf_example()
{
    using data_type = uint64_t;
    constexpr uint32_t n = 16;

    __cbuf__ data_type l1_buf[n];
    __fbuf__ data_type fixbuf[n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto fixbuf_tensor = make_tensor(make_mem_ptr(fixbuf), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto atom = make_copy(copy_l1_to_fixbuf{}, l1_to_fixbuf_trait_default{});
    copy(atom, fixbuf_tensor, l1_tensor);
}
```
