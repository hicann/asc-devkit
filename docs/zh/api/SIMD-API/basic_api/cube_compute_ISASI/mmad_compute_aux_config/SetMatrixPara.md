# SetMatrixPara<a name="ZH-CN_TOPIC_00000027121725633"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMatrixPara_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_macro_intf.h"`。

调用[MmadFix](../mmad_compute/MmadFix.md)接口进行矩阵乘计算前，通过调用该接口设置矩阵乘的M维度（左矩阵Height）、K维度（左矩阵Width/右矩阵Height）以及groupK（分组矩阵乘的K分组数）参数。N维度（右矩阵Width）通过MmadFixParams参数的n字段传入。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMatrixPara(const MatrixParaBitModeParams& params)

__aicore__ inline void SetMatrixPara(const MatrixParaParams& params)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| params | 输入 | 矩阵乘维度参数，类型为MatrixParaParams或MatrixParaBitModeParams。<br>MatrixParaParams的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`，MatrixParaBitModeParams的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>MatrixParaParams参数说明请参考[表2](#table2matrixpara)，MatrixParaBitModeParams参数说明请参考[表3](#table3matrixpara)。 |

<a name="table2matrixpara"></a>
**表2** MatrixParaParams结构体内参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| m | 左矩阵Height（M维度），uint16\_t类型，默认值为0。 |
| k | 左矩阵Width/右矩阵Height（K维度），uint16\_t类型，默认值为0。 |
| groupK | 分组矩阵乘的K分组数，uint16\_t类型，默认值为0。非分组场景下配置为0。 |

<a name="table3matrixpara"></a>
**表3** MatrixParaBitModeParams类参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| config0 | uint64\_t类型，与MatrixParaBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | MatrixParaBitModeConfig0位域（bit-field）结构体类型，参数参考[表4](#table4matrixpara)，与config0组成联合体（union）。 |

MatrixParaBitModeParams对于MatrixParaParams结构体构造进行了优化，本接口适用于scalar流水成为性能优化瓶颈的场景。MatrixParaBitModeParams类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅需要通过循环和位运算即可实现，不需要重新传入参数，减少了scalar计算，实现性能提升。

MatrixParaBitModeParams类可以直接使用MatrixParaParams结构体类型对象初始化：

```cpp
__aicore__ inline MatrixParaBitModeParams(const MatrixParaParams& matrixParaParams);
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0直接进行逐bit位修改来修改参数。

<a name="table4matrixpara"></a>
**表4** MatrixParaBitModeConfig0结构体参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| m | 左矩阵Height（M维度），uint16\_t类型。<br>该参数是位域结构体的最低位参数，占用16bit，可以使用MatrixParaBitModeParams类对象的SetM()函数设置其值，使用GetM()函数获取其值。 |
| k | 左矩阵Width/右矩阵Height（K维度），uint16\_t类型。<br>该参数是位域结构体的第二低位参数，占用16bit，可以使用MatrixParaBitModeParams类对象的SetK()函数设置其值，使用GetK()函数获取其值。 |
| groupK | 分组矩阵乘的K分组数，uint16\_t类型。<br>该参数是位域结构体的最高位参数，占用16bit，可以使用MatrixParaBitModeParams类对象的SetGroupK()函数设置其值，使用GetGroupK()函数获取其值。 |

## 数据类型

支持的数据类型为：uint16\_t、uint64\_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 该接口需配合[MmadFix](../mmad_compute/MmadFix.md)接口一起使用，需在MmadFix接口之前调用。
- 矩阵乘的M、K维度通过该接口配置，N维度通过MmadFixParams参数的n字段传入。
- 本章节涉及寄存器在相同核函数（Kernel）内部会维持状态直到被人为改变，而在不同核函数（Kernel）内部不会继承之前的状态。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
constexpr uint16_t m = 16;    // 左矩阵Height（M维度）
constexpr uint16_t k = 128;   // 左矩阵Width/右矩阵Height（K维度）
constexpr uint16_t groupK = 0;  // 非分组场景，groupK配置为0

// 方式一：使用MatrixParaParams结构体
AscendC::MatrixParaParams matrixParaParams(m, k, groupK);
AscendC::SetMatrixPara(matrixParaParams);

// 方式二：使用MatrixParaBitModeParams类（适用于scalar流水成为性能优化瓶颈的场景）
AscendC::MatrixParaBitModeParams matrixParaBitModeParams(matrixParaParams);
AscendC::SetMatrixPara(matrixParaBitModeParams);
```
