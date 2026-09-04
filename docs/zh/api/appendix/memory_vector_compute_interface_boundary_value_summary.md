# Memory矢量计算接口边界值汇总<a name="ZH-CN_TOPIC_0000002533514388"></a>

## 概述<a name="section3674181114910"></a>

本节汇总介绍了Memory矢量计算接口在边界值输入下的两类输出结果，以下内容针对如下型号生效：

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->

Memory矢量计算接口在边界值输入下，有两类输出结果：

- INF/NAN模式（非饱和模式）

    计算结果是inf、-inf、nan的三类场景，按原样输出。

- 饱和模式

    通过接口[SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md)进行设置，饱和模式下：inf和-inf会被饱和为MAX和MIN进行输出，nan会被饱和为0进行输出。

注：

- float数据类型输入仅支持INF/NAN模式（即使配置饱和模式，输出结果也与INF/NAN模式输出结果一致）；half、int16_t及int32_t类型输入，默认输出INF/NAN模式的结果。
- **绝大部分场景下，INF/NAN模式的输出结果与IEEE754标准结果一致。如存在不一致的情况，会在对应接口处备注说明。**

## Memory矢量计算接口输出inf/nan的场景总结

**表1**  浮点数计算输出inf/nan的场景

<a name="table1645210351498"></a>

| 场景描述 | 浮点数计算输出结果 | 说明 |
| --- | --- | --- |
| **输入包含inf**<br>例外场景：<br>&bull; exp(-inf)=0<br>&bull; inf * 0 = nan<br>&bull; inf + (-inf) = nan | inf | inf区分+inf和-inf |
| **计算结果发生溢出**<br>例外情况：<br>&bull;非nan值（不包含0）除以0，比如Div/Sqrt/Reciprocal等指令<br>&bull; ln(0.0) = -inf；ln(-0.0) = -inf<br>&bull; rsqrt(0.0) = inf; rsqrt(-0.0) = -inf | inf | inf区分+inf和-inf |
| **输入包含nan** | nan | nan不区分正负 |
| **计算结果产生nan**<br>举例：<br>&bull; (-inf) - (-inf) = nan，比如Add/Sub/Reduce等指令<br>&bull; (-inf) + inf = nan，比如Add/Sub/Reduce等指令<br>&bull; 0 * (+/-inf) = nan，比如Mul等指令<br>&bull; (+/-inf)/(+/-inf) = nan<br>&bull; 0/0 = nan<br>&bull; ln/Sqrt/Rsqrt指令输入为负值 | nan | nan不区分正负 |

具体明细请参考如下各类API接口。

## 基础算术<a name="section12606472427"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表2**  Exp特殊值/边界值输入的计算结果

<a name="table864650154918"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 0.0 | 0.0 |
| half | 65504.0（MAX） | inf | **65504.0** |
| half | 0.0 | 1.0 | 1.0 |
| half | -0.0 | 1.0 | 1.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | inf | inf |
| float | 0.0 | 1.0 | 1.0 |
| float | -0.0 | 1.0 | 1.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表3**  Ln特殊值/边界值输入的计算结果

<a name="table10972164142410"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | **0.0** |
| half | 65504.0（MAX） | 11.09 | 11.09 |
| half | 0.0 | -inf | **-65504.0** |
| half | -0.0 | -inf | **-65504.0** |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | nan | **0.0** |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 88.72 | 88.72284 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表4**  Abs特殊值/边界值输入的计算结果

<a name="table1975185932512"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | inf | **65504.0** |
| float | -3.4028235e+38（MIN） | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | 0.0 |
| float | inf | inf | inf |
| float | -inf | inf | inf |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表5**  Reciprocal特殊值/边界值输入的计算结果

<a name="table1046894562620"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | -0.000015 | -0.000015 |
| half | 65504.0（MAX） | 0.000015 | 0.000015 |
| half | 0.0 | inf | **65504.0** |
| half | -0.0 | -inf | **-65504.0** |
| half | nan | nan | **0.0** |
| half | inf | 0.0 | 0.0 |
| half | -inf | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | -2.938736e-39 | -2.938736e-39 |
| float | 3.4028235e+38（MAX） | 2.938736e-39 | 2.938736e-39 |
| float | 0.0 | inf | inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | 0.0 | 0.0 |
| float | -inf | -0.0 | -0.0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表6**  Sqrt特殊值/边界值输入的计算结果

<a name="table1648943062718"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | **0.0** |
| half | 65504.0（MAX） | 256.0 | 256.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | -0.0 | -0.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | nan | **0.0** |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 1.8446744e+19 | 1.8446744e+19 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | -0.0 | -0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表7**  Rsqrt特殊值/边界值输入的计算结果

<a name="table12701922112818"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | **0.0** |
| half | 65504.0（MAX） | 0.003906 | 0.003906 |
| half | 0.0 | inf | **65504.0** |
| half | -0.0 | -inf | **-65504.0** |
| half | nan | nan | **0.0** |
| half | inf | 0.0 | 0.0 |
| half | -inf | nan | **0.0** |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 5.421011e-20 | 5.421011e-20 |
| float | 0.0 | inf | inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | 0.0 | 0.0 |
| float | -inf | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表8**  Relu特殊值/边界值输入的计算结果

<a name="table135522523284"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 0.0 | 0.0 |
| half | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |
| int32_t | -2147483648（MIN） | 0 | 0 |
| int32_t | 2147483647（MAX） | 2147483647 | 2147483647 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表9**  Add特殊值/边界值输入的计算结果

<a name="table071981613"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -inf | **-65504.0** |
| half | -65504.0（MIN） | 65504.0（MAX） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | inf | **65504.0** |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | nan | **0.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/-inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 0.0/-0.0 | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX）/inf | inf | inf |
| float | 3.4028235e+38（MAX） | 0.0/-0.0 | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | -inf | -inf | -inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0 | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | nan | nan |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | **-32768** |
| int16_t | -32768（MIN） | 32767（MAX） | -1 | -1 |
| int16_t | 32767（MAX） | 32767（MAX） | -2 | **32767** |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | **-2147483648** |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -1 | -1 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | -2 | **2147483647** |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表10**  Sub特殊值/边界值输入的计算结果

