# GetArchVersion

<!-- md-trans-meta sourceCommit=53d6dd141dd5063557396f6918eca149e02f93ce translatedAt=2026-08-27T12:03:10.960Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

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

The architecture version number is hexadecimal-encoded. It is recommended to output and view it in hexadecimal format (such as `%x`), or convert it to a hexadecimal number yourself.

## Example

The following sample obtains the architecture version number of the AI processor by calling the `GetArchVersion` API.

```cpp
uint32_t coreVersion = 0;
AscendC::GetArchVersion(coreVersion);
AscendC::printf("core version is %x", coreVersion); //Print it as a hexadecimal number using %x.
```
