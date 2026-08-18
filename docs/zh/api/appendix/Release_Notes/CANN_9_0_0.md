# C API

CANN 9.0.0首次面向Ascend 950PR/Ascend 950DT（3510架构）提供试验性C语言扩展层单指令多数据（Single Instruction Multiple Data，SIMD）API。基于指针编程，提供C完备编程能力，通过声明静态数组管理本地内存，由开发者自主管理同步与内存，适配C语言开发习惯，开放全部底层硬件能力，支撑精细化调优与极致性能实现。接口覆盖数据结构、数据搬运、标量操作、矩阵计算、同步控制、系统变量、缓存控制、原子操作、其他操作、Reg数据搬运和Reg矢量计算等能力。

## 特性

### 数据结构

- `asc_set_l0c_copy_params_config`：用于设置L0C Buffer搬运过程中进行NZ格式转换为ND格式时所需的ND矩阵数量、源相邻NZ矩阵的起始地址偏移和目的相邻ND矩阵的起始地址偏移。
- `asc_load3d_v2_config`：用于设置`asc_copy_l12l0a`，`asc_copy_l12l0b`接口的repeat参数。
- `asc_ndim_pad_count_config`：用于`asc_set_ndim_pad_count`接口，设置`asc_ndim_copy_gm2ub`接口各个维度左右侧的填充元素个数。
- `asc_l13d_fmatrix_config`：用于设置`asc_copy_l12l0a`、`asc_copy_l12l0b`的3D格式搬运接口的特征图属性参数。

### 数据搬运

- `asc_copy_gm2ub_align`、`asc_copy_gm2ub_align_sync`：提供数据非对齐搬运功能，将数据从Global Memory搬运到Unified Buffer（UB），支持8位、16位、32位数据类型搬运。
- `asc_copy_ub2gm_align`、`asc_copy_ub2gm_align_sync`：将数据从UB搬运到Global Memory，支持8位、16位、32位分块搬运。
- `asc_ndim_copy_gm2ub`：多维数据搬运接口，将数据从Global Memory搬运到UB，最多处理5个维度，每个维度可单独配置步长，并支持随路填充功能。
- `asc_set_gm2ub_loop_size`：将数据从Global Memory搬运到UB时，设置数据搬运流程中的循环次数。
- `asc_set_gm2ub_loop1_stride`、`asc_set_gm2ub_loop2_stride`：将数据从Global Memory搬运到UB时，分别设置内层循环、外层循环中相邻迭代数据块间的间隔。
- `asc_set_ub2gm_loop_size`：将数据从UB搬运到Global Memory时，设置内层循环和外层循环的次数。
- `asc_set_ub2gm_loop1_stride`、`asc_set_ub2gm_loop2_stride`：将数据从UB搬运到Global Memory时，分别设置内层循环、外层循环中相邻迭代数据块间的间隔。
- `asc_set_gm2ub_pad`：使用`asc_copy_gm2ub_align`将数据从Global Memory搬运到UB且源操作数非对齐时，设置连续搬运数据块左右两侧需要填充的数据值。
- `asc_set_ndim_loop0_stride`、`asc_set_ndim_loop1_stride`、`asc_set_ndim_loop2_stride`、`asc_set_ndim_loop3_stride`、`asc_set_ndim_loop4_stride`：用于设置`asc_ndim_copy_gm2ub`接口每个维度内源操作数与目的操作数的元素间隔，最多设置5个维度。
- `asc_set_ndim_pad_count`：设置`asc_ndim_copy_gm2ub`接口各个维度左右侧的填充元素个数。
- `asc_set_ndim_pad_value`：当`asc_ndim_copy_gm2ub`接口的`padding_mode`参数为`true`时，设置填充固定值。
- `asc_set_l13d_rpt`：用于设置`asc_copy_l12l0a`，`asc_copy_l12l0b`接口的repeat参数。
- `asc_set_l13d_fmatrix`：设置特征图属性描述，用于`asc_copy_l12l0a`、`asc_copy_l12l0b`的3D格式搬运接口配置填充值。从左矩阵获取特征图属性时使用该接口。
- `asc_set_l0c_copy_prequant`：数据搬运过程中进行随路量化时，设置量化流程中的标量量化参数。
- `asc_set_l0c_copy_params`：从L0C Buffer搬运到Global Memory或L1 Buffer的过程中进行NZ格式转换为ND格式时，设置格式转换的相关配置。
- `asc_copy_l0c2l1`：将矩阵计算结果从L0C Buffer搬运至L1 Buffer，并可与其他接口组合配置多种随路能力。
- `asc_copy_l12l0a_mx`：将微缩放（Microscaling，MX）矩阵从L1 Buffer搬运到L0A Buffer。
- `asc_set_gm2l1_loop_size`：将数据从Global Memory搬运到L1 Buffer时，设置数据搬运流程中的循环次数。
- `asc_set_gm2l1_loop1_stride`、`asc_set_gm2l1_loop2_stride`：将数据从Global Memory搬运到L1 Buffer时，分别设置内层循环、外层循环中相邻迭代数据块间的间隔。
- `asc_set_gm2l1_pad`：将数据从Global Memory非对齐搬运到L1 Buffer时，设置连续搬运数据块左右两侧的填充值。

