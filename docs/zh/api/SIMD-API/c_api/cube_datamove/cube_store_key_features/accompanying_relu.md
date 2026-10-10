# 随路ReLU

## 特性说明

矩阵计算的搬出过程中支持随路ReLU能力，当前支持如下随路ReLU能力。

随路激活在结果搬出时对数值进行处理，C API入口为[asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md)、[asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1.md)。
<!-- npu="950" id2 -->
Ascend 950PR&950DT系列产品还包括[asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md)；下表使用Ascend 950PR&950DT系列产品的[asc_relu_pre_mode](../../defs/enum/asc_relu_pre_mode.md)名称。
<!-- end id2 -->

<!-- npu="A3,910b" id3 -->
Atlas A3系列产品和Atlas A2系列产品的`relu_pre`使用整数参数，需按Atlas A3系列产品和Atlas A2系列产品对应接口的支持范围配置，不能直接套用Ascend 950PR&950DT系列产品专用的配置重载。
<!-- end id3 -->

<!-- npu="950" id4 -->
下表的`asc_relu_pre_mode`枚举及其配置接口适用于Ascend 950PR&950DT系列产品。

| `asc_relu_pre_mode`枚举值 | 编码 | 作用及配置 |
| --- | --- | --- |
| `NONE` | 0 | 不启用激活 |
| `NORMAL` | 1 | Normal ReLU，负半轴系数为0 |
| `SCALAR` | 2 | 全矩阵共享激活系数，使用[asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md)配置 |
| `VECTOR` | 3 | 每个N通道独立激活系数，先将系数搬入，再用[asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md)配置`relu_pre_addr` |
<!-- end id4 -->

与随路量化组合使用的详细信息请参考[随路量化与随路ReLU场景组合](accompanying_quantization_and_relu_scenario_combination.md)。

Vector激活参数需通过[asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb.md)搬入Fixpipe Buffer的激活参数区，再由[asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md)配置`relu_pre_addr`。激活区与量化区独立，配置地址的单位及容量限制参见对应接口。

<!-- npu="950" id1 -->
Ascend 950PR&950DT系列产品搬出接口还提供`enable_clip_relu_pre`，其使用需结合Normal ReLU和量化模式，不能只设置一个开关而省略配套条件。具体支持组合及参数要求见[asc_copy_l0c2gm（Ascend 950PR&950DT系列产品）](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md)的参数和约束说明，相关Scalar配置接口见[asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md)和[asc_set_l0c_copy_lrelu_alpha](../cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md)。
<!-- end id1 -->

Normal ReLU的完整搬出调用可参考[asc_copy_l0c2gm样例](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/README.md)场景6。
