# asc_get_arch_ver

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

获取当前AI处理器架构版本号。

不同型号服务器有不同的架构版本号取值，如下所示：
<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：3510
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：2201
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：2201
<!-- end id10 -->

## 函数原型

```c
__aicore__ inline void asc_get_arch_ver(uint32_t& core_version)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| core_version | 输出 | AI处理器架构版本，数据类型uint32_t。接收当前核的核心版本字段值，接口返回时该参数被覆写为ARCH_VER寄存器中的核心版本字段值，调用前无需初始化。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

在调用asc_get_arch_ver接口前，需先定义core_version,调用asc_get_arch_ver接口后core_version会变成相对应架构版本号的值。

## 调用示例

本示例模拟算子部署诊断场景：Kernel读取架构版本并写入GM，Host侧检查返回值是否属于当前接口支持的架构。返回值可用于确认编译产物的目标架构。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id14 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id14 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint32_t);

/*
 * 本示例说明：
 * - Kernel调用asc_get_arch_ver获取当前架构版本号。
 * - 通过Scalar将版本号写入GM，并刷新对应的Cache Line。
 * - Host侧读取并检查版本号，辅助确认算子二进制的目标架构。
 */
__global__ __vector__ void asc_get_arch_ver_kernel(__gm__ uint32_t* output)
{
    asc_init();
    uint32_t core_version = 0;
    asc_get_arch_ver(core_version);
    output[0] = core_version;
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint32_t> output(ELEMENTS, 0);
    uint32_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    asc_get_arch_ver_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Architecture version: " << output[0] << std::endl;
    const bool passed = output[0] == 2201U || output[0] == 3510U;
    std::cout << (passed ? "[Success] asc_get_arch_ver passed."
                          : "[Failed] asc_get_arch_ver failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
