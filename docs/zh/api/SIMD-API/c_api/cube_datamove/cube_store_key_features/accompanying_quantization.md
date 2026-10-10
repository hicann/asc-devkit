# 随路量化

## 特性说明

矩阵搬出支持随路类型转换和带系数的量化，可在L0C Buffer结果搬出的过程中完成数值处理。量化模式决定处理方式，量化参数提供该模式所需的缩放系数、偏移量等信息；配置量化参数后，仍需在搬出接口中选择对应模式。

<!-- npu="950" id10 -->
针对Ascend 950PR&950DT系列产品：

通过[asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md)、[asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md)或[asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md)搬出时，使用`quant_pre_mode`选择量化模式，其取值见[asc_quant_mode](../../defs/enum/asc_quant_mode.md)。根据是否需要量化系数及系数的共享范围，可分为以下三种情况：

- **类型转换**：使用`asc_quant_mode::F322F16`或`asc_quant_mode::F322BF16`将float转换为half或bfloat16_t，无需配置量化系数，舍入模式采用CAST_RINT。
- **Scalar量化**：整个C矩阵共享一个量化参数，参数由QUANT_PRE寄存器提供。
- **Tensor/Vector量化**：C矩阵的每一列对应一个8B量化参数，参数shape为[N]，N为C矩阵的列数。参数由Fixpipe Buffer的量化参数区提供，每个元素均为打包后的`uint64_t`，不能直接使用float数组。

