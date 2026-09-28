# L1与L0数据搬运

L1到L0A/L0B用于为Cube矩阵计算装载左、右矩阵，由PIPE_MTE1执行；矩阵计算结果位于L0C，可由PIPE_FIX搬到L1。NPU架构版本3510的通路具有方向性，不提供L0A/L0B到L1或L1到L0C的通用复制。

## 总体说明

| 方向 | 搬运模式 | 接口 |
| --- | --- | --- |
| L1到L0A | 二维分形搬运 | [asc_copy_l12l0a](../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) |
| L1到L0A | 二维分形伴转置搬运 | [asc_copy_l12l0a_transpose](../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) |
| L1到L0A | 三维img2col搬运 | [asc_copy_l12l0a](../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md) |
| L1到L0A_MX | MX左矩阵量化系数搬运 | [asc_copy_l12l0a_mx](../cube_datamove/asc_copy_l12l0a_mx.md) |
| L1到L0B | 二维分形搬运 | [asc_copy_l12l0b](../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) |
| L1到L0B | 二维分形伴转置搬运 | [asc_copy_l12l0b_transpose](../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) |
| L1到L0B | 三维img2col搬运 | [asc_copy_l12l0b](../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md) |
| L1到L0B | 按Repeat和分形间隔执行二维分形转置 | [asc_copy_l12l0b_trans](../cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md) |
| L1到L0B_MX | MX右矩阵量化系数搬运 | [asc_copy_l12l0b_mx](../cube_datamove/asc_copy_l12l0b_mx.md) |
| L0C到L1 | 搬出矩阵结果，可组合量化、激活和格式转换 | [asc_copy_l0c2l1](../cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) |

## L1到L0A二维分形矩阵搬运

[asc_copy_l12l0a](../cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)按源矩阵行列起始位置、搬运分形数和源/目的分形步长，把L1中的二维矩阵装载到L0A。`asc_copy_l12l0a_transpose`使用相同参数形式，在搬运时对分形矩阵执行转置。

```c
__aicore__ inline void asc_copy_l12l0a(__ca__ <dtype>* dst,
                                       __cbuf__ <dtype>* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

- `src`需要32字节对齐，`dst`需要512字节对齐。
- `m_start_position`和`m_step`以16个元素为单位；`k_start_position`以32字节为单位。
- `src_stride`和`dst_stride`均以512字节分形为单位。
- 不同数据位宽的L0A分形形状不同：b4为16×64，b8为16×32，b16为16×16，b32为16×8；每个分形均占512字节。
- `m_step`或`k_step`为0时不执行搬运。转置模式还需满足不同数据位宽对应的步长倍数约束。

## L1到L0B二维分形矩阵搬运

[asc_copy_l12l0b](../cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)和`asc_copy_l12l0b_transpose`的参数语义与L0A二维搬运一致，但目的为L0B。不同数据位宽的L0B分形形状为：b4的64×16、b8的32×16、b16的16×16和b32的8×16。

二维搬运最小粒度为一个512字节完整分形。矩阵边界不足一个分形时，需要在L1中准备完整分形并按矩阵计算要求填充无效元素。L0B目的范围不得超过64KB。

## L1到L0A三维img2col搬运

L0A三维模式将L1中的NC1HWC0 Feature Map执行Image to Column展开，并按目的矩阵起点和范围选取分形写入L0A。主要参数包括：

- `k_extension`、`m_extension`：目的矩阵width、height方向的传输长度。
- `k_start_pt`、`m_start_pt`：目的矩阵width、height方向的起点。
- `stride_w`、`stride_h`：卷积核滑动步长。
- `filter_w`、`filter_h`和膨胀系数：描述卷积窗口。
- `channel_size`：源Feature Map通道数。

调用主接口前需要完成以下配置：

| 配置内容 | 接口 |
| --- | --- |
| Feature Map宽、高及四周Padding | [asc_set_l13d_fmatrix](../cube_datamove/asc_set_l13d_fmatrix.md) |
| Padding值与填充模式 | [asc_set_l12l0a_3d_padding](../cube_datamove/asc_set_l12l0a_3d_padding.md) |
| 目的步长、Repeat方向、次数和步长 | [asc_set_l13d_rpt](../cube_datamove/asc_set_l13d_rpt.md) |

`stride_w`和`stride_h`的有效范围为[1, 63]。传输长度、卷积核尺寸或通道数使接口成为空操作的条件，以及各数据位宽的通道余数约束，请以接口文档为准。

## L1到L0B三维img2col搬运

L0B三维模式也执行img2col展开，但写入L0B时硬件自动转置，主接口中的`transpose`参数不改变该行为。调用前分别通过以下接口配置右矩阵对应的寄存器组：

- [asc_set_l13d_fmatrix_b](../cube_datamove/asc_set_l13d_fmatrix_b.md)：Feature Map属性。
- [asc_set_l12l0b_3d_padding](../cube_datamove/asc_set_l12l0b_3d_padding.md)：Padding值与模式。
- [asc_set_l13d_rpt_b](../cube_datamove/asc_set_l13d_rpt_b.md)：目的步长和Repeat参数。

L0A和L0B三维接口也可用于普通二维矩阵装载，但地址、传输范围和配置寄存器仍应按三维接口规则计算。

## L1到L0B二维分形转置搬运

[asc_copy_l12l0b_trans](../cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md)按`repeat`次数处理一个或多个512字节分形，并通过`src_stride`、`dst_gap`、`src_frac_gap`和`dst_frac_gap`控制Repeat之间及同一Repeat内的分形排布。

```c
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ <dtype>* dst,
                                             __cbuf__ <dtype>* src,
                                             uint16_t index_id,
                                             uint8_t repeat,
                                             uint16_t src_stride,
                                             uint16_t dst_gap,
                                             uint16_t dst_frac_gap,
                                             uint16_t src_frac_gap)
