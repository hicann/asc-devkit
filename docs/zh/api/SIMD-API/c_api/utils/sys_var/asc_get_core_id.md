# asc_get_core_id

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

头文件路径为：`"c_api/utils/sys_var.h"`。

获取当前AI Core的编号。一个AI Core中包含1个AIC和若干个AIV，[内置变量](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#section199434523343)中的`block_idx`、`sub_block_idx`用于获取的是一个AI Core内部AIC或AIV的索引，而本接口获取的是一个AI Core相对其余AI Core的索引，二者层级不同。

该接口常用于按核编号对任务进行划分，使不同的AI Core处理不同的数据分片，以实现多核并行。

## 函数原型

```c
__aicore__ inline int64_t asc_get_core_id()
```

## 参数说明

无

## 返回值说明

返回当前AI Core的编号。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// 运行步骤：
//   1. 启动 2 个 block（<<<2, 0>>>）；
//   2. 每个 block 通过内建变量 block_idx 获取索引，并调用 asc_get_core_id() 获取
//      物理核编号；
//   3. 各 block 将核编号写入自身索引对应的位置；
//   4. 校验每个 block 都成功写入了非哨兵值的有效核编号。

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);

void PrintData(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}

void PrintHex(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":" << std::hex;
    for (uint32_t i = 0; i < 4; ++i) std::cout << " 0x" << data[i];
    std::cout << std::dec << std::endl;
}

__global__ __vector__ void AscGetCoreIdKernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t index = static_cast<uint64_t>(block_idx);
    output[index * 8] = static_cast<uint64_t>(asc_get_core_id());
    asc_dcci_single(output + index * 8);
}

} // namespace

int main()
{
    std::vector<uint64_t> output(ELEMENTS, ~0ULL);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint64_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscGetCoreIdKernel<<<2, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Observed", output);
    PrintHex("Observed hex", output);
    const bool passed = output[0] != ~0ULL && output[8] != ~0ULL;
    std::cout << (passed ? "[Success] asc_get_core_id passed." : "[Failed] asc_get_core_id failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
