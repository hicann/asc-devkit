# asc_copy_gm2l1_align

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

将数据从Global Memory搬运到L1 Buffer，提供非32B对齐搬运能力，搬运过程中对支持对数据进行padding。

若搬运数据长度非32字节对齐，会将搬运数据补齐至32字节对齐。可通过配置参数`burst_dst_stride`选择Normal模式或Compact模式。非32字节对齐场景支持以下两种填充方式：

- 手动填充：搬运前调用[asc_set_gm2l1_pad](asc_set_gm2l1_pad.md)设置填充值，并将`data_select_bit`设置为`true`。
- 自动填充：将`data_select_bit`设置为`false`，由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

数据搬运模式说明如下：

- Compact模式：将`burst_dst_stride`设置为`len_burst`，并将`left_padding_count`和`right_padding_count`设置为0。多个数据块在L1中紧密排列，在32字节补齐处理上被视为一个整体；若所有数据块的总长度，即`n_burst *len_burst`非32字节对齐，则仅在整体末尾通过上述手动或自动填充方式补齐至32字节对齐。填充数据的数据量为`32 - n_burst * len_burst % 32`，单位为字节。
- Normal模式：将`burst_dst_stride`设置为不等于`len_burst`且满足32字节对齐的值。相邻目的数据块的首地址间隔由`burst_dst_stride`指定，数据块之间可以预留间隔，无需紧密排列。
  - 当`left_padding_count`和`right_padding_count`均为0时，通过上述手动或自动填充方式将每个数据块分别补齐至32字节对齐。每个数据块填充数据的数据量为`32 - len_burst % 32`，单位为字节。
  - 当`left_padding_count`或`right_padding_count`非0时，即为左右填充模式。`data_select_bit`不生效，必须在搬运前调用`asc_set_gm2l1_pad`配置填充值。若数据块长度与左右Padding的总字节数非32字节对齐，硬件会在右侧继续填充dummy假数据至32字节对齐，dummy假数据的值为`asc_set_gm2l1_pad`配置的值。

例如，搬运3个`len_burst`为48字节的数据块时：

- Normal模式下将`burst_dst_stride`设置为64，各数据块在L1中的起始偏移依次为0、64、128，每个数据块分别补充16字节填充数据。

  **图1** Normal填充模式示意图

  ![Normal填充模式示意图](../../../figures/asc_copy_gm2l1_align_normal_padding.png "Normal填充模式示意图")

- Compact模式下将`burst_dst_stride`设置为48，各数据块在L1中的起始偏移依次为0、48、96，仅在144字节有效数据的末尾补充16字节填充数据。

  **图2** Compact填充模式示意图

  ![Compact填充模式示意图](../../../figures/asc_copy_gm2l1_align_compact_padding.png "Compact填充模式示意图")

当只搬运1个数据块，或`len_burst`已经32字节对齐且无左右Padding时，两种模式的搬运结果相同。

循环填充模式即支持多次循环调用Normal填充模式与Compact填充模式，不支持多次循环调用左右填充模式。必须配合[asc_set_gm2l1_loop_size](asc_set_gm2l1_loop_size.md)、[asc_set_gm2l1_loop1_stride](asc_set_gm2l1_loop1_stride.md)、[asc_set_gm2l1_loop2_stride](asc_set_gm2l1_loop2_stride.md)使用，通过上述接口配置loop1/loop2的搬运次数与源/目步长。

本接口仅在AIC上执行有效。

## 函数原型

```c
// 占位符形式
__aicore__ inline void asc_copy_gm2l1_align(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t n_burst,
                                            uint32_t len_burst,
                                            uint8_t left_padding_count,
                                            uint8_t right_padding_count,
                                            bool data_select_bit,
                                            uint8_t l2_cache_ctl,
                                            uint64_t burst_src_stride,
                                            uint32_t burst_dst_stride)
// 占位符形式
__aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ <dtype>* dst,
                                                 __gm__ <dtype>* src,
                                                 uint32_t n_burst,
                                                 uint32_t len_burst,
                                                 uint8_t left_padding_count,
                                                 uint8_t right_padding_count,
                                                 bool data_select_bit,
                                                 uint8_t l2_cache_ctl,
                                                 uint64_t burst_src_stride,
                                                 uint32_t burst_dst_stride)
```

