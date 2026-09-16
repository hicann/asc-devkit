# GetStride<a name="ZH-CN_TOPIC_0000002331573721"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:44:07.711Z -->

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


## Description<a name="zh-cn_topic_0000002042526794_zh-cn_topic_0000001602767550_zh-cn_topic_0000001600123102_section36583473819"></a>

Returns a **Stride** object that describes the memory access stride, corresponding one-to-one to the dimension information of **Shape**.

## Prototype<a name="zh-cn_topic_0000002042526794_zh-cn_topic_0000001602767550_zh-cn_topic_0000001600123102_section13230182415108"></a>

```cpp
__aicore__ inline constexpr decltype(auto) GetStride() {}    
__aicore__ inline constexpr decltype(auto) GetStride() const {}
```

## Parameters<a name="zh-cn_topic_0000002042526794_zh-cn_topic_0000001602767550_zh-cn_topic_0000001600123102_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000002042526794_zh-cn_topic_0000001602767550_zh-cn_topic_0000001600123102_section25791320141317"></a>

A **Stride** object that describes the memory access stride. It is a **Stride** structure type (an alias of the [Std::tuple](../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md) type), defined as follows:

```cpp
template <typename... Strides>
using Stride = Std::tuple<Strides...>;
```

## Constraints<a name="zh-cn_topic_0000002042526794_zh-cn_topic_0000001602767550_zh-cn_topic_0000001600123102_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002042526794_zh-cn_topic_0000001602767550_zh-cn_topic_0000001600123102_section320753512363"></a>

```cpp
// Initialize the Layout data structure and obtain the corresponding values.
AscendC::Shape<int,int,int> shape = AscendC::MakeShape(10, 20, 30);
AscendC::Stride<int,int,int> stride = AscendC::MakeStride(1, 100, 200);

AscendC::Layout<AscendC::Shape<int, int, int>, AscendC::Stride<int, int, int>> layoutInit(shape, stride);

int value = AscendC::Std::get<0>(layoutInit.GetStride()); // value = 1
value = AscendC::Std::get<1>(layoutInit.GetStride()); // value = 100
value = AscendC::Std::get<2>(layoutInit.GetStride()); // value = 200
```
