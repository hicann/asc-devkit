# Mc2FreeCcArgs

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:44:59.863Z pushedAt=2026-11-18T04:57:27.073Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2FreeCcArgs_res.md#id1 -->

## Description

Frees the MC2 communication parameter object created by [Mc2GetCcArgs](Mc2GetCcArgs.md).

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2FreeCcArgs(void* ccArgs)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccArgs | Input | Pointer to the MC2 communication parameter object returned by [Mc2GetCcArgs](Mc2GetCcArgs.md). |

## Return Value

-   0 indicates successful release.
-   A non-zero value indicates release failure, for example, when the parameter is a null pointer.

## Constraints

-   This API only frees ccArgs, not the ccResCtx returned by [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md).
-   ccResCtx is managed by the communicator lifecycle and does not need to be released separately by you.
-   Releasing the same ccArgs repeatedly is not supported.

## Examples

For examples of calling this API, see [HCCL MC2 Usage](HCCL_MC2_usage.md).
