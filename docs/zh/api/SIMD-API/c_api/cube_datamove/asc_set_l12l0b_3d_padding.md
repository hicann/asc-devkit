# asc_set_l12l0b_3d_padding

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

用于调用[asc_copy_l12l0b_arch_3510（3D矩阵搬运）](asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md)时设置3D格式分形矩阵搬运（im2col展开）搬运指令使用的填充值。当`asc_copy_l12l0b`接口中的`f_matrix_ctrl`参数设置为`true`时，3D格式分形矩阵搬运（im2col展开）场景会使用本接口设置的值作为填充值。

本接口为矩阵3D格式搬入相关配置接口，仅在AIC上生效。

## 函数原型

```c
__aicore__ inline void asc_set_l12l0b_3d_padding(<dtype> padding_value)
__aicore__ inline void asc_set_l12l0b_3d_padding(uint64_t config)
```

### dtype支持的数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：配置half类型的padding值。
__aicore__ inline void asc_set_l12l0b_3d_padding(half padding_value)
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

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t H = 2;
constexpr uint32_t W = 2;
constexpr uint32_t C0 = 16;
constexpr uint32_t CIN = C0;
constexpr uint32_t COUT = C0;
constexpr uint32_t PAD = 1;
constexpr uint32_t HOUT = H + 2 * PAD;
constexpr uint32_t WOUT = W + 2 * PAD;
constexpr uint32_t M = HOUT * WOUT;
constexpr uint32_t M_ALIGN = 16;
constexpr uint32_t K = CIN;
constexpr uint32_t K_ALIGN = 16;
constexpr half PAD_VALUE = static_cast<half>(2);
constexpr uint32_t FMAP_ELEMENTS = H * W * CIN;
constexpr uint32_t WEIGHT_ELEMENTS = COUT * K;
constexpr uint32_t OUTPUT_ELEMENTS = COUT * M;

__global__ __cube__ void asc_copy_l12l0b_3d_bside_kernel(
    __gm__ half* fmap, __gm__ half* weight, __gm__ half* output)
{
    asc_init();
    __cbuf__ half fmap_l1[FMAP_ELEMENTS];
    __cbuf__ half weight_l1[WEIGHT_ELEMENTS];
    __ca__ half weight_l0[COUT * K_ALIGN];
    __cb__ half fmap_l0[K_ALIGN * M_ALIGN];
    __cc__ float output_l0[COUT * M_ALIGN];

    asc_set_gm2l1_nz_para(1, 1, H * W, 0);
    asc_copy_gm2l1_nd2nz(fmap_l1, fmap, CIN * sizeof(half), 0, H * W, CIN, 0, false);
    asc_set_gm2l1_nz_para(1, 1, K_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(weight_l1, weight, K * sizeof(half), 0, COUT, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_copy_l12l0a(weight_l0, weight_l1, 0, 0, COUT / 16, K_ALIGN / 16, COUT / 16, COUT / 16);

    asc_l13d_fmatrix_config fmatrix_config;
    fmatrix_config.l1_width = W;
    fmatrix_config.l1_height = H;
    fmatrix_config.padding_left_size = PAD;
    fmatrix_config.padding_right_size = PAD;
    fmatrix_config.padding_top_size = PAD;
    fmatrix_config.padding_bottom_size = PAD;
    // f_matrix_ctrl=true时，3D搬运从B侧寄存器组读取Feature Map描述和padding值。
    asc_set_l13d_fmatrix_b(fmatrix_config);
    asc_set_l12l0b_3d_padding(PAD_VALUE);
    asc_set_l13d_rpt_b(0, 1, asc_l13d_repeat_direction::M_DIRECTION, M_ALIGN / 16, 0);
    // 目的地址为L0B Buffer；f_matrix_ctrl=true，选择上面配置的B侧寄存器组。
    asc_copy_l12l0b(
        fmap_l0, fmap_l1, K_ALIGN, M, 0, 0, 1, 1, 1, 1, 1, 1, false, false, false, true, CIN);

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(output_l0, weight_l0, fmap_l0, COUT, K_ALIGN, M, 0, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(
        output, output_l0, M, COUT, M, COUT, 0, 0, 0, static_cast<uint64_t>(QuantMode_t::F322F16), 0, false,
        true, 0, 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_ALL);
}

} // namespace

int main()
{
    // 有效Feature Map全为0；L0A矩阵全为1，使MMAD结果只反映L0B中的padding值。
    std::vector<half> fmap(FMAP_ELEMENTS, static_cast<half>(0));
    std::vector<half> weight(WEIGHT_ELEMENTS, static_cast<half>(0));
    std::vector<half> output(OUTPUT_ELEMENTS);
    for (uint32_t i = 0; i < WEIGHT_ELEMENTS; ++i) weight[i] = static_cast<half>(1);

    half *fmap_device = nullptr, *weight_device = nullptr, *output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&fmap_device), fmap.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&weight_device), weight.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS) {
        return 1;
    }
    aclrtMemcpy(fmap_device, fmap.size() * sizeof(half), fmap.data(), fmap.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(weight_device, weight.size() * sizeof(half), weight.data(), weight.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_l12l0b_3d_bside_kernel<<<1, 0>>>(fmap_device, weight_device, output_device);
    const bool runtime_ok = aclrtSynchronizeDevice() == ACL_SUCCESS &&
        aclrtMemcpy(output.data(), output.size() * sizeof(half), output_device, output.size() * sizeof(half), ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    uint32_t match_count = 0;
    uint32_t unexpected_count = 0;
    constexpr float EXPECTED_OUTPUT = 12.0f * static_cast<float>(PAD_VALUE);
    for (uint32_t i = 0; i < OUTPUT_ELEMENTS; ++i) {
        const float value = static_cast<float>(output[i]);
        if (std::fabs(value - EXPECTED_OUTPUT) <= 1e-3f) ++match_count;
        else {
            ++unexpected_count;
            if (unexpected_count <= 12) std::cout << "UNEXPECTED index=" << i << " value=" << value << std::endl;
        }
    }
    const bool passed = runtime_ok && match_count == OUTPUT_ELEMENTS && unexpected_count == 0;
    std::cout << "RESULT status=" << (passed ? "PASS" : "FAIL") << " expected=" << EXPECTED_OUTPUT
              << " match_count=" << match_count << '/' << OUTPUT_ELEMENTS
              << " unexpected=" << unexpected_count << std::endl;
    aclrtFree(output_device);
    aclrtFree(weight_device);
    aclrtFree(fmap_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