<a name="table12357151012179"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | **-65504.0** |
| half | -65504.0（MIN） | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | -inf | **-65504.0** |
| half | -65504.0（MIN） | -inf | inf | **65504.0** |
| half | 65504.0（MAX） | -65504.0（MIN） | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX） | 0.0 | 0.0 |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | inf | -inf | **-65504.0** |
| half | 65504.0（MAX） | -inf | inf | **65504.0** |
| half | 0.0/-0.0 | -65504.0（MIN） | 65504.0 | 65504.0 |
| half | 0.0/-0.0 | 65504.0（MAX） | -65504.0 | -65504.0 |
| half | 0.0/-0.0 | nan | nan | **0.0** |
| half | 0.0/-0.0 | inf | -inf | **-65504.0** |
| half | 0.0/-0.0 | -inf | inf | **65504.0** |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | -0.0 | -0.0 |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | nan | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | nan | **0.0** |
| half | inf | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/-inf | inf | **65504.0** |
| half | inf | nan/+inf | nan | **0.0** |
| half | -inf | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/inf | -inf | **-65504.0** |
| half | -inf | nan/-inf | nan | **0.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38（MIN） | 0.0/-0.0 | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | -inf | -inf |
| float | -3.4028235e+38（MIN） | -inf | inf | inf |
| float | 3.4028235e+38（MAX） | -3.4028235e+38（MIN） | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX） | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | 0.0/-0.0 | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | inf | -inf | -inf |
| float | 3.4028235e+38（MAX） | -inf | inf | inf |
| float | 0.0/-0.0 | -3.4028235e+38（MIN） | 3.4028235e+38 | 3.4028235e+38 |
| float | 0.0/-0.0 | 3.4028235e+38（MAX） | -3.4028235e+38 | -3.4028235e+38 |
| float | 0.0/-0.0 | nan | nan | nan |
| float | 0.0/-0.0 | inf | -inf | -inf |
| float | 0.0/-0.0 | -inf | inf | inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | -0.0 | -0.0 |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/-inf | inf | inf |
| float | inf | nan/inf | nan | nan |
| float | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/inf | -inf | -inf |
| float | -inf | nan/-inf | nan | nan |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | 0 |
| int16_t | -32768（MIN） | 32767（MAX） | 1 | **-32768** |
| int16_t | 32767（MAX） | -32768（MIN） | -1 | **32767** |
| int16_t | 32767（MAX） | 32767（MAX） | 0 | 0 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 0 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | 1 | **-2147483648** |
| int32_t | 2147483647（MAX） | -2147483648（MIN） | -1 | **2147483647** |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 0 | 0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表11**  Mul特殊值/边界值输入的计算结果

<a name="table89442511309"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | inf | **65504.0** |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | **-65504.0** |
| half | -65504.0（MIN） | 0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | -0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | -inf | **-65504.0** |
| half | -65504.0（MIN） | -inf | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX） | inf | **65504.0** |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | -0.0 | -0.0 | -0.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | inf | inf | **65504.0** |
| half | 65504.0（MAX） | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | **0.0** |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | **0.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | inf | **65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN） | inf | inf |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38（MIN） | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | -inf | -inf |
| float | -3.4028235e+38（MIN） | -inf | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX） | inf | inf |
| float | 3.4028235e+38（MAX） | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | inf | inf | inf |
| float | 3.4028235e+38（MAX） | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | **32767** |
| int16_t | -32768（MIN） | 32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 1 | **32767** |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | **2147483647** |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 1 | **2147483647** |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表12**  Div特殊值/边界值输入的计算结果

<a name="table118329223319"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | 1.0 | 1.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | -1.0 | -1.0 |
| half | -65504.0（MIN） | 0.0 | -inf | **-65504.0** |
| half | -65504.0（MIN） | -0.0 | inf | **65504.0** |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | -0.0 | -0.0 |
| half | -65504.0（MIN） | -inf | 0.0 | 0.0 |
| half | 65504.0（MAX） | -65504.0（MIN） | -1.0 | -1.0 |
| half | 65504.0（MAX） | 65504.0（MAX） | 1.0 | 1.0 |
| half | 65504.0（MAX） | 0.0 | inf | **65504.0** |
| half | 65504.0（MAX） | -0.0 | -inf | **-65504.0** |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | inf | 0.0 | 0.0 |
| half | 65504.0（MAX） | -inf | -0.0 | -0.0 |
| half | 0.0 | -65504.0（MIN）/-inf | -0.0 | -0.0 |
| half | 0.0 | 65504.0（MAX）/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0/-0.0/nan | nan | **0.0** |
| half | -0.0 | -65504.0（MIN）/-inf | 0.0 | 0.0 |
| half | -0.0 | 65504.0（MAX）/inf | -0.0 | -0.0 |
| half | -0.0 | 0.0/-0.0/nan | nan | **0.0** |
| half | nan | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | nan | **0.0** |
| half | inf | -65504.0（MIN）/-0.0 | -inf | **-65504.0** |
| half | inf | 65504.0（MAX）/0.0 | inf | **65504.0** |
| half | inf | nan/inf/-inf | nan | **0.0** |
| half | -inf | -65504.0（MIN）/-0.0 | inf | **65504.0** |
| half | -inf | 65504.0（MAX）/0.0 | -inf | **-65504.0** |
| half | -inf | nan/inf/-inf | nan | **0.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN） | 1.0 | 1.0 |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | -1.0 | -1.0 |
| float | -3.4028235e+38（MIN） | 0.0 | -inf | -inf |
| float | -3.4028235e+38（MIN） | -0.0 | inf | inf |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | -inf | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | -3.4028235e+38（MIN） | -1.0 | -1.0 |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX） | 1.0 | 1.0 |
| float | 3.4028235e+38（MAX） | 0.0 | inf | inf |
| float | 3.4028235e+38（MAX） | -0.0 | -inf | -inf |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | inf | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | -inf | -0.0 | -0.0 |
| float | 0.0 | -3.4028235e+38（MIN）/-inf | -0.0 | -0.0 |
| float | 0.0 | 3.4028235e+38（MAX）/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0/-0.0/nan | nan | nan |
| float | -0.0 | -3.4028235e+38（MIN）/-inf | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38（MAX）/inf | -0.0 | -0.0 |
| float | -0.0 | 0.0/-0.0/nan | nan | nan |
| float | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38（MIN）/-0.0 | -inf | -inf |
| float | inf | 3.4028235e+38（MAX）/0.0 | inf | inf |
| float | inf | nan/inf/-inf | nan | nan |
| float | -inf | -3.4028235e+38（MIN）/-0.0 | inf | inf |
| float | -inf | 3.4028235e+38（MAX）/0.0 | -inf | -inf |
| float | -inf | nan/inf/-inf | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表13**  Max特殊值/边界值输入的计算结果

<a name="table172607531318"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | -0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX）/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | inf | inf | **65504.0** |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf/-inf | inf | **65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/-inf | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | -3.4028235e+38（MIN） | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | -0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX）/0.0/-0.0/-inf | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | inf | inf | inf |
| float | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf/-inf | inf | inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | -32768 | -32768 |
| int16_t | -32768（MIN） | 32767（MAX） | 32767 | 32767 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | -2147483648 | -2147483648 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | 2147483647 | 2147483647 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 2147483647 | 2147483647 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表14**  Min特殊值/边界值输入的计算结果说明

