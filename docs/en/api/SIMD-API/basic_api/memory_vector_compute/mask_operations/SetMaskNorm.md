# SetMaskNorm<a name="ZH-CN_TOPIC_0000001836766205"></a>

<!-- md-trans-meta sourceCommit=27a8d829e1498b83498ed8bfc14b3c9e69ae662d translatedAt=2026-09-09T14:44:14.777Z -->

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

Sets the mask mode to Normal mode. This mode is the system default mode and allows developers to configure the number of iterations. This API is recommended to be used together with the isSetMask template parameter in the API. When isSetMask is false, you can call this API to manually manage the Normal mode and set the mask in Normal mode through [SetVectorMask](SetVectorMask.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMaskNorm()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API is recommended to be used together with the isSetMask template parameter. When isSetMask is set to false, you can call this API to manually manage the Normal mode and set the mask in Normal mode through [SetVectorMask](SetVectorMask.md).

<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, Mask is implemented through software simulation. The APIs for computing the first n data of a Tensor do not switch the Mask mode to Normal mode internally. If the current scenario has been set to Counter mode through [SetMaskCount](SetMaskCount.md), the mode remains Counter mode after this API is called. If Normal mode is required for subsequent computation, call this API manually to set it.
<!-- end id10 -->

## Example<a name="section837496171220"></a>

For details, see [Normal Mode Example](SetVectorMask.md#section837496171220).