### dtype支持的数据类型

dtype取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：int8_t类型的搬运
__aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int8_t* dst,
                                            __gm__ int8_t* src,
                                            uint32_t n_burst,
                                            uint32_t len_burst,
                                            uint8_t left_padding_count,
                                            uint8_t right_padding_count,
                                            bool data_select_bit,
                                            uint8_t l2_cache_ctl,
                                            uint64_t burst_src_stride,
                                            uint32_t burst_dst_stride)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数，存储位置为L1 Buffer。目的地址需32字节对齐。 |
| src | 输入 | 源操作数，存储位置为GM。源地址需1字节对齐。 |
| n_burst | 输入 | 待搬运的连续数据块个数。取值范围：[1, $2^{21}-1$]。 |
| len_burst | 输入 | 待搬运的每个连续数据块的长度，单位字节。取值范围：[1, $2^{21}-1$]。需满足dtype字节对齐：dtype为`b16`类型时len_burst需为2的倍数，dtype为`b32`时len_burst需为4的倍数。 |
| left_padding_count | 输入 | 左侧填充元素个数。填充字节数需满足`left_padding_count × sizeof(dtype) ≤ 32`字节，对应dtype上限：dtype为`b8`时取值范围：[0, 32]，dtype为`b16`时取值范围：[0, 16]，dtype为`b32`时取值范围：[0, 8]。 |
| right_padding_count | 输入 | 右侧填充元素个数。填充字节数需满足`right_padding_count × sizeof(dtype) ≤ 32`字节，对应dtype上限：dtype为`b8`时取值范围：[0, 32]，dtype为`b16`时取值范围：[0, 16]，dtype为`b32`时取值范围：[0, 8]。 |
| data_select_bit | 输入 | 填充数据来源选择。<br>&nbsp;&nbsp;&bull; `false`：首元素填充模式，填充数据取每个burst的首元素；<br>&nbsp;&nbsp;&bull; `true`：常量填充模式，填充数据取[asc_set_gm2l1_pad](asc_set_gm2l1_pad.md)预先配置的填充值。当`left_padding_count`与`right_padding_count`至少有一个非0时，硬件强制使用预先配置的常量填充值，本参数设置无效。 |
| l2_cache_ctl | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表2](#l2_cache_ctl_values)。 |
| burst_src_stride | 输入 | 相邻连续数据块在源端（Global Memory）的步长，单位字节。取值范围：[1, $2^{40}-1$]。 |
| burst_dst_stride | 输入 | 相邻连续数据块在目的端（L1 Buffer）的步长，单位字节。取值范围：[1, $2^{21}-1$]。当`burst_dst_stride`不等于`len_burst`时（即存在padding），`burst_dst_stride`需32字节对齐，否则触发异常（即使`n_burst`为1）。 |

**表2** `l2_cache_ctl`取值说明 <a id="l2_cache_ctl_values"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0 | Normal模式 | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级。 |
| 1 | Last模式 | &bull; 启用L2 Cache，并将分配的Cache Line标记为低替换优先级。<br>&bull; **Last模式暂不支持。**|
| 2 | Persistent模式 | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GM的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 被标记为驻留模式的Cache Line只能被其他同样标记为驻留模式的Cache Line替换。<br>&bull; **Persistent模式暂不支持。**|
| 4 | Disable模式 | 不启用L2 Cache，每次都直接从GM中读取，并保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- `dst`起始地址需32字节对齐（L1 Buffer对齐要求），否则触发异常。
- `src`起始地址需1字节对齐（Global Memory对齐要求），不满足触发异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L1 Buffer容量上限：L1 Buffer总容量512KB，dst偏移与搬运大小之和不可越界，否则触发目的地址越界异常。
- `len_burst`、`n_burst`、`burst_src_stride`、`burst_dst_stride`、`left_padding_count`与`right_padding_count`需满足参数说明的取值范围，否则会导致搬运结果不符合预期。
- `data_select_bit`设置为true时，或者左右填充模式（`left_padding_count`和`right_padding_count`任意不为0）时，须先调用[asc_set_gm2l1_pad](asc_set_gm2l1_pad.md)配置填充值。

### Normal填充模式约束

- 当`burst_dst_stride`不等于`len_burst`时，`burst_dst_stride`需32字节对齐，否则触发异常，该约束即使`n_burst`为1仍生效。

### 左右填充模式约束

- 左右填充模式下，`left_padding_count×sizeof(dtype) + right_padding_count×sizeof(dtype) + len_burst + pad`需32字节对齐，其中`pad`为不满足32字节对齐需要填充的部分，填充值不受`data_select_bit`控制，必须先调用[asc_set_gm2l1_pad](asc_set_gm2l1_pad.md)配置填充值。

### 循环填充模式约束

- 循环填充模式不能和左右填充模式同时开启。
- 循环填充模式`loop1 dst stride`与`loop2 dst stride`必须32字节对齐，该约束即使loop size为1仍生效。

## 调用示例

以下示例一次覆盖三种填充模式。三个场景均搬运3个48字节数据块：Normal场景每块的尾部补齐16字节，Compact场景只在144字节有效数据的整体尾部补齐16字节，左右填充场景在每块的左侧填充4字节、右侧填充12字节。示例使用物理L1/UB地址在AIC和AIV间传递结果，并在Host侧逐字节比对结果。

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t BURST_COUNT = 3;
constexpr uint32_t BURST_BYTES = 48;
constexpr uint32_t NORMAL_STRIDE = 64;
constexpr uint32_t INPUT_BYTES = BURST_COUNT * BURST_BYTES;
constexpr uint32_t NORMAL_OUTPUT_BYTES = BURST_COUNT * NORMAL_STRIDE;
constexpr uint32_t COMPACT_OUTPUT_BYTES = 160;
constexpr uint8_t PAD_BYTE = 0xA5;
constexpr uint32_t PAD_DWORD = 0xA5A5A5A5;
constexpr uint8_t LEFT_PADDING = 4;
constexpr uint8_t RIGHT_PADDING = 12;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

enum class PaddingMode : uint32_t { NORMAL, COMPACT, LEFT_RIGHT };

__global__ __mix__(1, 2) void asc_copy_gm2l1_align_modes_kernel(  // __mix__：一个AIC和两个AIV协同执行。
    __gm__ uint8_t* output, __gm__ uint8_t* input, uint32_t mode)
{
    asc_init();
    // AIC写L1、AIV写GM；跨核共享缓冲区使用物理地址，不能与数组形式的局部缓冲区混用。
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(NORMAL_OUTPUT_BYTES);
    uint32_t output_bytes = NORMAL_OUTPUT_BYTES;

    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            // 初始化AIV侧物理UB，并确保其MTE2操作完成后再通知AIC。
            asc_copy_gm2ub(staging_ub, input, INPUT_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_intra_arrive(PIPE_MTE3, AIV_TO_AIC_SYNC_ID);
        }
        asc_sync_block_arrive(PIPE_MTE3, CROSS_CORE_FLAG_ID);
        if (is_output_core) {
            output_bytes = mode == static_cast<uint32_t>(PaddingMode::COMPACT) ?
                COMPACT_OUTPUT_BYTES : NORMAL_OUTPUT_BYTES;
            asc_sync_intra_wait(PIPE_MTE3, AIC_TO_AIV_SYNC_ID);
            asc_copy_ub2gm(output, result_ub, output_bytes);
        }
    }

    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, AIV_TO_AIC_SYNC_ID);
        asc_sync_block_wait(PIPE_MTE1, CROSS_CORE_FLAG_ID);
        asc_set_gm2l1_pad(PAD_DWORD);
        if (mode == static_cast<uint32_t>(PaddingMode::NORMAL)) {
            // Normal：dst stride为64B，每个48B burst尾部用该burst首字节填充16B。
            asc_copy_gm2l1_align(l1, input, BURST_COUNT, BURST_BYTES, 0, 0, false,
                4, BURST_BYTES, NORMAL_STRIDE);
        } else if (mode == static_cast<uint32_t>(PaddingMode::COMPACT)) {
            // Compact：dst stride等于48B；仅在144B整体数据末尾用0xA5填充16B。
            output_bytes = COMPACT_OUTPUT_BYTES;
            asc_copy_gm2l1_align(l1, input, BURST_COUNT, BURST_BYTES, 0, 0, true,
                4, BURST_BYTES, BURST_BYTES);
        } else {
            // 左右填充：每个burst为4B左填充 + 48B数据 + 12B右填充，强制使用0xA5。
            asc_copy_gm2l1_align(l1, input, BURST_COUNT, BURST_BYTES, LEFT_PADDING, RIGHT_PADDING, false,
                4, BURST_BYTES, NORMAL_STRIDE);
        }
        // GM到L1在MTE2执行，L1到UB在MTE1执行；通过事件建立跨流水依赖。
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        // L1结果回读到另一个物理UB，交由AIV写回GM进行主机校验。
        asc_copy_l12ub(result_ub, l1, 0, 1, output_bytes / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_intra_arrive(PIPE_MTE1, AIC_TO_AIV_SYNC_ID);
    }
    asc_sync_pipe(PIPE_ALL);
}

