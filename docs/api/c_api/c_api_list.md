# C API
C API文档目录，整体使用时可以引入asc_simd.h，C API列表如下：

## 数据结构

|结构名|说明|
|-----------------------|-----------------------|
| [asc_load3d_v2_config](struct/asc_load3d_v2_config.md) | Load3Dv2接口的repeat参数 |
| [asc_store_atomic_config](struct/asc_store_atomic_config.md) | 原子操作使能位与原子操作类型的值 |
| [asc_fill_value_config](struct/asc_fill_value_config.md) | fill_value的初始化参数结构体，包含[asc_fill_l0a](cube_datamove/asc_fill_l0a.md)/[asc_fill_l0b](cube_datamove/asc_fill_l0b.md)/[asc_fill_l1](cube_datamove/asc_fill_l1.md)接口需要配置的各种初始化参数。 |

## 矢量计算

矢量计算类API，单独使用时可以引入vector_compute.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| [asc_get_cmp_mask](vector_compute/asc_get_cmp_mask.md) | 获取Compare操作的比较结果。 |
| [asc_set_cmp_mask](vector_compute/asc_set_cmp_mask.md) | 为Select操作设置用于选择的掩码。 |
| [asc_get_rsvd_count](vector_compute/asc_get_rsvd_count.md) | 获取GatherMask操作后剩余的元素数量。 |
| [asc_set_mask_count](vector_compute/asc_set_mask_count.md) | 设置Mask模式为Counter模式。 |
| [asc_set_mask_norm](vector_compute/asc_set_mask_norm.md) | 设置Mask模式为Normal模式。 |
| [asc_set_vector_mask](vector_compute/asc_set_vector_mask.md) | 设置Mask值。 |
| [asc_add](vector_compute/asc_add.md) | 按元素求和。 |
| [asc_add_scalar](vector_compute/asc_add_scalar.md) | 矢量内每个元素与标量求和。 |
| [asc_sub](vector_compute/asc_sub.md) | 按元素求差。 |
| [asc_sub_scalar](vector_compute/asc_sub_scalar.md) | 矢量内每个元素与标量求差。 |
| [asc_mul](vector_compute/asc_mul.md) | 按元素求积。 |
| [asc_mul_scalar](vector_compute/asc_mul_scalar.md) | 矢量内每个元素与标量求积。 |
| [asc_div](vector_compute/asc_div.md) | 按元素求商。 |
| [asc_exp](vector_compute/asc_exp.md) | 按元素取自然指数。 |
| [asc_relu](vector_compute/asc_relu.md) | 按元素做线性整流Relu。 |
| [asc_max](vector_compute/asc_max.md) | 按元素求最大值。 |
| [asc_max_scalar](vector_compute/asc_max_scalar.md) | 矢量内每个元素与标量求最大值。 |
| [asc_min](vector_compute/asc_min.md) | 按元素求最小值。 |
| [asc_datablock_reduce_sum](vector_compute/asc_datablock_reduce_sum.md) | 对每个DataBlock内所有元素求和。 |
| [asc_datablock_reduce_max](vector_compute/asc_datablock_reduce_max.md) | 对每个DataBlock内所有元素求最大值。 |
| [asc_datablock_reduce_min](vector_compute/asc_datablock_reduce_min.md) | 对每个DataBlock内所有元素求最小值。 |
| [asc_repeat_reduce_sum](vector_compute/asc_repeat_reduce_sum.md) | 对每个Repeat内所有元素求和。 |
| [asc_repeat_reduce_max](vector_compute/asc_repeat_reduce_max.md) | 对每个Repeat内所有元素求最大值。 |
| [asc_repeat_reduce_min](vector_compute/asc_repeat_reduce_min.md) | 对每个Repeat内所有元素求最小值。 |
| [asc_get_reduce_max_cnt](vector_compute/asc_get_reduce_max_cnt.md) | 获取执行asc_repeat_reduce_max操作后的最大值，以及第一个最大值时的索引。 |
| [asc_get_reduce_min_cnt](vector_compute/asc_get_reduce_min_cnt.md) | 获取执行asc_repeat_reduce_min操作后的最小值，以及第一个最小值时的索引。 |
| [asc_brcb](vector_compute/asc_brcb.md) | 将源操作数中的每一个数填充到目的操作数的一个DataBlock中。 |
| [asc_duplicate](vector_compute/asc_duplicate.md) | 将一个变量或立即数填充到一个矢量中。 |
| [asc_select](vector_compute/asc_select.md) | 根据掩码，从两个源操作数中选取元素，输出到目的操作数。 |
| [asc_bfloat162float](vector_compute/asc_bfloat162float.md) | 数据类型转换。将bfloat16_t类型的数据转换为float类型。 |
| [asc_bfloat162int32](vector_compute/asc_bfloat162int32.md) | 数据类型转换。将bfloat16_t类型的数据转换为int32_t类型。 |
| [asc_float2bfloat16](vector_compute/asc_float2bf16.md) | 数据类型转换。将float类型的数据转换为bfloat16_t类型。 |
| [asc_float2float](vector_compute/asc_float2float.md) | 数据类型转换。将float类型的数据转换为float类型。 |
| [asc_float2half](vector_compute/asc_float2half.md) | 数据类型转换。将float类型的数据转换为half类型。 |
| [asc_half2float](vector_compute/asc_half2float.md) | 数据类型转换。将half类型的数据转换为float类型。 |
| [asc_half2int4](vector_compute/asc_half2int4.md) | 数据类型转换。将half类型的数据转换为int4b_t类型。 |
| [asc_half2int8](vector_compute/asc_half2int8.md) | 数据类型转换。将half类型的数据转换为int8_t类型。 |
| [asc_half2int16](vector_compute/asc_half2int16.md) | 数据类型转换。将half类型的数据转换为int16_t类型。 |
| [asc_half2int32](vector_compute/asc_half2int32.md) | 数据类型转换。将half类型的数据转换为int32_t类型。 |
| [asc_int42half](vector_compute/asc_int42half.md) | 数据类型转换。将int4b_t类型的数据转换为half类型。 |
| [asc_int82half](vector_compute/asc_int82half.md) | 数据类型转换。将int8_t类型的数据转换为half类型。 |
| [asc_uint82half](vector_compute/asc_uint82half.md) | 数据类型转换。将uint8_t类型的数据转换为half类型。 |
| [asc_int162float](vector_compute/asc_int162float.md) | 数据类型转换。将int16_t类型的数据转换为float类型。 |
| [asc_int322int16](vector_compute/asc_int322int16.md) | 数据类型转换。将int32_t类型的数据转换为int16_t类型。 |
| [asc_int322int64](vector_compute/asc_int322int64.md) | 数据类型转换。将int32_t类型的数据转换为int64_t类型。 |
| [asc_int642int32](vector_compute/asc_int642int32.md) | 数据类型转换。将int64_t类型的数据转换为int32_t类型。 |
| [asc_deq_int162b8](vector_compute/asc_deq_int162b8.md) | 将int16_t类型转换为int8_t或uint8_t类型，并将数据存放在每个DataBlock的上半块或下半块。 |
| [asc_set_deq_scale](vector_compute/asc_set_deq_scale.md) | 设置DEQSCALAR寄存器的值。 |
| [asc_eq](vector_compute/asc_eq.md) | 比较src0与src1在对应索引位置的元素大小。若比较结果为真，则输出结果的对应比特位设为1，否则设为0。 |
| [asc_transpose](vector_compute/asc_transpose.md) | 用于实现16*16的二维矩阵数据块转置。 |
| [asc_sqrt](vector_compute/asc_sqrt.md) | 对元素进行开方。 |
| [asc_vaxpy](vector_compute/asc_axpy.md) | 源操作数中每个元素与标量求积后和目的操作数中的对应元素相加。 |
| [asc_lt](vector_compute/asc_lt.md) | 按元素判断src0 < src1是否成立，若成立则输出结果上的对应比特位为1，否则为0。 |
| [asc_ne_scalar](vector_compute/asc_ne_scalar.md) | 按元素判断是否不等于输入标量，若成立则输出结果上的对应比特位为1，否则为0。 |
| [asc_gather_datablock](vector_compute/asc_gather_datablock.md) | 根据偏移地址按照DataBlock的粒度将源操作数收集到目的操作数中。 |
| [asc_int162half](vector_compute/asc_int162half.md) | 数据类型转换。将int16_t类型的数据转换为half类型。 |
| [asc_rcp](vector_compute/asc_rcp.md) | 执行矢量的取倒数运算。 |
| [asc_shiftright](vector_compute/asc_shiftright.md) | 对源操作数中的每个元素执行右移。 |
| [asc_mul_add](vector_compute/asc_mul_add.md) | 执行矢量的乘加运算。 |
| [asc_eq_scalar](vector_compute/asc_eq_scalar.md) | 执行矢量与标量的比较运算，如果值相等则输出1，否则输出0。 |
| [asc_gather](vector_compute/asc_gather.md) | 将源操作数按照给定的偏移按元素收集到目的操作数中。 |
| [asc_min_scalar](vector_compute/asc_min_scalar.md) | 源操作数矢量逐元素与标量相比，取较小值。 |
| [asc_gt](vector_compute/asc_gt.md) | 按元素比较两个矢量的大小关系，若比较后的结果为真，则输出结果的对应比特位为1，否则为0。 |
| [asc_vdeq_int162b8](vector_compute/asc_vdeq_int162b8.md) | 将int16_t类型转换为int8_t或uint8_t类型，并将数据存放在每个DataBlock的上半块或下半块。 |
| [asc_int322float](vector_compute/asc_int322float.md) | 将int32_t类型数据转换为float类型。 |
| [asc_abs](vector_compute/asc_abs.md) | 按元素取绝对值 |
| [asc_add_relu](vector_compute/asc_add_relu.md) | 按元素求和，再进行Relu计算（结果和0对比取较大值），并提供转换最终结果的数据类型的功能(s162s8、f322f16、f162s8)。 |
| [asc_and](vector_compute/asc_and.md) | 执行矢量与运算。 |
| [asc_axpy](vector_compute/asc_axpy.md) | 源操作数src中每个元素与标量value求积后和目的操作数dst中的对应元素相加 |
| [asc_bitsort](vector_compute/asc_bitsort.md) | Score和Index分别存储在src0和src1中，按Score进行排序（Score大的元素排前面），排序后的Score与其对应的Index一起以（Score，Index）的结构存储在dst中。 |
| [asc_deq_int322half](vector_compute/asc_deq_int322half.md) | 对输入的int32_t类型的数据按元素做量化并转换为half类型 |
| [asc_float2int16](vector_compute/asc_float2int16.md) | 将float类型数据转换为int16_t类型。 |
| [asc_float2int32](vector_compute/asc_float2int32.md) | 将float类型数据转换为int32_t类型。 |
| [asc_float2int64](vector_compute/asc_float2int64.md) | 将float类型数据转换为int64_t类型。 |
| [asc_ge](vector_compute/asc_ge.md) | Ge（greater than or equal to），逐元素比较src0 >= src1是否成立，成立则输出结果为1，否则输出结果为0，每个元素的比较结果占一个bit。 |
| [asc_ge_scalar](vector_compute/asc_ge_scalar.md) | 按元素判断src >= value是否成立，若成立则输出结果为1，否则为0。 |
| [asc_gt_scalar](vector_compute/asc_gt_scalar.md) | src中的每个元素逐个与标量value比较大小，如果某个位置上的元素大于value，则输出结果dst上的对应比特位为1，否则为0。 |
| [asc_half2uint8](vector_compute/asc_half2uint8.md) | 将half类型数据转换为uint8_t类型，支持多种舍入模式。 |
| [asc_int642float](vector_compute/asc_int642float.md) | 将int64_t类型数据转换为float类型。 |
| [asc_le](vector_compute/asc_le.md) | 按元素判断src0 <= src1是否成立，若成立则输出结果为1，否则为0。 |
| [asc_leakyrelu](vector_compute/asc_leakyrelu.md) | 执行矢量Leaky Relu运算。 |
| [asc_le_scalar](vector_compute/asc_le_scalar.md) | 按元素判断src <= value是否成立，若成立则输出结果为1，否则为0。 |
| [asc_log](vector_compute/asc_log.md) | 按元素取自然对数。 |
| [asc_lt_scalar](vector_compute/asc_lt_scalar.md) | 执行矢量中每个位置和标量比较，如果值小于标量值则为1，否则为0，结果为每个bit位按小端序排布 |
| [asc_mrgsort4](vector_compute/asc_mrgsort4.md) | 将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。 |
| [asc_get_vms4_sr](vector_compute/asc_get_vms4_sr.md) | 获取执行asc_mrgsort4操作后的队列中，每个队列已经理过的Region Proposal个数 |
| [asc_fma](vector_compute/asc_fma.md) | 按元素将src0和src1相乘并和dst相加，将最终结果存放进dst中。 |
| [asc_mul_add_relu](vector_compute/asc_mul_add_relu.md) | 按元素将src0和dst相乘并加上src1，再进行Relu计算（结果和0对比取较大值），最终结果存放进dst中。 |
| [asc_mul_cast_half2int8](vector_compute/asc_mul_cast_half2int8.md) | 按元素求积，并将结果转换为int8_t类型 |
| [asc_mul_cast_half2uint8](vector_compute/asc_mul_cast_half2uint8.md) | 按元素求积，并将结果转换为uint8_t类型。 |
| [asc_ne](vector_compute/asc_ne.md) | 按元素判断src0 != src1是否成立，若成立则输出结果为1，否则为0。 |
| [asc_not](vector_compute/asc_not.md) | 按元素做按位取反，计算公式如下。 |
| [asc_or](vector_compute/asc_or.md) | 每对元素按位或运算。 |
| [asc_reduce](vector_compute/asc_reduce.md) | 以内置固定模式对应的二进制或者用户自定义输入的数值对应的gather mask（数据收集的掩码），从源操作数中选取元素写入目的操作数中。 |
| [asc_rsqrt](vector_compute/asc_rsqrt.md) | 按元素进行开方后取倒数的计算。 |
| [asc_set_va_reg](vector_compute/asc_set_va_reg.md) | 用于设置transpose的地址，将操作数地址序列与地址寄存器关联。 |
| [asc_shiftleft](vector_compute/asc_shiftleft.md) | 将所有元素左移distance位。 |
| [asc_sub_relu](vector_compute/asc_sub_relu.md) | 按元素求差，再进行Relu计算（结果和0对比取较大值），并提供转换最终结果的数据类型的功能(s162s8、f322f16、f162s8)。 |
| [asc_transto5hd](vector_compute/asc_transto5hd.md) | 数据格式转换，一般用于将NCHW格式转换成NC1HWC0格式。 |
| [asc_pair_reduce_sum](vector_compute/asc_pair_reduce_sum.md) | 对输入数据做归约操作，得到数据总和。 |
| [asc_copy](vector_compute/asc_copy.md) | 将数据从Unified Buffer搬运到Unified Buffer。支持高维切分。 |
| [asc_squeeze](vector_compute/asc_squeeze.md) | 通过比较掩码src1，将src0中的向量压缩成更短的向量，存储到dst中。 |

