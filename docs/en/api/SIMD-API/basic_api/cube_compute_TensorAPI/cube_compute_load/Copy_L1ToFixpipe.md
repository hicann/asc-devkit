# Copy (Data Transfer from L1 Buffer to Fixpipe Buffer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:17:31.667Z -->

> [!NOTE]Note
> This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers should pay attention to updates in later versions during use.

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

The Tensor API uses the `Copy` API to uniformly perform data transfer across different paths. This API is used to transfer quantized data from the L1 Buffer to the Fixpipe Buffer. The quantized data can be used for on-the-fly quantization when L0C outputs data to GM/UB.

## Prototype

- Copies data from the L1 Buffer to the Fixpipe Buffer.

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
    __aicore__ inline constexpr auto MakeCopy(
        const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## Parameters

**Table 1**  Parameters of the `Copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| **atomCopy** | Input | Copy atom object. For the L1 Buffer to Fixpipe Buffer path, it can be constructed through `MakeCopy(CopyL12FB{})` or `MakeCopy(CopyL12FB{}, CopyL12FBTraitDefault{})`. |
| **dst** | Output | Destination tensor, with the storage location `Location::FIXBUF` and the data format `ND`. |
| **src** | Input | Source tensor, with the storage location `Location::L1` and the data format `ND`. |

**Table 2**  Parameters of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| **copyOperation** | Input | Copy operation object. The L1 Buffer to Fixpipe Buffer path uses `CopyL12FB{}`. |
| **copyTrait** | Input | Copy trait object, used to specify copy characteristics. The default trait for the L1 Buffer to Fixpipe Buffer path uses `CopyL12FBTraitDefault{}`. |

## Data Types

The data type of both the source tensor and the destination tensor must be `uint64_t`.

## Return Value

`Copy` has no return value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The source **Tensor** memory address and size must meet the 32-byte alignment requirement.
- The destination **Tensor** memory address and size must meet the 128-byte alignment requirement.
- Because the **Fixpipe Buffer** size is 4 KB, a maximum of 4 KB of data can be moved per transfer.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToFixpipeExample()
{
    using DataType = uint64_t;
    constexpr uint32_t n = 16;

    __cbuf__ DataType l1Buf[n];
    __fbuf__ DataType fixbuf[n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto fixbufTensor = MakeTensor(MakeMemPtr(fixbuf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto copyAtom = MakeCopy(CopyL12FB{}, CopyL12FBTraitDefault{});
    Copy(copyAtom, fixbufTensor, l1Tensor);
}
```
