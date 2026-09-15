# **Copy** (Data Transfer from L1 Buffer to L0ScaleA Buffer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:20:13.271Z -->

> [!NOTE] Note
> This API is an experimental API and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers should pay attention to later version updates when using it.

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

The Tensor API uses the `Copy` API to uniformly perform data movement across different paths. This API moves the left cube scaling data in the L1 Buffer to the L0ScaleA Buffer. The start address of the left cube scaling data in the L0ScaleA Buffer is derived from 1/16 of the start address of the left cube in the L0A Buffer.

The data type of this path is fixed to `fp8_e8m0_t`. The fractal layout and scaling computation relationship of the scaling data of the left cube and right cube can be referenced in the following figure, where the left cube scaling data is in the `ZZ` data format:

**Figure 1** Scaling diagram of the left cube and right cube scaling data

![Scaling diagram of the left cube and right cube scaling data](../../../../figures/zh-cn_image_0000002549011155.png)

## Prototype

- Performs data movement from **L1 Buffer** to **L0ScaleA Buffer**.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- Constructs a default copy atomic object.

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- Constructs a copy atomic object with the specified **Trait**.

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(
        const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## Parameters

**Table 1** Parameters of the `Copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atomCopy | Input | Copy atom object. For the L1 Buffer to L0ScaleA Buffer path, it can be constructed through `MakeCopy(CopyL12L0ScaleA{})` or `MakeCopy(CopyL12L0ScaleA{}, CopyL12L0ScaleATraitDefault{})`. |
| dst | Output | Destination tensor, stored at `Location::L0ScaleA`, with the data format `ZZ`. |
| src | Input | Source tensor, stored at `Location::L1`, with the data format `ZZ`. |

**Table 2** Parameters of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Copy operation object. The L1 Buffer to L0ScaleA Buffer path uses `CopyL12L0ScaleA{}`. |
| copyTrait | Input | Copy trait object, used to specify the copy traits. The default trait for the L1 Buffer to L0ScaleA Buffer path uses `CopyL12L0ScaleATraitDefault{}`. |

## Data Type

Only the `fp8_e8m0_t` data type is supported.

The source tensor and the destination tensor must use the same data type.

## Return Value

`Copy` has no return value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The data format of the source tensor and destination tensor is `ZZ`, which is constructed using `MakeFrameLayout<ZZLayoutPtn, _2>(...)`.
- The source address and destination address must meet the 32-byte alignment requirement.

## Example

```cpp
#include "tensor_api/tensor.h"

 using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0ScaleAExample()
{
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 32;
    constexpr uint32_t scaleK = k / 16;

    __cbuf__ fp8_e8m0_t l1Buf[m * scaleK];
    __ca__ fp8_e5m2_t l0ABuf[m * k];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<ZZLayoutPtn, _2>(m, scaleK));

    // The ScaleA address is derived from the left cube's L0A Buffer address by 1/16 address encoding.
    auto l0ScaleAPtr = MakeMemPtr<Location::L0ScaleA, fp8_e8m0_t>(reinterpret_cast<uint64_t>(l0ABuf) / 16);
    auto l0ScaleATensor = MakeTensor(l0ScaleAPtr, MakeFrameLayout<ZZLayoutPtn, _2>(m, scaleK));

    auto copyAtom = MakeCopy(CopyL12L0ScaleA{}, CopyL12L0ScaleATraitDefault{});
    Copy(copyAtom, l0ScaleATensor, l1Tensor);
}
```
