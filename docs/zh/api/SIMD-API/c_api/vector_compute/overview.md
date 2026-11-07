# 概述

## API列表

### 基础算术

**表1** 基础算术接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_abs](vector_arith/asc_abs.md) | 按元素取绝对值。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_add](vector_arith/asc_add.md) | 按元素求和。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_add_scalar](vector_arith/asc_add_scalar.md) | 矢量内每个元素与标量求和。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_div](vector_arith/asc_div.md) | 按元素求商。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_exp](vector_arith/asc_exp.md) | 按元素取自然指数。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_log](vector_arith/asc_log.md) | 按元素取自然对数。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_max](vector_arith/asc_max.md) | 按元素求最大值。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_max_scalar](vector_arith/asc_max_scalar.md) | 矢量内每个元素与标量求最大值。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_min](vector_arith/asc_min.md) | 按元素求最小值。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_min_scalar](vector_arith/asc_min_scalar.md) | 源操作数矢量逐元素与标量相比，取较小值。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_mul](vector_arith/asc_mul.md) | 按元素求积。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_mul_scalar](vector_arith/asc_mul_scalar.md) | 矢量内每个元素与标量求积。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_rcp](vector_arith/asc_rcp.md) | 执行矢量的取倒数运算。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_rsqrt](vector_arith/asc_rsqrt.md) | 按元素进行开方后取倒数的计算。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_sqrt](vector_arith/asc_sqrt.md) | 对元素进行开方。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_sub](vector_arith/asc_sub.md) | 按元素求差。 | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_sub_scalar](vector_arith/asc_sub_scalar.md) | 矢量内每个元素与标量求差。 | `c_api/vector_compute/compute/vector_arith.h` |

### 逻辑计算

**表2** 逻辑计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_and](vector_logic/asc_and.md) | 执行矢量与运算。 | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_not](vector_logic/asc_not.md) | 对源操作数逐元素执行按位取反。 | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_or](vector_logic/asc_or.md) | 每对元素按位或运算。 | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_shiftleft](vector_logic/asc_shiftleft.md) | 将所有元素左移distance位。 | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_shiftright](vector_logic/asc_shiftright.md) | 对源操作数中的每个元素执行右移。 | `c_api/vector_compute/compute/vector_logic.h` |

### 归约计算

**表3** 归约计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_datablock_reduce_max](vector_reduce/asc_datablock_reduce_max.md) | 对每个DataBlock内所有元素求最大值。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_datablock_reduce_min](vector_reduce/asc_datablock_reduce_min.md) | 对每个DataBlock内所有元素求最小值。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_datablock_reduce_sum](vector_reduce/asc_datablock_reduce_sum.md) | 对每个DataBlock内所有元素求和。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_get_reduce_max_cnt](vector_reduce/asc_get_reduce_max_cnt.md) | 获取执行`asc_repeat_reduce_max`操作后的最大值，以及第一个最大值时的索引。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_get_reduce_min_cnt](vector_reduce/asc_get_reduce_min_cnt.md) | 获取执行`asc_repeat_reduce_min`操作后的最小值，以及第一个最小值时的索引。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_get_rsvd_count](vector_reduce/asc_get_rsvd_count.md) | 获取`asc_reduce`操作后保留的元素数量。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_pair_reduce_sum](vector_reduce/asc_pair_reduce_sum.md) | 将相邻两个元素成对求和，依次写入目的操作数。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_reduce](vector_reduce/asc_reduce.md) | 以内置固定模式对应的二进制或者用户自定义输入的数值对应的gather `mask`（数据收集的掩码），从源操作数中选取元素写入目的操作数中。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_repeat_reduce_max](vector_reduce/asc_repeat_reduce_max.md) | 对每个Repeat内所有元素求最大值。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_repeat_reduce_min](vector_reduce/asc_repeat_reduce_min.md) | 对每个Repeat内所有元素求最小值。 | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_repeat_reduce_sum](vector_reduce/asc_repeat_reduce_sum.md) | 对每个Repeat内所有元素求和。 | `c_api/vector_compute/compute/vector_reduce.h` |

### 广播操作

**表4** 广播操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_brcb](vector_broadcast/asc_brcb.md) | 将源操作数中的每一个数填充到目的操作数的一个DataBlock中。 | `c_api/vector_compute/compute/vector_broadcast.h` |
| [asc_duplicate](vector_broadcast/asc_duplicate.md) | 将一个变量或立即数填充到一个矢量中。 | `c_api/vector_compute/compute/vector_broadcast.h` |

### 复合计算

