# Copy (L1 Buffer to L0A Buffer Data Copy)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:18:10.379Z -->

> [!NOTE] Description
> This is an experimental API that may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in subsequent versions.

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

The Tensor API uses the `Copy` API to uniformly perform data copy over different paths. This API is used to copy the left cube data in the L1 Buffer to the L0A Buffer.

This path uses a 512-byte data fractal as the basic copy unit. The fractal cube shapes corresponding to different data types are as follows:

| data bit width | Fractal Cube Shape |
| :--- | :--- |
| b4 | 16 * 64 |
| b8 | 16 * 32 |
| b16 | 16 * 16 |
| b32 | 16 * 8 |

The API supports non-transpose copy and transpose copy. The Layout of the source tensor and the destination tensor must match the format combinations supported by the current path.

The API supports Batch mode. In Batch mode, the Layout of the source tensor and the destination tensor must add a Batch dimension before the original fractal Layout, with the Shape in the form of `(B, single-cube Shape)`, where `B` indicates the number of batches. You can use `MakeFrameLayout<NZLayoutPtn, DataType>(B, m, k)` or `MakeFrameLayout<ZNLayoutPtn, DataType>(B, m, k)` to construct a Layout with a Batch dimension.

## Prototype

- Performs data copy from the L1 Buffer to the L0A Buffer.

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
| atomCopy | Input | Copy atom object. For the L1 Buffer-to-L0A Buffer path, it can be constructed via `MakeCopy(CopyL12L0A{})` or `MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{})`. |
| dst | Output | Destination tensor, stored at `Location::L0A`. |
| src | Input | Source tensor, stored at `Location::L1`. |

**Table 2**  Parameters of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Copy operation object. For the L1 Buffer-to-L0A Buffer path, use `CopyL12L0A{}`. |
| copyTrait | Input | Copy trait object, used to specify the copy characteristics. For the L1 Buffer-to-L0A Buffer default trait, use `CopyL12L0ATraitDefault{}`. |

## Data Types

Supported data types include:

`fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `float`.

The data types of the source tensor and the destination tensor must be consistent.

## Return Value

`Copy` returns no value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- When the destination address is in the **L0A Buffer**, the start address must meet the 512-byte alignment requirement.
- When the source address is in the **L1 Buffer**, the start address must meet the 32-byte alignment requirement.
- Special data type constraints: When transpose is enabled on the L1 Buffer-to-L0A Buffer path, that is, `ZN2NZ`, the supported data type constraints are as follows:
  - The b32 data type requires the source cube shape to be 16-aligned in the K axis direction.
  - The b8 data type requires the source cube shape to be 32-aligned in the M axis direction.
  - The b4 data type requires the source cube shape to be 64-aligned in the M axis direction.
- Batch mode requires the source tensor and the destination tensor to have the same number of batches, and the tensor data must be contiguously arranged in the **L1 Buffer** and **L0A Buffer**.

## Key Feature Description

### Non-transpose Copy

When the source tensor and destination tensor use the `NZ` data format, the API copies data from the L1 Buffer to the L0A Buffer in a non-transpose manner.

**Figure 1**  Non-transpose copy schematic diagram

![Non-transpose copy schematic diagram](../../../../figures/notranspose.png)

### Transpose Copy

When the source tensor data format is `ZN` and the destination tensor data format is `NZ`, the API performs a transpose copy. Transpose copy transposes each fractal cube in the source tensor.

The transpose copy schematic diagrams for different data bit widths are as follows.

#### b4 transpose copy

**Figure 2**  b4 transpose copy schematic diagram

![b4 transpose copy schematic diagram](../../../../figures/transpose_b4.png)

#### b8 Transpose Copy

**Figure 3**  b8 transpose copy schematic diagram

![b8 transpose copy schematic diagram](../../../../figures/transpose_b8.png)

#### b16 Transpose Copy

**Figure 4**  b16 transpose copy schematic diagram

![b16 transpose copy schematic diagram](../../../../figures/transpose_b16.png)

#### b32 Transpose Copy

**Figure 5**  Schematic diagram of b32 transpose copy

![Schematic diagram of b32 transpose copy](../../../../figures/transpose_b32.png)

### Batch Copy

When both the source tensor and the destination tensor use a layout with a Batch dimension, the `Copy` API copies data from the L1 Buffer to the L0A Buffer according to the data range described by the Batch dimension. For non-transpose copy, both the source tensor and the destination tensor use the `NZ` format; for transpose copy, the source tensor uses the `ZN` format and the destination tensor uses the `NZ` format.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0AExample()
{
    using AType = half;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ AType l1Buf[m * k];
    __ca__ AType l0aBuf[m * k];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, AType>(m, k));

    auto l0aTensor = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, AType>(m, k));

    auto copyAtom = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyAtom, l0aTensor, l1Tensor);
}
```

The following shows an example in **Batch** mode.

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0ABatchExample()
{
    using AType = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ AType l1Buf[batch * m * k];
    __ca__ AType l0aBuf[batch * m * k];

    auto layout = MakeFrameLayout<NZLayoutPtn, AType>(batch, m, k);
    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), layout);
    auto l0aTensor = MakeTensor(MakeMemPtr(l0aBuf), layout);

    auto copyAtom = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyAtom, l0aTensor, l1Tensor);
}
```
