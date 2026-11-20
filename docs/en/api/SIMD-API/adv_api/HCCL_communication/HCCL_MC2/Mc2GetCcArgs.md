# Mc2GetCcArgs

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:44:56.366Z pushedAt=2026-11-18T04:57:28.910Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2GetCcArgs_res.md#id1 -->

## Description

Creates an MC2 communication parameter object and returns a pointer to it. The object is used to subsequently set the communication engine, data type, Reduce operation type, and communication algorithm.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2GetCcArgs(void** ccArgs)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccArgs | Output | Pointer to the MC2 communication parameter object. The object is released by [Mc2FreeCcArgs](Mc2FreeCcArgs.md). |

## Return Value

-   0 indicates that the object is created successfully.
-   A non-zero value indicates a creation failure, for example, a null pointer parameter or memory allocation failure.

## Constraints

-   This API applies to both the AI CPU path and the CCU path.
-   After creation, the default communication engine is AICPU_TS, the default input/output data type is HCCL_DATA_TYPE_FP16, the default Reduce operation type is HCCL_REDUCE_SUM, and the default algorithm name is empty.
-   When using the CCU path, you must call [Mc2SetCcCommEngine](Mc2SetCcCommEngine.md) to set the CCU communication engine.
-   After ccArgs is created, it must be released by calling [Mc2FreeCcArgs](Mc2FreeCcArgs.md).
-   Releasing the same ccArgs repeatedly is not supported.

## Examples

For a usage example of this API, see [HCCL MC2 usage](HCCL_MC2_usage.md).
