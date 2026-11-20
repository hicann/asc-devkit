# Mc2SetCcReduceType

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:48:34.791Z pushedAt=2026-11-18T04:57:38.265Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcReduceType_res.md#id1 -->

## Description

Sets the Reduce operation type for MC2 communication tasks. It takes effect only for reduction-type communication tasks such as AllReduce and ReduceScatter.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcReduceType(void* ccArgs, uint8_t reduceType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccArgs | Input | Pointer to the MC2 communication parameter object returned by [Mc2GetCcArgs](Mc2GetCcArgs.md). |
| reduceType | Input | Reduce operation type, which is an HcclReduceOp enum value. For details, see [Table 2](../HCCL_Kernel/HCCL_usage.md#table2469980529). |

## Return Value

-   0 indicates success.
-   A non-zero value indicates that the setting failed, for example, the parameter is a null pointer or the Reduce operation type is not supported.

## Constraints

-   Only reduction-type communication tasks need to configure this API.
-   If this API is not called, the default Reduce operation type is HCCL_REDUCE_SUM.

## Examples

```cpp
Mc2Result ret = Mc2SetCcReduceType(ccArgs, static_cast<uint8_t>(HCCL_REDUCE_SUM));
```
