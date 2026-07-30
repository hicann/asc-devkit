# C API

新增实验性C API 3510系列，开放底层完备能力，提供直接操作NPU硬件资源的接口，覆盖数据搬运、矢量计算、矩阵计算、同步控制、标量计算、缓存控制等全场景。支持以数组形式分配内存，一般基于指针编程。提供与业界一致的C语言编程体验。

## 特性

### 数据结构

提供配置类联合体定义，用于矩阵填充、原子操作、L0C Buffer搬运、3D加载、N维填充等场景的参数封装。

- `asc_fill_value_config`：fill_value的初始化参数结构体，包含 `asc_fill_l0a` / `asc_fill_l0b` / `asc_fill_l1` 接口需要配置的各种初始化参数。
- `asc_store_atomic_config`：原子操作使能位与原子操作类型的值。
- `asc_set_l0c_copy_params_config`：DataCopy（CO1->GM、CO1->A1）过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。
- `asc_load3d_v2_config`：Load3Dv2接口的repeat参数。
- `asc_ndim_pad_count_config`：N维搬运时各层循环的左右填充计数配置。
- `asc_l13d_fmatrix_config`：L1 3D fmatrix配置，用于设置 `asc_copy_l12l0a` / `asc_copy_l12l0b` 的3D格式搬运的fmatrix参数。

### 矢量数据搬运

- `asc_copy_gm2ub_align` / `asc_copy_gm2ub_align_sync`：提供数据非对齐搬运的功能，将数据从Global Memory搬运到Unified Buffer，扩展支持 `fp8_e5m2_t` / `fp8_e4m3fn_t` / `hifloat8_t`，增加 `enable_constant_pad` / `l2_cache_mode` 参数。
- `asc_copy_ub2gm_align` / `asc_copy_ub2gm_align_sync`：将数据从Unified Buffer搬运到Global Memory，增加 `l2_cache_mode` 参数。
- `asc_copy_ub2l1` / `asc_copy_ub2l1_sync`：将数据从Unified Buffer搬运到L1 Buffer。
- `asc_ndim_copy_gm2ub`：N维GM到UB搬运，支持 `int8_t` / `uint8_t` / `fp4x2_e2m1_t` / `fp4x2_e1m2_t` / `fp8_e8m0_t` / `fp8_e5m2_t` / `fp8_e4m3fn_t` / `int16_t` / `uint16_t` / `half` / `bfloat16_t` / `int32_t` / `uint32_t` / `float`。
- `asc_set_gm2ub_loop_size` / `asc_set_gm2ub_loop1_stride` / `asc_set_gm2ub_loop2_stride`：GM到UB循环搬运参数配置。
- `asc_set_ub2gm_loop_size` / `asc_set_ub2gm_loop1_stride` / `asc_set_ub2gm_loop2_stride`：UB到GM循环搬运参数配置。
- `asc_set_gm2ub_pad`：GM到UB填充值配置。
- `asc_set_ndim_loop0_stride` ~ `asc_set_ndim_loop4_stride`：N维循环步长配置。
- `asc_set_ndim_pad_count` / `asc_set_ndim_pad_value`：N维填充配置。

### Reg矢量计算

提供基于矢量寄存器的计算接口，使用 `__simd_callee__` 限定符，支持 `int8_t` / `uint8_t` / `int16_t` / `uint16_t` / `int32_t` / `uint32_t` / `half` / `float` / `bfloat16_t` / `fp8_e4m3fn_t` / `fp8_e5m2_t` / `hifloat8_t` / `fp8_e8m0_t` / `fp4x2_e2m1_t` / `fp4x2_e1m2_t` 等数据类型。

**寄存器数据类型：**

- `asc_create_iter_reg_b8` / `asc_create_iter_reg_b16` / `asc_create_iter_reg_b32`：地址寄存器通过该接口初始化，然后在循环之中使用地址寄存器存储地址偏移量。
- `asc_create_mask_b8` / `asc_create_mask_b16` / `asc_create_mask_b32`（宏）：根据入参生成相应的掩码寄存器。
- `asc_get_mask_spr_b16` / `asc_get_mask_spr_b32`：从特殊寄存器SPR{MASK1, MASK0} 读取mask值并根据数据类型格式返回对应的mask数据，MASK0、MASK1均为64bit的寄存器。
- `asc_update_mask_b8` / `asc_update_mask_b16` / `asc_update_mask_b32`：根据value大小生成对应的掩码寄存器中的值。

**reg数据搬入：**

