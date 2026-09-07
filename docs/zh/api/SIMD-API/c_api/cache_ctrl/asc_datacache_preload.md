# asc_datacache_preload

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

从Global Memory的指定地址预加载一个Cache Line（64字节）的数据到DCache中，为后续Scalar单元对该地址的读取操作提前填充缓存，降低首次访问延迟。

预取地址由`address`与`offset`相加得到，单位为字节。

本接口需配合后续通过Scalar单元读取Global Memory的指令使用，预取才能带来命中收益。

## 函数原型

```c
__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| address | 输入 | 源操作数在Global Memory中地址，单位为字节。 |
| offset | 输入 | 相对`address`的有符号字节偏移，预取起始地址 = `address` + `offset`，单位为字节。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

频繁调用此接口可能导致保留站（Reservation Station，用于暂存待执行指令的硬件队列）发生拥塞。在此情况下，该指令将被视为NOP指令，进而阻塞Scalar流水线。因此，建议连续调用此接口的次数不要超过4次。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// 本示例用于功能验证，不代表实际算子实现。
// 它模拟Scalar单元通过间接索引读取GM中的分散数据，并提前将下一次访问的Cache Line预取到DCache，
// 以减少后续Scalar读取的访存等待。
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DATA_SIZE = 256;
constexpr uint32_t INDEX_COUNT = 4;
constexpr uint32_t DATA_BYTES = DATA_SIZE * sizeof(uint8_t);
constexpr uint32_t INDEX_BYTES = INDEX_COUNT * sizeof(uint32_t);

__global__ __vector__ void AscDatacachePreloadKernel(__gm__ uint8_t* data, __gm__ uint32_t* indices,
    __gm__ uint32_t* result, uint32_t count)
{
    asc_init();
    if (count == 0) {
        result[0] = 0;
        return;
    }

    // indices保存待访问数据的字节偏移。本例先预取首个索引所在的64B Cache Line，
    // 再在读取当前数据时预取下一索引所在的Cache Line。这样可让下一轮Scalar读取
    // 更可能命中DCache；偏移向下按64B对齐，保证预取范围覆盖目标数据。
    uint32_t current_index = indices[0];
    int64_t cache_offset = static_cast<int64_t>(current_index & ~63ULL);
    // 首轮预取当前索引所在的Cache Line，后续循环提前预取下一轮要读取的数据。
    asc_datacache_preload(reinterpret_cast<__gm__ uint64_t*>(data), cache_offset);

    uint32_t sum = 0;
    for (uint32_t i = 0; i < count; ++i) {
        sum += data[current_index];
        if (i + 1 < count) {
            current_index = indices[i + 1];
            cache_offset = static_cast<int64_t>(current_index & ~63ULL);
            asc_datacache_preload(reinterpret_cast<__gm__ uint64_t*>(data), cache_offset);
        }
    }
    result[0] = sum;
}
} // namespace

int main()
{
    std::vector<uint8_t> data(DATA_SIZE);
    for (uint32_t i = 0; i < DATA_SIZE; ++i) {
        data[i] = static_cast<uint8_t>(i);
    }
    std::vector<uint32_t> indices = {0, 64, 128, 192};
    uint32_t result = 0;
    constexpr uint32_t GOLDEN = 384;
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* data_device = nullptr;
    uint32_t* indices_device = nullptr;
    uint32_t* result_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&data_device), DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&indices_device), INDEX_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&result_device), sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(data_device, DATA_BYTES, data.data(), DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(indices_device, INDEX_BYTES, indices.data(), INDEX_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscDatacachePreloadKernel<<<1, 0>>>(data_device, indices_device, result_device, INDEX_COUNT);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(&result, sizeof(uint32_t), result_device, sizeof(uint32_t), ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = ret == ACL_SUCCESS && result == GOLDEN;
    std::cout << (passed ? "[Success] asc_datacache_preload passed."
                         : "[Failed] asc_datacache_preload failed.") << std::endl;
    aclrtFree(data_device);
    aclrtFree(indices_device);
    aclrtFree(result_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
