# GetSsbufBaseAddr

<!-- md-trans-meta sourceCommit=53d6dd141dd5063557396f6918eca149e02f93ce translatedAt=2026-08-27T12:07:22.958Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

This API is used to obtain the base address of **SSBuffer**.

## Function Prototype

```cpp
__aicore__ inline __ssbuf__ void*  GetSsbufBaseAddr()
```

## Parameters

None

## Return Value

Returns a pointer to the base address of the **SSBuffer**.

## Constraints

- Dirty data may exist in **SSBuffer**, and the data is not guaranteed to be all zeros when read.
- **AIC** and **AIV** cannot access **SSBuffer** when they launch different tasks.
- Accessing an address beyond the end may cause an exception. When running in non-MIX mode, each core can independently occupy 1 KB of space (**AIC**, **AIV0**, and **AIV1** each occupy 1 KB); or when running in MIX mode, they share the entire 3 KB of space (**AIC**:**AIV** = 1:2). Currently, the size of **SSBuffer** is 3 KB.
- **SSBuffer** supports only 32-byte aligned access through read/write instructions.

## Example

```cpp
 __ssbuf__ void* ssbuf = GetSsbufBaseAddr(); // Returns (void*) 0 in the NPU domain, and returns the address allocated by CPU simulation in the CPU domain.
AscendC::printf("Ssbuf base address pointer: %p\n", ssbuf); // %p is the format specifier for printing a pointer. In the NPU environment, the special 0 pointer value is displayed as nil.
```