### 标量操作

- `asc_sflbits`：计算一个`int64_t`类型数字的二进制中，从最高数值位开始与符号位相同的连续比特位个数。
- `asc_ffz`、`asc_ffs`：分别获取一个`uint64_t`类型数字的二进制表示中，从最低有效位开始的第一个0、第一个1出现的位置，未找到时返回-1。
- `asc_popc`：获取一个`uint64_t`类型数字的二进制中1的个数。
- `asc_store_dev`：不经过数据缓存向Global Memory地址写数据。

### 矩阵计算

- `asc_set_l0c2gm_config`：数据搬运过程中进行随路量化时，设置量化流程中的矢量量化参数。
- `asc_set_l0c2gm_nz2nd`：数据搬运过程中进行NZ格式转换为ND格式时，设置格式转换的相关配置。
- `asc_mmad_mx`、`asc_mmad_mx_sync`：完成包含缩放功能的矩阵乘加操作，支持`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`输入组合。

### 同步控制

- `asc_sync`、`asc_sync_vec`：等待所有流水线操作完成。
- `asc_sync_mte2`、`asc_sync_mte3`：分别等待PIPE_MTE2流水、PIPE_MTE3流水完成。
- `asc_sync_notify`、`asc_sync_wait`：分别设置同步标志、等待同步标志。
- `asc_sync_pipe`：等待指定流水线操作完成。

### 系统变量

- `asc_get_ctrl`、`asc_set_ctrl`：分别读取、设置CTRL控制寄存器特定比特位上的值。
- `asc_get_block_num`：获取当前任务配置的核数，用于代码内部的多核逻辑控制等。
- `asc_get_system_cycle`：获取当前系统周期数。
- `asc_get_program_counter`：获取程序计数器的指针，程序计数器用于记录当前程序执行的位置。
- `asc_get_ar_spr`：读取指定特殊寄存器的值。

### 缓存控制

- `asc_icache_preload`、`asc_get_icache_preload_status`：分别从指令所在Global Memory地址预加载数据到对应的缓存行、获取指令缓存预加载状态。
- `asc_datacache_preload`：从源地址所在的特定Global Memory地址预加载数据到数据缓存。
- `asc_dcci_single`、`asc_ub_dcci_single`：分别刷新指定Global Memory地址或UB地址所在的单个缓存行（Cache Line）。
- `asc_dcci_entire_all`、`asc_dcci_entire_out`、`asc_dcci_entire_atomic`：分别以ALL、OUT或ATOMIC模式刷新全部缓存行，以维护标量单元访问与Global Memory之间的缓存一致性。

### 原子操作

- `asc_set_atomic_add_float16`、`asc_set_atomic_add_float`：设置后续从UB、L0C Buffer、L1 Buffer到Global Memory的数据搬运开启原子累加，累加的数据类型分别为`half`、`float`。
- `asc_set_atomic_max_float16`、`asc_set_atomic_max_float`：设置计算结果以原子比较方式搬运到Global Memory，将待搬运的`half`、`float`数据与Global Memory中已有数据逐元素比较，并将最大值写入Global Memory。
- `asc_set_store_atomic_config_v2`：设置原子操作启用位与原子操作类型的值。

### Reg数据搬运

提供使用`__simd_callee__`限定符的Reg数据搬运接口。

**掩码读取：**

- `asc_get_mask_spr_b16`、`asc_get_mask_spr_b32`：从特殊用途寄存器（Special Purpose Register，SPR）中读取掩码值，并按数据类型格式化后返回。

**数据搬入：**

