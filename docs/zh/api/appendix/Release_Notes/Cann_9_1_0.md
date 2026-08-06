# C API

C API 3510系列新增标量原子操作、Cube数据搬运全链路、同步控制、系统变量等领域接口，同时对已有接口进行了数据类型扩展、参数增强和Bug修复。

## 特性

### 矢量数据搬运

**新增接口：**

- `asc_copy_gm2ub`/`asc_copy_gm2ub_sync`：将数据从Global Memory搬运到Unified Buffer。
- `asc_copy_ub2gm`/`asc_copy_ub2gm_sync`：将数据从Unified Buffer搬运到Global Memory。
- `asc_copy_ub2ub`/`asc_copy_ub2ub_sync`：将数据从Unified Buffer搬运到Unified Buffer。
- `asc_copy_ub2l1`/`asc_copy_ub2l1_sync`：将数据从Unified Buffer搬运到L1 Buffer。
- `asc_ndim_copy_dci`：带DCI的N维搬运。
- `asc_set_copy_pad_val`：通用填充配置，支持`int8_t`/`uint8_t`/`int16_t`/`uint16_t`/`half`/`bfloat16_t`/`int32_t`/`uint32_t`/`float`共9种数据类型。

**数据类型扩展：**

- `asc_copy_gm2ub_align`/`asc_copy_ub2gm_align`：数据类型从9种扩展到12种，新增`fp8_e5m2_t`/`fp8_e4m3fn_t`/`hifloat8_t`。
- `asc_ndim_copy_gm2ub`：新增`fp4x2_e2m1_t`/`fp4x2_e1m2_t`/`fp8_e8m0_t`类型支持。

### Reg矢量计算

**接口重载：**

- `asc_gather`：根据索引位置index将源操作数src按元素收集到目的操作数dst中，新增`vector_uint32_t index`重载。
- `asc_scatter`：根据mask对输入的src0、src1按元素进行分散存储，新增9个重载。
- `asc_loadalign`及各种特性场景（`_brc`/`_unpack`/`_upsample`/`_downsample`/`_deintlv`）：新增`int4x2_t`类型支持，新增`_postupdate`地址自更新版本。
- `asc_storealign`及各种特性场景（`_1st`/`_pack`/`_intlv`）：新增`iter_reg offset`参数重载，新增`_postupdate`地址自更新版本。
- `asc_storeunalign`/`asc_storeunalign_post`/`asc_storeunalign_postupdate`/`asc_storeunalign_post_postupdate`：新增16种数据类型的非对齐搬出重载。
- `asc_add_scalar`：按元素执行矢量和标量的加法运算，新增9种数据类型重载。
- `asc_exp`：对源操作数逐元素计算指数，新增`half`/`float`重载。
- `asc_min_scalar`：源操作数矢量内每个元素与标量比较，取较小值，新增`uint16_t`/`int8_t`/`uint8_t`/`float`/`half`重载。
- `asc_reduce_max_datablock`：找出DataBlock中的最大值，新增6种数据类型重载（无尾部下划线版本）。
- `asc_pack`：将源操作数中的元素选取低8位写入目的操作数，新增`uint16_t`->`uint8_t`重载。
- `asc_copy`：完成数据在寄存器内的搬运，新增`vector_bool`<-`vector_uint16_t`/`vector_uint32_t`重载。
- `asc_set_va_reg`：用于设置transpose的地址，将操作数地址序列与地址寄存器关联，新增8种数据类型重载。
- `asc_duplicate_scalar`：根据mask将value填充到目的操作数dst，新增不带mask的重载版本（12种数据类型）。

**数据类型扩展：**

- `asc_gather_datablock`：新增`int4x2_t`/`int64_t`/`uint64_t`类型支持。
- `asc_store`：新增`int4b_t`类型支持。
- `asc_loadalign`/`asc_storealign`：新增`int4x2_t`类型支持。

### 矩阵数据搬运

**新增接口：**

