# Mc2SetCcCommEngine

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:47:16.896Z pushedAt=2026-11-18T04:57:33.438Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcCommEngine_res.md#id1 -->

## Description

Sets the communication engine used by MC2 communication tasks and automatically sets the startup protocol version of the communication server corresponding to that engine, so you do not need to configure the version separately.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcCommEngine(void* ccArgs, uint8_t commEngine)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccArgs | Input | Pointer to the MC2 communication parameter object returned by [Mc2GetCcArgs](Mc2GetCcArgs.md). |
| commEngine | Input | Communication engine. 2 (AICPU_TS) indicates using the AI CPU communication server; 5 (CCU_MS) indicates using the CCU MS mode communication server; 6 (CCU_SCHED) indicates using the CCU scheduling mode communication server, which is recommended for CCU scenarios. Currently, AI CPU supports only the AICPU_TS communication engine. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates that the setting failed, for example, the parameter is a null pointer or the communication engine is not supported.

## Constraints

-   Currently, only the three communication engine values above are supported. Other values return an error.

## Examples

CCU engine example:

```cpp
constexpr uint8_t kCcuSchedEngine = 6U; // CCU_SCHED
Mc2Result ret = Mc2SetCcCommEngine(ccArgs, kCcuSchedEngine);
```

AI CPU engine example:

```cpp
constexpr uint8_t kAicpuTsEngine = 2U; // AICPU_TS
Mc2Result ret = Mc2SetCcCommEngine(ccArgs, kAicpuTsEngine);
```
