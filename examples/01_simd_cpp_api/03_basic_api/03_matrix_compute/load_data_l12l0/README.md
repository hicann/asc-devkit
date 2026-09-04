# load_data_l12l0样例


## 概述

本样例展示B4 / B8 / B16 / B32输入数据类型下，A / B矩阵从GM到L1、L1到L0、矩阵乘和结果搬出的完整流程。

样例覆盖A / B矩阵转置和不转置输入组合，重点说明`LoadData`、`LoadDataWithTranspose`、`Mmad`和`Fixpipe`接口的使用方法。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── load_data_l12l0
│   ├── figures                     // 图示
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── load_data_l12l0.asc         // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

### 1. 整体流程

本样例固定矩阵乘规格为`[m, n, k] = [40, 50, 70]`，输出C矩阵为`[40, 50]`，ND格式。核函数名称为`KernelLoadDataL12L0`，通过`scenarioNum`选择不同场景。

一次完整的矩阵乘包含GM -> L1、L1 -> L0A / L0B、矩阵乘和L0C -> GM四个阶段，如下所示：

```text
GM(ND) -> L1(Nz/Zz) -> L0A(Zz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
       │            │                       │          │
    DataCopy     LoadData                 Mmad       Fixpipe
```

<div align="center">
  <img src="figures/cube.png" width="900"><br>
  图1：矩阵乘整体流程图
</div>

各阶段处理内容如下：

1. **GM -> L1**：调用`DataCopy`接口并配置`Nd2NzParams`结构体参数，将A / B矩阵从ND排布转换为Nz或Zz排布。
2. **L1 -> L0A / L0B**：根据数据类型、转置标志和L1排布，选择`LoadData（LoadData2DParams）`、`LoadDataWithTranspose`或`LoadData（LoadData3DParamsV2）`完成搬运和格式变换。为便于描述，下文分别简称为`Load2D`、`LoadDataWithTranspose`和`Load3Dv2`。
3. **矩阵乘**：调用`Mmad`接口执行矩阵乘法。
4. **L0C -> GM**：调用`Fixpipe`接口将结果C矩阵搬出到GM。

