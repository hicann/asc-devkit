# asc_get_phy_stack_base

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

获取物理堆栈基地址。常用于调试时定位当前核的堆栈在物理内存中的位置。

## 函数原型

```c
__aicore__ inline int64_t asc_get_phy_stack_base()
```

## 参数说明

无

## 返回值说明

物理堆栈基地址。

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
// 示例功能：演示 asc_get_phy_stack_base 接口的用法，读取当前核物理栈的基地址。
//
// 运行步骤：
//   1. 连续两次调用 asc_get_phy_stack_base()，分别写入 output[0] 与 output[1]；
//   2. 校验两次读到的栈基地址一致（说明该地址在核内是稳定的）。

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

__global__ __vector__ void AscGetPhyStackBaseKernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t phy_stack_base = static_cast<uint64_t>(asc_get_phy_stack_base());
    output[0] = phy_stack_base;
    output[1] = static_cast<uint64_t>(asc_get_phy_stack_base());
    asc_dcci_single(output);
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
    AscGetPhyStackBaseKernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Observed", output);
    PrintHex("Observed hex", output);
    const bool passed = output[0] == output[1];
    std::cout << (passed ? "[Success] asc_get_phy_stack_base passed." : "[Failed] asc_get_phy_stack_base failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
