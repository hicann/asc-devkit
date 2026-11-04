# MaskReg Move-In (MaskGenWithRegTensor)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:45:24.812Z pushedAt=2026-10-26T07:41:44.577Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/MaskGenWithRegTensor_MaskReg_res.md#id1 -->

## Description

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

**MaskGenWithRegTensor** transfers data from [RegTensor](../register_data_types/RegTensor.md) to [MaskReg](../register_data_types/MaskReg.md).

- When the data type is b16, RegTensor (256B) is divided into 16 data blocks (16B). You can select a data block through a parameter. Each bit of the data block is broadcast into 2 bits and becomes 32B, which is the output MaskReg.
- When the data type is b32, RegTensor (256B) is divided into 32 data blocks (8B). You can select a data block through a parameter. Each bit of the data block is broadcast into 4 bits and becomes 32B, which is the output MaskReg.

For the detailed transfer principle, see [Key Features](#key-features).

## Function Prototype

```cpp
template <typename T = DefaultType, int16_t offset, typename U>
__simd_callee__ inline void MaskGenWithRegTensor(MaskReg& dst, U& srcReg)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| offset | offset determines the data block to be transferred from srcReg. For details, see [Key Features](#key-features).<br>&bull; When the data type is b16, offset∈[0, 15].<br>&bull; When the data type is b32, offset∈[0, 31]. |
| U | RegTensor type of the source operand. For example, RegTensor\<half>. It is automatically deduced by the compiler, and the user does not need to fill it in manually. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dst | Output | Destination operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| srcReg | Input | Source operand, type is [RegTensor](../register_data_types/RegTensor.md). |

## Data Type

The data types supported by the source operand are **b16** and **b32**.

## Return Value

None

## Key Features

According to [MaskReg Principle](../register_data_types/MaskReg.md), when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the least significant bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the least significant bit of the 4 bits is valid.

MaskGenWithRegTensor supports b16 and b32. The transfer principle is shown in [Figure 1](#fig-maskgen-b16) and [Figure 2](#fig-maskgen-b32).

- When the operand type is b16, the size of one data block of RegTensor is 16B. After each bit is broadcast into 2 bits, it becomes 32B, that is, one MaskReg (32B). In addition, one RegTensor has 256B/16B = 16 data blocks, and the offset value range is [0, 15].
- When the operand type is b32, the size of one data block of RegTensor is 8B. After each bit is broadcast into 4 bits, it becomes 32B, that is, one MaskReg (32B). In addition, one RegTensor has 256B/8B = 32 data blocks, and the offset value range is [0, 31].

**Figure 1**  MaskGenWithRegTensor transfer principle (b16)<a id="fig-maskgen-b16"></a>  
![](../../../../figures/reg_maskgenwithregtensor_b16.png "MaskGenWithRegTensor transfer principle (b16)")

**Figure 2**  MaskGenWithRegTensor transfer principle (b32)<a id="fig-maskgen-b32"></a>  
![](../../../../figures/reg_maskgenwithregtensor_b32.png "MaskGenWithRegTensor transfer principle (b32)")

## Constraints

- When the data type is **b16**, **offset** ∈ [0, 15]; when the data type is **b32**, **offset** ∈ [0, 31].

## Calling Example

```cpp
template <typename T, int16_t offset>
__simd_vf__ inline void MaskGenWithRegTensorVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::LoadAlign(srcReg, srcAddr);
    AscendC::Reg::MaskGenWithRegTensor<T, offset>(mask, srcReg);
    AscendC::Reg::StoreAlign(dstAddr, mask);
}
```
