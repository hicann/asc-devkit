# API列表

C API文档按最细粒度公共头文件归类。除试验接口外，整体使用时可以包含`c_api/asc_simd.h`。

## 基础定义

### 枚举

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_hf32_round_mode](defs/enum/asc_hf32_round_mode.md)|表示Mmad计算开启HF32模式时由FP32舍入到HF32的舍入模式管理策略。|`c_api/defs/enum.h`|
|[asc_load_l2_cache_mode](defs/enum/asc_load_l2_cache_mode.md)|表示数据从GM搬运到UB时的L2 cache策略，也可用于标量load策略配置。|`c_api/defs/enum.h`|
|[asc_override_strategy](defs/enum/asc_override_strategy.md)|表示饱和控制策略。|`c_api/defs/enum.h`|
|[asc_saturation_mode](defs/enum/asc_saturation_mode.md)|表示饱和控制模式。|`c_api/defs/enum.h`|
|[asc_store_l2_cache_mode](defs/enum/asc_store_l2_cache_mode.md)|表示数据从UB搬运到GM时的L2 cache策略，也可用于标量store策略配置。|`c_api/defs/enum.h`|

### 联合体

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_fill_value_config](defs/union/asc_fill_value_config.md)|fill_value的初始化参数结构体，包含[asc_fill_l0a](cube_datamove/asc_fill_l0a.md)/[asc_fill_l0b](cube_datamove/asc_fill_l0b.md)/[asc_fill_l1](cube_datamove/asc_fill_l1.md)接口需要配置的各种初始化参数。|`c_api/defs/union.h`|
|[asc_l13d_fmatrix_config](defs/union/asc_l13d_fmatrix_config.md)|用于设置[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md)3D格式搬运接口的Feature map属性参数。|`c_api/defs/union.h`|
|[asc_load3d_v2_config](defs/union/asc_load3d_v2_config.md)|Load3Dv2接口的repeat参数。|`c_api/defs/union.h`|
|[asc_ndim_pad_count_config](defs/union/asc_ndim_pad_count_config.md)|用于[asc_set_ndim_pad_count](vector_datamove/asc_set_ndim_pad_count.md)接口中，设置[asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md)接口的各个维度左右侧的padding元素个数。|`c_api/defs/union.h`|
|[asc_store_atomic_config](defs/union/asc_store_atomic_config.md)|原子操作启用位与原子操作类型的值。|`c_api/defs/union.h`|

### 数据类型

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[reg数据类型定义](defs/type/data_type_definition.md)|介绍C API的矢量数据寄存器、掩码寄存器、非对齐寄存器和地址寄存器数据类型。|`c_api/defs/type.h`|

### 常量

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[ASC_C_API_MRGSORT_ELEMENT_LEN](defs/constant/asc_c_api_mrgsort_element_len.md)|MrgSort4操作支持的队列数量。|`c_api/defs/constant.h`|
|[ASC_VDEQ_SIZE](defs/constant/asc_vdeq_size.md)|asc_set_deq_scale接口中量化参数数组的元素个数。|`c_api/defs/constant.h`|

## 工具接口

### 系统初始化

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_init](utils/sys_init/asc_init.md)|初始化NPU状态。|`c_api/utils/sys_init.h`|

### 系统其他操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_nop](utils/sys_misc/asc_nop.md)|生成一条空操作指令，不执行实际计算。|`c_api/utils/sys_misc.h`|

### 系统资源和变量

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_get_core_id](utils/sys_var/asc_get_core_id.md)|获取当前核的编号。|`c_api/utils/sys_var.h`|
|[asc_get_ffts_base_addr](utils/sys_var/asc_get_ffts_base_addr.md)|获取核间同步寄存器的基地址。|`c_api/utils/sys_var.h`|
|[asc_get_phy_buf_addr](utils/sys_var/asc_get_phy_buf_addr.md)|基于偏移量获取片上实际物理地址。|`c_api/utils/sys_var.h`|
|[asc_get_phy_stack_base](utils/sys_var/asc_get_phy_stack_base.md)|获取物理堆栈基地址。|`c_api/utils/sys_var.h`|
|[asc_get_program_counter](utils/sys_var/asc_get_program_counter.md)|获取程序计数器的指针，程序计数器用于记录当前程序执行的位置。|`c_api/utils/sys_var.h`|
|[asc_get_smmu_tag_version](utils/sys_var/asc_get_smmu_tag_version.md)|获取SMMU（System Memory Management Unit）版本信息。|`c_api/utils/sys_var.h`|
|[asc_get_status](utils/sys_var/asc_get_status.md)|获取状态信息。|`c_api/utils/sys_var.h`|
|[asc_get_sub_block_id](utils/sys_var/asc_get_sub_block_id.md)|获取AI Core上Vector核的ID。|`c_api/utils/sys_var.h`|
|[asc_get_sub_block_num](utils/sys_var/asc_get_sub_block_num.md)|分离模式下，获取一个AI Core上Cube Core（AIC）或者Vector Core（AIV）的数量。|`c_api/utils/sys_var.h`|
|[asc_get_sys_virtual_base](utils/sys_var/asc_get_sys_virtual_base.md)|获取系统虚拟基地址。|`c_api/utils/sys_var.h`|
|[asc_get_system_cycle](utils/sys_var/asc_get_system_cycle.md)|获取当前系统cycle数。|`c_api/utils/sys_var.h`|
|[asc_get_vf_len](utils/sys_var/asc_get_vf_len.md)|获取Tensor位宽VL（Vector Length）的大小。|`c_api/utils/sys_var.h`|
|[asc_set_ffts_base_addr](utils/sys_var/asc_set_ffts_base_addr.md)|在[asc_sync_block_arrive](sync/asc_sync_block_arrive.md)和[asc_sync_block_wait](sync/asc_sync_block_wait.md)之前使用，设置核间同步寄存器的基地址。|`c_api/utils/sys_var.h`|

## 矩阵计算

### MMAD

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_enable_fp8](cube_compute/asc_enable_fp8.md)|用于设置Mmad计算开启FP8模式，开启该模式后L0A Buffer/L0B Buffer中的FP8数据在参与Mmad计算之前不会被转化为hifloat8_t类型数据。|`c_api/cube_compute/cube_compute.h`|
|[asc_enable_hf32](cube_compute/asc_enable_hf32.md)|用于设置Mmad计算开启HF32模式，开启该模式后L0A Buffer/L0B Buffer中的FP32数据将在参与Mmad计算之前被舍入为HF32。|`c_api/cube_compute/cube_compute.h`|
|[asc_enable_hf32_trans](cube_compute/asc_enable_hf32_trans.md)|设置HF32模式取整方式，需要先使用asc_enable_hf32开启HF32取整模式。|`c_api/cube_compute/cube_compute.h`|
|[asc_enable_hif8](cube_compute/asc_enable_hif8.md)|用于设置Mmad计算开启HiF8模式，开启该模式后L0A Buffer/L0B Buffer中的8bit数据将在参与矩阵乘法运算前被转化为hifloat8_t类型数据。|`c_api/cube_compute/cube_compute.h`|
|[asc_disable_hif8](cube_compute/asc_disable_hif8.md)|用于设置Mmad计算关闭HiF8模式，设置该模式后L0A Buffer/L0B Buffer中的FP8数据在参与Mmad计算之前不会被转化为hifloat8_t类型数据。|`c_api/cube_compute/cube_compute.h`|
|[asc_disable_hf32](cube_compute/asc_disable_hf32.md)|用于设置Mmad计算关闭HF32模式，设置该模式后L0A Buffer/L0B Buffer中的FP32数据在参与Mmad计算之前不做舍入处理。|`c_api/cube_compute/cube_compute.h`|
|[asc_mmad](cube_compute/asc_mmad.md)|完成矩阵乘加操作。|`c_api/cube_compute/cube_compute.h`|
|[asc_mmad_mx](cube_compute/asc_mmad_mx.md)|完成包含放缩功能的矩阵乘加操作。|`c_api/cube_compute/cube_compute.h`|
|[asc_mmad_sparse](cube_compute/asc_mmad_sparse.md)|完成矩阵乘加操作，传入的左矩阵A为稀疏矩阵，右矩阵B为稠密矩阵。|`c_api/cube_compute/cube_compute.h`|
|[asc_set_fp32_mode](cube_compute/asc_set_fp32_mode.md)|用于设置Mmad计算开启FP32模式，开启该模式后L0A Buffer/L0B Buffer中的FP32数据在参与Mmad计算之前不做舍入处理。|`c_api/cube_compute/cube_compute.h`|
|[asc_set_hf32_round_mode](cube_compute/asc_set_hf32_round_mode.md)|设置HF32模式舍入方式，使用该接口前需要先调用asc_enable_hf32开启HF32模式。|`c_api/cube_compute/cube_compute.h`|
|[asc_set_mmad_direction_m](cube_compute/asc_set_mmad_direction_m.md)|设置mmad计算时优先通过M/N中的M方向生成结果，然后通过N方向产生结果，M为矩阵的行，N为矩阵的列。|`c_api/cube_compute/cube_compute.h`|
|[asc_set_mmad_direction_n](cube_compute/asc_set_mmad_direction_n.md)|设置mmad计算时优先通过M/N中的N方向生成结果，然后通过M方向产生结果，M为矩阵的行，N为矩阵的列。|`c_api/cube_compute/cube_compute.h`|

