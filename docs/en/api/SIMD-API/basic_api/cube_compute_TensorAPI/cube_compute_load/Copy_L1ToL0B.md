# Copy (Data Copy from L1 Buffer to L0B Buffer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:18:30.422Z -->

> [!NOTE] Description
> This API is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions during use.

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
- Atlas 200I/500 A2 inference product: Not supported
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

The Tensor API uniformly performs data copy over different paths through the `Copy` API. This API is used to copy the right cube data in the L1 Buffer to the L0B Buffer.

This path uses a 512-byte data fractal as the basic copy unit. The fractal cube shapes corresponding to different data types are as follows:

| data bit width | Fractal Cube Shape |
| :--- | :--- |
| b4 | 64 * 16 |
| b8 | 32 * 16 |
| b16 | 16 * 16 |
| b32 | 8 * 16 |

The API supports non-transpose copy and transpose copy. The layouts of the source tensor and destination tensor must match the format combinations supported by the current path.

The API supports Batch mode. In Batch mode, the layouts of the source tensor and destination tensor must add a Batch dimension before the original fractal layout, with the shape in the form of `(B, single-cube shape)`, where `B` indicates the number of batches. You can use `MakeFrameLayout<ZNLayoutPtn, DataType>(B, k, n)` or `MakeFrameLayout<NZLayoutPtn, DataType>(B, k, n)` to construct a layout with a Batch dimension.

## Prototype

- Copies data from the L1 Buffer to the L0B Buffer.

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

**Table 1** Parameters of the `Copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atomCopy | Input | Copy atom object. For the L1 Buffer to L0B Buffer path, it can be constructed through `MakeCopy(CopyL12L0B{})` or `MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{})`. |
| dst | Output | Destination tensor, whose storage location must be `Location::L0B`. |
| src | Input | Source tensor, whose storage location must be `Location::L1`. |

**Table 2** Parameters of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Copy operation object. For the L1 Buffer to L0B Buffer path, use `CopyL12L0B{}`. |
| copyTrait | Input | Copy Trait object, used to specify the copy characteristics. For the L1 Buffer to L0B Buffer path, the default Trait uses `CopyL12L0BTraitDefault{}`. |

## Data Types

The supported data types include:

`fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `float`.

The data types of the source tensor and the destination tensor must be consistent.

## Return Value

`Copy` returns no value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- When the destination address is in the L0B Buffer, the start address must meet the 512-byte alignment requirement.
- When the source address is in the L1 Buffer, the start address must meet the 32-byte alignment requirement.
- Special data type constraints: When transpose is enabled on the L1 Buffer-to-L0B Buffer path, that is, `NZ2ZN`, the supported data types are constrained as follows:
  - For the b32 data type, the source cube Shape must be 16-aligned in the K axis direction.
  - For the b8 data type, the source cube Shape must be 32-aligned in the M axis direction.
  - For the b4 data type, the source cube Shape must be 64-aligned in the M axis direction.
- In **Batch** mode, the source tensor and destination tensor must have the same batch count, and the tensor data must be contiguously arranged in the L1 Buffer and L0B Buffer.

## Key Features

### Non-transpose Copy

When the source tensor and destination tensor both use the `ZN` data format, the API copies data from the L1 Buffer to the L0B Buffer in a non-transpose manner.

**Figure 1**  Non-transpose copy schematic diagram

![Non-transpose copy schematic diagram](../../../../figures/notranspose.png)

### Transpose Copy

When the source tensor data format is `NZ` and the destination tensor data format is `ZN`, the API performs a transpose copy. Transpose copy transposes each fractal cube of the source tensor.

The following shows the transpose copy schematic diagrams for different data bit widths.

#### b4 Transpose Copy

**Figure 2**  Schematic diagram of b4 transpose copy

![Schematic diagram of b4 transpose copy](../../../../figures/transpose_b4.png)

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

When the source tensor and destination tensor both use a Layout with a Batch dimension, the `Copy` API completes the data copy from L1 Buffer to L0B Buffer according to the data range described by the Batch dimension. For non-transpose copy, both the source tensor and destination tensor use the `ZN` format; for transpose copy, the source tensor uses the `NZ` format and the destination tensor uses the `ZN` format.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0BExample()
{
    using BType = half;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ BType l1Buf[k * n];
    __cb__ BType l0bBuf[k * n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<ZNLayoutPtn, BType>(k, n));

    auto l0bTensor = MakeTensor(MakeMemPtr(l0bBuf), MakeFrameLayout<ZNLayoutPtn, BType>(k, n));

    auto copyAtom = MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{});
    Copy(copyAtom, l0bTensor, l1Tensor);
}
```

The following shows an example in Batch mode.

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0BBatchExample()
{
    using BType = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ BType l1Buf[batch * k * n];
    __cb__ BType l0bBuf[batch * k * n];

    auto layout = MakeFrameLayout<ZNLayoutPtn, BType>(batch, k, n);
    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), layout);
    auto l0bTensor = MakeTensor(MakeMemPtr(l0bBuf), layout);

    auto copyAtom = MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{});
    Copy(copyAtom, l0bTensor, l1Tensor);
}
```
