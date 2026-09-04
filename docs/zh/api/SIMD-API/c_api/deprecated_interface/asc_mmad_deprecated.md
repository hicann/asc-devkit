# asc_mmad（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cube_compute/cube_compute.h"`。

**入参`unit_flag`类型为`uint8_t`的`asc_mmad`接口以及`asc_mmad_sync`接口已废弃。请使用入参`unit_flag_mode`类型为`asc_unit_flag_mode`的[asc_mmad](../cube_compute/asc_mmad.md)接口替代；原同步功能请通过新接口和`asc_sync()`实现，具体请参见[asc_sync](../sync/asc_sync.md)。**

<!-- npu="A3,910b" id18 -->
针对如下产品型号：

<!-- npu="A3" id22 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id22 -->
<!-- npu="910b" id23 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id23 -->
**入参带有`feat_offset`参数的`asc_mmad`、`asc_mmad_s4`接口以及全部`asc_mmad_sync`、`asc_mmad_s4_sync`接口已废弃。带`feat_offset`参数的接口请改用不带该参数且使用`asc_unit_flag_mode`的对应接口。**
<!-- end id18 -->


本接口是Ascend C面向昇腾AI芯片的矩阵乘加（Mmad）核心计算接口，专为高性能算子开发设计，封装了昇腾NPU硬件的矩阵乘加计算能力，广泛用于神经网络层（如全连接层、卷积层）、数值计算类算子的开发，其计算公式如下：

$$
C_{M \times N} = A_{M \times K} \times B_{K \times N} + C_{M \times N}
$$

其中，A、B、C分别为左、右、结果矩阵，C矩阵可以通过配置本接口的参数，初始化为全0矩阵、L0C Buffer中的矩阵或Bias矩阵，各矩阵的信息说明见下表：

<!-- npu="950" id8 -->
**表** 矩阵信息说明（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 矩阵 | 存储位置 | 形状（行数×列数） | 数据格式 | 分形大小（行数×列数） |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Nz | 16×K0 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias（用于C矩阵初始化） | BiasTable Buffer | 1×N，使用时通过广播复制M行来初始化C矩阵 | ND | - |

表格中K0的取值为`32B / sizeof(dtype)`，`dtype`为矩阵的数据类型。
<!-- end id8 -->

<!-- npu="A3,910b" id19 -->
**表** 矩阵信息说明（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 矩阵 | 存储位置 | 形状 | 数据格式 | 分形大小 |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Zz | 16×K0 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias（用于C矩阵初始化） | BiasTable Buffer | 1×N，使用时通过广播复制M行来初始化C矩阵 | ND | - |

K0的取值为`32B / sizeof(dtype)`，`dtype`为矩阵的数据类型。当数据类型为`int4b_t`时，`K0 = 64`。
<!-- end id19 -->

本接口为矩阵计算接口，仅在AIC上生效。

## 函数原型

<!-- npu="950" id12 -->
### 函数原型（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

**占位符形式：**

```c
__aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix,
                                __ca__ <a_dtype>* a_matrix,
                                __cb__ <b_dtype>* b_matrix,
                                uint16_t left_height,
                                uint16_t n_dim,
                                uint16_t right_width,
                                uint8_t unit_flag,
                                bool disable_gemv,
                                bool c_matrix_source,
                                bool c_matrix_init_val)

__aicore__ inline void asc_mmad_sync(__cc__ <c_dtype>* c_matrix,
                                     __ca__ <a_dtype>* a_matrix,
                                     __cb__ <b_dtype>* b_matrix,
                                     uint16_t left_height,
                                     uint16_t n_dim,
                                     uint16_t right_width,
                                     uint8_t unit_flag,
                                     bool disable_gemv,
                                     bool c_matrix_source,
                                     bool c_matrix_init_val)
```

**dtype支持的数据类型：**

`c_dtype`、`a_dtype`、`b_dtype`的取值组合见下表：

**表** 支持的数据类型组合（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="asc_mmad_data_type"></a>

| a_dtype | b_dtype | c_dtype | Bias数据类型 |
| --- | --- | --- | --- |
| `int8_t` | `int8_t` | `int32_t` | `int32_t` |
| `hifloat8_t` | `hifloat8_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` | `float` |
| `half` | `half` | `float` | `float` |
| `bfloat16_t` | `bfloat16_t` | `float` | `float` |
| `float` | `float` | `float` | `float` |

**典型示例：**

```c
__aicore__ inline void asc_mmad(__cc__ float* c_matrix,
                                __ca__ bfloat16_t* a_matrix,
                                __cb__ bfloat16_t* b_matrix,
                                uint16_t left_height,
                                uint16_t n_dim,
                                uint16_t right_width,
                                uint8_t unit_flag,
                                bool disable_gemv,
                                bool c_matrix_source,
                                bool c_matrix_init_val)
```
<!-- end id12 -->

<!-- npu="A3,910b" id20 -->
### 函数原型（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

