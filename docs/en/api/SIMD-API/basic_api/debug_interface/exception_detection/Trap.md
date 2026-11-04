# Trap<a name="ZH-CN_TOPIC_0000001834672609"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:12:25.137Z pushedAt=2026-10-26T07:41:44.378Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/exception_detection/Trap_res.md#id1 -->

## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Called on the kernel function (Kernel) side. In NPU mode, it interrupts the execution of the AI Core; in CPU mode, it is equivalent to assert. It can be used to debug exception scenarios on the kernel function (Kernel) side.

## Prototype<a name="section2067518173415"></a>

```cpp
__aicore__ inline void Trap()
```

## Parameters<a name="section158061867342"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

None

## Example<a name="section82241477610"></a>

```cpp
AscendC::LocalTensor<half> src0Local;
AscendC::LocalTensor<half> src1Local;
AscendC::LocalTensor<half> dstLocal;
constexpr int32_t count = 512;    // Number of elements involved in the computation.
if (src1Local[0] == 0) {    // If src1Local[0] is 0, the program terminates.
    AscendC::Trap();
} else {
    AscendC::Divs(dstLocal, src0Local, src1Local[0], count);
}
```
