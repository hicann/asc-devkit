# 随路ReLU

## 特性说明

矩阵计算的搬出过程中支持随路ReLU能力，当前支持如下随路ReLU能力。

| `asc_relu_pre_mode`枚举值 | 编码 | 作用及配置 |
| --- | --- | --- |
| `NONE` | 0 | 不启用激活 |
| `NORMAL` | 1 | Normal ReLU，负半轴系数为0 |
| `SCALAR` | 2 | 全矩阵共享激活系数，使用[asc_set_l0c_copy_relu_alpha](../asc_set_l0c_copy_relu_alpha.md)配置 |
| `VECTOR` | 3 | 每个N通道独立激活系数，先将系数搬入，再用[asc_set_l0c_copy_config](../asc_set_l0c_copy_config.md)配置`relu_pre_addr` |

与随路量化组合使用的详细信息请参考[随路量化与随路ReLU场景组合](accompanying_quantization_and_relu_scenario_combination.md)。
