# histograms

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径：`"tensor_api/experimental/arch/vector/histogram_compute.h"`。

该接口根据`src.mask`统计`uint8_t`输入值的频率分布或累计分布，并在`dst`原值上累加统计结果。计算公式如下：

对于返回值中的第$i$个元素（$i\in[0,127]$），两种统计模式分别为：

频数模式：

$$
\left[\operatorname{histograms}(dst, src)\right]_i =
\begin{cases}
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j=i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::low} \\
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j=128+i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::high}
\end{cases}
$$

累计模式：

$$
\left[\operatorname{histograms}(dst, src)\right]_i =
\begin{cases}
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j\le i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::low} \\
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j\le 128+i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::high}
\end{cases}
$$

其中，$(\mathrm{src.mask})_j$为`1`时，`src`的第$j$个元素参与统计；为`0`时不参与统计。`high`与`cumulative`组合使用时，第$i$个输出统计所有不大于$128+i$的输入值，因此包含`[0, 127]`范围内的输入值。

**表1**配置项说明

| 配置项 | 取值 | 含义 |
| --- | --- | --- |
| `options.half` | `histogram_half::low` | 统计输入值`[0, 127]`。 |
| `options.half` | `histogram_half::high` | 统计输入值`[128, 255]`。 |
| `options.mode` | `histogram_mode::frequency` | 每个输出元素表示对应输入值出现的次数。 |
| `options.mode` | `histogram_mode::cumulative` | 每个输出元素表示当前值及之前各值的累计次数。 |

## 函数原型

```cpp
template <const histogram_options& options = default_histogram_options, typename T, typename U>
__simd_callee__ inline reg_tensor<T> histograms(const reg_tensor<T>& dst, const reg_tensor<U>& src)
```

## 参数说明

**表2**模板参数说明

| 参数名 | 描述 |
| --- | --- |
| options | 直方图配置，类型为histogram_options。默认统计[0, 127]范围内各数值出现的次数。 |
| T | 输出元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 输入元素类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表3**参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入 | 统计初值，类型为`reg_tensor<T>`。接口不修改`dst`，返回值的`mask`与`dst.mask`相同。 |
| src | 输入 | 待统计数据，类型为`reg_tensor<U>`。`src.mask`用于标记参与统计的元素，对应位置为`1`时参与统计，为`0`时不参与统计。 |

## 返回值说明

返回新的`reg_tensor<T>`。频率模式在`dst`原值上累加每个输入值的出现次数，累计模式在`dst`原值上累加不大于对应值的输入数量。返回值的`mask`与`dst.mask`相同。

## 数据类型

**表4**数据类型组合

| src | dst |
| --- | --- |
| uint8_t | uint16_t |

## 约束说明

- `src.mask`需通过`with_mask`接口预先设置。未设置时，mask的内容不确定，会导致参与统计的元素位置错误。
- `src.mask`只用于筛选参与统计的输入元素，不用于筛选返回值中的统计位置。返回值中的所有位置都会在`dst`原值的基础上更新。
- 统计结果在`dst`原有数据的基础上累加。调用前必须初始化`dst.reg`；多次调用时，需要将前一次返回值作为后一次调用的`dst`输入。返回值的`mask`继承`dst.mask`，将返回值用于`store`前需要通过`with_mask`设置`dst.mask`。
- `dst`的元素类型为`uint16_t`，单个统计结果的最大值为65535。用户需要保证累加结果不超过该范围，否则会发生溢出。
- 使用非默认配置时，需要在命名空间作用域定义具名的`constexpr histogram_options`对象，并将该对象作为模板实参传入。

## 调用示例

以下示例统计输入数据高半区的累计分布，并在初值`3`上累加统计结果。

```cpp
#include "c_api/asc_simd.h"
#include "tensor_api/experimental/vector_compute.h"

constexpr uint16_t initial_value = 3;
constexpr asc::te::experimental::histogram_options histogram_config {
    asc::te::experimental::histogram_half::high,
    asc::te::experimental::histogram_mode::cumulative};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void histograms_vf(
    const SrcTensor src, DstTensor dst, uint16_t repeat_times, uint32_t total, uint32_t one_repeat_size)
{
    uint32_t remain = total;
    asc::te::experimental::reg_tensor<uint16_t> dst_reg;
    auto dst_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::all, uint16_t>();
    dst_reg.mask = dst_mask.reg;
    asc_duplicate_scalar(dst_reg.reg, initial_value, dst_reg.mask);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<uint8_t>(remain);
        auto src_reg = asc::te::experimental::load(src, coord).with_mask(mask);
        dst_reg = asc::te::experimental::histograms<histogram_config>(dst_reg, src_reg);
    }
    asc::te::experimental::store(dst, asc::te::make_coord(0), dst_reg);
}
```