## 数据搬运

数据搬运类API，单独使用时可以引入vector_datamove.h和cube_datamove.h，此类API列表如下：

| API名称                                                                                                                       |   说明   |
|-----------------------------------------------------------------------------------------------------------------------------|-----------|
| [asc_copy_gm2l0a](cube_datamove/asc_copy_gm2l0a.md)                                                                         | 将GM中的数据搬运到L0A中。 |
| [asc_copy_gm2l0b](cube_datamove/asc_copy_gm2l0b.md)                                                                         | 将GM中的数据搬运到L0B中。 | 
| [asc_copy_gm2l1](cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_arch_2201.md)                                                                              | 将GM中的数据搬运到L1中。|
| [asc_copy_gm2l1_nd2nz](cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_2201.md)                                                                  | 将GM中的数据搬运到L1中。|
| <cann-filter npu_type = "950"> [asc_copy_gm2l1_align](cube_datamove/asc_copy_gm2l1_align.md)                                | 将GM中的数据padding后搬运到L1中。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_copy_gm2l1](cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_arch_2201.md)                                               | 将GM中的数据搬运到L1中。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_copy_gm2l1_dn2nz](cube_datamove/asc_copy_gm2l1_dn2nz.md)                                | 将GM中的数据搬运到L1中，在此过程中执行DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0操作。 </cann-filter> |
| [asc_copy_l12gm](cube_datamove/asc_copy_l12gm.md)                                                                           | 将数据从L1搬运到GM。 |
| [asc_copy_l0c2gm](cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_2201.md)                                                                            | 将L0C中的数据搬运到GM中。 |
| [asc_copy_l0c2ub](cube_datamove/asc_copy_l0c2ub.md)                                                                         | 将L0C中的数据搬运到UB中。 |
| [asc_copy_l0c2l1](cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_2201.md)                                                                            | 矩阵计算完成后，对结果进行量化处理，之后将处理结果搬运到GM中。 |
| [asc_set_l13d_rpt](cube_datamove/asc_set_l13d_rpt.md)                                                                       | 用于设置Load3Dv2接口的repeat参数。 |
| [asc_fill_l0a](cube_datamove/asc_fill_l0a.md)                                                                               | 将L0A Buffer的Local Memory初始化为某一具体数值。 |
| [asc_fill_l0b](cube_datamove/asc_fill_l0b.md)                                                                               | 将L0B Buffer的Local Memory初始化为某一具体数值。 |
| [asc_fill_l1](cube_datamove/asc_fill_l1.md)                                                                                 | 将L1 Buffer的Local Memory初始化为某一具体数值。 |
| [asc_set_l13d_size](cube_datamove/asc_set_l13d_size.md)                                                                     | 设置[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)的3D格式搬运接口在L1 Buffer的边界值。 |
| [asc_load_image_to_cbuf](cube_datamove/asc_load_image_to_cbuf.md)                                                           | 将图像数据从Global Memory搬运到L1 Buffer。 |
| [asc_copy_l12bt](cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_2201.md)                                                                              | 将MMAD指令的Bias数据从L1 Buffer搬运到BiasTable Buffer中。 |
| [asc_copy_l12fb](cube_datamove/asc_copy_l12fb/asc_copy_l12fb_arch_2201.md)                                                                              | 将数据从L1 Buffer搬运到Fixpipe Buffer中，Fixpipe Buffer用于存放量化参数。 |
| [asc_copy_l12fb_v2](cube_datamove/asc_copy_l12fb_v2.md)                                                                     | 将数据从L1 Buffer搬运到Fixpipe Buffer中，Fixpipe Buffer用于存放量化参数。 |
| [asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)                                                                            | 用于搬运存放在L1 Buffer里的512B大小的矩阵到L0A Buffer里。 |
| <cann-filter npu_type = "950"> [asc_copy_l12ub](cube_datamove/asc_copy_l12ub.md)                                            | 将数据从L1 Buffer搬运到Unified Buffer中。</cann-filter> |
| [asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)                                                                            | 用于搬运存放在L1 Buffer里的512B大小的矩阵到L0B Buffer里。 |
| [asc_copy_l12l0c](cube_datamove/asc_copy_l12l0c.md)                                                                         | 将矩阵由L1 Buffer搬运到L0C Buffer中。 |
| [asc_copy_l12l0b_sparse](cube_datamove/asc_copy_l12l0b_sparse.md)                                                           | 用于搬运存放在L1 Buffer里的512B大小的稠密权重矩阵到L0B Buffer里，同时读取128B大小的索引矩阵用于稠密矩阵的稀疏化。 |
| [asc_copy_l12l0b_trans](cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_2201.md)                                                                | 该接口实现带转置的2D格式数据从L1 Buffer到L0B Buffer的加载。 |
| [asc_copy_l12l0a_trans](cube_datamove/asc_copy_l12l0a_trans.md)                                                             | 该接口实现带转置的2D格式数据从L1 Buffer到L0A Buffer的加载。 |
| [asc_set_l0c_copy_params](cube_datamove/asc_set_l0c_copy_params.md)                                                         | DataCopy（CO1->GM、CO1->A1）过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。 |
| [asc_set_l0c_copy_prequant](cube_datamove/asc_set_l0c_copy_prequant.md)                                                     | 数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的标量量化参数。 |
| <cann-filter npu_type = "950"> [asc_copy_l12l0a_mx](cube_datamove/asc_copy_l12l0a_mx.md)                                    | 将Mx scale矩阵从L1 Buffer搬运到L0A Buffer。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_copy_l12l0b_mx](cube_datamove/asc_copy_l12l0b_mx.md)                                    | 将Mx scale矩阵从L1 Buffer搬运到L0B Buffer。</cann-filter> |
| [asc_set_l13d_padding](cube_datamove/asc_set_l13d_padding.md)                                                               | 设置Pad属性描述，用于在调用asc_copy_l12l0a接口时配置填充数值。 |
| [asc_set_l13d_fmatrix](cube_datamove/asc_set_l13d_fmatrix.md)                                                               | 设置Feature map属性描述，用于在调用[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)的3D格式搬运接口时配置填充数值。从左矩阵获取FeatureMap的属性时使用该接口。|
| [asc_set_l13d_fmatrix_b](cube_datamove/asc_set_l13d_fmatrix_b.md)                                                           | 设置Feature map属性描述，用于在调用[asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)的3D格式搬运接口时配置填充数值。从右矩阵获取FeatureMap的属性时使用该接口。|
| [asc_set_l0c2gm_lrelu_alpha](cube_datamove/asc_set_l0c2gm_lrelu_alpha.md)                                                   | 用于设置asc_copy_l0c2l1或asc_copy_l0c2gm接口计算过程中使用的Leaky ReLU alpha值。该值只支持half和float两种数据类型。 |
| [asc_copy_ub2ub](vector_datamove/asc_copy_ub2ub.md)                                                                         | 将数据从Unified Buffer搬运到Unified Buffer。 |
| [asc_copy_gm2ub](vector_datamove/asc_copy_gm2ub.md)                                                                         | 将数据从Global Memory搬运到 Unified Buffer。 |
| [asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_2201.md)                                                                | 提供数据非对齐搬运的功能，将数据从Global Memory搬运到 Unified Buffer，并支持8位/16位/32位数据类型搬运。 |
| [asc_copy_ub2gm](vector_datamove/asc_copy_ub2gm.md)                                                                         | 将数据从Unified Buffer搬运到 Global Memory。 |
| [asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_2201.md)                                                                | 将数据从Unified Buffer搬运到 Global Memory，支持8位/16位/32位分块拷贝操作。 |
| [asc_set_copy_pad_val](vector_datamove/asc_set_copy_pad_val.md)                                                             | 和asc_copy_gm2ub_align或asc_copy_ub2gm_align接口配合使用，设置连续搬运数据块左右两侧需要填补的数据值。 |
| [asc_copy_ub2l1](vector_datamove/asc_copy_ub2l1.md)                                                                         | 将数据从Unified Buffer (UB) 搬运到 搬运到L1 Buffer。 |

