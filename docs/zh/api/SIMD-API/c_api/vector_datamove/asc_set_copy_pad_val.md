# asc_set_copy_pad_val

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

头文件路径为：`"c_api/vector_datamove/vector_datamove.h"`。

该接口与[asc_copy_gm2ub_align](./asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)配合使用，用于设置连续搬运数据块左右两侧的填充值。

<!-- npu="950" id8 -->
对于Ascend 950PR/Ascend 950DT产品：

- 该接口仅对`asc_copy_gm2ub_align`接口有效。
- `fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`类型的数据需转换成`int8_t`类型后再调用本接口。
<!-- end id8 -->

本接口仅在AIV上生效。

## 函数原型

```c
// 占位符形式
__aicore__ inline void asc_set_copy_pad_val(<dtype> pad_value)
```

### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：设置float类型的填充值
__aicore__ inline void asc_set_copy_pad_val(float pad_value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| pad_value | 输入 | 需要填补的数据值。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口在非AIV上调用直接返回。
- 本接口仅设置填充值，须先调用本接口写入配置，再调用对应搬运接口。
- 本接口设置的填充值仅在`asc_copy_gm2ub_align`接口的参数`enable_constant_pad`设置为`true`时被取用；若`enable_constant_pad`设置为`false`，填充值配置寄存器不被取用，本接口设置不生效。
- 本接口设置后对后续所有`asc_copy_gm2ub_align`调用生效，直至再次调用本接口覆盖配置；不同搬运任务间若需不同填充值，应在调用搬运接口前重新调用本接口设置。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id9 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 48;
constexpr uint32_t OUTPUT_BYTES = 64;

__global__ __vector__ void asc_set_copy_pad_val_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[OUTPUT_BYTES];
    asc_set_copy_pad_val(static_cast<uint8_t>(42));
    asc_copy_gm2ub_align(local, input, 2, 24, 4, 4, true,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 24, 32);
    asc_sync_mte2(0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
    asc_sync_mte3(0);
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    for (uint32_t block = 0; block < 2; ++block) {
        for (uint32_t i = 0; i < 4; ++i) golden[block * 32 + i] = 42;
        for (uint32_t i = 0; i < 24; ++i) golden[block * 32 + 4 + i] = input[block * 24 + i];
        for (uint32_t i = 28; i < 32; ++i) golden[block * 32 + i] = 42;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_copy_pad_val_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_copy_pad_val passed." : "[Failed] asc_set_copy_pad_val failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
