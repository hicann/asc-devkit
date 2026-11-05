# Memory矢量计算编程<a name="ZH-CN_TOPIC_0000002600000002"></a>

本文介绍基于Unified Buffer（UB）的Ascend C矢量计算开发方法，该计算能力简称Memory矢量计算编程，是Ascend C中最基础、应用最广泛的矢量编程方式。[NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md)原生支持完整的UB直接编程能力；[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入寄存器计算能力，推荐使用Reg矢量计算编程以获得更高性能。

## Memory矢量编程的基本步骤

Memory矢量计算的标准数据流分为数据搬入、计算、数据搬出三个阶段，完整流程如下：

1. 通过DMA将数据从全局内存搬运至本地内存；
2. 进行矢量计算；
3. 计算完成后，通过DMA将结果从本地内存搬运回全局内存（Global Memory）。

## Memory矢量计算内存管理

Memory矢量计算依托本地内存中的UB完成运算，开发者需先将输入数据搬运至UB，再调用矢量计算指令执行计算。UB支持通过数组形式声明，分为静态申请和动态申请两种方式。

- 静态申请：分配固定大小的内存空间，空间尺寸在编译阶段确定，运行时无法修改。

    ```c
    __global__ __vector__ void add_custom(...)
    {
        constexpr uint32_t ub_size = 256;
        __ubuf__ half static_buffer[ub_size];  // Compiler automatically aligns to 32 bytes
    }
    ```

- 动态申请：用户需要通过<<<>>>中参数dynamic_ub_size指定动态内存的空间大小，其大小在运行期确定。

    ```c
    // Device-side: declare dynamic shared memory
    __global__ __vector__ void add_custom(...)
    {
        extern __ubuf__ half dynamic_buf[];
        // ...
    }

    // Host-side: specify dynamic shared memory size via third parameter in <<<>>>
    int32_t main(int argc, char const *argv[])
    {
        // ...
        uint32_t dyn_ubuf_size = 1024 * sizeof(char);
        add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
        // ...
    }
    ```

## Memory数据搬运

昇腾NPU提供多组专用接口，实现全局内存（GM）与UB之间的数据搬运，适配各类业务场景。为保障矢量计算效率，标准流程为：数据从GM搬运至UB完成计算，计算结果再从UB回写至GM，形成完整数据链路。
开发者可灵活配置参数，精准控制数据块数量、长度、地址间隔等属性，实现连续数据搬运与非连续（高维切分）数据搬运，充分发挥硬件搬运与计算的协同能力，提升矢量算子整体执行效率。

<!-- npu="950" id1 -->
本节以[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)为例介绍数据搬运接口。

<table>
<thead>
<tr><th>数据通路</th><th>搬运模式</th><th>接口</th><th>适用场景</th></tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Global Memory -> UB</td>
<td>连续搬运</td>
<td><code>asc_copy_gm2ub(dst, src, size)</code><br><code>asc_copy_gm2ub_align(dst, src, size)</code></td>
<td>单段连续数据搬运，长度可非32B对齐</td>
</tr>
<tr>
<td>高维切分搬运</td>
<td><code>asc_copy_gm2ub(dst, src, burst_count, burst_len, src_stride, dst_stride)</code><br><code>asc_copy_gm2ub_align(dst, src, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, l2_cache_mode, src_stride, dst_stride)</code></td>
<td>多块数据搬运，可配置块数、长度和首地址间隔；带<code>_align</code>的接口还可配置左右Padding、填充方式和L2 Cache策略</td>
</tr>
<tr>
<td rowspan="2">UB -> Global Memory</td>
<td>连续搬运</td>
<td><code>asc_copy_ub2gm(dst, src, size)</code><br><code>asc_copy_ub2gm_align(dst, src, size)</code></td>
<td>单段连续数据搬运，长度可非32B对齐</td>
</tr>
<tr>
<td>高维切分搬运</td>
<td><code>asc_copy_ub2gm(dst, src, burst_count, burst_len, dst_stride, src_stride)</code><br><code>asc_copy_ub2gm_align(dst, src, burst_count, burst_len, l2_cache_mode, dst_stride, src_stride)</code></td>
<td>多块数据搬运，可配置块数、长度和首地址间隔；带<code>_align</code>的接口还可配置L2 Cache策略</td>
</tr>
</tbody>
</table>

以上接口均要求UB起始地址32B对齐，搬运长度需满足数据类型的字节对齐要求。长度非32B整数倍时，GM向UB搬入会在目的UB中写入补齐数据，需为补齐后的写入范围预留空间；UB向GM搬出会按32B对齐范围读取源UB，需为读取范围预留合法空间，但仅将有效数据写入GM。

**连续搬运**：若需将GM中连续地址的数据搬运至UB，仅需指定搬运数据的字节长度即可。下图搬运256B连续数据，以half类型为例，共128个元素，可调用`asc_copy_gm2ub`或`asc_copy_gm2ub_align`接口，以下两种调用方式任选其一：

<img src="../../../../../api/figures/asc_copy_gm2ub_continuous.png" alt="连续搬运示意图" width="800"/>

```c
uint32_t size = 128 * sizeof(half);
// Option 1
asc_copy_gm2ub(dst_ub, src_gm, size);
// Option 2
asc_copy_gm2ub_align(dst_ub, src_gm, size);
```

>[!NOTE]说明
>这些连续搬运接口主要为了提升用户数据搬运的易用性，由软件层面提供，底层通过高维切分接口实现。

**高维切分搬运**：若需将GM中多段非连续数据搬入UB，可通过高维切分模式，配置数据块数量、单块长度、块间间隔等参数。下图搬运两个数据块，每块96B，两个数据块在GM上间隔为32B，在UB上间隔为64B。共搬运192B有效数据，目的UB至少需预留256B空间。

<img src="../../../../../api/figures/asc_copy_gm2ub_highdim_split.png" alt="高维切分搬运示意图" width="800"/>

`burst_len`、`src_stride`和`dst_stride`均以字节为单位，stride表示相邻数据块首地址的间隔。上图对应的参数如下：

```c
uint16_t burst_count = 2;
uint16_t burst_len = 96;
// Source burst starts are 96B + 32B apart.
uint16_t src_stride = 96 + 32;
// Destination burst starts are 96B + 64B apart.
uint16_t dst_stride = 96 + 64;

asc_copy_gm2ub(dst_ub, src_gm, burst_count, burst_len, src_stride, dst_stride);
```

**高维切分搬运的对齐补齐与填充值配置**：对齐补齐用于满足UB侧的32B对齐要求，填充值配置用于指定补齐区域写入的数值。普通接口按默认方式补齐非32B对齐的数据；需要显式配置左右Padding或填充方式时，可使用`asc_copy_gm2ub_align`的高维切分原型。UB起始地址需32B对齐，搬运长度需满足数据类型的字节对齐要求。

以搬运3个长度为48B的数据块为例，将`left_padding_num`和`right_padding_num`均设置为0，通过`dst_stride`选择补齐模式：

Compact模式下，将`dst_stride`设置为`burst_len`，即48B。三个数据块在UB中的起始偏移依次为0B、48B、96B，仅在144B有效数据的末尾补齐16B，目的UB至少需预留160B空间。

<img src="../../../../../api/figures/asc_copy_gm2ub_align_compact_padding.png" alt="Compact模式32字节补齐示意图" width="800"/>

Normal模式下，将`dst_stride`设置为不等于`burst_len`且满足32B对齐的值，此例为64B。三个数据块在UB中的起始偏移依次为0B、64B、128B，每个数据块分别补齐16B，目的UB至少需预留192B空间。

<img src="../../../../../api/figures/asc_copy_gm2ub_align_normal_padding.png" alt="Normal模式32字节补齐示意图" width="800"/>

以下示例中，`src_gm`、`dst_ub_compact`和`dst_ub_normal`均为half类型指针，两个目的UB分别预留上述空间。源数据连续排列，`src_stride`为48B；通过设置`enable_constant_pad`为`true`，将图中的补齐区域填充为half类型的0：

```c
uint32_t burst_count = 3;
uint32_t burst_len = 48;
uint64_t src_stride = 48;
uint8_t left_padding_num = 0;
uint8_t right_padding_num = 0;

asc_set_copy_pad_val(static_cast<half>(0));
// Compact: pad only the end of all three bursts, using at least 160B of UB.
asc_copy_gm2ub_align(dst_ub_compact, src_gm, burst_count, burst_len,
                    left_padding_num, right_padding_num, true,
                    asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, src_stride, 48);
// Normal: pad each burst separately, using at least 192B of UB.
asc_copy_gm2ub_align(dst_ub_normal, src_gm, burst_count, burst_len,
                    left_padding_num, right_padding_num, true,
                    asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, src_stride, 64);
```

>[!NOTE]说明
>Compact模式要求左右Padding均为0。Normal模式支持显式配置左右Padding。左右Padding均为0时，`enable_constant_pad`用于选择补齐区域的填充方式：为`true`时使用`asc_set_copy_pad_val`配置的值，为`false`时由硬件自动填充dummy假数据。左右Padding任一非0时，`enable_constant_pad`不生效，必须提前通过[asc_set_copy_pad_val](../../../../../api/SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md)配置填充值，左右Padding区域及右侧为32B对齐继续补齐的区域均使用该值。

以上为GM向UB的数据搬入逻辑，UB向GM的数据搬出可使用同系列接口实现。更多搬运接口说明可参考[矢量数据搬运](../../../../../api/SIMD-API/c_api/vector_datamove/vector_datamove.md)。

<!-- end id1 -->

## Memory数据计算

Memory矢量计算包含连续计算和高维切分计算两种模式，同时支持掩码功能，可灵活控制参与计算的数据范围。
下文以加法接口为例，结合示意图说明两种模式的差异：连续计算用法简单，适用于一维张量的连续数据运算；高维切分计算灵活性更强，支持迭代运算与自定义地址间隔。开发者可根据业务场景按需选择。

<img src="../../../../figures/mem_vec_c.png" alt="Memory矢量计算模式示意图" width="800"/>

### 高维切分计算

高维切分能力主要是支持对计算接口设置`repeat_time`、`stride`和`mask`参数等，能精确控制指令的迭代执行、操作数的地址间隔和迭代内的掩码。

#### 迭代控制

矢量计算单元单次迭代从UB读取8个连续的DataBlock（每个32字节），运算结果写入目的UB的8个对应DataBlock。

<img src="../../../../figures/db_iter_c.png" alt="DataBlock迭代示意图" width="800"/>

若设置repeat_time（迭代次数）为2，单元将执行两轮迭代，总处理数据量为2 × 8 × 32字节 = 512字节；若数据类型为half（2字节 / 元素），则对应处理256个元素。

> 📌 硬件约束：repeat_time取值范围为1~255，该约束对[NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md)、[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)全系列产品生效。

<img src="../../../../figures/iter2_exp_c.png" alt="2次迭代Exp计算" width="800"/>

#### 地址间隔配置

矢量计算单元还支持带间隔的向量计算，通过`data_block_stride`（单次迭代内不同DataBlock间地址步长）和`repeat_stride`（相邻迭代间相同DataBlock的地址步长）来进行配置。`data_block_stride`是指单次迭代内不同DataBlock间地址步长。其取值不能超过UB大小限制。
连续计算，`data_block_stride`设置为1，对同一迭代内的8个DataBlock数据连续进行处理。
非连续计算，`data_block_stride`值大于1（如取2），同一迭代内不同DataBlock之间在读取数据时出现一个DataBlock的间隔，如下图所示。

<img src="../../../../figures/db_stride_c.png" alt="data_block_stride示例" width="800"/>

> 📌 若dst_block_stride == 0，等效于dst_block_stride = 1；若src_block_stride = 0，源操作数将始终复用第一个DataBlock。

下图给出了单次迭代内源操作数与目的操作数在UB空间的读写示例。示例中源操作数的`data_block_stride`配置为2，表示单次迭代内不同DataBlock间地址步长为2个DataBlock；目的操作数的`data_block_stride`配置为1，表示单次迭代内地址连续。

<img src="../../../../figures/iter_rw_c.png" alt="单次迭代内源和目的操作数读写示例" width="800"/>

其中**repeat_stride**是指相邻迭代间相同DataBlock的地址步长，可通过设置`repeat_stride`来灵活控制不同的场景。

| 场景 | repeat_stride取值 | 数据读取特点 | 适用场景 |
|------|------------------|-------------|--------|
| 连续计算 | 8 | 每轮迭代读取连续8个data_block，完成所有输入数据的计算 | <img src="../../../../figures/rep_cont_c.png" alt="repeat_stride连续计算场景" width="600"/> |
| 非连续计算 | >8 (如10) | 相邻迭代间存在data_block间隔，地址不连续 | <img src="../../../../figures/rep_discont_c.png" alt="repeat_stride非连续计算场景" width="600"/> |
| 反复计算 | 0 | 对首个连续8个data_block反复读取和计算 | <img src="../../../../figures/rep_repeat_c.png" alt="repeat_stride反复计算场景" width="600"/> |
| 部分重复计算 | (0, 8) | 相邻迭代间部分数据重复读取，一般场景不涉及 | <img src="../../../../figures/rep_partial_c.png" alt="repeat_stride部分重复计算" width="600"/> |

当 `repeat_time`>1 时，通过多次迭代完成计算。`repeat_stride` 表示相邻迭代间相同位置DataBlock的起始地址间隔（以DataBlock为单位）。例如 `repeat_stride` = 9 时，第一迭代的第1个DataBlock与第二迭代的第1个DataBlock间隔9个DataBlock。

<img src="../../../../figures/multi_iter_c.png" alt="多次迭代非连续场景示意图" width="800"/>

通过C风格接口配置`repeat_stride`和`data_block_stride`实现上述多次迭代功能。

```c
// In the above example with repeat interval of 9, the following configuration achieves the corresponding operation
uint8_t repeat_time = 1;
uint8_t dst_block_stride = 1;
uint8_t src0_block_stride = 1;
uint8_t src1_block_stride = 1;
uint8_t dst_repeat_stride = 9;
uint8_t src0_repeat_stride = 9;
uint8_t src1_repeat_stride = 9;

asc_add(z, x, y, repeat_time, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

#### 掩码设置

针对同一个迭代中的数据，可以通过mask参数进行掩码操作来控制实际参与计算的个数。下图为进行Abs计算时通过mask逐比特模式按位控制哪些元素参与计算的示意图，1表示参与计算，0表示不参与计算。

<img src="../../../../figures/mask_op_c.png" alt="掩码操作示意图" width="800"/>

每一位掩码对应数据中的一个元素的位置，通过有效位和无效位标记，实现对数据操作的精细化开关控制。
掩码为固定位宽数值，规则如下：有效位（值为1）代表对应元素参与计算，无效位（值为0）代表对应元素被屏蔽，不参与运算。

掩码设置提供了两种模式：Counter模式和Normal模式，Normal模式又包含连续模式、逐比特模式。
Counter模式适用于连续元素的计算场景。该模式下，掩码寄存器仅低64位有效，用于标识参与计算的元素总数；接口会自动忽略repeat_time，由系统根据元素数量自动匹配迭代次数。例如计算200个half类型元素时，受UB的32B对齐约束，需申请256个元素空间。此时使用Counter模式，可精确处理实际元素数量，避免多余计算。

<img src="../../../../figures/count_mode.png" alt="Counter模式计算" width="600"/>

```c
__ubuf__ half dst_ub[256];
__ubuf__ half src0_ub[256];
__ubuf__ half src1_ub[256];
half scalar_value = 1.0;
// 1. Direct transfer processing (temporarily omitted)
// 2. Use high-dimensional segmentation API for computation
// 2.1 Set mask counter mode
asc_set_mask_count();
// 2.2 Set Mask value to 200, indicating this computation processes 200 elements
asc_set_vector_mask(0, static_cast<uint64_t>(200)); 
asc_add_scalar(dst_ub, src0_ub, scalar_value, 1, 1, 1, 1, 8, 8, 8);

// 3. Default needs to reset to Normal mode, if subsequent still requires Counter mode then no need to reset to Normal mode
asc_set_mask_normal();
```

在Normal计算模式（默认模式，若未发生模式切换则无需重复配置）中，MASK寄存器会作为每个Repeat的掩码使用，mask参数可通过`asc_set_vector_mask`设置以控制单次迭代内参与计算的元素个数；只有高维切分支持此模式，在接口上区分为单次迭代内连续模式和单次迭代内逐bit模式。

- 单次迭代内连续模式：表示前n个连续元素参与计算。取值范围和操作数的数据类型有关，数据类型不同，每次迭代内能够处理的元素个数最大值不同。

    > **mask范围推导**：Vector计算单元单次迭代固定处理32B × 8 block = 256B数据，因此mask最大值 = 256B / sizeof(DataType)。当操作数为16位（sizeof=2B）时，mask∈[0, 128]（256/2=128）；当操作数为32位（sizeof=4B）时，mask∈[0, 64]（256/4=64）；当操作数为64位（sizeof=8B）时，mask∈[0, 32]（256/8=32）。此推导规则可推广至其他数据类型（如bfloat16=16bit→mask上限128，fp32=32bit→mask上限64）。

    连续模式下，mask超出范围按照对应数据类型下mask最大值计算；mask = 0表示指令不会执行计算操作，该接口将被视为NOP（空操作）；若设置为负值可能产生未定义行为。

    该模式下，可以指定单次迭代内计算前n个连续元素，其他数据不参与计算。例如：对src数据(shape(256)，数据类型为half)，该数据需通过2次迭代计算完（每次迭代计算128个元素），可通过设置mask = 100，表示第1次迭代计算[0, 99]范围的元素，第2次迭代计算[128, 227]范围的元素参与计算，其他元素不参与计算。

    <img src="../../../../figures/iter_cont.png" alt="单次迭代内连续计算" width="600"/>

    ```c
    __ubuf__ half dst_ub[256];
    __ubuf__ half src0_ub[256];
    __ubuf__ half src1_ub[256];
    half scalar_value = 1.0;

    // 1. Data transfer (temporarily omitted)
    // 2. Use high-dimensional segmentation API for computation
    // 2.1 Set mask Normal mode
    asc_set_mask_normal();
    // 2.2 Set Mask value to 100, indicating single iteration computes first 100 elements
    asc_set_vector_mask(0, static_cast<uint64_t>(100)); 
    asc_adds(dst_ub, src0_ub, scalar_value, 2, 1, 1, 1, 8, 8, 8);
    ```

- 单次迭代内逐比特模式：可以按位控制哪些元素参与计算，bit位的值为1表示参与计算，0表示不参与。分为maskHigh（高位mask）和maskLow（低位mask）。参数取值范围和操作数的数据类型有关，数据类型不同，每次迭代内能够处理的元素个数最大值不同。当操作数为16位时，maskLow、maskHigh∈[0, 264-1]，并且不同时为0；当操作数为32位时，maskHigh为0，maskLow∈(0, 264-1]；当操作数为64位时，maskHigh为0，maskLow∈(0, 232-1]。

    该模式下，可以指定maskHigh和maskLow的值，来处理各种掩码操作。如需进行交错计算，则mask设置成01010101...或者10101010...模式即可。
    以src数据(shape(256)，数据类型为half)为例，通过设置逐bit的掩码，可在单迭代中选取特定bit位的数值参与计算：

    <img src="../../../../figures/iter_bit.png" alt="单次迭代内逐bit计算" width="600"/>

    ```c
    __ubuf__ half dst_ub[256];
    __ubuf__ half src0_ub[256];
    __ubuf__ half src1_ub[256];

    // 1. Data transfer (temporarily omitted)
    // 2. Use high-dimensional segmentation API for computation
    // 2.1 Set mask Normal mode
    asc_set_mask_normal();
    // 2.2 Set Mask to bit-wise mode (0101...), implement interleaved computation
    asc_set_vector_mask(0x000000000000AAAA, 0x0000000000000000); 
    asc_add(dst_ub, src0_ub, src1_ub, 2, 1, 1, 1, 8, 8, 8);
    ```

### 连续计算

连续计算是软件层提供的简化接口，使用时无需配置掩码，可直接对源操作数中连续的N个数据执行运算，并将结果连续写入目的操作数，主要用于一维数据的批量计算。
以加法运算为例，如需对长度为4096的矢量执行加法，调用方式如下：

```c
asc_add(z, x, y, 4096); // 4096: number of consecutive elements involved in addition.
```

## Memory矢量计算同步控制

AI Core内部的MTE2搬运单元、矢量计算单元等均为异步并行运行，多个单元读写同一块存储资源时会产生数据依赖。因此Memory矢量计算必须借助流水同步接口，保障流程正常执行。Memory矢量计算流程比矩阵计算更简洁，分为数据搬入、计算、数据搬出三个阶段，分别对应流水线PIPE_MTE2、PIPE_V、PIPE_MTE3，需通过核内同步接口约束执行顺序，保证流程串行执行。在[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)产品中，可使用`asc_lock`与`asc_unlock`接口实现三阶段的同步控制。

关于`asc_lock`/`asc_unlock`的同步原理，以及双缓冲场景中的数据依赖和Buffer复用，请参考[基于Mutex实现多流水同步](../../../advanced_programming/advanced_kernel_programming/intra_core_sync/mutex_multi_pipeline_sync.md)。

以下为矢量加法的完整示例：

```c
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    // ...
    // 1. Step 1: Data transfer in, execution pipeline is PIPE_MTE2
    asc_lock(PIPE_MTE2, mutex_id);
    asc_copy_gm2ub_align(x_local, x_gm, 1, 48 * sizeof(half), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_copy_gm2ub_align(y_local, y_gm, 1, 48 * sizeof(half), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_unlock(PIPE_MTE2, mutex_id);

    // 2. Step 2: Compute, execution pipeline is PIPE_V
    asc_lock(PIPE_V, mutex_id);
    asc_add(z, x, y, 4096);
    asc_unlock(PIPE_V, mutex_id);
    
    // 3. Step 3: Data transfer out, execution pipeline is PIPE_MTE3
    asc_lock(PIPE_MTE3, mutex_id);
    asc_copy_ub2gm_align(z_gm, z_local, 1, 48 * sizeof(int8_t), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_unlock(PIPE_MTE3, mutex_id);
}
```

## Memory矢量计算示例

结合上文介绍的数据搬运、计算、同步能力，即可基于C语言开发完整的Memory矢量计算算子。完整工程示例可参考：[c_api_add样例](../../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add)。
