# Copy (L1 Buffer to L0ScaleB Buffer Data Transfer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:22:12.695Z -->

> [!NOTE]Note
>This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers should pay attention to subsequent version updates during use.

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

The Tensor API uses the `Copy` API to uniformly perform data movement across different paths. This API is used to move the right cube scaling data in the L1 Buffer to the L0ScaleB Buffer. The start address of the right cube scaling data in the L0ScaleB Buffer is derived from 1/16 of the start address of the right cube in the L0B Buffer.

The data type of this path is fixed to `fp8_e8m0_t`. The fractal layout and scaling computation relationship of the scaling data of the left and right matrices are shown in the following figure, where the right cube scaling data uses the `NN` data format:

**Figure 1** Scaling diagram of the left and right cube scaling data

![Scaling diagram of the left and right cube scaling data](../../../../figures/zh-cn_image_0000002549011155.png)

## Prototype

- Execute data movement from the L1 Buffer to the L0ScaleB Buffer.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- Construct a default movement atom object.

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- Construct a movement atom object with the specified Trait.

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(
        const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## Parameters

**Table 1**  Parameters of the `Copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atomCopy | Input | Copy atom object. For the L1 Buffer-to-L0ScaleB Buffer path, it can be constructed through `MakeCopy(CopyL12L0ScaleB{})` or `MakeCopy(CopyL12L0ScaleB{}, CopyL12L0ScaleBTraitDefault{})`. |
| dst | Output | Destination tensor, stored at `Location::L0ScaleB`, with the data format `NN`. |
| src | Input | Source tensor, stored at `Location::L1`, with the data format `NN`. |

**Table 2**  Parameters of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Copy operation object. The L1 Buffer-to-L0ScaleB Buffer path uses `CopyL12L0ScaleB{}`. |
| copyTrait | Input | Copy Trait object, used to specify the copy characteristics. The default Trait for the L1 Buffer-to-L0ScaleB Buffer path uses `CopyL12L0ScaleBTraitDefault{}`. |

## Data Type

Only the `fp8_e8m0_t` data type is supported.

The base data types of the source tensor and the destination tensor must be consistent.

## Return Value

`Copy` returns nothing. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The source tensor and destination tensor use the `NN` data format and are constructed with `MakeFrameLayout<NNLayoutPtn, _2>(...)`.
- The source address and destination address must meet the 32-byte alignment requirement.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0ScaleBExample()
{
    constexpr uint32_t k = 32;
    constexpr uint32_t n = 16;
    constexpr uint32_t scaleK = k / 16;

    __cbuf__ fp8_e8m0_t l1Buf[scaleK * n];
    __cb__ fp8_e5m2_t l0BBuf[k * n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NNLayoutPtn, _2>(scaleK, n));

    // The ScaleB address is derived from the L0B Buffer address of the right cube by 1/16 address encoding.
    auto l0ScaleBPtr = MakeMemPtr<Location::L0ScaleB, fp8_e8m0_t>(reinterpret_cast<uint64_t>(l0BBuf) / 16);
    auto l0ScaleBTensor = MakeTensor(l0ScaleBPtr, MakeFrameLayout<NNLayoutPtn, _2>(scaleK, n));

    auto copyAtom = MakeCopy(CopyL12L0ScaleB{}, CopyL12L0ScaleBTraitDefault{});
    Copy(copyAtom, l0ScaleBTensor, l1Tensor);
}
```