- `asc_mmad`及`asc_mmad_sync`：

  ```c
  __aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix, __ca__ <a_dtype>* a_matrix, __cb__ <b_dtype>* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val)
  __aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix, __ca__ <a_dtype>* a_matrix, __cb__ <b_dtype>* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val)
  __aicore__ inline void asc_mmad_sync(__cc__ <c_dtype>* c_matrix, __ca__ <a_dtype>* a_matrix, __cb__ <b_dtype>* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val)
  __aicore__ inline void asc_mmad_sync(__cc__ <c_dtype>* c_matrix, __ca__ <a_dtype>* a_matrix, __cb__ <b_dtype>* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val)
  ```

  `c_dtype`、`a_dtype`、`b_dtype`支持以下组合：`float/bfloat16_t/bfloat16_t`、`float/half/half`、`float/float/float`和`int32_t/int8_t/int8_t`。

- `asc_mmad_s4`及`asc_mmad_s4_sync`：

  ```c
  __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val)
  __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val)
  __aicore__ inline void asc_mmad_s4_sync(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val)
  __aicore__ inline void asc_mmad_s4_sync(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val)
  ```
<!-- end id20 -->

## 参数说明

<!-- npu="950" id14 -->
**表** 参数说明（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="asc_mmad_param_table"></a>

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| c_matrix | 输出 | 目的操作数，结果矩阵C在L0C Buffer中的起始地址，需按照1024字节对齐。数据类型由接口重载决定，具体请参见[支持的数据类型组合](#asc_mmad_data_type)。 |
| a_matrix | 输入 | 源操作数，左矩阵A在L0A Buffer中的起始地址，需按照512字节对齐。数据类型由接口重载决定，具体请参见[支持的数据类型组合](#asc_mmad_data_type)。 |
| b_matrix | 输入 | 源操作数，右矩阵B在L0B Buffer中的起始地址，需按照512字节对齐。数据类型由接口重载决定，具体请参见[支持的数据类型组合](#asc_mmad_data_type)。 |
| left_height | 输入 | 左矩阵A和结果矩阵C的M维大小，单位为元素，取值范围为[0, 4095]。 |
| n_dim | 输入 | 左矩阵A和右矩阵B的K维大小，单位为元素，取值范围为[0, 4095]。 |
| right_width | 输入 | 右矩阵B和结果矩阵C的N维大小，单位为元素，取值范围为[0, 4095]。 |
| unit_flag | 输入 | 用于控制矩阵乘加指令与矩阵搬出指令的细粒度并行，开启UnitFlag后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0：不开启UnitFlag。<br>&nbsp;&nbsp;&bull; 2：开启UnitFlag，硬件执行完指令后不改变单元标志位。<br>&nbsp;&nbsp;&bull; 3：开启UnitFlag，硬件执行完指令后改变单元标志位。<br>矩阵乘加指令与对应的矩阵搬出指令必须都开启或都不开启UnitFlag，开启后指令之间无需再插入同步指令。 |
| disable_gemv | 输入 | M为1时，配置是否关闭GEMV模式。<br>&nbsp;&nbsp;&bull; false：开启GEMV模式。<br>&nbsp;&nbsp;&bull; true：关闭GEMV模式。<br>M不为1时，该参数不生效。 |
| c_matrix_source | 输入 | 当参数`c_matrix_init_val`为false时，配置矩阵C的初始值来源。<br>&nbsp;&nbsp;&bull; false：矩阵C的初始值来源于L0C Buffer。<br>&nbsp;&nbsp;&bull; true：矩阵C的初始值来源于BiasTable Buffer。 |
| c_matrix_init_val | 输入 | 配置是否将矩阵C的初始值设置为0。<br>&nbsp;&nbsp;&bull; true：将矩阵C的初始值设置为0，参数`c_matrix_source`不生效。<br>&nbsp;&nbsp;&bull; false：不执行清零操作，矩阵C的初始值由参数`c_matrix_source`配置。 |

<!-- end id14 -->

<!-- npu="A3,910b" id21 -->
**表** 参数说明（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| c_matrix | 输出 | 结果矩阵C在L0C Buffer中的起始地址，需按照1024字节对齐。 |
| a_matrix | 输入 | 左矩阵A在L0A Buffer中的起始地址，需按照512字节对齐。 |
| b_matrix | 输入 | 右矩阵B在L0B Buffer中的起始地址，需按照512字节对齐。 |
| left_height | 输入 | 左矩阵A和结果矩阵C的M维大小，取值范围为[0, 4095]。 |
| n_dim | 输入 | 左矩阵A和右矩阵B的K维大小，取值范围为[0, 4095]。 |
| right_width | 输入 | 右矩阵B和结果矩阵C的N维大小，取值范围为[0, 4095]。 |
| feat_offset | 输入 | 无效参数，用户无需关注，传入0即可。 |
| smask_offset | 输入 | 无效参数，用户无需关注，传入0即可。 |
| unit_flag | 输入 | UnitFlag控制参数。0表示关闭；2表示开启且执行后保持单元标志位；3表示开启且执行后更新单元标志位。 |
| k_direction_align | 输入 | 使用float数据类型时控制K方向的对齐方式。false表示对齐到`ceil(n_dim / 8) * 8`，true表示对齐到`ceil(n_dim / 16) * 16`。 |
| is_weight_offset | 输入 | 无效参数，用户无需关注，传入false即可。 |
| c_matrix_source | 输入 | 当`c_matrix_init_val`为false时，配置矩阵C的初始值是否来自BiasTable Buffer。 |
| c_matrix_init_val | 输入 | 配置是否将矩阵C的初始值设置为0。 |

<!-- end id21 -->

## 返回值说明

无

## 流水类型

PIPE_M

## 约束说明

- 本接口仅在AIC上生效，在AIV上调用将直接返回。
- `left_height`、`n_dim`、`right_width`中的任意一个值为0时，接口将被视为NOP（空操作）。

- 内存使用约束说明：
  <!-- npu="950" id10 -->
  - 针对Ascend 950PR/Ascend 950DT:

      - L0C Buffer大小为256KB，L0A Buffer和L0B Buffer大小均为64KB。BiasTable Buffer大小为4KB。矩阵的起始地址和占用空间不能超出对应Buffer的范围。
      - 各矩阵的起始地址需满足[参数说明](#asc_mmad_param_table)中的对齐要求。操作数的其他地址约束请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
      - 申请矩阵存储空间时，需使用按照分形大小补齐后的数值进行申请：M、N分别向上补齐到16的倍数，K向上补齐到K0的倍数，K0的取值为`32B / sizeof(dtype)`，`dtype`为矩阵的数据类型。`left_height`、`n_dim`和`right_width`仍传入矩阵的有效M、K、N值，补齐部分为无效数据，不参与结果矩阵有效区域的计算。
      - 当M为1且`disable_gemv`为false时，将开启GEMV模式。此时从L0A Buffer读取矩阵A时按照ND格式读取，矩阵A需按照ND格式排布，起始地址仍需按照512字节对齐。
  <!-- end id10 -->

  <!-- npu="A3,910b" id11 -->
  - 针对如下产品型号：

 	  <!-- npu="A3" id16 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
 	  <!-- end id16 -->
 	  <!-- npu="910b" id17 -->
 	  Atlas A2 训练系列产品/Atlas A2 推理系列产品
 	  <!-- end id17 -->
 	  L0C Buffer大小为128KB，L0A Buffer和L0B Buffer大小均为64KB。BiasTable Buffer大小为1KB。矩阵的起始地址和占用空间不能超出对应Buffer的范围。
  <!-- end id11 -->

- 同步约束说明：

  针对输入矩阵沿K轴分块计算，并将结果累加到同一块L0C Buffer的场景，当`(left_height / 16) * (right_width / 16) < 10`时，需在相邻两次矩阵乘加指令之间调用[asc_sync_pipe](../sync/asc_sync_pipe.md)，并将入参`pipe`设置为`PIPE_M`。

- UnitFlag约束说明：

  - 开启UnitFlag时，矩阵乘加指令与对应矩阵搬出指令需同时开启UnitFlag。当希望同一块L0C Buffer内存空间能持续只被多条矩阵乘加指令或多条矩阵搬出指令操作时，需将前n-1条指令的unitFlag值设置为2，维持被操作内存空间的持续占用状态，最后一条指令设置为3，解除被占用状态。
  - 开启UnitFlag时，矩阵计算方向需与矩阵搬出读取顺序保持一致。矩阵搬出指令开启Nz2ND随路格式转换，或未进行随路格式转换但开启B8/B4量化并触发Channel Merge功能时，调用[asc_set_mmad_direction_n](../cube_compute/asc_set_mmad_direction_n.md)；其他场景调用[asc_set_mmad_direction_m](../cube_compute/asc_set_mmad_direction_m.md)。
  - 开启UnitFlag时，建议矩阵乘加的计算数据量与矩阵搬出的数据量保持一致。两者不一致可能导致执行异常。需要清除UnitFlag产生的残留状态时，可调用[asc_set_l0c2gm_config](../cube_datamove/asc_set_l0c2gm_config.md)，并将`enable_unit_flag`设置为true，将L0C Buffer中所有内存块的单元标志位设置为0并关闭UnitFlag。

- 特殊值/边界值约束说明：

  浮点类型的输入或输出包含inf/nan时，可通过[asc_set_ctrl](../spr/asc_set_ctrl.md)接口配置CTRL寄存器的CTRL\[48\]比特位控制计算时的模式：

  - 设置为0时使用饱和模式，inf输出饱和为±MAX、nan输出饱和为0；
  - 设置为1时使用非饱和模式，inf/nan保持原输出。

  注意，应避免nan输入，否则可能会产生执行报错；整数类型仅支持饱和模式。
