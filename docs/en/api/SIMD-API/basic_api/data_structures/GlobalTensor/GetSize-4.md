# GetSize<a name="ZH-CN_TOPIC_0000002167649065"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:51:07.534Z -->

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

Obtains the number of elements in **GlobalTensor**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline uint64_t GetSize() const
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the number of elements in **GlobalTensor**.

## Constraints<a name="section633mcpsimp"></a>

When a **GlobalTensor** is initialized using the [SetGlobalBuffer](SetGlobalBuffer.md) API that only passes in the global data pointer, the number of elements obtained through this API is 0.

## Example<a name="section17531157161314"></a>

```cpp
AscendC::LocalTensor<uint8_t> stackBuffer;
AscendC::PopStackBuffer<uint8_t, AscendC::TPosition::LCM>(stackBuffer);
uint32_t stackBufferSize = stackBuffer.GetSize();
```

