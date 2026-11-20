# Mc2SetCcAlgConfig

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:46:04.966Z pushedAt=2026-11-18T04:57:30.990Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcAlgConfig_res.md#id1 -->

## Description

Sets the communication algorithm for an MC2 communication task. If this API is not called or an empty string is passed, the API internally selects an algorithm automatically through the algorithm selector.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcAlgConfig(void* ccArgs, const char* algConfig)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccArgs | Input | Pointer to the MC2 communication parameter object returned by [Mc2GetCcArgs](Mc2GetCcArgs.md). |
| algConfig | Input | Communication algorithm configuration, a string ending with '\0' and with a maximum valid length of 127 bytes. For supported configuration formats and values, see the description of the `algConfig` parameter in [SetAlgConfig](../HCCL_Tiling/SetAlgConfig.md). |

## Return Value

-   0 indicates success.
-   A non-zero value indicates that the setting failed, for example, because a parameter is a null pointer or the algorithm configuration length is invalid.

## Constraints

-   The algorithm configuration length must be less than 128 bytes.
-   The communication algorithm set through this API must match the specified communication task type. For details, see the description of the `algConfig` parameter in [SetAlgConfig](../HCCL_Tiling/SetAlgConfig.md).

## Examples

```cpp
char algConfig[] = "sole[mesh]";
Mc2Result ret = Mc2SetCcAlgConfig(ccArgs, algConfig);
```