- `asc_loadalign`：对齐数据搬运接口，从UB连续对齐搬入目的操作数，实现NORM搬入模式，搬运一个矢量长度（Vector Length，VL）的数据。
- `asc_loadalign_brc`：对齐数据搬运接口，从UB连续对齐搬入一个8位、16位或32位数据，并广播到目的操作数的所有元素位置。
- `asc_loadalign_brc_v2`：对齐数据搬运接口，从UB连续对齐搬入一个32字节数据块（DataBlock），并广播到一个矢量长度。
- `asc_loadalign_brc_v3`：对齐数据搬运接口，从UB连续对齐搬入数据，并将每个16位或32位元素广播到一个32字节数据块。
- `asc_loadalign_unpack`：对齐数据搬运接口，从UB连续对齐搬入目的操作数，实现解压缩搬入模式：按8位、16位或32位无符号整数加载半个矢量长度的数据，扩展到一个矢量长度的16位、32位或64位数据，中间位置补0。
- `asc_loadalign_unpack_v2`：对齐数据搬运接口，从UB连续对齐搬入目的操作数，实现解压缩搬入模式：按8位无符号整数加载四分之一个矢量长度的数据，扩展到一个矢量长度的32位数据，中间位置补0。
- `asc_loadalign_upsample`：对齐数据搬运接口，从UB连续对齐搬入目的操作数，实现2倍上采样搬入模式：加载半个矢量长度的数据，每个输入元素重复两次。
- `asc_loadalign_downsample`：对齐数据搬运接口，从UB连续对齐搬入目的操作数，实现2倍下采样搬入模式：加载2倍矢量长度的数据，数据每隔一个保留。
- `asc_loadalign_deintlv`：从UB中读取以32字节对齐地址为起始位置的连续2倍矢量长度数据，在加载过程中完成解交织后，分别写入两个目的矢量数据寄存器。
- `asc_loadalign_postupdate`、`asc_loadalign_brc_postupdate`、`asc_loadalign_brc_postupdate_v2`、`asc_loadalign_brc_postupdate_v3`：实现相应的对齐搬入模式并启用Post Update，接口调用后自动更新源操作数地址。
- `asc_loadalign_unpack_postupdate`、`asc_loadalign_unpack_postupdate_v2`、`asc_loadalign_upsample_postupdate`、`asc_loadalign_downsample_postupdate`、`asc_loadalign_deintlv_postupdate`：实现相应的解压缩、2倍上采样、2倍下采样或解交织搬入模式并启用Post Update，接口调用后自动更新源操作数地址。
- `asc_loadunalign_pre`：用于非对齐数据搬入前的初始化，需配合`asc_loadunalign`或`asc_loadunalign_postupdate`使用。
- `asc_loadunalign`、`asc_loadunalign_postupdate`：适用于从UB非32字节对齐地址连续搬入矢量数据寄存器的场景；后者支持自动更新源操作数地址。
- `asc_load`：支持从UB非32字节对齐的源地址搬运至矢量数据寄存器，搬运量为一个矢量长度。
- `asc_gather`：根据索引位置将源操作数按元素收集到目的操作数中。
- `asc_gather_datablock`：给定源操作数在UB中的基地址和索引，根据索引位置将源操作数按32字节数据块收集到目的操作数中。

**数据搬出：**

- `asc_storealign`：从矢量数据寄存器或掩码寄存器连续对齐搬出到UB，实现NORM搬出模式，搬运一个矢量长度的数据。
- `asc_storealign_1st`：从矢量数据寄存器搬出第一个元素到UB，忽略掩码。
- `asc_storealign_pack`：从矢量数据寄存器压缩搬出到UB，根据掩码将源操作数中有效元素的低半部分比特位连续存储于目的操作数中。
- `asc_storealign_pack_v2`：从矢量数据寄存器压缩搬出到UB，根据掩码将源操作数中有效元素的低8位连续存储于目的操作数中。
- `asc_storealign_intlv`：从矢量数据寄存器交错搬出到UB，忽略掩码，将两个源操作数中的元素交错存储于目的操作数中。
- `asc_storealign_postupdate`、`asc_storealign_1st_postupdate`：实现NORM搬出模式或搬出第一个元素，并启用Post Update，接口调用后自动更新目的操作数地址。
- `asc_storealign_pack_postupdate`、`asc_storealign_pack_postupdate_v2`：实现低半部分比特位或低8位压缩搬出，并启用Post Update，接口调用后自动更新目的操作数地址。
- `asc_storeunalign`、`asc_storeunalign_postupdate`：适用于从矢量数据寄存器连续搬出到UB非32字节对齐地址的场景；后者支持自动更新目的操作数地址。
- `asc_storeunalign_post`、`asc_storeunalign_post_postupdate`：适用于从矢量数据寄存器连续搬出到UB非32字节对齐地址的尾块场景；后者支持自动更新目的操作数地址。
- `asc_scatter`：根据索引值将源操作数中的元素分散到目的操作数中。
- `asc_store`：适用于从矢量数据寄存器搬出到UB的场景，不区分是否对齐；对齐场景推荐使用`asc_storealign`。

