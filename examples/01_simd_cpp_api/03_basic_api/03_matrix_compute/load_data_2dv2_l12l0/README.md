# load_data_2dv2_l12l0样例

## 概述

本样例展示如何调用`LoadData`接口并配置`LoadData2DParamsV2`结构体参数（本样例中简称：`Load2Dv2`接口），将A / B矩阵从L1搬运到L0A / L0B。

样例覆盖int8_t、half、float三种输入数据类型，以及A / B矩阵转置和不转置输入场景。L0A数据排布为Nz格式，L0B数据排布为Zn格式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── load_data_2dv2_l12l0
│   ├── figures                          // 图示
│   ├── scripts
│   │   ├── gen_data.py                  // 输入数据和真值数据生成脚本
│   │   └── verify_result.py             // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                   // 编译工程文件
│   ├── data_utils.h                     // 数据读入写出函数
│   ├── load_data_2dv2_l12l0.asc         // Ascend C样例实现 & 调用样例
│   └── README.md                        // 样例说明文档
```

## 样例描述

### 1. 整体流程

样例整体流程如下：

```text
GM(ND) -> L1(Nz) -> L0A(Nz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
       │         │                  │          │
    DataCopy  Load2Dv2            Mmad        Fixpipe
```

**步骤详解**：

1. **GM -> L1**：使用`DataCopy`实现ND到Nz格式转换。
2. **L1 -> L0A / L0B**：使用`Load2Dv2`接口并配置`LoadData2DParamsV2`结构体参数，通过`ifTranspose`控制是否转置。
3. **矩阵乘**：使用`Mmad`接口执行矩阵乘法。
4. **L0C -> GM**：使用`Fixpipe`接口搬出结果。

本样例固定矩阵乘规格为`[m, n, k] = [40, 50, 70]`，输出C矩阵为`[40, 50]`，ND格式。核函数通过`scenarioNum`选择不同场景。

### 2. 场景关键差异

各场景的输入shape、转置标志和L1 -> L0搬运方式见[表1](#表1)。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：scenarioNum不同取值的含义</span></caption>
  <tr>
    <td ><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">输入数据类型</span></td>
    <td><span style="font-weight: bold;">输出数据类型</span></td>
    <td><span style="font-weight: bold;">输入shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">是否多搬多算</span></td>
    <td><span style="font-weight: bold;">L1 -> L0搬运方式</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2" >int8_t</td>
    <td rowspan="2" >int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>是</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="2" >half</td>
    <td rowspan="2" >float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td rowspan="2" >float</td>
    <td rowspan="2" >float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td rowspan="1" >int8_t</td>
    <td rowspan="1" >int32_t</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td>for循环 + <code>Load2Dv2</code></td>
  </tr>
</table>

下面按搬运模式说明场景关键差异。

**`Load2Dv2`不转置搬运：场景1 / 3 / 5**

- A矩阵不转置输入`[m, k]`，B矩阵转置输入`[n, k]`。
- L1 -> L0A / L0B均不需要转置，`loadDataParams.ifTranspose = false`。

**`Load2Dv2`转置搬运，单次调用：场景2 / 4 / 6**

- A矩阵转置输入`[k, m]`，B矩阵不转置输入`[k, n]`。
- L1 -> L0A / L0B均需要转置，`loadDataParams.ifTranspose = true`。
- 场景2中，int8_t转置单次搬运会导致m方向多搬多算脏数据分形，`Fixpipe`搬出时通过`fixpipeParams.mSize = m`只搬出有效区域。

**`Load2Dv2`转置搬运，for循环调用：场景7**

- 场景7与场景2同为int8_t转置输入。
- A矩阵采用for循环调用`Load2Dv2`，在写入L0A时跳过m方向尾部脏数据分形，使矩阵计算时没有额外脏数据分形参与。

### 3. 参数说明

后续代码和参数说明会反复使用分形和对齐相关变量，本节先集中定义这些概念。

- `fractalShape`：小分形的shape。B8 / B16 / B32输入数据类型的shape为`[16, 32 / sizeof(T)]`，其中`T`表示输入数据类型。本样例涉及的数据类型分形相关信息见[表2](#表2)。
- `fractalSize`：1个小分形包含的元素个数，具体见[表2](#表2)。
- `fractalNum`：当从L1 -> L0A / L0B需要转置时，`Load2Dv2`接口会按正方形矩阵转置。需要多个连续小分形合并为一个正方形矩阵时，`fractalNum`表示该正方形矩阵包含的小分形个数，具体见[表2](#表2)。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：不同数据类型分形相关信息</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center">[16, 32]</td>
    <td align="center">512</td>
    <td align="center">2</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center">[16, 16]</td>
    <td align="center">256</td>
    <td align="center">1</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
    <td align="center">[16, 8]</td>
    <td align="center">128</td>
    <td align="center">2</td>
  </tr>
</table>

- `CeilAlign`：向上对齐操作。例如`m = 30`时，`CeilAlign(30, 16) = 32`，表示将m轴向16对齐，对齐后m轴长度为32。

  ```cpp
  __aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
      return (size + alignValue - 1) / alignValue * alignValue;
  }
  ```

- `ceil_div`：向上取整除法，一般用于求解向上对齐后的循环次数。
- `mAlignValue`：m轴向`mAlignValue`对齐。例如`mAlignValue = 32`，代表m轴对齐到32；依次类推还有`nAlignValue`、`kaAlignValue`、`kbAlignValue`。
- `mAlignL1`和`mAlignL0`：A矩阵分别在L1和L0A上时，m轴对齐后的值。依次类推还有`nAlignL1`、`nAlignL0`、`kaAlignL1`、`kaAlignL0`、`kbAlignL1`、`kbAlignL0`。

### 4. 对齐要求

A / B矩阵在L1和L0上各轴的对齐要求不同，后续配置`Load2Dv2`、`Mmad`和`Fixpipe`参数时会用到这些值。[表3](#表3)、[表4](#表4)总结scenarioNum取1-6时A / B矩阵在L1和L0上的对齐要求；[表5](#表5)、[表6](#表6)总结scenarioNum取7时的对齐要求。

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：A、B矩阵在L1上各个轴对齐要求（L1排布格式为Nz）</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]</span></td>
    <td colspan="3" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td colspan="2" align="center">kaAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kaAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >mAlignValue = fractalShape[1]</td>
    <td colspan="1" align="center" >mAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td colspan="2" align="center">kbAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kbAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >nAlignValue = fractalShape[1]</td>
    <td colspan="1" align="center" >nAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]</span></td>
    <td colspan="3" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="表4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表4：A、B矩阵在L0上各个轴对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]，L1 -> L0A不需要转置</span></td>
    <td colspan="3" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置</span></td>
    <td colspan="2" align="center">kaAlignValue = fractalShape[1]</td>
    <td >kaAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1 -> L0B需要转置</span></td>
    <td colspan="2" align="center">kbAlignValue = fractalShape[1]</td>
      <td align="center">kbAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="2" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >nAlignValue = fractalShape[0]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]，L1 -> L0B不需要转置</span></td>
    <td colspan="3" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
</table>

当scenarioNum=7时，A矩阵用for循环 + `Load2Dv2`实现L1 -> L0A搬运，L0A只写入有效数据按分形对齐后的区域。

<a name="表5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表5：scenarioNum=7，A、B矩阵在L1上各个轴对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td align="center" >kaAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
    <tr>
    <td align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td align="center" >kbAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="表6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表6：scenarioNum=7，A、B矩阵在L0上各个轴对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置</span></td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1 -> L0B需要转置</span></td>
    <td align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
</table>


### 5. `Load2Dv2`

A / B矩阵在L1上均为Nz排布，在L0A、L0B上分别为Nz、Zn排布。L1 -> L0阶段调用`LoadData`接口并配置`LoadData2DParamsV2`结构体参数，本文简称为`Load2Dv2`。

阅读本章时可先按[表1](#表1)确定场景，再按[表3](#表3)-[表6](#表6)选择对齐值。`LoadData2DParamsV2`配置时重点关注以下参数：

- `ifTranspose`：控制L1 -> L0搬运时是否转置。A矩阵不转置输入`[m, k]`、B矩阵转置输入`[n, k]`时取`false`；A矩阵转置输入`[k, m]`、B矩阵不转置输入`[k, n]`时取`true`。
- `mStep`、`kStep`：分别表示本次搬运覆盖的row方向、col方向小分形个数。转置搬运时，B8 / B32需要按方型分形组合搬运，相关步长需满足`fractalNum`要求。
- `srcStride`、`dstStride`：分别表示L1源矩阵和L0目标矩阵row方向相邻小分形之间的间隔。
- `mStartPosition`、`kStartPosition`：表示本次搬运在L1源矩阵中的起始小分形位置。场景7中通过循环修改`mStartPosition`，分段搬运A矩阵。

#### 5.1. A矩阵 L1 -> L0A 不转置

L1 -> L0A不转置时，B8 / B16 / B32三种数据类型的参数配置基本相同，仅`fractalShape`不同，可参考[表2](#表2)。以int8_t为例展示图示。

<div align="center">
  <img src="figures/B8_A_l1_l0A_Load2dv2.png" width="800"><br>
  图1: int8_t数据类型下，L1 -> L0A不转置，调用Load2Dv2数据排布示意图
</div>

参数配置要点：

- `mStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[0])`，表示row方向搬运m轴对应的小分形个数。
- `kStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[1])`，表示col方向搬运k轴对应的小分形个数。
- `ifTranspose = false`，表示只完成L1 Nz到L0A Nz的排布搬运，不做转置。

```cpp
mAlignL1 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL1 = CeilAlign(k, fractalShape[1]); // 96
mAlignL0 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 96
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[0]); // 3
loadDataParams.kStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[1]); // 3
loadDataParams.srcStride = AscendC::Std::ceil_div(mAlignL1, fractalShape[0]); // 3
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

