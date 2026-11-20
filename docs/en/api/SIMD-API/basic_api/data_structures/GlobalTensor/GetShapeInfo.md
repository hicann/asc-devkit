# GetShapeInfo<a name="ZH-CN_TOPIC_0000002132091188"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:18:57.169Z pushedAt=2026-11-18T01:25:06.887Z nl=2 -->

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
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/GlobalTensor/GetShapeInfo_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the shape information of GlobalTensor. Note: Shape information has no default value. Only after Shape information has been set through SetShapeInfo can this API be called to obtain the correct ShapeInfo.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline ShapeInfo GetShapeInfo() const
```

## Parameters<a name="section622mcpsimp"></a>

None.

## Return Value<a name="section640mcpsimp"></a>

Returns the shape information of GlobalTensor, of the [ShapeInfo](../../aux_data_structures//ShapeInfo.md) type.

## Constraints<a name="section633mcpsimp"></a>

None.

## Examples<a name="section025788373"></a>

```cpp
// Obtain the ShapeInfo of the tensor, which is set through SetShapeInfo.
AscendC::ShapeInfo maxShapeInfo = tglobal.GetShapeInfo();
// Obtain the value of each dimension of the original shape.
uint32_t orgShape0 = maxShapeInfo.originalShape[0];
uint32_t orgShape1 = maxShapeInfo.originalShape[1];
uint32_t orgShape2 = maxShapeInfo.originalShape[2];
uint32_t orgShape3 = maxShapeInfo.originalShape[3];
// Obtain the value of the second dimension of the current shape.
uint32_t shape2 = maxShapeInfo.shape[2];
```