### Reg矢量计算

提供使用`__simd_callee__`限定符的Reg矢量计算接口。根据接口能力，支持的数据类型包括`bool`、`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`，具体支持组合以各接口说明为准。

**寄存器与掩码：**

- `asc_create_iter_reg_b8`、`asc_create_iter_reg_b16`、`asc_create_iter_reg_b32`：初始化地址寄存器，在循环中使用地址寄存器存储地址偏移量。
- `asc_create_mask_b8`、`asc_create_mask_b16`、`asc_create_mask_b32`：创建掩码寄存器，`pat_mode`参数指定哪些位置的元素参与计算。
- `asc_update_mask_b8`、`asc_update_mask_b16`、`asc_update_mask_b32`：根据元素个数生成掩码，并自动将元素个数减去当前矢量处理单元的元素个数。
- `asc_copy`：完成数据在寄存器间的搬运。

**计算能力：**

- 基础算术：`asc_add`、`asc_addc`、`asc_add_scalar`、`asc_sub`、`asc_subc`、`asc_mul`、`asc_mul_scalar`、`asc_madd`、`asc_mull`、`asc_max`、`asc_max_scalar`、`asc_min`、`asc_min_scalar`、`asc_abs`、`asc_neg`、`asc_abs_sub`、`asc_axpy`、`asc_relu`、`asc_leakyrelu`、`asc_prelu`、`asc_exp`、`asc_ln`、`asc_sqrt`、`asc_div`。
- 逻辑与移位：`asc_and`、`asc_or`、`asc_xor`、`asc_not`、`asc_shiftleft`、`asc_shiftleft_scalar`、`asc_shiftright`、`asc_shiftright_scalar`。
- 比较与选择：`asc_eq`、`asc_ne`、`asc_lt`、`asc_le`、`asc_gt`、`asc_ge`、`asc_eq_scalar`、`asc_ne_scalar`、`asc_lt_scalar`、`asc_le_scalar`、`asc_gt_scalar`、`asc_ge_scalar`、`asc_select`、`asc_squeeze`、`asc_squeeze_v2`。
- 类型转换接口分组如下。同组函数根据能力提供向下、向上、向零、最近偶数或最近远离零舍入模式，以及饱和或非饱和模式。
  - `int4b_t`、`fp4x2_e2m1_t`和`fp4x2_e1m2_t`转换：
    - `half`转换为`int4b_t`：`asc_half2int4x2_rd`、`asc_half2int4x2_rd_sat`、`asc_half2int4x2_rd_v2`、`asc_half2int4x2_rd_sat_v2`、`asc_half2int4x2_rd_v3`、`asc_half2int4x2_rd_sat_v3`、`asc_half2int4x2_rd_v4`、`asc_half2int4x2_rd_sat_v4`。9.0.0公开头文件中其他舍入模式的接口错误复用了上述向下舍入函数名。
    - `int4b_t`转换为其他类型：`asc_int4x22half`、`asc_int4x22bfloat16`、`asc_int4x22int16`。
    - `bfloat16_t`转换为`fp4x2_e2m1_t`：`asc_bfloat162e2m1x2_rd`、`asc_bfloat162e2m1x2_rd_v2`、`asc_bfloat162e2m1x2_rd_v3`、`asc_bfloat162e2m1x2_rd_v4`、`asc_bfloat162e2m1x2_rn`、`asc_bfloat162e2m1x2_rn_v2`、`asc_bfloat162e2m1x2_rn_v3`、`asc_bfloat162e2m1x2_rn_v4`、`asc_bfloat162e2m1x2_rna`、`asc_bfloat162e2m1x2_rna_v2`、`asc_bfloat162e2m1x2_rna_v3`、`asc_bfloat162e2m1x2_rna_v4`、`asc_bfloat162e2m1x2_ru`、`asc_bfloat162e2m1x2_ru_v2`、`asc_bfloat162e2m1x2_ru_v3`、`asc_bfloat162e2m1x2_ru_v4`、`asc_bfloat162e2m1x2_rz`、`asc_bfloat162e2m1x2_rz_v2`、`asc_bfloat162e2m1x2_rz_v3`、`asc_bfloat162e2m1x2_rz_v4`。
    - `fp4x2_e2m1_t`转换为`bfloat16_t`：`asc_e2m1x22bfloat16`。
    - `bfloat16_t`转换为`fp4x2_e1m2_t`：`asc_bfloat162e1m2x2_rd`、`asc_bfloat162e1m2x2_rd_v2`、`asc_bfloat162e1m2x2_rd_v3`、`asc_bfloat162e1m2x2_rd_v4`、`asc_bfloat162e1m2x2_rn`、`asc_bfloat162e1m2x2_rn_v2`、`asc_bfloat162e1m2x2_rn_v3`、`asc_bfloat162e1m2x2_rn_v4`、`asc_bfloat162e1m2x2_rna`、`asc_bfloat162e1m2x2_rna_v2`、`asc_bfloat162e1m2x2_rna_v3`、`asc_bfloat162e1m2x2_rna_v4`、`asc_bfloat162e1m2x2_ru`、`asc_bfloat162e1m2x2_ru_v2`、`asc_bfloat162e1m2x2_ru_v3`、`asc_bfloat162e1m2x2_ru_v4`、`asc_bfloat162e1m2x2_rz`、`asc_bfloat162e1m2x2_rz_v2`、`asc_bfloat162e1m2x2_rz_v3`、`asc_bfloat162e1m2x2_rz_v4`。
    - `fp4x2_e1m2_t`转换为`bfloat16_t`：`asc_e1m2x22bfloat16`。
  - `hifloat8_t`、`fp8_e5m2_t`和`fp8_e4m3fn_t`转换：
    - `hifloat8_t`转换：`asc_float2hif8_rh`、`asc_float2hif8_rh_sat`、`asc_float2hif8_rh_v2`、`asc_float2hif8_rh_sat_v2`、`asc_float2hif8_rh_v3`、`asc_float2hif8_rh_sat_v3`、`asc_float2hif8_rh_v4`、`asc_float2hif8_rh_sat_v4`、`asc_float2hif8_rna`、`asc_float2hif8_rna_sat`、`asc_float2hif8_rna_v2`、`asc_float2hif8_rna_sat_v2`、`asc_float2hif8_rna_v3`、`asc_float2hif8_rna_sat_v3`、`asc_float2hif8_rna_v4`、`asc_float2hif8_rna_sat_v4`、`asc_half2hif8_rh`、`asc_half2hif8_rh_sat`、`asc_half2hif8_rh_v2`、`asc_half2hif8_rh_sat_v2`、`asc_half2hif8_rna`、`asc_half2hif8_rna_sat`、`asc_half2hif8_rna_v2`、`asc_half2hif8_rna_sat_v2`、`asc_hif82half`。
    - `fp8_e5m2_t`转换：`asc_float2e5m2_rn`、`asc_float2e5m2_rn_sat`、`asc_float2e5m2_rn_v2`、`asc_float2e5m2_rn_sat_v2`、`asc_float2e5m2_rn_v3`、`asc_float2e5m2_rn_sat_v3`、`asc_float2e5m2_rn_v4`、`asc_float2e5m2_rn_sat_v4`、`asc_e5m22float`。
    - `fp8_e4m3fn_t`转换：`asc_float2e4m3_rn`、`asc_float2e4m3_rn_sat`、`asc_float2e4m3_rn_v2`、`asc_float2e4m3_rn_sat_v2`、`asc_float2e4m3_rn_v3`、`asc_float2e4m3_rn_sat_v3`、`asc_float2e4m3_rn_v4`、`asc_float2e4m3_rn_sat_v4`、`asc_e4m32float`。
    - `half`转换为`bfloat16_t`：`asc_half2bfloat16_rd`、`asc_half2bfloat16_rn`、`asc_half2bfloat16_rna`、`asc_half2bfloat16_ru`、`asc_half2bfloat16_rz`。
    - `float`转换为`bfloat16_t`：`asc_float2bfloat16_rd`、`asc_float2bfloat16_rd_sat`、`asc_float2bfloat16_rd_v2`、`asc_float2bfloat16_rd_sat_v2`、`asc_float2bfloat16_rd_sync`、`asc_float2bfloat16_rn`、`asc_float2bfloat16_rn_sat`、`asc_float2bfloat16_rn_v2`、`asc_float2bfloat16_rn_sat_v2`、`asc_float2bfloat16_rn_sync`、`asc_float2bfloat16_rna`、`asc_float2bfloat16_rna_sat`、`asc_float2bfloat16_rna_v2`、`asc_float2bfloat16_rna_sat_v2`、`asc_float2bfloat16_rna_sync`、`asc_float2bfloat16_ru`、`asc_float2bfloat16_ru_sat`、`asc_float2bfloat16_ru_v2`、`asc_float2bfloat16_ru_sat_v2`、`asc_float2bfloat16_ru_sync`、`asc_float2bfloat16_rz`、`asc_float2bfloat16_rz_sat`、`asc_float2bfloat16_rz_v2`、`asc_float2bfloat16_rz_sat_v2`、`asc_float2bfloat16_rz_sync`。
  - `half`、`bfloat16_t`和`float`之间的其他转换：`asc_half2float`、`asc_bfloat162half`、`asc_bfloat162float`、`asc_float2half`。
    - `int8_t`转换为`half`：`asc_int82half`。
    - `uint8_t`转换为`half`：`asc_uint82half`。
    - `int16_t`转换为浮点类型：`asc_int162half`、`asc_int162float`。
    - `half`转换为8位整数：`asc_half2int8`、`asc_half2uint8`。
    - `half`转换为`int16_t`：`asc_half2int16_rd`、`asc_half2int16_rd_sat`、`asc_half2int16_rd_sync`、`asc_half2int16_rn`、`asc_half2int16_rn_sat`、`asc_half2int16_rn_sync`、`asc_half2int16_rna`、`asc_half2int16_rna_sat`、`asc_half2int16_rna_sync`、`asc_half2int16_ru`、`asc_half2int16_ru_sat`、`asc_half2int16_ru_sync`、`asc_half2int16_rz`、`asc_half2int16_rz_sat`、`asc_half2int16_rz_sync`。
    - `half`转换为`int32_t`：`asc_half2int32_rd`、`asc_half2int32_rd_v2`、`asc_half2int32_rd_sync`、`asc_half2int32_rn`、`asc_half2int32_rn_v2`、`asc_half2int32_rn_sync`、`asc_half2int32_rna`、`asc_half2int32_rna_v2`、`asc_half2int32_rna_sync`、`asc_half2int32_ru`、`asc_half2int32_ru_v2`、`asc_half2int32_ru_sync`、`asc_half2int32_rz`、`asc_half2int32_rz_v2`、`asc_half2int32_rz_sync`。
    - `int32_t`转换为`float`：`asc_int322float`。
    - `float`转换为`int16_t`：`asc_float2int16_rd`、`asc_float2int16_rd_sat`、`asc_float2int16_rd_v2`、`asc_float2int16_rd_sat_v2`、`asc_float2int16_rd_sync`、`asc_float2int16_rn`、`asc_float2int16_rn_sat`、`asc_float2int16_rn_v2`、`asc_float2int16_rn_sat_v2`、`asc_float2int16_rn_sync`、`asc_float2int16_rna`、`asc_float2int16_rna_sat`、`asc_float2int16_rna_v2`、`asc_float2int16_rna_sat_v2`、`asc_float2int16_rna_sync`、`asc_float2int16_ru`、`asc_float2int16_ru_sat`、`asc_float2int16_ru_v2`、`asc_float2int16_ru_sat_v2`、`asc_float2int16_ru_sync`、`asc_float2int16_rz`、`asc_float2int16_rz_sat`、`asc_float2int16_rz_v2`、`asc_float2int16_rz_sat_v2`、`asc_float2int16_rz_sync`。
    - `float`转换为`int32_t`：`asc_float2int32_rd`、`asc_float2int32_rd_sat`、`asc_float2int32_rd_sync`、`asc_float2int32_rn`、`asc_float2int32_rn_sat`、`asc_float2int32_rn_sync`、`asc_float2int32_rna`、`asc_float2int32_rna_sat`、`asc_float2int32_rna_sync`、`asc_float2int32_ru`、`asc_float2int32_ru_sat`、`asc_float2int32_ru_sync`、`asc_float2int32_rz`、`asc_float2int32_rz_sat`、`asc_float2int32_rz_sync`。
    - `float`转换为`int64_t`：`asc_float2int64_rd`、`asc_float2int64_rd_sat`、`asc_float2int64_rd_v2`、`asc_float2int64_rd_sat_v2`、`asc_float2int64_rd_sync`、`asc_float2int64_rn`、`asc_float2int64_rn_sat`、`asc_float2int64_rn_v2`、`asc_float2int64_rn_sat_v2`、`asc_float2int64_rn_sync`、`asc_float2int64_rna`、`asc_float2int64_rna_sat`、`asc_float2int64_rna_v2`、`asc_float2int64_rna_sat_v2`、`asc_float2int64_rna_sync`、`asc_float2int64_ru`、`asc_float2int64_ru_sat`、`asc_float2int64_ru_v2`、`asc_float2int64_ru_sat_v2`、`asc_float2int64_ru_sync`、`asc_float2int64_rz`、`asc_float2int64_rz_sat`、`asc_float2int64_rz_v2`、`asc_float2int64_rz_sat_v2`、`asc_float2int64_rz_sync`。
    - `int64_t`转换为`float`：`asc_int642float_rd`、`asc_int642float_rd_v2`、`asc_int642float_rd_sync`、`asc_int642float_rn`、`asc_int642float_rn_v2`、`asc_int642float_rn_sync`、`asc_int642float_rna`、`asc_int642float_rna_v2`、`asc_int642float_rna_sync`、`asc_int642float_ru`、`asc_int642float_ru_v2`、`asc_int642float_ru_sync`、`asc_int642float_rz`、`asc_int642float_rz_v2`、`asc_int642float_rz_sync`。
  - 整数位宽和符号转换：`asc_int82int16`、`asc_int82int32`、`asc_uint82uint16`、`asc_uint82uint32`、`asc_int162uint8`、`asc_int162int32`、`asc_int162uint32`、`asc_uint162uint8`、`asc_uint162uint32`、`asc_int322uint8`、`asc_int322int16`、`asc_int322uint16`、`asc_int322int64`、`asc_uint322uint8`、`asc_uint322int16`、`asc_uint322uint16`、`asc_int642int32`。
  - 浮点取整：`asc_ceil`、`asc_floor`、`asc_rint`、`asc_round`、`asc_trunc`，结果仍保持源浮点数据类型。
