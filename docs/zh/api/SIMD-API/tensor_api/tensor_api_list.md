# Tensor API列表

## 基础数据结构

**表1**  基础数据结构接口列表

| 类型或接口名 | 功能描述 |
| --- | --- |
| [layout](layout/layout.md) | 使用Shape和Stride描述数据的逻辑形状及其内存布局。 |
| [Shape](layout/shape.md) | 描述Tensor各维度的逻辑形状，支持普通整数和嵌套元组形式。 |
| [Stride](layout/stride.md) | 描述Tensor各维度相邻元素在内存中的地址步长，支持普通整数和嵌套元组形式。 |
| [Coord](layout/coord.md) | 描述Tensor在各维度上的逻辑坐标。 |
| [global_tensor和local_tensor](tensor/tensor.md) | 分别描述Global Memory和片上Local Memory中的Tensor对象。 |
| [Pointer](tensor/pointer.md) | 描述Tensor的访问地址及其对应的存储位置。 |
| [make_layout](layout/make_layout.md) | 根据Shape和Stride构造普通Layout。 |
| [make_pattern_layout](layout/make_pattern_layout.md) | 根据Layout Pattern、Shape和Stride构造带有排布类型信息的Layout。 |
| [make_frame_layout](layout/make_frame_layout.md) | 根据卷积特征图或矩阵分形的Layout Pattern和形状参数构造Layout。 |
| [make_shape](layout/make_shape.md) | 构造Shape对象。 |
| [make_stride](layout/make_stride.md) | 构造Stride对象。 |
| [make_coord](layout/make_coord.md) | 构造Coord对象。 |
| [make_mem_ptr](tensor/make_mem_ptr.md) | 根据指针和存储位置构造Tensor API内存指针对象。 |
| [make_tensor](tensor/make_tensor.md) | 将内存指针和Layout绑定，构造global_tensor或local_tensor。 |
| [slice](tensor/slice.md) | 根据起始坐标和Shape或Layout描述创建原Tensor的子Tensor。 |

## Layout操作

**表2**  Layout操作接口列表

| 接口名 | 功能描述 |
| --- | --- |
| [capacity](layout/capacity.md) | 获取Layout或指定子维度对应的实际内存容量。 |
| [coshape](layout/coshape.md) | 在编译期获取Layout指定子维度的Shape结构。 |
| [cosize](layout/cosize.md) | 在编译期获取Layout指定子维度的容量或元素数量。 |
| [crd2idx](layout/crd2idx.md) | 将多维坐标按照Layout转换为线性内存索引。 |
| [get](layout/get.md) | 获取元组中的指定元素，支持单个或多个编译期索引。 |
| [get_shape](layout/get_shape.md) | 获取Layout的Shape或指定子维度的Shape。 |
| [get_stride](layout/get_stride.md) | 获取Layout的Stride或指定子维度的Stride。 |
| [rank](layout/rank.md) | 获取Layout的层级或维度信息。 |
| [select](layout/select.md) | 按指定索引从Layout的Shape和Stride中选择子结构。 |
| [size](layout/size.md) | 获取Layout的逻辑元素数量。 |
| [squeeze](layout/squeeze.md) | 删除Shape顶层结构中指定的大小为1的维度。 |

## Algorithm

**表3**  Algorithm接口和类型列表

| 接口或类型名 | 功能描述 |
| --- | --- |
| [Operation、Trait和Atom](algorithm.md) | 介绍数据搬运和矩阵计算中Operation、Trait、Params和Atom的组合方式，以及`copy`和`mmad`统一调用入口。 |

## 数据搬运

**表4**  数据搬运通路和接口列表

