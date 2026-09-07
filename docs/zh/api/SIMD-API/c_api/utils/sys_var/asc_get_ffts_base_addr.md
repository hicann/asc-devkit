# asc_get_ffts_base_addr

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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

头文件路径为：`"c_api/utils/sys_var.h"`。

获取核间同步寄存器的基地址。需要在Host侧调用接口aclrtGetHardwareSyncAddr获取核间同步寄存器的基地址，并作为参数传入[asc_set_ffts_base_addr](asc_set_ffts_base_addr.md)后使用。

## 函数原型

```c
__aicore__ inline int64_t asc_get_ffts_base_addr()
```

## 参数说明

无

## 返回值说明

核间同步寄存器的基地址。

## 流水类型

PIPE_S

## 约束说明

本接口为只读查询接口，读取的是核间同步寄存器的基地址，调用方不可直接设置或修改返回的基地址值；基地址需通过[asc_set_ffts_base_addr](asc_set_ffts_base_addr.md)预先配置后方可用于核间同步。

## 调用示例

本示例模拟核间同步初始化检查场景：Host侧通过`aclrtGetHardwareSyncAddr`获取硬件同步地址，Kernel将该地址转换为`uint64_t`类型并调用`asc_set_ffts_base_addr`完成设置，再通过`asc_get_ffts_base_addr`回读。Host侧检查设置值与回读值是否一致。完成该配置后，才能使用[asc_sync_block_arrive](../../sync/asc_sync_block_arrive.md)和[asc_sync_block_wait](../../sync/asc_sync_block_wait.md)进行核间同步。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="A3,910b" id8 -->
以[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-2201 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);
constexpr uint64_t UNSET_VALUE = ~0ULL;

/*
 * 本示例说明：
 * - ffts_addr由Host侧调用aclrtGetHardwareSyncAddr获得。
 * - Kernel将ffts_addr转换为uint64_t类型并设置核间同步寄存器基地址。
 * - Kernel回读基地址，Host侧检查设置值与回读值是否一致。
 */
__global__ __vector__ void asc_get_ffts_base_addr_kernel(
    __gm__ uint64_t* output, __gm__ uint64_t* ffts_addr)
{
    asc_init();
    const uint64_t config = reinterpret_cast<uint64_t>(ffts_addr);
    asc_set_ffts_base_addr(config);
    output[0] = config;
    output[1] = static_cast<uint64_t>(asc_get_ffts_base_addr());
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint64_t> output(ELEMENTS, UNSET_VALUE);
    uint64_t* output_device = nullptr;
    void* ffts_addr = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtGetHardwareSyncAddr(&ffts_addr);
    asc_get_ffts_base_addr_kernel<<<1, 0>>>(
        output_device, reinterpret_cast<uint64_t*>(ffts_addr));
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Configured FFTS base: 0x" << std::hex << output[0] << std::endl;
    std::cout << "Read-back FFTS base: 0x" << output[1] << std::dec << std::endl;
    const bool passed = output[0] != UNSET_VALUE && output[0] == output[1];
    std::cout << (passed ? "[Success] asc_get_ffts_base_addr passed."
                          : "[Failed] asc_get_ffts_base_addr failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
