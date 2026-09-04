# asc_set_l12l0a_3d_padding

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

用于调用[asc_copy_l12l0a_arch_3510（3D矩阵搬运）](asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md)时设置3D格式分形矩阵搬运（im2col展开）搬运指令使用的填充值。`asc_copy_l12l0a`接口中的`f_matrix_ctrl`参数设置为`false`时，3D格式分形矩阵搬运（im2col展开）场景会使用本接口设置的值做为填充值。

本接口为矩阵3D格式搬入相关配置接口，仅在AIC上生效。

## 函数原型

```c
__aicore__ inline void asc_set_l12l0a_3d_padding(<dtype> padding_value)
__aicore__ inline void asc_set_l12l0a_3d_padding(uint64_t config)
```

### dtype支持的数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：配置half类型的padding值。
__aicore__ inline void asc_set_l12l0a_3d_padding(half padding_value)
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
- 当前接口底层使用的寄存器与[asc_set_gm2l1_padding](asc_set_gm2l1_padding.md)为相同的寄存器，因此本设置的值也是[asc_copy_gm2l1_arch_3510（高维切分数据搬运）](asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md)搬运时使用的填充值。

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
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t BATCH = 4;
constexpr uint32_t C1 = 2;
constexpr uint32_t H = 9;
constexpr uint32_t W = 9;
constexpr uint32_t C0 = 16;
constexpr uint32_t CIN = C1 * C0;
constexpr uint32_t COUT = 16;
constexpr uint32_t KH = 3;
constexpr uint32_t KW = 3;
constexpr uint32_t STRIDE_H = 2;
constexpr uint32_t STRIDE_W = 2;
constexpr uint32_t DILATION_H = 2;
constexpr uint32_t DILATION_W = 2;
constexpr uint32_t PAD_LEFT = 2;
constexpr uint32_t PAD_RIGHT = 2;
constexpr uint32_t PAD_TOP = 2;
constexpr uint32_t PAD_BOTTOM = 2;
constexpr int32_t PAD_VALUE = 0;
constexpr uint32_t HOUT = 5;
constexpr uint32_t WOUT = 5;
constexpr uint32_t M = HOUT * WOUT;
constexpr uint32_t K = CIN * KH * KW;
constexpr uint32_t N = COUT;
constexpr uint32_t CUBE_BLOCK = 16;
constexpr uint32_t M_ALIGN = 32;
constexpr uint32_t K_ALIGN = 288;
constexpr uint32_t N_ALIGN = 16;
constexpr uint32_t FMAP_BATCH_SIZE = H * W * CIN;
constexpr uint32_t WEIGHT_SIZE = K * COUT;
constexpr uint32_t OUTPUT_BATCH_SIZE = HOUT * WOUT * COUT;
constexpr uint32_t FRACTAL_ELEMENTS = 512 / sizeof(half);
constexpr uint32_t LOAD3D_K_EXTENSION = K_ALIGN;