## 矩阵计算搬运

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_copy_gm2l0a](cube_datamove/asc_copy_gm2l0a.md)|将GM中的数据搬运到L0A中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_gm2l0b](cube_datamove/asc_copy_gm2l0b.md)|将GM中的数据搬运到L0B中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_gm2l1](cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1.md)|将GM中的数据搬运到L1中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_gm2l1_align](cube_datamove/asc_copy_gm2l1_align.md)|将GM中的数据padding后搬运到L1中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_gm2l1_dn2nz](cube_datamove/asc_copy_gm2l1_dn2nz.md)|将GM中的数据搬运到L1中，在此过程中执行DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0操作。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_gm2l1_nd2nz](cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz.md)|将GM中的数据搬运到L1中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l0c2gm](cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm.md)|将L0C中的数据搬运到GM中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l0c2l1](cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1.md)|矩阵计算完成后，对结果进行量化处理，之后将处理结果搬运到L1中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l0c2ub](cube_datamove/asc_copy_l0c2ub.md)|将L0C中的数据搬运到UB中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12bt](cube_datamove/asc_copy_l12bt/asc_copy_l12bt.md)|将MMAD指令的Bias数据从L1 Buffer搬运到BiasTable Buffer中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12fb](cube_datamove/asc_copy_l12fb/asc_copy_l12fb.md)|将数据从L1 Buffer搬运到Fixpipe Buffer中，Fixpipe Buffer用于存放量化参数。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12gm](cube_datamove/asc_copy_l12gm.md)|将数据从L1搬运到GM。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)|用于搬运存放在L1 Buffer里的512B大小的矩阵到L0A Buffer里。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0a_mx](cube_datamove/asc_copy_l12l0a_mx.md)|将Mx scale矩阵从L1 Buffer搬运到L0A Buffer。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0a_trans](cube_datamove/asc_copy_l12l0a_trans.md)|该接口实现带转置的2D格式数据从L1 Buffer到L0A Buffer的加载。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md)|用于搬运存放在L1 Buffer里的512B大小的矩阵到L0B Buffer里。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0b_mx](cube_datamove/asc_copy_l12l0b_mx.md)|将Mx scale矩阵从L1 Buffer搬运到L0B Buffer。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0b_sparse](cube_datamove/asc_copy_l12l0b_sparse.md)|用于搬运存放在L1 Buffer里的512B大小的稠密权重矩阵到L0B Buffer里，同时读取128B大小的索引矩阵用于稠密矩阵的稀疏化。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0b_trans](cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans.md)|该接口实现带转置的2D格式数据从L1 Buffer到L0B Buffer的加载。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12l0c](cube_datamove/asc_copy_l12l0c.md)|将矩阵由L1 Buffer搬运到L0C Buffer中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_copy_l12ub](cube_datamove/asc_copy_l12ub.md)|将数据从L1 Buffer搬运到UB中。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_fill_l0a](cube_datamove/asc_fill_l0a.md)|将L0A Buffer的Local Memory初始化为某一具体数值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_fill_l0b](cube_datamove/asc_fill_l0b.md)|将L0B Buffer的Local Memory初始化为某一具体数值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_fill_l1](cube_datamove/asc_fill_l1.md)|将L1 Buffer的Local Memory初始化为某一具体数值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_get_l0c2gm_prequant](cube_datamove/asc_get_l0c2gm_prequant.md)|数据搬运过程中进行随路量化时，通过调用该接口获取量化操作前矢量的起始地址。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_get_l0c2gm_relu](cube_datamove/asc_get_l0c2gm_relu.md)|数据搬运过程中进行随路量化时，通过调用该接口获取ReLU操作前矢量的起始地址。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_get_l0c2gm_unitflag](cube_datamove/asc_get_l0c2gm_unitflag.md)|数据搬运过程中进行随路量化时，通过调用该接口获取unit_flag设置。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_load_image_to_cbuf](cube_datamove/asc_load_image_to_cbuf.md)|将图像数据从Global Memory搬运到L1 Buffer。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_gm2l1_loop1_stride](cube_datamove/asc_set_gm2l1_loop1_stride.md)|将数据从GM搬运到L1 Buffer时，设置内层循环中相邻迭代数据块间的间隔。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_gm2l1_loop2_stride](cube_datamove/asc_set_gm2l1_loop2_stride.md)|将数据从GM搬运到L1 Buffer时，设置外层循环中相邻迭代数据块间的间隔。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_gm2l1_loop_size](cube_datamove/asc_set_gm2l1_loop_size.md)|将数据从GM搬运到L1 Buffer时，设置数据搬运流程中的循环次数。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_gm2l1_nz_para](cube_datamove/asc_set_gm2l1_nz_para.md)|设置MTE2_NZ_PARA寄存器的值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_gm2l1_pad](cube_datamove/asc_set_gm2l1_pad.md)|将数据从GM非对齐搬运到L1 Buffer时，设置连续搬运数据块左右两侧的填充值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c_copy_nz_para](cube_datamove/asc_set_l0c_copy_nz_para.md)|数据搬运过程中进行随路格式转换（NZ格式转换为ND格式）时，设置格式转换的相关配置。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c_copy_channel_para](cube_datamove/asc_set_l0c_copy_channel_para.md)|配置Nz2DN格式转换时源Nz矩阵中相邻行的地址偏移。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c2gm_channel_para](cube_datamove/asc_set_l0c2gm_channel_para.md)|对通道步长参数的专用寄存器的比特位进行设置。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c2gm_config](cube_datamove/asc_set_l0c2gm_config.md)|数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的矢量量化参数。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c2gm_lrelu_alpha](cube_datamove/asc_set_l0c2gm_lrelu_alpha.md)|用于设置asc_copy_l0c2l1或asc_copy_l0c2gm接口计算过程中使用的Leaky ReLU alpha值。该值只支持half和float两种数据类型。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c2gm_nz2nd](cube_datamove/asc_set_l0c2gm_nz2nd.md)|数据搬运过程中进行随路格式转换（NZ格式转换为ND格式）时，设置格式转换的相关配置。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c2gm_quant_post](cube_datamove/asc_set_l0c2gm_quant_post.md)|设置QUANT_POST寄存器的值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c2gm_relu_alpha](cube_datamove/asc_set_l0c2gm_relu_alpha.md)|设置RELU_ALPHA寄存器的值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_gm2l1_padding](cube_datamove/asc_set_gm2l1_padding.md)|设置[asc_copy_gm2l1_arch_3510（高维切分数据搬运）](cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md)的填充值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c_copy_params](cube_datamove/asc_set_l0c_copy_params.md)|DataCopy从L0C Buffer搬运到Global Memory或L1 Buffer过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l0c_copy_prequant](cube_datamove/asc_set_l0c_copy_prequant.md)|数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的标量量化参数。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l12l0_padding_val](cube_datamove/asc_set_l12l0_padding_val.md)|设置PADDING_B寄存器的值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l13d_fmatrix](cube_datamove/asc_set_l13d_fmatrix.md)|设置Feature map属性描述，用于在调用[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md)的3D格式搬运接口时配置填充数值。从左矩阵获取FeatureMap的属性时使用该接口。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l13d_fmatrix_b](cube_datamove/asc_set_l13d_fmatrix_b.md)|设置Feature map属性描述，用于在调用[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md)的3D格式搬运接口时配置填充数值。从右矩阵获取FeatureMap的属性时使用该接口。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l12l0a_3d_padding](cube_datamove/asc_set_l12l0a_3d_padding.md)|设置3D左矩阵搬运填充值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l12l0b_3d_padding](cube_datamove/asc_set_l12l0b_3d_padding.md)|设置3D右矩阵搬运填充值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l13d_padding](cube_datamove/asc_set_l13d_padding.md)|设置Pad属性描述，用于在调用asc_copy_l12l0a接口时配置填充数值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l13d_rpt](cube_datamove/asc_set_l13d_rpt.md)|用于设置Load3Dv2接口的repeat参数。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l13d_size](cube_datamove/asc_set_l13d_size.md)|设置[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)的3D格式搬运接口在L1 Buffer的边界值。|`c_api/cube_datamove/cube_datamove.h`|
|[asc_set_l3d_rpt_b](cube_datamove/asc_set_l3d_rpt_b.md)|用于设置接口asc_copy_l12l0a、asc_copy_l12l0b的2D格式搬运的repeat参数。|`c_api/cube_datamove/cube_datamove.h`|

## Memory矢量计算

### 基础算术

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_abs](vector_compute/vector_arith/asc_abs.md)|按元素取绝对值。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_add](vector_compute/vector_arith/asc_add.md)|按元素求和。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_add_scalar](vector_compute/vector_arith/asc_add_scalar.md)|矢量内每个元素与标量求和。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_div](vector_compute/vector_arith/asc_div.md)|按元素求商。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_exp](vector_compute/vector_arith/asc_exp.md)|按元素取自然指数。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_log](vector_compute/vector_arith/asc_log.md)|按元素取自然对数。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_max](vector_compute/vector_arith/asc_max.md)|按元素求最大值。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_max_scalar](vector_compute/vector_arith/asc_max_scalar.md)|矢量内每个元素与标量求最大值。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_min](vector_compute/vector_arith/asc_min.md)|按元素求最小值。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_min_scalar](vector_compute/vector_arith/asc_min_scalar.md)|源操作数矢量逐元素与标量相比，取较小值。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_mul](vector_compute/vector_arith/asc_mul.md)|按元素求积。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_mul_scalar](vector_compute/vector_arith/asc_mul_scalar.md)|矢量内每个元素与标量求积。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_rcp](vector_compute/vector_arith/asc_rcp.md)|执行矢量的取倒数运算。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_rsqrt](vector_compute/vector_arith/asc_rsqrt.md)|按元素进行开方后取倒数的计算。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_sqrt](vector_compute/vector_arith/asc_sqrt.md)|对元素进行开方。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_sub](vector_compute/vector_arith/asc_sub.md)|按元素求差。|`c_api/vector_compute/compute/vector_arith.h`|
|[asc_sub_scalar](vector_compute/vector_arith/asc_sub_scalar.md)|矢量内每个元素与标量求差。|`c_api/vector_compute/compute/vector_arith.h`|

### 复合计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_add_relu](vector_compute/vector_fused/asc_add_relu.md)|按元素求和，再进行Relu计算（结果和0对比取较大值），并提供转换最终结果的数据类型的功能(s162s8、f322f16、f162s8)。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_axpy](vector_compute/vector_fused/asc_axpy.md)|源操作数src中每个元素与标量value求积后和目的操作数dst中的对应元素相加。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_deq_int162b8](vector_compute/vector_fused/asc_deq_int162b8.md)|将int16_t类型转换为int8_t或uint8_t类型，并将数据存放在每个DataBlock的上半块或下半块。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_deq_int322half](vector_compute/vector_fused/asc_deq_int322half.md)|对输入的int32_t类型的数据按元素做量化并转换为half类型。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_fma](vector_compute/vector_fused/asc_fma.md)|按元素将src0和src1相乘并和dst相加，将最终结果存放进dst中。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_leakyrelu](vector_compute/vector_fused/asc_leakyrelu.md)|执行矢量Leaky Relu运算。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_mul_add](vector_compute/vector_fused/asc_mul_add.md)|执行矢量的乘加运算。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_mul_add_relu](vector_compute/vector_fused/asc_mul_add_relu.md)|按元素将src0和dst相乘并加上src1，再进行Relu计算（结果和0对比取较大值），最终结果存放进dst中。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_mul_cast_half2int8](vector_compute/vector_fused/asc_mul_cast_half2int8.md)|按元素求积，并将结果转换为int8_t类型。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_mul_cast_half2uint8](vector_compute/vector_fused/asc_mul_cast_half2uint8.md)|按元素求积，并将结果转换为uint8_t类型。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_relu](vector_compute/vector_fused/asc_relu.md)|按元素做线性整流Relu。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_set_deq_scale](vector_compute/vector_fused/asc_set_deq_scale.md)|设置DEQSCALAR寄存器的值。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_sub_relu](vector_compute/vector_fused/asc_sub_relu.md)|按元素求差，再进行Relu计算（结果和0对比取较大值），并提供转换最终结果的数据类型的功能(s162s8、f322f16、f162s8)。|`c_api/vector_compute/compute/vector_fused.h`|
|[asc_vdeq_int162b8](vector_compute/vector_fused/asc_vdeq_int162b8.md)|将int16_t类型转换为int8_t或uint8_t类型，并将数据存放在每个DataBlock的上半块或下半块。|`c_api/vector_compute/compute/vector_fused.h`|

