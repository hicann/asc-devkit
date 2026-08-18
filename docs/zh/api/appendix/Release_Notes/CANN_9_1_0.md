# C API

CANN 9.1.0继续完善Ascend 950PR/Ascend 950DT（3510架构）C API，新增矢量计算、数据搬运、标量操作、矩阵计算、同步控制、系统变量和缓存控制接口，并修复9.0.0中的函数命名、类型声明和地址自更新签名问题。

## 新增特性

### 矢量计算

- `asc_bitsort`、`asc_bitsort_sync`：分数（Score）和索引（Index）分别存储在两个源操作数中，按分数从大到小排序，将排序后的分数与对应索引组成数据结构存储在目的操作数中。
- `asc_mrgsort4`、`asc_mrgsort4_sync`：将已经排好序的最多4条队列合并排列成1条队列，结果按照分数域由大到小排序。
- `asc_transpose`、`asc_transpose_sync`：实现16×16的二维矩阵数据块转置。
- `asc_transto5hd_b8`、`asc_transto5hd_b8_sync`、`asc_transto5hd_b16`、`asc_transto5hd_b16_sync`、`asc_transto5hd_b32`、`asc_transto5hd_b32_sync`：执行数据格式转换，一般用于将NCHW格式转换成NC1HWC0格式，也可用于二维矩阵数据块转置。
- `asc_set_va_reg`：用于设置`asc_transto5hd_b8`的地址，将操作数地址序列与地址寄存器关联。

### 数据搬运

- `asc_copy_gm2ub`、`asc_copy_gm2ub_sync`：将数据从Global Memory搬运到Unified Buffer（UB）。
- `asc_copy_ub2gm`、`asc_copy_ub2gm_sync`：将数据从UB搬运到Global Memory。
- `asc_copy_ub2ub`、`asc_copy_ub2ub_sync`：将数据从UB搬运到UB。
- `asc_copy_ub2l1`、`asc_copy_ub2l1_sync`：将数据从UB搬运到L1 Buffer。
- `asc_set_copy_pad_val`：与`asc_copy_gm2ub_align`配合使用，设置连续搬运数据块左右两侧需要填充的数据值。Ascend 950PR/Ascend 950DT支持`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`数据需要先转换为`int8_t`再传入。
- `asc_ndim_copy_dci`：执行N维搬运场景所需的数据缓存失效（Data Cache Invalidate，DCI）操作。
- `asc_copy_gm2l1`、`asc_copy_gm2l1_sync`：将数据从Global Memory搬运到L1 Buffer。
- `asc_copy_gm2l1_align`、`asc_copy_gm2l1_align_sync`：将Global Memory中的数据填充后搬运到L1 Buffer。
- `asc_copy_gm2l1_nd2nz`、`asc_copy_gm2l1_nd2nz_sync`：在Global Memory到L1 Buffer搬运过程中完成ND到NZ格式转换。
- `asc_copy_gm2l1_dn2nz`、`asc_copy_gm2l1_dn2nz_sync`：将矩阵数据从Global Memory搬运到L1 Buffer，并完成DN到NZ、NCHW到NC1HWC0或NCHW到C1HWNC0格式转换。D维未按C0大小对齐时，在L1 Buffer最内层维度填充0。
- `asc_copy_l12l0a`、`asc_copy_l12l0a_sync`、`asc_copy_l12l0b`、`asc_copy_l12l0b_sync`：将L1 Buffer中的512字节矩阵搬运到L0A Buffer或L0B Buffer，支持2D格式搬运、2D格式转置搬运、3D格式搬运。
- `asc_copy_l12l0b_trans`、`asc_copy_l12l0b_trans_sync`：将矩阵从L1 Buffer转置搬运到L0B Buffer。
- `asc_copy_l12l0b_mx`、`asc_copy_l12l0b_mx_sync`：将微缩放（Microscaling，MX）矩阵从L1 Buffer搬运到L0B Buffer。
- `asc_copy_l0c2gm`、`asc_copy_l0c2gm_sync`：将L0C Buffer中的数据搬运到Global Memory，支持量化、ReLU、Leaky ReLU、通道拆分和NZ2ND随路控制。
- `asc_copy_l0c2ub`、`asc_copy_l0c2ub_sync`：将L0C Buffer中的数据搬运到UB，支持量化、ReLU、Leaky ReLU和通道拆分随路控制。
- `asc_copy_l12fb`、`asc_copy_l12fb_sync`：将量化参数从L1 Buffer搬运到Fixpipe Buffer。
- `asc_copy_l12bt`、`asc_copy_l12bt_sync`：将矩阵乘使用的偏置（Bias）数据从L1 Buffer搬运到BiasTable Buffer。
- `asc_copy_l12ub`、`asc_copy_l12ub_sync`：将数据从L1 Buffer搬运到UB。
- `asc_fill_l1`、`asc_fill_l1_sync`：将`value`的二进制值赋值给目的操作数，对L1 Buffer的Local Memory进行初始化。
- `asc_set_l13d_fmatrix_b`：设置特征图属性描述，用于`asc_copy_l12l0a`、`asc_copy_l12l0b`的3D格式搬运接口配置填充值。从右矩阵获取特征图属性时使用该接口。
- `asc_set_l0c2gm_lrelu_alpha`：用于设置`asc_copy_l0c2l1`或`asc_copy_l0c2gm`计算过程中使用的Leaky ReLU alpha值，该值只支持`half`、`float`两种数据类型。
- `asc_set_l13d_padding`：设置填充属性描述，用于调用`asc_copy_l12l0a`时配置填充值。