## 标量操作

标量操作类API，单独使用时可以引入scalar_compute.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| [asc_clz](scalar_compute/asc_clz.md)| 计算参数前导零的数量（二进制从最高位到第一个1共有多少个0）。 |
| [asc_set_nthbit](scalar_compute/asc_set_nthbit.md)| 计算一个uint64_t类型数字的指定二进制位置为1，其余位保持不变。 |
| [asc_sflbits](scalar_compute/asc_sflbits.md)| 计算一个int64_t类型数字的二进制中，从最高数值位开始与符号位相同的连续比特位的个数。 |
| [asc_clear_nthbit](scalar_compute/asc_clear_nthbit.md)| 位操作函数，用于将一个uint64_t整数bits的第idx位设置为0。 |
| [asc_ffs](scalar_compute/asc_ffs.md) | FindFirstSet接口，输入数据的二进制表示中从最低位向最高位查找第一个值为1的位，并返回其位置，如果没找到则返回-1。 |
| [asc_ffz](scalar_compute/asc_ffz.md) | 获取一个uint64_t类型数字的二进制表示中从最低有效位开始的第一个0出现的位置，如果没找到则返回-1。 |
| [asc_popc](scalar_compute/asc_popc.md) | 获取一个uint64_t类型数字的二进制中1的个数。 |
| [asc_zero_bits_cnt](scalar_compute/asc_zero_bits_cnt.md) | 获取一个uint64_t类型数字的二进制中0的个数。 |