### 广播操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_brcb](vector_compute/vector_broadcast/asc_brcb.md)|将源操作数中的每一个数填充到目的操作数的一个DataBlock中。|`c_api/vector_compute/compute/vector_broadcast.h`|
|[asc_duplicate](vector_compute/vector_broadcast/asc_duplicate.md)|将一个变量或立即数填充到一个矢量中。|`c_api/vector_compute/compute/vector_broadcast.h`|

### 归约计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_datablock_reduce_max](vector_compute/vector_reduce/asc_datablock_reduce_max.md)|对每个DataBlock内所有元素求最大值。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_datablock_reduce_min](vector_compute/vector_reduce/asc_datablock_reduce_min.md)|对每个DataBlock内所有元素求最小值。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_datablock_reduce_sum](vector_compute/vector_reduce/asc_datablock_reduce_sum.md)|对每个DataBlock内所有元素求和。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_get_reduce_max_cnt](vector_compute/vector_reduce/asc_get_reduce_max_cnt.md)|获取执行asc_repeat_reduce_max操作后的最大值，以及第一个最大值时的索引。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_get_reduce_min_cnt](vector_compute/vector_reduce/asc_get_reduce_min_cnt.md)|获取执行asc_repeat_reduce_min操作后的最小值，以及第一个最小值时的索引。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_get_rsvd_count](vector_compute/vector_reduce/asc_get_rsvd_count.md)|获取GatherMask操作后剩余的元素数量。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_pair_reduce_sum](vector_compute/vector_reduce/asc_pair_reduce_sum.md)|对输入数据做归约操作，得到数据总和。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_reduce](vector_compute/vector_reduce/asc_reduce.md)|以内置固定模式对应的二进制或者用户自定义输入的数值对应的gather mask（数据收集的掩码），从源操作数中选取元素写入目的操作数中。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_repeat_reduce_max](vector_compute/vector_reduce/asc_repeat_reduce_max.md)|对每个Repeat内所有元素求最大值。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_repeat_reduce_min](vector_compute/vector_reduce/asc_repeat_reduce_min.md)|对每个Repeat内所有元素求最小值。|`c_api/vector_compute/compute/vector_reduce.h`|
|[asc_repeat_reduce_sum](vector_compute/vector_reduce/asc_repeat_reduce_sum.md)|对每个Repeat内所有元素求和。|`c_api/vector_compute/compute/vector_reduce.h`|

### 排布变换

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_select](vector_compute/vector_permute_sel/asc_select.md)|根据掩码，从两个源操作数中选取元素，输出到目的操作数。|`c_api/vector_compute/compute/vector_permute_sel.h`|
|[asc_set_va_reg](vector_compute/vector_permute_sel/asc_set_va_reg.md)|用于设置transpose的地址，将操作数地址序列与地址寄存器关联。|`c_api/vector_compute/compute/vector_permute_sel.h`|
|[asc_squeeze](vector_compute/vector_permute_sel/asc_squeeze.md)|通过比较掩码src1，将src0中的向量压缩成更短的向量，存储到dst中。|`c_api/vector_compute/compute/vector_permute_sel.h`|
|[asc_transpose](vector_compute/vector_permute_sel/asc_transpose.md)|用于实现16*16的二维矩阵数据块转置。|`c_api/vector_compute/compute/vector_permute_sel.h`|
|[asc_transto5hd](vector_compute/vector_permute_sel/asc_transto5hd.md)|数据格式转换，一般用于将NCHW格式转换成NC1HWC0格式。|`c_api/vector_compute/compute/vector_permute_sel.h`|

### 排序计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_bitsort](vector_compute/vector_sort/asc_bitsort.md)|Score和Index分别存储在src0和src1中，按Score进行排序（Score大的元素排前面），排序后的Score与其对应的Index一起以（Score，Index）的结构存储在dst中。|`c_api/vector_compute/compute/vector_sort.h`|
|[asc_get_vms4_sr](vector_compute/vector_sort/asc_get_vms4_sr.md)|获取执行asc_mrgsort4操作后的队列中，每个队列已经理过的Region Proposal个数。|`c_api/vector_compute/compute/vector_sort.h`|
|[asc_mrgsort4](vector_compute/vector_sort/asc_mrgsort4.md)|将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。|`c_api/vector_compute/compute/vector_sort.h`|

### 掩码操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_set_mask_count](vector_compute/vector_mask_config/asc_set_mask_count.md)|设置Mask模式为Counter模式。|`c_api/vector_compute/compute/vector_mask_config.h`|
|[asc_set_mask_norm](vector_compute/vector_mask_config/asc_set_mask_norm.md)|设置Mask模式为Normal模式。|`c_api/vector_compute/compute/vector_mask_config.h`|
|[asc_set_vector_mask](vector_compute/vector_mask_config/asc_set_vector_mask.md)|设置Mask值。|`c_api/vector_compute/compute/vector_mask_config.h`|

### 比较计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_eq](vector_compute/vector_compare/asc_eq.md)|比较src0与src1在对应索引位置的元素大小。若比较结果为真，则输出结果的对应比特位设为1，否则设为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_eq_scalar](vector_compute/vector_compare/asc_eq_scalar.md)|执行矢量与标量的比较运算，如果值相等则输出1，否则输出0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_ge](vector_compute/vector_compare/asc_ge.md)|Ge（greater than or equal to），逐元素比较src0 >= src1是否成立，成立则输出结果为1，否则输出结果为0，每个元素的比较结果占一个bit。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_ge_scalar](vector_compute/vector_compare/asc_ge_scalar.md)|按元素判断src >= value是否成立，若成立则输出结果为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_get_cmp_mask](vector_compute/vector_compare/asc_get_cmp_mask.md)|获取Compare操作的比较结果。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_gt](vector_compute/vector_compare/asc_gt.md)|按元素比较两个矢量的大小关系，若比较后的结果为真，则输出结果的对应比特位为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_gt_scalar](vector_compute/vector_compare/asc_gt_scalar.md)|src中的每个元素逐个与标量value比较大小，如果某个位置上的元素大于value，则输出结果dst上的对应比特位为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_le](vector_compute/vector_compare/asc_le.md)|按元素判断src0 <= src1是否成立，若成立则输出结果为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_le_scalar](vector_compute/vector_compare/asc_le_scalar.md)|按元素判断src <= value是否成立，若成立则输出结果为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_lt](vector_compute/vector_compare/asc_lt.md)|按元素判断src0 < src1是否成立，若成立则输出结果上的对应比特位为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_lt_scalar](vector_compute/vector_compare/asc_lt_scalar.md)|执行矢量中每个位置和标量比较，如果值小于标量值则为1，否则为0，结果为每个bit位按小端序排布。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_ne](vector_compute/vector_compare/asc_ne.md)|按元素判断src0 != src1是否成立，若成立则输出结果为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_ne_scalar](vector_compute/vector_compare/asc_ne_scalar.md)|按元素判断是否不等于输入标量，若成立则输出结果上的对应比特位为1，否则为0。|`c_api/vector_compute/compute/vector_compare.h`|
|[asc_set_cmp_mask](vector_compute/vector_compare/asc_set_cmp_mask.md)|为Select操作设置用于选择的掩码。|`c_api/vector_compute/compute/vector_compare.h`|

### 类型转换

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_bfloat162float](vector_compute/vector_convert/asc_bfloat162float.md)|数据类型转换。将bfloat16_t类型的数据转换为float类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_bfloat162int32](vector_compute/vector_convert/asc_bfloat162int32.md)|数据类型转换。将bfloat16_t类型的数据转换为int32_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_float2bfloat16](vector_compute/vector_convert/asc_float2bfloat16.md)|数据类型转换。将float类型的数据转换为bfloat16_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_float2float](vector_compute/vector_convert/asc_float2float.md)|数据类型转换。将float类型的数据转换为float类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_float2half](vector_compute/vector_convert/asc_float2half.md)|数据类型转换。将float类型的数据转换为half类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_float2int16](vector_compute/vector_convert/asc_float2int16.md)|将float类型数据转换为int16_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_float2int32](vector_compute/vector_convert/asc_float2int32.md)|将float类型数据转换为int32_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_float2int64](vector_compute/vector_convert/asc_float2int64.md)|将float类型数据转换为int64_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_half2float](vector_compute/vector_convert/asc_half2float.md)|数据类型转换。将half类型的数据转换为float类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_half2int16](vector_compute/vector_convert/asc_half2int16.md)|数据类型转换。将half类型的数据转换为int16_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_half2int32](vector_compute/vector_convert/asc_half2int32.md)|数据类型转换。将half类型的数据转换为int32_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_half2int4](vector_compute/vector_convert/asc_half2int4.md)|数据类型转换。将half类型的数据转换为int4b_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_half2int8](vector_compute/vector_convert/asc_half2int8.md)|数据类型转换。将half类型的数据转换为int8_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_half2uint8](vector_compute/vector_convert/asc_half2uint8.md)|将half类型数据转换为uint8_t类型，支持多种舍入模式。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int162float](vector_compute/vector_convert/asc_int162float.md)|数据类型转换。将int16_t类型的数据转换为float类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int162half](vector_compute/vector_convert/asc_int162half.md)|数据类型转换。将int16_t类型的数据转换为half类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int322float](vector_compute/vector_convert/asc_int322float.md)|将int32_t类型数据转换为float类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int322int16](vector_compute/vector_convert/asc_int322int16.md)|数据类型转换。将int32_t类型的数据转换为int16_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int322int64](vector_compute/vector_convert/asc_int322int64.md)|数据类型转换。将int32_t类型的数据转换为int64_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int42half](vector_compute/vector_convert/asc_int42half.md)|数据类型转换。将int4b_t类型的数据转换为half类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int642float](vector_compute/vector_convert/asc_int642float.md)|将int64_t类型数据转换为float类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int642int32](vector_compute/vector_convert/asc_int642int32.md)|数据类型转换。将int64_t类型的数据转换为int32_t类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_int82half](vector_compute/vector_convert/asc_int82half.md)|数据类型转换。将int8_t类型的数据转换为half类型。|`c_api/vector_compute/compute/vector_convert.h`|
|[asc_uint82half](vector_compute/vector_convert/asc_uint82half.md)|数据类型转换。将uint8_t类型的数据转换为half类型。|`c_api/vector_compute/compute/vector_convert.h`|

### 聚合操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_gather](vector_compute/vector_gather/asc_gather.md)|将源操作数按照给定的偏移按元素收集到目的操作数中。|`c_api/vector_compute/compute/vector_gather.h`|
|[asc_gather_datablock](vector_compute/vector_gather/asc_gather_datablock.md)|根据偏移地址按照DataBlock的粒度将源操作数收集到目的操作数中。|`c_api/vector_compute/compute/vector_gather.h`|