**表5** 复合计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_add_relu](vector_fused/asc_add_relu.md) | 逐元素求和后执行ReLU计算，并支持转换结果的数据类型。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_axpy](vector_fused/asc_axpy.md) | 源操作数`src`中每个元素与标量`value`求积后和目的操作数`dst`中的对应元素相加。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_deq_int162b8](vector_fused/asc_deq_int162b8.md) | 将`int16_t`类型转换为`int8_t`或`uint8_t`类型，并将数据存放在每个DataBlock的上半块或下半块。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_deq_int322half](vector_fused/asc_deq_int322half.md) | 对输入的`int32_t`类型的数据按元素做量化并转换为`half`类型。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_fma](vector_fused/asc_fma.md) | 按元素将`src0`和`src1`相乘并和`dst`相加，将最终结果存放进`dst`中。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_leakyrelu](vector_fused/asc_leakyrelu.md) | 执行矢量Leaky Relu运算。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_add](vector_fused/asc_mul_add.md) | 执行矢量的乘加运算。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_add_relu](vector_fused/asc_mul_add_relu.md) | 按元素将`src0`和`dst`相乘并加上`src1`，再进行Relu计算（结果和0对比取较大值），最终结果存放进`dst`中。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_cast_half2int8](vector_fused/asc_mul_cast_half2int8.md) | 按元素求积，并将结果转换为`int8_t`类型。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_cast_half2uint8](vector_fused/asc_mul_cast_half2uint8.md) | 按元素求积，并将结果转换为`uint8_t`类型。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_relu](vector_fused/asc_relu.md) | 按元素做线性整流Relu。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_set_deq_scale](vector_fused/asc_set_deq_scale.md) | 设置DEQSCALAR寄存器的值。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_sub_relu](vector_fused/asc_sub_relu.md) | 逐元素求差后执行ReLU计算，并支持转换结果的数据类型。 | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_vdeq_int162b8](vector_fused/asc_vdeq_int162b8.md) | 将`int16_t`类型转换为`int8_t`或`uint8_t`类型，并将数据存放在每个DataBlock的上半块或下半块。 | `c_api/vector_compute/compute/vector_fused.h` |

### 比较计算

**表6** 比较计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_eq](vector_compare/asc_eq.md) | 比较`src0`与`src1`在对应索引位置的元素大小。若比较结果为真，则输出结果的对应比特位设为1，否则设为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_eq_scalar](vector_compare/asc_eq_scalar.md) | 执行矢量与标量的比较运算，如果值相等则输出1，否则输出0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ge](vector_compare/asc_ge.md) | Ge（greater than or equal to），逐元素比较`src0` >= `src1`是否成立，成立则输出结果为1，否则输出结果为0，每个元素的比较结果占一个bit。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ge_scalar](vector_compare/asc_ge_scalar.md) | 按元素判断`src` >= `value`是否成立，若成立则输出结果为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_get_cmp_mask](vector_compare/asc_get_cmp_mask.md) | 获取Compare操作的比较结果。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_gt](vector_compare/asc_gt.md) | 按元素比较两个矢量的大小关系，若比较后的结果为真，则输出结果的对应比特位为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_gt_scalar](vector_compare/asc_gt_scalar.md) | `src`中的每个元素逐个与标量`value`比较大小，如果某个位置上的元素大于`value`，则输出结果`dst`上的对应比特位为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_le](vector_compare/asc_le.md) | 按元素判断`src0` <= `src1`是否成立，若成立则输出结果为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_le_scalar](vector_compare/asc_le_scalar.md) | 按元素判断`src` <= `value`是否成立，若成立则输出结果为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_lt](vector_compare/asc_lt.md) | 按元素判断`src0` < `src1`是否成立，若成立则输出结果上的对应比特位为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_lt_scalar](vector_compare/asc_lt_scalar.md) | 执行矢量中每个位置和标量比较，如果值小于标量值则为1，否则为0，结果为每个bit位按小端序排布。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ne](vector_compare/asc_ne.md) | 按元素判断`src0` != `src1`是否成立，若成立则输出结果为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ne_scalar](vector_compare/asc_ne_scalar.md) | 按元素判断是否不等于输入标量，若成立则输出结果上的对应比特位为1，否则为0。 | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_set_cmp_mask](vector_compare/asc_set_cmp_mask.md) | 为Select操作设置用于选择的掩码。 | `c_api/vector_compute/compute/vector_compare.h` |

### 排布变换

**表7** 排布变换接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_select](vector_permute_sel/asc_select.md) | 根据掩码，从两个源操作数中选取元素，输出到目的操作数。 | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_set_va_reg](vector_permute_sel/asc_set_va_reg.md) | 用于设置transpose的地址，将操作数地址序列与地址寄存器关联。 | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_squeeze](vector_permute_sel/asc_squeeze.md) | 通过比较掩码`src1`，将`src0`中的向量压缩成更短的向量，存储到`dst`中。 | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_transpose](vector_permute_sel/asc_transpose.md) | 对16×16的二维矩阵数据块执行转置。 | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_transto5hd](vector_permute_sel/asc_transto5hd.md) | 数据格式转换，一般用于将NCHW格式转换成NC1HWC0格式。 | `c_api/vector_compute/compute/vector_permute_sel.h` |

