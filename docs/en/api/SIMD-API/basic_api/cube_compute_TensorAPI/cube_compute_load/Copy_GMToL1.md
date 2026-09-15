# Copy (Global Memory to L1 Buffer Data Transfer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:14:30.920Z -->

> [!NOTE]
> This API is experimental and may be adjusted or improved in subsequent versions, with no guarantee of backward compatibility. Developers should pay attention to subsequent version updates during use.

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

The Tensor API uses the `Copy` API to uniformly perform data transfer over different paths. This API transfers data from Global Memory to L1 Buffer. The `Copy` API selects a specific transfer implementation based on the storage location, data type, and Layout of the source tensor and destination tensor. The number of transfer blocks, transfer length, source/destination stride, and format conversion information are derived from the Tensor Layout, so users do not need to pass additional transfer parameters in the `Copy` call.

This path is used for cube computation input transfer-in and supports the following scenarios:

- Contiguous data transfer-in: The data format remains unchanged after the source data is transferred from Global Memory to L1 Buffer.
- High-dimensional splitting data transfer-in: When strides exist on the source/destination side, multi-segment transfer is derived based on the Layout.
- On-the-fly format conversion transfer-in: Cube format conversion from ND/DN to NZ/ZN is completed while transferring data from Global Memory to L1 Buffer.
- Batch multi-cube transfer-in: When the source/destination Layout has a Batch axis, a single `Copy` completes the transfer of multiple matrices, supporting the following on-the-fly format conversions: `ND2ND`, `ND2NZ`, `DN2NZ`, `ND2ZN`, `DN2ZN`, `ScaleAND2ZZ`, `ScaleADN2ZZ`, `ZZ2ZZ`, `ScaleBND2NN`, `ScaleBDN2NN`, and `NN2NN`.
- Scale data transfer-in: Used to transfer ScaleA/ScaleB data from Global Memory to L1 Buffer in MX cube computation.

## Prototype

- Performs a transfer from Global Memory to L1 Buffer.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy,
        const DstTensor& dst, const SrcTensor& src)
    ```

- Constructs a transfer atomic object using the default trait.

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- Constructs a transfer atomic object using the specified trait.

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation,
        const CopyTraitType& copyTrait)
    ```

The Global Memory-to-L1 Buffer path uses `CopyGM2L1{}` as `copyOperation` and `CopyGM2L1TraitDefault{}` as the default trait object.

## Parameters

**Template Parameter Description**

| Parameter | Description |
| :--- | :--- |
| AtomType | Template parameter of `CopyAtom`, derived from `MakeCopy(CopyGM2L1{})` or `MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{})`. |
| DstTensor | Destination tensor type. For Global Memory to L1 Buffer transfer, it must be the Tensor API Tensor type, with the storage location being `Location::L1`. |
| SrcTensor | Source tensor type. For Global Memory to L1 Buffer transfer, it must be the Tensor API Tensor type, with the storage location being `Location::GM`. |
| CopyOperationType | Transfer operation object type. For Global Memory to L1 Buffer transfer, use `CopyGM2L1`. |
| CopyTraitType | Transfer trait object type. For Global Memory to L1 Buffer transfer, use `CopyGM2L1TraitDefault` by default. |

**Function Parameter Description**