### 逻辑计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_and](vector_compute/vector_logic/asc_and.md)|执行矢量与运算。|`c_api/vector_compute/compute/vector_logic.h`|
|[asc_not](vector_compute/vector_logic/asc_not.md)|按元素做按位取反，计算公式如下。|`c_api/vector_compute/compute/vector_logic.h`|
|[asc_or](vector_compute/vector_logic/asc_or.md)|每对元素按位或运算。|`c_api/vector_compute/compute/vector_logic.h`|
|[asc_shiftleft](vector_compute/vector_logic/asc_shiftleft.md)|将所有元素左移distance位。|`c_api/vector_compute/compute/vector_logic.h`|
|[asc_shiftright](vector_compute/vector_logic/asc_shiftright.md)|对源操作数中的每个元素执行右移。|`c_api/vector_compute/compute/vector_logic.h`|

## 矢量数据搬运

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_copy](vector_datamove/asc_copy.md)|将数据从UB搬运到UB。支持高维切分。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_copy_gm2ub](vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md)|将数据从Global Memory搬运到UB。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)|提供数据非对齐搬运的功能，将数据从Global Memory搬运到UB，并支持8位/16位/32位数据类型搬运。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_copy_ub2gm](vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md)|将数据从UB搬运到Global Memory。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)|将数据从UB搬运到Global Memory，支持8位/16位/32位分块拷贝操作。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_copy_ub2l1](vector_datamove/asc_copy_ub2l1.md)|将数据从UB搬运到L1 Buffer。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_copy_ub2ub](vector_datamove/asc_copy_ub2ub.md)|将数据从UB搬运到UB。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md)|多维数据搬运接口，将数据从Global Memory (GM)搬运到UB。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_copy_pad_val](vector_datamove/asc_set_copy_pad_val.md)|和asc_copy_gm2ub_align或asc_copy_ub2gm_align接口配合使用，设置连续搬运数据块左右两侧需要填补的数据值。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_gm2ub_loop1_stride](vector_datamove/asc_set_gm2ub_loop1_stride.md)|使用[asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory (GM)搬运到UB时，设置内层循环中相邻迭代数据块间的间隔。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_gm2ub_loop2_stride](vector_datamove/asc_set_gm2ub_loop2_stride.md)|使用[asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory (GM)搬运到UB时，设置外层循环中相邻迭代数据块间的间隔。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_gm2ub_loop_size](vector_datamove/asc_set_gm2ub_loop_size.md)|使用[asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory (GM)搬运到UB时，设置数据搬运流程中的循环次数。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_ndim_loop_stride](vector_datamove/asc_set_ndim_loop_stride.md)|设置[asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md)在进行多维搬运时每个维度内的源操作数与目的操作数的元素之间的间隔，最多设置5个维度。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_ndim_pad_count](vector_datamove/asc_set_ndim_pad_count.md)|设置[asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md)接口各个维度左右侧的Padding元素个数。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_ndim_pad_value](vector_datamove/asc_set_ndim_pad_value.md)|设置[asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md)接口Padding的填充固定值。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_ub2gm_loop1_stride](vector_datamove/asc_set_ub2gm_loop1_stride.md)|使用[asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)将数据从UB搬运到Global Memory (GM)时，设置内层循环中源操作数在相邻迭代间的数据块间隔，以及目的操作数在相邻迭代间的数据块间隔。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_ub2gm_loop2_stride](vector_datamove/asc_set_ub2gm_loop2_stride.md)|使用[asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)将数据从UB搬运到Global Memory (GM)时，设置外层循环中源操作数在相邻迭代间的数据块间隔，以及目的操作数在相邻迭代间的数据块间隔。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_ub2gm_loop_size](vector_datamove/asc_set_ub2gm_loop_size.md)|使用[asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)将数据从UB搬运到Global Memory (GM)时，设置内层循环和外层循环的次数。|`c_api/vector_datamove/vector_datamove.h`|

## Reg矢量计算

### Reg数据搬入

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_loadalign](reg_compute/load/asc_loadalign.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，NORM搬入模式。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_datablock_strided](reg_compute/load/asc_loadalign_datablock_strided.md)|对齐数据搬运接口，从UB非连续对齐搬入8个DataBlock，通过函数返回值返回矢量数据寄存器。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_mask](reg_compute/load/asc_loadalign_mask.md)|对齐数据搬运接口，从UB连续对齐搬入掩码数据，通过函数返回值返回掩码寄存器。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_brc_datablock](reg_compute/load/asc_loadalign_brc_datablock.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，读取一个DataBlock并广播到VL。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_brc_datablock_postupdate](reg_compute/load/asc_loadalign_brc_datablock_postupdate.md)|对齐数据搬运接口，读取一个DataBlock并广播到VL，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_brc_elem](reg_compute/load/asc_loadalign_brc_elem.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，搬运一个元素并广播到VL。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_brc_elem2datablock](reg_compute/load/asc_loadalign_brc_elem2datablock.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，将每个元素广播到一个DataBlock中。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_brc_elem2datablock_postupdate](reg_compute/load/asc_loadalign_brc_elem2datablock_postupdate.md)|对齐数据搬运接口，将每个元素广播到一个DataBlock中，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_brc_elem_postupdate](reg_compute/load/asc_loadalign_brc_elem_postupdate.md)|对齐数据搬运接口，搬运一个元素并广播到VL，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_deintlv](reg_compute/load/asc_loadalign_deintlv.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，随路完成deinterleave操作。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_deintlv_postupdate](reg_compute/load/asc_loadalign_deintlv_postupdate.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，随路完成deinterleave操作，启用postupdate模式，自动更新UB地址参数。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_downsample](reg_compute/load/asc_loadalign_downsample.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，2倍下采样模式。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_mask_downsample](reg_compute/load/asc_loadalign_mask_downsample.md)|对齐数据搬运接口，从UB连续对齐搬入掩码数据，2倍下采样模式，通过函数返回值返回掩码寄存器。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_downsample_postupdate](reg_compute/load/asc_loadalign_downsample_postupdate.md)|对齐数据搬运接口，2倍下采样模式，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_postupdate](reg_compute/load/asc_loadalign_postupdate.md)|将数据从UB搬入MaskReg，使能post mod。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_unpack](reg_compute/load/asc_loadalign_unpack.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，解压缩模式。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_unpack4](reg_compute/load/asc_loadalign_unpack4.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，解压缩模式。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_unpack4_postupdate](reg_compute/load/asc_loadalign_unpack4_postupdate.md)|对齐数据搬运接口，解压缩模式，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_unpack_postupdate](reg_compute/load/asc_loadalign_unpack_postupdate.md)|对齐数据搬运接口，解压缩模式，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_upsample](reg_compute/load/asc_loadalign_upsample.md)|对齐数据搬运接口，从UB连续对齐搬入目的操作数，2倍上采样模式。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_mask_upsample](reg_compute/load/asc_loadalign_mask_upsample.md)|对齐数据搬运接口，从UB连续对齐搬入掩码数据，2倍上采样模式，通过函数返回值返回掩码寄存器。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_upsample_postupdate](reg_compute/load/asc_loadalign_upsample_postupdate.md)|对齐数据搬运接口，2倍上采样模式，启用Post Update。|`c_api/reg_compute/load/loadalign.h`|
|[asc_load](reg_compute/load/asc_load.md)|reg计算数据搬运接口，支持从UB非32字节对齐的源地址src搬运至矢量数据寄存器，搬运量为VL。|`c_api/composite/loadunalign_composite.h`|
|[asc_loadunalign](reg_compute/load/asc_loadunalign.md)|reg计算数据搬运接口，适用于从UB非32B对齐的起始地址连续搬入矢量数据寄存器的场景。|`c_api/reg_compute/load/loadunalign.h`|
|[asc_loadunalign_postupdate](reg_compute/load/asc_loadunalign_postupdate.md)|数据搬运接口，适用于从UB非32B对齐地址起始连续搬入，并自动更新源操作数地址。|`c_api/reg_compute/load/loadunalign.h`|
|[asc_loadunalign_pre](reg_compute/load/asc_loadunalign_pre.md)|用于在进行非对齐数据搬入前的初始化，需配合[asc_loadunalign](reg_compute/load/asc_loadunalign.md)接口使用。|`c_api/reg_compute/load/loadunalign.h`|

### Reg数据搬出

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_storealign](reg_compute/store/asc_storealign.md)|reg计算数据搬运接口，从矢量数据寄存器连续对齐搬出到UB，NORM搬出模式。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_1st](reg_compute/store/asc_storealign_1st.md)|reg计算数据搬运接口，忽略mask向dst搬出src第一个元素。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_1st_postupdate](reg_compute/store/asc_storealign_1st_postupdate.md)|reg计算数据搬运接口，忽略mask向dst搬出src第一个元素，启用Post Update。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_intlv](reg_compute/store/asc_storealign_intlv.md)|reg计算数据搬运接口，将src0和src1中的元素交错搬出到UB。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack](reg_compute/store/asc_storealign_pack.md)|reg计算数据搬运接口，根据mask将src中有效元素的低半部分数据连续存储于dst中。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack_postupdate](reg_compute/store/asc_storealign_pack_postupdate.md)|reg计算数据搬运接口，根据mask压缩搬出，启用Post Update。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack_quarter](reg_compute/store/asc_storealign_pack_quarter.md)|reg计算数据搬运接口，根据mask将src中有效元素的低8bits数据连续存储于dst中。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack_quarter_postupdate](reg_compute/store/asc_storealign_pack_quarter_postupdate.md)|reg计算数据搬运接口，根据mask将src中有效元素的低8bits数据连续存储于dst中，启用Post Update。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_postupdate](reg_compute/store/asc_storealign_postupdate.md)|reg计算数据搬运接口，从矢量数据寄存器连续对齐搬出到UB，NORM搬出模式，启用Post Update。|`c_api/reg_compute/store/storealign.h`|
|[asc_store](reg_compute/store/asc_store.md)|reg计算数据搬运接口，适用于从矢量数据寄存器搬出到UB的场景，不区分是否对齐，在追求极致性能时，应尽量避免使用该接口。|`c_api/composite/storeunalign_composite.h`|
|[asc_storeunalign](reg_compute/store/asc_storeunalign.md)|reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的场景。|`c_api/reg_compute/store/storeunalign.h`|
|[asc_storeunalign_post](reg_compute/store/asc_storeunalign_post.md)|reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址搬出到UB的尾块场景。|`c_api/reg_compute/store/storeunalign.h`|
|[asc_storeunalign_post_postupdate](reg_compute/store/asc_storeunalign_post_postupdate.md)|reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址搬出到UB的尾块场景，并自动更新目的操作数地址。|`c_api/reg_compute/store/storeunalign.h`|
|[asc_storeunalign_postupdate](reg_compute/store/asc_storeunalign_postupdate.md)|reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的场景。|`c_api/reg_compute/store/storeunalign.h`|

### Reg离散搬出

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_scatter](reg_compute/scatter/asc_scatter.md)|根据索引值index将源操作数src中的元素分散到目的操作数dst中。|`c_api/reg_compute/scatter/ub_scatter.h`|

