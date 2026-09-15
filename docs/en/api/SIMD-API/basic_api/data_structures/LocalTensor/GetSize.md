# GetSize<a name="ZH-CN_TOPIC_0000002330927374"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:04:42.226Z -->

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

Obtains the size of the current **LocalTensor**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline uint32_t GetSize() const
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the current **LocalTensor** size, in elements.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

See [Example](localtensor_constructor.md#section17531157161314).

```cpp
// Example.
auto size = inputLocal.GetSize(); // Obtain the length of inputLocal. The size is the number of elements in inputLocal.
// The result of Example 5 is as follows:
// The size is srcLen, 256.
```

