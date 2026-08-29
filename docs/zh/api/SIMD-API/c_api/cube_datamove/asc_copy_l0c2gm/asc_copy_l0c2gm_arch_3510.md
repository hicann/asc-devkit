# asc_copy_l0c2gm_arch_3510

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

将矩阵计算结果从L0C Buffer搬运至Global Memory，搬运过程中可同步支持随路量化、随路激活、随路格式转换（Nz2ND/Nz2DN）等能力组合。

下图展示了随路量化、随路ReLU、随路格式转换、随路通道拆分以及随路通道合并的有效组合、中间数据类型和数据路径。下图中的F32-\>F16与F32-\>BF16为Cast，其余为随路scalar/tensor量化模式。

**图1** asc_copy_l0c2gm随路功能组合

![](../../../../figures/C_API_L0C2GM_Function_Combination_950.png)

本接口支持多种随路能力的组合，需通过配套接口预先配置量化参数、激活参数、通道参数等寄存器，再调用本接口完成搬运。

- Nz2ND格式转换场景下，需通过[asc_set_l0c_copy_nz_para](../asc_set_l0c_copy_nz_para.md)预先配置格式转换参数，并且需要搭配本接口`enable_nz2nd`使用；
- Nz2DN格式转换场景下，需通过[asc_set_l0c_copy_nz_para](../asc_set_l0c_copy_nz_para.md)、[asc_set_l0c_copy_channel_para](../asc_set_l0c_copy_channel_para.md)预先配置格式转换参数，并且需要搭配本接口`enable_nz2dn`使用；
- 随路scalar量化模式下，需通过[asc_set_l0c_copy_prequant](../asc_set_l0c_copy_prequant.md)设置随路scalar量化参数，并且需要搭配本接口`quant_pre_mode`使用;
- 随路tensor量化模式下，需通过[asc_set_l0c2gm_config](../asc_set_l0c2gm_config.md)设置随路tensor量化使用tensor的起始地址，其中量化tensor的每个元素都代表一个量化参数，并且需要搭配本接口`quant_pre_mode`使用;
- 随路激活模式下，需通过[asc_set_l0c2gm_relu_alpha](../asc_set_l0c2gm_relu_alpha.md)、[asc_set_l0c2gm_lrelu_alpha](../asc_set_l0c2gm_lrelu_alpha.md)预先配置ReLU/Leaky ReLU激活参数，并且需要搭配本接口`enable_clip_relu_pre`与`relu_pre_mode`使用；

`quant_pre_mode`量化模式参数支持的枚举值如下：

- NoQuant：不开启量化功能。
- DEQF16：int32_t量化成half， scalar量化。
- VDEQF16：int32_t量化成half，tensor量化。
- REQ4：int32_t量化成int4b_t，scalar量化。
- VREQ4：int32_t量化成int4b_t，tensor量化。
- REQ8：int32_t量化成int8_t/uint8_t，scalar量化。
- VREQ8：int32_t量化成int8_t/uint8_t，tensor量化。
- QS322BF16_PRE：int32_t量化成bfloat16_t，scalar量化。
- VQS322BF16_PRE：int32_t量化成bfloat16_t，tensor量化。
- QF322F16_PRE：float量化成half，scalar量化。
- VQF322F16_PRE：float量化成half，tensor量化。
- QF322BF16_PRE：float量化成bfloat16_t，scalar量化。
- VQF322BF16_PRE：float量化成bfloat16_t，tensor量化。
- F322F16：float cast成half，cast mode为CAST_RINT模式。
- F322BF16：float cast成bfloat16_t，cast mode为CAST_RINT模式。
- QF322S4_PRE：float量化成int4b_t，scalar量化。
- VQF322S4_PRE：float量化成int4b_t，tensor量化。
- QF322B8_PRE：float量化成int8_t/uint8_t，scalar量化。
- VQF322B8_PRE：float量化成int8_t/uint8_t，tensor量化。
- QF322FP8_PRE：float量化成fp8_e4m3fn_t，scalar量化。
- VQF322FP8_PRE：float量化成fp8_e4m3fn_t，tensor量化。
- QF322HIF8_PRE：float量化成hifloat8_t(Half to Away Round)，scalar量化。
- VQF322HIF8_PRE：float量化成hifloat8_t(Half to Away Round)，tensor量化。
- QF322HIF8_PRE_HYBRID：float量化成hifloat8_t(Hybrid Round)，scalar量化。
- VQF322HIF8_PRE_HYBRID：float量化成hifloat8_t(Hybrid Round)，tensor量化。
- QF322F32_PRE：float量化成float，scalar量化，精度可以达到双千分之一，无法达到双万分之一。
- VQF322F32_PRE：float量化成float，tensor量化，精度可以达到双千分之一，无法达到双万分之一。