<a name="table179652226325"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（寄存器非饱和模式） | dst的元素取值<br>（寄存器饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | -inf | -inf | **-65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | -0.0 | -0.0 | -0.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/inf | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | -inf | -inf | -inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX）/inf | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38（MAX） | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0/inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN）/32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 2147483647 | 2147483647 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表15**  Adds特殊值/边界值输入的计算结果

<a name="table15619175083314"></a>

| src/dst的数据类型 | src的元素取值 | scalarValue取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -inf | **-65504.0** |
| half | -65504.0（MIN） | 65504.0（MAX） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | inf | **65504.0** |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | nan | **0.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/-inf | -inf | -inf |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 0.0/-0.0 | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX）/inf | inf | inf |
| float | 3.4028235e+38（MAX） | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | -inf | -inf | -inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0 | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | nan | nan |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | **-32768** |
| int16_t | -32768（MIN） | 32767（MAX） | -1 | -1 |
| int16_t | 32767（MAX） | 32767（MAX） | -2 | **32767** |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | **-2147483648** |
| int32_t | -2147483648（MIN） | 2147483648（MAX） | -1 | -1 |
| int32_t | 2147483648（MAX） | 2147483648（MAX） | -2 | **2147483648** |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表16**  Muls特殊值/边界值输入的计算结果

<a name="table1510462203414"></a>

| src/dst的数据类型 | src的元素取值 | scalarValue取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | inf | **65504.0** |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | **-65504.0** |
| half | -65504.0（MIN） | 0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | -0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | -inf | **-65504.0** |
| half | -65504.0（MIN） | -inf | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX） | inf | **65504.0** |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | -0.0 | -0.0 | -0.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | inf | inf | **65504.0** |
| half | 65504.0（MAX） | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | **0.0** |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | **0.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | inf | **65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN） | inf | inf |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38（MIN） | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | -inf | -inf |
| float | -3.4028235e+38（MIN） | -inf | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX） | inf | inf |
| float | 3.4028235e+38（MAX） | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | inf | inf | inf |
| float | 3.4028235e+38（MAX） | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | **32767** |
| int16_t | -32768（MIN） | 32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 1 | **32767** |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | **2147483648** |
| int32_t | -2147483648（MIN） | 2147483648（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483648（MAX） | 2147483648（MAX） | 1 | **2147483648** |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表17**  Maxs特殊值/边界值输入的计算结果

<a name="table825085423412"></a>

| src/dst的数据类型 | src的元素取值 | scalarValue取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | -0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | inf | **65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX）/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | inf | inf | **65504.0** |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf/-inf | inf | **65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/-inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | 3.40282E+38 | 3.40282E+38 |
| float | -3.4028235e+38（MIN） | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | -0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | inf | inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX）/0.0/-0.0/-inf | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | inf | inf | inf |
| float | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf/-inf | inf | inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | -32768 | -32768 |
| int16_t | -32768（MIN） | 32767（MAX） | 32767 | 32767 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | -2147483648 | -2147483648 |
| int32_t | -2147483648（MIN） | 2147483648（MAX） | 2147483648 | 2147483648 |
| int32_t | 2147483648（MAX） | 2147483648（MAX） | 2147483648 | 2147483648 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表18**  Mins特殊值/边界值输入的计算结果

<a name="table714843013511"></a>

| src/dst的数据类型 | src的元素取值 | scalarValue取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | -inf | -inf | **-65504.0** |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | -0.0 | -0.0 | -0.0 |
| half | 65504.0（MAX） | nan | nan | **0.0** |
| half | 65504.0（MAX） | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | -inf | -inf | -inf |
| float | 3.4028235e+38（MAX） | 3.4028235e+38（MAX）/inf | 3.40E+38 | 3.40E+38 |
| float | 3.4028235e+38（MAX） | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38（MAX） | nan | nan | nan |
| float | 3.4028235e+38（MAX） | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0/inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN）/32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN）/2147483648（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483648（MAX） | 2147483648（MAX） | 2147483648 | 2147483648 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表19**  LeakyRelu特殊值/边界值输入的计算结果

<a name="table2399171617392"></a>

| src/dst的数据类型 | src的元素取值 | scalarValue取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | inf | **65504.0** |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | **-65504.0** |
| half | -65504.0（MIN） | 0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | -0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | **0.0** |
| half | -65504.0（MIN） | inf | -inf | **65504.0** |
| half | -65504.0（MIN） | -inf | inf | **-65504.0** |
| half | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | 65504.0 | 65504.0 |
| half | 0.0 | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| half | -0.0 | -65504.0（MIN）/-0.0 | 0.0 | 0.0 |
| half | -0.0 | 65504.0（MAX）/0.0 | -0.0 | -0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | nan | **0.0** |
| half | inf | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | inf | **65504.0** |
| half | -inf | -65504.0（MIN）/-inf | inf | **65504.0** |
| half | -inf | 65504.0（MAX）/inf | -inf | **-65504.0** |
| half | -inf | 0.0/-0.0/nan | nan | **0.0** |
| float | -3.4028235e+38（MIN） | -3.4028235e+38（MIN） | inf | inf |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38（MIN） | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38（MIN） | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan | nan |
| float | -3.4028235e+38（MIN） | inf | -inf | -inf |
| float | -3.4028235e+38（MIN） | -inf | inf | inf |
| float | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | 3.40E+38 | 3.40E+38 |
| float | 0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| float | -0.0 | -3.4028235e+38（MIN）/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38（MAX）/0.0 | -0.0 | -0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | inf | inf |
| float | -inf | -3.4028235e+38（MIN）/-inf | inf | inf |
| float | -inf | 3.4028235e+38（MAX）/inf | -inf | -inf |
| float | -inf | 0.0/-0.0/nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 复合计算<a name="section1978519408454"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表20**  Axpy特殊值/边界值输入的计算结果

<a name="table1819218568247"></a>

