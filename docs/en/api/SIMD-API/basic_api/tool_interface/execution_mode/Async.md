# Async

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T12:13:57.053Z -->

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
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_utils_intf.h"`.

When developing a fusion operator based on [separate mode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md) (with AIC and AIV separated), the operator logic usually contains both AIV and AIC processing logic. In this case, you need to use the macros `ASCEND_IS_AIV`/`ASCEND_IS_AIC` provided by Ascend C to implement the following hardware conditional branches to isolate the AIV and AIC code:

```cpp
if ASCEND_IS_AIV {
// AIV processing logic.
}
if ASCEND_IS_AIC {
// AIC processing logic.
}
```

**Async** encapsulates this isolation pattern through template functions and provides a unified API for executing a specific function on different execution units (AIC or AIV), thereby avoiding the use of hardware conditional branches in the code.

## Prototype

```cpp
template <EngineType engine, auto funPtr, class... Args>
__aicore__ void Async(Args... args)
```

## Parameters

**Table 1** Template parameter description

| Parameter     | Description                                                                |
| ------------- | ------------------------------------------------------------------- |
| engine        | **EngineType** enumeration, which can take the following values corresponding to different hardware execution units:<br>&bull; **AIC** <br>&bull; **AIV** |
| funPtr        | Function pointer that specifies the function to be executed. The function signature and parameter types are determined by **class... Args**. |
| class... Args | Variadic template representing the list of types of the function parameters, used to pass to **funPtr**.            |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| Args... args | Input | Parameter list corresponding to **class... Args**, representing the actual arguments passed to **funPtr**. |

## Return Value

None

## Constraints

None

## Example

```cpp
__aicore__ inline void cubeProcess(KernelMmad &op, GM_ADDR A, GM_ADDR B, GM_ADDR c)
{
    op.InitAIC(A, B, c);
    op.ProcessAIC();
}
__aicore__ inline void vectorProcess(KernelMmad &op, GM_ADDR a, GM_ADDR b, GM_ADDR A, GM_ADDR B, GM_ADDR c)
{
    op.InitAIV(a, b, A, B, c);
    op.ProcessAIV();
}
__global__ __mix__(1,2) void mmad_custom(GM_ADDR a, GM_ADDR b, GM_ADDR A, GM_ADDR B, GM_ADDR c)
{
    AscendC::InitSocState();
    KernelMmad op;
    AscendC::Async<AscendC::EngineType::AIC, cubeProcess>(op, A, B, c);
    AscendC::Async<AscendC::EngineType::AIV, vectorProcess>(op, a, b, A, B, c);
}
```
