# asc_atomic_dec

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

对Global Memory中`address`指向的一个`uint32_t`或`uint64_t`元素执行原子递减操作。读取该元素的旧值`old_value`，按照以下公式计算并写回新值`new_value`：

$$
new\_value = (old\_value == 0 || old\_value > val) ? val : (old\_value - 1)
$$

接口返回`old_value`。整个读取、判断和写回过程为原子操作。

## 函数原型

```c
__aicore__ inline <dtype> asc_atomic_dec(__gm__ <dtype>* address,
                                         <dtype> val)
```

### dtype支持数据类型

dtype支持的数据类型为`uint32_t`、`uint64_t`。

### 函数原型典型示例

```c
// 示例：uint32_t类型标量原子递减。
__aicore__ inline uint32_t asc_atomic_dec(__gm__ uint32_t* address,
                                          uint32_t val)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | Global Memory的地址。 |
| val | 输入 | 源操作数。 |

## 返回值说明

`address`地址中计算前的原始数据。

## 流水类型

PIPE_S

## 约束说明

- 在开启编译器自动同步功能的前提下，编译器能够自动在PIPE_MTE2/PIPE_MTE3与PIPE_S之间插入同步。但是，asc_atomic_dec为标量计算，在读写GM时如果与搬运单元（MTE2/MTE3）存在数据依赖，编译器却无法自动插入同步，开发者需要根据实际情况手动插入同步。
- Scalar原子操作会绕过DCache，需要调用[asc_dcci](../../cache_ctrl/asc_dcci.md)接口确保GM与DCache的一致性。

## 调用示例

将以下代码保存为`example.asc`后，执行对应的编译运行命令。

<!-- npu="950" id8 -->

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENTS = 8;

__global__ __vector__ void asc_atomic_dec_kernel(__gm__ uint32_t* output)
{
    asc_init();
    // 确保Atomic访问GM前，目标Cache Line中的数据已写回。
    asc_dcci_single(output);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = asc_atomic_dec(output, 20U);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    // Atomic绕过DCache，普通Scalar写前需使目标Cache Line中的旧副本失效。
    asc_dcci_single(output);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    output[1] = old_value;
    // 将普通Scalar写产生的Dirty Cache Line写回GM。
    asc_dcci_single(output);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
}
} // namespace

int main()
{
    std::vector<uint32_t> input = {10, 0};
    std::vector<uint32_t> golden = {9, 10};
    input.resize(ELEMENTS, 0);
    golden.resize(ELEMENTS, 0);
    std::vector<uint32_t> output(ELEMENTS, 0xffffffffU);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENTS) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, input.size() * sizeof(uint32_t), input.data(), input.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_atomic_dec_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_atomic_dec passed." : "[Failed] asc_atomic_dec failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
