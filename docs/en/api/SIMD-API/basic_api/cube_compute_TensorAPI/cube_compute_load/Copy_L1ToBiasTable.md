# Copy (Data Transfer from L1 Buffer to BiasTable Buffer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:17:30.302Z -->

> [!NOTE] Note
> This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

## Description

The header file path is `tensor_api/tensor.h`.

The Tensor API uses the `Copy` API to uniformly perform data transfer across different paths. This API is used to transfer bias data from the L1 Buffer to the BiasTable Buffer, serving as the bias input for Mmad computation.

The API supports batch mode. Batch mode is used to transfer multiple blocks of bias data in a single operation. The Layout adds a Batch dimension to the outermost layer of the original Layout, which can be constructed through `MakeFrameLayout<NDLayoutPtn>(batch, 1, n)` or `MakeFrameLayout<NDExtLayoutPtn>(batch, 1, n)`.

## Prototype

- Copies data from the L1 Buffer to the BiasTable Buffer.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- Constructs a default copy atom object.

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- Constructs a copy atom object with the specified Trait.

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## Parameters

**Table 1** Copy API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atomCopy | Input | Copy atom object. For the L1 Buffer to BiasTable Buffer path, it can be constructed via `MakeCopy(CopyL12BT{})` or `MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{})`. |
| dst | Output | Destination tensor, stored in `Location::BIAS`, in ND format. |
| src | Input | Source tensor, stored in `Location::L1`, in ND format. |

**Table 2** MakeCopy API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Copy operation object. For the L1 Buffer to BiasTable Buffer path, use `CopyL12BT{}`. |
| copyTrait | Input | Copy trait object, used to specify the copy characteristics. For the L1 Buffer to BiasTable Buffer path, the default trait uses `CopyL12BTTraitDefault{}`. |

## Data Types

The supported data type combinations of the destination tensor and source tensor are as follows:

**Table 3**  Supported data type combinations

| destination tensor data type | source tensor data type |
| :--- | :--- |
| `float` | `bfloat16_t` |
| `float` | `half` |
| `float` | `float` |
| `int32_t` | `int32_t` |

## Return Value

`Copy` has no return value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The memory address and size of the source tensor must meet the 32-byte alignment requirement.
- The memory address and size of the destination tensor must meet the 64-byte alignment requirement.
- Because the **BiasTable Buffer** size is 4 KB, at most 4 KB of data can be transferred at a time.
- In **batch** mode, the source tensor and the destination tensor must have the same batch count.
- In **batch** mode, if the shape size on **Global Memory** is not aligned, the column-direction shape size of the source tensor and the destination tensor must be aligned to 32 bytes because of the data alignment requirements on the **L1 Buffer** and the **BiasTable Buffer**.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToBTExample()
{
    using SrcType = half;
    using DstType = float;
    constexpr uint32_t n = 16;

    __cbuf__ SrcType l1Buf[n];
    __biasbuf__ DstType btBuf[n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto btTensor = MakeTensor(MakeMemPtr(btBuf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto copyAtom = MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{});
    Copy(copyAtom, btTensor, l1Tensor);
}
```

Example of calling in batch mode:

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyBatchL1ToBTExample()
{
    using SrcType = half;
    using DstType = float;
    constexpr uint32_t batch = 2;
    constexpr uint32_t n = 15;

    // In batch mode, the column-direction shapes of the source/destination tensors must be aligned to C0_SIZE (32 bytes),
    // and aligned upward to C0_ELEMENT<srcType> and C0_ELEMENT<dstType>, respectively.
    constexpr uint32_t srcN = Std::ceil_align(n, Std::Int<C0_ELEMENT<SrcType>>{});
    constexpr uint32_t dstN = Std::ceil_align(n, Std::Int<C0_ELEMENT<DstType>>{});

    __cbuf__ SrcType l1Buf[batch * srcN];
    __biasbuf__ DstType btBuf[batch * dstN];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(batch, 1, srcN));

    auto btTensor = MakeTensor(MakeMemPtr(btBuf), MakeFrameLayout<NDExtLayoutPtn>(batch, 1, dstN));

    auto copyAtom = MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{});
    Copy(copyAtom, btTensor, l1Tensor);
}
```