- `asc_loadalign`：对齐数据搬运接口，从UB连续对齐搬入目的操作数，支持多种搬入模式。
- `asc_loadalign_brc` / `asc_loadalign_brc_v2` / `asc_loadalign_brc_v3`：广播加载对齐数据。
- `asc_loadalign_unpack` / `asc_loadalign_unpack_v2`：解包加载对齐数据。
- `asc_loadalign_upsample` / `asc_loadalign_downsample`：上采样/下采样加载对齐数据。
- `asc_loadalign_deintlv`：解交织加载对齐数据。
- `asc_loadalign_postupdate` 及各变体的 `_postupdate` 版本：地址自更新加载。
- `asc_loadunalign`：reg计算数据搬运接口，适用于从UB非32B对齐的起始地址连续搬入矢量数据寄存器的场景。
- `asc_loadunalign_pre`：用于在进行非对齐数据搬入前的初始化，需配合 `asc_loadunalign` 接口使用。
- `asc_loadunalign_postupdate`：非对齐搬入的地址自更新版本。
- `asc_load`：reg计算数据搬运接口，支持从UB非32字节对齐的源地址src搬运至矢量数据寄存器，搬运量为VL。
- `asc_gather`：根据索引位置index将源操作数src按元素收集到目的操作数dst中。
- `asc_gather_datablock`：给定源操作数在UB中的基地址和索引，根据索引位置将源操作数按DataBlock收集到目的操作数中。
- `asc_copy`：完成数据在寄存器内的搬运。

**reg数据搬出：**

- `asc_storealign`：reg计算数据搬运接口，适用于从矢量数据寄存器连续对齐搬出到UB的场景，并支持多种搬出模式。
- `asc_storealign_1st`：首次对齐搬出。
- `asc_storealign_pack` / `asc_storealign_pack_v2`：打包搬出。
- `asc_storealign_intlv`：交织搬出。
- `asc_storeunalign`：reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的场景。
- `asc_storeunalign_post` / `asc_storeunalign_postupdate` / `asc_storeunalign_post_postupdate`：非对齐搬出的地址自更新变体。
- `asc_storealign_postupdate` 及各变体的 `_postupdate` 版本：地址自更新搬出。
- `asc_scatter`：数据分散存储。
- `asc_store`：reg计算数据搬运接口，适用于从矢量数据寄存器搬出到UB的场景，不区分是否对齐，在追求极致性能时，应尽量避免使用该接口。

**基础算术：**

- `asc_add`：按元素执行加法运算。
- `asc_addc`：按元素执行带进位的加法运算。
- `asc_add_scalar`：按元素执行矢量和标量的加法运算。
- `asc_sub`：按元素执行减法运算。
- `asc_subc`：按元素执行带借位的减法运算。
- `asc_mul`：对源操作数src0和src1进行乘法运算，将结果写入目的操作数dst。
- `asc_mul_scalar`：对源操作数src、value进行按元素乘法操作，将结果写入目的操作数dst。
- `asc_madd`：madd（multiply-add），对源操作数执行逐元素乘法和加法。
- `asc_mull`：根据mask对输入数据src0、src1按元素相乘，将结果写入dst0，溢出部分写入dst1。
- `asc_max`：按元素求最大值。
- `asc_max_scalar`：按元素求矢量和标量的最大值。
- `asc_min`：根据mask对源操作数src0、src1进行按元素求最小值操作，将结果写入目的操作数dst。
- `asc_min_scalar`：源操作数矢量内每个元素与标量比较，取较小值。
- `asc_abs`：逐元素计算绝对值。
- `asc_neg`：根据mask对源操作数src进行取相反数操作，将结果写入目的操作数dst。
- `asc_abs_sub`：逐元素计算差的绝对值。
- `asc_axpy`：根据mask对源操作数src、value进行按元素做乘加操作，将结果写入目的操作数dst。
- `asc_relu`：逐元素执行ReLU运算。
- `asc_leakyrelu`：按元素执行Leaky ReLU（Leaky Rectified Linear Unit）操作。
- `asc_prelu`：源操作数src0大于0的情况下直接将src0写入目的操作数dst，否则将src0 * src1的结果写入dst。
- `asc_exp`：对源操作数逐元素计算指数。
- `asc_ln`：对源操作数逐元素计算自然对数。
- `asc_sqrt`：逐元素计算平方根。
- `asc_div`：按元素执行除法运算。

**逻辑计算：**

