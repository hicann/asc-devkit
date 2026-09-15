# layout<a name="ZH-CN_TOPIC_0000002332620129"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:46:22.680Z -->

## Applicable Products<a name="section73648168211"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section36583473819"></a>

Obtains the instantiated **Layout** object.

## Prototype<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section13230182415108"></a>

```cpp
__aicore__ inline constexpr decltype(auto) layout() {}
__aicore__ inline constexpr decltype(auto) layout() const {}
```

## Parameters<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section25791320141317"></a>

Returns the **Layout** instantiated object.

## Constraints<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511"></a>

The **Shape** and **Stride** structures passed in when constructing a **Layout** object must be of the [Std::tuple](../../../../Utils-API/cpp_stdlib/Container Functions/Container Functions.md) structure type and comply with the usage constraints of the **Std::tuple** structure type.

## Example<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

```cpp
AscendC::Shape<int,int,int> shape = AscendC::MakeShape(10, 20, 30);
AscendC::Stride<int,int,int> stride = AscendC::MakeStride(1, 100, 200);

AscendC::Layout<AscendC::Shape<int, int, int>, AscendC::Stride<int, int, int>> layoutInit(shape, stride);

// Obtain the instantiated object using the layout function.
constexpr auto& layout = layoutInit.layout();
```
