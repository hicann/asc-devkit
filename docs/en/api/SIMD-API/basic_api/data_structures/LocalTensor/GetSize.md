# GetSize<a name="ZH-CN_TOPIC_0000002330927374"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:51:42.420Z pushedAt=2026-10-26T07:41:44.308Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/GetSize_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the current **LocalTensor** size.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline uint32_t GetSize() const
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the current **LocalTensor** **Size**, in elements.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

See [Example](LocalTensor_constructor.md#section17531157161314).

```cpp
// Example.
auto size = inputLocal.GetSize(); // Obtain the length of inputLocal. The size is the number of elements in inputLocal.
// The result of Example 5 is as follows:
// The size is srcLen, 256.
```
