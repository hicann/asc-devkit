# GetProgramCounter(ISASI)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T05:41:22.974Z pushedAt=2026-11-18T01:25:31.210Z nl=2 -->

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
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetProgramCounter_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the value of the program counter to track the current execution position of the program.

## Function Prototype

```cpp
__aicore__ inline int64_t GetProgramCounter()
```

## Parameters

None

## Return Value

Returns the current program counter value.

## Constraints

None

## Example

```cpp
int64_t pc = AscendC::GetProgramCounter(); // Obtain the value of the program counter and save it to the variable pc.
```
