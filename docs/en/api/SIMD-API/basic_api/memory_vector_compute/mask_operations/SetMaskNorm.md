# SetMaskNorm<a name="ZH-CN_TOPIC_0000001836766205"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:20:34.712Z pushedAt=2026-10-28T03:14:39.546Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskNorm_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

Sets the mask mode to Normal mode. This mode is the system default mode and supports developers in configuring the iteration count. This API is recommended to be used together with the isSetMask template parameter in the API. When isSetMask is false, users can call this API to manually manage the Normal mode and use [SetVectorMask](SetVectorMask.md) to set the mask in Normal mode.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMaskNorm()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API is recommended to be used together with the IsSetMask template parameter in the API. When isSetMask is false, users can call this API to manually manage the Normal mode and use [SetVectorMask](SetVectorMask.md) to set the mask in Normal mode.

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, Mask is implemented by software simulation. The API does not switch the mask mode to Normal mode internally when computing the first n data of a tensor. If the current scenario has been set to Counter mode through [SetMaskCount](SetMaskCount.md), the mode remains Counter mode after this API is called. When subsequent computation requires Normal mode, call this API manually to set it.
<!-- end id10 -->

## Example<a name="section837496171220"></a>

See [Normal mode call example](SetVectorMask.md#section837496171220).