- `asc_and`：对掩码寄存器操作：根据mask对源操作数src0、src1的有效bit进行逻辑与运算，得到新的掩码寄存器。
- `asc_or`：对源操作数src0和src1进行或（|）运算，将结果写入目的操作数dst。
- `asc_xor`：根据mask对输入的src0、src1按元素异或（^）进行操作，将结果写入dst。
- `asc_not`：执行矢量非运算。
- `asc_shiftleft`：根据掩码mask对输入数据src0，按照src1对应元素进行左移操作，完成后将结果写入dst中。
- `asc_shiftleft_scalar`：根据mask对源操作数src执行左移，左移的位数由输入参数value决定，将结果写入目的操作数dst。
- `asc_shiftright`：根据掩码mask对输入数据src0，按照src1对应元素进行右移操作，完成后将结果写入dst中。
- `asc_shiftright_scalar`：根据mask对源操作数src执行右移，右移的位数由输入参数value决定，将结果写入目的操作数dst。

**比较与选择：**

- `asc_eq`：对源操作数逐元素比较是否相等。
- `asc_eq_scalar`：对源操作数逐元素比较是否与标量相等。
- `asc_ne`：ne（not equal），对源操作数执行逐元素比较。
- `asc_ne_scalar`：src中的每个元素逐个与标量value比较大小，如果src_i != value，则输出结果dst对应比特位为1，否则为0。
- `asc_lt`：lt（less than），对源操作数执行逐元素比较。
- `asc_lt_scalar`：lt（less than），对源操作数与标量执行逐元素比较。
- `asc_le`：le（less than or equal to），对源操作数执行逐元素比较。
- `asc_le_scalar`：le（less than or equal to），对源操作数与标量执行逐元素比较。
- `asc_gt`：gt（greater than），对源操作数执行逐元素比较。
- `asc_gt_scalar`：gt（greater than），对源操作数与标量执行逐元素比较。
- `asc_ge`：ge（greater than or equal to），对源操作数执行逐元素比较。
- `asc_ge_scalar`：ge（greater than or equal to），对源操作数与标量执行逐元素比较。
- `asc_select`：根据mask的比特位值，从源操作数src0、src1中选择元素，得到目的操作数dst。
- `asc_squeeze` / `asc_squeeze_v2`：将src中被mask选择的有效元素依次复制到dst，有效元素在dst上连续排列。

**类型转换：**

