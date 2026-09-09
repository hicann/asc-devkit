# asc_copy_l0c2l1

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

将矩阵计算结果从L0C Buffer搬运到L1 Buffer。搬运过程中可组合随路量化、ReLU、Nz2ND格式转换和通道拆分能力，具体支持的dtype组合及参数以函数原型为准。

下图展示了随路量化、随路ReLU、随路格式转换、随路通道拆分以及随路通道合并的有效组合、中间数据类型和数据路径，通过连线颜色区分支持的场景组合。下图中的F32-\>F16与F32-\>BF16为Cast，其余为随路scalar/tensor量化模式。

**图1** asc_copy_l0c2l1随路功能组合

![](../../../../figures/C_API_L0C2L1_Function_Combination_950.png)

本接口支持多种随路能力的组合，需通过配套接口预先配置量化参数、激活参数、通道参数等寄存器，再调用本接口完成搬运。

- Nz2ND格式转换场景下，需通过[asc_set_l0c_copy_nz_para](../asc_set_l0c_copy_nz_para.md)预先配置格式转换参数，并且需要搭配本接口`enable_nz2nd`使用；
- Nz2DN格式转换场景下，需通过[asc_set_l0c_copy_nz_para](../asc_set_l0c_copy_nz_para.md)、[asc_set_l0c_copy_channel_para](../asc_set_l0c_copy_channel_para.md)预先配置格式转换参数，并且需要搭配本接口`enable_nz2dn`使用；
- 随路scalar量化模式下，需通过[asc_set_l0c_copy_prequant](../asc_set_l0c_copy_prequant.md)设置随路scalar量化参数，并且需要搭配本接口`quant_pre_mode`使用;
- 随路tensor量化模式下，需通过[asc_set_l0c2gm_config](../asc_set_l0c2gm_config.md)设置随路tensor量化使用tensor的起始地址，其中量化tensor的每个元素都代表一个量化参数，并且需要搭配本接口`quant_pre_mode`使用;
- 随路激活模式下，需通过[asc_set_l0c2gm_relu_alpha](../asc_set_l0c2gm_relu_alpha.md)、[asc_set_l0c2gm_lrelu_alpha](../asc_set_l0c2gm_lrelu_alpha.md)预先配置ReLU/Leaky ReLU激活参数，并且需要搭配本接口`enable_clip_relu_pre`与`relu_pre_mode`使用。

quant_pre_mode量化模式参数支持的枚举值如下：

- NoQuant：不开启量化功能
- DEQF16：int32_t量化成half， scalar量化
- VDEQF16：int32_t量化成half，tensor量化
- REQ4：int32_t量化成int4b_t，scalar量化
- VREQ4：int32_t量化成int4b_t，tensor量化
- REQ8：int32_t量化成int8_t/uint8_t，scalar量化
- VREQ8：int32_t量化成int8_t/uint8_t，tensor量化
- QS322BF16_PRE：int32_t量化成bfloat16_t，scalar量化
- VQS322BF16_PRE：int32_t量化成bfloat16_t，tensor量化
- QF322F16_PRE：float量化成half，scalar量化
- VQF322F16_PRE：float量化成half，tensor量化
- QF322BF16_PRE：float量化成bfloat16_t，scalar量化
- VQF322BF16_PRE：float量化成bfloat16_t，tensor量化
- F322F16：float cast成half，cast mode为CAST_RINT模式
- F322BF16：float cast成bfloat16_t，cast mode为CAST_RINT模式
- QF322S4_PRE：float量化成int4b_t，scalar量化
- VQF322S4_PRE：float量化成int4b_t，tensor量化
- QF322B8_PRE：float量化成int8_t/uint8_t，scalar量化
- VQF322B8_PRE：float量化成int8_t/uint8_t，tensor量化
- QF322FP8_PRE：float量化成fp8_e4m3fn_t，scalar量化
- VQF322FP8_PRE：float量化成fp8_e4m3fn_t，tensor量化
- QF322HIF8_PRE：float量化成hifloat8_t(Half to Away Round)，scalar量化
- VQF322HIF8_PRE：float量化成hifloat8_t(Half to Away Round)，tensor量化
- QF322HIF8_PRE_HYBRID：float量化成hifloat8_t(Hybrid Round)，scalar量化
- VQF322HIF8_PRE_HYBRID：float量化成hifloat8_t(Hybrid Round)，tensor量化
- QF322F32_PRE：float量化成float，scalar量化，精度可以达到双千分之一，无法达到双万分之一。
- VQF322F32_PRE：float量化成float，tensor量化，精度可以达到双千分之一，无法达到双万分之一。双万分之一是指每个实际数据和真值数据之间的误差不超过万分之一，误差超过万分之一的数据总和不超过总数据数的万分之一。