- `asc_copy_gm2l1`/`asc_copy_gm2l1_sync`：将数据从Global Memory搬运到L1 Buffer，支持`bfloat16_t`/`half`/`float`/`int32_t`/`int8_t`/`uint32_t`/`uint8_t`类型。
- `asc_copy_gm2l1_align`/`asc_copy_gm2l1_align_sync`：对齐GM到L1 Buffer搬运。
- `asc_copy_gm2l1_nd2nz`/`asc_copy_gm2l1_nd2nz_sync`：GM到L1 Buffer ND2NZ格式转换搬运。
- `asc_copy_gm2l1_dn2nz`/`asc_copy_gm2l1_dn2nz_sync`：GM到L1 Buffer DN2NZ格式转换搬运。
- `asc_copy_l12l0a`/`asc_copy_l12l0a_sync`：用于搬运存放在L1 Buffer里的512B大小的矩阵到L0A Buffer里，支持`bfloat16_t`/`fp8_e4m3fn_t`/`fp8_e5m2_t`/`half`/`float`/`hifloat8_t`/`int16_t`/`int32_t`/`int8_t`/`uint16_t`/`uint32_t`/`uint8_t`/`int4b_t`/`fp4x2_e2m1_t`/`fp4x2_e1m2_t`共14种数据类型，集成img2col功能。
- `asc_copy_l12l0b`/`asc_copy_l12l0b_sync`：用于搬运存放在L1 Buffer里的512B大小的矩阵到L0B Buffer里，同上14种数据类型。
- `asc_copy_l12l0a_trans`/`asc_copy_l12l0b_trans`/`asc_copy_l12l0a_trans_sync`/`asc_copy_l12l0b_trans_sync`：带转置的L1 Buffer到L0A Buffer/L0B Buffer搬运。
- `asc_copy_l12l0b_mx`/`asc_copy_l12l0b_mx_sync`：L1 Buffer到L0B Buffer搬运。
- `asc_copy_l0c2gm`/`asc_copy_l0c2gm_sync`：将L0C Buffer中的数据搬运到GM中，支持量化、ReLU、裁剪、广播、NZ2ND等随路操作。
- `asc_copy_l0c2ub`/`asc_copy_l0c2ub_sync`：将L0C Buffer中的数据搬运到UB中，支持量化、ReLU、裁剪、广播等随路操作。
- `asc_copy_gm2l0a`/`asc_copy_gm2l0b`/`asc_copy_gm2l0a_sync`/`asc_copy_gm2l0b_sync`：将数据从GM搬运到L0A Buffer/L0B Buffer。
- `asc_copy_l12fb`/`asc_copy_l12fb_sync`：将数据从L1 Buffer搬运到Fixpipe Buffer中。
- `asc_copy_l12bt`/`asc_copy_l12bt_sync`：将数据从L1 Buffer搬运到BiasTable Buffer中。
- `asc_copy_l12ub`/`asc_copy_l12ub_sync`：将数据从L1 Buffer搬运到UB中。
- `asc_fill_l0a`/`asc_fill_l0b`/`asc_fill_l1`/`asc_fill_l0a_sync`/`asc_fill_l0b_sync`/`asc_fill_l1_sync`：将L0A Buffer/L0B Buffer/L1 Buffer的Local Memory初始化为某一具体数值。
- `asc_load_image_to_cbuf`/`asc_load_image_to_cbuf_sync`：将图像数据从Global Memory搬运到L1 Buffer。
- `asc_copy_l12l0b_sparse`/`asc_copy_l12l0b_sparse_sync`：用于搬运存放在L1 Buffer里的512B大小的稠密权重矩阵到L0B Buffer里，同时读取128B大小的索引矩阵用于稠密矩阵的稀疏化。
- `asc_set_l13d_fmatrix_b`：3D fmatrix B参数配置。
- `asc_set_l0c2gm_lrelu_alpha`：L0C Buffer到GM LeakyReLU alpha参数配置。

### 矩阵计算

**新增接口：**

