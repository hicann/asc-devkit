# asc_set_gm2l1_padding

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

用于设置[asc_copy_gm2l1_arch_3510（高维切分数据搬运）](asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md)接口搬运过程的padding模式使用的padding值。

本接口仅在AIC上生效。

## 函数原型

```c
__aicore__ inline void asc_set_gm2l1_padding(<dtype> padding_value)
__aicore__ inline void asc_set_gm2l1_padding(uint64_t config)
```

### dtype支持的数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：配置half类型的padding值。
__aicore__ inline void asc_set_gm2l1_padding(half padding_value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| padding_value | 输入 | 搬运过程中使用的填充值。 |
| config | 输入 | 搬运过程中使用的填充值配置寄存器。<br>&nbsp;&nbsp;&bull; 若填充值类型为b32，`[0:31]`表示的值作为填充值；若填充值类型为b16，`[0:15]`表示的值作为填充值；若填充值类型为b8，要求`[15:8]`与`[7:0]`取值相同，`[7:0]`表示的值作为填充值；若填充值类型为b4，要求`[15:12]`、`[11:8]`、`[7:4]`与`[3:0]`取值相同，`[3:0]`表示的值作为填充值；<br>&nbsp;&nbsp;&bull; `[32:63]`未使用比特位设置为`0`即可。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口非AIC调用直接返回。
- 当前接口底层使用的寄存器与[asc_set_l12l0a_3d_padding](asc_set_l12l0a_3d_padding.md)为相同的寄存器，因此本设置的值也是[asc_copy_l12l0a_arch_3510（3D矩阵搬运）](asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md)搬运时使用的填充值。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

以下示例将16B输入数据按`pad_func_mode=5`搬运到L1 Buffer。调用`asc_set_gm2l1_padding`将填充值设置为`0xA5`，再通过L1→UB→GM回读完整32B L1数据块，并在主机侧逐字节校验前16B为输入、后16B为填充值。

```cpp
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t INPUT_BYTES = 16;
constexpr uint32_t OUTPUT_BYTES = 32;
constexpr uint8_t PAD_VALUE = 0xA5;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

__global__ __mix__(1, 2) void asc_set_gm2l1_padding_kernel(  // __mix__：AIC配置并回读L1，AIV写回GM。
    __gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(OUTPUT_BYTES);

    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            // 在AIC回传L1结果前初始化AIV侧物理UB。
            asc_copy_gm2ub(staging_ub, input, INPUT_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_intra_arrive(PIPE_MTE3, AIV_TO_AIC_SYNC_ID);
        }
        asc_sync_block_arrive(PIPE_MTE3, CROSS_CORE_FLAG_ID);
        if (is_output_core) {
            asc_sync_intra_wait(PIPE_MTE3, AIC_TO_AIV_SYNC_ID);
            asc_copy_ub2gm(output, result_ub, OUTPUT_BYTES);
        }
    }

    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, AIV_TO_AIC_SYNC_ID);
        asc_sync_block_wait(PIPE_MTE1, CROSS_CORE_FLAG_ID);
        // EXPAND_16B按16B有效输入分组，padding值由本接口配置。
        asc_set_gm2l1_padding(PAD_VALUE);
        asc_copy_gm2l1(l1, input, 1, 1, asc_channel_pad_mode::EXPAND_16B, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(result_ub, l1, 0, 1, OUTPUT_BYTES / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_intra_arrive(PIPE_MTE1, AIC_TO_AIV_SYNC_ID);
    }
    asc_sync_pipe(PIPE_ALL);
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS) return 1;
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_gm2l1_padding_kernel<<<1, 0>>>(output_device, input_device);
    const bool runtime_ok = aclrtSynchronizeDevice() == ACL_SUCCESS &&
        aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    std::vector<uint8_t> golden(OUTPUT_BYTES, PAD_VALUE);
    std::memcpy(golden.data(), input.data(), INPUT_BYTES);
    const bool passed = runtime_ok && output == golden;
    std::cout << "RESULT status=" << (passed ? "PASS" : "FAIL")
              << " input_bytes=" << INPUT_BYTES << " output_bytes=" << OUTPUT_BYTES
              << " pad_value=" << static_cast<uint32_t>(PAD_VALUE) << std::endl;
    aclrtFree(output_device);
    aclrtFree(input_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