#### 5.2. A矩阵 L1 -> L0A 转置

L1 -> L0A需要转置时，`loadDataParams.ifTranspose = true`。转置时`Load2Dv2`会按方型分形组合完成小分形转置和L0A Nz排布写入；不同数据类型的方型分形组合方式不同，下面按B8 / B16 / B32分别说明。

##### 5.2.1. B8输入数据类型

B8输入数据类型分形为16 * 32，L1 -> L0转置时会按照方型将row方向的2个16 * 32分形拼成1个32 * 32方型进行转置。下面以int8_t为例说明单次调用和for循环调用两种方式。

**单次调用**

调用一次`Load2Dv2`完成L1 -> L0A搬运和转置的图示如下：

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_load2dv2.png" width="800"><br>
  图2: int8_t数据类型下，L1 -> L0A转置，调用一次Load2Dv2数据排布示意图
</div>

本样例`m = 40`，B8转置搬运时`mAlignL1 = CeilAlign(m, fractalShape[1]) = 64`，`mAlignL1 - m = 24 > 16`。单次调用`Load2Dv2`时，m方向会多搬1个无效分形；`Mmad`计算时需要设置`mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)`让该分形参与计算，最后由`Fixpipe`通过`fixpipeParams.mSize = m`只搬出有效区域。

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 96
mAlignL1 = CeilAlign(m, fractalShape[1]); // 64
mAlignL0 = CeilAlign(m, fractalShape[0] * fractalNum); // 64
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 96
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 6
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[1]); // 2
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 6
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**for循环调用**