| src取值/scalarValue取值 | 中间阶段（src*scalarValue） | 中间阶段（饱和模式，仅输出为half数据类型时支持） | dst取值 | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +0 | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -0 | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +inf | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -inf | nan | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MIN | +inf | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | nan | nan | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +0 | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -0 | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +inf | nan | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -inf | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MAX | -inf | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | nan | nan | +0 |
| 使得src*scalarValue=MAX | MAX | MAX | +0 | MAX | MAX |
| 使得src*scalarValue=MAX | MAX | MAX | -0 | MAX | MAX |
| 使得src*scalarValue=MAX | MAX | MAX | +inf | +inf | MAX |
| 使得src*scalarValue=MAX | MAX | MAX | -inf | -inf | MIN |
| 使得src*scalarValue=MAX | MAX | MAX | MAX | MAX | MAX |
| 使得src*scalarValue=MAX | MAX | MAX | MIN | +0 | +0 |
| 使得src*scalarValue=MAX | MAX | MAX | nan | nan | +0 |
| 使得src*scalarValue=MIN | MIN | MIN | +0 | MIN | MIN |
| 使得src*scalarValue=MIN | MIN | MIN | -0 | MIN | MIN |
| 使得src*scalarValue=MIN | MIN | MIN | +inf | +inf | MAX |
| 使得src*scalarValue=MIN | MIN | MIN | -inf | -inf | MIN |
| 使得src*scalarValue=MIN | MIN | MIN | MAX | +0 | +0 |
| 使得src*scalarValue=MIN | MIN | MIN | MIN | -inf | MIN |
| 使得src*scalarValue=MIN | MIN | MIN | nan | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表21**  MulAddDst特殊值/边界值输入的计算结果

<a name="table1042313583406"></a>

| src0取值/src1取值 | 中间阶段（src0*src1） | 中间阶段（饱和模式，仅输出为half数据类型时支持） | dst取值 | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +0 | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -0 | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +inf | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -inf | nan | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | +inf | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MIN | +inf | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | nan | nan | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +0 | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -0 | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +inf | nan | MAX |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -inf | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MAX | -inf | +0 |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | -inf | MIN |
| 当源操作数与目的操作数数据类型一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>当源操作数与目的操作数数据类型不一致：<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | nan | nan | +0 |
| 使得src0*src1=MAX | MAX | MAX | +0 | MAX | MAX |
| 使得src0*src1=MAX | MAX | MAX | -0 | MAX | MAX |
| 使得src0*src1=MAX | MAX | MAX | +inf | +inf | MAX |
| 使得src0*src1=MAX | MAX | MAX | -inf | -inf | MIN |
| 使得src0*src1=MAX | MAX | MAX | MAX | MAX | MAX |
| 使得src0*src1=MAX | MAX | MAX | MIN | +0 | +0 |
| 使得src0*src1=MAX | MAX | MAX | nan | nan | +0 |
| 使得src0*src1=MIN | MIN | MIN | +0 | MIN | MIN |
| 使得src0*src1=MIN | MIN | MIN | -0 | MIN | MIN |
| 使得src0*src1=MIN | MIN | MIN | +inf | +inf | MAX |
| 使得src0*src1=MIN | MIN | MIN | -inf | -inf | MIN |
| 使得src0*src1=MIN | MIN | MIN | MAX | +0 | +0 |
| 使得src0*src1=MIN | MIN | MIN | MIN | -inf | MIN |
| 使得src0*src1=MIN | MIN | MIN | nan | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表22**  FusedMulAdd特殊值/边界值输入的计算结果

<a name="table1158211814515"></a>

| src0取值/dst取值 | 中间阶段（src0*dst） | 中间阶段（饱和模式，仅输出为half数据类型时支持） | src1取值 | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +0 | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -0 | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +inf | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -inf | nan | MIN |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MIN | +inf | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | nan | nan | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +0 | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -0 | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +inf | nan | MAX |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -inf | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MAX | -inf | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MIN | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | nan | nan | +0 |
| 使得src0*dst=MAX | MAX | MAX | +0 | MAX | MAX |
| 使得src0*dst=MAX | MAX | MAX | -0 | MAX | MAX |
| 使得src0*dst=MAX | MAX | MAX | +inf | +inf | MAX |
| 使得src0*dst=MAX | MAX | MAX | -inf | -inf | MIN |
| 使得src0*dst=MAX | MAX | MAX | MAX | MAX | MAX |
| 使得src0*dst=MAX | MAX | MAX | MIN | +0 | +0 |
| 使得src0*dst=MAX | MAX | MAX | nan | nan | +0 |
| 使得src0*dst=MIN | MIN | MIN | +0 | MIN | MIN |
| 使得src0*dst=MIN | MIN | MIN | -0 | MIN | MIN |
| 使得src0*dst=MIN | MIN | MIN | +inf | +inf | MAX |
| 使得src0*dst=MIN | MIN | MIN | -inf | -inf | MIN |
| 使得src0*dst=MIN | MIN | MIN | MAX | +0 | +0 |
| 使得src0*dst=MIN | MIN | MIN | MIN | -inf | MIN |
| 使得src0*dst=MIN | MIN | MIN | nan | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表23**  MulAddRelu特殊值/边界值输入的计算结果

<a name="table126194714473"></a>

| src0取值/dst取值 | 中间阶段1（src0*dst） | 中间阶段1（饱和模式，仅输出为half数据类型时支持） | src1取值 | 中间阶段2（INF/NAN模式） | 中间阶段2（饱和模式，仅输出为half数据类型时支持） | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf | +inf | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 | nan | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +0 | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -0 | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +inf | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -inf | nan | MIN | nan | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MAX | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MIN | +inf | +0 | +inf | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | nan | nan | +0 | nan | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +0 | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -0 | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +inf | nan | MAX | nan | MAX |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -inf | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MAX | -inf | +0 | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MIN | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | nan | nan | +0 | nan | +0 |
| 使得src0*dst=MAX | MAX | MAX | +0 | MAX | MAX | MAX | MAX |
| 使得src0*dst=MAX | MAX | MAX | -0 | MAX | MAX | MAX | MAX |
| 使得src0*dst=MAX | MAX | MAX | +inf | +inf | MAX | +inf | MAX |
| 使得src0*dst=MAX | MAX | MAX | -inf | -inf | MIN | +0 | +0 |
| 使得src0*dst=MAX | MAX | MAX | MAX | MAX | MAX | MAX | MAX |
| 使得src0*dst=MAX | MAX | MAX | MIN | +0 | +0 | +0 | +0 |
| 使得src0*dst=MAX | MAX | MAX | nan | nan | +0 | nan | +0 |
| 使得src0*dst=MIN | MIN | MIN | +0 | MIN | MIN | +0 | +0 |
| 使得src0*dst=MIN | MIN | MIN | -0 | MIN | MIN | +0 | +0 |
| 使得src0*dst=MIN | MIN | MIN | +inf | +inf | MAX | +inf | MAX |
| 使得src0*dst=MIN | MIN | MIN | -inf | -inf | MIN | +0 | +0 |
| 使得src0*dst=MIN | MIN | MIN | MAX | +0 | +0 | +0 | +0 |
| 使得src0*dst=MIN | MIN | MIN | MIN | -inf | MIN | +0 | +0 |
| 使得src0*dst=MIN | MIN | MIN | nan | nan | +0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX | nan | MAX |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN | nan | +0 |
| nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 | nan | +0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表24**  AddRelu特殊值/边界值输入的计算结果

<a name="table143801137164310"></a>