**Table 1**  Parameters of the `Copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atomCopy | Input | Transfer atom object. For Global Memory to L1 Buffer transfer, it can be constructed through `MakeCopy(CopyGM2L1{})` or `MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{})`. |
| dst | Output | Destination tensor, with the storage location being `Location::L1`. |
| src | Input | Source tensor, with the storage location being `Location::GM`. |

**Table 2**  Parameters of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Transfer operation object. For Global Memory to L1 Buffer transfer, use `CopyGM2L1{}`. |
| copyTrait | Input | Transfer trait object. For Global Memory to L1 Buffer transfer, use `CopyGM2L1TraitDefault{}` by default. |

## Data Types

The data types of the source tensor and the destination tensor must be consistent.

| source operand data format | destination operand data format | source operand/destination operand data type |
| -- | -- | -- |
| ND | ND | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ND | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| ND | ZN | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | NZ | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NZ | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ZN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ScaleAND | ZZ | fp8_e8m0_t |
| ScaleADN | ZZ | fp8_e8m0_t |
| ZZ | ZZ | fp8_e8m0_t |
| ScaleBND | NN | fp8_e8m0_t |
| ScaleBDN | NN | fp8_e8m0_t |
| NN | NN | fp8_e8m0_t |

## Return Value

`Copy` has no return value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The source address in Global Memory must be 1B-aligned, and the destination address in L1 Buffer must be 32B-aligned.
- In the contiguous transfer scenario, the number of bytes to be transferred must be 32B-aligned. If it is not aligned, the transfer amount is rounded down to a 32B-aligned value.
- In the high-dimensional splitting transfer scenario, the transfer length, source stride, and destination stride are all 32B-aligned.
- When the input data is of the b4 type, it is processed at the b8-type transfer granularity, and the Layout derivation parameters must satisfy the corresponding granularity constraints.
- Batch multi-cube transfer-in supports only the following format conversions: `ND2ND`, `ND2NZ`, `DN2NZ`, `ND2ZN`, `DN2ZN`, `ScaleAND2ZZ`, `ScaleADN2ZZ`, `ZZ2ZZ`, `ScaleBND2NN`, `ScaleBDN2NN`, and `NN2NN`. The Batch count is limited by the field range of the transfer instruction and must not exceed 4095.

## Key Features

### Inline Format Conversion Transfer-in

Inline format conversion transfer-in means that cube format conversion is completed synchronously when data is transferred from Global Memory to L1 Buffer. The API automatically selects the conversion direction based on the source Layout and destination Layout, and supports the following format conversions:
`ND2ND`, `ND2NZ`, `ND2ZN`, `DN2NZ`, `DN2ZN`, `NZ2NZ`, `ZN2ZN`.
The format conversion from ND to NZ is equivalent to that from DN to ZN, and the format conversion from DN to NZ is equivalent to that from ND to ZN.

#### ND2NZ Contiguous Transfer

ND2NZ contiguous transfer means that the source cube is laid out contiguously in ND format and the destination cube is laid out contiguously in NZ format. In this scenario, the single-segment transfer parameters are automatically derived from the Layout, the ND-to-NZ format conversion is completed during the transfer process, and the destination side is padded with zeros to 32B alignment.

#### ND2NZ Non-contiguous Transfer

ND2NZ non-contiguous transfer indicates that high-dimensional splitting information such as stride and split dimensions exists on the source side or the destination side. This scenario falls under high-dimensional splitting data transfer-in. `Copy` automatically derives multi-segment transfer parameters based on `Layout`, completes the ND-to-NZ format conversion during the transfer process, and pads the destination side with zeros according to 32B alignment.

#### DN2NZ Contiguous Transfer

DN2NZ contiguous transfer means that the source cube is contiguously laid out in DN format and the destination cube is contiguously laid out in NZ format. In this scenario, **Layout** automatically derives single-segment transfer parameters, and the layout conversion from DN to NZ is completed during the transfer process, with the destination side padded with zeros to 32B alignment.

#### DN2NZ Non-contiguous Transfer

DN2NZ non-contiguous transfer indicates that high-dimensional splitting information such as stride and splitting dimensions exists on the source side or the destination side. This scenario falls under high-dimensional splitting data transfer-in. `Copy` automatically derives multi-segment transfer parameters based on `Layout`, completes the DN-to-NZ format conversion during the transfer process, and pads the destination side with zeros according to 32B alignment.

### Scale Data Transfer-in

Scale data transfer-in is used to transfer scale-related data from **Global Memory** to **L1 Buffer** during cube computation. The A-cube-related scale supports format conversion from `ScaleAND` to `ZZ`, from `ScaleADN` to `ZZ`, and from `ZZ` to `ZZ`. The B-cube-related scale supports format conversion from `ScaleBND` to `NN`, from `ScaleBDN` to `NN`, and from `NN` to `NN`.

### Batch Multi-Cube Transfer-in

Batch multi-cube transfer-in means that a single `Copy` completes the transfer of multiple matrices. When the layouts of the source tensor and destination tensor have a Batch axis, a single `Copy` instruction completes the transfer of multiple matrices. The API is called in exactly the same way as for single-cube transfer, with no additional Batch-related parameters required.

A layout with a Batch axis adds a Batch dimension outside the original cube layout. The Batch memory-contiguous scenario can be constructed with `MakeFrameLayout`, and the Batch memory non-contiguous scenario can be constructed with `MakeLayout` by explicitly specifying the shape and stride.

For example, the Batch memory-contiguous scenario is constructed with `MakeFrameLayout<LayoutPattern>(B, M, K)`, where `B` is the number of batches, and `M` and `K` are the rows and columns of a single cube. The layout semantics are as follows:

- The shape is `(B, (M, K))`.
- The stride is `(M * K, (K, 1))`, meaning that the matrices are laid out contiguously in memory at intervals of `M * K`, and a single cube is laid out contiguously in memory in the ND format.

The Batch memory non-contiguous scenario can be constructed with `MakeLayout(shape, stride)`, where `shape` describes the number of batches and the shape of a single cube, and `stride` describes the address interval of the Batch axis and each axis within the cube. Taking an ND cube laid out contiguously across batches in the K direction as an example, the layout semantics are as follows:

- The shape is `(B, (M, K))`.
- The stride is `(K, (B * K, 1))`, meaning that the matrices are non-contiguous in memory, and the memory is laid out contiguously across batches in the K direction.

Transfer behavior:

- Source (Global Memory) side: The source address spacing between Batch matrices is determined by the Batch axis stride of the Layout. With the Batch axis stride parameter, Batch data supports two layout forms between batches: contiguous and non-contiguous.
- Destination (L1 Buffer) side: Each Batch cube is stored contiguously in the L1 Buffer, and the address interval between batches equals the occupied space of a single cube after alignment.

## Example

The following example transfers an ND cube from Global Memory into the L1 Buffer and converts it to the NZ format.

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t M = 32;
constexpr uint32_t K = 64;

__aicore__ inline void CopyGmToL1(__gm__ half* gmAddr)
{
    __cbuf__ half l1Buf[M * K];

    // Source tensor: an ND-format cube in Global Memory; destination tensor: an NZ-format cube in the L1 Buffer.
    auto gmA = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(M, K));
    auto l1A = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, half>(M, K));

    // Construct the GM-to-L1 transfer atomic object. Copy automatically performs the on-the-fly ND-to-NZ format conversion based on the source/destination layouts.
    auto copyGm2L1 = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
    Copy(copyGm2L1, l1A, gmA);
}
```

The following example transfers B ND matrices from Global Memory into the L1 Buffer at once and converts them to the NZ format. Both the source tensor and the destination tensor use layouts with a Batch axis, and the API is invoked in the same way as for single-cube transfer.

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t B = 4;
constexpr uint32_t M = 32;
constexpr uint32_t K = 64;

__aicore__ inline void CopyBatchGmToL1(__gm__ half* gmAddr)
{
    __cbuf__ half l1Buf[B * M * K];

    // Construct the source/destination layouts with a Batch axis (B matrices). MakeFrameLayout adds a Batch dimension outside the original layout.
    auto gmA = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(B, M, K));
    auto l1A = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, half>(B, M, K));

    // The invocation is the same as for a single cube. Copy completes the ND-to-NZ transfer of B matrices with a single instruction.
    auto copyGm2L1 = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
    Copy(copyGm2L1, l1A, gmA);
}
```