std::vector<uint8_t> MakeInput()
{
    std::vector<uint8_t> input(INPUT_BYTES);
    for (uint32_t burst = 0; burst < BURST_COUNT; ++burst) {
        for (uint32_t i = 0; i < BURST_BYTES; ++i) {
            input[burst * BURST_BYTES + i] = static_cast<uint8_t>(0x10 * (burst + 1) + i);
        }
    }
    return input;
}

std::vector<uint8_t> MakeGolden(const std::vector<uint8_t>& input, PaddingMode mode)
{
    const uint32_t output_bytes = mode == PaddingMode::COMPACT ? COMPACT_OUTPUT_BYTES : NORMAL_OUTPUT_BYTES;
    std::vector<uint8_t> golden(output_bytes, PAD_BYTE);
    for (uint32_t burst = 0; burst < BURST_COUNT; ++burst) {
        const uint32_t src_offset = burst * BURST_BYTES;
        if (mode == PaddingMode::NORMAL) {
            const uint32_t dst_offset = burst * NORMAL_STRIDE;
            std::memcpy(golden.data() + dst_offset, input.data() + src_offset, BURST_BYTES);
            std::memset(golden.data() + dst_offset + BURST_BYTES, input[src_offset], NORMAL_STRIDE - BURST_BYTES);
        } else if (mode == PaddingMode::COMPACT) {
            std::memcpy(golden.data() + src_offset, input.data() + src_offset, BURST_BYTES);
        } else {
            const uint32_t dst_offset = burst * NORMAL_STRIDE;
            std::memcpy(golden.data() + dst_offset + LEFT_PADDING, input.data() + src_offset, BURST_BYTES);
        }
    }
    return golden;
}