| 数据通路或接口名 | 功能描述 |
| --- | --- |
| [Global Memory到Unified Buffer](reg_vector_compute/vector_compute_load/copy_gm_to_ub.md) | 使用`copy`将Global Memory中的数据搬运到Unified Buffer，支持坐标区域搬运。 |
| [Unified Buffer到Global Memory](reg_vector_compute/vector_compute_store/copy_ub_to_gm.md) | 使用`copy`将Unified Buffer中的数据搬运到Global Memory，支持坐标区域搬运。 |
| [Global Memory到L1 Buffer](matrix_compute/cube_compute_load/copy_gm_to_l1.md) | 使用`copy`将Global Memory中的数据搬运到L1 Buffer，并支持多种矩阵格式转换和Batch搬运。 |
| [Unified Buffer到L1 Buffer](reg_vector_compute/vector_compute_load/copy_ub_to_l1.md) | 使用`copy`将Unified Buffer中的数据搬运到L1 Buffer。 |
| [L1 Buffer到Unified Buffer](matrix_compute/cube_compute_load/copy_l1_to_ub.md) | 使用`copy`将L1 Buffer中的数据搬运到Unified Buffer。 |
| [L1 Buffer到L0A Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0a.md) | 使用`copy`将L1 Buffer中的左矩阵数据搬运到L0A Buffer，支持矩阵格式转换和卷积特征图搬运。 |
| [L1 Buffer到L0B Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0b.md) | 使用`copy`将L1 Buffer中的右矩阵数据搬运到L0B Buffer，支持矩阵格式转换和Batch搬运。 |
| [L1 Buffer到L0ScaleA Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0scalea.md) | 使用`copy`将L1 Buffer中的ScaleA数据搬运到L0ScaleA Buffer。 |
| [L1 Buffer到L0ScaleB Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0scaleb.md) | 使用`copy`将L1 Buffer中的ScaleB数据搬运到L0ScaleB Buffer。 |
| [L1 Buffer到BiasTable Buffer](matrix_compute/cube_compute_load/copy_l1_to_biastable.md) | 使用`copy`将L1 Buffer中的Bias数据搬运到BiasTable Buffer。 |
| [L1 Buffer到Fixpipe Buffer](matrix_compute/cube_compute_load/copy_l1_to_fixbuf.md) | 使用`copy`将L1 Buffer中的量化参数搬运到Fixpipe Buffer。 |
| [L0C Buffer到Global Memory](matrix_compute/cube_compute_store/copy_l0c_to_gm.md) | 使用`copy`将L0C Buffer中的矩阵结果搬运到Global Memory，支持格式转换和量化输出。 |
| [L0C Buffer到Unified Buffer](matrix_compute/cube_compute_store/copy_l0c_to_ub.md) | 使用`copy`将L0C Buffer中的矩阵结果搬运到Unified Buffer，支持格式转换和量化输出。 |
| [L0C Buffer到L1 Buffer](matrix_compute/cube_compute_store/copy_l0c_to_l1.md) | 使用`copy`将L0C Buffer中的矩阵结果搬运到L1 Buffer。 |
| [Unified Buffer内部搬运](reg_vector_compute/vector_compute_load/copy_ub_to_ub.md) | 使用`copy`完成Unified Buffer内部Tensor之间的数据搬运。 |

## 矩阵计算

**表5**  矩阵计算接口和特性列表

