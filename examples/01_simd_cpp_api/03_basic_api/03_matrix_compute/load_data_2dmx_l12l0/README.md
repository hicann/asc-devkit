# load_data_2dmx_l12l0样例

## 概述

本样例展示MX量化矩阵乘法中，如何使用基础API `LoadData` 完成L1到L0的数据搬运。

样例以FP4 / FP8类型的A / B矩阵和fp8_e8m0_t类型的scaleA / scaleB矩阵为输入，覆盖A / B矩阵转置和不转置输入场景。主要演示：

1. 使用`LoadData2DParamsV2`控制A / B矩阵搬运到L0A / L0B；
2. 使用`LoadData2DMxParams`控制scaleA / scaleB矩阵搬运到L0A_MX / L0B_MX；
3. 配合`MmadMx`完成MX矩阵乘法，并通过`Fixpipe`将结果搬出到GM。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── load_data_2dmx_l12l0
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── figures                     // 图示
│   ├── load_data_2dmx_l12l0.asc    // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

### 1. 整体流程

MX矩阵乘整体流程如下所示：

```
GM(ND) --DataCopy--> L1(Nz/Zz/Nn) --LoadData(LoadData2DParamsV2, LoadData2DMxParams)--> L0A(Nz)/L0B(Zn)/L0A_MX(Zz)/L0B_MX(Nn) --MmadMx--> L0C(Nz) --Fixpipe--> GM(ND)
```

**步骤详解**：

1. **GM -> L1**：
   - 调用`DataCopy`指令，传入`Nd2NzParams`结构体参数，实现A / B矩阵从ND到Nz的格式转换。
   - 调用`DataCopy`指令，传入`Nd2NzParams`或`Dn2NzParams`结构体参数，并按B16视图实现scaleA / scaleB矩阵从ND到Zz / Nn的格式转换。
   - 使用`Fill`将无效数据填0，防止脏数据参与`MmadMx`计算。
2. **L1 -> L0**：
   - 调用`LoadData`指令，传入`LoadData2DParamsV2`和`LoadData2DMxParams`两个结构体参数。
   - `LoadData2DParamsV2`控制A / B矩阵搬运，`LoadData2DMxParams`控制scaleA / scaleB矩阵搬运。
3. **矩阵乘**：使用`MmadMx`接口执行MX矩阵乘法。
4. **L0C -> GM**：使用`Fixpipe`接口搬出结果。

需要注意，`LoadData2DMxParams`不包含类似`ifTranspose`的转置控制参数，scale矩阵在L1 -> L0A_MX / L0B_MX阶段按`x` / `y`方向和stride直接搬运。因此，scaleA / scaleB需要在GM -> L1阶段提前整理成L0A_MX / L0B_MX侧期望的排布。

