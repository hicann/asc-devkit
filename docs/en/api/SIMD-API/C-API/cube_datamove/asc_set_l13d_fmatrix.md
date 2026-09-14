# asc_set_l13d_fmatrix

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-27T12:11:55.065Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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

Sets the Feature map attribute description, which is used to configure the fill value when calling the 3D format copy APIs [asc_copy_l12l0a](asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b.md). This API is used to set the Feature map attribute description only when the **f_matrix_ctrl** parameter of the **asc_copy_l12l0a**/**asc_copy_l12l0b** API indicates that the FeatureMap attributes are obtained from the left matrix; otherwise, use the **asc_set_l13d_fmatrix_b** API.

## Prototype

```cpp
__aicore__ inline void asc_set_l13d_fmatrix(asc_l13d_fmatrix_config& config)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Used to set the Feature map attribute parameters of the 3D format copy APIs **asc_copy_l12l0a**/**asc_copy_l12l0b**. For details, see [asc_l13d_fmatrix_config](../struct/asc_l13d_fmatrix_config.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
asc_l13d_fmatrix_config config;
asc_set_l13d_fmatrix(config);
```