| 接口或主题 | 功能描述 |
| --- | --- |
| [矩阵计算概述](matrix_compute/overview.md) | 介绍Tensor API矩阵计算的基本流程和接口组成。 |
| [矩阵计算分形介绍](matrix_compute/cube_compute_fractal_intro.md) | 介绍矩阵计算中常用的数据分形格式及其Layout表达方式。 |
| [mmad](matrix_compute/mmad_compute/mmad.md) | 使用L0A、L0B和可选Bias数据执行矩阵乘加计算，并将结果写入L0C。 |
| [mmad计算关键特性](matrix_compute/mmad_compute_key_features/mmad_compute_key_features.md) | 介绍GEMV、HF32和unit_flag等矩阵乘加关键特性。 |
| [GEMV](matrix_compute/mmad_compute_key_features/gemv.md) | 介绍M为1场景下的GEMV计算模式。 |
| [HF32](matrix_compute/mmad_compute_key_features/hf32.md) | 介绍矩阵计算中的HF32精度模式。 |
| [unit_flag](matrix_compute/mmad_compute_key_features/unit_flag.md) | 介绍mmad与后续矩阵数据搬出之间的细粒度并行控制。 |
| [mmad计算辅助配置](matrix_compute/mmad_compute_aux_config.md) | 列出mmad计算所需的C API辅助配置接口。 |
| [矩阵计算搬入总体说明](matrix_compute/cube_compute_load/overall_description.md) | 说明矩阵数据从Global Memory或L1 Buffer搬入各级矩阵计算Buffer的通路。 |
| [矩阵计算搬出总体说明](matrix_compute/cube_compute_store/overall_description.md) | 说明矩阵结果从L0C Buffer搬出到Global Memory、Unified Buffer或L1 Buffer的通路。 |
| [矩阵搬出关键特性](matrix_compute/cube_store_key_features/cube_store_key_features.md) | 介绍随路量化、随路ReLU、通道拆分、通道合并、Batch搬运和双目标模式。 |
| [随路量化](matrix_compute/cube_store_key_features/quant_pre.md) | 介绍矩阵结果搬出过程中的随路量化能力。 |
| [随路ReLU](matrix_compute/cube_store_key_features/relu_pre.md) | 介绍矩阵结果搬出过程中的随路ReLU能力。 |
| [F32 Channel Split](matrix_compute/cube_store_key_features/f32_channel_split.md) | 介绍F32结果搬出时的通道拆分能力。 |
| [Int8 Channel Merge](matrix_compute/cube_store_key_features/int8_channel_merge.md) | 介绍Int8结果搬出时的通道合并能力。 |
| [Batch搬运](matrix_compute/cube_store_key_features/batch_copy.md) | 介绍矩阵结果的多Batch搬出能力。 |
| [L0C到Unified Buffer双目标模式](matrix_compute/cube_store_key_features/l0c_to_ub_dual_dst.md) | 介绍L0C到Unified Buffer的双目标搬出模式。 |

## 矢量计算接口

**表6**  矢量计算接口列表

| 接口或类型名 | 功能描述 |
| --- | --- |
| [reg_tensor](reg_vector_compute/reg_tensor/reg_tensor.md) | 封装矢量数据寄存器及其掩码寄存器，用于寄存器数据计算。 |
| [reg_pair](reg_vector_compute/reg_tensor/reg_pair.md) | 保存两个元素类型相同的`reg_tensor`，用于具有双结果的寄存器接口。 |
| [all_mask](reg_vector_compute/mask_reg_compute/all_mask.md) | 创建与指定元素类型位宽匹配的全有效Mask寄存器。 |
| [make_mask](reg_vector_compute/mask_reg_compute/make_mask.md) | 根据Mask模式和数据元素宽度创建Mask寄存器。 |
| [none_mask](reg_vector_compute/mask_reg_compute/none_mask.md) | 创建与指定元素类型位宽匹配的全无效Mask寄存器。 |
| [update_mask](reg_vector_compute/mask_reg_compute/update_mask.md) | 根据剩余待处理元素数生成有效位Mask，并更新剩余元素数量。 |
| [deinterleave（掩码寄存器解交织）](reg_vector_compute/mask_reg_compute/deinterleave.md) | 将两个Mask寄存器按指定元素宽度解交织，返回偶数组和奇数组结果。 |
| [load](reg_vector_compute/reg_data_load/load.md) | 将Unified Buffer中的数据搬入`reg_tensor`，支持多种数据排列方式。 |
| [load_broadcast](reg_vector_compute/reg_data_load/load_broadcast.md) | 从Unified Buffer搬入数据，并按指定模式广播到`reg_tensor`。 |
| [store](reg_vector_compute/reg_data_store/store.md) | 将`reg_tensor`中的数据搬出到Unified Buffer，支持多种数据排列方式。 |
| [cast](reg_vector_compute/type_conversion/cast.md) | 转换寄存器数据类型，支持配置数据排布、舍入和饱和模式。 |
| [trunc](reg_vector_compute/type_conversion/trunc.md) | 将Mask选中的浮点元素向零取整，并保留原数据类型。 |
| [log](reg_vector_compute/basic_arithmetic/log.md) | 对源操作数中的有效元素逐元素计算自然对数。 |
| [max](reg_vector_compute/basic_arithmetic/max.md) | 对两个源操作数逐元素计算最大值。 |
| [operator+](reg_vector_compute/basic_arithmetic/operator_add.md) | 对两个源操作数逐元素执行加法计算。 |
| [operator-](reg_vector_compute/basic_arithmetic/operator_sub.md) | 对两个源操作数逐元素执行减法计算。 |
| [operator*](reg_vector_compute/basic_arithmetic/operator_mul.md) | 对两个源操作数逐元素执行乘法计算。 |
| [or](reg_vector_compute/logical_compute/or.md) | 对两个源操作数逐元素执行按位或计算。 |
| [选择与比较](reg_vector_compute/compare_and_select/compare_and_select.md) | 根据mask的比特位值，从源操作数src0、src1中选择元素，得到目的操作数。 |
| [select](reg_vector_compute/compare_and_select/select.md) | 根据条件Mask从两个源寄存器中逐元素选择数据。 |
| [数据填充](reg_vector_compute/data_padding/data_padding.md) | 根据mask将源操作数src的最低位元素或者一个scalar操作数填充到目的操作数。 |
| [fill](reg_vector_compute/data_padding/fill.md) | 将标量或源寄存器最低位元素广播并填充到目的寄存器。 |
| [数据重排](reg_vector_compute/data_reorder/data_reorder.md) | 给定源操作数src0和src1，将src0和src1中的元素解交织存入结果操作数。 |
| [deinterleave（数据寄存器解交织）](reg_vector_compute/data_reorder/deinterleave.md) | 将两个源寄存器中的元素解交织到两个结果寄存器。 |

