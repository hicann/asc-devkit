# asc_set_ndim_pad_count

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:52:42.946Z pushedAt=2026-09-08T13:17:42.534Z -->

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

Sets the number of padding elements on the left and right sides of each dimension for the [asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md) API.

## Prototype

```c++
__aicore__ inline void asc_set_ndim_pad_count(asc_ndim_pad_count_config& config)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Sets the number of padding elements on the left and right sides of each dimension for the asc_ndim_copy_gm2ub API. For details, see [asc_ndim_pad_count_config.md](../struct/asc_ndim_pad_count_config.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Must be used together with [asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md).

## Example

```c++
asc_ndim_pad_count_config config;  // The default initial value of config is 0.
asc_set_ndim_pad_count(config); 
```