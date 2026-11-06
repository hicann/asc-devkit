# asc_shiftleft_scalar

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_logic.h"`。

根据掩码将源操作数矢量数据寄存器中的元素左移，左移位数由标量立即数指定，并将结果通过函数返回值返回或写入目的操作数。掩码对应位置为1的元素参与计算，为0的元素在输出结果中置零。计算公式如下：

$$
dst_i = src_i \ll shift
$$

根据源操作数的数据类型，左移操作分为以下两种情况：

- **无符号整数：执行逻辑左移。** 逻辑左移会将二进制数整体向左移动指定的位数，最高位被丢弃，最低位用0填充。例如，二进制数1010101010101010（`uint16_t`类型）逻辑左移1位后，结果为0101010101010100。
- **有符号整数：执行算术左移。** 算术左移会将二进制数整体向左移动指定的位数，最高位被丢弃（符号位会被丢弃），最低位用0填充。例如，二进制数1010101010101010（`int16_t`类型）算术左移1位后，结果为0101010101010100；算术左移3位后，结果为0101010101010000。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_shiftleft_scalar(vector_<dtype> src,
                                                           int16_t shift,
                                                           vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_shiftleft_scalar(vector_<dtype>& dst,
                                                 vector_<dtype> src,
                                                 int16_t shift,
                                                 vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`。

### 函数原型典型示例

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_int8_t asc_shiftleft_scalar(vector_int8_t src,
                                                          int16_t shift,
                                                          vector_bool mask)

// 通过引用参数输出结果
__simd_callee__ inline void asc_shiftleft_scalar(vector_int8_t& dst,
                                                 vector_int8_t src,
                                                 int16_t shift,
                                                 vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量数据寄存器）。仅无返回值类型接口包含该参数，数据类型须与`src`一致。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| shift | 输入 | 位移量（标量），类型为`int16_t`。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与计算的元素。对应位置为1时参与计算，为0时不参与计算且输出结果对应元素置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

对于返回值类型接口，返回保存左移结果的矢量数据寄存器，数据类型与`src`保持一致。

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 掩码位为0的元素位置不参与左移运算，输出结果对应位置写0。

### 位移约束

<!-- npu="950" id8 -->
- Ascend 950PR&950DT系列产品，当`shift`设置为负数时，将对源操作数执行右移操作，右移规则取决于源操作数的数据类型：
    - 源操作数为无符号整型：执行逻辑右移，高位补`0`。
    - 源操作数为有符号整型：执行算术右移，高位补符号位。
- Ascend 950PR&950DT系列产品，当`shift`为负数，且绝对值大于`src`的数据类型位宽时，输出结果取决于源操作数的数据类型：
    - 源操作数为无符号整型：结果为 `0`；
    - 源操作数为有符号整型：结果收敛至符号位，源操作数为正数或`0`得到 `0`，为负数得到 `-1`。
<!-- end id8 -->
- 当`shift`为正数，且值大于`src`的数据类型位宽时，输出结果中的有效元素置`0`。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id9 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 257 * sizeof(uint16_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint16_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint16_t);

__simd_vf__ inline void shiftleft_scalar_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t src;
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src, input + offset);
        vector_uint16_t dst = asc_shiftleft_scalar(src, 1, mask);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_shiftleft_scalar_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<shiftleft_scalar_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(TOTAL_LENGTH);
    std::vector<uint16_t> output(TOTAL_LENGTH, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<uint16_t>(i);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_shiftleft_scalar_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const uint16_t expected = static_cast<uint16_t>(input[i] << 1);
        if (output[i] != expected) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_shiftleft_scalar completed."
                         : "[Failed] asc_shiftleft_scalar output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
