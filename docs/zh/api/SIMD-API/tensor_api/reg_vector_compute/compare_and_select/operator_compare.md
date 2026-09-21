# operator_compare

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"tensor_api/experimental/arch/vector/compare_and_select.h"`。

比较运算符对源操作数执行逐元素比较，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。支持reg_tensor与reg_tensor、reg_tensor与标量、标量与reg_tensor三种操作数组合。计算规则如下：

| 运算符 | 计算规则 |
| --- | --- |
| `operator==` | `dst_i = (src0_i == src1_i)` |
| `operator!=` | `dst_i = (src0_i != src1_i)` |
| `operator<` | `dst_i = (src0_i < src1_i)` |
| `operator<=` | `dst_i = (src0_i <= src1_i)` |
| `operator>` | `dst_i = (src0_i > src1_i)` |
| `operator>=` | `dst_i = (src0_i >= src1_i)` |

## 函数原型

<a id="operator-eq"></a>
### operator==

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-ne"></a>
### operator!=

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-lt"></a>
### operator<

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-le"></a>
### operator<=

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-gt"></a>
### operator>

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-ge"></a>
### operator>=

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(
    const T& src0, const reg_tensor<T>& src1)
```

## 参数说明

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 源操作数，类型为`reg_tensor<T>`或标量T。 |
| src1 | 输入 | 源操作数，类型为`reg_tensor<T>`或标量T。 |

## 数据类型

两个源操作数的数据类型必须相同。支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

## 返回值说明

返回类型为`reg_tensor<bool>`。返回值的reg保存逐元素比较结果：对应位置为1表示比较结果成立，为0表示比较结果不成立。当src0和src1都为reg_tensor类型时，返回值的mask和src0的mask值相同，当src0和src1分别为reg_tensor和标量形式时，，返回值的mask和reg_tensor操作数的mask相同。

## 约束说明

- 用于控制计算的mask需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与计算的元素位置错误。
- 本接口应在Vector Function（`__simd_vf__`标记的函数）内调用。
- 输入mask比特位为0时，计算结果对应比特位写0。
- 浮点比较时，+0.0与-0.0视为相等。
- 浮点比较输入含nan时，计算结果对应比特位写0。

## 调用示例

```cpp
#include "tensor_api/experimental/vector_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void compare_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator>=;
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0))
                   .with_mask(asc::te::experimental::all_mask<float>());
    auto condition = src >= 0.0f;
    auto zero = asc::te::experimental::fill(0.0f, asc::te::experimental::all_mask<float>());
    auto dst = asc::te::experimental::select(condition, src, zero);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
