# GetArchVersion

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:21:04.762Z pushedAt=2026-10-26T07:41:44.863Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id7 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id4 -->
- Atlas training products: supported
<!-- end id4 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetArchVersion_res.md#id1 -->


## Description

The header file path is: `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the architecture version number of the current AI processor.

## Prototype

```cpp
__aicore__ inline void GetArchVersion(uint32_t& coreVersion)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| coreVersion | Output | AI processor architecture version. |

## Return Value

None

## Constraints

None

## Example

The following example obtains the architecture version number of the AI processor by calling the `GetArchVersion` API.

```cpp
uint32_t coreVersion = 0;
AscendC::GetArchVersion(coreVersion);
AscendC::printf("core version is %u", coreVersion);
```

Different server models have different architecture version number values, as shown below:
<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: 3510
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: 2201
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: 2201
<!-- end id10 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: 3002
<!-- end id13 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: 2002
<!-- end id12 -->
<!-- npu="910" id11 -->
- Atlas training products: 1001
<!-- end id11 -->
