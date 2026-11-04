# SetMaskCount<a name="ZH-CN_TOPIC_0000001790047006"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:18:15.952Z pushedAt=2026-10-28T03:14:39.545Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskCount_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

Sets the Mask mode to Counter mode. In this mode, you do not need to be aware of the iteration count or handle unaligned tail blocks. You can directly pass the amount of data to be computed, and the actual iteration count is automatically inferred by the Vector compute unit. This API is recommended to be used together with the isSetMask template parameter in the API. When isSetMask is false, you can call this API to manually manage the Counter mode and use [SetVectorMask](SetVectorMask.md) to set the number of elements involved in the computation in Counter mode.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMaskCount()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API is recommended to be used together with the IsSetMask template parameter in the API. When IsSetMask is false, you can call this API to manually manage the Counter mode and use [SetVectorMask](SetVectorMask.md) to set the number of elements involved in the computation in Counter mode.

- In scenarios where the Counter mode is set, call [SetMaskNorm](SetMaskNorm.md) to restore the Mask mode to Normal mode after the vector computation is complete, to avoid affecting subsequent computations.

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, Mask is implemented through software emulation. The API for computing the first n data of a tensor does not switch the Mask mode to Normal mode internally. If the current scenario has been set to Counter mode through this API, the mode remains Counter mode after the API is called. When Normal mode is required for subsequent computations, call [SetMaskNorm](SetMaskNorm.md) manually to set it.
<!-- end id10 -->

## Example<a name="section837496171220"></a>

See [Counter mode example](SetVectorMask.md#section837496171220).
