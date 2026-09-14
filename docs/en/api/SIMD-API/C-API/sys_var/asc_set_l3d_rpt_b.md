# asc_set_l3d_rpt_b

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:29:32.088Z pushedAt=2026-09-08T03:45:05.442Z -->

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

Sets the repeat parameter for 2D-format data movement of the [asc_copy_l12l0a](../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_3510.md) and [asc_copy_l12l0b](../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_3510.md) APIs.

## Prototype

```cpp
__aicore__ inline void asc_set_l3d_rpt_b(uint64_t config)
```

## Parameters

Table 1 Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Sets the repeat parameter for 2D-format data movement of the asc_copy_l12l0a and asc_copy_l12l0b APIs. See Table 2 for the bit description. |

Table 2 Bit description of the common repeat control register

|L3D_RPT_B Bit    |Function|
| :-------     | :---- |
| L3D_RPT_B[15:0]    | Indicates the repeat stride.    | 
| L3D_RPT_B[23:16]   | Indicates the number of repeats in the M or K direction, with a default value of 1.      | 
| L3D_RPT_B[24]      | Indicates the repeat mode. <br> - 1'b0: Repeat in the M direction. <br> - 1'b1: Repeat in the K direction.     | 
| L3D_RPT_B[47:32]   | Indicates the stride of the output matrix in the K direction, in units of fractals.     | 
| L3D_RPT_B[63:48]   | Indicates the start position of the output matrix in the M direction, in units of fractals.      | 

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with the [asc_copy_l12l0a](../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_3510.md) and [asc_copy_l12l0b](../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_3510.md) APIs.

## Example

```cpp
uint64_t config = 0;
asc_set_l3d_rpt_b(config);
```
