# asc_set_ctrl

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

头文件路径为：`"c_api/spr/spr.h"`。

对CTRL寄存器（控制寄存器）的特定比特位进行设置。

## 函数原型

```c
__aicore__ inline void asc_set_ctrl(uint64_t config)
```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
| :------ | :---  | :------------ |
|config   |输入   |待设置的寄存器值。常用CTRL寄存器比特位说明参考表2。|

**表2** 常用CTRL寄存器比特位说明<a id="ctrl_bit_desc"></a>

|CTRL比特位    |功能|
| :-------     | :---- |
|CTRL[48]      |用于控制浮点计算/部分浮点精度转换的饱和模式。<br>- 1'b0：饱和模式，INF输出会被饱和为±MAX，NAN输出会被饱和为0；<br> - 1'b1：非饱和模式，INF/NAN保持原输出。<br> 该控制位仅支持如下数据类型：<br>- 浮点数计算时支持half数据类型；<br>- 浮点数精度转换时支持如下数据类型：hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t。|
|CTRL[49]      |用于控制标量单元访问Unified Buffer（UB）时是否经过DCache。DCache的读取速度快于UB，因此在对UB进行大量读取，且UB中数据不被Vector单元写入时，可选择开启该模式。<br>- 1'b0：标量单元访问UB时，直接从UB中读取；<br> - 1'b1：标量单元访问UB时，从DCache中读取。|
|CTRL[50]      |用于控制fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t相关场景的饱和模式。<br>- 1'b0：饱和模式；<br> - 1'b1：非饱和模式。|
|CTRL[53]      |用于控制整数计算指令的饱和模式。<br>- 1'b0：截断模式，溢出值按目标数据类型位数截断，保留低位，舍弃高位；<br>- 1'b1：饱和模式，溢出值饱和到±MAX。|
|CTRL[56]      |MASK模式控制位。MASK用于辅助SIMD指令执行，由于CTRL[56]指示两种模式：<br>- 1'b0：向量操作中的掩码寄存器。每位对应向量中一个元素，标记该元素是否参与计算。'1'表示对应元素将被计算；'0'表示对应元素将不被计算；<br>- 1'b1：MASK[31:0]用于指示SIMD指令实际操作的元素数量。实际重复次数由VECTOR自动推断，此时参数repeat time(Xt[63:56])被忽略。|
|CTRL[59]      |用于控制浮点数转整数或整数转整数时的精度转换饱和模式，仅在CTRL[60]开启时生效。<br>- 1'b0：饱和模式，溢出值饱和到±MAX；<br>- 1'b1：截断模式：溢出值按照目标数据类型位数截断，保留低位，舍弃高位。|
|CTRL[60]      |饱和控制策略选择位。<br>- 1'b0：按单指令/API路径配置饱和（对应`asc_override_strategy::USE_API`）；<br>- 1'b1：使用全局CTRL饱和配置（对应`asc_override_strategy::USE_GLOBAL`），此时CTRL[59]的CAST配置才会生效。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 使用前需要先用[asc_get_ctrl()](asc_get_ctrl.md)获取当前值，仅对需要变更的bit位进行修改。
- 常用可配置比特位包括CTRL[48]、CTRL[50]、CTRL[53]、CTRL[56]、CTRL[59]、CTRL[60]。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

void print_data(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}

void print_hex(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":" << std::hex;
    for (uint32_t i = 0; i < 4; ++i) std::cout << " 0x" << data[i];
    std::cout << std::dec << std::endl;
}

__global__ __vector__ void asc_set_ctrl_kernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t before = static_cast<uint64_t>(asc_get_ctrl());
    const uint64_t changed = before ^ (1ULL << 56);
    asc_set_ctrl(changed);
    const uint64_t after = static_cast<uint64_t>(asc_get_ctrl());
    asc_set_ctrl(before);
    const uint64_t restored = static_cast<uint64_t>(asc_get_ctrl());
    output[0] = before;
    output[1] = after;
    output[2] = restored;
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
    asc_set_ctrl_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = ((output[0] ^ output[1]) & (1ULL << 56)) != 0 && output[2] == output[0];
    std::cout << (passed ? "[Success] asc_set_ctrl passed." : "[Failed] asc_set_ctrl failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
