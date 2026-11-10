# SetFixPipeReluAlpha<a name="ZH-CN_TOPIC_0000002738231208"></a>

## 产品支持情况<a name="section18204144912492"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- KirinX90处理器系列产品：不支持
<!-- end id8 -->
<!-- npu="9020" id9 -->
- Kirin9020处理器系列产品：不支持
<!-- end id9 -->
<!-- npu="9030" id10 -->
- Kirin9030处理器系列产品：不支持
<!-- end id10 -->
<!-- npu="9050" id11 -->
- Kirin9050处理器系列产品：支持
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixPipeReluAlpha_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_macro_intf.h"`。

MacroCube计算（如[MmadFix](../mmad_compute/MmadFix.md)、ConvFix）过程中，开启激活（ReLU）后处理功能时，通过调用该接口设置激活函数的LUT（Look-Up Table）模式和preValue参数。

lut\_mode\_t枚举类型定义如下：

```cpp
enum class lut_mode_t : uint8_t { Gelu, Silu, Sigmoid, Tanh };
```

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaBitModeParams& params)

__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaParams& params)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| params | 输入 | 激活相关参数，类型为FixPipeReluAlphaParams或FixPipeReluAlphaBitModeParams。<br>FixPipeReluAlphaParams的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`，FixPipeReluAlphaBitModeParams的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>FixPipeReluAlphaParams参数说明请参考[表2](#table2fixpipereulalpha)，FixPipeReluAlphaBitModeParams参数说明请参考[表3](#table3fixpipereulalpha)。 |

<a name="table2fixpipereulalpha"></a>
**表2** FixPipeReluAlphaParams结构体内参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| lut | 激活函数的LUT模式，lut\_mode\_t枚举类型，默认值为Gelu。取值包括：Gelu、Silu、Sigmoid、Tanh。 |
| preValue | 激活函数计算时使用的scalar参数，uint32\_t类型，默认值为0。传入时需将float类型的参数转换为uint32\_t类型。 |

<a name="table3fixpipereulalpha"></a>
**表3** FixPipeReluAlphaBitModeParams类参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| config0 | uint64\_t类型，与FixPipeReluAlphaBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | FixPipeReluAlphaBitModeConfig0位域（bit-field）结构体类型，参数参考[表4](#table4fixpipereulalpha)，与config0组成联合体（union）。 |

FixPipeReluAlphaBitModeParams对于FixPipeReluAlphaParams结构体构造进行了优化，本接口适用于scalar流水成为性能优化瓶颈的场景。FixPipeReluAlphaBitModeParams类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅需要通过循环和位运算即可实现，不需要重新传入参数，减少了scalar计算，实现性能提升。

FixPipeReluAlphaBitModeParams类可以直接使用FixPipeReluAlphaParams结构体类型对象初始化：

```cpp
__aicore__ inline FixPipeReluAlphaBitModeParams(const FixPipeReluAlphaParams& fixPipeReluAlphaParams);
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0直接进行逐bit位修改来修改参数。

<a name="table4fixpipereulalpha"></a>
**表4** FixPipeReluAlphaBitModeConfig0结构体参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| lut | 激活函数的LUT模式，lut\_mode\_t枚举类型。<br>该参数是位域结构体的最低位参数，占用3bit，可以使用FixPipeReluAlphaBitModeParams类对象的SetLut()函数设置其值，使用GetLut()函数获取其值。 |
| reserved0 | 预留参数，占用10bit。 |
| preValue | 激活函数计算时使用的scalar参数。<br>该参数是位域结构体的最高位参数，占用19bit，可以使用FixPipeReluAlphaBitModeParams类对象的SetPreValue()函数设置其值，使用GetPreValue()函数获取其值。 |

## 数据类型

支持的数据类型为：uint32\_t、uint64\_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 该接口需配合MacroCube系列计算接口（如[MmadFix](../mmad_compute/MmadFix.md)、ConvFix）一起使用，需在MacroCube计算接口之前调用。
- 本章节涉及寄存器在相同核函数（Kernel）内部会维持状态直到被人为改变，而在不同核函数（Kernel）内部不会继承之前的状态。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
// 设置激活函数LUT模式和preValue参数
AscendC::lut_mode_t lutMode = AscendC::lut_mode_t::Gelu;
float tmp = (float)0.5;
// 将float类型的tmp转换为uint32_t的preValue
uint32_t preValue = static_cast<uint32_t>(*reinterpret_cast<int32_t*>(&tmp));

// 方式一：使用FixPipeReluAlphaParams结构体
AscendC::FixPipeReluAlphaParams reluAlphaParams(lutMode, preValue);
AscendC::SetFixPipeReluAlpha(reluAlphaParams);

// 方式二：使用FixPipeReluAlphaBitModeParams类（适用于scalar流水成为性能优化瓶颈的场景）
AscendC::FixPipeReluAlphaBitModeParams reluAlphaBitModeParams(reluAlphaParams);
AscendC::SetFixPipeReluAlpha(reluAlphaBitModeParams);
```
