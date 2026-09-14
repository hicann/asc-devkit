# asc_get_arch_ver

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:06:48.703Z pushedAt=2026-09-08T03:45:05.413Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Obtains the current AI processor architecture version number.

## Prototype

```cpp
__aicore__ inline void asc_get_arch_ver(uint32_t& core_version)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| core_version | Output | AI processor architecture version, with the data type uint32_t. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Before calling the **asc_get_arch_ver** API, define **core_version** first. After the **asc_get_arch_ver** API is called, **core_version** is set to the value of the corresponding architecture version number.
Due to hardware constraints, when viewing the converted AI processor architecture version number, you need to print it as a hexadecimal number or convert it to a hexadecimal number.

## Example

```cpp
uint32_t core_version = 0;// Define the AI processor version.
asc_get_arch_ver(core_version);
printf("core version is %x", core_version);// Print it as a hexadecimal number using %x.
```