| src0取值/src1取值 | 中间阶段(src0+src1) | 中间阶段（饱和模式，仅输出为half数据类型时支持） | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- |
| +0/+0<br>+0/-0 | +0 | +0 | +0 | +0 |
| -0/-0 | -0 | -0 | +0 | +0 |
| +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | MAX | +inf | MAX |
| -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | MIN | +0 | +0 |
| 使得src0+src1=MAX | MAX | MAX | MAX | MAX |
| 使得src0+src1=MIN | MIN | MIN | +0 | +0 |
| nan/任意值<br>+inf/-inf | nan | +0 | nan | +0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表25**  SubRelu特殊值/边界值输入的计算结果

<a name="table7783335111811"></a>

| src0取值/src1取值 | 中间阶段(src0-src1) | 中间阶段（饱和模式，仅输出为half数据类型时支持） | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- |
| +0/+0<br>+0/-0 | +0 | +0 | +0 | +0 |
| -0/+0 | -0 | -0 | +0 | +0 |
| +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+0/-inf<br>-0/-inf<br>MAX/MIN<br>MAX/-inf<br>MIN/-inf | +inf | MAX | +inf | MAX |
| -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | MIN | +0 | +0 |
| 使得src0-src1=MAX | MAX | MAX | MAX | MAX |
| 使得src0-src1=MIN | MIN | MIN | +0 | +0 |
| nan/任意值<br>+inf/+inf<br>-inf/-inf | nan | +0 | nan | +0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表26**  AddReluCast特殊值/边界值输入的计算结果

<a name="table494414452367"></a>

| src0数据类型/src1数据类型/dst数据类型 | src0取值/src1取值 | 中间阶段1（src0+src1）（INF/NAN模式） | 中间阶段2（INF/NAN模式） | dst输出（INF/NAN模式） |
| --- | --- | --- | --- | --- |
| float/float/half | +0/+0<br>+0/-0 | +0 | +0 | +0 |
| float/float/half | -0/-0 | -0 | +0 | +0 |
| float/float/half | +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | +inf | +inf |
| float/float/half | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | +0 |
| float/float/half | 使得src0+src1=MAX | MAX | MAX | MAX |
| float/float/half | 使得src0+src1=MIN | MIN | +0 | +0 |
| float/float/half | nan/任意值<br>+inf/-inf | nan | nan | +0 |
| half/half/int8_t | +0/+0<br>+0/-0 | +0 | +0 | 0 |
| half/half/int8_t | -0/-0 | -0 | +0 | 0 |
| half/half/int8_t | +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | +inf | MAX |
| half/half/int8_t | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | 0 |
| half/half/int8_t | 使得src0+src1=MAX | MAX | MAX | MAX |
| half/half/int8_t | 使得src0+src1=MIN | MIN | +0 | 0 |
| half/half/int8_t | nan/任意值<br>+inf/-inf | nan | nan | 0 |
| int16_t/int16_t/int8_t | 0/0 | 0 | 0 | 0 |
| int16_t/int16_t/int8_t | MAX/MAX | MAX | MAX | MAX |
| int16_t/int16_t/int8_t | MIN/MIN | MIN | 0 | 0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表27**  SubReluCast特殊值/边界值输入的计算结果

<a name="table1418126172917"></a>

| src0数据类型/src1数据类型/dst数据类型 | src0取值/src1取值 | 中间阶段1（src0-src1）（INF/NAN模式） | 中间阶段2（INF/NAN模式） | dst输出（INF/NAN模式） |
| --- | --- | --- | --- | --- |
| float/float/half | +0/+0<br>+0/-0<br>-0/-0 | +0 | +0 | +0 |
| float/float/half | -0/+0 | -0 | +0 | +0 |
| float/float/half | +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+0/-inf<br>-0/-inf<br>MAX/MIN | +inf | +inf | +inf |
| float/float/half | -inf/+0<br>-inf/+0<br>-inf/MAX<br>-inf/MIN<br>+0/+inf<br>-0/+inf<br>MIN/MAX | -inf | +0 | +0 |
| float/float/half | 使得src0-src1=MAX | MAX | MAX | MAX |
| float/float/half | 使得src0-src1=MIN | MIN | +0 | +0 |
| float/float/half | nan/任意值<br>+inf/+inf<br>-inf/-inf | nan | nan | +0 |
| half/half/int8_t | +0/+0<br>+0/-0<br>-0/-0 | +0 | +0 | 0 |
| half/half/int8_t | -0/-0 | -0 | +0 | 0 |
| half/half/int8_t | +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+inf/0<br>+inf/-0<br>+0/-inf<br>-0/-inf<br>MAX/MIN | +inf | +inf | MAX |
| half/half/int8_t | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | 0 |
| half/half/int8_t | 使得src0-src1=MAX | MAX | MAX | MAX |
| half/half/int8_t | 使得src0-src1=MIN | MIN | +0 | 0 |
| half/half/int8_t | nan/任意值<br>+inf/+inf<br>-inf/-inf | nan | nan | 0 |
| int16_t/int16_t/int8_t | 0/0 | 0 | 0 | 0 |
| int16_t/int16_t/int8_t | MAX/MIN | MAX | MAX | MAX |
| int16_t/int16_t/int8_t | MIN/MAX | MIN | 0 | 0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表28**  MulCast特殊值/边界值输入的计算结果

<a name="table1698216295159"></a>

| src0取值/src1取值 | 中间阶段（src0*src1） | 中间阶段（饱和模式，仅输出为half数据类型时支持） | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | 0 | 0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | 0 | 0 |
| MAX/MAX<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | MAX |
| MAX/MIN<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | MIN |
| 使得src0*src1=MAX | MAX | MAX | MAX | MAX |
| 使得src0*src1=MIN | MIN | MIN | MIN | MIN |
| nan/任意数<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | 0 | 0 |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表29**  CastDequant特殊值/边界值输入的计算结果（MAX/MIN对应操作数对应数据类型下的最大值和最小值，scale=1，offset=0）

<a name="table18277108174810"></a>

| src数据类型/dst数据类型 | src取值 | dst输出（INF/NAN模式） | dst输出（饱和模式，仅输出为half数据类型时支持） |
| --- | --- | --- | --- |
| int32_t/half | 0 | 0 | 0 |
| int32_t/half | 65504 | MAX | MAX |
| int32_t/half | -65504 | MIN | MIN |
| int32_t/half | MAX | +inf | MAX |
| int32_t/half | MIN | -inf | MIN |
| int16_t/int8_t | 0 | 0 | / |
| int16_t/int8_t | MAX | MAX | / |
| int16_t/int8_t | MIN | MIN | / |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 比较与选择<a name="section829519484517"></a>

注：
- 下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。
- GetCmpMask为设置比较掩码寄存器的数据搬运操作，不涉及特殊值/边界值。