for循环调用多次`Load2Dv2`完成L1 -> L0A搬运和转置的图示如下：

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_for_load2dv2.png" width="800"><br>
  图3: int8_t数据类型下，L1 -> L0A转置，for循环调用多次Load2Dv2数据排布示意图
</div>

for循环调用时，沿L1 row方向（A矩阵k轴）分段搬运，每次搬运k轴方向2个分形、m轴方向`AscendC::Std::ceil_div(mAlignL0, fractalShape[1])`个分形。`dstStride`按m方向有效数据向`fractalShape[0]`对齐配置，写入L0A时跳过转置多读的m方向脏数据分形，使`Mmad`计算时m方向没有额外脏数据分形参与。

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 96
mAlignL1 = CeilAlign(m, fractalShape[1]); // 64
mAlignL0 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 96
// 输入为int8类型，A矩阵[k,m]转置输入，L1 -> L0A需要转置
// for循环调用Load2Dv2，以k轴方向做for循环，每次循环在L1的k方向搬运2个分形，在L0A上跳过m方向尾脏数据分形，m方向多搬运数据不超过1个分形
uint16_t L0ALoopNum = AscendC::Std::ceil_div(kaAlignL0, fractalShape[0] * fractalNum); // 3
loadDataParams.mStep = INT8_M_STEP_ALIGN; // 2
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL0, fractalShape[1]); // 2
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 6
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = true;
uint32_t dstOffset = 0;
for (uint16_t loopIdx = 0; loopIdx < L0ALoopNum; ++loopIdx) {
    loadDataParams.mStartPosition = INT8_M_STEP_ALIGN * loopIdx;
    AscendC::LoadData(a2Local[dstOffset], a1Local, loadDataParams);
    dstOffset += CeilAlign(mAlignL0, fractalShape[0]) * fractalShape[1]; // 1536
}
```

##### 5.2.2. B16输入数据类型

B16输入数据类型分形为16 * 16，一个分形即一个方型，L1 -> L0转置时会按照小分形进行转置，调用一次`Load2Dv2`即可完成L1 -> L0A数据搬运及转置。本样例以half为例展示图示。

<div align="center">
  <img src="figures/B16_A_l1_l0A_trans_load2dv2.png" width="800"><br>
  图4: half数据类型下，L1 -> L0A转置，调用一次Load2Dv2数据排布示意图
</div>

参数配置要点：

- `mStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0])`，表示row方向搬运k轴对应的小分形个数。
- `kStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[1])`，表示col方向搬运m轴对应的小分形个数。
- `ifTranspose = true`，表示搬运到L0A时完成转置。

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 80
mAlignL1 = CeilAlign(m, fractalShape[1]); // 48
mAlignL0 = CeilAlign(m, fractalShape[0] * fractalNum); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 80
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[1]); // 3
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

##### 5.2.3. B32输入数据类型

B32输入数据类型分形为16 * 8，L1 -> L0转置时会按照方型将col方向的2个16 * 8分形拼成1个16 * 16方型进行转置。本样例以float为例展示图示。

<div align="center">
  <img src="figures/B32_A_l1_l0A_trans_load2dv2.png" width="800"><br>
  图5: float数据类型下，L1 -> L0A转置，调用一次Load2Dv2数据排布示意图
</div>

本样例`m = 40`。B32转置搬运时需要将col方向2个分形拼成方型，**`kStep`必须是2的倍数**，因此L1上col方向（m方向）会多读1个无效分形。写入L0A后，k方向尾部会多1个无效分形；由于L0A为Nz排布，`Mmad`配置`mmadParams.k = k`即可只让有效k轴数据参与计算。

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0]); // 80
mAlignL1 = CeilAlign(m, fractalShape[1] * fractalNum); // 48
mAlignL0 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1] * fractalNum); // 80
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[1]); // 6
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

#### 5.3. B矩阵 L1 -> L0B 不转置

L1 -> L0B不转置时，B8 / B16 / B32三种数据类型的参数配置基本相同，仅`fractalShape`不同，可参考[表2](#表2)。以float为例展示图示。

<div align="center">
  <img src="figures/B32_B_l1_l0B_load2dv2.png" width="1100"><br>
  图6: float数据类型下，L1 -> L0B不转置，调用一次Load2Dv2数据排布示意图
</div>

参数配置要点：

- `mStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[0])`，表示row方向搬运n轴对应的小分形个数。
- `kStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[1])`，表示col方向搬运k轴对应的小分形个数。
- `ifTranspose = false`，表示只完成L1 Nz到L0B Zn的排布搬运，不做转置。

```cpp
nAlignL1 = CeilAlign(n, fractalShape[0]); // 64
kbAlignL1 = CeilAlign(k, fractalShape[1]); // 72
kbAlignL0 = CeilAlign(k, fractalShape[1]); // 72
nAlignL0 = CeilAlign(n, fractalShape[0]); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[0]); // 4
loadDataParams.kStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[1]); // 9
loadDataParams.srcStride = AscendC::Std::ceil_div(nAlignL1, fractalShape[0]); // 4
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

