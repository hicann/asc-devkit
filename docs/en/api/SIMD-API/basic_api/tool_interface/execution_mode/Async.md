# Async

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T05:29:44.640Z pushedAt=2026-11-18T01:25:31.184Z nl=2 -->

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/execution_mode/Async_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_utils_intf.h"`.

When developing a fusion operator based on [separate mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md) (where AIC and AIV are separated), the operator logic usually contains both AIV and AIC processing logic. In this case, you need to use the macros `ASCEND_IS_AIV`/`ASCEND_IS_AIC` provided by Ascend C to implement the following hardware conditional branch to isolate the AIV and AIC code:

```cpp
if ASCEND_IS_AIV {
// AIV processing logic.
}
if ASCEND_IS_AIC {
// AIC processing logic.
}
```

**Async** encapsulates this isolation mode through a template function and provides a unified API for executing a specific function on different execution units (AIC or AIV), thereby avoiding the use of hardware conditional branches in the code.

## Function Prototype

```cpp
template <EngineType engine, auto funPtr, class... Args>
__aicore__ void Async(Args... args)
```

## Parameters

**Table 1** Template parameters

| Parameter        | Description                                                                |
| ------------- | ------------------------------------------------------------------- |
| engine        | **EngineType** enumeration type. The following values are supported, corresponding to different hardware execution units:<br>&bull; **AIC** <br>&bull; **AIV** |
| funPtr        | Function pointer that specifies the function to be executed. The function signature and parameter types are determined by **class... Args**. |
| class... Args | Variadic template that represents the type list of the function parameters, used to pass to **funPtr**.            |

**Table 2** Parameters

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