__global__ __cube__ void Conv2dForwardCapi(
    __gm__ half* fmap, __gm__ half* weight, __gm__ half* output)
{
    asc_init();
    const uint32_t batch_idx = static_cast<uint32_t>(asc_get_block_idx());
    if (batch_idx >= BATCH) {
        return;
    }

    __cbuf__ half fmap_l1[FMAP_BATCH_SIZE];
    __cbuf__ half weight_l1[K_ALIGN * N_ALIGN];
    __ca__ half fmap_l0[M_ALIGN * K_ALIGN];
    __cb__ half weight_l0[K_ALIGN * N_ALIGN];
    __cc__ float output_l0[M_ALIGN * N_ALIGN];

    asc_set_gm2l1_nz_para(1, 1, H * W, 0);
    asc_copy_gm2l1_nd2nz(
        fmap_l1, fmap + batch_idx * FMAP_BATCH_SIZE, CIN * sizeof(half), 0, H * W, CIN, 0, false);

    asc_set_gm2l1_nz_para(1, 1, K_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(weight_l1, weight, COUT * sizeof(half), 0, K, COUT, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_l13d_fmatrix_config fmatrix_config;
    fmatrix_config.l1_width = W;
    fmatrix_config.l1_height = H;
    fmatrix_config.padding_left_size = PAD_LEFT;
    fmatrix_config.padding_right_size = PAD_RIGHT;
    fmatrix_config.padding_top_size = PAD_TOP;
    fmatrix_config.padding_bottom_size = PAD_BOTTOM;
    asc_set_l13d_fmatrix(fmatrix_config);
    asc_set_l12l0a_3d_padding(static_cast<half>(PAD_VALUE));
    constexpr uint16_t repeat_stride = 0;
    constexpr uint8_t repeat_time = 1;
    constexpr uint8_t repeat_mode = 0;
    constexpr uint16_t dst_stride = M_ALIGN / CUBE_BLOCK;
    asc_load3d_v2_config repeat_config;
    // SetLoadDataRepeatWithStride encodes dstStride in bits [47:32].
    repeat_config.config = static_cast<uint64_t>(repeat_stride) |
        (static_cast<uint64_t>(repeat_time) << 16) |
        (static_cast<uint64_t>(repeat_mode) << 24) |
        (static_cast<uint64_t>(dst_stride) << 32);
    asc_set_l13d_rpt(repeat_config);
    asc_copy_l12l0a(
        fmap_l0, fmap_l1, LOAD3D_K_EXTENSION, M, 0, 0, STRIDE_W, STRIDE_H, KW, KH, DILATION_W, DILATION_H,
        false, false, false, false, CIN);

    for (uint16_t i = 0; i < K_ALIGN / CUBE_BLOCK; ++i) {
        asc_copy_l12l0b_transpose(weight_l0 + i * FRACTAL_ELEMENTS, weight_l1, 0, i, 1, 1, 1, 1);
    }
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_mmad(output_l0, fmap_l0, weight_l0, M, K_ALIGN, N, 0, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(
        output + batch_idx * OUTPUT_BATCH_SIZE, output_l0, N, M, N, M_ALIGN, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::F322F16), 0, false, true, 0, 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_ALL);
}

size_t FmapIndex(uint32_t n, uint32_t h, uint32_t w, uint32_t c)
{
    return ((n * H + h) * W + w) * CIN + c;
}

size_t WeightIndex(uint32_t c1, uint32_t kh, uint32_t kw, uint32_t c0, uint32_t co)
{
    const uint32_t k = ((c1 * KH + kh) * KW + kw) * C0 + c0;
    return k * COUT + co;
}

std::vector<half> MakeGolden(const std::vector<half>& fmap, const std::vector<half>& weight)
{
    std::vector<half> golden(BATCH * OUTPUT_BATCH_SIZE);
    for (uint32_t n = 0; n < BATCH; ++n) {
        for (uint32_t ho = 0; ho < HOUT; ++ho) {
            for (uint32_t wo = 0; wo < WOUT; ++wo) {
                for (uint32_t co = 0; co < COUT; ++co) {
                    float acc = 0.0f;
                    for (uint32_t c1 = 0; c1 < C1; ++c1) {
                        for (uint32_t kh = 0; kh < KH; ++kh) {
                            for (uint32_t kw = 0; kw < KW; ++kw) {
                                const int32_t hi = static_cast<int32_t>(ho * STRIDE_H + kh * DILATION_H) - PAD_TOP;
                                const int32_t wi = static_cast<int32_t>(wo * STRIDE_W + kw * DILATION_W) - PAD_LEFT;
                                for (uint32_t c0 = 0; c0 < C0; ++c0) {
                                    const uint32_t ci = c1 * C0 + c0;
                                    const float x = hi < 0 || hi >= static_cast<int32_t>(H) || wi < 0 ||
                                            wi >= static_cast<int32_t>(W)
                                        ? static_cast<float>(PAD_VALUE)
                                        : static_cast<float>(fmap[FmapIndex(n, hi, wi, ci)]);
                                    acc += x * static_cast<float>(weight[WeightIndex(c1, kh, kw, c0, co)]);
                                }
                            }
                        }
                    }
                    golden[((n * HOUT + ho) * WOUT + wo) * COUT + co] = static_cast<half>(acc);
                }
            }
        }
    }
    return golden;
}
} // namespace

int main()
{
    std::vector<half> fmap(BATCH * FMAP_BATCH_SIZE);
    std::vector<half> weight(WEIGHT_SIZE);
    std::vector<half> output(BATCH * OUTPUT_BATCH_SIZE);
    for (size_t i = 0; i < fmap.size(); ++i) {
        fmap[i] = static_cast<half>(static_cast<int32_t>(i % 5) - 2);
    }
    for (uint32_t c1 = 0; c1 < C1; ++c1) {
        for (uint32_t kh = 0; kh < KH; ++kh) {
            for (uint32_t kw = 0; kw < KW; ++kw) {
                for (uint32_t c0 = 0; c0 < C0; ++c0) {
                    for (uint32_t co = 0; co < COUT; ++co) {
                        weight[WeightIndex(c1, kh, kw, c0, co)] =
                            static_cast<half>(static_cast<int32_t>((c1 + kh + 2 * kw + c0 + co) % 3) - 1);
                    }
                }
            }
        }
    }
    const std::vector<half> golden = MakeGolden(fmap, weight);

    half *fmap_device = nullptr, *weight_device = nullptr, *output_device = nullptr;
    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&fmap_device), fmap.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&weight_device), weight.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(fmap_device, fmap.size() * sizeof(half), fmap.data(), fmap.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(weight_device, weight.size() * sizeof(half), weight.data(), weight.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    Conv2dForwardCapi<<<BATCH, 0>>>(fmap_device, weight_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(half), output_device, output.size() * sizeof(half), ACL_MEMCPY_DEVICE_TO_HOST);

    size_t mismatch_count = 0;
    float max_diff = 0.0f;
    for (size_t i = 0; i < output.size(); ++i) {
        const float diff = std::fabs(static_cast<float>(output[i]) - static_cast<float>(golden[i]));
        max_diff = diff > max_diff ? diff : max_diff;
        mismatch_count += diff > 1e-3f;
    }
    std::cout << "Example 1: max diff=" << max_diff
              << ", mismatches=" << mismatch_count << '/' << output.size() << std::endl;
    std::cout << (mismatch_count == 0 ? "[Success] conv2d_forward C API passed."
                                     : "[Failed] conv2d_forward C API mismatch.") << std::endl;

    aclrtFree(fmap_device);
    aclrtFree(weight_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return mismatch_count == 0 ? 0 : 1;
}

```