A / B矩阵和scaleA / scaleB矩阵在不同存储单元的数据排布格式不同，如[表1](#表1)：

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：不同存储单元的数据排布格式</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">GM</span></td>
    <td align="center"><span style="font-weight: bold;">L1</span></td>
    <td align="center"><span style="font-weight: bold;">L0A</span></td>
    <td align="center"><span style="font-weight: bold;">L0A_MX</span></td>
    <td align="center"><span style="font-weight: bold;">L0B</span></td>
    <td align="center"><span style="font-weight: bold;">L0B_MX</span></td>
    <td align="center"><span style="font-weight: bold;">L0C</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">Nz</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B矩阵</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">Zn</td>
    <td align="center">-</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">scaleA矩阵</span></td>
    <td align="center">ND</td>
    <td align="center">Zz</td>
    <td align="center">-</td>
    <td align="center">Zz</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">scaleB矩阵</span></td>
    <td align="center">ND</td>
    <td align="center">Nn</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">Nn</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">C矩阵</span></td>
    <td align="center">ND</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">Nz</td>
  </tr>
</table>

本样例固定矩阵乘规格为`[m, n, k] = [40, 50, 70]`，输出C矩阵为`[40, 50]`，float类型，ND格式。核函数通过`scenarioNum`选择不同场景，具体输入shape和L1 -> L0搬运方式见[表2](#表2)。

6个场景由FP4 / FP8数据类型、A / B是否转置输入，以及A转置输入时是否使用for循环搬运组合得到。其中，`isAtranspose`同时表示A矩阵和scaleA矩阵是否转置输入，`isBtranspose`同时表示B矩阵和scaleB矩阵是否转置输入。场景5 / 6使用for循环搬运A矩阵，用于避免A转置输入时单次`LoadData`多搬脏数据超过1个分形。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：scenarioNum不同取值、输入shape及L1 -> L0搬运方式</span></caption>
  <tr>
    <td><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">A / B数据类型</span></td>
    <td><span style="font-weight: bold;">A矩阵类型</span></td>
    <td><span style="font-weight: bold;">B矩阵类型</span></td>
    <td><span style="font-weight: bold;">输入shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">A矩阵搬运</span></td>
    <td><span style="font-weight: bold;">B矩阵搬运</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td>FP4</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp4x2_e2m1_t</td>
    <td>A [40, 70]<br>B [50, 70]<br>scaleA [40, 4]<br>scaleB [50, 4]</td>
    <td align="center">false</td>
    <td align="center">true</td>
    <td>LoadData2DParamsV2（不转置）</td>
    <td>LoadData2DParamsV2（不转置）</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>FP4</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>LoadData2DParamsV2（转置）</td>
    <td>LoadData2DParamsV2（转置）</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td>FP8</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e5m2_t</td>
    <td>A [40, 70]<br>B [50, 70]<br>scaleA [40, 4]<br>scaleB [50, 4]</td>
    <td align="center">false</td>
    <td align="center">true</td>
    <td>LoadData2DParamsV2（不转置）</td>
    <td>LoadData2DParamsV2（不转置）</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>FP8</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e4m3fn_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>LoadData2DParamsV2（转置）</td>
    <td>LoadData2DParamsV2（转置）</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td>FP4</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>for循环 + LoadData2DParamsV2（转置）</td>
    <td>LoadData2DParamsV2（转置）</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>FP8</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e4m3fn_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>for循环 + LoadData2DParamsV2（转置）</td>
    <td>LoadData2DParamsV2（转置）</td>
  </tr>
</table>

### 2. 场景说明

各场景的输入shape见[表2](#表2)。本节只说明不同场景在L1 -> L0搬运、补零和后续计算参数上的差异。

**场景1：输入FP4数据类型，isAtranspose=false，isBtranspose=true**

- A矩阵不转置输入`[m, k]`，L1 -> L0A不需要转置，`LoadData2DParamsV2`的`ifTranspose = false`。
- B矩阵转置输入`[n, k]`，L1 -> L0B不需要转置，`LoadData2DParamsV2`的`ifTranspose = false`。
- scaleA输入shape为`[m, scaleK]`，scaleB输入shape为`[n, scaleK]`。

<div align="center">
  <img src="figures/whole_process/B4_A_scaleA_.png" width="1000"><br>
  图1：场景1 MX矩阵乘GM -> L1 -> L0A / L0A_MX流程图
</div>

<div align="center">
  <img src="figures/whole_process/B4_B_scaleB_NK.png" width="1000"><br>
  图2：场景1 MX矩阵乘GM -> L1 -> L0B / L0B_MX流程图
</div>

**场景2：输入FP4数据类型，isAtranspose=true，isBtranspose=false**

- A矩阵转置输入`[k, m]`，L1 -> L0A需要转置，`LoadData2DParamsV2`的`ifTranspose = true`。
- B矩阵不转置输入`[k, n]`，L1 -> L0B需要转置，`LoadData2DParamsV2`的`ifTranspose = true`。
- scaleA输入shape为`[scaleK/2, m, 2]`，scaleB输入shape为`[scaleK/2, n, 2]`。
- A矩阵m方向多搬脏数据超过1个分形，`MmadMx`需设置`mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)`，`Fixpipe`搬出时跳过无效分形参与计算的结果。

<div align="center">
  <img src="figures/whole_process/B4_A_scaleA_trans_KM.png" width="1000"><br>
  图3：场景2 MX矩阵乘GM -> L1 -> L0A / L0A_MX流程图
</div>

<div align="center">
  <img src="figures/whole_process/B4_B_scaleB_trans_KN.png" width="1000"><br>
  图4：场景2 MX矩阵乘GM -> L1 -> L0B / L0B_MX流程图
</div>

**场景3：输入FP8数据类型，isAtranspose=false，isBtranspose=true**

与场景1类似，但数据类型为FP8。

- A矩阵和B矩阵的L1 -> L0搬运均不需要转置，`LoadData2DParamsV2`的`ifTranspose = false`。
- scaleA输入shape为`[m, scaleK]`，scaleB输入shape为`[n, scaleK]`。
- FP8数据类型下，A矩阵不转置输入`[m, k]`、B矩阵转置输入`[n, k]`时k在col方向，`DataCopy`仅将k方向向32B对齐，需要在L1上将k方向尾数据刷为0，防止脏数据参与计算。

<div align="center">
  <img src="figures/whole_process/B8_A_scaleA_.png" width="1000"><br>
  图5：场景3 MX矩阵乘GM -> L1 -> L0A / L0A_MX流程图
</div>

<div align="center">
  <img src="figures/whole_process/B8_B_scaleB_NK.png" width="1000"><br>
  图6：场景3 MX矩阵乘GM -> L1 -> L0B / L0B_MX流程图
</div>

**场景4：输入FP8数据类型，isAtranspose=true，isBtranspose=false**

与场景2类似，但数据类型为FP8。

- A矩阵转置输入`[k, m]`，B矩阵不转置输入`[k, n]`，L1 -> L0搬运均需要转置，`LoadData2DParamsV2`的`ifTranspose = true`。
- scaleA输入shape为`[scaleK/2, m, 2]`，scaleB输入shape为`[scaleK/2, n, 2]`。
- A矩阵m方向多搬脏数据超过1个分形，`MmadMx`需设置`mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)`。
- A矩阵转置输入`[k, m]`、B矩阵不转置输入`[k, n]`时，均需要在L1上将k方向剩余脏数据刷为0。

<div align="center">
  <img src="figures/whole_process/B8_A_scaleA_trans_KM.png" width="1000"><br>
  图7：场景4 MX矩阵乘GM -> L1 -> L0A / L0A_MX流程图
</div>

<div align="center">
  <img src="figures/whole_process/B8_B_scaleB_trans_KN.png" width="1000"><br>
  图8：场景4 MX矩阵乘GM -> L1 -> L0B / L0B_MX流程图
</div>

**场景5：输入FP4数据类型，isAtranspose=true，isBtranspose=false**

- A矩阵转置输入`[k, m]`，L1 -> L0A需要转置，使用for循环方式调用`LoadData`搬运。
- for循环每次在L0A上跳过m方向尾部脏数据分形，使m方向多搬运数据不超过1个分形。
- B矩阵不转置输入`[k, n]`，L1 -> L0B需要转置，单次调用`LoadData`搬运，`LoadData2DParamsV2`的`ifTranspose = true`。
- scaleA输入shape为`[scaleK/2, m, 2]`，scaleB输入shape为`[scaleK/2, n, 2]`。
- A矩阵和B矩阵均需要在L1上将k方向剩余脏数据刷为0。

<div align="center">
  <img src="figures/whole_process/B4_A_scaleA_for_trans_KM.png" width="1000"><br>
  图9：场景5 MX矩阵乘GM -> L1 -> L0A / L0A_MX流程图
</div>

<div align="center">
  <img src="figures/whole_process/B4_B_scaleB_trans_KN.png" width="1000"><br>
  图10：场景5 MX矩阵乘GM -> L1 -> L0B / L0B_MX流程图
</div>

**场景6：输入FP8数据类型，isAtranspose=true，isBtranspose=false**

与场景5类似，但数据类型为FP8。

- A矩阵转置输入`[k, m]`，L1 -> L0A需要转置，使用for循环方式调用`LoadData`搬运，并避免m方向多搬脏数据超过1个分形。
- B矩阵不转置输入`[k, n]`，L1 -> L0B需要转置，单次调用`LoadData`搬运，`LoadData2DParamsV2`的`ifTranspose = true`。
- scaleA输入shape为`[scaleK/2, m, 2]`，scaleB输入shape为`[scaleK/2, n, 2]`。
- A矩阵和B矩阵均需要在L1上将k方向剩余脏数据刷为0。

<div align="center">
  <img src="figures/whole_process/B8_A_scaleA_for_trans_KM.png" width="1000"><br>
  图11：场景6 MX矩阵乘GM -> L1 -> L0A / L0A_MX流程图
</div>

<div align="center">
  <img src="figures/whole_process/B8_B_scaleB_trans_KN.png" width="1000"><br>
  图12：场景6 MX矩阵乘GM -> L1 -> L0B / L0B_MX流程图
</div>

### 3. 参数说明

后续代码和参数说明会反复使用分形、对齐和scale相关变量，本节先集中定义这些概念。

- `fractalShape`：小分形的shape。本样例A / B矩阵输入涉及FP4和FP8，scale矩阵按B16视图搬运，分形相关信息见[表3](#表3)。
- `fractalSize`：1个小分形包含的元素个数，具体见[表3](#表3)。
- `fractalNum`：当从L1 -> L0A / L0B需要转置时，`LoadData`接口会按正方形矩阵转置。需要多个连续小分形合并为一个正方形矩阵时，`fractalNum`表示该正方形矩阵包含的小分形个数，具体见[表3](#表3)。
- `packedK`：按B8视图搬运时，A / B矩阵k方向在`DataCopy`参数中的等效列数。FP4输入实际仍有k个元素，但在ND -> Nz搬运过程中按B8视图处理，k方向每2个FP4元素对应1个B8视图元素，因此`packedK = AscendC::Std::ceil_div(k, 2)`；FP8输入按原始元素数配置，`packedK = k`。
- `scaleK`：scale矩阵k轴对齐后的长度。`scaleK = AscendC::Std::ceil_div(k, SCALE_BASE_FACTOR) * SCALE_EVEN_NUMBER`，其中`SCALE_BASE_FACTOR = 64`，`SCALE_EVEN_NUMBER = 2`。本样例中`k = 70`时，`scaleK = AscendC::Std::ceil_div(70, 64) * 2 = 4`。
- `alignK`：A / B矩阵k轴对齐后的长度。MX矩阵乘法中`MmadMx`接口要求k方向按`SCALE_BASE_FACTOR = 64`对齐，因此`alignK = CeilAlign(k, SCALE_BASE_FACTOR) = CeilAlign(k, 64)`。本样例中`k = 70`时，`alignK = CeilAlign(70, 64) = 128`。该64对齐要求只作用于k方向，m / n方向仍按对应数据类型的小分形或正方形转置粒度对齐。

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：不同数据类型分形相关信息</span></caption>
  <tr>
    <td></td>
    <td align="center"><code>本样例用途</code></td>
    <td align="center"><code>fractalShape</code></td>
    <td align="center"><code>fractalSize</code></td>
    <td align="center"><code>fractalNum</code></td>
    <td align="center"><code>packedK</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">FP4</span></td>
    <td align="center">A / B矩阵FP4输入</td>
    <td align="center">[16, 64]</td>
    <td align="center">1024</td>
    <td align="center">4</td>
    <td align="center"><code>AscendC::Std::ceil_div(k, 2)</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">FP8</span></td>
    <td align="center">A / B矩阵FP8输入</td>
    <td align="center">[16, 32]</td>
    <td align="center">512</td>
    <td align="center">2</td>
    <td align="center"><code>k</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center">scale矩阵按B16视图搬运</td>
    <td align="center">[16, 16]</td>
    <td align="center">256</td>
    <td align="center">1</td>
    <td align="center">-</td>
  </tr>
</table>

- `CeilAlign`：向上对齐操作。例如`m = 30`时，`CeilAlign(30, 16) = 32`，表示将m轴向16对齐，对齐后m轴长度为32。

```cpp
__aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
    return (size + alignValue - 1) / alignValue * alignValue;
}
```

- `ceil_div`：向上取整除法，一般用于求解向上对齐后的循环次数。
- `mAlignValue`：m轴向`mAlignValue`对齐。例如`mAlignValue = 16`，代表m轴对齐到16；依次类推还有`nAlignValue`、`kAlignValue`。
- `mAlignL1`和`mAlignL0`：A矩阵分别在L1和L0A上时，m轴对齐后的值。依次类推还有`kaAlignL1`、`kaAlignL0`、`nAlignL1`、`nAlignL0`、`kbAlignL1`、`kbAlignL0`。


### 4. 对齐要求

A / B / scaleA / scaleB矩阵在L1和L0上各轴的对齐要求不同，后续配置`DataCopy`、`LoadData`、`MmadMx`和`Fixpipe`参数时会用到这些值。A / B矩阵的k轴在MX场景下均按`SCALE_BASE_FACTOR = 64`对齐，即`kAlignValue = 64`；[表4](#表4)和[表5](#表5)仅列出随输入排布和搬运方式变化的m轴 / n轴对齐要求。

- [表4](#表4)用于确定GM -> L1后A / B矩阵在L1上的shape，以及后续`Fill`补零范围。
- [表5](#表5)用于确定L1 -> L0阶段`LoadData`搬运后的L0A / L0B对齐值，并影响后续`MmadMx`和`Fixpipe`参数配置。
- [表6](#表6)用于确定scaleA / scaleB矩阵在L1、L0A_MX和L0B_MX上的对齐shape。

<a name="表4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表4：A、B矩阵在L1上m轴 / n轴的对齐要求（L1排布格式为Nz）</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">FP4（<code>fractalNum = 4</code>）</span></td>
    <td align="center"><span style="font-weight: bold;">FP8（<code>fractalNum = 2</code>）</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]</span></td>
    <td colspan="2" align="center"><code>mAlignValue = fractalShape[0]</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td align="center" ><code>mAlignValue = fractalShape[1] = 64</code></td>
    <td align="center" ><code>mAlignValue = fractalShape[1] = 32</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td align="center" ><code>nAlignValue = fractalShape[1] = 64</code></td>
    <td align="center" ><code>nAlignValue = fractalShape[1] = 32</code></td>
  </tr>
 <tr>
    <td align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]</span></td>
    <td colspan="2" align="center"><code>nAlignValue = fractalShape[0]</code></td>
  </tr>
</table>

<a name="表5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表5：A、B矩阵在L0上m轴 / n轴的对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">FP4（<code>fractalNum = 4</code>）</span></td>
    <td align="center"><span style="font-weight: bold;">FP8（<code>fractalNum = 2</code>）</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]，L1 -> L0A不需要转置<br>（场景1 / 3）</span></td>
    <td colspan="2" align="center"><code>mAlignValue = fractalShape[0]</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置<br>（场景2 / 4，单次调用）</span></td>
    <td align="center"><code>mAlignValue = fractalShape[0] * fractalNum = 64</code></td>
    <td align="center"><code>mAlignValue = fractalShape[0] * fractalNum = 32</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置<br>（场景5 / 6，for循环调用）</span></td>
    <td colspan="2" align="center"><code>mAlignValue = fractalShape[0]</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1 -> L0B需要转置<br>（场景2 / 4 / 5 / 6）</span></td>
    <td align="center"><code>nAlignValue = fractalShape[0] * fractalNum = 64</code></td>
    <td align="center"><code>nAlignValue = fractalShape[0] * fractalNum = 32</code></td>
  </tr>
 <tr>
    <td align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]，L1 -> L0B不需要转置<br>（场景1 / 3）</span></td>
    <td colspan="2" align="center"><code>nAlignValue = fractalShape[0]</code></td>
  </tr>
</table>

<a name="表6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表6：scaleA / scaleB矩阵在L1 / L0上各个轴的对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">scaleA矩阵（L1排布格式为Zz）</span></td>
    <td align="center"><span style="font-weight: bold;">scaleB矩阵（L1排布格式为Nn）</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">m轴 / n轴对齐</span></td>
    <td align="center"><code>scaleMAlignL1 = CeilAlign(m, fractalShape[0])</code></td>
    <td align="center"><code>scaleNAlignL1 = CeilAlign(n, fractalShape[0])</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">k轴对齐</span></td>
    <td align="center"><code>scaleK = AscendC::Std::ceil_div(k, 64) * 2</code></td>
    <td align="center"><code>scaleK = AscendC::Std::ceil_div(k, 64) * 2</code></td>
  </tr>
</table>


### 5. GM到L1（`DataCopy`和`Fill`）

本小节按A矩阵、B矩阵、scaleA矩阵和scaleB矩阵分别说明GM -> L1阶段的`DataCopy`参数配置，以及需要在L1上执行的补零操作。需要重点关注两类约束：

1. scaleA / scaleB在L1 -> L0A_MX / L0B_MX阶段不支持转置，因此需要在GM -> L1阶段提前转换为L0侧需要的排布格式。
2. MX矩阵乘法中`MmadMx`接口要求**k方向向64对齐**，而GM -> L1阶段`DataCopy`的对齐行为与k所在轴有关，需要按场景将剩余k方向数据刷为0，防止脏数据参与计算：
   - k在col方向时，FP8数据类型下`DataCopy`仅将k方向向32B对齐，即向32个元素对齐，需要继续补齐到64个元素对齐。
   - k在row方向时，`DataCopy`将k方向向16对齐，需要继续补齐到64对齐。

下文的`DataCopy（Nd2NzParams）`表示调用`DataCopy`指令并传入`Nd2NzParams`结构体参数，将ND排布转换为Nz大小分形排布。

#### 5.1. A矩阵GM -> L1

##### 5.1.1. A矩阵GM上输入为[m, k]

<div align="center">
  <img src="figures/GM2L1/FP4_A_GM2L1_MK.png" width="900"><br>
  图13：FP4数据类型下，A矩阵[m, k]输入，GM -> L1，ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_A_GM2L1_MK.png" width="900"><br>
  图14：FP8数据类型下，A矩阵[m, k]输入，GM -> L1，ND -> Nz
</div>

**DataCopy（Nd2NzParams）**

- **dValue**：取`packedK`。FP4输入按B8视图搬运，k方向每2个FP4元素对应1个B8视图元素；FP8输入按原始k配置。
- **dstNzC0Stride**：单位为32B，取L1上Nz矩阵对齐后的行数，即`mAlignL1`。
- **FP4处理**：根据接口约束，FP4输入在ND -> Nz过程中按B8视图处理，参数配置也按B8视图设置。

```cpp
AscendC::Nd2NzParams nd2nzA1Params;
nd2nzA1Params.ndNum = 1; // ND矩阵的数目
nd2nzA1Params.nValue = m; // 源操作ND矩阵的行数
nd2nzA1Params.dValue = packedK; // 源操作ND矩阵按B8视图配置的列数
nd2nzA1Params.srcNdMatrixStride = 0; // 源操作数相邻ND矩阵间的起始地址偏移
nd2nzA1Params.srcDValue = packedK;  // 源操作数同一ND矩阵内相邻行的起始地址偏移，按B8视图配置
nd2nzA1Params.dstNzC0Stride = mAlignL1; // ND -> Nz后，同一行数据切分后各段的起始地址间隔，单位32B
nd2nzA1Params.dstNzNStride = 1;     // ND矩阵第x行与第x+1行转Nz后在dst中的偏移
nd2nzA1Params.dstNzMatrixStride = 0; // 目的Nz矩阵中，相邻Nz矩阵起始地址的偏移，配置为0时，无意义
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```

**Fill**

- A矩阵为`[m, k]`输入，且输入数据类型为FP8。
- **补零原因**：k在col方向，`DataCopy`仅将k方向向32B对齐；MX矩阵乘法中`MmadMx`接口要求k方向向64对齐。
- **补零方式**：调用`AscendC::Fill`将k方向尾部1个block的数据刷为0，如图14。

```cpp
if constexpr (AscendC::IsSameType<TA, fp8_e4m3fn_t>::value || AscendC::IsSameType<TA, fp8_e5m2_t>::value) {
    // 将A矩阵L1数据按照uint16类型填0；dst位于A1时，Fill的blockNum单位为32B。
    const uint32_t heightAlign = CeilAlign(m, fractalShape[0]);
    auto padTensor = a1Local.template ReinterpretCast<uint16_t>();
    AscendC::InitConstValueParams<uint16_t> initConstValueParams;
    // repeatTimes表示迭代次数；以row方向做迭代，覆盖m向16对齐后的每一行。
    initConstValueParams.repeatTimes = heightAlign;
    // blockNum表示每次迭代初始化的数据块(32B)个数；这里每次只填充col方向尾部1个32B数据。
    initConstValueParams.blockNum = 1;
    // initValue表示初始化值；无效数据填0，防止参与MmadMx计算。
    initConstValueParams.initValue = 0;
    // dstOffset定位到col方向当前已搬运数据末尾，后续Fill按row方向逐行填充尾部1个32B数据。
    uint32_t dstOffset = heightAlign * (CeilAlign(packedK, SCALE_CEIL_NUMBER) / 2);
    AscendC::Fill(padTensor[dstOffset], initConstValueParams);
}
```
##### 5.1.2. A矩阵GM上输入为[k, m]

<div align="center">
  <img src="figures/GM2L1/FP4_A_GM2L1_TRANS_KM.png" width="700"><br>
  图15：FP4数据类型下，A矩阵[k, m]输入，GM -> L1，ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_A_GM2L1_TRANS_KM.png" width="700"><br>
  图16：FP8数据类型下，A矩阵[k, m]输入，GM -> L1，ND -> Nz
</div>

**DataCopy（Nd2NzParams）**

- **nValue**：取`k`。
- **dValue**：FP4数据类型下取`AscendC::Std::ceil_div(m, 2)`；FP8数据类型下取`m`。
- **dstNzC0Stride**：单位为32B，取L1上Nz矩阵对齐后的行数，即k方向对齐后的长度`alignK`。
- **FP4处理**：根据接口约束，FP4输入在ND -> Nz过程中按B8视图处理，参数配置也按B8视图设置。

```cpp
AscendC::Nd2NzParams nd2nzA1Params;
uint16_t aColValue = isFP4 ? AscendC::Std::ceil_div(m, 2) : m;
nd2nzA1Params.ndNum = 1; // ND矩阵的数目
nd2nzA1Params.nValue = k; // 源操作ND矩阵的行数
nd2nzA1Params.dValue = aColValue; // 源操作ND矩阵按B8视图配置的列数，FP4输入时2个元素对应1个B8视图元素
nd2nzA1Params.srcNdMatrixStride = 0; // 源操作数相邻ND矩阵间的起始地址偏移
nd2nzA1Params.srcDValue = aColValue; // 源操作数同一ND矩阵内相邻行的起始地址偏移，按B8视图配置
nd2nzA1Params.dstNzC0Stride = alignK; // ND -> Nz后，同一行数据切分后各段的起始地址间隔，单位32B
nd2nzA1Params.dstNzNStride = 1;      // ND矩阵第x行与第x+1行转Nz后在dst中的偏移
nd2nzA1Params.dstNzMatrixStride = 0; // 目的Nz矩阵中，相邻Nz矩阵起始地址的偏移，配置为0时，无意义
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```

**Fill**

- A矩阵为`[k, m]`输入。
- **补零原因**：k在row方向，`DataCopy`将k方向向16对齐；MX矩阵乘法中`MmadMx`接口要求k方向向64对齐。
- **补零方式**：调用`AscendC::Fill`将k方向超出原始长度的脏数据刷为0，FP4数据类型如图15，FP8数据类型如图16。

```cpp
// 补齐row方向[k, alignK]范围内的无效数据。
// 将A矩阵L1数据按照uint16类型填0；dst位于A1时，Fill的blockNum和dstGap单位为32B。
auto padTensor = a1Local.template ReinterpretCast<uint16_t>();
AscendC::InitConstValueParams<uint16_t> initConstValueParams;
// repeatTimes表示迭代次数；以col方向做迭代。
initConstValueParams.repeatTimes = AscendC::Std::ceil_div(m, FP8_C0SIZE);
// blockNum表示每次迭代初始化的数据块(32B)个数；这里每次填充row方向尾部无效行数。
initConstValueParams.blockNum = alignK - k;
// dstGap表示前一次迭代结束地址到后一次迭代起始地址的距离；跳过row方向有效数据。
initConstValueParams.dstGap = k;
// initValue表示初始化值；无效数据填0，防止参与MmadMx计算。
initConstValueParams.initValue = 0;
// 起始地址定位到第一个需要补0的row方向的分形。
AscendC::Fill(padTensor[k * fractalShape[0]], initConstValueParams);
```
#### 5.2. B矩阵GM -> L1

##### 5.2.1. B矩阵GM上输入为[k, n]

<div align="center">
  <img src="figures/GM2L1/FP4_B_GM2L1_TRANS_KN.png" width="700"><br>
  图17：FP4数据类型下，B矩阵[k, n]输入，GM -> L1，ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_B_GM2L1_TRANS_KN.png" width="700"><br>
  图18：FP8数据类型下，B矩阵[k, n]输入，GM -> L1，ND -> Nz
</div>

**DataCopy（Nd2NzParams）**

- **nValue**：取`k`。
- **dValue**：FP4数据类型下取`AscendC::Std::ceil_div(n, 2)`；FP8数据类型下取`n`。
- **dstNzC0Stride**：单位为32B，取L1上Nz矩阵对齐后的行数，即k方向对齐后的长度`alignK`。
- **FP4处理**：根据接口约束，FP4输入在ND -> Nz过程中按B8视图处理，参数配置也按B8视图设置。

```cpp
AscendC::Nd2NzParams nd2nzB1Params;
uint16_t bColValue = isFP4 ? AscendC::Std::ceil_div(n, 2) : n;
nd2nzB1Params.ndNum = 1; // ND矩阵的数目
nd2nzB1Params.nValue = k; // 源操作ND矩阵的行数
nd2nzB1Params.dValue = bColValue; // 源操作ND矩阵按B8视图配置的列数，FP4输入时2个元素对应1个B8视图元素
nd2nzB1Params.srcNdMatrixStride = 0; // 源操作数相邻ND矩阵间的起始地址偏移
nd2nzB1Params.srcDValue = bColValue; // 源操作数同一ND矩阵内相邻行的起始地址偏移，按B8视图配置
nd2nzB1Params.dstNzC0Stride = alignK; // ND -> Nz后，同一行数据切分后各段的起始地址间隔，单位32B
nd2nzB1Params.dstNzNStride = 1; // ND矩阵第x行与第x+1行转Nz后在dst中的偏移
nd2nzB1Params.dstNzMatrixStride = 0; // 目的Nz矩阵中，相邻Nz矩阵起始地址的偏移，配置为0时，无意义
AscendC::DataCopy(b1Local, bGM, nd2nzB1Params);
```

**Fill**

- B矩阵为`[k, n]`输入。
- **补零原因**：k在row方向，`DataCopy`将k方向向16对齐；MX矩阵乘法中`MmadMx`接口要求k方向向64对齐。
- **补零方式**：调用`AscendC::Fill`将k方向超出原始长度的脏数据刷为0，FP4数据类型如图17，FP8数据类型如图18。

```cpp
// 补齐row方向[k, alignK]范围内的无效数据。
// 将B矩阵L1数据按照uint16类型填0；dst位于B1时，Fill的blockNum和dstGap单位为32B。
auto padTensor = b1Local.template ReinterpretCast<uint16_t>();
AscendC::InitConstValueParams<uint16_t> initConstValueParams;
// repeatTimes表示迭代次数；以col方向做迭代。
initConstValueParams.repeatTimes = AscendC::Std::ceil_div(n, FP8_C0SIZE);
// blockNum表示每次迭代初始化的数据块(32B)个数；这里每次填充row方向尾部无效行数。
initConstValueParams.blockNum = alignK - k;
// dstGap表示前一次迭代结束地址到后一次迭代起始地址的距离；跳过row方向有效数据。
initConstValueParams.dstGap = k;
// initValue表示初始化值；无效数据填0，防止参与MmadMx计算。
initConstValueParams.initValue = 0;
// 起始地址定位到第一个需要补0的row方向的分形。
AscendC::Fill(padTensor[k * fractalShape[0]], initConstValueParams);
```
##### 5.2.2. B矩阵GM上输入为[n, k]

<div align="center">
  <img src="figures/GM2L1/FP4_B_GM2L1_NK.png" width="800"><br>
  图19：FP4数据类型下，B矩阵[n, k]输入，GM -> L1，ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_B_GM2L1_NK.png" width="800"><br>
  图20：FP8数据类型下，B矩阵[n, k]输入，GM -> L1，ND -> Nz
</div>

**DataCopy（Nd2NzParams）**

- **nValue**：取`n`。
- **dValue**：取`packedK`。FP4输入按B8视图搬运，k方向每2个FP4元素对应1个B8视图元素；FP8输入按原始k配置。
- **dstNzC0Stride**：单位为32B，取L1上Nz矩阵对齐后的行数，即n方向对齐后的长度`nAlignL1`。
- **FP4处理**：根据接口约束，FP4输入在ND -> Nz过程中按B8视图处理，参数配置也按B8视图设置。

```cpp
AscendC::Nd2NzParams nd2nzB1Params;
nd2nzB1Params.ndNum = 1; // ND矩阵的数目
nd2nzB1Params.nValue = n; // 源操作ND矩阵的行数
nd2nzB1Params.dValue = packedK; // 源操作ND矩阵按B8视图配置的列数
nd2nzB1Params.srcNdMatrixStride = 0; // 源操作数相邻ND矩阵间的起始地址偏移
nd2nzB1Params.srcDValue = packedK; // 源操作数同一ND矩阵内相邻行的起始地址偏移，按B8视图配置
nd2nzB1Params.dstNzC0Stride = nAlignL1; // ND -> Nz后，同一行数据切分后各段的起始地址间隔，单位32B
nd2nzB1Params.dstNzNStride = 1; // ND矩阵第x行与第x+1行转Nz后在dst中的偏移
nd2nzB1Params.dstNzMatrixStride = 0; // 目的Nz矩阵中，相邻Nz矩阵起始地址的偏移，配置为0时，无意义
AscendC::DataCopy(b1Local, bGM, nd2nzB1Params);
```

**Fill**

- B矩阵为`[n, k]`输入，且输入数据类型为FP8。
- **补零原因**：k在col方向，`DataCopy`仅将k方向向32B对齐；MX矩阵乘法中`MmadMx`接口要求k方向向64对齐。
- **补零方式**：调用`AscendC::Fill`将k方向尾部1个block的数据刷为0，如图20。

```cpp
if constexpr (AscendC::IsSameType<TB, fp8_e4m3fn_t>::value || AscendC::IsSameType<TB, fp8_e5m2_t>::value) {
    // 将B矩阵L1数据按照uint16类型填0；dst位于B1时，Fill的blockNum单位为32B。
    const uint32_t heightAlign = CeilAlign(n, fractalShape[0]);
    auto padTensor = b1Local.template ReinterpretCast<uint16_t>();
    AscendC::InitConstValueParams<uint16_t> initConstValueParams;
    // repeatTimes表示迭代次数；以row方向做迭代，覆盖n向16对齐后的每一行。
    initConstValueParams.repeatTimes = heightAlign;
    // blockNum表示每次迭代初始化的数据块(32B)个数；这里每次只填充col方向尾部1个32B数据。
    initConstValueParams.blockNum = 1;
    // initValue表示初始化值；无效数据填0，防止参与MmadMx计算。
    initConstValueParams.initValue = 0;
    // dstOffset定位到col方向当前已搬运数据末尾，后续Fill按row方向逐行填充尾部1个32B数据。
    uint32_t dstOffset = heightAlign * (CeilAlign(packedK, SCALE_CEIL_NUMBER) / 2);
    AscendC::Fill(padTensor[dstOffset], initConstValueParams);
}
```
#### 5.3. scaleA矩阵GM -> L1

scaleA矩阵为fp8_e8m0_t数据类型，按fp8_e8m0_t真实数据类型排布时，scaleA在L1上为Zz格式。由于硬件约束，scale矩阵需要K方向2byte连续，`DataCopy`时需将fp8_e8m0_t按B16（half）视图进行搬运（每2个fp8_e8m0_t元素对应1个half元素），此时L1上表现为B16数据类型的Nz排布。搬运方式取决于isAtranspose的取值。

##### 5.3.1. scaleA矩阵GM上输入为[m, scaleK]（isAtranspose=false）

使用`Dn2NzParams`结构体参数（B16视图）搬运。

<div align="center">
  <img src="figures/GM2L1/scaleA_GM2L1_MK.png" width="900"><br>
  图21：scaleA矩阵[m, scaleK]输入，GM -> L1，ND -> Zz
</div>

- **B16视图**：每2个`fp8_e8m0_t`元素按1个half元素搬运。
- **关键参数**：`dValue = m`，`nValue = scaleK / 2`，`dstNzC0Stride = scaleK / 2`。

```cpp
AscendC::GlobalTensor<half> scaleAGMB16;
scaleAGMB16.SetGlobalBuffer((__gm__ half *)(scaleAGM.GetPhyAddr()), m * scaleK / 2);
auto scaleA1LocalB16 = scaleA1Local.ReinterpretCast<half>();

AscendC::Dn2NzParams dn2nzParams;
dn2nzParams.dnNum = 1; // 源操作数中DN矩阵的个数
dn2nzParams.dValue = m; // 源操作DN矩阵的行数
dn2nzParams.nValue = scaleK / 2; // 源操作DN矩阵的列数，按B16视图后2个fp8_e8m0_t合并为1个half
dn2nzParams.srcDnMatrixStride = 0; // 源操作数相邻DN矩阵间的起始地址偏移
dn2nzParams.srcDValue = scaleK / 2; // 源操作数同一DN矩阵内相邻行的起始地址偏移
dn2nzParams.dstNzC0Stride = scaleK / 2; // DN转Nz后，同一行数据切分后各段的起始地址间隔，单位32B
dn2nzParams.dstNzNStride = 1; // DN矩阵第x行与第x+1行转Nz后在dst中的偏移
dn2nzParams.dstNzMatrixStride = 0; // 相邻Nz矩阵间的起始地址偏移
AscendC::DataCopy(scaleA1LocalB16, scaleAGMB16, dn2nzParams);
```

##### 5.3.2. scaleA矩阵GM上输入为[scaleK, m, 2]（isAtranspose=true）

使用`Nd2NzParams`结构体参数（B16视图）搬运。

<div align="center">
  <img src="figures/GM2L1/scaleA_GM2L1_KM.png" width="1000"><br>
  图22：scaleA矩阵[scaleK, m, 2]输入，GM -> L1，ND -> Zz
</div>

- **B16视图**：每2个`fp8_e8m0_t`元素按1个half元素搬运。
- **关键参数**：`nValue = scaleK / 2`，`dValue = m`，`dstNzC0Stride = scaleK / 2`。

```cpp
AscendC::GlobalTensor<half> scaleAGMB16;
scaleAGMB16.SetGlobalBuffer((__gm__ half *)(scaleAGM.GetPhyAddr()), m * scaleK / 2);
auto scaleA1LocalB16 = scaleA1Local.ReinterpretCast<half>();

AscendC::Nd2NzParams nd2nzParams;
nd2nzParams.ndNum = 1; // 源操作数中ND矩阵的个数
nd2nzParams.nValue = scaleK / 2; // 源操作ND矩阵的行数，按B16视图后2个fp8 scale合并为1个half
nd2nzParams.dValue = m; // 源操作ND矩阵的列数
nd2nzParams.srcDValue = m; // 源操作数同一ND矩阵内相邻行的起始地址偏移
nd2nzParams.dstNzC0Stride = scaleK / 2; // ND -> Nz后，同一行数据切分后各段的起始地址间隔，单位32B
nd2nzParams.dstNzNStride = 1; // ND矩阵第x行与第x+1行转Nz后在dst中的偏移
nd2nzParams.dstNzMatrixStride = 0; // 相邻Nz矩阵间的起始地址偏移
AscendC::DataCopy(scaleA1LocalB16, scaleAGMB16, nd2nzParams);
```
#### 5.4. scaleB矩阵GM -> L1

scaleB矩阵的搬运方式与scaleA类似。按fp8_e8m0_t真实数据类型排布时，scaleB在L1上为Nn格式。由于硬件约束，scale矩阵需要k方向2byte连续，同样需按B16视图做`DataCopy`搬运，此时L1上表现为B16数据类型的Nz排布。搬运方式取决于isBtranspose的取值。

##### 5.4.1. scaleB矩阵GM上输入为[scaleK, n, 2]（isBtranspose=false）

使用`Nd2NzParams`结构体参数（B16视图）搬运。

<div align="center">
  <img src="figures/GM2L1/scaleB_GM2L1_KN.png" width="1000"><br>
  图23：scaleB矩阵[scaleK, n, 2]输入，GM -> L1，ND -> Nn
</div>

- **B16视图**：每2个`fp8_e8m0_t`元素按1个half元素搬运。
- **关键参数**：`nValue = scaleK / 2`，`dValue = n`，`dstNzC0Stride = scaleK / 2`。

```cpp
AscendC::GlobalTensor<half> scaleBGMB16;
scaleBGMB16.SetGlobalBuffer((__gm__ half *)(scaleBGM.GetPhyAddr()), n * scaleK / 2);
auto scaleB1LocalB16 = scaleB1Local.ReinterpretCast<half>();

AscendC::Nd2NzParams nd2nzParams;
nd2nzParams.ndNum = 1; // 源操作数中ND矩阵的个数
nd2nzParams.nValue = scaleK / 2; // 源操作ND矩阵的行数，按B16视图后2个fp8 scale合并为1个half
nd2nzParams.dValue = n; // 源操作ND矩阵的列数
nd2nzParams.srcDValue = n; // 源操作数同一ND矩阵内相邻行的起始地址偏移
nd2nzParams.dstNzC0Stride = scaleK / 2; // ND -> Nz后，同一行数据切分后各段的起始地址间隔，单位32B
nd2nzParams.dstNzNStride = 1; // ND矩阵第x行与第x+1行转Nz后在dst中的偏移
nd2nzParams.dstNzMatrixStride = 0; // 相邻Nz矩阵间的起始地址偏移
AscendC::DataCopy(scaleB1LocalB16, scaleBGMB16, nd2nzParams);
```

##### 5.4.2. scaleB矩阵GM上输入为[n, scaleK]（isBtranspose=true）

使用`Dn2NzParams`结构体参数（B16视图）搬运。

<div align="center">
  <img src="figures/GM2L1/scaleB_GM2L1_NK.png" width="1000"><br>
  图24：scaleB矩阵[n, scaleK]输入，GM -> L1，ND -> Nn
</div>

- **B16视图**：每2个`fp8_e8m0_t`元素按1个half元素搬运。
- **关键参数**：`dValue = n`，`nValue = scaleK / 2`，`dstNzC0Stride = scaleK / 2`。

```cpp
AscendC::GlobalTensor<half> scaleBGMB16;
scaleBGMB16.SetGlobalBuffer((__gm__ half *)(scaleBGM.GetPhyAddr()), n * scaleK / 2);
auto scaleB1LocalB16 = scaleB1Local.ReinterpretCast<half>();

AscendC::Dn2NzParams dn2nzParams;
dn2nzParams.dnNum = 1; // 源操作数中DN矩阵的个数
dn2nzParams.dValue = n; // 源操作DN矩阵的行数
dn2nzParams.nValue = scaleK / 2; // 源操作DN矩阵的列数，按B16视图后2个fp8 scale合并为1个half
dn2nzParams.srcDnMatrixStride = 0; // 源操作数相邻DN矩阵间的起始地址偏移
dn2nzParams.srcDValue = scaleK / 2; // 源操作数同一DN矩阵内相邻行的起始地址偏移
dn2nzParams.dstNzC0Stride = scaleK / 2; // DN转Nz后，同一行数据切分后各段的起始地址间隔，单位32B
dn2nzParams.dstNzNStride = 1; // DN矩阵第x行与第x+1行转Nz后在dst中的偏移
dn2nzParams.dstNzMatrixStride = 0; // 相邻Nz矩阵间的起始地址偏移
AscendC::DataCopy(scaleB1LocalB16, scaleBGMB16, dn2nzParams);
```
### 6. L1到L0（`LoadData`）

本小节介绍A / B矩阵从L1搬运到L0A / L0B、scaleA / scaleB矩阵从L1搬运到L0A_MX / L0B_MX时，如何调用`LoadData`指令，并通过`LoadData2DParamsV2`和`LoadData2DMxParams`两个结构体参数完成数据搬运及格式变换。

#### 6.1. `LoadData2DParamsV2`结构体参数说明

`LoadData2DParamsV2`结构体参数用于控制A / B矩阵数据从L1搬运到L0A / L0B（该过程中可以进行转置），包含：

- **sid**：源矩阵标识，默认为0
- **mStartPosition**：源矩阵row方向起始位置，单位为16个元素
- **kStartPosition**：源矩阵col方向起始位置，单位为32B
- **mStep**：源矩阵row方向搬运长度，单位为16个元素
- **kStep**：源矩阵col方向搬运长度，单位为32B
- **srcStride**：源矩阵col方向相邻分形起始地址间隔，单位为512B
- **dstStride**：目标矩阵col方向相邻分形起始地址间隔，单位为512B
- **ifTranspose**：是否启用转置功能，对每个分形矩阵进行转置，默认为false。配置为true时，会完成小分形转置，并同步调整大分形排布。

注意：A / B矩阵数据的分形大小为512B

#### 6.2. `LoadData2DMxParams`结构体参数说明

`LoadData2DMxParams`结构体参数用于控制scale矩阵数据从L1搬运到L0A_MX / L0B_MX（纯搬运，不存在排布格式变化），包含：

- **xStartPosition**：源矩阵row方向起始位置，单位为1个32B分形
- **yStartPosition**：源矩阵col方向起始位置，单位为32B
- **xStep**：源矩阵row方向搬运长度，单位为1个32B分形
- **yStep**：源矩阵col方向搬运长度，单位为32B
- **srcStride**：源矩阵row方向相邻分形（16*2）起始地址间隔，单位为32B
- **dstStride**：目标矩阵row方向相邻分形（16*2）起始地址间隔，单位为32B

注意：scale矩阵数据的分形大小为16\*2\*1=32B

调用一次`LoadData`指令并传入`LoadData2DParamsV2`、`LoadData2DMxParams`两个结构体参数，可同时完成A矩阵到L0A、对应scale矩阵到L0A_MX的搬运。L0A_MX Buffer和L0A的地址是固定比例关系，`LoadData`指令会自行按照L0A的地址进行推导，用户无需配置。B矩阵和scaleB同理：

```cpp
AscendC::LoadData(a2Local, a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
AscendC::LoadData(b2Local, b1Local, scaleB1Local, loadDataParams, loadMxDataParams);
```
#### 6.3. A矩阵 L1 -> L0A，scaleA矩阵 L1 -> L0A_MX

##### 6.3.1. A矩阵 L1 -> L0A 不转置（场景1 / 3）

<div align="center">
  <img src="figures/L12L0/FP4_A_L12L0_MK.png" width="1000"><br>
  图25：FP4数据类型下，A矩阵[m, k]输入，L1 -> L0A不转置，loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/FP8_A_L12L0_MK.png" width="1000"><br>
  图26：FP8数据类型下，A矩阵[m, k]输入，L1 -> L0A不转置，loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/scaleA_l12l0_KM.png" width="500"><br>
  图27：scaleA矩阵 L1 -> L0A_MX
</div>

**关键配置**

- A矩阵L1 -> L0A不转置，`loadDataParams.ifTranspose = false`。
- A矩阵在L1上为Nz排布，`mStep`按row方向分形数配置，`kStep`按col方向32B块数配置。
- `srcStride` / `dstStride`表示源 / 目的矩阵col方向相邻分形起始地址间隔，单位为512B。
- scaleA矩阵通过同一次`LoadData`搬运到L0A_MX，`xStep`对应row方向搬运长度，`yStep`对应col方向搬运长度。

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// 从A矩阵L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// A矩阵不转置输入[m, k]，L1 -> L0A不需要转置
// mStep/kStep分别表示搬运row方向分形数、col方向32B块数
loadDataParams.mStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[1]);
// srcStride/dstStride表示源/目的矩阵col方向相邻分形起始地址间隔，单位512B
loadDataParams.srcStride = AscendC::Std::ceil_div(mAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = false;

AscendC::LoadData2DMxParams loadMxDataParams;
// scaleA同步从L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep配置scaleA的row/col方向搬运长度；stride按row方向相邻分形间隔配置
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleMAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(a2Local, a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
```

##### 6.3.2. A矩阵 L1 -> L0A 转置，单次调用（场景2 / 4）

<div align="center">
  <img src="figures/L12L0/FP4_A_L12L0_TRANS_KM.png" width="900"><br>
  图28：FP4数据类型下，A矩阵[k, m]输入，L1 -> L0A转置，loadDataParams.ifTranspose = true，单次调用LoadData
</div>

<div align="center">
  <img src="figures/L12L0/FP8_A_L12L0_TRANS_KM.png" width="900"><br>
  图29：FP8数据类型下，A矩阵[k, m]输入，L1 -> L0A转置，loadDataParams.ifTranspose = true，单次调用LoadData
</div>

**关键配置**

- A矩阵L1 -> L0A需要转置，`loadDataParams.ifTranspose = true`。
- L1上A矩阵shape为`[kaAlignL1, mAlignL1]`，row方向对应逻辑k维度，col方向对应逻辑m维度。
- `mStep`配置row方向搬运长度，`kStep`配置col方向搬运长度。
- 单次调用时，A矩阵m方向多搬运数据超过1个分形，后续`MmadMx`参数需要配合处理，详见第7节。
- scaleA矩阵仍通过同一次`LoadData`搬运到L0A_MX。

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// 从A矩阵L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// A矩阵转置输入[k, m]，L1 -> L0A需要转置
// 转置搬运时，L1上A矩阵shape为[kaAlignL1, mAlignL1]，row方向对应逻辑k维度，col方向对应逻辑m维度
loadDataParams.mStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[1]);
// srcStride/dstStride表示源/目的矩阵col方向相邻分形起始地址间隔，单位512B
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;

AscendC::LoadData2DMxParams loadMxDataParams;
// scaleA同步从L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep配置scaleA的row/col方向搬运长度；stride按row方向相邻分形间隔配置
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleMAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(a2Local, a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
```

##### 6.3.3. A矩阵 L1 -> L0A 转置，for循环调用（场景5 / 6）

<div align="center">
  <img src="figures/L12L0/FP4_A_L12L0_for_TRANS_KM.png" width="900"><br>
  图30：FP4数据类型下，A矩阵[k, m]输入，L1 -> L0A转置，loadDataParams.ifTranspose = true，for循环调用LoadData
</div>

<div align="center">
  <img src="figures/L12L0/FP8_A_L12L0_for_TRANS_KM.png" width="900"><br>
  图31：FP8数据类型下，A矩阵[k, m]输入，L1 -> L0A转置，loadDataParams.ifTranspose = true，for循环调用LoadData
</div>

<div align="center">
  <img src="figures/L12L0/scaleA_l12l0_KM.png" width="500"><br>
  图32：scaleA矩阵L1 -> L0A_MX，for循环调用LoadData
</div>

**关键配置**

- A矩阵L1 -> L0A需要转置，`loadDataParams.ifTranspose = true`。
- for循环用于避免单次调用时m方向多搬脏数据超过1个分形。
- `mStepAlign`表示每次循环搬运的row方向分形数，FP4取4，FP8取2。
- `L0ALoopNum = AscendC::Std::ceil_div(kaAlignL0, fractalShape[0] * fractalNum)`，`mStartPosition`随`loopIdx`递增。
- scaleA矩阵只在第一次循环搬运到L0A_MX，后续循环设置`xStep = 0`、`yStep = 0`跳过搬运。
- `dstOffset`记录L0A上每次循环的目的地址偏移，每次循环跳过m方向尾部脏数据分形。

```cpp
uint16_t mStepAlign = isFP4 ? FP4_M_STEP_ALIGN : FP8_M_STEP_ALIGN;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// kStartPosition固定为0，每次循环通过mStartPosition选择A矩阵源row方向上的当前分形
loadDataParams.kStartPosition = 0;
// 转置搬运要求mStep按数据类型对齐：FP4为4个分形，FP8为2个分形
loadDataParams.mStep = mStepAlign;
// kStep对应A矩阵源col方向上的32B块数
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL0, fractalShape[1]);
// srcStride/dstStride表示源/目的矩阵col方向相邻分形起始地址间隔，单位512B
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;

AscendC::LoadData2DMxParams loadMxDataParams;
// scaleA的row方向起始分形、col方向起始32B块固定为0
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// srcStride/dstStride表示scaleA源/目的矩阵row方向相邻分形起始地址间隔，单位32B
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

uint32_t dstOffset = 0;
uint16_t L0ALoopNum = AscendC::Std::ceil_div(kaAlignL0, fractalShape[0] * fractalNum);
for (uint16_t loopIdx = 0; loopIdx < L0ALoopNum; ++loopIdx) {
    // mStartPosition递增，A矩阵每次搬运按m方向更新起始地址，scaleA矩阵第一次for循环全部搬运完成，后续for循环不进行搬运
    loadDataParams.mStartPosition = mStepAlign * loopIdx;
    if (loopIdx != 0) {
        loadMxDataParams.xStep = 0;
        loadMxDataParams.yStep = 0;
    } else {
        loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleMAlignL1, fractalShape[0]);
        loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
    }
    AscendC::LoadData(a2Local[dstOffset], a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
    dstOffset += CeilAlign(mAlignL0, fractalShape[0]) * fractalShape[1];
}
```

#### 6.4. B矩阵 L1 -> L0B，scaleB矩阵 L1 -> L0B_MX

B / scaleB矩阵的搬运方式与A / scaleA矩阵类似。差异在于：B矩阵在L0B上为Zn排布，scaleB矩阵在L0B_MX上为Nn排布。

##### 6.4.1. B矩阵 L1 -> L0B 不转置（场景1 / 3）

<div align="center">
  <img src="figures/L12L0/FP4_B_L12L0_NK.png" width="900"><br>
  图33：FP4数据类型下，B矩阵[n, k]输入，L1 -> L0B不转置，loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/FP8_B_L12L0_NK.png" width="900"><br>
  图34：FP8数据类型下，B矩阵[n, k]输入，L1 -> L0B不转置，loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/scaleB_l12l0_KN.png" width="700"><br>
  图35：scaleB矩阵 L1 -> L0B_MX
</div>

**关键配置**

- B矩阵L1 -> L0B不转置，`loadDataParams.ifTranspose = false`。
- L1上B矩阵shape为`[nAlignL1, kbAlignL1]`，row方向对应逻辑n维度，col方向对应逻辑k维度。
- `mStep`配置row方向搬运长度，`kStep`配置col方向搬运长度。
- scaleB矩阵通过同一次`LoadData`搬运到L0B_MX，`xStep`对应row方向搬运长度，`yStep`对应col方向搬运长度。

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// 从B矩阵L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// B矩阵转置输入[n, k]，L1->L0B不需要转置
// mStep/kStep分别表示搬运row方向分形数、col方向32B块数
loadDataParams.mStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[1]);
// srcStride/dstStride表示源/目的矩阵col方向相邻分形起始地址间隔，单位512B
loadDataParams.srcStride = AscendC::Std::ceil_div(nAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = false;

// 配置scaleB从L1->L0B_MX搬运的参数时，可将scaleNAlignL1的方向看做row，scaleK所在的方向看做col
// 即小分形16*2，16在row方向，2在col方向
AscendC::LoadData2DMxParams loadMxDataParams;
// scaleB同步从L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep配置scaleB的row/col方向搬运长度；stride按row方向相邻分形间隔配置
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleNAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(b2Local, b1Local, scaleB1Local, loadDataParams, loadMxDataParams);
```

##### 6.4.2. B矩阵 L1 -> L0B 转置（场景2 / 4 / 5 / 6）

<div align="center">
  <img src="figures/L12L0/FP4_B_L12L0_TRANS_KN.png" width="1000"><br>
  图36：FP4数据类型下，B矩阵[k, n]输入，L1 -> L0B转置，loadDataParams.ifTranspose = true
</div>

<div align="center">
  <img src="figures/L12L0/FP8_B_L12L0_TRANS_KN.png" width="1000"><br>
  图37：FP8数据类型下，B矩阵[k, n]输入，L1 -> L0B转置，loadDataParams.ifTranspose = true
</div>

**关键配置**

- B矩阵L1 -> L0B需要转置，`loadDataParams.ifTranspose = true`。
- L1上B矩阵shape为`[kbAlignL1, nAlignL1]`，row方向对应逻辑k维度，col方向对应逻辑n维度。
- `mStep`配置row方向搬运长度，`kStep`配置col方向搬运长度。
- B矩阵不转置输入`[k, n]`时，n方向多搬运数据不超过1个分形。
- scaleB矩阵通过同一次`LoadData`搬运到L0B_MX。

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// 从B矩阵L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// B矩阵不转置输入[k, n]，L1 -> L0B需要转置
// 转置搬运时，L1上B矩阵shape为[kbAlignL1, nAlignL1]，row方向对应逻辑k维度，col方向对应逻辑n维度
loadDataParams.mStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[1]);
// srcStride/dstStride表示源/目的矩阵col方向相邻分形起始地址间隔，单位512B
loadDataParams.srcStride = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;

// 配置scaleB从L1->L0B_MX搬运的参数时，可将scaleNAlignL1的方向看做row，scaleK所在的方向看做col
// 即小分形16*2，16在row方向，2在col方向
AscendC::LoadData2DMxParams loadMxDataParams;
// scaleB同步从L1源操作数的row方向第0个分形、col方向第0个32B块开始搬运
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep配置scaleB的row/col方向搬运长度；stride按row方向相邻分形间隔配置
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleNAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(b2Local, b1Local, scaleB1Local, loadDataParams, loadMxDataParams);
```

### 7. 矩阵乘（`MmadMx`）

MX矩阵乘法公式为`C = (scaleA ⊗ A) * (scaleB ⊗ B)`，`MmadMx`接口会自动完成左 / 右矩阵与对应scale矩阵的广播乘法，K方向上每32个元素共享一个量化因子。

**关键配置**

- `mmadParams.k`设置为`alignK = CeilAlign(k, 64) = 128`，满足MX矩阵乘法中`MmadMx`接口对k方向64对齐的要求。该约束来自MX矩阵乘k方向scale粒度：每64个k元素对应2个scale因子，`scaleK = AscendC::Std::ceil_div(k, 64) * 2`；m / n方向不需要按64对齐。
- `mmadParams.cmatrixInitVal = true`表示初始化C矩阵。
- 场景2 / 4中，A矩阵转置输入`[k, m]`且单次调用`LoadData`，m方向多搬脏数据超过1个分形。因此`mmadParams.m`设置为`CeilAlign(m, fractalShape[0] * fractalNum)`，让多搬的分形参与计算，后续`Fixpipe`搬出时跳过无效分形对应的结果。
- 其余场景中，`mmadParams.m = m`。场景5 / 6虽然A矩阵也转置输入，但for循环方式已避免m方向多搬脏数据超过1个分形。

```cpp
AscendC::MmadParams mmadParams;
if constexpr (scenarioNum == 2 || scenarioNum == 4) {
    // mmad默认m轴向16对齐，但由于A转置过程m轴向fractalShape[0]*fractalNum对齐，
    // 此处按fractalShape[0] * fractalNum对齐，让多搬分形参与计算，
    // 后续Fixpipe搬出时跳过无效分形对应的结果。
    mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum);
} else {
    mmadParams.m = m;
}
mmadParams.n = n;
// MX矩阵乘法要求k方向按64对齐。
mmadParams.k = alignK;
// 初始化C矩阵。
mmadParams.cmatrixInitVal = true;
AscendC::MmadMx(c1Local, a2Local, b2Local, mmadParams);
```

### 8. L0C到GM（`Fixpipe`）

`FixpipeParamsArch3510`是dav-3510架构专用的Fixpipe参数结构体，`CO2Layout`设置为`ROW_MAJOR`表示输出为行主序ND格式。

**关键配置**

- `fixpipeParams.nSize = n`、`fixpipeParams.mSize = m`，只搬出C矩阵有效区域。
- `fixpipeParams.dstStride = n`，表示目标ND矩阵中相邻行的起始地址偏移。
- `fixpipeParams.srcStride`单位是元素，表示源Nz矩阵中相邻Z排布的起始地址偏移。
- 场景2 / 4中，`srcStride`需要与`MmadMx`的m对齐值一致，取`CeilAlign(m, fractalShape[0] * fractalNum)`。
- 其余场景中，`srcStride = CeilAlign(m, fractalShape[0])`。

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.nSize = n;
fixpipeParams.mSize = m;
if constexpr (scenarioNum == 2 || scenarioNum == 4){
    // 场景2/4：A矩阵转置输入[k, m]单次调用，m方向多搬脏数据超过1个分形
    fixpipeParams.srcStride = CeilAlign(m, fractalShape[0] * fractalNum);
} else {
    fixpipeParams.srcStride = CeilAlign(m, fractalShape[0]);
}
fixpipeParams.dstStride = n;
AscendC::Fixpipe<U, U, AscendC::CFG_ROW_MAJOR>(cGM, c1Local, fixpipeParams);
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
- 安装Python依赖

  本样例的`gen_data.py`脚本需要使用`ml_dtypes`生成FP8输入数据，使用`en_dtypes`生成FP4输入数据。建议安装如下版本：

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

  如果未安装上述依赖，脚本无法正确生成FP4/FP8类型的输入数据和真值数据，可能导致精度校验失败。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
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
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4`、`5`、`6` | 场景编号 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
