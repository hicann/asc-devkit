# asc_dci

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

头文件路径为：`"c_api/cache_ctrl/cache_ctrl.h"`。

数据缓存无效化接口，将整个DCache的所有Cache Line标记为无效，使其从缓存中移除。本接口不检查缓存行是否为脏（dirty，表示数据已被修改但尚未写回到GM），若存在脏数据将被直接丢弃，不执行回写。调用后，下一次访问这些内存地址时会从GM重新加载数据，而不是使用可能过期的缓存数据。

本接口无参数，调用即触发整个DCache的全量失效操作。

## 函数原型

```c
__aicore__ inline void asc_dci()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

在调用`asc_dci`之前，必须调用[asc_sync_data_barrier](../sync/asc_sync_data_barrier.md)，并将参数设置为`mem_dsb_t::DSB_ALL`，等待此前的内存访问指令执行结束，再使整个数据缓存失效。调用顺序如下：

```cpp
asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
asc_dci();
```

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <array>
#include <cstdint>
#include <iostream>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr int32_t GM_VALUE = 11;
constexpr int32_t DIRTY_VALUE = 101;
constexpr uint32_t DATA_COUNT = 16;

__global__ __vector__ void AscDciKernel(__gm__ int32_t* data, __gm__ int32_t* output)
{
    asc_init();
    // 本示例用于验证asc_dci不会将DCache中的脏数据回写到GM。Scalar先将GM地址写为脏值，
    // 失效后再次通过Scalar读取该地址，应从GM重新加载初始值，而非得到脏值。
    volatile __gm__ int32_t* data_gm = data;
    data_gm[0] = DIRTY_VALUE;

    // 等待前序访存完成后，使整个DCache失效，直接丢弃其中的脏数据。
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    asc_dci();

    const int32_t value_after_dci = data_gm[0];
    asc_store_dev(output, value_after_dci);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
}

bool CheckResult(int32_t output)
{
    return output == GM_VALUE;
}
} // namespace

int main()
{
    std::array<int32_t, DATA_COUNT> input{};
    input.fill(GM_VALUE);
    int32_t output = 0;
    aclInit(nullptr);
    aclrtSetDevice(0);

    int32_t* input_device = nullptr;
    int32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(int32_t), input.data(), input.size() * sizeof(int32_t), ACL_MEMCPY_HOST_TO_DEVICE);
    AscDciKernel<<<1, 0>>>(input_device, output_device);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(&output, sizeof(int32_t), output_device, sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = ret == ACL_SUCCESS && CheckResult(output);
    std::cout << (passed ? "[Success] asc_dci passed." : "[Failed] asc_dci failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
