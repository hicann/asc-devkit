# asc_set_l13d_fmatrix

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

本接口用于配置3D img2col搬运模式下左矩阵的Feature Map属性描述参数，该配置供参数`f_matrix_ctrl`取值为`false`的[asc_copy_l12l0a（3D矩阵搬运）](asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md)接口使用。

本接口为矩阵搬入相关配置接口，仅在AIC上生效。

## 函数原型

```cpp
// 逐参数形式
__aicore__ inline void asc_set_l13d_fmatrix(uint16_t fmatrix_w,
                                            uint16_t fmatrix_h,
                                            uint8_t pad_left,
                                            uint8_t pad_right,
                                            uint8_t pad_top,
                                            uint8_t pad_bottom)
// 位域参数形式
__aicore__ inline void asc_set_l13d_fmatrix(asc_l13d_fmatrix_config& config)
```

## 参数说明

**表1** 接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| fmatrix_w | 输入 | Feature Map的width，取值范围：[1, 32767]。 |
| fmatrix_h | 输入 | Feature Map的height，取值范围：[1, 32767]。 |
| pad_left | 输入 | 左侧padding大小，取值范围：[0, 255]。 |
| pad_right | 输入 | 右侧padding大小，取值范围：[0, 255]。 |
| pad_top | 输入 | 上侧padding大小，取值范围：[0, 255]。 |
| pad_bottom | 输入 | 下侧padding大小，取值范围：[0, 255]。 |
| config | 输入 | Feature Map属性配置，仅传入config位域类型参数的接口使用，详细说明请参见[asc_l13d_fmatrix_config](../defs/union/asc_l13d_fmatrix_config.md)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口仅在AIC上生效，非AIC调用直接返回。
- 本接口必须在对应的3D搬运接口之前调用，3D搬运接口的参数`f_matrix_ctrl`取值为`false`时配置才会生效。
- 调用后配置会持续生效，直至再次调用本接口覆盖，不同3D搬运任务使用的配置不同时，需在对应搬运接口执行前重新配置。

<!-- npu="950" id8 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

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

__global__ __cube__ void conv2d_forward_c_api(
    __gm__ half* fmap, __gm__ half* weight, __gm__ half* output)
{
    asc_init();
    const uint32_t batch_idx = static_cast<uint32_t>(block_idx);
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
        fmap_l1, fmap + batch_idx * FMAP_BATCH_SIZE, CIN * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, H * W, CIN, 0, false);

    asc_set_gm2l1_nz_para(1, 1, K_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(weight_l1, weight, COUT * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, K, COUT, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_set_l13d_fmatrix(W, H, PAD_LEFT, PAD_RIGHT, PAD_TOP, PAD_BOTTOM);
    asc_set_l12l0a_3d_padding(static_cast<half>(PAD_VALUE));
    constexpr uint16_t repeat_stride = 0;
    constexpr uint8_t repeat_time = 1;
    constexpr uint16_t dst_stride = M_ALIGN / CUBE_BLOCK;
    asc_set_l13d_rpt(repeat_stride, repeat_time, asc_l13d_repeat_direction::M_DIRECTION, dst_stride, 0);
    asc_copy_l12l0a(
        fmap_l0, fmap_l1, LOAD3D_K_EXTENSION, M, 0, 0, STRIDE_W, STRIDE_H, KW, KH, DILATION_W, DILATION_H,
        false, false, false, false, CIN);

    for (uint16_t i = 0; i < K_ALIGN / CUBE_BLOCK; ++i) {
        asc_copy_l12l0b_transpose(weight_l0 + i * FRACTAL_ELEMENTS, weight_l1, 0, i, 1, 1, 1, 1);
    }
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_mmad(output_l0, fmap_l0, weight_l0, M, K_ALIGN, N,
        asc_unit_flag_mode::DISABLE, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output + batch_idx * OUTPUT_BATCH_SIZE, output_l0, N, M, N, M_ALIGN,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, asc_quant_mode::F322F16,
        asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_ALL);
}

size_t fmap_index(uint32_t n, uint32_t h, uint32_t w, uint32_t c)
{
    return ((n * H + h) * W + w) * CIN + c;
}

size_t weight_index(uint32_t c1, uint32_t kh, uint32_t kw, uint32_t c0, uint32_t co)
{
    const uint32_t k = ((c1 * KH + kh) * KW + kw) * C0 + c0;
    return k * COUT + co;
}

std::vector<half> make_golden(const std::vector<half>& fmap, const std::vector<half>& weight)
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
                                        : static_cast<float>(fmap[fmap_index(n, hi, wi, ci)]);
                                    acc += x * static_cast<float>(weight[weight_index(c1, kh, kw, c0, co)]);
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
                        weight[weight_index(c1, kh, kw, c0, co)] =
                            static_cast<half>(static_cast<int32_t>((c1 + kh + 2 * kw + c0 + co) % 3) - 1);
                    }
                }
            }
        }
    }
    const std::vector<half> golden = make_golden(fmap, weight);

    half *fmap_device = nullptr, *weight_device = nullptr, *output_device = nullptr;
    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&fmap_device), fmap.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&weight_device), weight.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(fmap_device, fmap.size() * sizeof(half), fmap.data(), fmap.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(weight_device, weight.size() * sizeof(half), weight.data(), weight.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    conv2d_forward_c_api<<<BATCH, 0>>>(fmap_device, weight_device, output_device);
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
<!-- end id8 -->