本接口仅在AIC上执行有效。

## 函数原型

```c
// 占位符形式
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t enable_clip_relu_pre,
                                       uint8_t unit_flag_mode,
                                       uint64_t quant_pre_mode,
                                       uint8_t relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       uint64_t quant_post,
                                       uint8_t relu_post,
                                       bool clip_relu_post,
                                       uint8_t eltwise_op,
                                       uint8_t eltwise_antq_en,
                                       bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int4b_t* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t l2_cache_mode,
                                       uint8_t enable_clip_relu_pre,
                                       uint8_t unit_flag_mode,
                                       uint64_t quant_pre_mode,
                                       uint8_t relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       uint64_t quant_post,
                                       uint8_t relu_post,
                                       bool clip_relu_post,
                                       uint8_t eltwise_op,
                                       bool eltwise_antq_en,
                                       bool c0_pad_en,
                                       bool broadcast_en,
                                       bool enable_nz2dn)
__aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ int4b_t* dst,
                                            __cc__ <src_dtype>* src,
                                            uint16_t n_size,
                                            uint16_t m_size,
                                            uint32_t dst_stride,
                                            uint16_t src_stride,
                                            uint8_t l2_cache_mode,
                                            uint8_t enable_clip_relu_pre,
                                            uint8_t unit_flag_mode,
                                            uint64_t quant_pre_mode,
                                            uint8_t relu_pre_mode,
                                            bool enable_channel_split,
                                            bool enable_nz2nd,
                                            uint64_t quant_post,
                                            uint8_t relu_post,
                                            bool clip_relu_post,
                                            uint8_t eltwise_op,
                                            bool eltwise_antq_en,
                                            bool c0_pad_en,
                                            bool broadcast_en,
                                            bool enable_nz2dn)
```

### dtype支持的数据类型

- 普通原型中，`src_dtype`为`int32_t`时，`dst_dtype`支持`half`、`int8_t`、`uint8_t`、`int32_t`；`src_dtype`为`float`时，`dst_dtype`支持`half`、`bfloat16_t`、`int8_t`、`uint8_t`、`float`。
- 带`l2_cache_mode`及同步版本的原型中，`dst_dtype`为`int4b_t`，`src_dtype`支持`int32_t`、`float`。

### 函数原型典型示例