- `asc_float2half`：将float类型转换为half类型，并支持多种舍入模式。
- `asc_half2float`：将half类型数据转换为float类型。
- `asc_float2bfloat16`：将float类型转换为bfloat16_t类型，并支持多种舍入模式。
- `asc_bfloat162float`：将bfloat16_t类型转换为float类型，无舍入模式。
- `asc_float2e4m3`：将float类型转换为fp8_e4m3fn_t类型，支持RINT舍入模式。
- `asc_float2e5m2`：将float类型数据转换为fp8_e5m2类型。
- `asc_float2hif8`：将float类型转换为hifloat8_t类型，并支持多种舍入模式。
- `asc_e4m32float`：将fp8_e4m3fn_t类型转换为float类型。
- `asc_e5m22float`：将vector_fp8_e5m2_t类型的源操作数以256B为单位分为四部分，读取其中一部分元素，将其转换成vector_float类型并写入目的操作数。
- `asc_hif82half`：将hifloat8_t类型数据转换为half类型。
- `asc_half2hif8`：将half类型转换为hifloat8_t类型，并支持多种舍入模式。
- `asc_bfloat162e1m2x2`：将bfloat16_t类型转换为fp4x2_e1m2_t类型，并支持多种舍入模式。
- `asc_bfloat162e2m1x2`：将bfloat16_t类型转换为fp4x2_e2m1_t类型，并支持多种舍入模式。
- `asc_e1m2x22bfloat16`：将fp4x2_e1m2_t类型转换为bfloat16_t类型。
- `asc_e2m1x22bfloat16`：将fp4x2_e2m1_t类型转换为bfloat16_t类型。
- `asc_bfloat162half`：将bfloat16_t类型转换为half类型，并支持多种舍入模式和饱和/非饱和模式。
- `asc_half2bf16`：将half类型数据转换为bfloat16_t类型，并支持多种舍入模式。
- `asc_int4x22half`：将int4x2_t类型数据转换为half类型。
- `asc_int4x22int16`：将int4x2_t类型转换为int16_t类型。
- `asc_int4x22bfloat16`：将int4x2_t类型转换为bfloat16_t类型，无舍入模式。
- `asc_half2int4x2`：将half类型数据转换为int4x2_t类型，并支持多种舍入模式。
- `asc_half2int8`：将half类型转换为int8_t类型，并支持多种舍入模式。
- `asc_half2int16`：将half类型数据转换为int16_t类型，并支持多种舍入模式。
- `asc_half2int32`：将half类型数据转换为int32_t类型，并支持多种舍入模式。
- `asc_half2uint8`：将half类型转换为uint8_t类型，并支持多种舍入模式。
- `asc_float2int16`：将float类型转换为int16_t类型，并支持多种舍入模式。
- `asc_float2int32`：将float类型转换为int32_t类型，并支持多种舍入模式。
- `asc_float2int64`：将float类型转换为int64_t类型，并支持多种舍入模式。
- `asc_int162float`：将int16_t类型转换为float类型。
- `asc_int322float`：将int32_t类型转换为float类型，并支持多种舍入模式。
- `asc_int642float`：将int64_t类型数据转换为float类型，并支持多种舍入模式。
- `asc_int162half`：将int16_t类型转换为half类型。
- `asc_int162int32`：将int16_t类型转换为int32_t类型。
- `asc_int162uint32`：将int16_t类型转换为uint32_t类型。
- `asc_int162uint8`：将int16_t类型转换为uint8_t类型。
- `asc_int322int16`：将int32_t类型转换为int16_t类型。
- `asc_int322int64`：将int32_t类型转换为int64_t类型。
- `asc_int322uint16`：将int32_t类型转换为uint16_t类型。
- `asc_int322uint8`：将int32_t类型转换为uint8_t类型。
- `asc_int642int32`：将int64_t类型转换为int32_t类型。
- `asc_int82half`：将int8_t类型转换为half类型。
- `asc_int82int16`：将int8_t类型转换为int16_t类型。
- `asc_int82int32`：将int8_t类型转换为int32_t类型。
- `asc_uint82half`：将uint8_t类型转换为half类型。
- `asc_uint82uint16`：将uint8_t类型转换为uint16_t类型。
- `asc_uint82uint32`：将uint8_t类型转换为uint32_t类型。
- `asc_uint162uint32`：将uint16_t类型转换为uint32_t类型。
- `asc_uint162uint8`：将uint16_t类型转换为uint8_t类型。
- `asc_uint322int16`：将uint32_t类型转换为int16_t类型。
- `asc_uint322uint16`：将uint32_t类型转换为uint16_t类型。
- `asc_uint322uint8`：将uint32_t类型转换为uint8_t类型。
- `asc_ceil` / `asc_floor` / `asc_rint` / `asc_round` / `asc_trunc`：将源操作数的浮点数元素截断到整数位，同时源操作数的数据类型保持不变，并支持多种舍入模式。

**归约计算：**

- `asc_reduce_max`：根据mask对源操作数src进行归约最大值操作，将结果写入目的操作数dst。
- `asc_reduce_min`：根据mask对源操作数src进行归约最小值操作，将结果写入目的操作数dst。
- `asc_reduce_sum`：归约求和功能，用于将src中的所有参与计算的元素求和，得到的结果保存在dst中。
- `asc_reduce_max_datablock`：找出DataBlock中的最大值，并将最终的计算结果依次保存在dst中的最低位。
- `asc_reduce_min_datablock`：根据mask将每个DataBlock（32B）中的最小值，依次保存在dst中的最低位。
- `asc_reduce_sum_datablock`：归约求和功能，用于将src每个DataBlock（32B）中参与计算的元素求和，得到的结果依次保存在dst中。
- `asc_pair_reduce_sum`：PairReduceSum，相邻两个（奇偶）元素求和，结果写入dst。

**数据压缩：**

- `asc_pack` / `asc_pack_v2`：将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的低半部分或高半部分。
- `asc_unpack`：矢量解包操作。
- `asc_unsqueeze`：根据mask进行解压缩，将生成的数据输出到dst。

**数据填充：**

- `asc_duplicate`：根据mask将源操作数src的最低位元素填充到目的操作数dst。
- `asc_duplicate_scalar`：根据mask将value填充到目的操作数dst。

**数据重排：**

- `asc_intlv`：将源操作数src0和src1中的元素交织存入目的操作数dst0和dst1中。
- `asc_deintlv`：给定源操作数src0和src1，将src0和src1中的元素解交织存入结果操作数dst0和dst1中。

**直方图计算：**

- `asc_cumulative_histogram`：对直方图数据进行累计统计。
- `asc_frequency_histogram`：对直方图数据进行频率统计。

**索引操作：**

- `asc_arange` / `asc_arange_descend`：以传入的value为起始值，生成递增/递减的索引，并将生成的索引保存在dst中。

**同步控制：**

