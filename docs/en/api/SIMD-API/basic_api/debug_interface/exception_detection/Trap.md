# Trap<a name="ZH-CN_TOPIC_0000001834672609"></a>

<!-- md-trans-meta sourceCommit=1de5b49f75a2c8f83a238b6ccbb54bf6cf8fb65c translatedAt=2026-08-27T11:29:04.904Z -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Called on the Kernel side, it interrupts AI Core execution in NPU mode and is equivalent to assert in CPU mode. It can be used for debugging abnormal scenarios on the Kernel side.

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