```c
// 示例：int8_t类型的搬运
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst,
                                       __cc__ int32_t* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t enable_clip_relu_pre,
                                       uint8_t unit_flag_mode,
                                       uint64_t quant_pre_mode,
                                       uint8_t relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       uint64_t quant_post,
                                       uint8_t relu_post,
                                       bool clip_relu_post,
                                       uint8_t eltwise_op,
                                       uint8_t eltwise_antq_en,
                                       bool c0_pad_en)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|:-----------------|:------|:-----------------------------------|
| dst | 输出 | 目的操作数，存储位置为L1 Buffer。数据格式为Nz、ND或DN格式，起始地址需32字节对齐。 |
| src | 输入 | 源操作数，存储位置为L0C Buffer。数据格式为Nz格式，起始地址需64字节对齐。 |
| n_size | 输入 | 源Nz矩阵在N方向上的大小，取值范围：[1, 4095]。<br>对于Nz输出场景：<br>&nbsp;&nbsp;&bull; 若`enable_channel_split`设置为`true`开启Channel Split功能，`n_size`必须为8的倍数。<br>&nbsp;&nbsp;&bull; 若不开启Channel Split功能，`n_size`必须为16的倍数。<br>对于ND输出场景：<br>&nbsp;&nbsp;&bull; $n\_size\times\operatorname{sizeof}(dst\_dtype)$必须为32的倍数；若`dst_dtype`设置为`int4b_t`，`n_size`必须为64的倍数。<br> |
| m_size | 输入 | 源Nz矩阵在M方向上的大小。<br>&nbsp;&nbsp;&bull; 不开启Nz2ND功能，取值范围：[1, 65535]；<br>&nbsp;&nbsp;&bull; 开启Nz2ND功能，取值范围：[1, 8192]。<br>&nbsp;&nbsp;&bull;对于DN输出场景：$m\_size\times\operatorname{sizeof}(dst\_dtype)$必须为32的倍数；若`dst_dtype`设置为`int4b_t`，`m_size`必须为64的倍数。<br>  |
| dst_stride | 输入 | &nbsp;&nbsp;&bull; 若不开启Nz2ND功能，dst_stride表示目的Nz矩阵中相邻Z排布的起始地址偏移，取值不为0，单位：element。<br>&nbsp;&nbsp;&bull; 若开启Nz2ND/Nz2DN功能，dst_stride表示目的ND矩阵每一行中的元素个数，取值不为0，单位：element对于不同`dst_dtype`的对齐约束如下：`int4b_t`输出需为64的倍数，8位输出需为32的倍数，16位输出需为16的倍数，32位输出需为8的倍数。<br> |
| src_stride | 输入 | 源Nz矩阵中相邻Z排布的起始地址偏移，取值范围：[0, 65535]，单位为64字节，即$16\times\operatorname{sizeof}(src\_dtype)$。 |
| l2_cache_mode | 输入 | 无效参数，用户无需关注，传入0即可。 |
| enable_clip_relu_pre | 输入 | 是否开启Clip ReLU，需搭配Normal ReLU一起使用，且需要开启量化功能，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启Clip ReLU；<br>&nbsp;&nbsp;&bull; 1：开启Clip ReLU（scalar 模式）。 |
| unit_flag_mode | 输入 | unit_flag是mmad类指令和矩阵搬出类指令细粒度的并行功能，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0：不开启unit_flag；<br>&nbsp;&nbsp;&bull; 2：开启unit_flag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将mmad类指令和矩阵搬出类指令的unit_flag值设置为2或3。 |
| quant_pre_mode | 输入 | 预处理阶段量化参数。取值见[功能说明](#功能说明)。 |
| relu_pre_mode | 输入 | 预处理阶段ReLU模式控制，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启ReLU；<br>&nbsp;&nbsp;&bull; 1：开启Normal ReLU；<br>&nbsp;&nbsp;&bull; 2：开启Scalar ReLU；<br>&nbsp;&nbsp;&bull; 3：开启Vector ReLU。 |
| enable_channel_split | 输入 | 是否开启通道拆分的功能。仅在src和dst都为float时才能开启Channel Split，且不能同时开启Channel Split和Nz2ND功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |
| enable_nz2nd | 输入 | 是否开启Nz2ND功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |
| quant_post | 输入 | 无效参数，用户无需关注，传入0即可。 |
| relu_post | 输入 | 无效参数，用户无需关注，传入0即可。 |
| clip_relu_post | 输入 | 无效参数，用户无需关注，传入false即可。 |
| eltwise_op | 输入 | 无效参数，用户无需关注，传入0即可。 |
| eltwise_antq_en | 输入 | 无效参数，用户无需关注，传入false或0即可。 |
| c0_pad_en | 输入 | 无效参数，用户无需关注，传入false即可。 |
| broadcast_en | 输入 | 无效参数，用户无需关注，传入false即可。 |
| enable_nz2dn | 输入 | 是否开启Nz2DN功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |

## 返回值说明

无

## 流水类型

PIPE_FIX

## 约束说明

### 通用约束

- 本接口仅在AIC上生效，非AIC调用直接返回。
- `dst`起始地址需32字节对齐（L1 Buffer对齐要求），否则会导致搬运异常。
- `src`起始地址需64字节对齐（L0C Buffer对齐要求，`src`为`int32_t`或`float`时），否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L0C Buffer容量上限：L0C Buffer总容量256KB，`src`偏移加搬运大小不可越界，否则触发异常。
- L1 Buffer容量上限：L1 Buffer总容量512KB，`dst`偏移加搬运大小不可越界，否则触发异常。

### 随路转换约束

- `n_size`、`m_size`、`dst_stride`需根据dtype与功能模式确定对齐约束，详见参数说明，不满足对齐约束会导致搬运结果不符合预期。
- src与dst dtype组合需与`quant_pre_mode`量化模式匹配，否则会导致搬运结果不符合预期。
- `enable_channel_split`仅在输出dtype为`float`且输出为Nz格式时可设为true。
- 量化与激活模式中使用的量化系数不可为INF/NAN和非规格化数，否则会导致量化激活结果错误。
- 开启Nz2DN转换时，需通过[asc_set_l0c_copy_channel_para](../asc_set_l0c_copy_channel_para.md)预先配置源矩阵步长，且源矩阵步长不可为0，否则会导致搬运异常。
- 开启Nz2DN转换时，仅当通过[asc_set_l0c_copy_channel_para](../asc_set_l0c_copy_channel_para.md)配置源矩阵步长为1时，可同时开启UnitFlag功能。
- `enable_clip_relu_pre`设为Clip ReLU（标量模式）时需搭配`relu_pre_mode`与量化功能一起使用。

## 调用示例

完整工程样例请参考[data_copy_l0c2l1样例](../../../../../../../examples/02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2l1)。

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

本示例固定`M=N=K=128`，通过MMAD生成L0C Buffer结果，调用本接口转换为ND格式并搬至L1 Buffer，随后经UB回传GM与Host侧Golden结果比较。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t M = 128, N = 128, K = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr int64_t READY_FLAG = 11;

__global__ __mix__(1, 2) void CopyL0c2l1Kernel(
    __gm__ int8_t* a, __gm__ int8_t* b, __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __cbuf__ int32_t output_l1[ELEMENTS];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];
    __ubuf__ int32_t* output_ub = reinterpret_cast<__ubuf__ int32_t*>(0);
    if ASC_IS_AIC {
        asc_set_gm2l1_nz_para(1, 1, M, 0);
        asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
        asc_set_gm2l1_nz_para(1, 1, K, 0);
        asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / 16, K / 32, M / 16, M / 16);
        asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / 16, K / 32, N / 16, N / 16);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c_copy_nz_para(1, 0, 0);
        asc_copy_l0c2l1(output_l1, c_l0, N, M, N, M, 0, 0,
            static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true, 0, 0, false, 0, 0, false);
        asc_sync_notify(PIPE_FIX, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_FIX, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(output_ub, output_l1, 0, 1, ELEMENTS * sizeof(int32_t) / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_block_arrive(PIPE_MTE1, READY_FLAG);
    }
    if ASC_IS_AIV {
        asc_sync_block_wait(PIPE_MTE3, READY_FLAG);
        if (asc_get_sub_block_id() == 0)
            asc_copy_ub2gm_align(output, output_ub, ELEMENTS * sizeof(int32_t));
    }
    asc_sync_pipe(PIPE_ALL);
}
} // namespace

int main()
{
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t r = 0; r < M; ++r) for (uint32_t k = 0; k < K; ++k)
        a[r * K + k] = static_cast<int8_t>(static_cast<int32_t>((r + 2 * k) % 5) - 2);
    for (uint32_t c = 0; c < N; ++c) for (uint32_t k = 0; k < K; ++k)
        b[c * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * c + k) % 5) - 2);
    for (uint32_t r = 0; r < M; ++r) for (uint32_t c = 0; c < N; ++c)
        for (uint32_t k = 0; k < K; ++k) golden[r * N + c] += a[r * K + k] * b[c * K + k];
    aclInit(nullptr); aclrtSetDevice(0);
    int8_t *ad = nullptr, *bd = nullptr; int32_t* od = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&ad), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&bd), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&od), output.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(ad, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(bd, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    CopyL0c2l1Kernel<<<1, 0>>>(ad, bd, od);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), od,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = ret == ACL_SUCCESS && output == golden;
    std::printf("M=%u N=%u K=%u asc_copy_l0c2l1=%s\n", M, N, K, passed ? "PASS" : "FAIL");
    aclrtFree(ad); aclrtFree(bd); aclrtFree(od); aclrtResetDevice(0); aclFinalize();
    return passed ? 0 : 1;
}
```