本接口仅在AIC上执行有效。

## 函数原型

```cpp
__aicore__ inline void asc_copy_l0c2gm(__gm__ <dst_dtype>* dst,
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
__aicore__ inline void asc_copy_l0c2gm_sync(__gm__ <dst_dtype>* dst,
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

src dtype与dst dtype支持以下组合：

- `src_dtype`为`int32_t`时，`dst_dtype`支持`int4b_t`、`int8_t`、`uint8_t`、`half`、`bfloat16_t`、`int32_t`。
- `src_dtype`为`float`时，`dst_dtype`支持`int4b_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`float`。

### 函数原型典型示例

```cpp
// 示例：将float类型数据转换为bfloat16_t类型后搬运。
__aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t* dst,
                                       __cc__ float* src,
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

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 含义 |
|---|---|---|
| dst | 输出 | 目的操作数，存储位置为GM。起始地址需要按照1字节对齐。 |
| src | 输入 | 源操作数，存储位置为L0C Buffer。起始地址需要按照64字节对齐。 |
| n_size | 输入 | 源Nz矩阵在N方向上的大小，取值范围为$[1, 2^{12}-1]$。<br>对于Nz输出场景：<br>&nbsp;&nbsp;&bull; 若`enable_channel_split`设置为`true`开启Channel Split功能，`n_size`必须为8的倍数。<br>&nbsp;&nbsp;&bull; 若不开启Channel Split功能，`n_size`必须为16的倍数。<br>对于ND输出场景：<br>&nbsp;&nbsp;&bull; 若`dst_dtype`设置为`int4b_t`，`n_size`必须为2的倍数。<br> |
| m_size | 输入 | 源Nz矩阵在M方向上的大小，取值范围为$[1, 2^{16}-1]$。<br>对于DN输出场景：<br>&nbsp;&nbsp;&bull; 若`dst_dtype`设置为`int4b_t`，`m_size`必须为2的倍数。<br> |
| dst_stride | 输入 | 目的矩阵步长，取值范围为$[1, 2^{32}-1]$。<br>&nbsp;&nbsp;&bull; 若不开启Nz2ND功能，`dst_stride`表示目的Nz矩阵中相邻Z排布的起始地址偏移，单位为元素。<br>&nbsp;&nbsp;&bull; 若开启Nz2ND/Nz2DN功能，`dst_stride`表示目的ND/DN矩阵每一行中的元素个数，单位为元素。<br>对于`dst_dtype`设置为`int4b_t`的输出场景，`dst_stride`必须为2的倍数。 |
| src_stride | 输入 | 源Nz矩阵中相邻Z排布的起始地址偏移，单位为64字节，即$16\times\operatorname{sizeof}(T)$，其中$T$为`src`的数据类型。取值范围为$[0, 2^{16}-1]$。 |
| l2_cache_mode | 输入 | 配置输出GM数据在L2 Cache中的管理策略。取值需为已定义的策略，即0、1、2或4，其他值触发L2 Cache策略异常。取值说明见[表2](#l2_cache_mode_values)。 |
| enable_clip_relu_pre | 输入 | 是否开启Clip ReLU，需搭配Normal ReLU一起使用，且需要开启量化功能，取值如下：<br>&nbsp;&nbsp;&bull; `0`：不开启Clip ReLU。<br>&nbsp;&nbsp;&bull; `1`：开启Clip ReLU（scalar模式）。<br> |
| unit_flag_mode | 输入 | UnitFlag是MMAD类指令和矩阵搬出类指令细粒度的并行功能，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; `0`：不开启UnitFlag。<br>&nbsp;&nbsp;&bull; `2`：开启UnitFlag，硬件执行完指令之后，不复位单元标记位。<br>&nbsp;&nbsp;&bull; `3`：开启UnitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将MMAD类指令和矩阵搬出类指令的UnitFlag值设置为2或3。 |
| quant_pre_mode | 输入 | 预处理阶段量化模式，取值见[功能说明](#功能说明)。 |
| relu_pre_mode | 输入 | 预处理阶段ReLU模式控制，取值如下：<br>&nbsp;&nbsp;&bull; `0`：不开启ReLU。<br>&nbsp;&nbsp;&bull; `1`：开启Normal ReLU。<br>&nbsp;&nbsp;&bull; `2`：开启Scalar ReLU。<br>&nbsp;&nbsp;&bull; `3`：开启Vector ReLU。<br> |
| enable_channel_split | 输入 | 是否开启通道拆分功能。仅在`src_dtype`和`dst_dtype`均为`float`且输出为Nz格式时可开启。<br>&nbsp;&nbsp;&bull; `false`：不开启。<br>&nbsp;&nbsp;&bull; `true`：开启。<br> |
| enable_nz2nd | 输入 | Nz2ND格式转换使能。<br>&nbsp;&nbsp;&bull; `false`：关闭Nz2ND转换。<br>&nbsp;&nbsp;&bull; `true`：开启Nz2ND转换。<br> |
| quant_post | 输入 | 预留参数，当前须设置为0。 |
| relu_post | 输入 | 预留参数，当前须设置为0。 |
| clip_relu_post | 输入 | 预留参数，当前须设置为`false`。 |
| eltwise_op | 输入 | 预留参数，当前须设置为0。 |
| eltwise_antq_en | 输入 | 预留参数，当前须设置为`false`。 |
| c0_pad_en | 输入 | 预留参数，当前须设置为`false`。 |
| broadcast_en | 输入 | 预留参数，当前须设置为`false`。 |
| enable_nz2dn | 输入 | Nz2DN格式转换使能。<br>&nbsp;&nbsp;&bull; `false`：关闭Nz2DN转换。<br>&nbsp;&nbsp;&bull; `true`：开启Nz2DN转换。<br> |

**表2** l2_cache_mode取值说明 <a id="l2_cache_mode_values"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0 | `NORMAL模式` | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级。 |
| 1 | `LAST模式` | &bull; 启用L2 Cache，并将分配的Cache Line标记为低替换优先级。<br>&bull; **LAST模式，功能暂不支持。** |
| 2 | `PERSISTENT模式` | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GM的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。<br>&bull; **PERSISTENT模式，功能暂不支持。** |
| 4 | `DISABLE模式` | 不启用L2 Cache，每次都直接从GM中读取，并保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_FIX

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- `src`起始地址需64字节对齐，否则会导致搬运异常。
- `dst`起始地址需1字节对齐，否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令，详见[核内同步](../../sync/system_sync_overview.md)。
- L0C Buffer容量上限：L0C Buffer总容量256KB，src偏移量与搬运大小之和不可超过L0C Buffer容量，否则触发源地址越界异常。
- Nz矩阵以16×16个元素为一个基本分形。边界分形中超出`n_size`或`m_size`指定范围的数据不属于有效搬运结果。

### 随路转换约束

- `n_size`、`m_size`、`dst_stride`需根据dtype与功能模式确定对齐约束，详见参数说明，不满足对齐约束会导致搬运结果不符合预期。
- src与dst dtype组合需与`quant_pre_mode`量化模式匹配，否则会导致搬运结果不符合预期。
- `enable_channel_split`仅在输出dtype为`float`且输出为Nz格式时可设为true。
- 量化与激活模式中使用的量化系数不可为INF/NaN和非规格化数，否则会导致量化激活结果错误。
- 开启Nz2DN转换时，需通过[asc_set_l0c_copy_channel_para](../asc_set_l0c_copy_channel_para.md)预先配置源矩阵步长，且源矩阵步长不可为0，否则会导致搬运异常。
- 开启Nz2DN转换时，仅当通过[asc_set_l0c_copy_channel_para](../asc_set_l0c_copy_channel_para.md)配置源矩阵步长为1时，可同时开启UnitFlag功能。
- `enable_clip_relu_pre`设为Clip ReLU（标量模式）时需搭配`relu_pre_mode`与量化功能一起使用。

## 调用示例

完整工程样例请参考[data_copy_l0c2gm样例](../../../../../../../examples/02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2gm)。

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

本示例占用32KB L1 Buffer、16KB L0A Buffer、16KB L0B Buffer和64KB L0C Buffer，并使用`EVENT_ID0`完成流水同步。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;

__global__ __cube__ void AscCopyL0c2gmKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];

    // 将两个128x128的ND输入转换为MMAD所需的Nz排布。
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // 将A、B矩阵从L1 Buffer搬入L0A Buffer和L0B Buffer。
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // 执行128x128x128矩阵乘并将结果写入L0C Buffer。
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // 将L0C Buffer中的Nz结果转换为连续ND格式并搬出至GM。
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
}

template <typename T>
void PrintRow(const char* label, const std::vector<T>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << +data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(ELEMENTS), golden(ELEMENTS);
    // 构造非对称输入并在Host侧计算Golden结果。
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = static_cast<int8_t>(static_cast<int32_t>((row + 2 * k) % 5) - 2);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * col + k) % 5) - 2);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                golden[row * N + col] += a[row * K + k] * b[col * K + k];
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    int8_t *a_device = nullptr, *b_device = nullptr;
    int32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyL0c2gmKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_l0c2gm result is correct."
                         : "[Failed] asc_copy_l0c2gm result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