A / B矩阵在不同存储单元的数据排布格式见[表1](#表1)。其中L1常规场景为Nz排布；场景14中L1上A / B矩阵为Zz排布，用于展示float转置输入时通过`LoadDataWithTranspose`搬运的特殊方式。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：不同存储单元的数据排布格式</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">GM</span></td>
    <td align="center"><span style="font-weight: bold;">L1</span></td>
    <td align="center"><span style="font-weight: bold;">L0A</span></td>
    <td align="center"><span style="font-weight: bold;">L0B</span></td>
    <td align="center"><span style="font-weight: bold;">L0C</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">数据排布格式</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">Zz</td>
    <td align="center">Zn</td>
    <td align="center">Zz</td>
  </tr>
</table>

14个场景由输入数据类型、`isAtranspose`、`isBtranspose`、L1排布格式和L1 -> L0搬运方式组合得到。[表2](#表2)先按输入数据类型和是否需要转置说明L1 -> L0阶段可调用的接口。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：L1 -> L0，不同场景下可调用的<code>LoadData</code>指令</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A不转置输入[m, k]<br>L1 -> L0A不需要转置</span></td>
    <td align="center"><code>Load2D</code>、<code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>、<code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>、<code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>、<code>Load3Dv2</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">A转置输入[k, m]<br>L1 -> L0A需要转置</span></td>
    <td align="center">不支持</td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load2D</code>、<code>Load3Dv2</code>、<code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load3Dv2</code>、<br><code>LoadDataWithTranspose</code>（L1上数据排布为Zz）</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B不转置输入[k, n]<br>L1 -> L0B需要转置</span></td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load2D</code>、<code>Load3Dv2</code>、<code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load3Dv2</code>、<br><code>LoadDataWithTranspose</code>（L1上数据排布为Zz）</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B转置输入[n, k]<br>L1 -> L0B不需要转置</span></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
  </tr>
</table>

`loadDataMode`是本文用于描述L1 -> L0搬运方式的统称，不是Ascend C接口参数。代码中A矩阵和B矩阵分别通过模板参数`loadData2AMode`、`loadData2BMode`选择L1 -> L0搬运方式：

- `0`：使用`Load2D`，即`LoadData（LoadData2DParams）`。
- `1`：使用`LoadDataWithTranspose`。
- `2`：使用`Load3Dv2`，即`LoadData（LoadData3DParamsV2）`。

### 2. 场景关键差异

各场景的输入shape、转置标志和L1 -> L0搬运方式见[表3](#表3)。

注：scenarioNum取值1到13时，L1上A、B矩阵均为Nz排布；scenarioNum=14时，L1上A、B矩阵均为Zz排布。

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：scenarioNum不同取值的含义</span></caption>
  <tr>
    <td ><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">输入数据类型</span></td>
    <td><span style="font-weight: bold;">输出数据类型</span></td>
    <td><span style="font-weight: bold;">输入shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">A矩阵调用<code>LoadData</code>指令类型</span></td>
    <td><span style="font-weight: bold;">B矩阵调用<code>LoadData</code>指令类型</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2" >int4_t</td>
    <td rowspan="2" >int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [40, 70]<br>B [70, 50]</td>
    <td>false</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="3" >int8_t</td>
    <td rowspan="3" >int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td rowspan="5" >half</td>
    <td rowspan="5" >float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">8</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">9</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">10</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load3Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">11</span></td>
    <td rowspan="3" >float</td>
    <td rowspan="3" >float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">12</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">13</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load3Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">14</span></td>
    <td rowspan="1" >float</td>
    <td rowspan="1" >float</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
</table>

本节按搬运模式说明关键差异。

**`Load2D`不转置搬运：场景1 / 3 / 6 / 11**

- A矩阵不转置输入`[m, k]`，使用`Load2D`搬运到L0A，沿m方向for循环，一次搬运k轴方向上的`AscendC::Std::ceil_div(k, fractalShape[1])`个分形。
- B矩阵转置输入`[n, k]`，使用`Load2D`搬运到L0B，沿k方向for循环，一次搬运n方向上的`AscendC::Std::ceil_div(n, fractalShape[0])`个分形。
- A / B矩阵均通过配置`srcStride`、`dstGap`等参数完成L1 -> L0搬运及大分形排布格式变化。

**A侧`Load3Dv2`不转置搬运：场景2 / 4 / 7 / 12**

- A矩阵不转置输入`[m, k]`，使用`Load3Dv2`从L1搬运到L0A，将L1上的Nz排布转换为L0A需要的Zz排布。
- `Load3Dv2`将N配置为1，卷积核width和height配置为1，padding配置为0，滑动步长配置为1，卷积核膨胀系数配置为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布。
- A矩阵通过配置`l1H`、`l1W`、`kExtension`、`mExtension`等参数完成搬运。
- B矩阵按场景使用`LoadDataWithTranspose`或`Load2D`：场景2中B矩阵不转置输入`[k, n]`，通过`LoadDataWithTranspose`完成转置搬运；场景4 / 7 / 12中B矩阵转置输入`[n, k]`，通过`Load2D`完成不转置搬运。

**`LoadDataWithTranspose`转置搬运：场景5 / 9 / 14**

- 场景5 / 9中，L1上A / B矩阵均为Nz排布，A矩阵转置输入`[k, m]`，B矩阵不转置输入`[k, n]`，A / B矩阵均使用`LoadDataWithTranspose`搬运并完成转置。
- 场景14中，L1上A / B矩阵均为Zz排布，A / B矩阵均使用`LoadDataWithTranspose`完成L1 -> L0搬运，搬运过程中完成转置。

**`Load2D`转置搬运：场景8**

- A / B矩阵均使用`Load2D`，并配置`ifTranspose = true`。
- A矩阵沿m方向for循环，B矩阵沿k方向for循环；一次搬运的分形个数与`Load2D`不转置搬运场景相同。
- 在L1 -> L0搬运过程中，`Load2D`通过`ifTranspose = true`完成小分形转置和大分形排布格式变化。

**`Load3Dv2`转置搬运：场景10 / 13**

- A / B矩阵均使用`Load3Dv2`。
- A矩阵通过配置`enTranspose`、`l1H`、`l1W`、`kExtension`、`mExtension`等参数完成L1 -> L0A转置搬运。
- B矩阵调用`Load3Dv2`时默认开启转置，不需要配置`enTranspose`，其他配置与A矩阵搬运类似。

### 3. 参数说明

后续代码和参数说明会反复使用分形和对齐相关变量，本节先集中定义这些概念。

- `fractalShape`：小分形的shape。B8 / B16 / B32输入数据类型的shape为`[16, 32 / sizeof(T)]`，其中`T`表示输入数据类型；B4输入数据类型的shape为`[16, 64]`。本样例涉及的数据类型分形相关信息见[表4](#表4)。
- `fractalSize`：1个小分形包含的元素个数，具体见[表4](#表4)。
- `fractalNum`：当从L1 -> L0A / L0B需要转置时，搬运接口会按正方形矩阵转置。需要多个连续小分形合并为一个正方形矩阵时，`fractalNum`表示该正方形矩阵包含的小分形个数，具体见[表4](#表4)。

<a name="表4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表4：不同数据类型分形相关信息</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center">[16, 64]</td>
    <td align="center">1024</td>
    <td align="center">4</td>
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
- `mAlignValue`：m轴向`mAlignValue`对齐。例如`mAlignValue = 32`，代表m轴对齐到32；依次类推还有`nAlignValue`、`kAlignValue`。
- `mAlignL1`和`mAlignL0`：A矩阵分别在L1和L0A上时，m轴对齐后的值。依次类推还有`kAlignL1`、`kAlignL0`、`nAlignL1`、`nAlignL0`。
- `srcoffset`和`dstoffset`：在L1上，A / B矩阵外轴方向每循环一次时，LocalTensor的地址偏移量；在L0A / L0B上，A / B矩阵外轴方向每循环一次时，LocalTensor的地址偏移量。

### 4. 对齐要求

注意，为了方便理解，本次样例全部默认将A矩阵的m轴和B矩阵的k轴作为外轴循环，暂不考虑将m轴与k轴中较长轴作为外轴的场景。

另外，A、B矩阵在L1和L0上在row和col方向上对齐的要求也不相同，现将[表3](#表3)中scenarioNum取1-13对应的13种场景（L1上的排布格式为Nz）的对齐要求总结如[表5](#表5)、[表6](#表6)所示：

<a name="表5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表5：A、B矩阵在L1上各个轴的对齐要求（L1排布格式为Nz）</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="表6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表6：A、B矩阵在L0上各个轴的对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]，L1 -> L0A不需要转置</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
    <td >kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1 -> L0B需要转置</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
      <td align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]，L1 -> L0B不需要转置</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
</table>

特别的，当scenarioNum=14时，L1上的排布格式为Zz，A、B矩阵在L1和L0上在高度和宽度方向上对齐的要求如[表7](#表7)。

<a name="表7"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表7：scenarioNum=14，A、B矩阵在L1（Zz排布）和L0上的对齐要求</span></caption>
  <tr>
    <td align="center"><span style="font-weight: bold;">矩阵输入</span></td>
    <td align="center"><span style="font-weight: bold;">L1（Zz排布）</span></td>
    <td align="center"><span style="font-weight: bold;">L0</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span><br>float (fractalNum=2)</td>
    <td align="center">kAlignValue = fractalShape[0]<br>mAlignValue = fractalShape[1] * fractalNum</td>
    <td align="center">mAlignValue = fractalShape[0]<br>kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span><br>float (fractalNum=2)</td>
    <td align="center">kAlignValue = fractalShape[0]<br>nAlignValue = fractalShape[1] * fractalNum</td>
    <td align="center">kAlignValue = fractalShape[1] * fractalNum<br>nAlignValue = fractalShape[0]</td>
  </tr>
</table>

### 5. GM到L1（`DataCopy`）

本节说明如何调用`DataCopy`接口并配置`Nd2NzParams`，将A / B矩阵从GM上的ND排布搬运到L1。先根据场景判断L1目标排布：

- **场景1-13**：A / B矩阵搬运到L1后均为Nz排布，属于常规ND -> Nz搬运。
- **场景14**：A / B矩阵搬运到L1后均为Zz排布，用于后续通过`LoadDataWithTranspose`完成float转置搬运。

`Nd2NzParams`配置时重点关注以下参数：

- `nValue`、`dValue`：源ND矩阵的逻辑shape。
- `dstNzC0Stride`：L1上Nz矩阵row方向对齐后的长度，单位为32B。
- `ndNum`、`srcNdMatrixStride`、`dstNzMatrixStride`：仅在构造Zz排布时需要按高度轴切分多个ND矩阵。

#### 5.1. A矩阵GM -> L1

A矩阵有两类GM输入shape：不转置输入`[m, k]`和转置输入`[k, m]`。场景1-13搬运到L1后为Nz排布；场景14需要构造Zz排布。不同数据类型的主要差异是`dstNzC0Stride`取值，具体可参考[表5](#表5)和[表7](#表7)。

##### 5.1.1. A矩阵GM上输入为[m, k]

A矩阵GM输入不转置`[m, k]`时，按一个ND矩阵搬运到L1并转换为Nz排布。以下以half输入为例。

<div align="center">
  <img src="figures/GM_L1_FP16_A_input_m_k_to_Nz.png" width="600"><br>
  图2：A矩阵不转置输入([m,k])，half数据类型下，GM -> L1数据排布示意
</div>

参数配置要点：

- `nValue = m`，`dValue = k`，对应源ND矩阵shape。
- `dstNzC0Stride = CeilAlign(m, fractalShape[0])`，表示L1上Nz矩阵row方向对齐后的长度，单位为32B。
- `ndNum = 1`，表示源数据按一个ND矩阵搬运。

```cpp
nd2nzA1Params.ndNum = 1;
nd2nzA1Params.nValue = m;
nd2nzA1Params.dValue = k;
nd2nzA1Params.srcNdMatrixStride = 0;
nd2nzA1Params.srcDValue = k;

// 以下这个参数取A矩阵在L1上，row方向的对齐后的长度
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]);

nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = 0;
```

##### 5.1.2. A矩阵GM上输入为[k, m]

**1. L1上的格式为Nz**

A矩阵GM输入转置`[k, m]`且L1目标排布为Nz时，仍按一个ND矩阵搬运。以下以float输入为例。

<div align="center">
  <img src="figures/GM_L1_FP32_A_transInput_k_m_to_Nz.png" width="500"><br>
  图3：A矩阵转置输入，float数据类型下，GM -> L1，ND -> Nz
</div>

参数配置要点：

- `nValue = k`，`dValue = m`，对应源ND矩阵shape。
- `dstNzC0Stride = CeilAlign(k, fractalShape[0])`，表示L1上Nz矩阵row方向对齐后的长度，单位为32B。
- `ndNum = 1`，表示源数据按一个ND矩阵搬运。

```cpp
nd2nzA1Params.ndNum = 1;
nd2nzA1Params.nValue = k;
nd2nzA1Params.dValue = m;
nd2nzA1Params.srcNdMatrixStride = 0;
nd2nzA1Params.srcDValue = m;
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = 0;
// 以下这个参数取A矩阵在L1上，row方向的对齐后的长度
if constexpr (AscendC::IsSameType<T, float>::value) {
  nd2nzA1Params.dstNzC0Stride = CeilAlign(k, fractalShape[0]);
}
```

**2. L1上的格式为Zz**

A矩阵GM输入转置`[k, m]`且L1目标排布为Zz时，需要将源ND矩阵沿高度轴按`fractalShape[0]`切分成多个小ND矩阵，再通过`DataCopy`搬运成多个只有1个高度分形的Nz矩阵，等效得到Zz排布。

<div align="center">
  <img src="figures/GM_L1_FP32_A_inputTrans_k_m_to_Zz.png" width="600"><br>
  图4：A矩阵转置输入，float数据类型下，GM -> L1，ND -> Zz
</div>

场景14中，float转置输入若使用`LoadDataWithTranspose`完成L1 -> L0A转置搬运，L1上的A矩阵需要满足Zz排布。因此GM -> L1阶段需要通过以下配置构造Zz：

- `ndNum = AscendC::Std::ceil_div(k, fractalShape[0])`，表示沿k轴按16行切分。
- `nValue = fractalShape[0]`，每个小ND矩阵高度为16。
- `dstNzMatrixStride = fractalShape[0] * CeilAlign(m, fractalShape[1] * fractalNum)`，控制相邻小ND矩阵搬运后的写入间隔。

```cpp
nd2nzA1Params.ndNum = AscendC::Std::ceil_div(k, fractalShape[0]);
nd2nzA1Params.nValue = fractalShape[0];
nd2nzA1Params.dValue = m;
nd2nzA1Params.srcNdMatrixStride = fractalShape[0] * m;
nd2nzA1Params.srcDValue = m;
nd2nzA1Params.dstNzC0Stride = fractalShape[0];
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = fractalShape[0] * CeilAlign(m, fractalShape[1] * fractalNum);
```

#### 5.2. B矩阵GM -> L1

B矩阵有两类GM输入shape：不转置输入`[k, n]`和转置输入`[n, k]`。场景1-13搬运到L1后为Nz排布；场景14需要构造Zz排布。不同数据类型的主要差异是`dstNzC0Stride`取值，具体可参考[表5](#表5)和[表7](#表7)。

##### 5.2.1. B矩阵GM上输入为[k, n]

**1. L1上的格式为Nz**

B矩阵GM输入不转置`[k, n]`且L1目标排布为Nz时，按一个ND矩阵搬运到L1并转换为Nz排布。以下以float输入为例。

<div align="center">
  <img src="figures/GM_L1_FP32_B_input_k_n_to_Nz.png" width="500"><br>
  图5：B矩阵不转置输入，float数据类型下，GM -> L1，ND -> Nz
</div>

参数配置要点：

- `nValue = k`，`dValue = n`，对应源ND矩阵shape。
- `dstNzC0Stride = CeilAlign(k, fractalShape[0])`，表示L1上Nz矩阵row方向对齐后的长度，单位为32B。
- `ndNum = 1`，表示源数据按一个ND矩阵搬运。

```cpp
nd2nzB1Params.ndNum = 1;
nd2nzB1Params.nValue = k;
nd2nzB1Params.dValue = n;
nd2nzB1Params.srcNdMatrixStride = 0;
nd2nzB1Params.srcDValue = n;
nd2nzB1Params.dstNzNStride = 1;
nd2nzB1Params.dstNzMatrixStride = 0;
nd2nzB1Params.dstNzC0Stride = CeilAlign(k, fractalShape[0]);
```

**2. L1上的格式为Zz**

B矩阵GM输入不转置`[k, n]`且L1目标排布为Zz时，与A矩阵构造Zz排布的思路相同：将源ND矩阵沿高度轴按`fractalShape[0]`切分成多个小ND矩阵，再搬运成等效Zz排布。

<div align="center">
  <img src="figures/GM_L1_FP32_B_input_k_n_to_Zz.png" width="600"><br>
  图6：B矩阵不转置输入，float数据类型下，GM -> L1，ND -> Zz
</div>

场景14中，float不转置输入若使用`LoadDataWithTranspose`完成L1 -> L0B转置搬运，L1上的B矩阵需要满足Zz排布。因此GM -> L1阶段需要通过以下配置构造Zz：

- `ndNum = AscendC::Std::ceil_div(k, fractalShape[0])`，表示沿k轴按16行切分。
- `nValue = fractalShape[0]`，每个小ND矩阵高度为16。
- `dstNzMatrixStride = fractalShape[0] * CeilAlign(n, fractalShape[1] * fractalNum)`，控制相邻小ND矩阵搬运后的写入间隔。

```cpp
nd2nzB1Params.ndNum = AscendC::Std::ceil_div(k, fractalShape[0]);
nd2nzB1Params.nValue = fractalShape[0];
nd2nzB1Params.dValue = n;
nd2nzB1Params.srcNdMatrixStride = fractalShape[0] * n;
nd2nzB1Params.srcDValue = n;
nd2nzB1Params.dstNzC0Stride = fractalShape[0];
nd2nzB1Params.dstNzNStride = 1;
nd2nzB1Params.dstNzMatrixStride = fractalShape[0] * CeilAlign(n, fractalShape[1] * fractalNum);
```

##### 5.2.2. B矩阵GM上输入为[n, k]

B矩阵GM输入转置`[n, k]`时，按一个ND矩阵搬运到L1并转换为Nz排布。以下以half输入为例。

<div align="center">
  <img src="figures/GM_L1_FP16_B_transInput_n_k_to_Nz.png" width="600"><br>
  图7：B矩阵转置，half数据类型下，GM -> L1，ND -> Nz
</div>

参数配置要点：

- `nValue = n`，`dValue = k`，对应源ND矩阵shape。
- `dstNzC0Stride = CeilAlign(n, fractalShape[0])`，表示L1上Nz矩阵row方向对齐后的长度，单位为32B。
- `ndNum = 1`，表示源数据按一个ND矩阵搬运。

```cpp
nd2nzB1Params.ndNum = 1;
nd2nzB1Params.nValue = n;
nd2nzB1Params.dValue = k;
nd2nzB1Params.srcNdMatrixStride = 0;
nd2nzB1Params.srcDValue = k;

// 以下这个参数取B矩阵在L1上，row方向的对齐后的长度
nd2nzB1Params.dstNzC0Stride = CeilAlign(n, fractalShape[0]);
nd2nzB1Params.dstNzNStride = 1;
nd2nzB1Params.dstNzMatrixStride = 0;
```

### 6. L1到L0（`LoadData`）

本节说明如何将A / B矩阵从L1搬运到L0A / L0B。阅读时先根据[表3](#表3)确定当前场景，再按以下规则进入对应小节：

- **不转置搬运**：只发生大分形排布格式变化，A矩阵搬到L0A后为Zz，B矩阵搬到L0B后为Zn。
- **转置搬运**：同时发生小分形转置和大分形排布格式变化。
- **L1为Nz排布**：可根据数据类型选择`Load2D`、`LoadDataWithTranspose`或`Load3Dv2`。
- **L1为Zz排布**：场景14使用`LoadDataWithTranspose`完成float转置搬运。

代码块中重点关注`repeatTimes`、`srcStride`、`dstGap`、`dstFracGap`、`ifTranspose`和`enTranspose`等参数，它们决定一次搬运的分形数量、相邻分形间隔以及是否转置。

#### 6.1. A矩阵 L1 -> L0A

A矩阵搬入L0A后目标排布为Zz。根据A矩阵是否需要转置，分为以下两类：

- **A矩阵不转置输入`[m, k]`**：L1 -> L0A不需要转置，可使用`Load2D`或`Load3Dv2`。
- **A矩阵转置输入`[k, m]`**：L1 -> L0A需要转置，需根据数据类型和L1排布选择接口，详见[表2](#表2)。

##### 6.1.1. A矩阵 L1 -> L0A 不转置

该场景仅发生大分形排布格式变化：L1上的A矩阵为Nz排布，搬入L0A后为Zz排布。B4 / B8 / B16 / B32输入数据类型均可使用`Load2D`或`Load3Dv2`，参数配置基本相同，主要差异是`fractalShape`，可参考[表4](#表4)。下文以half为例。

**1. `Load2D`接口**

调用`Load2D`接口图示如下：

<div align="center">
  <img src="figures/L1_L0A_FP16_A_Load2D.png" width="800"><br>
  图8：half数据类型下，L1 -> L0A不转置，调用Load2D数据排布示意图
</div>

参数配置要点：

- 以m轴方向作为外轴进行for循环，以k轴方向作为内轴配置`repeatTimes`。
- `srcOffset`表示L1上A矩阵m轴方向每循环一次时的LocalTensor地址偏移。
- `dstOffset`表示L0A上A矩阵m轴方向每循环一次时的LocalTensor地址偏移。
- `ifTranspose = false`，表示仅进行Nz -> Zz的大分形排布格式变化。

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1]) * fractalSize;
uint32_t srcOffset = fractalSize;
// Nz -> Zz
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[1]);
loadDataParams.srcStride = AscendC::Std::ceil_div(m, fractalShape[0]);
// K轴方向相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = false;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[0]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

**2. `Load3Dv2`接口**

调用`Load3Dv2`接口如下所示：

<div align="center">
  <img src="figures/L1_L0A_F16_A_Load3DV2.png" width="800"><br>
  图9：half数据类型下，L1 -> L0A不转置，调用Load3Dv2数据排布示意图
</div>

`Load3D`的本质是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置。当按照如下代码所示的参数配置时，可调用一次`Load3Dv2`，实现L1搬运到L0A时数据排布格式由Nz转换为Zz。根据`Load3Dv2`完成img2col的过程可知，img2col后A矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为CeilAlign(m, fractalShape[0])；img2col后A矩阵宽度为ho * wo、ci * kh * kw，代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(k, fractalShape[1])。

```cpp
// Load3Dv2: Nz -> Zz
AscendC::LoadData3DParamsV2<T> loadDataParams;
// 源操作数height
loadDataParams.l1H = 1;
// 源操作数width
loadDataParams.l1W = CeilAlign(m, fractalShape[0]);
// 源操作数的通道数，
// img2col的结果矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1w
// img2col的结果矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(k, fractalShape[1]);
// 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
loadDataParams.kExtension = CeilAlign(k, fractalShape[1]);
// 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t，应为16的倍数；覆盖的情况则无倍数要求。
loadDataParams.mExtension = CeilAlign(m, fractalShape[0]);
// 卷积核在源操作数width维度滑动的步长
loadDataParams.strideW = 1;
// 卷积核在源操作数height维度滑动的步长
loadDataParams.strideH = 1;
// 卷积核width
loadDataParams.filterW = 1;
// 卷积核height
loadDataParams.filterH = 1;
// 卷积核width膨胀系数
loadDataParams.dilationFilterW = 1;
// 卷积核height膨胀系数
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = false;
loadDataParams.fMatrixCtrl = false;
```

##### 6.1.2. A矩阵 L1 -> L0A 转置

该场景同时发生小分形转置和大分形排布格式变化。Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持B4数据类型；B8 / B16 / B32输入数据类型可用接口不同，下文分别以int8_t、half、float为例说明。

- **int8_t**：使用`LoadDataWithTranspose`。
- **half**：可使用`Load2D`、`LoadDataWithTranspose`或`Load3Dv2`。
- **float**：L1为Nz时使用`Load3Dv2`；L1为Zz时可使用`LoadDataWithTranspose`。

###### 6.1.2.1. int8_t输入数据类型

调用`LoadDataWithTranspose`接口如下所示：

<div align="center">
  <img src="figures/L1_L0A_B8_A_trans_LoadDataWithTranspose.png" width="800"><br>
  图10：int8_t数据类型下，L1 -> L0A转置，调用LoadDataWithTranspose数据排布示意图
</div>

参数配置要点：

- 以m轴方向作为外轴进行for循环，以k轴方向作为内轴配置`repeatTimes`。
- 转置时连续多个分形合并为一个方块矩阵，因此`repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum)`。
- `dstFracGap`用于控制同一次转置内目的分形之间的间隔。

```cpp
// LoadDataWithTranspose: Nz-> Zz
// 根据以下函数原型，offset的数据类型是uint32_t
// __aicore__ inline LocalTensor operator[](const uint32_t offset) const
// dstoffset要根据A矩阵在L0上，宽度方向的对齐来求解
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1]) * fractalSize * fractalNum;
// srcoffset要根据A矩阵在L1上，高度方向的对齐来求解
uint32_t srcOffset = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
loadDataParams.startIndex = 0;
// 迭代次数，每次迭代转置一个方块矩阵
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
// 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
loadDataParams.srcStride = 1;
// 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
loadDataParams.dstGap = 0;
// 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
loadDataParams.dstFracGap = AscendC::Std::ceil_div(k, fractalShape[1]) - 1;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

###### 6.1.2.2. half输入数据类型

**1. `Load2D`接口**

调用`Load2D`接口如下所示：

<div align="center">
  <img src="figures/L1_L0A_F16_A_trans_Load2D.png" width="800"><br>
  图11：half数据类型下，L1 -> L0A转置，调用Load2D数据排布示意图
</div>

参数配置要点：

- 以m轴方向作为外轴进行for循环，以k轴方向作为内轴配置`repeatTimes`。
- `srcOffset`表示L1上A矩阵m轴方向每循环一次时的LocalTensor地址偏移。
- `dstOffset`表示L0A上A矩阵m轴方向每循环一次时的LocalTensor地址偏移。
- `ifTranspose = true`，表示从L1 -> L0A时对每个小分形进行转置。

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[0]) * fractalSize;
uint32_t srcOffset = AscendC::Std::ceil_div(k, fractalShape[0]) * fractalSize;
AscendC::LoadData2DParams loadDataParams;
// 迭代次数，每个迭代可以处理512B数据
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0]);
// 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔，单位：512B
loadDataParams.srcStride = 1;
// 相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔，单位：512B
loadDataParams.dstGap = 0;
// 是否启用转置功能，对每个分形矩阵进行转置，默认为false
loadDataParams.ifTranspose = true;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

**2. `LoadDataWithTranspose`接口**

调用`LoadDataWithTranspose`接口的图示与`Load2D`一致，以m轴方向作为外轴进行for循环（如图11红框部分），以k轴方向作为内轴来配置loadDataParams.repeatTimes，默认小分形转置。

```cpp
// dstoffset要根据A矩阵在L0上，宽度方向的对齐来求解
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1]) * fractalSize * fractalNum;
// srcoffset要根据A矩阵在L1上，高度方向的对齐来求解
uint32_t srcOffset = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
loadDataParams.startIndex = 0;
// 迭代次数，每次迭代转置一个方块矩阵
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
// 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
loadDataParams.srcStride = 1;
// 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
loadDataParams.dstGap = 0;
// 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
loadDataParams.dstFracGap = AscendC::Std::ceil_div(k, fractalShape[1]) - 1;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

**3. `Load3Dv2`接口**

`Load3D`的本质是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置，配置loadDataParams.enTranspose参数为true，在搬运过程中完成小分形转置。根据`Load3Dv2`指令完成img2col的过程，可知 img2col后A矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为 CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ho * wo、ci * kh * kw，代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。如图所示：

<div align="center">
  <img src="figures/L1_L0A_FP16_A_trans_Load3DV2.png" width="800"><br>
  图12：half数据类型下，L1 -> L0A转置，调用Load3Dv2数据排布示意图
</div>

```cpp
// 源操作数height
loadDataParams.l1H = 1;
// 源操作数width
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// 源操作数的通道数，
// img2col的结果矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1w
// img2col的结果矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t，应为16的倍数；覆盖的情况则无倍数要求。
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// 卷积核在源操作数width维度滑动的步长
loadDataParams.strideW = 1;
// 卷积核在源操作数height维度滑动的步长
loadDataParams.strideH = 1;
// 卷积核width
loadDataParams.filterW = 1;
// 卷积核height
loadDataParams.filterH = 1;
// 卷积核width膨胀系数
loadDataParams.dilationFilterW = 1;
// 卷积核height膨胀系数
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

###### 6.1.2.3. float输入数据类型

float的分形为[16, 8]，需要将2个[16, 8]的分形拼成1个方型进行转置。当A矩阵在L1上数据排布为Nz时，并不满足`LoadDataWithTranspose`指令两个连续分形合并为方块矩阵的要求，如图13所示。当L1上排布为Nz时，`LoadDataWithTranspose`会读2个连续的小分形，如图左边红色框部分，拼成32*8的分形，不满足转置需要按照方型的要求，此时可使用`Load3Dv2`实现L1 -> L0A转置搬运；当A矩阵在L1上数据排布为Zz时，`LoadDataWithTranspose`会读2个连续的小分形，如图右边红色框部分，拼成16 * 16的方型，满足转置要求，可使用`LoadDataWithTranspose`指令实现L1 -> L0A转置搬运。

<div align="center">
  <img src="figures/A矩阵L1上为Nz和Zz的区别.png" width="600"><br>
  图13：float数据类型，L1上数据排布格式为Nz和Zz时，LoadDataWithTranspose一次读取数据对比图
</div>

**1. L1上数据排布为Nz，L1 -> L0A调用`Load3Dv2`**

如图14所示，调用`Load3Dv2`指令时，在写入L0A之前会先分别将A矩阵高度和宽度轴向16、8对齐，在转置搬运过程中`Load3Dv2`指令会自动填充虚拟数据保证按照方型转置，在写入L0A时虚拟数据的空间会被跳过，如图白色部分，保证最终写入到L0A的A矩阵是Zz排布。

<div align="center">
  <img src="figures/L1_L0A_F32_A_trans_Load3DV2.png" width="800"><br>
  图14：float数据类型下，L1 -> L0A转置，调用Load3Dv2数据排布示意图
</div>

```cpp
// 源操作数height
loadDataParams.l1H = 1;
// 源操作数width
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// 源操作数的通道数，
// img2col的结果矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1w
// img2col的结果矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t/int4_t，应为16的倍数；覆盖的情况则无倍数要求。
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// 卷积核在源操作数width维度滑动的步长
loadDataParams.strideW = 1;
// 卷积核在源操作数height维度滑动的步长
loadDataParams.strideH = 1;
// 卷积核width
loadDataParams.filterW = 1;
// 卷积核height
loadDataParams.filterH = 1;
// 卷积核width膨胀系数
loadDataParams.dilationFilterW = 1;
// 卷积核height膨胀系数
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**2. L1上数据排布为Zz，L1 -> L0A调用`LoadDataWithTranspose`**

如图15所示，当L1上的A矩阵满足Zz排布才能满足`LoadDataWithTranspose`指令要求两个连续分形合并为方块矩阵的要求。

<div align="center">
  <img src="figures/L1_L0A_F32_A_trans_LoadDataWithTranspose.png" width="800"><br>
  图15：float数据类型下，L1 -> L0A转置，调用LoadDataWithTranspose数据排布示意图
</div>

以m轴方向作为外轴进行for循环（如图15红框部分），以k轴方向作为内轴来配置loadDataParams.repeatTimes。如图15所示，srcoffset和dstoffset的含义分别是：在L1上，A矩阵m轴方向每循环一次时，LocalTensor的地址偏移量；在L0A上，A矩阵m轴方向每循环一次时，LocalTensor的地址偏移量。配置`LoadData2dTransposeParams`结构体时，需要特别注意的是，由于转置时连续两个分形合并为一个方块，如图蓝色和绿色方框表示2个方块，因此loadDataParams.repeatTimes=AscendC::Std::ceil_div(k, fractalShape[1] * fractalNum)。

```cpp
// A矩阵在L0A上shape为[m，k]、Zz排布，因此dstoffset要根据A矩阵在K轴方向的对齐来求解
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
// A矩阵在L1上shape为[K,M]、Zz排布，因此srcoffset为1个分形包含的元素个数
uint32_t srcOffset = fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
loadDataParams.startIndex = 0;
// 迭代次数，每次迭代转置一个方块矩阵
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[1] * fractalNum);
// 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
loadDataParams.srcStride = AscendC::Std::ceil_div(m, fractalShape[1] * fractalNum);
// 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
loadDataParams.dstGap = 1;
// 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
loadDataParams.dstFracGap = 0;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

#### 6.2. B矩阵 L1 -> L0B

B矩阵搬入L0B后目标排布为Zn。根据B矩阵是否需要转置，分为以下两类：

- **B矩阵转置输入`[n, k]`**：L1 -> L0B不需要转置，可使用`Load2D`。
- **B矩阵不转置输入`[k, n]`**：L1 -> L0B需要转置，需根据数据类型和L1排布选择接口，详见[表2](#表2)。

##### 6.2.1. B矩阵 L1 -> L0B 不转置

该场景仅发生大分形排布格式变化：L1上的B矩阵为Nz排布，搬入L0B后为Zn排布。B4 / B8 / B16 / B32输入数据类型均可使用`Load2D`，参数配置基本相同，主要差异是`fractalShape`，可参考[表4](#表4)。下文以half为例。

<div align="center">
  <img src="figures/L1_L0B_F16_B_Load2D.png" width="800"><br>
  图16：half数据类型下，L1 -> L0B不转置，调用Load2D数据排布示意图
</div>

参数配置要点：

- 以k轴方向作为外轴进行for循环，以n轴方向作为内轴配置`repeatTimes`。
- `srcOffset`表示L1上B矩阵k轴方向每循环一次时的LocalTensor地址偏移。
- `dstOffset`表示L0B上B矩阵k轴方向每循环一次时的LocalTensor地址偏移。
- 该场景下L1上的B矩阵与L0B上的B矩阵等价，因此`srcOffset`和`dstOffset`相同。

```cpp
// srcOffset和dstOffset相同
// n轴向fractalShape[0]对齐
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0]) * fractalSize;
uint32_t srcOffset = AscendC::Std::ceil_div(n, fractalShape[0]) * fractalSize;
// Nz -> Zn
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[0]);
loadDataParams.srcStride = 1;
// n轴方向相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = false;
// k轴向fractalShape[1]对齐
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[1]); ++i) {
    AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

##### 6.2.2. B矩阵 L1 -> L0B 转置

该场景同时发生小分形转置和大分形排布格式变化。B4 / B8 / B16 / B32输入数据类型可用接口不同，下文分别以int4_t / int8_t、half、float为例说明。

- **int4_t / int8_t**：使用`LoadDataWithTranspose`。
- **half**：可使用`Load2D`、`LoadDataWithTranspose`或`Load3Dv2`。
- **float**：L1为Nz时使用`Load3Dv2`；L1为Zz时可使用`LoadDataWithTranspose`。

###### 6.2.2.1. int4_t / int8_t输入数据类型

输入数据类型为int4_t/int8_t，且L1 -> L0B需要转置时，可使用的接口都是`LoadDataWithTranspose`，参数配置基本相似，仅fractalShape、fractalNum、fractalSize不同。可参考[表4](#表4)，本节以int8_t为例进行图示介绍

<div align="center">
  <img src="figures/L1_L0B_B8_B_trans_LoadDataWithTranspose.png" width="700"><br>
  图17：int8_t数据类型下，L1 -> L0B转置，调用LoadDataWithTranspose数据排布示意图
</div>

参数配置要点：

- 以k轴方向作为外轴进行for循环，以n轴方向作为内轴配置`repeatTimes`。
- 转置时连续多个分形合并为一个方块矩阵，因此`repeatTimes`需要按方块矩阵粒度配置。
- 转置前后同一方块内的分形地址保持连续，因此`dstFracGap = 0`。

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[1]);
loadDataParams.srcStride = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
loadDataParams.dstGap = 1;
loadDataParams.dstFracGap = 0;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

###### 6.2.2.2. half输入数据类型

输入数据类型为half，且L1 -> L0B需要转置时，可使用的接口是`Load2D`、`LoadDataWithTranspose`、`Load3Dv2`，下面分别介绍这3种接口的调用方式及图示说明。

**1. 调用`Load2D`**
<div align="center">
  <img src="figures/L1_L0B_FP16_B_trans_Load2D.png" width="700"><br>
  图18：half数据类型下，L1 -> L0B转置，调用Load2D数据排布示意图
</div>

根据[表2](#表2)可知，当B矩阵转置时，b矩阵也转置，当且仅当输入数据类型为half时，可以调用`LoadData`指令完成转置。

如上图18所示，以k轴方向作为外轴进行for循环，以n轴方向作为内轴来配置loadDataParams.repeatTimes。结合图示，并根据前述srcoffset和dstoffset的定义，可以得出如下配置信息。

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
// Nz -> Zn
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum);
loadDataParams.srcStride = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = true;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

**2. 调用`LoadDataWithTranspose`**

调用`LoadDataWithTranspose`接口的图示与`Load2D`一致，以k轴方向作为外轴进行for循环，以n轴方向作为内轴来配置loadDataParams.repeatTimes，默认小分形转置。

```cpp
// LoadDataWithTranspose: Nz -> Zn
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[1]);
loadDataParams.srcStride = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = 0;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

**3. 调用`Load3Dv2`**

<div align="center">
  <img src="figures/L1_L0B_FP16_B_trans_Load3DV2.png" width="800"><br>
  图19：half数据类型下，L1 -> L0B转置，调用Load3Dv2数据排布示意图
</div>

`Load3D`的本质是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置，配置loadDataParams.enTranspose参数为true，在搬运过程中完成小分形转置。根据`Load3Dv2`指令完成img2col的过程，可知 img2col后B矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：B矩阵的高度为 CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ho * wo、ci * kh * kw，代入kh=1，kw=1，可知B矩阵的宽度为CeilAlign(n, fractalShape[1])。

```cpp
// Load3Dv2: Nz -> Zn
AscendC::LoadData3DParamsV2<T> loadDataParams;
loadDataParams.l1H = 1;
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
loadDataParams.channelSize = CeilAlign(n, fractalShape[1]);
loadDataParams.kExtension = CeilAlign(n, fractalShape[1]);
loadDataParams.mExtension = CeilAlign(k, fractalShape[0]);
loadDataParams.strideW = 1;
loadDataParams.strideH = 1;
loadDataParams.filterW = 1;
loadDataParams.filterH = 1;
loadDataParams.dilationFilterW = 1;
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
// 对于Load3Dv2接口，当目的地址为L0B时，B矩阵会自动转置
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

###### 6.2.2.3. float输入数据类型

与A矩阵float转置输入类似，当输入数据类型为float且小分形需要转置时，L1上排布为Nz或Zz会影响可调用的接口。

**1. L1上数据排布为Nz，L1 -> L0B调用`Load3Dv2`**

如图20所示，调用`Load3Dv2`指令时，在写入L0B之前会先分别将B矩阵高度和宽度轴向16、8对齐，在转置搬运过程中`Load3Dv2`指令会自动填充虚拟数据保证按照方型转置，在写入L0B时虚拟数据的空间会被跳过，如图白色部分，保证最终写入到L0B的B矩阵是Zn排布。

对齐要求方面，在L1上B矩阵的k轴向`fractalShape[0] * fractalNum`对齐，n轴向`fractalShape[1]`对齐；在L0B上B矩阵的k轴向`fractalShape[0] * fractalNum`对齐，n轴向`fractalShape[1]`对齐。

<div align="center">
  <img src="figures/L1_L0B_FP32_B_trans_Load3DV2.png" width="800"><br>
  图20：float数据类型下，L1 -> L0B转置，调用Load3Dv2数据排布示意图
</div>

参数配置要点：

- `Load3Dv2`完成img2col后，B矩阵高度为`CeilAlign(k, fractalShape[0])`，宽度为`CeilAlign(n, fractalShape[1])`。
- 目的地址为L0B时，B矩阵默认转置，`enTranspose`取值不影响功能。

```cpp
loadDataParams.l1H = 1;
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
loadDataParams.channelSize = CeilAlign(n, fractalShape[1]);
loadDataParams.kExtension = CeilAlign(n, fractalShape[1]);
loadDataParams.mExtension = CeilAlign(k, fractalShape[0]);
loadDataParams.strideW = 1;
loadDataParams.strideH = 1;
loadDataParams.filterW = 1;
loadDataParams.filterH = 1;
loadDataParams.dilationFilterW = 1;
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
```

**2. L1上数据排布为Zz，L1 -> L0B调用`LoadDataWithTranspose`**

如图21所示，当L1上的B矩阵满足Zz排布才能满足`LoadDataWithTranspose`指令要求两个连续分形合并为方块矩阵的要求。

<div align="center">
  <img src="figures/L1_L0B_FP32_B_trans_LoadDataWithTranspose.png" width="800"><br>
  图21：float数据类型下，L1 -> L0B转置，调用LoadDataWithTranspose数据排布示意图
</div>

配置`LoadData2dTransposeParams`结构体时，需要特别注意的是，由于转置时连续两个分形合并为一个方块，因此loadDataParams.repeatTimes=AscendC::Std::ceil_div(n, fractalShape[1] * fractalNum)。另外，由于在目的操作数中同属一个方块矩阵的两个分形地址不再连续，此时参数loadDataParams.dstFracGap = AscendC::Std::ceil_div(n, fractalShape[0]) - 1。

```cpp
// B矩阵在L0B上shape为[k, n]、Zn排布，因此dstoffset要根据B矩阵在N轴方向的对齐来求解
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0]) * fractalSize * fractalNum;
// B矩阵在L1上shape为[K,N]、ZZ排布，因此srcoffset要根据B矩阵在N轴方向的对齐来求解
uint32_t srcOffset = AscendC::Std::ceil_div(n, fractalShape[1] * fractalNum) * fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
loadDataParams.startIndex = 0;
// 迭代次数，每次迭代转置一个方块矩阵
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[1] * fractalNum);
// 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
loadDataParams.srcStride = 1;
// 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
loadDataParams.dstGap = 0;
// 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
loadDataParams.dstFracGap = AscendC::Std::ceil_div(n, fractalShape[0]) - 1;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0]); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

### 7. 矩阵乘（`Mmad`）

下面将介绍如何配置
`Mmad`指令的`MmadParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

需要注意的是当`Mmad`指令执行时，
矩阵计算单元会从L0A/L0B连续读入多个分形参与矩阵乘计算，读入分形的数量根据`MmadParams`结构体的成员变量m、n、k的取值以及`Mmad`指令对L0A/L0B上A矩阵和B矩阵各个轴的对齐要求来计算的。由于`Mmad`指令，即A矩阵分形为[16, 32]、B矩阵分形为[32, 16]来连续读入分形的，也就是说矩阵计算单元从L0A/L0B连续读入的分形总数目分别为：AscendC::Std::ceil_div(m, 16) * AscendC::Std::ceil_div(k, 32)、AscendC::Std::ceil_div(k, 32) * AscendC::Std::ceil_div(n, 16)。

因此当L0A/L0B上对A矩阵和B矩阵在各个轴的实际对齐要求与`Mmad`指令默认的对齐要求不一致时，就可能导致连续读入分形时，错误读入完全由无效数据填充的分形而忽略了包含有效数据的分形。

如下图22所示，以场景5为例，输入数据类型为int8_t，A转置输入、B矩阵不转置输入。由前文内容可知：L0A在m轴和k轴分别向16 * 2、32对齐，L0B在k轴和n轴分别向32、16 * 2对齐，而`Mmad`指令默认在m、k、n三个轴的对齐要求分别是向16、32、16对齐，假设n=70时，按照L0上的对齐要求nAlignL0 = CeilAlign(70, 16 * 2) = 96，按照`Mmad`指令默认的对齐要求，nAlignL0 = CeilAlign(70, 16) = 80，此时n轴实际对齐要求与`Mmad`指令默认的对齐要求不一致。m轴同理。

以B矩阵n轴为例，如图22左边子图所示，如果设置mmadParams.n = n = 70，就会导致读入编号为5的分形，同时没能将包含有效数据的编号为10的分形。

如图22右边子图所示，如果设置mmadParams.n = CeilAlign(n, fractalShape[0] * fractalNum) = 96，此时会读入全部分形，虽然矩阵计算结果中包含了无效数据参与计算的结果，但是在`Fixpipe`指令搬出数据时通过设置fixpipeParams.nSize = n来保证无效数据参与计算的结果不会被搬出。m轴参数配置同理。

<div align="center">
  <img src="figures/B_B8_read_invalid_data.png" width="900"><br>
  图22：B矩阵GM不转置输入，int8_t数据类型下，n轴实际对齐要求与Mmad指令默认的对齐要求不一致
</div>

上述场景类似，如场景13，当输入数据类型为float、A矩阵GM转置输入([k, m])时，k轴实际对齐要求与`Mmad`指令默认的对齐要求也不一致，但是此种场景下的解决方法与上述场景有所不同，需要单独引入mmadParams.kDirectionAlign参数来解决，下面说明原因。

根据矩阵乘法的计算公式可知，k轴作为A、B矩阵公共的维度，此时如果像上述场景那样设置mmadParams.k = CeilAlign(k, fractalShape[1] * fractalNum) = CeilAlign(70, 16) = 80，会导致C矩阵中每个元素的数值都受到多读入的无效数据的影响，并且也不能通过设置fixpipeParams的参数来保证无效数据参与计算的结果不会被搬出。

如下图23所示，mmadParams.kDirectionAlign仅在输入数据类型为float时生效。当A矩阵转置时，该参数需要设置为真，此时L0A上A矩阵在k方向向16对齐，矩阵计算单元从L0A读取数据会跳过填充的无效数据，其余场景下该参数取默认值为假，此时L0A上A矩阵在k方向向8对齐。

<div align="center">
  <img src="figures/Mmad_FP32_A_trans_kDirectionAlign.png" width="900"><br>
  图23：A矩阵GM转置输入，float数据类型下，k轴实际对齐要求与Mmad指令默认的对齐要求不一致
</div>

```cpp
AscendC::MmadParams mmadParams;
// 左矩阵Height
mmadParams.m = m;
// 右矩阵width
mmadParams.n = n;
if constexpr (AscendC::IsSameType<T, int8_t>::value && AscendC::IsSameType<U, int32_t>::value) {
    if constexpr (!isBtranspose) {
        // mmad默认n轴向16对齐，但是由于b转置过程n轴向2 * 16对齐，填充了一个全部由无效数据的32 * 16的分形，
        // 如果仍然设置mmadParams.n = n，cube单元会多读入无效数据的分形同时有效数据的分形也未被读入。
        // 此时可以通过设置n向32对齐，让此分形参与计算，搬出时跳过无效分形参与计算的得到的分形即可
        mmadParams.n = CeilAlign(n, fractalShape[0] * fractalNum);
    }
}
// 左矩阵Width、右矩阵Height
mmadParams.k = k;
if constexpr (AscendC::IsSameType<T, float>::value && AscendC::IsSameType<U, float>::value) {
    if constexpr (isAtranspose) {
        mmadParams.kDirectionAlign = true;
    }
}
```

### 8. L0C到GM（`Fixpipe`）

下面将介绍如何配置
`Fixpipe`指令的`FixpipeParamsV220`结构体的成员，各个成员变量的具体含义这里不再赘述。其中，fixpipeParams.srcStride的单位是32/sizeof(T)个元素，其含义是源Nz矩阵中相邻Z排布的起始地址偏移（ND矩阵中同一行的元素在源Nz矩阵中处于相邻的Z排布），该参数的取值是L0C上C矩阵m轴向16对齐后的长度。

```cpp
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.nSize = n;
fixpipeParams.mSize = m;

// 源操作数来源于L0c，因此m只需要向16对齐，与数据类型无关
// 源NZ矩阵中相邻Z排布的起始地址偏移
fixpipeParams.srcStride = CeilAlign(m, fractalShape[0]);
fixpipeParams.dstStride = n;

fixpipeParams.ndNum = 1;
fixpipeParams.srcNdStride = 0;
fixpipeParams.dstNdStride = 0;
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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  使用NPU仿真 模式时，添加`-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认） | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4`、`5`、`6`、`7`、`8`、`9`、`10`、`11`、`12`、`13`、`14` | 场景编号 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