### Reg聚合搬入

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_gather](reg_compute/ub_gather/asc_gather.md)|根据索引位置index将源操作数src按元素收集到目的操作数dst中（UB源收集模式）。|`c_api/reg_compute/gather/ub_gather.h`|
|[asc_gather_datablock](reg_compute/ub_gather/asc_gather_datablock.md)|给定源操作数在UB中的基地址和索引，根据索引位置将源操作数按DataBlock收集到目的操作数中。|`c_api/reg_compute/gather/ub_gather.h`|

### Reg聚合

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_gather](reg_compute/reg_gather/asc_gather.md)|根据索引位置index将源操作数src按元素收集到目的操作数dst中（寄存器源收集模式）。|`c_api/reg_compute/compute/reg_gather.h`|

### Reg间搬运

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_copy](reg_compute/reg_copy/asc_copy.md)|完成数据在寄存器内的搬运。|`c_api/reg_compute/reg_copy.h`|
|[asc_extract_mask](reg_compute/reg_copy/asc_extract_mask.md)|从矢量数据寄存器中提取掩码寄存器，通过函数返回值返回结果。|`c_api/reg_compute/reg_copy.h`|
|[asc_mask](reg_compute/reg_copy/asc_mask.md)|通过函数返回值返回矢量数据寄存器或掩码寄存器的掩码复制结果。|`c_api/reg_compute/reg_copy.h`|

### 同步控制

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_mem_bar](reg_compute/reg_sync/asc_mem_bar.md)|Reg计算宏函数内不同流水线之间的同步指令。|`c_api/reg_compute/reg_sync.h`|

### 地址寄存器操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_update_addr_reg](reg_compute/reg_addr_reg/asc_update_addr_reg.md)|地址寄存器通过该接口初始化，然后在循环之中使用地址寄存器存储地址偏移量。|`c_api/reg_compute/compute/reg_addr_reg.h`|

### 基础算术

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_abs](reg_compute/reg_arith/asc_abs.md)|逐元素计算绝对值。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_add](reg_compute/reg_arith/asc_add.md)|按元素执行加法运算。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_add_scalar](reg_compute/reg_arith/asc_add_scalar.md)|按元素执行矢量和标量的加法运算。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_addc](reg_compute/reg_arith/asc_addc.md)|按元素执行带进位的加法运算。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_div](reg_compute/reg_arith/asc_div.md)|按元素执行除法运算。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_exp](reg_compute/reg_arith/asc_exp.md)|对源操作数逐元素计算指数。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_ln](reg_compute/reg_arith/asc_ln.md)|对源操作数逐元素计算自然对数。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_max](reg_compute/reg_arith/asc_max.md)|按元素求最大值。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_max_scalar](reg_compute/reg_arith/asc_max_scalar.md)|按元素求矢量和标量的最大值。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_min](reg_compute/reg_arith/asc_min.md)|根据mask对源操作数src0、src1进行按元素求最小值操作，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_min_scalar](reg_compute/reg_arith/asc_min_scalar.md)|源操作数矢量内每个元素与标量比较，取较小值。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_mul](reg_compute/reg_arith/asc_mul.md)|对源操作数src0和src1进行乘法运算，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_mul_scalar](reg_compute/reg_arith/asc_mul_scalar.md)|对源操作数src、value进行按元素乘法操作，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_mull](reg_compute/reg_arith/asc_mull.md)|根据mask对输入数据src0、src1按元素相乘，将结果写入dst0，溢出部分写入dst1。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_neg](reg_compute/reg_arith/asc_neg.md)|根据mask对源操作数src进行取相反数操作，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_sqrt](reg_compute/reg_arith/asc_sqrt.md)|逐元素计算平方根。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_sub](reg_compute/reg_arith/asc_sub.md)|按元素执行减法运算。|`c_api/reg_compute/compute/reg_arith.h`|
|[asc_subc](reg_compute/reg_arith/asc_subc.md)|按元素执行带借位的减法运算。|`c_api/reg_compute/compute/reg_arith.h`|

### 复合计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_abs_sub](reg_compute/reg_fused/asc_abs_sub.md)|逐元素计算差的绝对值。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_axpy](reg_compute/reg_fused/asc_axpy.md)|根据mask对源操作数src、value进行按元素做乘加操作，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_exp_sub](reg_compute/reg_fused/asc_exp_sub.md)|根据mask将src0与src1按元素相减，并计算以差值为指数的自然指数函数。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_fma](reg_compute/reg_fused/asc_fma.md)|逐元素乘加运算。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_leakyrelu](reg_compute/reg_fused/asc_leakyrelu.md)|按元素执行Leaky ReLU（Leaky Rectified Linear Unit）操作。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_madd](reg_compute/reg_fused/asc_madd.md)|madd（multiply-add），对源操作数执行逐元素乘法和加法。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_mula](reg_compute/reg_fused/asc_mula.md)|根据掩码将两个源寄存器按元素相乘，并与目的寄存器累加。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_prelu](reg_compute/reg_fused/asc_prelu.md)|源操作数src0大于0的情况下直接将src0写入目的操作数dst，否则将src0 * src1的结果写入dst。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_relu](reg_compute/reg_fused/asc_relu.md)|逐元素执行ReLU运算。|`c_api/reg_compute/compute/reg_fused.h`|

### 广播操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_duplicate](reg_compute/reg_broadcast/asc_duplicate.md)|根据mask将源操作数src的最低位元素填充到目的操作数dst。|`c_api/reg_compute/compute/reg_broadcast.h`|
|[asc_duplicate_highest](reg_compute/reg_broadcast/asc_duplicate_highest.md)|根据mask将源操作数src的最高位元素填充到目的操作数dst，dst中未被mask筛选的元素置为0。|`c_api/reg_compute/compute/reg_broadcast.h`|
|[asc_duplicate_highest_merge](reg_compute/reg_broadcast/asc_duplicate_highest_merge.md)|根据mask将源操作数src的最高位元素填充到目的操作数dst，dst中未被mask筛选的元素保留原值。|`c_api/reg_compute/compute/reg_broadcast.h`|
|[asc_duplicate_merge](reg_compute/reg_broadcast/asc_duplicate_merge.md)|根据mask将源操作数src的最低位元素填充到目的操作数dst，dst中未被mask筛选的元素保留原值。|`c_api/reg_compute/compute/reg_broadcast.h`|
|[asc_duplicate_scalar](reg_compute/reg_broadcast/asc_duplicate_scalar.md)|根据mask将value填充到目的操作数dst。|`c_api/reg_compute/compute/reg_broadcast.h`|
|[asc_duplicate_scalar_merge](reg_compute/reg_broadcast/asc_duplicate_scalar_merge.md)|根据mask将value填充到目的操作数dst，dst中未被mask筛选的元素保留原值。|`c_api/reg_compute/compute/reg_broadcast.h`|

### 归约计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_pair_reduce_sum](reg_compute/reg_reduce/asc_pair_reduce_sum.md)|PairReduceSum：相邻两个（奇偶）元素求和，结果写入dst。|`c_api/reg_compute/compute/reg_reduce.h`|
|[asc_reduce_max](reg_compute/reg_reduce/asc_reduce_max.md)|根据mask对源操作数src进行归约最大值操作，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_reduce.h`|
|[asc_reduce_max_datablock](reg_compute/reg_reduce/asc_reduce_max_datablock.md)|找出DataBlock中的最大值，并将最终的计算结果依次保存在dst中的最低位。|`c_api/reg_compute/compute/reg_reduce.h`|
|[asc_reduce_min](reg_compute/reg_reduce/asc_reduce_min.md)|根据mask对源操作数src进行归约最小值操作，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_reduce.h`|
|[asc_reduce_min_datablock](reg_compute/reg_reduce/asc_reduce_min_datablock.md)|根据mask将每个DataBlock(32B)中的最小值，依次保存在dst中的最低位。|`c_api/reg_compute/compute/reg_reduce.h`|
|[asc_reduce_sum](reg_compute/reg_reduce/asc_reduce_sum.md)|归约求和功能，用于将src中的所有参与计算的元素求和，得到的结果保存在dst中。|`c_api/reg_compute/compute/reg_reduce.h`|
|[asc_reduce_sum_datablock](reg_compute/reg_reduce/asc_reduce_sum_datablock.md)|归约求和功能，用于将src每个DataBlock(32B)中参与计算的元素求和，得到的结果依次保存在dst中。|`c_api/reg_compute/compute/reg_reduce.h`|

### 排布变换

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_deintlv](reg_compute/reg_permute_sel/asc_deintlv.md)|给定源操作数src0和src1，将src0和src1中的元素解交织存入结果操作数dst0和dst1中。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_intlv](reg_compute/reg_permute_sel/asc_intlv.md)|将源操作数src0和src1中的元素交织存入目的操作数dst0和dst1中。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_pack_to_high](reg_compute/reg_permute_sel/asc_pack_to_high.md)|将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的高半部分（当数据类型为vector_bool时，每2bit选取其中的低位1bit）。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_pack_to_low](reg_compute/reg_permute_sel/asc_pack_to_low.md)|将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的低半部分（当数据类型为vector_bool时，每2bit选取其中的低位1bit）。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_select](reg_compute/reg_permute_sel/asc_select.md)|根据mask的比特位值，从源操作数src0、src1中选择元素，得到目的操作数dst。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_squeeze](reg_compute/reg_permute_sel/asc_squeeze.md)|将src中被mask选择的有效元素依次复制到dst中，有效元素从低到高连续排列，剩余位置元素置为0。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_squeeze_and_storeunalign](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)|reg计算数据搬运接口，适用于从矢量数据寄存器中筛选出有效元素后按照连续非32B对齐的起始地址连续搬出到UB的场景。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_squeeze_and_storeunalign_finalize](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md)|reg计算数据搬运接口，适用于从矢量数据寄存器中筛选出有效元素后按照连续非32B对齐的起始地址连续搬出到UB的尾块场景。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_squeeze_and_storeunalign_init](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md)|对AR特殊寄存器进行清零操作。AR寄存器用于配合[asc_squeeze_and_storeunalign](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)及[asc_squeeze_and_storeunalign_finalize](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md)使用。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_unpack](reg_compute/reg_permute_sel/asc_unpack.md)|矢量解包操作。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_unsqueeze](reg_compute/reg_permute_sel/asc_unsqueeze.md)|根据mask进行解压缩，将生成的数据输出到dst。|`c_api/reg_compute/compute/reg_permute_sel.h`|

### 掩码寄存器操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_create_mask](reg_compute/reg_mask/asc_create_mask.md)|根据入参生成相应的掩码寄存器。|`c_api/reg_compute/compute/reg_mask.h`|
|[asc_get_mask_spr](reg_compute/reg_mask/asc_get_mask_spr.md)|从特殊寄存器SPR{MASK1, MASK0}读取mask值并根据数据类型格式返回对应的mask数据，MASK0、MASK1均为64bit的寄存器。|`c_api/reg_compute/compute/reg_mask.h`|
|[asc_update_mask](reg_compute/reg_mask/asc_update_mask.md)|根据value大小生成对应的掩码寄存器中的值。|`c_api/reg_compute/compute/reg_mask.h`|