- `asc_mem_bar`（宏）：Reg计算宏函数内不同流水线之间的同步指令。

**系统变量访问：**

- `asc_clear_ar_spr`：对AR寄存器进行清理，AR寄存器通常由 `asc_squeeze` 接口使用。

### 矩阵数据搬运

- `asc_set_l13d_size`：设置 `asc_copy_l12l0a` / `asc_copy_l12l0b` 的3D格式搬运接口在L1 Buffer的边界值。
- `asc_set_l13d_rpt`：用于设置Load3Dv2接口的repeat参数。
- `asc_set_l13d_padding`：设置Pad属性描述，用于在调用 `asc_copy_l12l0a` 接口时配置填充数值。
- `asc_set_l13d_fmatrix`：3D fmatrix配置。
- `asc_set_l0c_copy_prequant`：数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的标量量化参数。
- `asc_set_l0c_copy_params`：DataCopy（CO1->GM、CO1->A1）过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。
- `asc_copy_l0c2l1` / `asc_copy_l0c2l1_sync`：L0C Buffer到L1 Buffer搬运，扩展量化、ReLU、裁剪等随路操作参数。
- `asc_copy_l12l0a_mx` / `asc_copy_l12l0a_mx_sync`：L1 Buffer到L0A Buffer微缩格式搬运。
- `asc_set_gm2l1_loop_size` / `asc_set_gm2l1_loop1_stride` / `asc_set_gm2l1_loop2_stride` / `asc_set_gm2l1_pad`：GM到L1 Buffer循环搬运参数配置。

### 矩阵计算

- `asc_mmad_mx` / `asc_mmad_mx_sync`：微缩格式矩阵乘法，支持 `fp4x2_e1m2_t` / `fp4x2_e2m1_t` / `fp8_e4m3fn_t` / `fp8_e5m2_t`。
- `asc_set_l0c2gm_config`：数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的矢量量化参数。
- `asc_set_l0c2gm_nz2nd`：NZ到ND格式转换配置。

### 标量计算

- `asc_sflbits`：计算一个int64_t类型数字的二进制中，从最高数值位开始与符号位相同的连续比特位的个数。
- `asc_ffz`：获取一个uint64_t类型数字的二进制表示中从最低有效位开始的第一个0出现的位置，如果没找到则返回 -1。
- `asc_ffs`：FindFirstSet接口，输入数据的二进制表示中从最低位向最高位查找第一个值为1的位，并返回其位置，如果没找到则返回 -1。
- `asc_popc`：获取一个uint64_t类型数字的二进制中1的个数。
- `asc_store_dev`：标量数据存储到Global Memory，支持 `int8_t` / `uint8_t` / `int16_t` / `uint16_t` / `int32_t` / `uint32_t` / `int64_t` / `uint64_t`等数据类型。

### 同步控制

- `asc_sync`：等待所有流水线操作完成。
- `asc_sync_vec`：同步所有流水线。
- `asc_sync_mte2`：等待PIPE_MTE2流水完成。
- `asc_sync_mte3`：等待PIPE_MTE3流水完成。
- `asc_sync_notify`：设置同步标志（宏定义）。
- `asc_sync_wait`：等待同步标志（宏定义）。
- `asc_sync_pipe`：等待指定流水线操作完成（宏定义）。
- `asc_sync_data_barrier`：用于阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可以通过参数控制）执行结束。

### 系统变量

- `asc_get_ctrl`：读取CTRL寄存器（控制寄存器）的值。
- `asc_set_ctrl`：设置CTRL寄存器（控制寄存器）的值。
- `asc_get_block_num`：获取AI核数。
- `asc_get_system_cycle`：获取当前系统cycle数。

### 缓存控制

- `asc_icache_preload`：从指令所在DDR地址预加载数据到对应的cacheline中。
- `asc_get_icache_preload_status`：获取ICache的Preload的状态。
- `asc_datacache_preload`：从源地址所在的特定GM地址预加载数据到Data Cache中。
- `asc_dcci_single` / `asc_dcci_entire_out` / `asc_dcci_entire_all` / `asc_dcci_entire_atomic`：用于刷新Cache，保证Cache的一致性。
- `asc_ub_dcci_single`：Unified Buffer数据缓存失效。

### 原子操作

- `asc_set_atomic_add_float`：设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为float。
- `asc_set_atomic_max_float16`：设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的half数据与GM中已有数据进行逐元素比较，并将最大值写入GM。
- `asc_set_store_atomic_config_v2`：设置原子操作使能位与原子操作类型的值。

### 其他操作

- `asc_init`：初始化NPU状态。
