# Init<a name="ZH-CN_TOPIC_0000001717287036"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:42:20.885Z pushedAt=2026-10-26T07:41:44.679Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/Init_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Initializes the memory and the synchronization pipeline event EventID.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void Init()
```

## Constraints<a name="section633mcpsimp"></a>

-   When a Tpipe object is reused, it must be used in pairs with the [Destroy](Destroy.md) API. Each reuse requires calling **Destroy** first to release resources, and then calling **init** to reinitialize.
-   The Tpipe constructor already calls **Init** internally, so there is no need to call it again after the Tpipe object is initialized.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>


```cpp
// Instantiate a custom operator object of the floating-point type.
KernelTPipeInit<float> op;
uint32_t srcSize = 128;

// Construct a Tpipe object.
AscendC::TPipe tpipe;

// First execution flow.
op.Init(x, z, srcSize, &tpipe);
op.Process();
tpipe.Destroy();  // To reuse the tpipe, call Destroy before Init.

// Second execution flow: reuse the operator object and the tpipe object.
tpipe.Init();
op.Init(x, z, srcSize, &tpipe);
op.Process();
tpipe.Destroy();  // Destroy the tpipe resources.
```