### 排序计算

**表8** 排序计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_bitsort](vector_sort/asc_bitsort.md) | Score和Index分别存储在`src0`和`src1`中，按Score进行排序（Score大的元素排前面），排序后的Score与其对应的Index一起以（Score，Index）的结构存储在`dst`中。 | `c_api/vector_compute/compute/vector_sort.h` |
| [asc_get_vms4_sr](vector_sort/asc_get_vms4_sr.md) | 获取`asc_mrgsort4`操作后各队列已处理的Region Proposal个数。 | `c_api/vector_compute/compute/vector_sort.h` |
| [asc_mrgsort4](vector_sort/asc_mrgsort4.md) | 将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。 | `c_api/vector_compute/compute/vector_sort.h` |

### 聚合操作

**表9** 聚合操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_gather](vector_gather/asc_gather.md) | 将源操作数按照给定的偏移按元素收集到目的操作数中。 | `c_api/vector_compute/compute/vector_gather.h` |
| [asc_gather_datablock](vector_gather/asc_gather_datablock.md) | 根据偏移地址按照DataBlock的粒度将源操作数收集到目的操作数中。 | `c_api/vector_compute/compute/vector_gather.h` |

### 类型转换

**表10** 类型转换接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_bfloat162float](vector_convert/asc_bfloat162float.md) | 数据类型转换。将`bfloat16_t`类型的数据转换为`float`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_bfloat162int32](vector_convert/asc_bfloat162int32.md) | 数据类型转换。将`bfloat16_t`类型的数据转换为`int32_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2bfloat16](vector_convert/asc_float2bfloat16.md) | 数据类型转换。将`float`类型的数据转换为`bfloat16_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2float](vector_convert/asc_float2float.md) | 数据类型转换。将`float`类型的数据转换为`float`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2half](vector_convert/asc_float2half.md) | 数据类型转换。将`float`类型的数据转换为`half`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2int16](vector_convert/asc_float2int16.md) | 将`float`类型数据转换为`int16_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2int32](vector_convert/asc_float2int32.md) | 将`float`类型数据转换为`int32_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2int64](vector_convert/asc_float2int64.md) | 将`float`类型数据转换为`int64_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2float](vector_convert/asc_half2float.md) | 数据类型转换。将`half`类型的数据转换为`float`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int16](vector_convert/asc_half2int16.md) | 数据类型转换。将`half`类型的数据转换为`int16_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int32](vector_convert/asc_half2int32.md) | 数据类型转换。将`half`类型的数据转换为`int32_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int4](vector_convert/asc_half2int4.md) | 数据类型转换。将`half`类型的数据转换为`int4b_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int8](vector_convert/asc_half2int8.md) | 数据类型转换。将`half`类型的数据转换为`int8_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2uint8](vector_convert/asc_half2uint8.md) | 将`half`类型数据转换为`uint8_t`类型，支持多种舍入模式。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int162float](vector_convert/asc_int162float.md) | 数据类型转换。将`int16_t`类型的数据转换为`float`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int162half](vector_convert/asc_int162half.md) | 数据类型转换。将`int16_t`类型的数据转换为`half`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int322float](vector_convert/asc_int322float.md) | 将`int32_t`类型数据转换为`float`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int322int16](vector_convert/asc_int322int16.md) | 数据类型转换。将`int32_t`类型的数据转换为`int16_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int322int64](vector_convert/asc_int322int64.md) | 数据类型转换。将`int32_t`类型的数据转换为`int64_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int42half](vector_convert/asc_int42half.md) | 数据类型转换。将`int4b_t`类型的数据转换为`half`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int642float](vector_convert/asc_int642float.md) | 将`int64_t`类型数据转换为`float`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int642int32](vector_convert/asc_int642int32.md) | 数据类型转换。将`int64_t`类型的数据转换为`int32_t`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int82half](vector_convert/asc_int82half.md) | 数据类型转换。将`int8_t`类型的数据转换为`half`类型。 | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_uint82half](vector_convert/asc_uint82half.md) | 数据类型转换。将`uint8_t`类型的数据转换为`half`类型。 | `c_api/vector_compute/compute/vector_convert.h` |

### 掩码操作

**表11** 掩码操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_set_mask_count](vector_mask_config/asc_set_mask_count.md) | 设置Mask模式为Counter模式。 | `c_api/vector_compute/compute/vector_mask_config.h` |
| [asc_set_mask_norm](vector_mask_config/asc_set_mask_norm.md) | 设置Mask模式为Normal模式。 | `c_api/vector_compute/compute/vector_mask_config.h` |
| [asc_set_vector_mask](vector_mask_config/asc_set_vector_mask.md) | 设置Mask值。 | `c_api/vector_compute/compute/vector_mask_config.h` |