## 矩阵计算

标量操作类API，单独使用时可以引入cube_compute.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| [asc_set_mmad_direction_m](cube_compute/asc_set_mmad_direction_m.md)| 设置mmad计算时优先通过M/N中的N方向，然后通过M方向产生结果，M为矩阵的行，N为矩阵的列。 |
| [asc_set_mmad_direction_n](cube_compute/asc_set_mmad_direction_n.md)| 设置mmad计算时优先通过M/N中的M方向，然后通过N方向产生结果，M为矩阵的行，N为矩阵的列。 |
| [asc_enable_hf32_trans](cube_compute/asc_enable_hf32_trans.md)| 设置HF32模式取整方式，需要先使用asc_enable_hf32开启HF32取整模式。 |
| [asc_mmad](cube_compute/asc_mmad.md) | 完成矩阵乘加操作。 |
| [asc_mmad_sparse](cube_compute/asc_mmad_sparse.md) | 完成矩阵乘加操作，传入的左矩阵A为稀疏矩阵，右矩阵B为稠密矩阵。 |
| [asc_set_fp32_mode](cube_compute/asc_set_fp32_mode.md) | 用于设置Mmad计算开启FP32模式，开启该模式后L0A Buffer/L0B Buffer中的FP32数据在参与Mmad计算之前不做舍入处理。 |
| [asc_set_l0c2gm_config](cube_compute/asc_set_l0c2gm_config.md) | 数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的矢量量化参数。 |
| [asc_get_l0c2gm_relu](cube_compute/asc_get_l0c2gm_relu.md) | 数据搬运过程中进行随路量化时，通过调用该接口获取ReLU操作前矢量的起始地址。 |
| [asc_get_l0c2gm_unitflag](cube_compute/asc_get_l0c2gm_unitflag.md) | 数据搬运过程中进行随路量化时，通过调用该接口获取unit_flag设置。 |
| [asc_get_l0c2gm_prequant](cube_compute/asc_get_l0c2gm_prequant.md) | 数据搬运过程中进行随路量化时，通过调用该接口获取量化操作前矢量的起始地址。 |
| [asc_enable_hf32](cube_compute/asc_enable_hf32.md) | 用于设置Mmad计算开启HF32模式，开启该模式后L0A Buffer/L0B Buffer中的FP32数据将在参与Mmad计算之前被舍入为HF32。 |


## 同步控制

同步控制类API，单独使用时可以引入sync.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| <cann-filter npu_type = "950"> [asc_unlock](sync/asc_unlock.md)| 释放缓存。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_lock](sync/asc_lock.md)| 用于AI Core内部异步流水线同步的接口。</cann-filter> |
| [asc_sync_notify](sync/asc_sync_notify.md)| 设置同步标志。 |
| [asc_sync_wait](sync/asc_sync_wait.md)| 等待同步标志。 |
| [asc_sync_pipe](sync/asc_sync_pipe.md)| 等待指定流水线操作完成。 |
| [asc_sync](sync/asc_sync.md)| 等待所有流水线操作完成。 |
| [asc_sync_vec](sync/asc_sync_vec.md)| 同步所有流水线。 |
| [asc_sync_mte3](sync/asc_sync_mte3.md)| 等待PIPE_MTE3流水完成。 |
| [asc_sync_mte2](sync/asc_sync_mte2.md)| 等待PIPE_MTE2流水完成。 |
| [asc_sync_data_barrier](sync/asc_sync_data_barrier.md) | 用于阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可以通过参数控制）执行结束。 |
| [asc_sync_block_arrive](sync/asc_sync_block_arrive.md) | 该指令用于发送同步信息数据到核间同步寄存器，设置同步点。 |
| [asc_sync_block_wait](sync/asc_sync_block_wait.md) | 和[asc_sync_block_arrive](sync/asc_sync_block_arrive.md)配合使用（通过flagID关联），用于等待所有同步对象到达flagID对应的同步点。 |
| <cann-filter npu_type = "950"> [asc_sync_intra_wait](sync/asc_sync_intra_wait.md)| 等待核间同步寄存器同步标志。 </cann-filter> |
| <cann-filter npu_type = "950"> [asc_sync_intra_arrive](sync/asc_sync_intra_arrive.md)| 向核间同步寄存器发送同步信号。 </cann-filter> |
| <cann-filter npu_type = "950"> [asc_sync_inter_wait](sync/asc_sync_inter_wait.md)| 等待block内同步标志。 </cann-filter> |

## 系统变量

