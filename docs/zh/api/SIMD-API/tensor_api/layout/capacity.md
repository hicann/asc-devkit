# capacity

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

capacity用于计算layout所需的实际内存容量。

与size（有效元素个数）不同，capacity包含了因非连续步长导致的无效数据区域。对于连续排列（stride紧凑），capacity与size相等；对于有间隔的排列，capacity通常大于size。

## 函数原型

  ```cpp
  template <size_t... Is, typename Layout>
  __aicore__ inline constexpr auto capacity(const Layout& layout)
  ```

## 参数说明

**表1**  模板参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| Is... | size_t | 索引序列，指定子维度范围，仅对选定维度计算容量。 |
| Layout | 输入 | layout对象的类型。 |

**表2**  参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| layout | 输入 | 用于描述张量的布局。 |

## 返回值说明

返回layout实际所占面积大小，包括有效数据和脏数据。

## 约束说明

Is...必须为有效范围内的索引。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// 示例1：紧凑排列，capacity = size
auto layout1 = make_layout(make_shape(10, 20), make_stride(20, 1));
auto cap1 = capacity(layout1);
// dim0: 10 * 20 = 200; dim1: 20 * 1 = 20; max = 200
// size = 10 * 20 = 200

// 示例2：非连续排列，capacity > size
auto layout2 = make_layout(make_shape(10, 20), make_stride(1, 100));
auto cap2 = capacity(layout2);
// dim0: 10 * 1 = 10; dim1: 20 * 100 = 2000; max = 2000
// size = 10 * 20 = 200

// 示例3：指定子维度
auto layout3 = make_layout(make_shape(10, 20, 30), make_stride(1, 10, 200));
auto cap3_all = capacity(layout3);       // max(10*1, 20*10, 30*200) = max(10, 200, 6000) = 6000
auto cap3_0_1 = capacity<0, 1>(layout3); // max(10*1, 20*10) = max(10, 200) = 200
```
