# asc_get_smmu_tag_version

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

获取系统内存管理单元（System Memory Management Unit，SMMU）版本信息。常用于在运行时判断当前SMMU的版本或发布日期，以便进行版本相关的处理。

## 函数原型

```c
__aicore__ inline int64_t asc_get_smmu_tag_version()
```

## 参数说明

无

## 返回值说明

SMMU版本信息。各bit含义如下：

**表1** SMMU标签版本比特位说明

| bit范围    | 含义 |
| ----------- |:----|
| 63:28 | 保留位。 |
| 27:0 | SMMU标签版本。 |
| 27:14 | 年。格式：YY-YYYY-YYYY。 |
| 13:8 | 月。格式：MM-MMMM。 |
| 7:2 | 日。格式：DD-DDDD。 |
| 1:0 | 同一日期的子版本。 |

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

本示例模拟运行环境兼容性诊断场景：Kernel读取SMMU标签版本并写入GM，Host侧按照返回值说明解析年、月、日和同日子版本字段。部分设备可能返回0，示例仅检查Kernel是否完成写回，不把非零值作为成功条件。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
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
 * - Kernel调用asc_get_smmu_tag_version读取SMMU标签版本。
 * - 原始版本值写入GM并刷新对应的Cache Line。
 * - Host侧按照接口定义解析各版本字段，便于记录运行环境信息。
 */
__global__ __vector__ void asc_get_smmu_tag_version_kernel(__gm__ uint64_t* output)
{
    asc_init();
    output[0] = static_cast<uint64_t>(asc_get_smmu_tag_version());
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint64_t> output(ELEMENTS, UNSET_VALUE);
    uint64_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_get_smmu_tag_version_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const uint64_t version = output[0] & 0x0FFFFFFFULL;
    const uint64_t year = (version >> 14U) & 0x3FFFU;
    const uint64_t month = (version >> 8U) & 0x3FU;
    const uint64_t day = (version >> 2U) & 0x3FU;
    const uint64_t sub_version = version & 0x3U;
    std::cout << "SMMU tag version: 0x" << std::hex << version << std::dec << std::endl;
    std::cout << "Decoded fields: year=" << year << ", month=" << month
              << ", day=" << day << ", sub-version=" << sub_version << std::endl;

    const bool passed = output[0] != UNSET_VALUE;
    std::cout << (passed ? "[Success] asc_get_smmu_tag_version passed."
                          : "[Failed] asc_get_smmu_tag_version failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