### 比较计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_eq](reg_compute/reg_compare/asc_eq.md)|对源操作数逐元素比较是否相等。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_eq_scalar](reg_compute/reg_compare/asc_eq_scalar.md)|对源操作数逐元素比较是否与标量相等。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_ge](reg_compute/reg_compare/asc_ge.md)|ge（greater than or equal to），对源操作数执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_ge_scalar](reg_compute/reg_compare/asc_ge_scalar.md)|ge（greater than or equal to），对源操作数与标量执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_gt](reg_compute/reg_compare/asc_gt.md)|gt（greater than），对源操作数执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_gt_scalar](reg_compute/reg_compare/asc_gt_scalar.md)|gt（greater than），对源操作数与标量执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_le](reg_compute/reg_compare/asc_le.md)|le（less than or equal to），对源操作数执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_le_scalar](reg_compute/reg_compare/asc_le_scalar.md)|le（less than or equal to），对源操作数与标量执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_lt](reg_compute/reg_compare/asc_lt.md)|lt（less than），对源操作数执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_lt_scalar](reg_compute/reg_compare/asc_lt_scalar.md)|lt（less than），对源操作数与标量执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_ne](reg_compute/reg_compare/asc_ne.md)|ne（not equal），对源操作数执行逐元素比较。|`c_api/reg_compute/compute/reg_compare.h`|
|[asc_ne_scalar](reg_compute/reg_compare/asc_ne_scalar.md)|src中的每个元素逐个与标量value比较大小，如果src_i != value，则输出结果dst对应比特位为1，否则为0。|`c_api/reg_compute/compute/reg_compare.h`|

### 直方图

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_cumulative_histogram](reg_compute/reg_histogram/asc_cumulative_histogram.md)|对直方图数据进行累计统计。|`c_api/reg_compute/compute/reg_histogram.h`|
|[asc_frequency_histogram](reg_compute/reg_histogram/asc_frequency_histogram.md)|对直方图数据进行频率统计。|`c_api/reg_compute/compute/reg_histogram.h`|

### 类型转换

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_bfloat162float](reg_compute/reg_convert/asc_bfloat162float.md)|将bfloat16_t类型转换为float类型，无舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_bfloat162half](reg_compute/reg_convert/asc_bfloat162half.md)|将bfloat16_t类型转换为half类型，并支持多种舍入模式和饱和/非饱和模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_bfloat162int32](reg_compute/reg_convert/asc_bfloat162int32.md)|将bfloat16_t类型数据转换为int32_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_ceil](reg_compute/reg_convert/asc_ceil.md)|将源操作数中的浮点数元素按照CEIL（向正无穷方向舍入）模式舍入到整数值，结果保持原浮点数据类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2bfloat16](reg_compute/reg_convert/asc_float2bfloat16.md)|将float类型转换为bfloat16_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2half](reg_compute/reg_convert/asc_float2half.md)|将float类型转换为half类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2int16](reg_compute/reg_convert/asc_float2int16.md)|将float类型转换为int16_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2int32](reg_compute/reg_convert/asc_float2int32.md)|将float类型转换为int32_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2int64](reg_compute/reg_convert/asc_float2int64.md)|将float类型转换为int64_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_floor](reg_compute/reg_convert/asc_floor.md)|将源操作数中的浮点数元素按照FLOOR（向负无穷方向舍入）模式舍入到整数值，结果保持原浮点数据类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2bfloat16](reg_compute/reg_convert/asc_half2bfloat16.md)|将half类型数据转换为bfloat16_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2float](reg_compute/reg_convert/asc_half2float.md)|将half类型数据转换为float类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2hif8](reg_compute/reg_convert/asc_half2hif8.md)|将half类型转换为hifloat8_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2int16](reg_compute/reg_convert/asc_half2int16.md)|将half类型数据转换为int16_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2int32](reg_compute/reg_convert/asc_half2int32.md)|将half类型数据转换为int32_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2int8](reg_compute/reg_convert/asc_half2int8.md)|将half类型转换为int8_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2uint8](reg_compute/reg_convert/asc_half2uint8.md)|将half类型转换为uint8_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_hif82half](reg_compute/reg_convert/asc_hif82half.md)|将hifloat8_t类型数据转换为half类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162float](reg_compute/reg_convert/asc_int162float.md)|将int16_t类型转换为float类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162half](reg_compute/reg_convert/asc_int162half.md)|将int16_t类型转换为half类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162int32](reg_compute/reg_convert/asc_int162int32.md)|将int16_t类型转换为int32_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162uint32](reg_compute/reg_convert/asc_int162uint32.md)|将int16_t类型转换为uint32_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162uint8](reg_compute/reg_convert/asc_int162uint8.md)|将int16_t类型转换为uint8_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322float](reg_compute/reg_convert/asc_int322float.md)|将int32_t类型转换为float类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322int16](reg_compute/reg_convert/asc_int322int16.md)|将int32_t类型转换为int16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322int64](reg_compute/reg_convert/asc_int322int64.md)|将int32_t类型转换为int64_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322uint16](reg_compute/reg_convert/asc_int322uint16.md)|将int32_t类型转换为uint16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int642float](reg_compute/reg_convert/asc_int642float.md)|将int64_t类型数据转换为float类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_int642int32](reg_compute/reg_convert/asc_int642int32.md)|将int64_t类型转换为int32_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int82half](reg_compute/reg_convert/asc_int82half.md)|将int8_t类型转换为half类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int82int16](reg_compute/reg_convert/asc_int82int16.md)|将int8_t类型转换为int16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_rint](reg_compute/reg_convert/asc_rint.md)|将源操作数中的浮点数元素按照RINT（四舍六入五成双）模式舍入到整数值，结果保持原浮点数据类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_round](reg_compute/reg_convert/asc_round.md)|将源操作数中的浮点数元素按照ROUND（四舍五入）模式舍入到整数值，结果保持原浮点数据类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_trunc](reg_compute/reg_convert/asc_trunc.md)|将源操作数中的浮点数元素按照TRUNC（向零方向截断）模式截断到整数值，结果保持原浮点数据类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint162uint32](reg_compute/reg_convert/asc_uint162uint32.md)|将uint16_t类型转换为uint32_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint162uint8](reg_compute/reg_convert/asc_uint162uint8.md)|将uint16_t类型转换为uint8_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint322int16](reg_compute/reg_convert/asc_uint322int16.md)|将uint32_t类型转换为int16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint322uint16](reg_compute/reg_convert/asc_uint322uint16.md)|将uint32_t类型转换为uint16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint82half](reg_compute/reg_convert/asc_uint82half.md)|将uint8_t类型转换为half类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint82uint16](reg_compute/reg_convert/asc_uint82uint16.md)|将uint8_t类型转换为uint16_t类型。|`c_api/reg_compute/reg_convert.h`|

### 索引操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_arange](reg_compute/reg_index/asc_arange.md)|以传入的value为起始值，生成递增/递减的索引，并将生成的索引保存在dst中。|`c_api/reg_compute/compute/reg_index.h`|

### 逻辑计算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_and](reg_compute/reg_logic/asc_and.md)|对掩码寄存器操作：根据mask对源操作数src0、src1的有效bit进行逻辑与运算，得到新的掩码寄存器。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_not](reg_compute/reg_logic/asc_not.md)|执行矢量非运算。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_or](reg_compute/reg_logic/asc_or.md)|对源操作数src0和src1进行或（\|）运算，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_shiftleft](reg_compute/reg_logic/asc_shiftleft.md)|根据掩码mask对输入数据src0，按照src1对应元素进行左移操作，完成后将结果写入dst中。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_shiftleft_scalar](reg_compute/reg_logic/asc_shiftleft_scalar.md)|根据mask对源操作数src执行左移，左移的位数由输入参数value决定，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_shiftright](reg_compute/reg_logic/asc_shiftright.md)|根据掩码mask对输入数据src0，按照src1对应元素进行右移操作，完成后将结果写入dst中。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_shiftright_scalar](reg_compute/reg_logic/asc_shiftright_scalar.md)|根据mask对源操作数src执行右移，右移的位数由输入参数value决定，将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_xor](reg_compute/reg_logic/asc_xor.md)|根据mask对输入的src0、src1按元素异或（^）进行操作，将结果写入dst。|`c_api/reg_compute/compute/reg_logic.h`|

## 标量计算

### 位运算

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_clear_nthbit](scalar_compute/scalar_bit/asc_clear_nthbit.md)|位操作函数，用于将一个uint64_t整数bits的第idx位设置为0。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_clz](scalar_compute/scalar_bit/asc_clz.md)|计算参数前导零的数量（二进制从最高位到第一个1共有多少个0）。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_ffs](scalar_compute/scalar_bit/asc_ffs.md)|FindFirstSet接口，输入数据的二进制表示中从最低位向最高位查找第一个值为1的位，并返回其位置，如果没找到则返回-1。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_ffz](scalar_compute/scalar_bit/asc_ffz.md)|获取一个uint64_t类型数字的二进制表示中从最低有效位开始的第一个0出现的位置，如果没找到则返回-1。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_popc](scalar_compute/scalar_bit/asc_popc.md)|获取一个uint64_t类型数字的二进制中1的个数。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_set_nthbit](scalar_compute/scalar_bit/asc_set_nthbit.md)|计算一个uint64_t类型数字的指定二进制位置为1，其余位保持不变。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_sflbits](scalar_compute/scalar_bit/asc_sflbits.md)|计算一个int64_t类型数字的二进制中，从最高数值位开始与符号位相同的连续比特位的个数。|`c_api/scalar_compute/compute/scalar_bit.h`|
|[asc_zero_bits_cnt](scalar_compute/scalar_bit/asc_zero_bits_cnt.md)|获取一个uint64_t类型数字的二进制中0的个数。|`c_api/scalar_compute/compute/scalar_bit.h`|

### 数据搬入

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_load_dev](scalar_compute/scalar_load/asc_load_dev.md)|不经过DCache直接从GM地址读取整型数据。|`c_api/scalar_compute/scalar_load.h`|

### 数据搬出

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_store_dev](scalar_compute/scalar_store/asc_store_dev.md)|不经过DCache向GM地址上写数据。|`c_api/scalar_compute/scalar_store.h`|

### 类型转换

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_float2half](scalar_compute/scalar_convert/asc_float2half.md)|将float类型标量转换为half类型，舍入模式固定为ODD（最近邻奇数舍入）。|`c_api/scalar_compute/compute/scalar_convert.h`|
|[asc_float2int32](scalar_compute/scalar_convert/asc_float2int32.md)|将float类型转化为int32_t类型，并支持多种舍入模式。|`c_api/scalar_compute/compute/scalar_convert.h`|