**表30**  Compare特殊值/边界值输入的计算结果

<a name="table1961220798"></a>

| src数据类型/src1数据类型 | cmpMode比较模式 | src0数据类型 | src1数据类型 | dst输出数据（INF/NAN模式） | dst输出数据（饱和模式） |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | /0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/0/1/1 |
| half/half | CMPMODE::NE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/0/1/1 |
| half/half | CMPMODE::NE | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| half/half | CMPMODE::NE | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::NE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/0/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/0/1/1 |
| float/float | CMPMODE::NE | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647（MAX） | -2147483648（MIN）/2147483647（MAX） | 0/1 | 0/1 |

注：

-   CMPMODE::NE模式中，在数据类型为half和float时，Compare接口计算结果存在与IEEE754理论结果不同：
    -   src0=nan，src1=MIN/MAX/+0/-0/nan/+inf/-inf时，实际结果：dst输出数据（INF/NAN模式和饱和模式）均为0/0/0/0/0/0/0，IEEE754理论结果：1/1/1/1/1/1/1。
    -   src0=MIN/MAX/+0/-0/nan/+inf/-inf，src1=nan时，实际结果：dst输出数据（INF/NAN模式和饱和模式）均为0/0/0/0/0/0/0，IEEE754理论结果：1/1/1/1/1/1/1。

-   在其余模式中，dst输出数据（INF/NAN模式）、dst输出数据（饱和模式）结果，与IEEE754理论结果一致。

**表31**  Compares特殊值/边界值输入的计算结果

<a name="table1126411512176"></a>

| src数据类型/src1数据类型 | cmpMode比较模式 | src0数据类型 | scalar数据类型 | dst输出数据（INF/NAN模式） | dst输出数据（饱和模式） |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | /0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/1/1/1 |
| half/half | CMPMODE::NE | +0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | -0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::NE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/0/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/0/1/1 |
| float/float | CMPMODE::NE | +0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | -0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647（MAX） | -2147483648（MIN）/2147483647（MAX） | 0/1 | 0/1 |

注：

-   CMPMODE::NE模式中，在数据类型为half和float时，Compares接口计算结果存在与IEEE754理论结果不同：
    -   src0=nan，scalar=MIN/MAX/+0/-0/nan/+inf/-inf时，实际结果：dst输出数据（INF/NAN模式和饱和模式）均为0/0/0/0/0/0/0，IEEE754理论结果：1/1/1/1/1/1/1。
    -   src0=MIN/MAX/+0/-0/nan/+inf/-inf，scalar=nan时，实际结果：dst输出数据（INF/NAN模式和饱和模式）均为0/0/0/0/0/0/0，IEEE754理论结果：1/1/1/1/1/1/1。

-   在其余模式中，dst输出数据（INF/NAN模式）、dst输出数据（饱和模式）结果，与IEEE754理论结果一致。

## 类型转换<a name="section3558104315715"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表32**  Cast特殊值/边界值输入的计算结果

<a name="table1225292414418"></a>

| src数据类型/dst数据类型 | src输入数据 | dst输出数据（INF/NAN模式） | dst输出数据（饱和模式，仅输出数据类型为half、bfloat16有效） |
| --- | --- | --- | --- |
| int4b_t/half | MIN | -8 | -8 |
| int4b_t/half | MAX | 7 | 7 |
| uint8_t/half | 0 | 0 | 0 |
| uint8_t/half | MAX | 255 | 255 |
| int8_t/half | MAX | 127 | 127 |
| int8_t/half | MIN | -128 | -128 |
| half/int4b_t | MAX | 7 | 7 |
| half/int4b_t | MIN | -8 | -8 |
| half/int4b_t | +0/-0 | 0 | 0 |
| half/int4b_t | nan | 0 | 0 |
| half/int4b_t | +inf | 7 | 7 |
| half/int4b_t | -inf | -8 | -8 |
| half/int8_t | MAX | 127 | 127 |
| half/int8_t | MIN | -128 | -128 |
| half/int8_t | +0/-0 | 0 | 0 |
| half/int8_t | nan | 0 | 0 |
| half/int8_t | +inf | 127 | 127 |
| half/int8_t | -inf | -128 | -128 |
| half/uint8_t | MAX | 255 | 255 |
| half/uint8_t | MIN | 0 | 0 |
| half/uint8_t | +0/-0 | 0 | 0 |
| half/uint8_t | nan | 0 | 0 |
| half/uint8_t | +inf | 255 | 255 |
| half/uint8_t | -inf | 0 | 0 |
| half/int16_t | MAX | 65504 | 65504 |
| half/int16_t | MIN | -65504 | -65504 |
| half/int16_t | +0/-0 | 0 | 0 |
| half/int16_t | nan | 0 | 0 |
| half/int16_t | +inf | 65504 | 65504 |
| half/int16_t | -inf | -65504 | -65504 |
| half/int32_t | MAX | 65504 | 65504 |
| half/int32_t | MIN | -65504 | -65504 |
| half/int32_t | +0/-0 | 0 | 0 |
| half/int32_t | nan | 0 | 0 |
| half/int32_t | +inf | 2147483647 | 2147483647 |
| half/int32_t | -inf | -2147483648 | -2147483648 |
| half/float | MAX | 65504 | 65504 |
| half/float | MIN | -65504 | -65504 |
| half/float | +0/-0 | 0 | 0 |
| half/float | nan | nan | nan |
| half/float | +inf | +inf | +inf |
| half/float | -inf | -inf | -inf |
| int16_t/half | MAX | 32768.00 | 32768.00 |
| int16_t/half | MIN | -32768.00 | -32768.00 |
| int16_t/half | 0 | +0 | +0 |
| int16_t/float | MAX | 32768.00 | 32768.00 |
| int16_t/float | MIN | -32768.00 | -32768.00 |
| int16_t/float | 0 | +0 | +0 |
| bfloat16_t/int32_t | 3.3895314e+38（MAX） | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -3.3895314e+38（MIN） | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | 0 | 0 | 0 |
| bfloat16_t/int32_t | +inf | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -inf | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | nan | 0 | 0 |
| bfloat16_t/float | 3.3895314e+38（MAX） | 3.3895314e+38 | 3.3895314e+38 |
| bfloat16_t/float | -3.3895314e+38（MIN） | -3.3895314e+38 | -3.3895314e+38 |
| bfloat16_t/float | 0 | 0 | 0 |
| bfloat16_t/float | +inf | +inf | +inf |
| bfloat16_t/float | -inf | -inf | -inf |
| bfloat16_t/float | nan | nan | nan |
| int32_t/int16_t | MAX | MAX | MAX |
| int32_t/int16_t | MIN | MIN | MIN |
| int32_t/int16_t | 0 | 0 | 0 |
| int32_t/half（参考CastDequant） | / | / | / |
| int32_t/float | 2147483647（MAX） | 2147483520.00 | 2147483520.00 |
| int32_t/float | -2147483648（MIN） | -2147483648.00 | -2147483648.00 |
| int32_t/int64_t | 2147483647（MAX） | 2147483647 | 2147483647 |
| int32_t/int64_t | -2147483648（MIN） | -2147483648 | -2147483648 |
| float/int16_t | 3.4028235e+38（MAX） | 32767 | 32767 |
| float/int16_t | -3.4028235e+38（MIN） | -32768 | -32768 |
| float/int16_t | 0 | 0 | 0 |
| float/int16_t | +inf | 32767 | 32767 |
| float/int16_t | -inf | -32768 | -32768 |
| float/int16_t | nan | 0 | 0 |
| float/bfloat16_t | 3.4028235e+38（MAX） | +inf | MAX |
| float/bfloat16_t | -3.4028235e+38（MIN） | -inf | MIN |
| float/bfloat16_t | +0 | +0 | +0 |
| float/bfloat16_t | -0 | -0 | -0 |
| float/bfloat16_t | +inf | +inf | MAX |
| float/bfloat16_t | -inf | -inf | MIN |
| float/bfloat16_t | nan | nan | 0 |
| float/half | 3.4028235e+38（MAX） | +inf | MAX |
| float/half | -3.4028235e+38（MIN） | -inf | MIN |
| float/half | 0 | 0 | 0 |
| float/half | +inf | +inf | MAX |
| float/half | -inf | -inf | MIN |
| float/half | nan | nan | 0 |
| float/int32_t | 3.4028235e+38（MAX） | 2147483647 | 2147483647 |
| float/int32_t | -3.4028235e+38（MIN） | -2147483648 | -2147483648 |
| float/int32_t | 0 | 0 | 0 |
| float/int32_t | +inf | 2147483647 | 2147483647 |
| float/int32_t | -inf | -2147483648 | -2147483648 |
| float/int32_t | nan | 0 | 0 |
| float/float | MAX | MAX | MAX |
| float/float | MIN | MIN | MIN |
| float/float | +0 | +0 | +0 |
| float/float | -0 | +0 | +0 |
| float/float | +inf | +inf | +inf |
| float/float | -inf | -inf | -inf |
| float/float | nan | nan | nan |
| float/int64_t | 3.4028235e+38（MAX） | 9.22337E+18 | 9.22337E+18 |
| float/int64_t | -3.4028235e+38（MIN） | -9.22337E+18 | -9.22337E+18 |
| float/int64_t | 0 | 0 | 0 |
| float/int64_t | +inf | 9.22337E+18 | 9.22337E+18 |
| float/int64_t | -inf | -9.22337E+18 | -9.22337E+18 |
| float/int64_t | nan | 0 | 0 |
| int64_t/int32_t | MAX | 2147483647 | 2147483647 |
| int64_t/int32_t | MIN | -2147483648 | -2147483648 |
| int64_t/float | MAX | 9223372036854775808.00（MAX） | 9223372036854775808.00（MAX） |
| int64_t/float | MIN | -9223372036854775808.00（MIN） | -9223372036854775808.00（MIN） |
| int16_t/int8_t | MAX | 127 | 127 |
| int16_t/int8_t | MIN | -128 | -128 |
| int16_t/uint8_t | MAX | 255 | 255 |
| int16_t/uint8_t | MIN | 0 | 0 |

