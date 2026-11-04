# GetSsbufBaseAddr

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:24:17.206Z pushedAt=2026-10-26T07:41:44.868Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr_res.md#id1 -->

## Description

The header file path is: `"basic_api/kernel_operator_sys_var_intf.h"`.

This API is used to obtain the base address of the SSBuffer.

## Function Prototype

```cpp
__aicore__ inline __ssbuf__ void*  GetSsbufBaseAddr()
```

## Parameters

None

## Return Value

Returns a pointer to the base address of **SSBuffer**.

## Constraints

- **SSBuffer** may contain dirty data, and the data is not guaranteed to be all zeros when read.
- **AIC** and **AIV** cannot access **SSBuffer** when they launch different tasks.
- Accessing an address beyond the last one causes an exception. When running in non-MIX mode, each core can independently occupy 1 KB of space (**AIC**, **AIV0**, and **AIV1** each occupy 1 KB); or when running in MIX mode, they share the entire 3 KB of space (**AIC**:**AIV** = 1:2). Currently, the size of **SSBuffer** is 3 KB.
- **SSBuffer** supports only 32-byte aligned access through read/write instructions.

## Example

```cpp
 __ssbuf__ void* ssbuf = GetSsbufBaseAddr(); // Returns (void*) 0 in the NPU domain, and returns the address allocated by CPU simulation in the CPU domain.
AscendC::printf("Ssbuf base address pointer: %p\n", ssbuf); // %p is the format specifier for printing pointers. In the NPU environment, the special 0 pointer value is displayed as nil.
```
