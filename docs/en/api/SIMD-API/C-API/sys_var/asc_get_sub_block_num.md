# asc_get_sub_block_num

<!-- md-trans-meta sourceCommit=dda4c6b0ddd773ea533849b7c2bd57f8cdeb9cda translatedAt=2026-09-05T13:16:12.811Z pushedAt=2026-09-08T03:45:05.430Z -->

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

In separate mode, obtains the number of Cube Cores (AIC) or Vector Cores (AIV) on an AI Core.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_sub_block_num()
```

## Parameters

None

## Return Value


For different kernel types, the return values of this API when called on AIC and AIV are as follows:

|Kernel Type|KERNEL_TYPE_AIV_ONLY|KERNEL_TYPE_AIC_ONLY|KERNEL_TYPE_MIX_AIC_1_2|KERNEL_TYPE_MIX_AIC_1_1|KERNEL_TYPE_MIX_AIC_1_0|KERNEL_TYPE_MIX_AIV_1_0|
| :------ | :------------------ | :----------------- | :-------------------- | :--------------------- | :-------------------- | :-------------------- |
|AIV      |1                    |-                   |2                      |1                       |-                      |1                      |
|AIC      |-                    |1                   |1                      |1                       |1                      |-                      |

## Pipeline Type

PIPE_S

## Constraints

None

## Example

None