## 原子操作

### 搬运原子操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_set_atomic_add](atomic/datamove_atomic/asc_set_atomic_add.md)|设置对后续的从UB/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为int16_t。|`c_api/atomic/datamove_atomic.h`|
|[asc_set_atomic_max](atomic/datamove_atomic/asc_set_atomic_max.md)|设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int16_t数据与GM中已有数据进行逐元素比较，并将最大值写入GM。|`c_api/atomic/datamove_atomic.h`|
|[asc_set_atomic_min](atomic/datamove_atomic/asc_set_atomic_min.md)|设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int16_t数据与GM中已有数据进行逐元素比较，并将最小值写入GM。|`c_api/atomic/datamove_atomic.h`|
|[asc_disable_dma_atomic](atomic/datamove_atomic/asc_disable_dma_atomic.md)|清空原子操作的状态。|`c_api/atomic/datamove_atomic.h`|

### 标量原子操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_atomic_add](atomic/scalar_atomic/asc_atomic_add.md)|对Global Memory中的数据与指定数据执行原子加操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_and](atomic/scalar_atomic/asc_atomic_and.md)|对Global Memory中的数据与指定数据执行原子与操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_cas](atomic/scalar_atomic/asc_atomic_cas.md)|对Global Memory中address指向的元素执行原子比较赋值操作，如果address上的数值等于指定数值compare，则对address赋值为指定数值val，否则address的数值不变。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_dec](atomic/scalar_atomic/asc_atomic_dec.md)|对Global Memory中address指向的计数器执行原子递减操作，如果address上的数值等于0或大于指定数值val，则对address赋值为val，否则将address上数值减1。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_exch](atomic/scalar_atomic/asc_atomic_exch.md)|对Global Memory中address指向的元素执行原子赋值操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_inc](atomic/scalar_atomic/asc_atomic_inc.md)|对Global Memory中address指向的计数器执行原子递增操作，如果address上的数值大于等于指定数值val，则对address赋值为0，否则将address上数值加1。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_max](atomic/scalar_atomic/asc_atomic_max.md)|对Global Memory中的数据与指定数据执行原子求最大值操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_min](atomic/scalar_atomic/asc_atomic_min.md)|对Global Memory中的数据与指定数据执行原子求最小值操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_or](atomic/scalar_atomic/asc_atomic_or.md)|对Global Memory中的数据与指定数据执行原子或操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_sub](atomic/scalar_atomic/asc_atomic_sub.md)|对Global Memory中的数据与指定数据执行原子减操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_atomic_xor](atomic/scalar_atomic/asc_atomic_xor.md)|对Global Memory中的数据与指定数据执行原子异或操作。|`c_api/atomic/scalar_atomic.h`|
|[asc_get_store_atomic_config](atomic/scalar_atomic/asc_get_store_atomic_config.md)|获取原子操作启用位与原子操作类型的值。|`c_api/atomic/scalar_atomic.h`|
|[asc_set_store_atomic_config_v1](atomic/scalar_atomic/asc_set_store_atomic_config_v1.md)|设置原子操作启用位与原子操作类型的值，适用于Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品。|`c_api/atomic/scalar_atomic.h`|

## 缓存控制

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_datacache_preload](cache_ctrl/asc_datacache_preload.md)|从源地址所在的特定GM地址预加载数据到Data Cache中。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_dcci](cache_ctrl/asc_dcci.md)|用于刷新Cache，保证Cache的一致性。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_dci](cache_ctrl/asc_dci.md)|数据缓存失效，使整个数据缓存无效化。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_get_icache_preload_status](cache_ctrl/asc_get_icache_preload_status.md)|获取ICache的Preload的状态。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_get_scalar_load_cache_mode](cache_ctrl/asc_get_scalar_load_cache_mode.md)|获取标量load路径的L2 Cache管理策略。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_get_scalar_store_cache_mode](cache_ctrl/asc_get_scalar_store_cache_mode.md)|获取标量store路径的L2 Cache管理策略。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_icache_preload](cache_ctrl/asc_icache_preload.md)|从指令所在GM地址预加载数据到对应的cacheline中。|`c_api/cache_ctrl/cache_ctrl.h`|
|[asc_set_scalar_cache_mode](cache_ctrl/asc_set_scalar_cache_mode.md)|配置标量单元访问Global Memory时的L2 Cache管理策略。|`c_api/cache_ctrl/cache_ctrl.h`|

## 同步控制

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_lock](sync/asc_lock.md)|用于AI Core内部异步流水线同步的接口。|`c_api/sync/sync.h`|
|[asc_sync](sync/asc_sync.md)|等待所有流水线操作完成。|`c_api/sync/sync.h`|
|[asc_sync_block_arrive](sync/asc_sync_block_arrive.md)|该指令用于发送同步信息数据到核间同步寄存器，设置同步点。|`c_api/sync/sync.h`|
|[asc_sync_block_wait](sync/asc_sync_block_wait.md)|和[asc_sync_block_arrive](sync/asc_sync_block_arrive.md)配合使用（通过flag_id关联），用于等待所有同步对象到达flag_id对应的同步点。|`c_api/sync/sync.h`|
|[asc_sync_data_barrier](sync/asc_sync_data_barrier.md)|用于阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可以通过参数控制）执行结束。|`c_api/sync/sync.h`|
|[asc_sync_inter_arrive](sync/asc_sync_inter_arrive.md)|和[asc_sync_inter_wait](sync/asc_sync_inter_wait.md)配合使用（通过flag_id关联），用于组间block的信号同步。|`c_api/sync/sync.h`|
|[asc_sync_inter_wait](sync/asc_sync_inter_wait.md)|等待block内同步标志。|`c_api/sync/sync.h`|
|[asc_sync_intra_arrive](sync/asc_sync_intra_arrive.md)|向核间同步寄存器发送同步信号。|`c_api/sync/sync.h`|
|[asc_sync_intra_wait](sync/asc_sync_intra_wait.md)|等待核间同步寄存器同步标志。|`c_api/sync/sync.h`|
|[asc_sync_mte2](sync/asc_sync_mte2.md)|等待PIPE_MTE2流水完成。|`c_api/sync/sync.h`|
|[asc_sync_mte3](sync/asc_sync_mte3.md)|等待PIPE_MTE3流水完成。|`c_api/sync/sync.h`|
|[asc_sync_notify](sync/asc_sync_notify.md)|设置同步标志。|`c_api/sync/sync.h`|
|[asc_sync_pipe](sync/asc_sync_pipe.md)|等待指定流水线操作完成。|`c_api/sync/sync.h`|
|[asc_sync_subblock_arrive](sync/asc_sync_subblock_arrive.md)|和[asc_sync_subblock_wait](sync/asc_sync_subblock_wait.md)配合使用（通过flag_id关联），用于组内subblock间的信号同步。|`c_api/sync/sync.h`|
|[asc_sync_subblock_wait](sync/asc_sync_subblock_wait.md)|等待subblock间同步标志。|`c_api/sync/sync.h`|
|[asc_sync_vec](sync/asc_sync_vec.md)|同步所有流水线。|`c_api/sync/sync.h`|
|[asc_sync_wait](sync/asc_sync_wait.md)|等待同步标志。|`c_api/sync/sync.h`|
|[asc_unlock](sync/asc_unlock.md)|释放缓存。|`c_api/sync/sync.h`|

## 其他操作

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[指令发射队列空闲槽位查询](experimental/asc_get_pipe_idle_slot_count.md)|获取指定流水线对应发射队列当前的空闲槽位数量。|`c_api/experimental/asc_get_pipe_idle_slot_count.h`|

## 特殊寄存器访问

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_get_ctrl](spr/asc_get_ctrl.md)|读取CTRL寄存器（控制寄存器）的值。|`c_api/spr/spr.h`|
|[asc_get_saturation_flag](spr/asc_get_saturation_flag.md)|读取指定饱和模式的开关状态。|`c_api/spr/spr.h`|
|[asc_get_saturation_strategy](spr/asc_get_saturation_strategy.md)|读取饱和控制策略。|`c_api/spr/spr.h`|
|[asc_get_squeeze_status](spr/asc_get_squeeze_status.md)|读取squeeze操作后保存至AR特殊寄存器的有效数据长度值。|`c_api/spr/spr.h`|
|[asc_set_ctrl](spr/asc_set_ctrl.md)|设置CTRL寄存器（控制寄存器）的值。|`c_api/spr/spr.h`|
|[asc_set_saturation_flag](spr/asc_set_saturation_flag.md)|设置指定饱和模式的开关状态。|`c_api/spr/spr.h`|
|[asc_set_saturation_strategy](spr/asc_set_saturation_strategy.md)|设置饱和控制策略。|`c_api/spr/spr.h`|

## 废弃接口

废弃接口的替代关系同时汇总在[附录废弃接口](../../appendix/deprecated_interface.md)。