- `asc_mmad`/`asc_mmad_sync`：新增标准矩阵乘法，支持`bfloat16_t`/`fp8_e4m3fn_t`/`fp8_e5m2_t`/`half`/`float`/`int8_t`/`hifloat8_t`输入类型组合，共16个重载。使用`disable_gemv`参数替代9.0.0中`asc_mmad_mx`的`k_direction_align`参数。
- `asc_enable_fp8`：启用FP8计算模式。
- `asc_enable_hif8`：启用HiFloat8计算模式。

**数据类型扩展：**

- `asc_mmad_mx`：新增`fp8_e4m3fn_t`与`fp8_e5m2_t`、`fp8_e5m2_t`与`fp8_e4m3fn_t`等混合精度组合。

### 标量计算

**新增接口：**

新增完整的标量原子操作接口集，支持对Global Memory数据执行原子读写操作，覆盖`int32_t`/`uint32_t`/`float`/`int64_t`/`uint64_t`数据类型。

- `asc_atomic_add`：原子加，返回计算前的原始数据（5个重载）。
- `asc_atomic_sub`：原子减，返回计算前的原始数据（5个重载）。
- `asc_atomic_max`：原子最大值，返回计算前的原始数据（5个重载）。
- `asc_atomic_min`：原子最小值，返回计算前的原始数据（5个重载）。
- `asc_atomic_exch`：原子交换，返回计算前的原始数据（5个重载）。
- `asc_atomic_cas`：原子比较并交换，返回计算前的原始数据（5个重载）。
- `asc_atomic_and`：原子按位与（4个重载，不支持float）。
- `asc_atomic_or`：原子按位或（4个重载，不支持float）。
- `asc_atomic_xor`：原子按位异或（4个重载，不支持float）。
- `asc_atomic_inc`：原子自增（2个重载，仅`uint32_t`/`uint64_t`）。
- `asc_atomic_dec`：原子自减（2个重载，仅`uint32_t`/`uint64_t`）。

### 同步控制

**新增接口：**

- `asc_lock`/`asc_unlock`：异步流水线之间的同步（宏定义），支持阻塞和非阻塞模式（`ASC_LOCK_BLOCK`/`ASC_LOCK_NON_BLOCK`）。
- `asc_sync_intra_arrive`/`asc_sync_intra_wait`：核内到达/等待同步（宏定义）。
- `asc_sync_subblock_arrive`/`asc_sync_subblock_wait`：子Block级到达/等待同步（宏定义）。
- `asc_sync_inter_arrive`/`asc_sync_inter_wait`：核间到达/等待同步（宏定义）。

**接口签名变更：**

- `asc_sync_block_arrive`：9.0.0为函数声明`asc_sync_block_arrive(pipe_t pipe, uint8_t mode, int64_t flagID)`（3个参数），9.1.0改为宏定义`asc_sync_block_arrive(pipe, flag_id)`（2个参数，移除`mode`参数）。
- `asc_sync_block_wait`：9.0.0为函数声明`asc_sync_block_wait(int64_t flagID)`（1个参数），9.1.0改为宏定义`asc_sync_block_wait(pipe, flag_id)`（2个参数，新增`pipe`参数）。
- `asc_sync_data_barrier`：9.0.0为函数声明`asc_sync_data_barrier(mem_dsb_t arg)`，9.1.0改为宏定义`asc_sync_data_barrier(arg)`。

### 系统变量

**新增接口：**

- `asc_get_smmu_tag_version`：获取SMMU标签版本。
- `asc_get_phy_stack_base`：获取物理栈基地址。
- `asc_get_status`：获取系统状态。
- `asc_get_sys_virtual_base`：获取系统虚拟基地址。
- `asc_get_vf_len`：获取矢量长度（`constexpr`，编译期求值）。
- `asc_set_l0c2gm_channel_para`：L0C Buffer到GM通道参数配置。
- `asc_set_l3d_rpt_b`：L1 3D重复B配置。
- `asc_set_gm2l1_nz_para`：GM到L1 Buffer NZ参数配置。
- `asc_set_l13d_padding`：L1 3D填充配置（4个重载：`uint64_t`/`half`/`int16_t`/`uint16_t`）。
- `asc_set_l12l0_padding_val`：L1 Buffer到L0填充值配置。
- `asc_set_l0c2gm_quant_post`：L0C Buffer到GM后量化配置。
- `asc_set_l0c2gm_relu_alpha`：L0C Buffer到GM ReLU alpha配置。

