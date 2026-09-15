# GetShapeInfo<a name="ZH-CN_TOPIC_0000002330927394"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:03:32.621Z -->

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
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Obtains the Shape information of **LocalTensor**. Note: Shape information has no default value. Only after the Shape information has been set through [SetShapeInfo](SetShapeInfo.md) can this API be called to obtain the correct Shape information.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline ShapeInfo GetShapeInfo() const
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the Shape information of **LocalTensor**, of the [ShapeInfo](../../aux_data_structures/ShapeInfo.md) structure type.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// Obtain the ShapeInfo information of the Tensor.
AscendC::ShapeInfo maxShapeInfo = maxUb.GetShapeInfo();
uint32_t orgShape0 = maxShapeInfo.originalShape[0];
uint32_t orgShape1 = maxShapeInfo.originalShape[1];
uint32_t orgShape2 = maxShapeInfo.originalShape[2];
uint32_t orgShape3 = maxShapeInfo.originalShape[3];
uint32_t shape2 = maxShapeInfo.shape[2];
```
