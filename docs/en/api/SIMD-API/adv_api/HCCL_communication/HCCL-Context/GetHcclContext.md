# GetHcclContext

<!-- md-trans-meta sourceCommit=a7bd096e48cfa10647b950eeeaea038734e07be8 translatedAt=2026-11-02T05:59:35.562Z pushedAt=2026-11-18T04:58:27.321Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL-Context/GetHcclContext_res.md#id1 -->

## Description

Obtains the context (message area) address of the communicator specified by **index**.

## Prototype

```
template <uint32_t index>
__aicore__ inline __gm__ uint8_t* __gm__ GetHcclContext(void)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| index | Template parameter, indicating the communicator ID to be obtained. Currently only two communicators are supported, so index can only be 0/1. |

## Return Value

Address of the **context** (message area) of the specified communicator.

## Constraints

Currently, a maximum of two communicators are supported.

## Examples

```
GM_ADDR contextGM = AscendC::GetHcclContext<0>();
```