注：float->float转换，-0会被转为+0，与IEEE754不一致。

## 归约计算<a name="section156745111398"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表33**  ReduceDataBlock<MAX\>特殊值/边界值输入的计算结果

<a name="table2883162019243"></a>

| src/dst的数据类型 | src（1个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -65504.0 | -65504.0 |
| half | 全为65504.0（MAX） | 65504.0 | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan交替 | nan | 0.0 |
| half | inf，-inf交替 | inf | 65504.0 |
| half | inf，nan交替 | nan | 0.0 |
| half | -inf，nan交替 | nan | 0.0 |
| half | 15个1.0加1个inf | inf | 65504.0 |
| half | 15个1.0加1个-inf | 1.0 | 1.0 |
| half | 15个1.0加1个nan | nan | 0.0 |
| float | 全为-3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| float | 全为3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | inf | inf |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 7个1.0加1个inf | inf | inf |
| float | 7个1.0加1个-inf | 1.0 | 1.0 |
| float | 7个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表34**  ReduceDataBlock<MIN\>特殊值/边界值输入的计算结果

<a name="table6602183015217"></a>

| src/dst的数据类型 | src（1个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -65504.0 | -65504.0 |
| half | 全为65504.0（MAX） | 65504.0 | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan交替 | nan | 0.0 |
| half | inf，-inf交替 | -inf | -65504 |
| half | inf，nan交替 | nan | 0.0 |
| half | -inf，nan交替 | nan | 0.0 |
| half | 15个1.0加1个inf | 1.0 | 1.0 |
| half | 15个1.0加1个-inf | -inf | -65504.0 |
| half | 15个1.0加1个nan | nan | 0.0 |
| float | 全为-3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| float | 全为3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | -inf | -inf |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 7个1.0加1个inf | 1.0 | 1.0 |
| float | 7个1.0加1个-inf | -inf | -inf |
| float | 7个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表35**  ReduceDataBlock<SUM\>特殊值/边界值输入的计算结果

<a name="table47791758122218"></a>

| src/dst的数据类型 | src（1个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -inf | -65504.0 |
| half | 全为65504.0（MAX） | inf | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan交替 | nan | 0.0 |
| half | inf，-inf交替 | nan | 0.0 |
| half | inf，nan交替 | nan | 0.0 |
| half | -inf，nan交替 | nan | 0.0 |
| half | 15个1.0加1个inf | inf | 65344.0 |
| half | 15个1.0加1个-inf | -inf | -65344.0 |
| half | 15个1.0加1个nan | nan | 14.0 |
| float | 全为-3.4028235e+38（MIN） | -inf | -inf |
| float | 全为3.4028235e+38（MAX） | inf | inf |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | nan | nan |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 7个1.0加1个inf | inf | inf |
| float | 7个1.0加1个-inf | -inf | -inf |
| float | 7个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表36**  ReduceRepeat<MAX\>特殊值/边界值输入的计算结果

<a name="table818992302819"></a>

