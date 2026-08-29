# pointer

## 功能说明

Tensor API通过`make_mem_ptr`构造带物理存储位置信息的指针适配器。指针适配器保留原始指针或迭代器的访问能力，并通过`ptr_pattern`标记Global Memory、L1 Buffer、L0A Buffer等物理位置。使用时只需调用对外接口`make_mem_ptr`，不需要直接构造内部指针适配器类型。

## 约束说明

- 原始指针必须指向有效的内存空间。
- 指定的`location`必须与实际物理存储位置一致。
- 指针适配器的数据类型必须与实际数据类型一致。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr int32_t size = 256;
__ubuf__ float ub_data[size];
__cbuf__ float l1_data[size];
__ca__ float l0a_data[size];

// 创建Unified Buffer（UB）指针
auto ub_ptr = asc::te::make_mem_ptr(ub_data);

// 创建L1 Buffer指针
auto l1_ptr = asc::te::make_mem_ptr(l1_data);

// 创建L0A Buffer指针
auto ca_ptr = asc::te::make_mem_ptr(l0a_data);
```