#### 5.4. B矩阵 L1 -> L0B 转置

L1 -> L0B需要转置时，`loadDataParams.ifTranspose = true`。转置时`Load2Dv2`会按方型分形组合完成小分形转置和L0B Zn排布写入；不同数据类型的方型分形组合方式不同，下面按B8 / B16 / B32分别说明。

##### 5.4.1. B8输入数据类型

B8输入数据类型分形为16 * 32，L1 -> L0转置时会按照方型将row方向的2个16 * 32的分形拼成1个32 * 32的方型进行转置。本样例以int8_t为例展示图示。

<div align="center">
  <img src="figures/B8_B_l1_l0B_trans_load2dv2.png" width="1100"><br>
  图7: int8_t数据类型下，L1 -> L0B转置，调用一次Load2Dv2数据排布示意图
</div>

本样例`k = 70`。B8转置搬运时需要将row方向2个分形拼成方型，**`mStep`必须是2的倍数**，因此L1上row方向（k方向）会多读1个无效分形。

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 96
nAlignL1 = CeilAlign(n, fractalShape[1]); // 64
kbAlignL0 = CeilAlign(k, fractalShape[1]); // 96
nAlignL0 = CeilAlign(n, fractalShape[0] * fractalNum); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]); // 6
loadDataParams.kStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[1]); // 2
loadDataParams.srcStride = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]); // 6
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

