# asc_set_l0c_copy_params

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:55:37.445Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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
When in-path format conversion (from NZ format to ND format) is performed during DataCopy (CO1->GM, CO1->A1), call this API to set the related configuration for format conversion.

## Prototype

```cpp
__aicore__ inline void asc_set_l0c_copy_params(uint16_t nd_num, uint16_t src_nd_stride, uint16_t dst_nd_stride)
```


## Parameters

Table 1 Parameter description

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| **nd_num**       | Input    | 	Number of ND matrices. Value range: nd_num∈[1, 65535].            |
| **src_nd_stride** | Input    | Source stride in units of fractal size, which is the offset between the start addresses of adjacent source NZ matrices.<br> src_nd_stride∈[1, 512], unit: fractal_size 1024B.|
| **dst_nd_stride** | Input    | 	Offset between the start addresses of adjacent destination ND matrices, in units of elements. <br> dst_nd_stride∈[1, 65535].      |




## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
uint16_t nd_num = 2;
uint16_t src_nd_stride = 2;
uint16_t dst_nd_stride = 1;
asc_set_l0c_copy_params(nd_num, src_nd_stride, dst_nd_stride);
```