系统变量类API，单独使用时可以引入sys_var.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| [asc_get_block_num](sys_var/asc_get_block_num.md) | 获取AI核数。 |
| [asc_get_block_idx](sys_var/asc_get_block_idx.md) | 获取当前运行核的索引。 |
| [asc_get_core_id](sys_var/asc_get_core_id.md) | 获取当前核的编号。 |
| [asc_get_sub_block_id](sys_var/asc_get_sub_block_id.md) | 获取AI Core上Vector核的ID。 |
| [asc_get_sub_block_num](sys_var/asc_get_sub_block_num.md) | 分离模式下，获取一个AI Core上Cube Core（AIC）或者Vector Core（AIV）的数量。 |
| [asc_set_ctrl](sys_var/asc_set_ctrl.md) | 设置CTRL寄存器（控制寄存器）的值。 |
| [asc_get_ctrl](sys_var/asc_get_ctrl.md) | 读取CTRL寄存器（控制寄存器）的值。 |
| [asc_get_phy_buf_addr](sys_var/asc_get_phy_buf_addr.md) | 基于偏移量获取片上实际物理地址。 |
| [asc_get_system_cycle](sys_var/asc_get_system_cycle.md) | 获取当前系统cycle数。 |
| [asc_get_arch_ver](sys_var/asc_get_arch_ver.md) | 获取当前AI处理器架构版本号。 |
| [asc_get_program_counter](sys_var/asc_get_program_counter.md) | 获取程序计数器的指针，程序计数器用于记录当前程序执行的位置。 |
| [asc_get_ffts_base_addr](sys_var/asc_get_ffts_base_addr.md) | 获取核间同步寄存器的基地址。 |
| [asc_set_ffts_base_addr](sys_var/asc_set_ffts_base_addr.md) | 在[asc_sync_block_arrive](sync/asc_sync_block_arrive.md)和[asc_sync_block_wait](sync/asc_sync_block_wait.md)之前使用，设置核间同步寄存器的基地址。 |
| [asc_get_phy_stack_base](sys_var/asc_get_phy_stack_base.md) | 获取物理堆栈基地址。 |
| [asc_get_smmu_tag_version](sys_var/asc_get_smmu_tag_version.md) | 获取SMMU（System Memory Management Unit）版本信息。 |
| [asc_get_status](sys_var/asc_get_status.md) | 获取状态信息。 |
| [asc_get_sys_virtual_base](sys_var/asc_get_sys_virtual_base.md) | 获取系统虚拟基地址。 |
| [asc_get_vf_len](sys_var/asc_get_vf_len.md) | 获取Tensor位宽VL（Vector Length）的大小。 |
| <cann-filter npu_type = "950"> [asc_set_gm2l1_nz_para](sys_var/asc_set_gm2l1_nz_para.md) | 设置MTE2_NZ_PARA寄存器的值。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_set_l12l0_padding_val](sys_var/asc_set_l12l0_padding_val.md) | 设置PADDING_B寄存器的值。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_set_l0c2gm_quant_post](sys_var/asc_set_l0c2gm_quant_post.md) | 设置QUANT_POST寄存器的值。</cann-filter> |
| <cann-filter npu_type = "950"> [asc_set_l0c2gm_relu_alpha](sys_var/asc_set_l0c2gm_relu_alpha.md) | 设置RELU_ALPHA寄存器的值。</cann-filter> |
| [asc_set_l0c2gm_channel_para](sys_var/asc_set_l0c2gm_channel_para.md) | 对通道步长参数的专用寄存器的比特位进行设置。 |
| [asc_set_l3d_rpt_b](sys_var/asc_set_l3d_rpt_b.md) | 用于设置接口asc_copy_l12l0a、asc_copy_l12l0b的2D格式搬运的repeat参数。|


## 缓存控制

缓存控制类API，单独使用时可以引入cache_ctrl.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| [asc_datacache_preload](cache_ctrl/asc_datacache_preload.md)| 从源地址所在的特定GM地址预加载数据到Data Cache中。 |
| [asc_dcci](cache_ctrl/asc_dcci.md) | 用于刷新Cache， 保证Cache的一致性。 |
| <cann-filter npu_type = "950"> [asc_dci](cache_ctrl/asc_dci.md) | 数据缓存失效，使整个数据缓存无效化。</cann-filter>|
| [asc_get_icache_preload_status](cache_ctrl/asc_get_icache_preload_status.md) | 获取ICache的Preload的状态。 |
| [asc_icache_preload](cache_ctrl/asc_icache_preload.md) | 从指令所在DDR地址预加载数据到对应的cacheline中。 |

## 原子操作

原子操作类API，单独使用时可以引入atomic.h，此类API列表如下：