##### 5.4.2. B16输入数据类型

B16输入数据类型分形为16 * 16，一个分形即一个方型，L1 -> L0转置时会按照小分形进行转置，调用一次`Load2Dv2`即可完成L1 -> L0B数据搬运及转置。本样例以half为例展示图示。

<div align="center">
  <img src="figures/B16_B_l1_l0B_trans_load2dv2.png" width="800"><br>
  图8: half数据类型下，L1 -> L0B转置，调用一次Load2Dv2数据排布示意图
</div>

参数配置要点：

- `mStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0])`，表示row方向搬运k轴对应的小分形个数。
- `kStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[1])`，表示col方向搬运n轴对应的小分形个数。
- `ifTranspose = true`，表示搬运到L0B时完成转置。

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 80
nAlignL1 = CeilAlign(n, fractalShape[1]); // 64
kbAlignL0 = CeilAlign(k, fractalShape[1]); // 80
nAlignL0 = CeilAlign(n, fractalShape[0] * fractalNum); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[1]); // 4
loadDataParams.srcStride = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

##### 5.4.3. B32输入数据类型

B32输入数据类型分形为16 * 8，L1 -> L0转置时会按照方型将col方向的2个16 * 8的分形拼成1个16 * 16的方型进行转置。本样例以float为例展示图示。

<div align="center">
  <img src="figures/B32_B_l1_l0B_trans_load2dv2.png" width="1100"><br>
  图9: float数据类型下，L1 -> L0B转置，调用一次Load2Dv2数据排布示意图
</div>

本样例`n = 50`。B32转置搬运时需要将col方向2个分形拼成方型，**`kStep`必须是2的倍数**，因此L1上col方向（n方向）会多读1个无效分形。写入L0B后，k方向尾部会多1个无效分形；由于L0B为Zn排布，`Mmad`配置`mmadParams.k = k`即可只让有效k轴数据参与计算。

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0]); // 80
nAlignL1 = CeilAlign(n, fractalShape[1] * fractalNum); // 64
kbAlignL0 = CeilAlign(k, fractalShape[1] * fractalNum); // 80
nAlignL0 = CeilAlign(n, fractalShape[0]); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[1]); // 8
loadDataParams.srcStride = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin
  ```
  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`-`7` | 场景编号：不同数据类型和转置组合 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