```

- `src`需要32字节对齐，`dst`需要512字节对齐。
- 所有步长和间隔参数均以512字节分形为单位；`dst_gap`、`dst_frac_gap`和`src_frac_gap`的硬件生效步长为传入值加1。
- `index_id`指定源矩阵的起始分形序号；每次Repeat处理的分形个数和方块矩阵形状随数据位宽变化，应按接口文档为对应数据类型计算。
- 目的分形不能发生重叠，`repeat`为0时不执行搬运。

## MX量化系数搬运

MX矩阵计算需要把主矩阵和对应量化系数分别装载到L0A/L0A_MX或L0B/L0B_MX。量化系数搬运接口包括：

- [asc_copy_l12l0a_mx](../cube_datamove/asc_copy_l12l0a_mx.md)：量化系数分形固定为16×2，对应L0A中一个16×32数据分形的地址映射。
- [asc_copy_l12l0b_mx](../cube_datamove/asc_copy_l12l0b_mx.md)：量化系数分形固定为2×16，对应L0B中一个32×16数据分形的地址映射。

两个接口的源L1和目的MX Buffer地址均需要32字节对齐，最小搬运粒度为一个32字节量化系数分形。L0A_MX和L0B_MX容量各为4KB。`x_step`或`y_step`为0时接口为空操作；目的地址和步长必须与主矩阵分形建立正确映射。

## asc_copy_l0c2l1（L0C到L1随路处理搬运）

该接口从L0C搬出`int32_t`或`float`矩阵结果，在写入L1时可组合类型转换、scalar/tensor量化、ReLU或Leaky ReLU、Nz2ND/Nz2DN、通道拆分和UnitFlag。

启用相应能力前，通过以下接口配置状态：

| 功能 | 配置接口 |
| --- | --- |
| Nz2ND/Nz2DN | [asc_set_l0c_copy_nz_para](../cube_datamove/asc_set_l0c_copy_nz_para.md) |
| Nz2DN通道参数 | [asc_set_l0c_copy_channel_para](../cube_datamove/asc_set_l0c_copy_channel_para.md) |
| scalar量化 | [asc_set_l0c_copy_prequant](../cube_datamove/asc_set_l0c_copy_prequant.md) |
| tensor量化参数地址 | [asc_set_l0c_copy_config](../cube_datamove/asc_set_l0c_copy_config.md) |
| ReLU/Leaky ReLU参数 | [asc_set_l0c_copy_relu_alpha](../cube_datamove/asc_set_l0c_copy_relu_alpha.md)、[asc_set_l0c_copy_lrelu_alpha](../cube_datamove/asc_set_l0c_copy_lrelu_alpha.md) |

`src`需要64字节对齐，`dst`需要32字节对齐。`n_size`、`m_size`和`dst_stride`的对齐由目的数据位宽及输出格式共同决定，源、目的数据类型必须与`quant_pre_mode`匹配。

## 同步与边界检查

- L1由PIPE_MTE2或其他流水写入后，PIPE_MTE1读取前需要建立依赖；L0A/L0B写入后，PIPE_M发起矩阵计算前也需要建立依赖。
- PIPE_M写入L0C后，PIPE_FIX搬出结果前需要同步，或在满足接口约束时使用UnitFlag建立分形粒度的并行。
- L1容量上限为512KB，L0A/L0B各64KB，L0C为256KB；二维和三维模式均需把完整边界分形计入访问范围。
- 配置接口设置的Feature Map、Padding、Repeat、量化和格式转换状态会影响后续搬运，复用前应按当前任务重新核对。
