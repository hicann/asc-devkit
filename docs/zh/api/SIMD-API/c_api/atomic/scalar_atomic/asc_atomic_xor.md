# asc_atomic_xor

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

头文件路径为：`"c_api/atomic/scalar_atomic.h"`。

对Global Memory中`address`指向的单个元素执行原子按位异或操作：读取该地址中的旧值`old_value`，将`old_value`与输入标量值`val`进行按位异或运算，将结果`new_value`写回该地址，并返回`old_value`。整个读取、计算和写回过程为原子操作。

计算公式如下：

$$
new\_value = old\_value\ \oplus\ val
$$

## 函数原型

```c
__aicore__ inline <dtype> asc_atomic_xor(__gm__ <dtype>* address,
                                         <dtype> val)
```

### dtype支持数据类型

`dtype`取值为：`int32_t`、`uint32_t`、`int64_t`、`uint64_t`。

### 函数原型典型示例

```c
// 示例：int32_t类型标量原子按位异或
__aicore__ inline int32_t asc_atomic_xor(__gm__ int32_t* address,
                                         int32_t val)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入/输出 | Global Memory的地址。 |
| val | 输入 | 标量值，数据类型与`address`指向元素类型一致。 |

## 返回值说明

返回`address`地址中计算前的原始数据`old_value`。

## 约束说明

- `address`必须落在Global Memory地址空间。
- `address`需按`sizeof(dtype)`字节对齐。
- 对同一`address`的并发调用以原子方式完成“读取、计算、写回”，不会丢失更新；最终写入结果为各参与值的按位异或结果。
- 本接口运行在标量流水（`PIPE_S`）上，同一标量流水内的数据依赖由指令执行顺序保证。若本接口与`PIPE_MTE2`或`PIPE_MTE3`上的数据搬运指令访问同一GM地址，且执行顺序影响结果，编译器无法自动完成跨流水同步，调用方需按实际依赖插入[asc_sync_pipe](../../sync/asc_sync_pipe.md)，或配合使用[asc_sync_notify](../../sync/asc_sync_notify.md)与[asc_sync_wait](../../sync/asc_sync_wait.md)保证执行顺序。
- 本接口访问GM时绕过DCache，不维护缓存一致性。若其他核或其他通路通过缓存访问同一GM地址，调用方需使用[asc_dcci](../../cache_ctrl/asc_dcci.md)清理或失效对应Cache Line，并使用[asc_sync_data_barrier](../../sync/asc_sync_data_barrier.md)保证相关访存操作的执行顺序和数据可见性。详情可参考[Scalar原子操作与DCache一致性](../../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md#scalar原子操作与dcache一致性)。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS = 8;

__global__ __vector__ void asc_atomic_xor_kernel(__gm__ int64_t* output)
{
    asc_init();
    __gm__ uint32_t* address = reinterpret_cast<__gm__ uint32_t*>(output);
    asc_dcci_entire_all();
    const uint32_t old_value = asc_atomic_xor(address, 15U);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    asc_dcci_entire_all();
    output[1] = static_cast<int64_t>(old_value);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {10, 0};
    std::vector<int64_t> golden = {5, 10};
    input.resize(ELEMENTS, 0);
    golden.resize(ELEMENTS, 0);
    std::vector<int64_t> output(ELEMENTS, -1);
    aclInit(nullptr);
    aclrtSetDevice(0);
    int64_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENTS) * sizeof(int64_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, input.size() * sizeof(int64_t), input.data(), input.size() * sizeof(int64_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_atomic_xor_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_atomic_xor passed." : "[Failed] asc_atomic_xor failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
