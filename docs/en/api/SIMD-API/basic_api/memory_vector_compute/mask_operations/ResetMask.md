# ResetMask<a name="ZH-CN_TOPIC_0000001857638449"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T14:40:33.066Z -->

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

Restores the mask value to the default value (all 1s), indicating that all elements in each iteration of vector computation will participate in the operation.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ResetMask()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Values<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

After the SetVectorMask API is called, if you need to restore the mask value to the default value (all 1s), call this API.

## Example<a name="section837496171220"></a>

After setting the mask value using [SetVectorMask](SetVectorMask.md), use ResetMask to restore the mask value to the default value.

```cpp
AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(128);
AscendC::ResetMask();
```

For more examples, see [Setting Mask Outside the API](../SIMD_compute/mask.md).
