# asc_float2half

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

将`float`类型的标量转换为`half`类型并返回，舍入模式固定为ODD（最近邻奇数舍入）。

关于舍入模式和饱和/非饱和模式的详细说明，请参见[舍入模式与饱和模式](../../reg_compute/reg_convert/rounding_mode.md)。

## 函数原型

```c
__aicore__ inline half asc_float2half_ro(float value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| value | 输入 | 源操作数（标量），数据类型为`float`。 |

## 返回值说明

返回输入数据`value`类型转换成`half`的结果。

## 约束说明

- 饱和/非饱和模式可通过[asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md)接口配置。
- `nan`、`inf`和超出`half`表示范围的输入值的处理方式，请参见[舍入模式与饱和模式](../../reg_compute/reg_convert/rounding_mode.md)。
- 本接口运行在标量流水（`PIPE_S`）上，同一流水内的数据依赖由指令执行顺序保证，无需额外同步。

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
constexpr uint32_t ELEMENTS = 1;
constexpr uint32_t BYTES = ELEMENTS * sizeof(half);
constexpr float INPUT_VALUE = 1.5f;

__global__ __vector__ void asc_float2half_kernel(__gm__ half* output)
{
    asc_init();
    output[0] = asc_float2half_ro(INPUT_VALUE);
}
} // namespace

int main()
{
    std::vector<half> output(ELEMENTS, half(0.0f));
    const std::vector<half> golden(ELEMENTS, half(INPUT_VALUE));
    aclInit(nullptr);
    aclrtSetDevice(0);
    half* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    asc_float2half_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_float2half_ro passed."
                         : "[Failed] asc_float2half_ro failed.")
              << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
