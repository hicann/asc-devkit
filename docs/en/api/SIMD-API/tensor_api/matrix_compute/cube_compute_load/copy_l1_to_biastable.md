# copy (Movement from L1 Buffer to BiasTable Buffer)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:40:36.039Z pushedAt=2026-10-27T13:03:35.322Z nl=2 -->

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

The Tensor API uses the `copy` API to uniformly perform movement across different paths. The BiasTable Buffer is used to store bias data in cube computation. This API moves bias data from the L1 Buffer to the BiasTable Buffer for use as the bias input in mmad computation.

The API supports moving a complete tensor, and also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`.

The API supports batch mode. Batch mode is used to move multiple bias data blocks at once. The layout adds a Batch dimension to the outermost layer of the original layout, which can be constructed through `make_frame_layout<nd_layout_ptn>(batch, 1, n)` or `make_frame_layout<nd_ext_layout_ptn>(batch, 1, n)`.

## Function Prototype

- Performs movement from the L1 Buffer to the BiasTable Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and the destination tensor, and uses the [default trait](../../algorithm.md#copy_traits) to perform movement from the L1 Buffer to the BiasTable Buffer.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs L1 Buffer to BiasTable Buffer movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and the destination tensor, and uses the default trait to perform L1 Buffer to BiasTable Buffer movement according to the specified source coordinate, destination coordinate, and movement shape.

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
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation, const CopyTrait& trait)
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

**Table 2**  Parameters of the copy API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, stored at `location::bias` in ND format. |
| src | Input | Source tensor, stored at `location::l1` in ND format. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor; can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor; can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 3**  Parameters of the make_copy API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. The L1 Buffer to BiasTable Buffer path uses `copy_l1_to_biastable{}`. |
| trait | Input | Movement trait object, used to specify the movement characteristics. The default trait for the L1 Buffer to BiasTable Buffer path is `l1_to_biastable_trait_default{}`. |

### **copy_l1_to_biastable** Description

`copy_l1_to_biastable` identifies the movement path from the L1 Buffer to the BiasTable Buffer. Only its public members are listed:

```cpp
struct copy_l1_to_biastable {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and movement parameters, and dispatches the movement from the L1 Buffer to the BiasTable Buffer.

### l1_to_biastable_trait Description

`l1_to_biastable_trait` represents the static configuration of this path. Currently, it has no configurable members and is defined as follows:

```cpp
struct l1_to_biastable_trait {};
```

`l1_to_biastable_trait_default` provides the default Trait type and default value for this path:

```cpp
struct l1_to_biastable_trait_default {
    using trait_type = l1_to_biastable_trait;
    static constexpr const trait_type value = {};
};
```

## Data Type

The supported combinations of destination tensor and source tensor data types are as follows:

**Table 4**  Supported data type combinations

| Destination Tensor Data Type | Source Tensor Data Type |
| :--- | :--- |
| `float` | `bfloat16_t` |
| `float` | `half` |
| `float` | `float` |
| `int32_t` | `int32_t` |

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The source tensor memory address and size must meet the 32-byte alignment requirement.
- The destination tensor memory address and size must meet the 64-byte alignment requirement.
- Because the BiasTable Buffer size is 4 KB, at most 4 KB of data can be copied at a time.
- In batch mode, the source tensor and destination tensor must have the same batch count.
- In batch mode, if the shape size on Global Memory is not aligned, the column-direction shape sizes of the source tensor and destination tensor must be aligned to 32 bytes because of the data alignment requirements on the L1 Buffer and BiasTable Buffer.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_biastable_example()
{
    using src_type = half;
    using dst_type = float;
    constexpr uint32_t n = 16;

    __cbuf__ src_type l1_buf[n];
    __biasbuf__ dst_type bt_buf[n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto bt_tensor = make_tensor(make_mem_ptr(bt_buf), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto atom = make_copy(copy_l1_to_biastable{}, l1_to_biastable_trait_default{});
    copy(atom, bt_tensor, l1_tensor);
}
```

Batch mode call example:

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_batch_l1_to_biastable_example()
{
    using src_type = half;
    using dst_type = float;
    constexpr uint32_t batch = 2;
    constexpr uint32_t n = 15;

    // In batch mode, the column-direction shape of the source/destination tensor must be aligned to c0_size (32 bytes),
    // rounded up to c0_element<src_type> and c0_element<dst_type>, respectively.
    constexpr uint32_t src_n = Std::ceil_align(n, Std::Int<c0_element<src_type>>{});
    constexpr uint32_t dst_n = Std::ceil_align(n, Std::Int<c0_element<dst_type>>{});

    __cbuf__ src_type l1_buf[batch * src_n];
    __biasbuf__ dst_type bt_buf[batch * dst_n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nd_ext_layout_ptn>(batch, 1, src_n));

    auto bt_tensor = make_tensor(make_mem_ptr(bt_buf), make_frame_layout<nd_ext_layout_ptn>(batch, 1, dst_n));

    auto atom = make_copy(copy_l1_to_biastable{}, l1_to_biastable_trait_default{});
    copy(atom, bt_tensor, l1_tensor);
}
```