|API名称|说明|最细粒度公共头文件|
|---|---|---|
|[asc_abs_sync（废弃）](deprecated_interface/asc_abs_sync_deprecated.md)|按元素取绝对值同步接口。|`c_api/composite/vector_compute_composite.h`|
|[asc_and（废弃）](deprecated_interface/asc_and_deprecated.md)|`asc_and`的浮点类型重载已废弃，请使用同位宽的整数重载替代。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_or（废弃）](deprecated_interface/asc_or_deprecated.md)|`asc_or`的浮点类型重载已废弃，请使用同位宽的整数重载替代。|`c_api/reg_compute/compute/reg_logic.h`|
|[asc_bfloat162e1m2x2 (废弃)](reg_compute/reg_convert/asc_bfloat162e1m2x2_deprecated.md)|将bfloat16_t类型转换为fp4x2_e1m2_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_bfloat162e2m1x2 (废弃)](reg_compute/reg_convert/asc_bfloat162e2m1x2_deprecated.md)|将bfloat16_t类型转换为fp4x2_e2m1_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_bfloat162float（废弃）](deprecated_interface/asc_bfloat162float_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_bfloat162int32（废弃）](deprecated_interface/asc_bfloat162int32_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_clear_ar_spr（废弃）](reg_compute/reg_permute_sel/asc_clear_ar_spr_deprecated.md)|对AR寄存器进行清理。请使用[asc_squeeze_and_storeunalign_init](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md)替换。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_create_iter_reg (废弃)](reg_compute/reg_addr_reg/asc_create_iter_reg_deprecated.md)|该接口已废弃，请使用asc_update_addr_reg实现此功能。|`c_api/reg_compute/compute/reg_addr_reg.h`|
|[asc_e1m2x22bfloat16 (废弃)](reg_compute/reg_convert/asc_e1m2x22bfloat16_deprecated.md)|将fp4x2_e1m2_t类型转换为bfloat16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_e2m1x22bfloat16 (废弃)](reg_compute/reg_convert/asc_e2m1x22bfloat16_deprecated.md)|将fp4x2_e2m1_t类型转换为bfloat16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_e4m32float (废弃)](reg_compute/reg_convert/asc_e4m32float_deprecated.md)|将fp8_e4m3fn_t类型转换为float类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_e5m22float (废弃)](reg_compute/reg_convert/asc_e5m22float_deprecated.md)|将vector_fp8_e5m2_t类型的源操作数以256B为单位分为四部分，读取其中一部分元素，将其转换成vector_float类型并写入目的操作数。|`c_api/reg_compute/reg_convert.h`|
|[asc_exp_sub（废弃）](deprecated_interface/asc_exp_sub_deprecated.md)|`half`输入且不带`src_pos`参数的`asc_exp_sub`原型以及`asc_exp_sub_v2`原型已废弃，`half`输入请使用`asc_exp_sub_half2float`原型（显式传入`src_pos`参数）。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_float2bfloat16（废弃）](deprecated_interface/asc_float2bfloat16_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2e4m3 (废弃)](reg_compute/reg_convert/asc_float2e4m3_deprecated.md)|将float类型转换为fp8_e4m3fn_t类型，支持RINT舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2e5m2 (废弃)](reg_compute/reg_convert/asc_float2e5m2_deprecated.md)|将float类型数据转换为fp8_e5m2类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2half（废弃）](deprecated_interface/asc_float2half_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2hif8 (废弃)](reg_compute/reg_convert/asc_float2hif8_deprecated.md)|将float类型转换为hifloat8_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2int16（废弃）](deprecated_interface/asc_float2int16_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_float2int64（废弃）](deprecated_interface/asc_float2int64_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_get_ar_spr (废弃)](spr/asc_get_ar_spr_deprecated.md)|读取指定特殊寄存器的值。|`c_api/spr/spr.h`|
|[asc_get_block_idx（废弃）](deprecated_interface/asc_get_block_idx_deprecated.md)|获取当前运行核的索引。|`c_api/utils/sys_var.h`|
|[asc_get_block_num（废弃）](deprecated_interface/asc_get_block_num_deprecated.md)|获取AI核数。|`c_api/utils/sys_var.h`|
|[asc_half2float（废弃）](deprecated_interface/asc_half2float_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2hif8（废弃）](deprecated_interface/asc_half2hif8_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2int32（废弃）](deprecated_interface/asc_half2int32_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2int4x2 (废弃)](reg_compute/reg_convert/asc_half2int4x2_deprecated.md)|将half类型数据转换为int4x2_t类型，并支持多种舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2int8（废弃）](deprecated_interface/asc_half2int8_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_half2uint8（废弃）](deprecated_interface/asc_half2uint8_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_hif82float (废弃)](reg_compute/reg_convert/asc_hif82float_deprecated.md)|将hifloat8_t类型数据转换为float类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_hif82half（废弃）](deprecated_interface/asc_hif82half_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162float（废弃）](deprecated_interface/asc_int162float_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162int32（废弃）](deprecated_interface/asc_int162int32_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162uint32（废弃）](deprecated_interface/asc_int162uint32_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int162uint8（废弃）](deprecated_interface/asc_int162uint8_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322int16（废弃）](deprecated_interface/asc_int322int16_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322int64（废弃）](deprecated_interface/asc_int322int64_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322uint16（废弃）](deprecated_interface/asc_int322uint16_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int322uint8 (废弃)](reg_compute/reg_convert/asc_int322uint8_deprecated.md)|将int32_t类型转换为uint8_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int4x22bfloat16 (废弃)](reg_compute/reg_convert/asc_int4x22bfloat16_deprecated.md)|将int4x2_t类型转换为bfloat16_t类型，无舍入模式。|`c_api/reg_compute/reg_convert.h`|
|[asc_int4x22half (废弃)](reg_compute/reg_convert/asc_int4x22half_deprecated.md)|将int4x2_t类型数据转换为half类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int4x22int16 (废弃)](reg_compute/reg_convert/asc_int4x22int16_deprecated.md)|将int4x2_t类型转换为int16_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_int642float（废弃）](deprecated_interface/asc_int642float_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int642int32（废弃）](deprecated_interface/asc_int642int32_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int82half（废弃）](deprecated_interface/asc_int82half_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int82int16（废弃）](deprecated_interface/asc_int82int16_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_int82int32 (废弃)](reg_compute/reg_convert/asc_int82int32_deprecated.md)|将int8_t类型转换为int32_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_loadalign_v1（废弃）](deprecated_interface/asc_loadalign_v1_deprecated.md)|不传入偏移的BRCV2、BRCV3和UNPACKV2搬入接口。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_v2（废弃）](deprecated_interface/asc_loadalign_v2_deprecated.md)|通过地址寄存器传入偏移的BRCV2、BRCV3和UNPACKV2搬入接口。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_v3（废弃）](deprecated_interface/asc_loadalign_v3_deprecated.md)|通过int32_t传入偏移的BRCV2、BRCV3和UNPACKV2搬入接口。|`c_api/reg_compute/load/loadalign.h`|
|[asc_loadalign_v4（废弃）](deprecated_interface/asc_loadalign_v4_deprecated.md)|通过int32_t传入偏移并启用Post Update的BRCV2、BRCV3和UNPACKV2搬入接口。|`c_api/reg_compute/load/loadalign.h`|
|[asc_muls (废弃)](reg_compute/reg_fused/asc_muls_deprecated.md)|对源操作数src和value进行乘法运算再按照数据类型转换的ROUND舍入模式转成half类型，根据mask将结果写入目的操作数dst。|`c_api/reg_compute/compute/reg_fused.h`|
|[asc_pack (废弃)](reg_compute/reg_permute_sel/asc_pack_deprecated.md)|将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的低半部分或高半部分。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_scatter（废弃）](deprecated_interface/asc_scatter_deprecated.md)|根据索引值index将源操作数src中的元素分散到目的操作数dst中。|`c_api/reg_compute/scatter/ub_scatter.h`|
|[asc_set_atomic_add_int（废弃）](deprecated_interface/asc_set_atomic_add_int_deprecated.md)|设置对后续的从UB/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为int32_t。|`c_api/atomic/datamove_atomic.h`|
|[asc_set_atomic_max_int（废弃）](deprecated_interface/asc_set_atomic_max_int_deprecated.md)|设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int32_t数据与GM中已有数据进行逐元素比较，并将最大值写入GM。|`c_api/atomic/datamove_atomic.h`|
|[asc_set_atomic_min_int（废弃）](deprecated_interface/asc_set_atomic_min_int_deprecated.md)|设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int32_t数据与GM中已有数据进行逐元素比较，并将最小值写入GM。|`c_api/atomic/datamove_atomic.h`|
|[asc_set_atomic_none（废弃）](deprecated_interface/asc_set_atomic_none_deprecated.md)|清空原子操作的状态。|`c_api/atomic/datamove_atomic.h`|
|[asc_set_gm2ub_pad（废弃）](vector_datamove/asc_set_gm2ub_pad_deprecated.md)|使用[asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory (GM)搬运到UB且源操作数非对齐时，设置连续搬运数据块左右两侧需要填补的数据值。|`c_api/vector_datamove/vector_datamove.h`|
|[asc_set_store_atomic_config_v2（废弃）](deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md)|设置原子操作启用位与原子操作类型的值，适用于Ascend 950PR/Ascend 950DT。|`c_api/atomic/scalar_atomic.h`|
|[asc_squeeze_v2 (废弃)](reg_compute/reg_permute_sel/asc_squeeze_deprecated.md)|将src中被mask选择的有效元素依次复制到dst，有效元素在dst上连续排列。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_squeeze_with_status（废弃）](reg_compute/reg_permute_sel/asc_squeeze_with_status_deprecated.md)|将src中被mask选择的有效元素依次复制到dst，并将有效数据大小保存至AR寄存器。请使用[asc_squeeze_and_storeunalign](reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md)替换。|`c_api/reg_compute/compute/reg_permute_sel.h`|
|[asc_storealign_pack_v2（废弃）](deprecated_interface/asc_storealign_v1_deprecated.md)|将有效32bit元素的低8bit数据压缩搬出到UB，不传入偏移量。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack_v2（废弃）](deprecated_interface/asc_storealign_v2_deprecated.md)|将有效32bit元素的低8bit数据压缩搬出到UB，通过地址寄存器传入偏移量。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack_v2（废弃）](deprecated_interface/asc_storealign_v3_deprecated.md)|将有效32bit元素的低8bit数据压缩搬出到UB，通过`int32_t`传入偏移量。|`c_api/reg_compute/store/storealign.h`|
|[asc_storealign_pack_postupdate_v2（废弃）](deprecated_interface/asc_storealign_v4_deprecated.md)|将有效32bit元素的低8bit数据压缩搬出到UB，并自动更新目的操作数地址。|`c_api/reg_compute/store/storealign.h`|
|[asc_storeunalign_postupdate（废弃）](deprecated_interface/asc_storeunalign_postupdate_deprecated.md)|reg计算数据搬运接口，适用于从矢量数据寄存器或掩码寄存器连续非32B对齐的起始地址连续搬出到UB的场景。|`c_api/reg_compute/store/storeunalign.h`|
|[asc_storeunalign_post（废弃）](deprecated_interface/asc_storeunalign_post_deprecated.md)|reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的尾块场景。|`c_api/reg_compute/store/storeunalign.h`|
|[asc_uint322uint8 (废弃)](reg_compute/reg_convert/asc_uint322uint8_deprecated.md)|将uint32_t类型转换为uint8_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint162uint32（废弃）](deprecated_interface/asc_uint162uint32_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint162uint8（废弃）](deprecated_interface/asc_uint162uint8_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint322int16（废弃）](deprecated_interface/asc_uint322int16_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint322uint16（废弃）](deprecated_interface/asc_uint322uint16_deprecated.md)|通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint82half（废弃）](deprecated_interface/asc_uint82half_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint82uint16（废弃）](deprecated_interface/asc_uint82uint16_deprecated.md)|通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用显式位置参数重载。|`c_api/reg_compute/reg_convert.h`|
|[asc_uint82uint32 (废弃)](reg_compute/reg_convert/asc_uint82uint32_deprecated.md)|将uint8_t类型转换为uint32_t类型。|`c_api/reg_compute/reg_convert.h`|
|[asc_mmad(废弃)](deprecated_interface/asc_mmad_deprecated.md)|矩阵乘加计算接口。|`c_api/cube_compute/cube_compute.h`|
|[asc_mmad_mx(废弃)](deprecated_interface/asc_mmad_mx_deprecated.md)|带有量化系数的矩阵乘加计算接口。|`c_api/cube_compute/cube_compute.h`|
|[asc_mmad_sparse(废弃)](deprecated_interface/asc_mmad_sparse_deprecated.md)|稀疏矩阵乘加计算接口。|`c_api/cube_compute/cube_compute.h`|
| [asc_transto5hd_sync（废弃）](deprecated_interface/asc_transto5hd_sync_deprecated.md) | 数据格式转换接口。 |