### 缓存控制

**新增接口：**

- `asc_dci`：数据缓存失效（Data Cache Invalidate）指令。

## Bug修复

### 函数命名错误修复

- `asc_half2int4x2`：9.0.0中`ru`/`rz`/`rn`/`rna`四种舍入模式的函数声明均错误命名为`asc_half2int4x2_rd`，9.1.0已修正为各自的正确名称（`_ru`/`_rz`/`_rn`/`_rna`）。
- `asc_int162half`：9.0.0中存在`asc_int162half_rc`（舍入模式`rc`不存在），9.1.0修正为`asc_int162half_rz`。
- `asc_reduce_min_datablock_`：9.0.0中函数名带尾部下划线`asc_reduce_min_datablock_`，9.1.0修正为`asc_reduce_min_datablock`（去除尾部下划线，与`asc_reduce_max_datablock`命名风格统一）。

### 类型名称修复

- `asc_bfloat162e1m2x2`：9.0.0中目的操作数类型为`vector_f4e1m2x2`，9.1.0修正为`vector_fp4x2_e1m2_t`。
- `asc_half2uint8`：9.0.0中使用`vector_u8`/`vector_f16`类型别名，9.1.0修正为标准类型名`vector_uint8_t`/`vector_half`。
- `asc_min_scalar`（half重载）：9.0.0中源操作数类型错误写为`vector_f16`，9.1.0修正为`vector_half`。

### 参数命名统一

- `asc_get_arch_ver`：9.0.0中参数名为`coreVersion`（驼峰命名），9.1.0修正为`core_version`（蛇形命名）。
- `asc_arange`/`asc_arange_descend`：9.0.0中参数名为`index`，9.1.0修正为`value`。
- `asc_update_mask_b8`/`asc_update_mask_b16`/`asc_update_mask_b32`：9.0.0中参数名为`scalar`，9.1.0修正为`value`。

## 增强

**参数增强：**

- `asc_copy_gm2ub_align`/`asc_copy_ub2gm_align`：新增`enable_constant_pad`/`l2_cache_mode`参数，支持常量填充模式和L2缓存控制。
- `asc_copy_gm2ub_align`/`asc_copy_ub2gm_align`：新增size-only简化重载（12种数据类型），仅需`dst`/`src`/`size`三个参数。
- `asc_create_iter_reg_b8`/`asc_create_iter_reg_b16`/`asc_create_iter_reg_b32`：地址寄存器初始化，从单维偏移扩展到支持1~4维偏移参数。
- `asc_create_iter_reg_b8`/`asc_create_iter_reg_b16`/`asc_create_iter_reg_b32`：地址寄存器初始化，从单维偏移扩展到支持1~4维偏移参数。

## 接口废弃

本版本标记以下接口为**deprecated**，建议使用替代接口：

| 废弃接口 | 替代接口 | 说明 |
| :--- | :--- | :--- |
| `asc_get_store_atomic_config` | `asc_atomic_add`等 | 原子操作查询接口废弃，改用标量原子操作接口 |
| `asc_set_store_atomic_config_v2` | `asc_atomic_add`等 | 原子操作配置接口废弃，改用标量原子操作接口 |
| `asc_exp_sub_v2` | `asc_exp_sub` | 指数减法V2版本废弃 |
| `asc_set_gm2ub_pad` | `asc_set_copy_pad_val` | GM到UB填充配置废弃，改用通用填充值配置接口 |

**注：** 详情可参考[废弃接口](../deprecated_interface.md)。
