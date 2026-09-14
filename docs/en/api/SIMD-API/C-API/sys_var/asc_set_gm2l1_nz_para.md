# asc_set_gm2l1_nz_para

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:24:02.098Z pushedAt=2026-09-08T03:45:05.437Z -->

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

Sets the value in the **MTE2_NZ_PARA** register. You can set the related parameters before calling [asc_copy_gm2l1_dn2nz](../cube_datamove/asc_copy_gm2l1_dn2nz.md) and [asc_copy_gm2l1_nd2nz](../cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md).

**MTE2_NZ_PARA** is a 64-bit register. The meaning of each bit is as follows:

| Bit Range |        Meaning         |
|:------|:-----------------:|
| 15:0  |  Number of ND/DN matrices to be moved.  | 
| 31:16 | Interval between two adjacent blocks in the N dimension. |
| 47:32 | Interval between two C0 columns in the same NZ matrix. |
| 63:48 | Interval between two NZ matrices in two ND/DN matrices. |

## Prototype

```cpp
__aicore__ inline void asc_set_gm2l1_nz_para(uint64_t config)
```

## Parameters

| Parameter | Input/Output | Description        |
| :------ | :---  |:----------|
| config | Input | Register value to be set. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example
```cpp
uint64_t nz_dst_stride = 16;
uint64_t config = nz_dst_stride << 48; //[63:48]
uint64_t c0_stride = 32;
config |= c0_stride << 32; //[47:32]
uint64_t n_dst_stride = 32;
config |= n_dst_stride << 16; //[31:16]
uint64_t nd_num = 64;
config |= nd_num; //[15:0]

asc_set_gm2l1_nz_para(config);
```