# 原子操作边界值汇总<a name="ZH-CN_TOPIC_0000002608310699"></a>

## 概述<a name="section3674181114910"></a>

本节汇总介绍了原子操作接口在边界值输入下的两类输出结果，以下内容针对如下型号生效：

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->

原子操作接口在边界值输入下，有两类输出结果：

- INF/NAN模式（非饱和模式）

    计算结果是inf、-inf、nan的三类场景，按原样输出。

- 饱和模式

    通过接口[SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md)进行设置，饱和模式下：inf和-inf会被饱和为MAX和MIN进行输出，nan会被饱和为0进行输出。

## 原子操作接口边界值输入下的具体计算结果<a name="section8282511541"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表1**  SetAtomicAdd特殊值/边界值输入的计算结果

<a name="table15675934145714"></a>
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

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表2**  SetAtomicMax特殊值/边界值输入的计算结果

<a name="table57228341577"></a>
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

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表3**  SetAtomicMin特殊值/边界值输入的计算结果说明

<a name="table673793455714"></a>
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

注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。