# Mc2SetCcSrcDataType

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:48:56.786Z pushedAt=2026-11-18T04:57:40.102Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcSrcDataType_res.md#id1 -->

## Description

Sets the data type of the input data for an MC2 communication task.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcSrcDataType(void* ccArgs, uint8_t srcDataType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccArgs | Input | Pointer to the MC2 communication parameter object returned by [Mc2GetCcArgs](Mc2GetCcArgs.md). |
| srcDataType | Input | Input data type, which is an HcclDataType enumeration value. For details, see [Table 1](../HCCL_Kernel/HCCL_usage.md#table116710585514). |

## Return Value

-   0 indicates success.
-   A non-zero value indicates that the setting failed, for example, when the parameter is a null pointer or the data type is not supported.

## Constraints

-   Different communication tasks support different input data types. For specific constraints, see the documentation of the corresponding communication task API.
-   If this API is not called, the default input data type is HCCL_DATA_TYPE_FP16.

## Examples

```cpp
Mc2Result ret = Mc2SetCcSrcDataType(ccArgs, HCCL_DATA_TYPE_FP32);
```
