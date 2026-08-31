# Cast<a name="ZH-CN_TOPIC_0000001956827113"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_vconv_intf.h"`。

Cast用于数据类型精度转换，将源操作数数据类型转换成目的操作数数据类型，能够实现[浮点转整数](#表6-浮点转整数)、[浮点转浮点](#表7-浮点转浮点)、[整数转浮点](#表8-整数转浮点)、[整数转整数](#表9-整数转整数)的数据类型转换。转换过程中，由于位宽变化、精度变化，支持配置如下参数进行功能实现：
- [RegLayout](../aux_data_types/RegLayout.md)：源操作数和目的操作数位宽不同时，单条指令计算量以位宽更大的数据类型为准，RegLayout用于控制位宽小的元素在寄存器中的排布方式。
- [SatMode](../aux_data_types/SatMode.md)：用于设置饱和与不饱和模式。
- [MaskMergeMode](../aux_data_types/MaskMergeMode.md)：用于指定写入寄存器数据模式，mask未选择的元素在dst中置零或保留dst原值。
- [RoundMode](../aux_data_types/RoundMode.md)：用于设置舍入模式。

不同数据类型下元素对应的mask位宽不一致，在Cast进行类型转换时，MaskReg根据输入的源操作数进行有效元素筛选。
图[b16到b32类型转换过程](#fig1)和图[b32到b16类型转换过程](#fig2)展示了MaskReg和RegLayout同时作用时b16和b32进行类型转换的过程。

**图1** b16到b32类型转换过程<a id="fig1"></a>  

![](../../../../figures/b16_to_b32_conversion.png)

**图2** b32到b16类型转换过程<a id="fig2"></a>  

![](../../../../figures/b32_to_b16_conversion.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, const CastTrait& trait = castTrait, typename S, typename V>
__simd_callee__ inline void Cast(S& dstReg, V& srcReg, MaskReg& mask);
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| trait | CastTrait类型，类型转换模式结构体。包括[RegLayout](../aux_data_types/RegLayout.md)、[SatMode](../aux_data_types/SatMode.md)、[MaskMergeMode](../aux_data_types/MaskMergeMode.md)、[RoundMode](../aux_data_types/RoundMode.md)。<br><code>struct CastTrait {<br>    RegLayout layoutMode = RegLayout::UNKNOWN;<br>    SatMode satMode = SatMode::UNKNOWN;<br>    MaskMergeMode mrgMode = MaskMergeMode::UNKNOWN;<br>    RoundMode roundMode = RoundMode::UNKNOWN;<br>};</code><br>使能SatMode生效需与SetCtrlSpr配合使用。不饱和模式和饱和模式的具体配置请参考[表5 饱和模式全局或单指令生效配置表](#表5-饱和模式全局或单指令生效配置表)。<br>注：SetCtrlSpr需在SIMD_VF外调用。 |
| S | 目的操作数的[RegTensor](../register_data_types/RegTensor.md)类型，例如RegTensor&lt;float&gt;，由编译器自动推导，用户不需要填写。 |
| V | 源操作数的[RegTensor](../register_data_types/RegTensor.md)类型，例如RegTensor&lt;int32_t&gt;，由编译器自动推导，用户不需要填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。<br>注：mask会按照输入来筛选。 |

## 数据类型

**表 3**  数据类型组合情况

| src | dst |
| --- | --- |
| int4x2_t | int16_t、half、bfloat16_t |
| int8_t | int16_t、half、int32_t |
| uint8_t | uint16_t、half、uint32_t |
| fp4x2_e2m1_t | bfloat16_t |
| fp4x2_e1m2_t | bfloat16_t |
| hifloat8_t | half、float |
| fp8_e8m0_t | bfloat16_t |
| fp8_e5m2_t | float |
| fp8_e4m3fn_t | float |
| int16_t | int4x2_t、uint8_t、half、int32_t、uint32_t、float |
| uint16_t | uint8_t、uint32_t |
| half | int4x2_t、int8_t、uint8_t、hifloat8_t、int16_t、bfloat16_t、int32_t、float |
| bfloat16_t | fp4x2_e2m1_t、fp4x2_e1m2_t、fp8_e8m0_t、half、int32_t、float |
| int32_t | uint8_t、int16_t、float、int64_t |
| uint32_t | uint8_t、int16_t、uint16_t |
| float | hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、half、bfloat16_t、int32_t、int64_t |
| int64_t | int32_t、float |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 当目的操作数位宽比源操作数小时，再MaskReg和RegLayout作用下，目的操作数中的无效元素均为0。
- 该指令需配合SetCtrlSpr指令使用，通过设置寄存器的值控制Cast的不饱和模式和饱和模式。
- 舍入模式仅在可能导致精度损失且支持该舍入模式的转换中生效，具体支持情况请参考[关键特性说明](#关键特性说明)。
- 不同类型转换的不饱和与饱和模式的描述有所区别，全局或单指令生效配置方法也不同，具体内容参考[表4 不同类型转换场景的不饱和模式和饱和模式](#表4-不同类型转换场景的不饱和模式和饱和模式)、[表5 饱和模式全局或单指令生效配置表](#表5-饱和模式全局或单指令生效配置表)。
- 浮点数转浮点数：
    - 当输出类型float32时，只支持不饱和模式。
    - 不饱和模式：当输出类型为fp8_e4m3fn_t时，由于fp8_e4m3fn_t没有inf表示格式，所以输出为nan。
    - 饱和模式：当输出类型为fp8_e5m2_t/fp8_e4m3fn_t时，输入nan，默认输出为0。如果CTRL\[50\] = 1'b1，则输出为nan。
    - fp4x2_e2m1_t/fp4x2_e1m2_t数据类型没有inf和nan的定义。对于bfloat16到float4的数据类型转换，输入bfloat16类型的值为inf或超出fp4x2_e2m1_t/fp4x2_e1m2_t数据最值范围时，会返回对应符号的fp4x2_e2m1_t/fp4x2_e1m2_t最值；输入nan时，fp4x2_e2m1_t/fp4x2_e1m2_t输出0。
    - 对于fp8_e8m0_t类型：输入bfloat16_t +/-inf或绝对值超出fp8_e8m0_t类型最大值，则返回fp8_e8m0_t最大值0b11111110；输入bfloat16_t nan输出fp8_e8m0_t nan = 0b11111111。

- 整数转整数

    对于窄数据类型例如int16_t\(2Byte\)转宽数据类型uint32_t\(4Byte\)，只支持饱和模式，输入负数会被饱和成0。

- 特别地，int4x2_t/fp4x2_e2m1_t/fp4x2_e1m2_t和b16之间的转换，指令会以每2个元素为一对进行读写，大转小时mask有效位以偶数位为准。

    图[fp4x2_e2m1_t到bfloat16_t转换过程](#fig3)和图[bfloat16_t到fp4x2_e2m1_t转换过程](#fig4)展示了MaskReg和RegLayout同时作用时fp4x2_e2m1_t和bfloat16_t之间的转换。

    **图3**  fp4x2_e2m1_t到bfloat16_t转换过程<a id="fig3"></a>  
    
    ![](../../../../figures/reg_cast_1.png)

    **图4**  bfloat16_t到fp4x2_e2m1_t转换过程<a id="fig4"></a>  
    ![](../../../../figures/reg_cast_2.png)

## 关键特性说明

### 不同类型转换场景下的不饱和模式和饱和模式

<a id="表4-不同类型转换场景的不饱和模式和饱和模式"></a>
**表 4**  不同类型转换场景的不饱和模式和饱和模式

| 场景 | 不饱和模式 | 饱和模式 |
| --- | --- | --- |
| 浮点转整数 | 输入数据超过输出类型最值时，结果被截断为目标格式的数据宽度（保留最低有效位），例如输入half值为257，输出uint8_t值为1；输入为+/-inf时，则返回输出类型的对应最值；输入为nan时，返回0。 | 输入数据超过输出类型最值时，返回输出类型的对应最值，例如输入half值为257，输出uint8值为255，输入half值为-inf，输出uint8_t值为0；输入为nan时，返回0。 |
| 浮点转浮点 | 输入数据为nan时，输出为nan；输入+/-inf时，输出为+/-inf。 | 输入为nan时，输出为0；输入数据超过输出类型最值时，返回输出类型的对应最值。 |
| 整数转浮点 | 不支持不饱和模式 | 输入为nan时，输出为0；输入数据超过输出类型最值时，返回输出类型的对应最值。该场景默认饱和模式，无需配置。 |
| 整数转整数 | 输入数据会截断为目标数据宽度，例如，输入int32_t值为256，输出uint8_t值为0。 | 输入数据超出目标数据范围，会饱和为目标数据最值。 |

### 不饱和模式和饱和模式配置

<a id="表5-饱和模式全局或单指令生效配置表"></a>
**表 5**  饱和模式全局或单指令生效配置表

| 全局使能位 | 控制位 | 功能描述 |
| --- | --- | --- |
| CTRL[60] = 1'b0 | Reg矢量计算Cast API的trait模板参数中satMode设置为SatMode::NO_SAT。 | 单指令非饱和模式。 |
| CTRL[60] = 1'b0 | Reg矢量计算Cast API的trait模板参数中satMode设置为SatMode::SAT。 | 单指令饱和模式。 |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b1 | 全局非饱和模式（浮点数计算和浮点数精度转换）。 |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b0 | 全局饱和模式（浮点数计算和浮点数精度转换）。 |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b1 | 全局非饱和模式（浮点数转整数或整数转整数时的精度转换）。 |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b0 | 全局饱和模式（浮点数转整数或整数转整数时的精度转换）。 |

### 不同类型转换支持的CastTrait类型取值

<a id="表6-浮点转整数"></a>
**表 6**  浮点转整数

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| half | int4x2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | int8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | uint8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | int16_t | RegLayout::UNKNOWN | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | int32_t | RegLayout::UNKNOWN | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | int64_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |

<a id="表7-浮点转浮点"></a>
**表 7**  浮点转浮点

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| hifloat8_t | half | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| hifloat8_t | float | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| fp8_e4m3fn_t | float | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| fp8_e5m2_t | float | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| fp8_e8m0_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| fp4x2_e2m1_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| fp4x2_e1m2_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| half | hifloat8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_ROUND,  RoundMode::HYBRID |
| half | bfloat16_t | RegLayout::UNKNOWN | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | half | RegLayout::UNKNOWN | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | fp4x2_e2m1_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | fp4x2_e1m2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | fp8_e8m0_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| bfloat16_t | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| float | hifloat8_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_ROUND,  RoundMode::HYBRID |
| float | fp8_e4m3fn_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT |
| float | fp8_e5m2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT |
| float | half | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_ODD, RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | bfloat16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |

<a id="表8-整数转浮点"></a>
**表 8**  整数转浮点

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| int4x2_t | half | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int4x2_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int8_t | half | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint8_t | half | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int16_t | half | RegLayout::UNKNOWN | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| int16_t | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int32_t | float | RegLayout::UNKNOWN | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| int64_t | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |

<a id="表9-整数转整数"></a>
**表 9**  整数转整数

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| int4x2_t | int16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int8_t | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int8_t | int32_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint8_t | uint16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint8_t | uint32_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int16_t | int4x2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int16_t | uint8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int16_t | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int16_t | uint32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint16_t | uint8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint16_t | uint32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int32_t | uint8_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int32_t | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int32_t | uint16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int32_t | int64_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint32_t | uint8_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint32_t | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| uint32_t | uint16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |
| int64_t | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING | RoundMode::UNKNOWN |

### float转hifloat8_t转换规则

```
tmp2[31 : 0] = f32_src_data[i][31 : 0];
Ev = tmp2[30 : 23] - 8'b01111111, thr = tmp2[13 : 0];
if (Ev == 128 && tmp2[22 : 0] != 23'b0) then
    tmp3[7 : 0] = HiF8NAN(8'b10000000);
else if ((Ev == 128 && tmp2[22 : 0] == 23'b0) || (Ev > 15)) then
    tmp3[7 : 0] = (tmp2[31] == 1'b0) ? HiF8+inf(8'b01101111) : HiF8-inf(8'b11101111);
else if (Ev < -23) then
    tmp3[7 : 0] = HiF8ZERO (8'b00000000);
else if (Ev == -23) then
    if ((Half To Away Round) || (Hybrid Round && {1’b1, tmp2[22 : 10]} >= thr)) then
        tmp3[7 : 0] = (tmp2[31] == 1'b0) ? 8'b00000001 : 8'b10000001; // min subnormal
    else
        tmp3[7 : 0] = HiF8ZERO(8'b00000000);
    end if
else
    if (Ev == 0) then
        M = tmp2[22 : 20], T A_bit = tmp2[19], frac = tmp2[19 : 6];
    else if (Ev == ±1) then
        M = tmp2[22 : 20], T A_bit = tmp2[19], frac = tmp2[19 : 6];
    else if (Ev == ±[2, 3]) then
        M = tmp2[22 : 20], T A_bit = tmp2[19], frac = tmp2[19:6];
    else if (Ev == ±[4, 7]) then
        M = tmp2[22 : 21], T A_bit = tmp2[20], frac = tmp2[20 : 7];
    else if (Ev == ±[8, 15]) then
        M = tmp2[22], T A_bit = tmp2[21], frac = tmp2[21 : 8];
    else if (Ev == [-16, -22]) then
        M = Ev + 23, T A_bit = tmp2[22], frac = tmp2[22 : 9]; // subnormal
    end if
    if (Ev == ±[0, 3]) then
        if (T A_bit == 1’b1) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == ±[4, 15]) then
        if (HALF To Away Round && T A_bit == 1’b1) || (Hybrid Round && frac >= thr) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == [-16, -22]) then
        if ((Half To Away Round) && T A_bit == 1'b1) || (Hybrid Round && frac >= thr) then
            M_tmp = Ev + 23, Ev = Ev + 1, M = (Ev == -15) ? 0 : M_tmp;
        end if
    end if
    encode {tmp3[31], Ev, M} to tmp3[7 : 0] following HiF encoding rule;
end if
result[i][7 :0] = saturation(tmp[7 : 0]) according to control bit;
```

### half转hifloat8_t转换规则
```
tmp2[15 : 0] = f16_src_data[i][15 : 0];
Ev = tmp2[14 : 10] - 5'b01111, thr = {tmp2[0], 1’b1};
if (Ev == 16 && tmp2[9 : 0] != 10'b0) then
    tmp3[7 : 0] = HiF8NAN(8'b10000000);
else if (Ev == 16 && tmp2[9:0] == 10'b0) && (Ev > 15) then
    tmp3[7 : 0] = (tmp2[31] == 1'b0) ? HiF8+inf(8'b01101111) : HiF8-inf(8'b11101111);
else if (Ev < -23) then
    tmp3[7 : 0] = HiF8ZERO(8'b00000000);
else if (Ev == -23) then
    if ((Half To Away Round) || (Hybrid Round && {1’b1, tmp2[9]} >= thr)) then
        tmp3[7 : 0] = (tmp2[31] == 1'b0) ? 8'b00000001 : 8'b10000001; // min subnormal
    else
        tmp3[7 : 0] = HiF8ZERO(8'b00000000);
    end if
else
    if (Ev == 0) then
        M = tmp2[9 : 7], T A_bit = tmp2[6], frac = tmp2[6 : 5];
    else if (Ev == ±1) then
        M = tmp2[9 : 7], T A_bit = tmp2[6], frac = tmp2[6 : 5];
    else if (Ev == ±[2, 3]) then
        M = tmp2[9 : 7], T A_bit = tmp2[6], frac = tmp2[6 : 5];
    else if (Ev == ±[4, 7]) then
        M = tmp2[9 : 8], T A_bit = tmp2[7], frac = tmp2[7 : 6];
    else if (Ev == ±[8, 15]) then
        M = tmp2[9], T A_bit = tmp2[8], frac = tmp2[8 : 7];
    else if (Ev == [-16, -22]) then
        M = Ev + 23, T A_bit = tmp2[9], frac = tmp2[9 : 8]; // subnormal
    end if
    if (Ev == ±[0, 3]) then
        if (T A_bit == 1’b1) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == ±[4, 15]) then
        if (HALF To Away Round && T A_bit == 1’b1) || (Hybrid Round && frac >= thr) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == [-16, -22]) then
        if ((Half To Away Round) && T A_bit == 1'b1) || (Hybrid Round && frac >= thr) then
            M_tmp = Ev + 23, Ev = Ev + 1, M = (Ev == -15) ? 0 : M_tmp;
        end if
    end if
    encode {tmp3[31], Ev, M} to tmp3[7 : 0] following HiF encoding rule;
end if
result[i][7 : 0] = saturation(tmp3[7 : 0]) according to control bit; 
```

## 调用示例<a name="section642mcpsimp"></a>

-   场景1：位宽小转大，以half→int32_t为例

    ```cpp
    __simd_vf__ inline void CastVFF162S32(__ubuf__ half* xAddr, __ubuf__ int32_t* yAddr,
        uint32_t repeatTimes, uint16_t oneRepeatSize)
    {
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<int32_t, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::RegTensor<half> xReg;
        AscendC::Reg::RegTensor<int32_t> yReg;
        static constexpr AscendC::Reg::CastTrait castTrait = {
            AscendC::Reg::RegLayout::ZERO,
            AscendC::Reg::SatMode::UNKNOWN,
            AscendC::Reg::MaskMergeMode::ZEROING,
            AscendC::RoundMode::CAST_FLOOR
        };
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<half, AscendC::Reg::PostLiteral::POST_MODE_UPDATE,
                AscendC::Reg::LoadDist::DIST_UNPACK_B16>(xReg, xAddr, oneRepeatSize);
            AscendC::Reg::Cast<int32_t, half, castTrait>(yReg, xReg, mask);
            AscendC::Reg::StoreAlign<int32_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(
                yAddr, yReg, oneRepeatSize, mask);
        }
    }
    ```

-   场景2：位宽大转小，以float→int16_t为例

    ```cpp
    __simd_vf__ inline void CastVFF322S16(__ubuf__ float* xAddr, __ubuf__ int16_t* yAddr,
        uint32_t repeatTimes, uint16_t oneRepeatSize)
    {
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::RegTensor<float> xReg;
        AscendC::Reg::RegTensor<int16_t> yReg;
        static constexpr AscendC::Reg::CastTrait castTrait = {
            AscendC::Reg::RegLayout::ZERO,
            AscendC::Reg::SatMode::SAT,
            AscendC::Reg::MaskMergeMode::ZEROING,
            AscendC::RoundMode::CAST_ROUND
        };
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<float, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);
            AscendC::Reg::Cast<int16_t, float, castTrait>(yReg, xReg, mask);
            AscendC::Reg::StoreAlign<int16_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE,
                AscendC::Reg::StoreDist::DIST_PACK_B32>(yAddr, yReg, oneRepeatSize, mask);
        }
    }
    ```