const char* ModeName(PaddingMode mode)
{
    return mode == PaddingMode::NORMAL ? "normal" :
        (mode == PaddingMode::COMPACT ? "compact" : "left_right");
}

bool RunMode(PaddingMode mode, const std::vector<uint8_t>& input,
    uint8_t* input_device, uint8_t* output_device)
{
    const std::vector<uint8_t> golden = MakeGolden(input, mode);
    std::vector<uint8_t> output(golden.size(), 0);
    asc_copy_gm2l1_align_modes_kernel<<<1, 0>>>(output_device, input_device, static_cast<uint32_t>(mode));
    if (aclrtSynchronizeDevice() != ACL_SUCCESS ||
        aclrtMemcpy(output.data(), output.size(), output_device, output.size(), ACL_MEMCPY_DEVICE_TO_HOST) != ACL_SUCCESS) {
        return false;
    }
    const bool passed = output == golden;
    std::cout << "RESULT mode=" << ModeName(mode) << " status=" << (passed ? "PASS" : "FAIL")
              << " output_bytes=" << output.size() << std::endl;
    return passed;
}
} // namespace

int main()
{
    const std::vector<uint8_t> input = MakeInput();
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), NORMAL_OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE) != ACL_SUCCESS) {
        return 1;
    }
    const bool passed = RunMode(PaddingMode::NORMAL, input, input_device, output_device) &&
        RunMode(PaddingMode::COMPACT, input, input_device, output_device) &&
        RunMode(PaddingMode::LEFT_RIGHT, input, input_device, output_device);
    std::cout << "SUMMARY total=3 passed=" << (passed ? 3 : 0) << std::endl;
    aclrtFree(output_device);
    aclrtFree(input_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
