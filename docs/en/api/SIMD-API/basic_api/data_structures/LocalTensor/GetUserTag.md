# GetUserTag<a name="ZH-CN_TOPIC_0000002364845757"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:05:09.081Z -->

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

Obtains the **Tag** information of a specified **Tensor** block. Users can perform different operations on the **Tensor** based on the **Tag** information.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline TTagType GetUserTag() const
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the Tag information of the specified Tensor block. **TTagType** is defined as follows:

```cpp
using TTagType = int32_t
```

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// Example 8
AscendC::LocalTensor<half> tensor1 = que1.DeQue<half>();
AscendC::TTagType tag1 = tensor1.GetUserTag();
AscendC::LocalTensor<half> tensor2 = que2.DeQue<half>();
AscendC::TTagType tag2 = tensor2.GetUserTag();
AscendC::LocalTensor<half> tensor3 = que3.AllocTensor<half>();
/*Use Tag to control conditional statement execution.*/
if ((tag1 <= 10) && (tag2 >= 9)) {
    AscendC::Add(tensor3, tensor1, tensor2, TILE_LENGTH); // The addition operation can be performed only when tag1 is less than or equal to 10 and tag2 is greater than or equal to 9.
}
```

