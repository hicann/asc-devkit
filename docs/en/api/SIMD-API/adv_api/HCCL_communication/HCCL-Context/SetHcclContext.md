# SetHcclContext

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T06:01:26.865Z pushedAt=2026-11-18T04:58:36.046Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
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
