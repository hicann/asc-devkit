# asc_set_l0c2gm_nz2nd

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:35:53.759Z -->

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

During data transfer, when performing on-the-fly format conversion (converting the NZ format to the ND format), call this API to set the related format conversion configuration.

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_nz2nd(uint64_t nd_num, uint64_t src_nd_stride, uint64_t dst_nd_stride)
```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| **nd_num** | Input | Number of ND matrices. Value range: [1, 65535]. |
| **src_nd_stride** | Input | Source stride in units of fractal size, that is, the offset between adjacent source NZ matrices. Value range: [0, 65535], in units of 32B. |
| **dst_nd_stride** | Input | Offset between adjacent destination ND matrices. Value range: [1, $2^{32}$ - 1], in units of elements. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
uint64_t nd_num = 2;
uint64_t src_nd_stride = 2;
uint64_t dst_nd_stride = 1;
asc_set_l0c2gm_nz2nd(nd_num, src_nd_stride, dst_nd_stride);
```
