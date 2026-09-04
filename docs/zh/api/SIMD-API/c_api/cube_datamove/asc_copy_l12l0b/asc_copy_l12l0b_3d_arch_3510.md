# asc_copy_l12l0b_arch_3510（3D矩阵搬运）

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

3D卷积数据搬运是用于将NC1HWC0格式的Feature Map完成Image to Column展开，按卷积步长、卷积核大小、膨胀系数配置生成img2col二维矩阵，再从展开后的矩阵中选取指定数据块搬运到L0B Buffer，用于3D卷积场景的矩阵计算数据加载。

搬运过程中是以512字节的数据分形为单位进行搬运的，此接口也可以用于普通矩阵计算所需的2D格式数据的搬运。如何使用此接口进行2D格式数据的搬运可以参考[关键特性说明](#l12l0b_3d_key_features)。支持从L1 Buffer到L0B Buffer的数据搬运。

3D img2col搬运模式下，Feature Map的属性描述（fm_w、fm_h及四周padding）需通过[asc_set_l13d_fmatrix_b](../asc_set_l13d_fmatrix_b.md)预先配置，填充值与填充模式需通过[asc_set_l12l0b_3d_padding](../asc_set_l12l0b_3d_padding.md)预先配置；如需使用repeat模式，repeat方向、次数与步长需通过[asc_set_l13d_rpt_b](../asc_set_l3d_rpt_b.md)预先配置。

本接口仅在AIC上执行有效。

## 函数原型

```cpp
__aicore__ inline void asc_copy_l12l0b(__cb__ <dtype>* dst,
                                       __cbuf__ <dtype>* src,
                                       uint16_t k_extension,
                                       uint16_t m_extension,
                                       uint16_t k_start_pt,
                                       uint16_t m_start_pt,
                                       uint8_t stride_w,
                                       uint8_t stride_h,
                                       uint8_t filter_w,
                                       uint8_t filter_h,
                                       uint8_t dilation_filter_w,
                                       uint8_t dilation_filter_h,
                                       bool filter_size_w,
                                       bool filter_size_h,
                                       bool transpose,
                                       bool f_matrix_ctrl,
                                       uint16_t channel_size)
__aicore__ inline void asc_copy_l12l0b_sync(__cb__ <dtype>* dst,
                                            __cbuf__ <dtype>* src,
                                            uint16_t k_extension,
                                            uint16_t m_extension,
                                            uint16_t k_start_pt,
                                            uint16_t m_start_pt,
                                            uint8_t stride_w,
                                            uint8_t stride_h,
                                            uint8_t filter_w,
                                            uint8_t filter_h,
                                            uint8_t dilation_filter_w,
                                            uint8_t dilation_filter_h,
                                            bool filter_size_w,
                                            bool filter_size_h,
                                            bool transpose,
                                            bool f_matrix_ctrl,
                                            uint16_t channel_size)
```

### dtype支持的数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```cpp
// 示例：half类型的3D搬运。
__aicore__ inline void asc_copy_l12l0b(__cb__ half* dst,
                                       __cbuf__ half* src,
                                       uint16_t k_extension,
                                       uint16_t m_extension,
                                       uint16_t k_start_pt,
                                       uint16_t m_start_pt,
                                       uint8_t stride_w,
                                       uint8_t stride_h,
                                       uint8_t filter_w,
                                       uint8_t filter_h,
                                       uint8_t dilation_filter_w,
                                       uint8_t dilation_filter_h,
                                       bool filter_size_w,
                                       bool filter_size_h,
                                       bool transpose,
                                       bool f_matrix_ctrl,
                                       uint16_t channel_size)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| `dst` | 输出 | 目的操作数，存储位置为L0B Buffer。目的地址需512字节对齐。 |
| `src` | 输入 | 源操作数，存储位置为L1 Buffer。源地址需32字节对齐。 |
| `k_extension` | 输入 | 目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于b32类型，应为8的倍数，对于b16类型，应为16的倍数，对于b8类型，应为32的倍数；覆盖的情况则任何数据类型都无倍数要求。取值范围：[1, 65535]。 |
| `m_extension` | 输入 | 目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于b8、b16类型，应为16的倍数，b32类型无要求；覆盖的情况则任何数据类型都无倍数要求。取值范围：[0, 65535]。 |
| `k_start_pt` | 输入 | 目的操作数width维度的起点，对于b32类型，应为8的倍数，对于b16类型，应为16的倍数，对于b8类型，应为32的倍数；取值范围：[0, 65535]。 |
| `m_start_pt` | 输入 | 目的操作数height维度的起点，取值范围：[0, 32767]。 |
| `stride_w` | 输入 | 卷积核在源操作数width维度滑动的步长，取值范围：[1, 63]。 |
| `stride_h` | 输入 | 卷积核在源操作数height维度滑动的步长，取值范围：[1, 63]。 |
| `filter_w` | 输入 | 卷积核width，取值范围：[1, 255]。 |
| `filter_h` | 输入 | 卷积核height，取值范围：[1, 255]。 |
| `dilation_filter_w` | 输入 | 卷积核width膨胀系数，取值范围：[1, 255]。 |
| `dilation_filter_h` | 输入 | 卷积核height膨胀系数，取值范围：[1, 255]。 |
| `filter_size_w` | 输入 | 是否在`filter_w`的基础上将卷积核width增加256个元素。<br>&nbsp;&nbsp;&bull; `true`：增加；<br>&nbsp;&nbsp;&bull; `false`：不增加。 |
| `filter_size_h` | 输入 | 是否在`filter_h`的基础上将卷积核height增加256个元素。<br>&nbsp;&nbsp;&bull; `true`：增加；<br>&nbsp;&nbsp;&bull; `false`：不增加。 |
| `transpose` | 输入 | 是否启用转置功能，本接口对写入L0B Buffer的分形矩阵自动执行转置，该参数被硬件忽略，传入true或false不影响转置行为。 |
| `f_matrix_ctrl` | 输入 | 3D数据搬运Feature Map的属性描述寄存器组选择位。<br>&nbsp;&nbsp;&bull; `true`：从右矩阵中获取Feature Map的属性描述，配合[asc_set_l13d_fmatrix_b](../asc_set_l13d_fmatrix_b.md)、[asc_set_l12l0b_3d_padding](../asc_set_l12l0b_3d_padding.md)、[asc_set_l13d_rpt_b](../asc_set_l3d_rpt_b.md)使用；<br>&nbsp;&nbsp;&bull; `false`：从左矩阵中获取FeatureMap的属性描述，配合[asc_set_l13d_fmatrix](../asc_set_l13d_fmatrix.md)、[asc_set_l12l0a_3d_padding](../asc_set_l12l0a_3d_padding.md)、[asc_set_l13d_rpt](../asc_set_l13d_rpt.md)使用。 |
| `channel_size` | 输入 | 源操作数的通道数N。<br>对于b32类型，`channel_size`除8的余数应当为0或4，最大值可取为65532。<br>对于b16类型，`channel_size`除16的余数应当为0，4或8，最大值可取为65528。<br>对于b8类型，`channel_size`除32的余数应当为0，4，8或16，最大值可取为65520。 |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- dst起始地址需512字节对齐（L0B Buffer对齐要求），否则会导致搬运异常。
- src起始地址需32字节对齐（L1 Buffer对齐要求），否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L0B Buffer容量上限：L0B Buffer总容量64KB，dst偏移量与搬运大小之和不可越界，否则触发异常。
- L1 Buffer容量上限：L1 Buffer总容量512KB，src偏移量与源矩阵占用大小之和不可越界，否则触发异常。

### 3D img2col搬运模式约束

- 此接口必须先调用[asc_set_l13d_rpt_b](../asc_set_l3d_rpt_b.md)接口配置dst_stride参数，dst_stride为输出矩阵在height维度对齐后的大小。
- 此接口需要先调用配套的寄存器设置接口[asc_set_l13d_fmatrix_b](../asc_set_l13d_fmatrix_b.md)与[asc_set_l12l0b_3d_padding](../asc_set_l12l0b_3d_padding.md)预先配置feature map描述、填充值与填充模式，否则会导致搬运接口不符合预期。如需使用repeat模式，须先调用[asc_set_l13d_rpt_b](../asc_set_l3d_rpt_b.md)接口配置repeat方向、次数与步长。
- 3D格式搬运到L0B Buffer时会自动进行转置，transpose参数无效，不支持非转置场景。
- `k_extension`、`m_extension`或`channel_size`为`0`，`filter_w`为`0`且`filter_size_w`为`false`，或`filter_h`为`0`且`filter_size_h`为`false`时不执行搬运，本接口被视为NOP（空操作）。
- 对于b32类型，`channel_size`除8的余数应当为0或4，最大值可取为65532。对于b16类型，`channel_size`除16的余数应当为0，4或8，最大值可取为65528。对于b8类型，`channel_size`除32的余数应当为0，4，8或16，最大值可取为65520。否则会导致搬运结果不符合预期。
- `stride_w`与`stride_h`不可为0，传入0硬件按1处理。
- `stride_w`取值范围为[1, 63]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- `stride_h`取值范围为[1, 63]，超出取值范围的值会被截断，导致搬运结果不符合预期。

## 关键特性说明<a id="l12l0b_3d_key_features"></a>

### 使用3D卷积数据搬运完成矩阵计算所需2D格式数据的搬运

3D数据搬运在b8/b16/b32数据类型下均支持转置，L1 Buffer->L0B Buffer通路自动开启转置搬运，下面按照类型详细说明。

- 对于b8场景：每个分形大小是16×32，搬运示意图如下：

    ![](../../../../figures/load3d_l1tol0b_b8_trans_950.png)

- 对于b16场景：每个分形大小是16×16，搬运示意图如下：

    ![](../../../../figures/load3d_l1tol0b_b16_trans_950.png)

- 对于b32场景：每个分形大小是16×8，搬运示意图如下：

    ![](../../../../figures/load3d_l1tol0b_b32_trans_950.png)

## 调用示例

以下示例使用`asc_copy_l12l0b`将2×2、16通道的全零Feature Map扩展为带一圈padding的4×4矩阵，并自动转置写入L0B Buffer。B侧padding值设置为2，`f_matrix_ctrl`设置为`true`。使用全1的L0A矩阵进行MMAD后，每个输出累加4×4区域中的12个padding位置，期望值为`12 × 2 = 24`。

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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
