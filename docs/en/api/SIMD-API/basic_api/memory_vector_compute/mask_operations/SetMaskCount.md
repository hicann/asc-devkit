# SetMaskCount<a name="ZH-CN_TOPIC_0000001790047006"></a>

<!-- md-trans-meta sourceCommit=27a8d829e1498b83498ed8bfc14b3c9e69ae662d translatedAt=2026-09-09T14:42:22.998Z -->

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
- Atlas 200I/500 A2 inference products: Supported
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


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

Sets the Mask mode to Counter mode. In this mode, developers do not need to perceive the number of iterations or process unaligned tail blocks. Instead, they can directly pass in the amount of data to be computed, and the actual number of iterations is automatically inferred by the Vector computing unit. This API is recommended to be used together with the isSetMask template parameter in the API. When isSetMask is false, users can call this API to manually manage the Counter mode and use [SetVectorMask](SetVectorMask.md) to set the number of elements involved in the computation in Counter mode.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMaskCount()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API is recommended to be used together with the isSetMask template parameter in the API. When isSetMask is false, you can call this API to manually manage the Counter mode and use [SetVectorMask](SetVectorMask.md) to set the number of elements involved in the computation in Counter mode.

- In scenarios where the Counter mode is set, call [SetMaskNorm](SetMaskNorm.md) to restore the Mask mode to Normal mode after the vector computation is complete, so as to avoid affecting subsequent computations.

<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, Mask is implemented by software simulation. The API for computing the first n data of a Tensor does not switch the Mask mode to Normal mode internally. If the current scenario has been set to Counter mode through this API, it remains in Counter mode after the API is called. When Normal mode is required for subsequent computations, manually call [SetMaskNorm](SetMaskNorm.md) to set it.
<!-- end id10 -->

## Example<a name="section837496171220"></a>

For details, see [Counter mode call example](SetVectorMask.md#section837496171220).