## 工具接口

**表7**  工具接口列表

| 接口或类型名 | 功能描述 |
| --- | --- |
| [编译期整型别名](utils/compile_time_integer_aliases.md) | 提供用于表达编译期整数值的类型别名。 |
| [维度保留标记](utils/dimension_keep_mark.md) | 提供用于标记维度保留行为的类型和常量。 |
| [get_layout_pattern](utils/get_layout_pattern.md) | 从Layout类型中提取Layout Pattern类型信息。 |
| [类型判断与提取工具](utils/type_traits.md) | 提供Layout、Tensor、Coord、Shape、量化参数等类型判断工具。 |

## 废弃接口

**表8**  废弃接口列表

| 接口名 | 功能描述 |
| --- | --- |
| [废弃接口说明](deprecated_interface/deprecated_interface.md) | 汇总Tensor API中用于兼容旧版大驼峰命名的废弃接口。 |
| [layout::Capacity](deprecated_interface/layout_capacity_deprecated.md) | 旧版Layout容量查询接口。 |
| [layout::Shape](deprecated_interface/layout_shape_deprecated.md) | 旧版Layout Shape查询接口。 |
| [layout::Stride](deprecated_interface/layout_stride_deprecated.md) | 旧版Layout Stride查询接口。 |
| [layout::Rank](deprecated_interface/layout_rank_deprecated.md) | 旧版Layout层级查询接口。 |
| [layout::Size](deprecated_interface/layout_size_deprecated.md) | 旧版Layout逻辑元素数量查询接口。 |
| [layout::Get](deprecated_interface/layout_get_deprecated.md) | 旧版Layout元素获取接口。 |
| [base_tensor::Tensor](deprecated_interface/tensor_tensor_deprecated.md) | 旧版Tensor对象访问接口。 |
| [base_tensor::Engine](deprecated_interface/tensor_engine_deprecated.md) | 旧版Tensor Engine访问接口。 |
| [base_tensor::Layout](deprecated_interface/tensor_layout_deprecated.md) | 旧版Tensor Layout访问接口。 |
| [base_tensor::Data](deprecated_interface/tensor_data_deprecated.md) | 旧版Tensor数据地址访问接口。 |
| [base_tensor::Shape](deprecated_interface/tensor_shape_deprecated.md) | 旧版Tensor Shape访问接口。 |
| [base_tensor::Stride](deprecated_interface/tensor_stride_deprecated.md) | 旧版Tensor Stride访问接口。 |
| [base_tensor::Size](deprecated_interface/tensor_size_deprecated.md) | 旧版Tensor逻辑元素数量查询接口。 |
| [base_tensor::Capacity](deprecated_interface/tensor_capacity_deprecated.md) | 旧版Tensor容量查询接口。 |
| [base_tensor::Slice](deprecated_interface/tensor_slice_deprecated.md) | 旧版Tensor切片接口。 |
| [global_tensor::SetL2CacheHint](deprecated_interface/set_l2_cache_hint_deprecated.md) | 旧版Global Tensor L2 Cache配置接口。 |