|   API名称   |   说明   |
|----------|-----------|
| [asc_set_atomic_add_bfloat](simd_atomic/asc_set_atomic_add.md) | 设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为bfloat16_t。 |
| [asc_set_atomic_add_float](simd_atomic/asc_set_atomic_add.md) | 设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为float。 |
| [asc_set_atomic_add_float16](simd_atomic/asc_set_atomic_add.md) | 设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为half。 |
| [asc_set_atomic_add_int](simd_atomic/asc_set_atomic_add.md) | 设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为int32_t。 |
| [asc_set_atomic_add_int8](simd_atomic/asc_set_atomic_add.md) | 设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为int8_t。 |
| [asc_set_atomic_add_int16](simd_atomic/asc_set_atomic_add.md) | 设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为int16_t。 |
| [asc_set_atomic_max_bfloat](simd_atomic/asc_set_atomic_max.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的bfloat16_t数据与GM中已有数据进行逐元素比较，并将最大值写入GM。 |
| [asc_set_atomic_max_float](simd_atomic/asc_set_atomic_max.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的float数据与GM中已有数据进行逐元素比较，并将最大值写入GM。 |
| [asc_set_atomic_max_float16](simd_atomic/asc_set_atomic_max.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的half数据与GM中已有数据进行逐元素比较，并将最大值写入GM。 |
| [asc_set_atomic_max_int](simd_atomic/asc_set_atomic_max.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int32_t数据与GM中已有数据进行逐元素比较，并将最大值写入GM。 |
| [asc_set_atomic_max_int8](simd_atomic/asc_set_atomic_max.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int8_t数据与GM中已有数据进行逐元素比较，并将最大值写入GM。 |
| [asc_set_atomic_max_int16](simd_atomic/asc_set_atomic_max.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int16_t数据与GM中已有数据进行逐元素比较，并将最大值写入GM。 |
| [asc_set_atomic_min_bfloat](simd_atomic/asc_set_atomic_min.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的bfloat16_t数据与GM中已有数据进行逐元素比较，并将最小值写入GM。 |
| [asc_set_atomic_min_float](simd_atomic/asc_set_atomic_min.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的float数据与GM中已有数据进行逐元素比较，并将最小值写入GM。 |
| [asc_set_atomic_min_float16](simd_atomic/asc_set_atomic_min.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的half数据与GM中已有数据进行逐元素比较，并将最小值写入GM。 |
| [asc_set_atomic_min_int](simd_atomic/asc_set_atomic_min.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int32_t数据与GM中已有数据进行逐元素比较，并将最小值写入GM。 |
| [asc_set_atomic_min_int8](simd_atomic/asc_set_atomic_min.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int8_t数据与GM中已有数据进行逐元素比较，并将最小值写入GM。 |
| [asc_set_atomic_min_int16](simd_atomic/asc_set_atomic_min.md) | 设置计算结果以原子比较的方式传输到GM。在拷贝前，将待传输的int16_t数据与GM中已有数据进行逐元素比较，并将最小值写入GM。 |
| [asc_set_store_atomic_config_v1](simd_atomic/asc_set_store_atomic_config_v1.md)| 设置原子操作使能位与原子操作类型的值，适用于Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品。 |
<cann-filter npu_type="950">
| [asc_set_store_atomic_config_v2](simd_atomic/asc_set_store_atomic_config_v2.md)| 设置原子操作使能位与原子操作类型的值，适用于Ascend 950PR/Ascend 950DT。 |
</cann-filter>
| [asc_get_store_atomic_config](simd_atomic/asc_get_store_atomic_config.md)| 获取原子操作使能位与原子操作类型的值。 |
| [asc_set_atomic_none](simd_atomic/asc_set_atomic_none.md) | 清空原子操作的状态。 |

## 其他操作

|   API名称   |   说明   |
|----------|-----------|
| [asc_init](misc/asc_init.md)| 初始化NPU状态。 |

<cann-filter npu_type="950">

## Reg数据搬运

|   API名称   |   说明   |
|----------|-----------|
| [asc_loadalign](reg/reg_load/asc_loadalign/asc_loadalign_v1.md) | 对齐数据搬运接口，从UB连续对齐搬入目的操作数，支持多种搬入模式。 |
| [asc_loadalign_postupdate](reg/reg_load/asc_loadalign_postupdate.md) | 将数据从UB搬入MaskReg，使能post mod。 |
| [asc_storealign](reg/reg_store/asc_storealign/asc_storealign_v1.md) | reg计算数据搬运接口，适用于从矢量数据寄存器连续对齐搬出到UB的场景，并支持多种搬出模式。 |
| [asc_gather](reg/reg_load/asc_gather.md) | 根据索引位置index将源操作数src按元素收集到目的操作数dst中。 |
| [asc_gather_datablock](reg/reg_load/asc_gather_datablock.md) | 给定源操作数在UB中的基地址和索引，根据索引位置将源操作数按DataBlock收集到目的操作数中。 |
| [asc_get_mask_spr](reg/reg_load/asc_get_mask_spr.md) | 从特殊寄存器SPR{MASK1, MASK0}读取mask值并根据数据类型格式返回对应的mask数据，MASK0、MASK1均为64bit的寄存器。 |
| [asc_load](reg/reg_load/asc_load.md) | reg计算数据搬运接口，支持从UB非32字节对齐的源地址src搬运至矢量数据寄存器，搬运量为VL。 |
| [asc_loadunalign](reg/reg_load/asc_loadunalign.md) | reg计算数据搬运接口，适用于从UB非32B对齐的起始地址连续搬入矢量数据寄存器的场景。 |
| [asc_loadunalign_pre](reg/reg_load/asc_loadunalign_pre.md) | 用于在进行非对齐数据搬入前的初始化，需配合[asc_loadunalign](reg/reg_load/asc_loadunalign.md)接口使用。 |
| [asc_store](reg/reg_store/asc_store.md) | reg计算数据搬运接口，适用于从矢量数据寄存器搬出到UB的场景，不区分是否对齐，在追求极致性能时，应尽量避免使用该接口。 |
| [asc_storeunalign](reg/reg_store/asc_storeunalign.md) | reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的场景。 |
| [asc_storeunalign_postupdate](reg/reg_store/asc_storeunalign_postupdate.md) | reg计算数据搬运接口，适用于从矢量数据寄存器连续非32B对齐的起始地址连续搬出到UB的场景。 |

## Reg矢量计算

|   API名称   |   说明   |
|----------|-----------|
| [asc_abs](reg/reg_vector/asc_abs.md) | 逐元素计算绝对值。 |
| [asc_abs_sub](reg/reg_vector/asc_abs_sub.md) | 逐元素计算差的绝对值。 |
| [asc_add](reg/reg_vector/asc_add.md) | 按元素执行加法运算。 |
| [asc_addc](reg/reg_vector/asc_addc.md) | 按元素执行带进位的加法运算。 |
| [asc_add_scalar](reg/reg_vector/asc_add_scalar.md) | 按元素执行矢量和标量的加法运算。 |
| [asc_and](reg/reg_vector/asc_and.md) | 对掩码寄存器操作：根据mask对源操作数src0、src1的有效bit进行逻辑与运算，得到新的掩码寄存器。 |
| [asc_arange](reg/reg_vector/asc_arange.md) | 以传入的value为起始值，生成递增/递减的索引，并将生成的索引保存在dst中。 |
| [asc_axpy](reg/reg_vector/asc_axpy.md) | 根据mask对源操作数src、value进行按元素做乘加操作，将结果写入目的操作数dst。 |
| [asc_bfloat162e1m2x2](reg/reg_vector/asc_bfloat162e1m2x2.md) | 将bfloat16_t类型转换为fp4x2_e1m2_t类型，并支持多种舍入模式。 |
| [asc_bfloat162e2m1x2](reg/reg_vector/asc_bfloat162e2m1x2.md) | 将bfloat16_t类型转换为fp4x2_e2m1_t类型，并支持多种舍入模式。 |
| [asc_bfloat162float](reg/reg_vector/asc_bfloat162float.md) | 将bfloat16_t类型转换为float类型，无舍入模式。 |
| [asc_bfloat162half](reg/reg_vector/asc_bfloat162half.md) | 将bfloat16_t类型转换为half类型，并支持多种舍入模式和饱和/非饱和模式。 |
| [asc_bfloat162int32](reg/reg_vector/asc_bfloat162int32.md) | 将bfloat16_t类型数据转换为int32_t类型，并支持多种舍入模式。 |
| [asc_clear_ar_spr](reg/reg_vector/asc_clear_ar_spr.md) | 对AR寄存器进行清理，AR寄存器通常由[asc_squeeze](reg/reg_vector/asc_squeeze.md)接口使用。 |
| [asc_copy](reg/reg_vector/asc_copy.md) | 完成数据在寄存器内的搬运。 |
| [asc_create_iter_reg](reg/reg_vector/asc_create_iter_reg.md) | 地址寄存器通过该接口初始化，然后在循环之中使用地址寄存器存储地址偏移量。 |
| [asc_create_mask](reg/reg_vector/asc_create_mask.md) | 根据入参生成相应的掩码寄存器。 |
| [asc_cumulative_histogram](reg/reg_vector/asc_cumulative_histogram.md) | 对直方图数据进行累计统计。 |
| [asc_deintlv](reg/reg_vector/asc_deintlv.md) | 给定源操作数src0和src1，将src0和src1中的元素解交织存入结果操作数dst0和dst1中。 |
| [asc_div](reg/reg_vector/asc_div.md) | 按元素执行除法运算。 |
| [asc_duplicate](reg/reg_vector/asc_duplicate.md) | 根据mask将源操作数src的最低位元素填充到目的操作数dst。 |
| [asc_duplicate_scalar](reg/reg_vector/asc_duplicate_scalar.md) | 根据mask将value填充到目的操作数dst。 |
| [asc_e1m2x22bfloat16](reg/reg_vector/asc_e1m2x22bfloat16.md) | 将fp4x2_e1m2_t类型转换为bfloat16_t类型。 |
| [asc_e2m1x22bfloat16](reg/reg_vector/asc_e2m1x22bfloat16.md) | 将fp4x2_e2m1_t类型转换为bfloat16_t类型。 |
| [asc_e4m32float](reg/reg_vector/asc_e4m32float.md) | 将fp8_e4m3fn_t类型转换为float类型。 |
| [asc_e5m22float](reg/reg_vector/asc_e5m22float.md) | 将vector_fp8_e5m2_t类型的源操作数以256B为单位分为四部分，读取其中一部分元素，将其转换成vector_float类型并写入目的操作数。 |
| [asc_eq](reg/reg_vector/asc_eq.md) | 对源操作数逐元素比较是否相等。 |
| [asc_eq_scalar](reg/reg_vector/asc_eq_scalar.md) | 对源操作数逐元素比较是否与标量相等。 |
| [asc_exp](reg/reg_vector/asc_exp.md) | 对源操作数逐元素计算指数。 |
| [asc_exp_sub](reg/reg_vector/asc_exp_sub.md) | 将src0与src1相减，差值作为e的指数计算。 |
| [asc_float2bfloat16](reg/reg_vector/asc_float2bfloat16.md) | 将float类型转换为bfloat16_t类型，并支持多种舍入模式。 |
| [asc_float2e4m3](reg/reg_vector/asc_float2e4m3.md) | 将float类型转换为fp8_e4m3fn_t类型，支持RINT舍入模式。 |
| [asc_float2e5m2](reg/reg_vector/asc_float2e5m2.md) | 将float类型数据转换为fp8_e5m2类型。 |
| [asc_float2half](reg/reg_vector/asc_float2half.md) | 将float类型转换为half类型，并支持多种舍入模式。 |
| [asc_float2hif8](reg/reg_vector/asc_float2hif8.md) | 将float类型转换为hifloat8_t类型，并支持多种舍入模式。 |
| [asc_float2int16](reg/reg_vector/asc_float2int16.md) | 将float类型转换为int16_t类型，并支持多种舍入模式。 |
| [asc_float2int32](reg/reg_vector/asc_float2int32.md) | 将float类型转换为int32_t类型，并支持多种舍入模式。 |
| [asc_float2int64](reg/reg_vector/asc_float2int64.md) | 将float类型转换为int64_t类型，并支持多种舍入模式。 |
| [asc_frequency_histogram](reg/reg_vector/asc_frequency_histogram.md) | 对直方图数据进行频率统计。 |
| [asc_ge](reg/reg_vector/asc_ge.md) | ge（greater than or equal to），对源操作数执行逐元素比较。 |
| [asc_ge_scalar](reg/reg_vector/asc_ge_scalar.md) | ge（greater than or equal to），对源操作数与标量执行逐元素比较。 |
| [asc_gt](reg/reg_vector/asc_gt.md) | gt（greater than），对源操作数执行逐元素比较。 |
| [asc_gt_scalar](reg/reg_vector/asc_gt_scalar.md) | gt（greater than），对源操作数与标量执行逐元素比较。 |
| [asc_half2bf16](reg/reg_vector/asc_half2bf16.md) | 将half类型数据转换为bfloat16_t类型，并支持多种舍入模式。 |
| [asc_half2float](reg/reg_vector/asc_half2float.md) | 将half类型数据转换为float类型。 |
| [asc_half2hif8](reg/reg_vector/asc_half2hif8.md) | 将half类型转换为hifloat8_t类型，并支持多种舍入模式。 |
| [asc_half2int16](reg/reg_vector/asc_half2int16.md) | 将half类型数据转换为int16_t类型，并支持多种舍入模式。 |
| [asc_half2int32](reg/reg_vector/asc_half2int32.md) | 将half类型数据转换为int32_t类型，并支持多种舍入模式。 |
| [asc_half2int4x2](reg/reg_vector/asc_half2int4x2.md) | 将half类型数据转换为int4x2_t类型，并支持多种舍入模式。 |
| [asc_half2int8](reg/reg_vector/asc_half2int8.md) | 将half类型转换为int8_t类型，并支持多种舍入模式。 |
| [asc_half2uint8](reg/reg_vector/asc_half2uint8.md) | 将half类型转换为uint8_t类型，并支持多种舍入模式。 |
| [asc_hif82half](reg/reg_vector/asc_hif82half.md) | 将hifloat8_t类型数据转换为half类型。 |
| [asc_int162float](reg/reg_vector/asc_int162float.md) | 将int16_t类型转换为float类型。 |
| [asc_int162half](reg/reg_vector/asc_int162half.md) | 将int16_t类型转换为half类型。 |
| [asc_int162int32](reg/reg_vector/asc_int162int32.md) | 将int16_t类型转换为int32_t类型。 |
| [asc_int162uint32](reg/reg_vector/asc_int162uint32.md) | 将int16_t类型转换为uint32_t类型。 |
| [asc_int162uint8](reg/reg_vector/asc_int162uint8.md) | 将int16_t类型转换为uint8_t类型。 |
| [asc_int322float](reg/reg_vector/asc_int322float.md) | 将int32_t类型转换为float类型，并支持多种舍入模式。 |
| [asc_int322int16](reg/reg_vector/asc_int322int16.md) | 将int32_t类型转换为int16_t类型。 |
| [asc_int322int64](reg/reg_vector/asc_int322int64.md) | 将int32_t类型转换为int64_t类型。 |
| [asc_int322uint16](reg/reg_vector/asc_int322uint16.md) | 将int32_t类型转换为uint16_t类型。 |
| [asc_int322uint8](reg/reg_vector/asc_int322uint8.md) | 将int32_t类型转换为uint8_t类型。 |
| [asc_int4x22bfloat16](reg/reg_vector/asc_int4x22bfloat16.md) | 将int4x2_t类型转换为bfloat16_t类型，无舍入模式。 |
| [asc_int4x22half](reg/reg_vector/asc_int4x22half.md) | 将int4x2_t类型数据转换为half类型。 |
| [asc_int4x22int16](reg/reg_vector/asc_int4x22int16.md) | 将int4x2_t类型转换为int16_t类型。 |
| [asc_int642float](reg/reg_vector/asc_int642float.md) | 将int64_t类型数据转换为float类型，并支持多种舍入模式。 |
| [asc_int642int32](reg/reg_vector/asc_int642int32.md) | 将int64_t类型转换为int32_t类型。 |
| [asc_int82half](reg/reg_vector/asc_int82half.md) | 将int8_t类型转换为half类型。 |
| [asc_int82int16](reg/reg_vector/asc_int82int16.md) | 将int8_t类型转换为int16_t类型。 |
| [asc_int82int32](reg/reg_vector/asc_int82int32.md) | 将int8_t类型转换为int32_t类型。 |
| [asc_intlv](reg/reg_vector/asc_intlv.md) | 将源操作数src0和src1中的元素交织存入目的操作数dst0和dst1中。 |
| [asc_le](reg/reg_vector/asc_le.md) | le（less than or equal to），对源操作数执行逐元素比较。 |
| [asc_leakyrelu](reg/reg_vector/asc_leakyrelu.md) | 按元素执行Leaky ReLU（Leaky Rectified Linear Unit）操作。 |
| [asc_le_scalar](reg/reg_vector/asc_le_scalar.md) | le（less than or equal to），对源操作数与标量执行逐元素比较。 |
| [asc_ln](reg/reg_vector/asc_ln.md) | 对源操作数逐元素计算自然对数。 |
| [asc_lt](reg/reg_vector/asc_lt.md) | lt（less than），对源操作数执行逐元素比较。 |
| [asc_lt_scalar](reg/reg_vector/asc_lt_scalar.md) | lt（less than），对源操作数与标量执行逐元素比较。 |
| [asc_madd](reg/reg_vector/asc_madd.md) | madd（multiply-add），对源操作数执行逐元素乘法和加法。 |
| [asc_max](reg/reg_vector/asc_max.md) | 按元素求最大值。 |
| [asc_max_scalar](reg/reg_vector/asc_max_scalar.md) | 按元素求矢量和标量的最大值。 |
| [asc_mem_bar](reg/reg_vector/asc_mem_bar.md) | Reg计算宏函数内不同流水线之间的同步指令。 |
| [asc_min](reg/reg_vector/asc_min.md) | 根据mask对源操作数src0、src1进行按元素求最小值操作，将结果写入目的操作数dst。 |
| [asc_min_scalar](reg/reg_vector/asc_min_scalar.md) | 源操作数矢量内每个元素与标量比较，取较小值。 |
| [asc_mul](reg/reg_vector/asc_mul.md) | 对源操作数src0和src1进行乘法运算，将结果写入目的操作数dst。 |
| [asc_mull](reg/reg_vector/asc_mull.md) | 根据mask对输入数据src0、src1按元素相乘，将结果写入dst0，溢出部分写入dst1。 |
| [asc_muls](reg/reg_vector/asc_muls.md) | 对源操作数src和value进行乘法运算再按照数据类型转换的ROUND舍入模式转成half类型，根据mask将结果写入目的操作数dst。 |
| [asc_mul_scalar](reg/reg_vector/asc_mul_scalar.md) | 对源操作数src、value进行按元素乘法操作，将结果写入目的操作数dst。 |
| [asc_ne](reg/reg_vector/asc_ne.md) | ne（not equal），对源操作数执行逐元素比较。 |
| [asc_neg](reg/reg_vector/asc_neg.md) | 根据mask对源操作数src进行取相反数操作，将结果写入目的操作数dst。 |
| [asc_ne_scalar](reg/reg_vector/asc_ne_scalar.md) | src中的每个元素逐个与标量value比较大小，如果src_i != value，则输出结果dst对应比特位为1，否则为0。 |
| [asc_not](reg/reg_vector/asc_not.md) | 执行矢量非运算。 |
| [asc_or](reg/reg_vector/asc_or.md) | 对源操作数src0和src1进行或（|）运算，将结果写入目的操作数dst。 |
| [asc_pack](reg/reg_vector/asc_pack.md) | 将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的低半部分或高半部分。 |
| [asc_pair_reduce_sum](reg/reg_vector/asc_pair_reduce_sum.md) | PairReduceSum: 相邻两个（奇偶）元素求和，结果写入dst。 |
| [asc_prelu](reg/reg_vector/asc_prelu.md) | 源操作数src0大于0的情况下直接将src0写入目的操作数dst，否则将src0 * src1的结果写入dst。 |
| [asc_reduce_max](reg/reg_vector/asc_reduce_max.md) | 根据mask对源操作数src进行归约最大值操作，将结果写入目的操作数dst。 |
| [asc_reduce_max_datablock](reg/reg_vector/asc_reduce_max_datablock.md) | 找出DataBlock中的最大值，并将最终的计算结果依次保存在dst中的最低位。 |
| [asc_reduce_min](reg/reg_vector/asc_reduce_min.md) | 根据mask对源操作数src进行归约最小值操作，将结果写入目的操作数dst。 |
| [asc_reduce_min_datablock](reg/reg_vector/asc_reduce_min_datablock.md) | 根据mask将每个DataBlock(32B)中的最小值，依次保存在dst中的最低位。 |
| [asc_reduce_sum](reg/reg_vector/asc_reduce_sum.md) | 归约求和功能，用于将src中的所有参与计算的元素求和，得到的结果保存在dst中。 |
| [asc_reduce_sum_datablock](reg/reg_vector/asc_reduce_sum_datablock.md) | 归约求和功能，用于将src每个DataBlock(32B)中参与计算的元素求和，得到的结果依次保存在dst中。 |
| [asc_relu](reg/reg_vector/asc_relu.md) | 逐元素执行ReLU运算。 |
| [asc_select](reg/reg_vector/asc_select.md) | 根据mask的比特位值，从源操作数src0、src1中选择元素，得到目的操作数dst。 |
| [asc_shiftleft](reg/reg_vector/asc_shiftleft.md) | 根据掩码mask对输入数据src0，按照src1对应元素进行左移操作，完成后将结果写入dst中。 |
| [asc_shiftleft_scalar](reg/reg_vector/asc_shiftleft_scalar.md) | 根据mask对源操作数src执行左移，左移的位数由输入参数value决定，将结果写入目的操作数dst。 |
| [asc_shiftright](reg/reg_vector/asc_shiftright.md) | 根据掩码mask对输入数据src0，按照src1对应元素进行右移操作，完成后将结果写入dst中。 |
| [asc_shiftright_scalar](reg/reg_vector/asc_shiftright_scalar.md) | 根据mask对源操作数src执行右移，右移的位数由输入参数value决定，将结果写入目的操作数dst。 |
| [asc_sqrt](reg/reg_vector/asc_sqrt.md) | 逐元素计算平方根。 |
| [asc_squeeze](reg/reg_vector/asc_squeeze.md) | 将src中被mask选择的有效元素依次复制到dst，有效元素在dst上连续排列。 |
| [asc_sub](reg/reg_vector/asc_sub.md) | 按元素执行减法运算。 |
| [asc_subc](reg/reg_vector/asc_subc.md) | 按元素执行带借位的减法运算。 |
| [asc_truncate](reg/reg_vector/asc_truncate.md) | 将源操作数的浮点数元素截断到整数位，同时源操作数的数据类型保持不变，并支持多种舍入模式。 |
| [asc_uint162uint32](reg/reg_vector/asc_uint162uint32.md) | 将uint16_t类型转换为uint32_t类型。 |
| [asc_uint162uint8](reg/reg_vector/asc_uint162uint8.md) | 将uint16_t类型转换为uint8_t类型。 |
| [asc_uint322int16](reg/reg_vector/asc_uint322int16.md) | 将uint32_t类型转换为int16_t类型。 |
| [asc_uint322uint16](reg/reg_vector/asc_uint322uint16.md) | 将uint32_t类型转换为uint16_t类型。 |
| [asc_uint322uint8](reg/reg_vector/asc_uint322uint8.md) | 将uint32_t类型转换为uint8_t类型。 |
| [asc_uint82half](reg/reg_vector/asc_uint82half.md) | 将uint8_t类型转换为half类型。 |
| [asc_uint82uint16](reg/reg_vector/asc_uint82uint16.md) | 将uint8_t类型转换为uint16_t类型。 |
| [asc_uint82uint32](reg/reg_vector/asc_uint82uint32.md) | 将uint8_t类型转换为uint32_t类型。 |
| [asc_unpack](reg/reg_vector/asc_unpack.md) | 矢量解包操作。 |
| [asc_unsqueeze](reg/reg_vector/asc_unsqueeze.md) | 根据mask进行解压缩，将生成的数据输出到dst。 |
| [asc_update_mask](reg/reg_vector/asc_update_mask.md) | 根据value大小生成对应的掩码寄存器中的值。 |
| [asc_xor](reg/reg_vector/asc_xor.md) | 根据mask对输入的src0、src1按元素异或（^）进行操作，将结果写入dst。 |

</cann-filter>
