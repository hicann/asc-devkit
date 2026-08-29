# global_tensor::SetL2CacheHint（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
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

头文件路径为：`tensor_api/tensor.h`。

**该接口已废弃，请使用[global_tensor::set_l2_cache_hint](../tensor/tensor.md#set_l2_cache_hint)替代。**

设置Global Tensor访问Global Memory时使用的L2 Cache Hint。迁移时需要将PascalCase的`AscendC::Te::CacheMode`枚举值替换为snake_case的`cache_mode`枚举值。

## 函数原型

```cpp
__aicore__ inline constexpr void SetL2CacheHint(AscendC::Te::CacheMode mode);
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| mode | 输入 | L2 Cache模式。旧接口使用`AscendC::Te::CacheMode`，替代接口使用`cache_mode`。 |

## 返回值说明

无。

## 流水类型

无

## 约束说明

该接口仅适用于`global_tensor`，不适用于`local_tensor`。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

gm_tensor.SetL2CacheHint(AscendC::Te::CacheMode::CACHE_MODE_DISABLE); // 废弃写法
gm_tensor.set_l2_cache_hint(cache_mode::disable);                      // 替代写法
```
