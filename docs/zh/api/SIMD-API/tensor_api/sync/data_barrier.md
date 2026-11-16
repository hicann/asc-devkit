# sync_data_barrier

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

头文件路径为：`tensor_api/tensor.h`。

阻塞后续的指令执行，直到此前已发出但尚未完成的内存访问指令执行完成。可以通过参数指定需要等待的内存范围。

## 函数原型

```cpp
__aicore__ inline void sync_data_barrier(mem_dsb_t scope)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| scope | 输入 | 需要等待的内存作用域，类型为`mem_dsb_t`。取值为： <br>• `DSB_ALL`：等待GM和UB访问指令。 <br>• `DSB_DDR`：等待GM访问指令。 <br>• `DSB_UB`：等待UB访问指令。 |

## 返回值说明

无

## 约束说明

- `mem_dsb_t`类型的入参必须直接传递枚举值字面量。
- 等待的内存位置取值为`DSB_ALL`时，只能等待GM和Unified Buffer（UB）的访问指令，不包括其他内存位置，例如L0C Buffer、L1 Buffer。

## 调用示例

以下示例先写入第一个GM数据，调用本接口等待写入完成后，再写入第二个GM数据，保证两次写入的发布顺序。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr int32_t first_value = 1;
constexpr int32_t second_value = 2;

__global__ __vector__ void data_barrier_kernel(__gm__ int32_t* output)
{
    using namespace asc::te;
    output[0] = first_value;
    sync_data_barrier(mem_dsb_t::DSB_DDR);
    output[1] = second_value;
}
```
