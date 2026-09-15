# GetShape<a name="ZH-CN_TOPIC_0000002297494516"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:43:35.928Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
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


## Description<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section36583473819"></a>

Returns a **Shape** object that describes the shape of the tensor.

## Prototype<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section13230182415108"></a>

```cpp
__aicore__ inline constexpr decltype(auto) GetShape() {}   
__aicore__ inline constexpr decltype(auto) GetShape() const {}
```

## Parameters<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section25791320141317"></a>

Returns a **Shape** object that describes the tensor shape. The **Shape** structure type (an alias of the [Std::tuple](../../../../Utils-API/cpp_stdlib/Container Functions/Container Functions.md) type) is defined as follows:

```cpp
template <typename... Shapes>
using Shape = Std::tuple<Shapes...>;
```

## Constraints<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section320753512363"></a>

```cpp
// Initialize the Layout data structure and obtain the corresponding values.
AscendC::Shape<int,int,int> shape = AscendC::MakeShape(10, 20, 30);
AscendC::Stride<int,int,int> stride = AscendC::MakeStride(1, 100, 200);

auto layoutMake = AscendC::MakeLayout(shape, stride);

int value = AscendC::Std::get<0>(layoutMake.GetShape()); // value = 10
value = AscendC::Std::get<1>(layoutMake.GetShape()); // value = 20
value = AscendC::Std::get<2>(layoutMake.GetShape()); // value = 30
```
