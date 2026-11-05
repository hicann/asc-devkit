# SetHcclContext

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:10:41.118Z pushedAt=2026-10-27T11:49:41.764Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL-Context/SetHcclContext_res.md#id1 -->

## Description

Sets the context (message area) address corresponding to each communicator of the compute-communication fusion operator.

## Prototype

```
template <uint32_t index>
__aicore__ inline void SetHcclContext(__gm__ uint8_t* context)
```

## Parameters

**Table 1**  Parameters

| Parameter | Description |
| --- | --- |
| index | Template parameter, indicating the communicator ID to be set. Currently only two communicators are supported, so index can only be 0/1. |
| context | Context (message area) address of the corresponding communicator. |

## Return Value

None

## Constraints

Currently, a maximum of two communicators are supported.

## Examples

```
// Set the message area address for GROUP_0.
AscendC::SetHcclContext<0>(contextGM);
```
