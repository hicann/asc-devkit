# ResetMask<a name="ZH-CN_TOPIC_0000001857638449"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:17:23.677Z pushedAt=2026-10-28T03:14:39.542Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/mask_operations/ResetMask_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

Restores the mask value to the default value (all 1s), indicating that all elements in each iteration of the vector computation will participate in the operation.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ResetMask()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

After calling the SetVectorMask API, call this API to restore the mask value to the default value (all 1s).

## Example<a name="section837496171220"></a>

After setting the mask value with [SetVectorMask](SetVectorMask.md) and finishing using it, use ResetMask to restore the mask value to the default value.

```cpp
AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(128);
AscendC::ResetMask();
```

For more examples, see [Setting Mask Outside the API](../SIMD_compute/mask.md#setting-the-mask-outside-the-api).
