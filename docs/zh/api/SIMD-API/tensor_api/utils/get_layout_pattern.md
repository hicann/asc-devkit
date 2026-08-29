# get_layout_pattern

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

get_layout_pattern用于从layout类型中提取其LayoutPattern类型信息。

## 原型定义

```cpp
template <typename Layout>
using get_layout_pattern = typename get_pattern<Std::remove_cvref_t<Layout>>::type;
```

## 参数说明

模板参数说明

**表1**  模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| Layout | 输入 | 待提取LayoutPattern的layout类型。 |

## 返回值说明

若Layout包含layout布局模式信息，则返回对应的LayoutPattern类型。

若Layout为make_layout构造的layout，get_layout_pattern会根据shape和stride的类型结构自动推导可识别的LayoutPattern；无法根据类型唯一识别时返回Std::ignore_t。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

using layout_type = decltype(make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(16, 16));
using pattern = get_layout_pattern<layout_type>;
// pattern = nz_layout_ptn

using inferred_layout = decltype(make_layout(make_shape(asc::te::_16{}, asc::te::_16{})));
using inferred_pattern = get_layout_pattern<inferred_layout>;
// inferred_pattern = nd_layout_ptn

using plain_layout = decltype(make_layout(make_shape(16, 16), make_stride(16, 1)));
using plain_pattern = get_layout_pattern<plain_layout>;
// plain_pattern = Std::ignore_t，运行时数值未编码到类型中时无法自动推导
```