| src/dst的数据类型 | src（8个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -65504.0 | -65504.0 |
| half | 全为65504.0（MAX） | 65504.0 | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan交替 | nan | 0.0 |
| half | inf，-inf交替 | inf | 65504.0 |
| half | inf，nan交替 | nan | 0.0 |
| half | -inf，nan交替 | nan | 0.0 |
| half | 127个1.0加1个inf | inf | 65504.0 |
| half | 127个1.0加1个-inf | 1 | 1.0 |
| half | 127个1.0加1个nan | nan | 0.0 |
| float | 全为-3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| float | 全为3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | inf | inf |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 63个1.0加1个inf | inf | inf |
| float | 63个1.0加1个-inf | 1 | 1 |
| float | 63个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表37**  ReduceRepeat<MIN\>特殊值/边界值输入的计算结果

<a name="table11721236104119"></a>

| src/dst的数据类型 | src（8个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -65504.0 | -65504.0 |
| half | 全为65504.0（MAX） | 65504.0 | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0 |
| half | 全为inf | inf | 65504 |
| half | 全为-inf | -inf | -65504 |
| half | inf，-inf，nan交替 | nan | 0 |
| half | inf，-inf交替 | -inf | -65504 |
| half | inf，nan交替 | nan | 0 |
| half | -inf，nan交替 | nan | 0 |
| half | 127个1.0加1个inf | 1 | 1 |
| half | 127个1.0加1个-inf | -inf | -65504 |
| half | 127个1.0加1个nan | nan | 0 |
| float | 全为-3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| float | 全为3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | -inf | -inf |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 63个1.0加1个inf | 1.0 | 1.0 |
| float | 63个1.0加1个-inf | -inf | -inf |
| float | 63个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表38**  ReduceRepeat<SUM\>特殊值/边界值输入的计算结果

<a name="table9281103820417"></a>

| src/dst的数据类型 | src（8个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -inf | -65504.0 |
| half | 全为65504.0（MAX） | inf | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan交替 | nan | 0.0 |
| half | inf，-inf交替 | nan | 0.0 |
| half | inf，nan交替 | nan | 0.0 |
| half | -inf，nan交替 | nan | 0.0 |
| half | 127个1.0加1个inf | inf | 65504.0 |
| half | 127个1.0加1个-inf | -inf | -65376.0 |
| half | 127个1.0加1个nan | nan | 126.0 |
| float | 全为-3.4028235e+38（MIN） | -inf | -inf |
| float | 全为3.4028235e+38（MAX） | inf | inf |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | nan | nan |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 63个1.0加1个inf | inf | inf |
| float | 63个1.0加1个-inf | -inf | -inf |
| float | 63个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表39**  ReduceMax特殊值/边界值输入的计算结果

<a name="table491948105210"></a>

| src/dst的数据类型 | src（8个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -65504.0 | -65504.0 |
| half | 全为65504.0（MAX） | 65504.0 | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan加253个1.0 | nan | 1.0 |
| half | inf，-inf加254个1.0 | inf | 65504.0 |
| half | inf，nan加254个1.0 | nan | 1.0 |
| half | -inf，nan加254个1.0 | nan | 1.0 |
| half | 255个1.0加一个inf | inf | 65504.0 |
| half | 255个1.0加一个-inf | 1.0 | 1.0 |
| half | 255个1.0加一个nan | nan | 1.0 |
| float | 全为-3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| float | 全为3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | inf | inf |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 63个1.0加1个inf | inf | inf |
| float | 63个1.0加1个-inf | 1.0 | 1.0 |
| float | 63个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表40**  ReduceMin特殊值/边界值输入的计算结果

<a name="table144621256001"></a>

| src/dst的数据类型 | src（8个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -65504.0 | -65504.0 |
| half | 全为65504.0（MAX） | 65504.0 | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan加253个1.0 | nan | 0.0 |
| half | inf，-inf加254个1.0 | -inf | -65504.0 |
| half | inf，nan加254个1.0 | nan | 0.0 |
| half | -inf，nan加254个1.0 | nan | 0.0 |
| half | 255个1.0加一个inf | 1.0 | 1.0 |
| half | 255个1.0加一个-inf | -inf | -65504.0 |
| half | 255个1.0加一个nan | nan | 0.0 |
| float | 全为-3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| float | 全为3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | -inf | -inf |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 63个1.0加1个inf | 1.0 | 1.0 |
| float | 63个1.0加1个-inf | -inf | -inf |
| float | 63个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表41**  ReduceSum特殊值/边界值输入的计算结果

<a name="table16600131416"></a>

| src/dst的数据类型 | src（8个DataBlock）的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | 全为-65504.0（MIN） | -inf | -65504.0 |
| half | 全为65504.0（MAX） | inf | 65504.0 |
| half | 全为0.0 | 0.0 | 0.0 |
| half | 全为-0.0 | -0.0 | -0.0 |
| half | 全为nan | nan | 0.0 |
| half | 全为inf | inf | 65504.0 |
| half | 全为-inf | -inf | -65504.0 |
| half | inf，-inf，nan加253个1.0 | nan | 252.0 |
| half | inf，-inf加254个1.0 | nan | 254.0 |
| half | inf，nan加254个1.0 | nan | 254.0 |
| half | -inf，nan加254个1.0 | nan | 254.0 |
| half | 255个1.0加一个inf | inf | 65504.0 |
| half | 255个1.0加一个-inf | -inf | -65248.0 |
| half | 255个1.0加一个nan | nan | 254.0 |
| float | 全为-3.4028235e+38（MIN） | -inf | -inf |
| float | 全为3.4028235e+38（MAX） | inf | inf |
| float | 全为0.0 | 0.0 | 0.0 |
| float | 全为-0.0 | -0.0 | -0.0 |
| float | 全为nan | nan | nan |
| float | 全为inf | inf | inf |
| float | 全为-inf | -inf | -inf |
| float | inf，-inf，nan交替 | nan | nan |
| float | inf，-inf交替 | nan | nan |
| float | inf，nan交替 | nan | nan |
| float | -inf，nan交替 | nan | nan |
| float | 63个1.0加1个inf | inf | inf |
| float | 63个1.0加1个-inf | -inf | -inf |
| float | 63个1.0加1个nan | nan | nan |

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 数据排布转换<a name="section33343814910"></a>

Transpose和TransDataTo5HD属于搬运类接口，不涉及特殊值/边界值。

## 数据填充<a name="section10713185619915"></a>

Brcb和Duplicate属于填充类接口，不涉及特殊值/边界值。

CreateVecIndex特殊值/边界值输入的计算结果和Adds接口一致。

## 排序组合<a name="section1342519199109"></a>

Sort32、MrgSort、GetMrgSortResult不涉及特殊值/边界值。

## 离散与聚合<a name="section14495123581018"></a>

Gather和Gatherb属于搬运类接口，不涉及特殊值/边界值。

## 掩码操作<a name="section0460049181018"></a>

SetMaskCount、SetMaskNorm、SetVectorMask和ResetMask为掩码配置接口，不涉及特殊值/边界值。