### 标量操作

- `asc_clear_nthbit`、`asc_set_nthbit`：清除或设置`uint64_t`值中的指定比特位。
- `asc_atomic_add`、`asc_atomic_sub`、`asc_atomic_max`、`asc_atomic_min`、`asc_atomic_exch`、`asc_atomic_cas`：对Global Memory中的`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`数据执行标量原子操作，并返回操作前的值。
- `asc_atomic_and`、`asc_atomic_or`、`asc_atomic_xor`：对Global Memory中的`int32_t`、`uint32_t`、`int64_t`、`uint64_t`数据执行原子按位运算。
- `asc_atomic_inc`：对Global Memory中的`uint32_t`或`uint64_t`计数器执行原子递增操作；计数器数值大于等于`val`时赋值为0，否则加1。
- `asc_atomic_dec`：对Global Memory中的`uint32_t`或`uint64_t`计数器执行原子递减操作；计数器数值等于0或大于`val`时赋值为`val`，否则减1。

### 矩阵计算

- `asc_mmad`、`asc_mmad_sync`：完成矩阵乘加操作，支持`int8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`float`同精度和混合精度输入组合。
- `asc_enable_fp8`：设置矩阵乘加计算开启FP8模式并关闭HiF8模式，L0A Buffer、L0B Buffer中的FP8数据在参与矩阵乘加计算之前不会转换为`hifloat8_t`数据。
- `asc_enable_hif8`：设置矩阵乘加计算开启HiF8模式，L0A Buffer、L0B Buffer中的8位数据在参与矩阵乘法运算前转换为`hifloat8_t`数据。
- `asc_enable_hf32_trans`：设置HF32模式舍入方式，使用该接口前需要先使用`asc_enable_hf32`开启HF32模式。
- `asc_enable_hf32`：设置矩阵乘加计算开启HF32模式，提升FP32数据的计算性能，但会带来一定的精度损失。
- `asc_set_fp32_mode`：设置矩阵乘加计算开启FP32模式，L0A Buffer、L0B Buffer中的FP32数据在参与矩阵乘加计算之前不做舍入处理。
- `asc_set_mmad_direction_m`、`asc_set_mmad_direction_n`：分别设置矩阵乘加计算优先通过M方向、N方向生成结果。

### 同步控制

- `asc_lock`、`asc_unlock`：用于AI Core内部异步流水线同步。
- `asc_sync_intra_arrive`、`asc_sync_intra_wait`：用于核间信号同步。
- `asc_sync_subblock_arrive`、`asc_sync_subblock_wait`：用于组内子块（Subblock）间的信号同步。
- `asc_sync_inter_arrive`、`asc_sync_inter_wait`：用于组间计算块（Block）的信号同步。
- `asc_sync_data_barrier`：等待指定范围内先前发起的内存访问完成。

### 系统变量

- `asc_get_smmu_tag_version`、`asc_get_phy_stack_base`、`asc_get_status`、`asc_get_sys_virtual_base`：分别获取系统内存管理单元（System Memory Management Unit，SMMU）版本信息、物理堆栈基地址、状态信息和系统虚拟基地址。
- `asc_get_block_idx`：获取当前运行核的索引。
- `asc_get_sub_block_num`：在分离模式下获取一个AI Core上的Cube Core或Vector Core数量。
- `asc_get_sub_block_id`：获取AI Core上的Vector Core ID。
- `asc_get_phy_buf_addr`：根据偏移量获取片上实际物理地址。
- `asc_get_core_id`、`asc_get_arch_ver`：分别获取核ID和架构版本。
- `asc_get_vf_len`：获取Tensor位宽VL（Vector Length）的大小。
- `asc_set_gm2l1_nz_para`：设置MTE2_NZ_PARA寄存器的值。
- `asc_set_l0c2gm_channel_para`：设置通道步长参数专用寄存器的比特位。
- `asc_set_l0c2gm_quant_post`：设置QUANT_POST寄存器的值。试验接口。
- `asc_set_l0c2gm_relu_alpha`：设置RELU_ALPHA寄存器的值。
- `asc_set_l12l0_padding_val`：设置PADDING_B寄存器的值。
- `asc_set_l3d_rpt_b`：用于设置`asc_copy_l12l0a`、`asc_copy_l12l0b`的2D格式搬运repeat参数。

### 缓存控制

- `asc_dci`：使整个数据缓存失效，不回写缓存行，未同步到主存的修改会被丢弃。调用前必须先执行`asc_sync_data_barrier(mem_dsb_t::DSB_ALL)`，等待此前的内存访问完成。

## Bug修复

- `asc_sync_block_arrive`由9.0.0的`asc_sync_block_arrive(pipe, mode, flagID)`调整为`asc_sync_block_arrive(pipe, flag_id)`，删除`mode`参数并统一事件标号命名。
- `asc_sync_block_wait`由9.0.0的`asc_sync_block_wait(flagID)`调整为`asc_sync_block_wait(pipe, flag_id)`，新增流水线参数并统一事件标号命名。
- `asc_get_arch_ver`参数名由`coreVersion`统一为`core_version`。
- 修复`cache_ctrl.h`复用原子操作头文件保护宏的问题，避免包含顺序导致缓存控制声明被跳过。
- 统一`asc_simd.h`、`misc.h`、`cache_ctrl.h`、`sync.h`、`sys_var.h`和`utils_intf.h`中的头文件引用路径，避免安装目录变化导致包含失败。
- 将`asc_loadalign_postupdate`、`asc_loadalign_brc_postupdate`、`asc_loadalign_brc_postupdate_v2`、`asc_loadalign_brc_postupdate_v3`、`asc_loadalign_upsample_postupdate`、`asc_loadalign_downsample_postupdate`、`asc_loadalign_unpack_postupdate`、`asc_loadalign_unpack_postupdate_v2`、`asc_loadalign_deintlv_postupdate`中需要自动更新的UB源指针参数修正为指针引用，确保调用后地址变化对调用方可见。
- 将`asc_storealign_postupdate`、`asc_storealign_1st_postupdate`、`asc_storealign_pack_postupdate`、`asc_storealign_pack_postupdate_v2`中需要自动更新的UB目的指针参数修正为指针引用，确保调用后地址变化对调用方可见。
- 将不更新地址的`asc_storeunalign`目的参数由指针引用修正为普通指针，并补齐`asc_storeunalign_postupdate`的地址更新重载。
- 修复`half`转换为`int4b_t`接口的舍入模式命名。9.0.0将以下四种舍入模式的接口声明为向下舍入函数名，9.1.0修复为与实际舍入模式一致的函数名：
  - 向上舍入：`asc_half2int4x2_ru`、`asc_half2int4x2_ru_sat`、`asc_half2int4x2_ru_v2`、`asc_half2int4x2_ru_sat_v2`、`asc_half2int4x2_ru_v3`、`asc_half2int4x2_ru_sat_v3`、`asc_half2int4x2_ru_v4`、`asc_half2int4x2_ru_sat_v4`。
  - 向零舍入：`asc_half2int4x2_rz`、`asc_half2int4x2_rz_sat`、`asc_half2int4x2_rz_v2`、`asc_half2int4x2_rz_sat_v2`、`asc_half2int4x2_rz_v3`、`asc_half2int4x2_rz_sat_v3`、`asc_half2int4x2_rz_v4`、`asc_half2int4x2_rz_sat_v4`。
  - 最近偶数舍入：`asc_half2int4x2_rn`、`asc_half2int4x2_rn_sat`、`asc_half2int4x2_rn_v2`、`asc_half2int4x2_rn_sat_v2`、`asc_half2int4x2_rn_v3`、`asc_half2int4x2_rn_sat_v3`、`asc_half2int4x2_rn_v4`、`asc_half2int4x2_rn_sat_v4`。
  - 最近远离零舍入：`asc_half2int4x2_rna`、`asc_half2int4x2_rna_sat`、`asc_half2int4x2_rna_v2`、`asc_half2int4x2_rna_sat_v2`、`asc_half2int4x2_rna_v3`、`asc_half2int4x2_rna_sat_v3`、`asc_half2int4x2_rna_v4`、`asc_half2int4x2_rna_sat_v4`。
- 将`asc_int162half_rc`修正为`asc_int162half_rz`。
- 将`asc_reduce_min_datablock_`修正为`asc_reduce_min_datablock`，去除多余的尾部下划线。
- 将`asc_half2uint8_rd_sat_v2_impl`、`asc_half2uint8_rn_sat_v2_impl`、`asc_half2uint8_rna_sat_v2_impl`、`asc_half2uint8_ru_sat_v2_impl`、`asc_half2uint8_rz_sat_v2_impl`分别修正为`asc_half2uint8_rd_sat_v2`、`asc_half2uint8_rn_sat_v2`、`asc_half2uint8_rna_sat_v2`、`asc_half2uint8_ru_sat_v2`、`asc_half2uint8_rz_sat_v2`。
- 将以下`bfloat16_t`转换为`fp4x2_e1m2_t`接口的目的类型从`vector_f4e1m2x2`修正为`vector_fp4x2_e1m2_t`：
  - 向下舍入：`asc_bfloat162e1m2x2_rd`、`asc_bfloat162e1m2x2_rd_v2`、`asc_bfloat162e1m2x2_rd_v3`、`asc_bfloat162e1m2x2_rd_v4`。
  - 最近偶数舍入：`asc_bfloat162e1m2x2_rn`、`asc_bfloat162e1m2x2_rn_v2`、`asc_bfloat162e1m2x2_rn_v3`、`asc_bfloat162e1m2x2_rn_v4`。
  - 最近远离零舍入：`asc_bfloat162e1m2x2_rna`、`asc_bfloat162e1m2x2_rna_v2`、`asc_bfloat162e1m2x2_rna_v3`、`asc_bfloat162e1m2x2_rna_v4`。
  - 向上舍入：`asc_bfloat162e1m2x2_ru`、`asc_bfloat162e1m2x2_ru_v2`、`asc_bfloat162e1m2x2_ru_v3`、`asc_bfloat162e1m2x2_ru_v4`。
  - 向零舍入：`asc_bfloat162e1m2x2_rz`、`asc_bfloat162e1m2x2_rz_v2`、`asc_bfloat162e1m2x2_rz_v3`、`asc_bfloat162e1m2x2_rz_v4`。
- 将`asc_half2uint8`和`asc_min_scalar`的非标准类型别名`vector_u8`、`vector_f16`修正为`vector_uint8_t`、`vector_half`。
- `asc_arange`、`asc_arange_descend`参数名由`index`统一为`value`。
- `asc_update_mask_b8`、`asc_update_mask_b16`、`asc_update_mask_b32`参数名由`scalar`统一为`value`。

## 增强

- `asc_copy_gm2ub_align`、`asc_copy_gm2ub_align_sync`、`asc_copy_ub2gm_align`、`asc_copy_ub2gm_align_sync`新增仅传入`dst`、`src`和`size`的简化重载。
- `asc_copy_l0c2l1`新增`asc_copy_l0c2l1_sync`，并新增`int16_t`目的数据与`int32_t`源数据、`bfloat16_t`目的数据与`float`源数据的组合，以及目的参数为`__cbuf__ void*`、源参数分别为`__cc__ int32_t*`或`__cc__ float*`的通用重载。
- `asc_copy_l12l0a_mx`新增`asc_copy_l12l0a_mx_sync`。
- `asc_set_atomic_add_int8`、`asc_set_atomic_add_int16`、`asc_set_atomic_add_int`：设置后续从UB、L0C Buffer到Global Memory的数据搬运开启原子累加，累加的数据类型分别为`int8_t`、`int16_t`、`int32_t`。
- `asc_gather`为`int16_t`、`uint16_t`、`half`、`bfloat16_t`数据新增使用`vector_uint32_t`索引的重载。
- `asc_gather_datablock`新增`int4b_t`重载和无需传入掩码参数的`uint64_t`重载。
- `asc_loadalign`、`asc_loadalign_brc`、`asc_loadalign_brc_v2`、`asc_loadalign_upsample`、`asc_loadalign_downsample`、`asc_loadalign_unpack`、`asc_loadalign_unpack_v2`、`asc_loadalign_deintlv`新增`int4b_t`、`vector_int4x2_t`重载。
- `asc_loadalign`、`asc_loadalign_upsample`、`asc_loadalign_downsample`、`asc_loadalign_postupdate`新增`vector_bool`重载。
- `asc_storealign`、`asc_storealign_1st`、`asc_storealign_intlv`、`asc_storeunalign`、`asc_storeunalign_postupdate`、`asc_storeunalign_post`、`asc_storeunalign_post_postupdate`、`asc_store`新增`int4b_t`、`vector_int4x2_t`数据重载。
- `asc_storealign`、`asc_storealign_pack`、`asc_storealign_postupdate`、`asc_storealign_pack_postupdate`、`asc_storeunalign_postupdate`新增`vector_bool`输出重载。
- `asc_scatter`新增9组目的UB地址在前、源矢量寄存器在后的重载，根据索引将源矢量寄存器元素分散写入UB。支持的数据类型为`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。
- `asc_create_iter_reg_b8`、`asc_create_iter_reg_b16`、`asc_create_iter_reg_b32`从单个偏移扩展为支持1至4个偏移。
- `asc_copy`新增将`vector_uint16_t`、`vector_uint32_t`矢量数据寄存器复制到`vector_bool`掩码寄存器的重载。
- `asc_duplicate_scalar`新增无需传入掩码参数的重载。

## 接口废弃

本版本标记以下接口或特定重载为`deprecated`：

### 数据搬运

| 废弃接口或重载 | 替代接口 | 说明 |
| :--- | :--- | :--- |
| `asc_set_gm2ub_pad` | `asc_set_copy_pad_val` | 使用通用搬运填充值配置接口。 |
| `asc_get_store_atomic_config`（Ascend 950PR/Ascend 950DT） | `asc_atomic_add` | 标量原子加无需预先配置和查询数据搬运原子状态。 |
| `asc_set_store_atomic_config_v2` | `asc_atomic_add` | 标量原子加改为直接调用原子接口。 |
| 源矢量寄存器在前、目的UB地址在后的`asc_scatter`重载 | 目的UB地址在前、源矢量寄存器在后的`asc_scatter`重载 | 支持的数据类型不变，仅调整参数顺序。 |
| `asc_exp_sub_v2(vector_float&, vector_float, vector_float, vector_bool)` | `asc_exp_sub` | `half`输入的`asc_exp_sub_v2`仍保留，用于读取源操作数的奇数索引元素；仅`float`输入的该重载废弃。 |

**注：** 详情可参考[废弃接口](../deprecated_interface.md)。