其中，`QF322F16_PRE`/`VQF322F16_PRE`和`QF322BF16_PRE`/`VQF322BF16_PRE`虽然也输出half或bfloat16_t，但属于带系数量化，需按Scalar或Vector模式准备参数。模式名称中的`V`前缀表示Vector粒度。Scalar和Vector使用相同的量化参数编码，各字段含义见[Ascend 950PR&950DT系列产品编码表](#quant-pre-3510)。

**配置与调用顺序：**

1. 根据源/目的数据类型、搬出通路和输出布局，在对应搬出接口中确认支持的量化模式及其组合约束。
2. 按所选模式准备参数：类型转换无需配置系数；Scalar量化通过[asc_set_l0c_copy_prequant(scale, offset, is_signed)](../cube_store_aux_config/asc_set_l0c_copy_prequant.md)设置参数；Vector量化将每列参数准备在L1 Buffer，经[asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb_arch_3510.md)搬入Fixpipe Buffer的量化参数区，再通过[asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md)设置`quant_pre_addr`。若Vector参数原先位于GM，需先通过[asc_copy_gm2l1](../cube_compute_load/asc_copy_gm2l1.md)搬入L1 Buffer。
3. 根据输入数据及参数搬运的依赖关系完成[核内同步](../../sync/intra_core_sync/intra_core_sync.md)后，调用L0C搬出接口，并在`quant_pre_mode`中传入所选的`asc_quant_mode`枚举值。

Vector量化的`quant_pre_addr`为量化参数区起始字节地址除以128，不能直接传入元素下标。若还需随路激活，参见[随路ReLU](accompanying_relu.md)：激活参数地址索引以64B为单位，与量化参数区独立寻址。两类参数的起始地址及占用范围均不能超出各自区域，具体要求见[asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md)。
<!-- end id10 -->

<!-- npu="A3,910b" id2 -->
针对以下产品型号：

<!-- npu="A3" id4 -->
- Atlas A3系列产品
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2系列产品
<!-- end id5 -->

通过[asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_2201.md)或[asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_2201.md)搬出时，使用整数参数`quant_pre`选择量化模式，可显式转换[asc_quant_mode](../../defs/enum/asc_quant_mode.md)枚举得到模式编码。Scalar量化通过[asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md)的`uint64_t config`重载传入打包后的参数，其中低32位为float的位模式，不能通过浮点数到整数的数值转换得到；Vector量化为每列准备一个打包后的`uint64_t`参数，经[asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb_arch_2201.md)搬入Fixpipe Buffer，再通过[asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md)配置量化参数起始地址索引。参数编码见[量化参数编码表](#quant-pre-2201)，调用时需遵循对应接口的搬运、对齐及同步约束。
<!-- end id2 -->

当前支持的量化模式及其功能描述如下：

```text
DEQF16,                // DeQuant_Float16：int32_t反量化成half，scalar量化
VDEQF16,               // Vector_DeQuant_Float16：int32_t反量化成half，tensor量化
QF322B8_PRE,           // Quant_Float32_2_B8：float量化成int8_t/uint8_t，scalar量化
VQF322B8_PRE,          // Vector_Quant_Float32_2_B8：float量化成int8_t/uint8_t，tensor量化
REQ8,                  // ReQuant_int8：int32_t重量化成int8_t/uint8_t，scalar量化
VREQ8,                 // Vector_ReQuant_int8：int32_t重量化成int8_t/uint8_t，tensor量化
```

<!-- npu="950" id3 -->
除上述量化模式外，Ascend 950PR&950DT系列产品还额外支持以下量化模式：

```text
QF322FP8_PRE,          // Quant_Float32_2_FP8: float量化成fp8_e4m3fn_t，scalar量化
VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: float量化成fp8_e4m3fn_t，tensor量化
QF322HIF8_PRE,         // Quant_Float32_2_HIF8: float量化成hifloat8_t(Half to Away Round)，scalar量化
VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: float量化成hifloat8_t(Half to Away Round)，tensor量化
QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: float量化成hifloat8_t(Hybrid Round)，scalar量化
VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: float量化成hifloat8_t(Hybrid Round)，tensor量化
QS322BF16_PRE,         // Quant_Int32_2_BFloat16: int32_t量化成bfloat16_t，scalar量化
VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: int32_t量化成bfloat16_t，tensor量化
QF322F16_PRE,          // Quant_Float32_2_Float16: float量化成half，scalar量化
VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: float量化成half，tensor量化
QF322BF16_PRE,         // Quant_Float32_2_BFloat16: float量化成bfloat16_t，scalar量化
VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: float量化成bfloat16_t，tensor量化
QF322F32_PRE,          // Quant_Float32_2_Float32: float量化成float，scalar量化，使用有限精度的量化系数，需按输入范围验证误差
VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: float量化成float，tensor量化，使用有限精度的量化系数，需按输入范围验证误差
```
<!-- end id3 -->

不同量化模式的量化算法详细介绍参见[随路量化与随路ReLU场景组合](accompanying_quantization_and_relu_scenario_combination.md)。

量化参数比特位含义见下表：

<!-- npu="A3,910b" id8 -->
<a id="quant-pre-2201"></a>

**表1** 量化参数QUANT\_PRE比特位含义映射表（[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 模式 | 比特位数 | 变量名 | 作用介绍 |
| ------ | ---------- | -------- | ---------- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 0~31 | M1 | 32位数视为float，硬件以(1, 8, 10)的格式，即1个符号位、8个指数位和10个尾数位作为量化计算所需要乘的值。其中31位为符号位，23~30位为指数位，13~22位为尾数位。 |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 32~35 | N | 4位比特位，表示范围为\[1, 16\]（b'0000对应表示1，b'1111对应表示16）。<br>当模式为(V)REQ8时，MCB标志位置为1时，将输入的值进行右移N比特位。当模式为(V)QF322B8_PRE，N为无效变量。 |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 36 | MCB标志位 | Mode Control Bit。如果置为0，输入的int32_t会被直接转换为float，N为无效值。如果置为1，输入的int32_t会先右移N比特位，转变成int16_t，然后转换为float。当模式为(V)QF322B8_PRE时，此标志位为无效比特。 |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 37~45 | Offset | 9bit的整型数据，源数据乘以量化系数或者随路系数的计算结果，可以与Offset表示的整数值进行相加。若不使用offset，请置为0。<br>当模式为(V)DEQF16时，此变量为无效变量。 |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 46 | Sign标志位 | 如果置为1，表明量化结果是signed(int8)；如果置为0，表明量化结果是unsigned(uint8)。仅在(V)REQ8、(V)QF322B8_PRE中会用到。 |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 47~63 | - | 无效比特位，用户无需关注。 |
<!-- end id8 -->

<!-- npu="950" id9 -->
<a id="quant-pre-3510"></a>

**表2** 量化参数QUANT\_PRE比特位含义映射表（[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 模式 | 比特位数 | 变量名 | 作用介绍 |
| --- | --- | --- | --- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16<br>&bull;(V)QF322FP8_PRE<br>&bull;(V)QF322HIF8_PRE<br>&bull;(V)QF322HIF8_PRE_HYBRID<br>&bull;(V)QS322BF16_PRE<br>&bull;(V)QF322F16_PRE<br>&bull;(V)QF322BF16_PRE<br>&bull;(V)QF322F32_PRE<br>&bull;(V)REQ4<br>&bull;(V)QF322S4_PRE | 0~12 | - | 无效比特位，用户无需关注。 |
| 同第一行 | 13~31 | M1 | 数据类型视为float，硬件以(1, 8, 10)的格式，即1个符号位、8个指数位和10个尾数位作为量化计算所需要乘的值。其中31位为符号位，23~30位为指数位，13~22位为尾数位。其值不能为inf/nan。 |
| 同第一行 | 32~36 | - | 无效比特位，用户无需关注。 |
| 同第一行 | 37~45 | Offset | 9bit整形数据，源数据乘以量化系数或者随路系数的计算结果，可以与Offset表示的整数值进行相加。若不使用offset，请置为0。当模式为(V)REQ8、(V)QF322B8_PRE、(V)REQ4、(V)QF322S4_PRE时该变量生效。 |
| 同第一行 | 46 | Sign标志位 | 如果置为1，表明量化结果是signed(int8)；如果为置为0，表明量化结果是unsigned(uint8)。仅在(V)REQ8、(V)QF322B8_PRE中会用到。 |
| 同第一行 | 47~63 | - | 无效比特位，用户无需关注。 |
<!-- end id9 -->

**使用约束与参数精度：**

- 量化参数不能为inf、nan或非规格化数；配置前需检查缩放系数，具体限制见对应配置接口。
- 缩放系数按表中有效位参与计算，不保留完整FP32尾数精度。输出仍为float的量化模式也会受到该有效精度的影响。
- Offset、Sign只在表中列出的模式下生效。

  <!-- npu="A3,910b" id17 -->
  Atlas A3系列产品和Atlas A2系列产品的N/MCB只在表中列出的模式下生效。
  <!-- end id17 -->

  <!-- npu="950" id18 -->
  Ascend 950PR&950DT系列产品中32~36位无效，不能沿用Atlas A3系列产品和Atlas A2系列产品的右移配置。
  <!-- end id18 -->

- Vector量化的参数按原始C矩阵的N维列索引对应。分块搬出时，应同步检查参数区偏移、列数、对齐及占用范围。Fixpipe Buffer的量化区和激活区独立寻址，不能把元素下标直接传给地址索引参数。
- 不同搬出任务需要不同系数时，应在对应搬出执行前重新配置。`asc_set_l0c_copy_config`每次同时覆盖激活地址、量化地址和UnitFlag初始化参数，三者的含义见该接口的参数说明。

**使用示例：**

[asc_copy_l0c2gm样例](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/README.md)的场景4、5分别展示Scalar和Vector量化。运行时按样例声明的产品支持范围选择环境。算法示意及量化与ReLU的配合见[随路量化与随路ReLU场景组合](accompanying_quantization_and_relu_scenario_combination.md)。
