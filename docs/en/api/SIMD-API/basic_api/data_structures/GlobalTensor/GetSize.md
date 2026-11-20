# GetSize<a name="ZH-CN_TOPIC_0000002167649065"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:20:40.152Z pushedAt=2026-11-18T01:25:06.888Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/GlobalTensor/GetSize_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the number of elements in a GlobalTensor.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline uint64_t GetSize() const
```

## Parameters<a name="section622mcpsimp"></a>

None.

## Return Value<a name="section640mcpsimp"></a>

Number of elements in the GlobalTensor.

## Constraints<a name="section633mcpsimp"></a>

If the GlobalTensor is initialized by using the [SetGlobalBuffer](SetGlobalBuffer.md) API that passes only the global data pointer, the number of elements obtained by this API is 0.

## Example<a name="section17531157161314"></a>

```cpp
AscendC::LocalTensor<uint8_t> stackBuffer;
AscendC::PopStackBuffer<uint8_t, AscendC::TPosition::LCM>(stackBuffer);
uint32_t stackBufferSize = stackBuffer.GetSize();
```