- 归约：`asc_reduce_max`、`asc_reduce_min`、`asc_reduce_sum`、`asc_reduce_max_datablock`、`asc_reduce_min_datablock`、`asc_reduce_sum_datablock`、`asc_pair_reduce_sum`。
- 数据压缩：`asc_pack`、`asc_pack_v2`将源操作数元素的低8位、低16位或低32位写入目的操作数的低半部分或高半部分。
- 位宽扩展：`asc_unpack_lower`、`asc_unpack_upper`分别选取源操作数低半部分、高半部分的元素并扩充位宽。无符号整数高位填0，有符号整数保持符号位扩充，`bool`类型将每个比特位展开为2个比特位且高位填0。
- 掩码解压缩：`asc_unsqueeze`将目的操作数的第0个元素置为0，第i个元素设置为掩码中从第0个到第i-1个元素中1的数量。
- 填充与重排：`asc_duplicate`、`asc_duplicate_scalar`、`asc_intlv`、`asc_deintlv`。
- 累计直方图：`asc_cumulative_histogram_bin0`、`asc_cumulative_histogram_bin1`用于对直方图数据进行累计统计，分别统计源操作数中低位区间、高位区间的数据分布情况。
- 频率直方图：`asc_frequency_histogram_bin0`、`asc_frequency_histogram_bin1`对输入数据进行频率统计，分别统计低位区间、高位区间，统计结果在目的操作数原有数据基础上累加。
- 索引生成：`asc_arange`、`asc_arange_descend`以传入值为起始值，生成递增、递减的索引，并将生成的索引保存在目的操作数中。
- `asc_mem_bar`：Reg计算宏函数内不同流水线之间的同步指令，目的流水线等待源流水线上所有指令完成后执行。
- `asc_clear_ar_spr`：对AR特殊寄存器进行清零操作。

### 其他操作

- `asc_init`：初始化神经网络处理器（Neural-network Processing Unit，NPU）状态。
