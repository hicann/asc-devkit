# GetLength<a name="ZH-CN_TOPIC_0000002364805853"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:28:03.894Z pushedAt=2026-11-18T01:25:06.906Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/GetLength_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the data length of a **LocalTensor**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline uint32_t GetLength() const
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

The length of the **LocalTensor** data, in bytes.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// Obtain the length of localTensor (in bytes). The data type is int32_t, so the length is 16*sizeof(int32_t).
uint32_t len = inputLocal.GetLength();
// inputLocal:0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
// len: 64
```

