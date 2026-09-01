# load_data_l12l0_950样例

## 概述

本样例展示如何调用`asc_copy_l12l0a` / `asc_copy_l12l0b`及其`_transpose`接口，将A / B矩阵从L1搬运到L0A / L0B Buffer。

样例覆盖int8_t、half、float三种输入数据类型，以及A / B矩阵转置和不转置输入场景。L0A数据排布为Nz格式，L0B数据排布为Zn格式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── load_data_l12l0_950
│   ├── figures                          // 图示
│   ├── scripts
│   │   ├── gen_data.py                  // 输入数据和真值数据生成脚本
│   │   └── verify_result.py             // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                   // 编译工程文件
│   ├── data_utils.h                     // 数据读入写出函数
│   ├── load_data_l12l0_950.asc          // SIMD C API样例实现 & 调用样例
│   └── README.md                        // 样例说明文档
```

## 样例描述

### 1. 整体流程

样例整体流程如下：

```text
GM(ND) ----------> L1(Nz) -> L0A(Nz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
          │               │                  │          │
asc_copy_gm2l1_nd2nz   asc_copy_l12l0*    asc_mmad   asc_copy_l0c2gm
```

**步骤详解**：

1. **GM -> L1**：使用`asc_set_gm2l1_nz_para`配置Nz排布参数，使用`asc_copy_gm2l1_nd2nz`实现ND到Nz格式转换。
2. **L1 -> L0A / L0B**：使用`asc_copy_l12l0a` / `asc_copy_l12l0b`进行非转置搬运，使用对应的`_transpose`接口进行转置搬运。
3. **矩阵乘**：使用`asc_mmad`接口执行矩阵乘法。
4. **L0C -> GM**：使用`asc_set_l0c_copy_nz_para`配置Nz到ND格式转换参数，使用`asc_copy_l0c2gm`接口搬出结果。

本样例固定矩阵乘规格为`[m, n, k] = [40, 50, 70]`，输出C矩阵为`[40, 50]`，ND格式。核函数通过`scenario_num`选择不同场景。

### 2. 场景关键差异

各场景的输入shape、转置标志和L1 -> L0搬运方式见[表1](#表1)。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：scenario_num不同取值的含义</span></caption>
  <tr>
    <td ><span style="font-weight: bold;">scenario_num</span></td>
    <td><span style="font-weight: bold;">输入数据类型</span></td>
    <td><span style="font-weight: bold;">输出数据类型</span></td>
    <td><span style="font-weight: bold;">输入shape</span></td>
    <td><span style="font-weight: bold;">is_a_transpose</span></td>
    <td><span style="font-weight: bold;">is_b_transpose</span></td>
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
    <td><code>asc_copy_l12l0a</code> / <code>asc_copy_l12l0b</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>是</td>
    <td><code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="2" >half</td>
    <td rowspan="2" >float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td><code>asc_copy_l12l0a</code> / <code>asc_copy_l12l0b</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td><code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td rowspan="2" >float</td>
    <td rowspan="2" >float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td><code>asc_copy_l12l0a</code> / <code>asc_copy_l12l0b</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td><code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td rowspan="1" >int8_t</td>
    <td rowspan="1" >int32_t</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td>for循环 + <code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
</table>

下面按搬运模式说明场景关键差异。

**C API不转置搬运：场景1 / 3 / 5**

- A矩阵不转置输入`[m, k]`，B矩阵转置输入`[n, k]`。
- L1 -> L0A / L0B均不需要转置，分别调用`asc_copy_l12l0a` / `asc_copy_l12l0b`。

**C API转置搬运，单次调用：场景2 / 4 / 6**

- A矩阵转置输入`[k, m]`，B矩阵不转置输入`[k, n]`。
- L1 -> L0A / L0B均需要转置，分别调用`asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose`。
- 场景2中，int8_t转置单次搬运会导致m方向多搬多算脏数据分形，`asc_copy_l0c2gm`通过参数`m_size = m`只搬出有效区域。

**C API转置搬运，for循环调用：场景7**

- 场景7与场景2同为int8_t转置输入。
- A矩阵采用for循环调用`asc_copy_l12l0a_transpose`，在写入L0A时跳过m方向尾部脏数据分形，使矩阵计算时没有额外脏数据分形参与。

### 3. 参数说明

后续代码和参数说明会反复使用分形和对齐相关变量，本节先集中定义这些概念。

- `fractal_shape`：小分形的shape。B8 / B16 / B32输入数据类型的shape为`[16, 32 / sizeof(T)]`，其中`T`表示输入数据类型。本样例涉及的数据类型分形相关信息见[表2](#表2)。
- `fractal_size`：1个小分形包含的元素个数，具体见[表2](#表2)。
- `fractal_num`：当从L1 -> L0A / L0B需要转置时，`asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose`会按正方形矩阵转置。需要多个连续小分形合并为一个正方形矩阵时，`fractal_num`表示该正方形矩阵包含的小分形个数，具体见[表2](#表2)。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：不同数据类型分形相关信息</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractal_shape</span></td>
    <td align="center"><span style="font-weight: bold;">fractal_size</span></td>
    <td align="center"><span style="font-weight: bold;">fractal_num</span></td>
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

- `ceil_align`：向上对齐操作。例如`m = 30`时，`ceil_align(30, 16) = 32`，表示将m轴向16对齐，对齐后m轴长度为32。

  ```cpp
  #define ceil_div(value, align) (((value) + (align) - 1) / (align))
  #define ceil_align(value, align) (ceil_div((value), (align)) * (align))
  ```

- `ceil_div`：向上取整除法，一般用于求解向上对齐后的循环次数。
- `m_align_value`：m轴向`m_align_value`对齐。例如`m_align_value = 32`，代表m轴对齐到32；依次类推还有`n_align_value`、`ka_align_value`、`kb_align_value`。
- `m_align_l1`和`m_align_l0`：A矩阵分别在L1和L0A上时，m轴对齐后的值。依次类推还有`n_align_l1`、`n_align_l0`、`ka_align_l1`、`ka_align_l0`、`kb_align_l1`、`kb_align_l0`。

### 4. 对齐要求

A / B矩阵在L1和L0上各轴的对齐要求不同，后续配置`asc_copy_l12l0a` / `asc_copy_l12l0b`、`asc_mmad`和`asc_copy_l0c2gm`参数时会用到这些值。[表3](#表3)、[表4](#表4)总结scenario_num取1-6时A / B矩阵在L1和L0上的对齐要求；[表5](#表5)、[表6](#表6)总结scenario_num取7时的对齐要求。

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：A、B矩阵在L1上各个轴对齐要求（L1排布格式为Nz）</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractal_num=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractal_num=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractal_num=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]</span></td>
    <td colspan="3" align="center">m_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >ka_align_value = fractal_shape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td colspan="2" align="center">ka_align_value = fractal_shape[0] * fractal_num</td>
    <td colspan="1" align="center">ka_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >m_align_value = fractal_shape[1]</td>
    <td colspan="1" align="center" >m_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td colspan="2" align="center">kb_align_value = fractal_shape[0] * fractal_num</td>
    <td colspan="1" align="center">kb_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >n_align_value = fractal_shape[1]</td>
    <td colspan="1" align="center" >n_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]</span></td>
    <td colspan="3" align="center">n_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kb_align_value = fractal_shape[1]</td>
  </tr>
</table>

<a name="表4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表4：A、B矩阵在L0上各个轴对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractal_num=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractal_num=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractal_num=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]，L1 -> L0A不需要转置</span></td>
    <td colspan="3" align="center">m_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >ka_align_value = fractal_shape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置</span></td>
    <td colspan="2" align="center">ka_align_value = fractal_shape[1]</td>
    <td >ka_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >m_align_value = fractal_shape[0] * fractal_num</td>
    <td align="center" >m_align_value = fractal_shape[0]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1 -> L0B需要转置</span></td>
    <td colspan="2" align="center">kb_align_value = fractal_shape[1]</td>
      <td align="center">kb_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
  <tr>
    <td colspan="2" align="center">n_align_value = fractal_shape[0] * fractal_num</td>
    <td align="center" >n_align_value = fractal_shape[0]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]，L1 -> L0B不需要转置</span></td>
    <td colspan="3" align="center">n_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kb_align_value = fractal_shape[1]</td>
  </tr>
</table>

当scenario_num=7时，A矩阵用for循环 + `asc_copy_l12l0a_transpose`实现L1 -> L0A搬运，L0A只写入有效数据按分形对齐后的区域。

<a name="表5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表5：scenario_num=7，A、B矩阵在L1上各个轴对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractal_num=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td align="center" >ka_align_value = fractal_shape[0] * fractal_num</td>
  </tr>
    <tr>
    <td align="center" >m_align_value = fractal_shape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td align="center" >kb_align_value = fractal_shape[0] * fractal_num</td>
  </tr>
    <tr>
    <td align="center" >n_align_value = fractal_shape[1]</td>
  </tr>
</table>

<a name="表6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表6：scenario_num=7，A、B矩阵在L0上各个轴对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractal_num=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1 -> L0A需要转置</span></td>
    <td align="center" >m_align_value = fractal_shape[0]</td>
  </tr>
    <tr>
    <td align="center" >ka_align_value = fractal_shape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1 -> L0B需要转置</span></td>
    <td align="center" >kb_align_value = fractal_shape[1]</td>
  </tr>
    <tr>
    <td align="center" >n_align_value = fractal_shape[0] * fractal_num</td>
  </tr>
</table>


### 5. L1 -> L0 C API

A / B矩阵在L1上均为Nz排布，在L0A、L0B上分别为Nz、Zn排布。L1 -> L0阶段根据目标存储位置和是否转置，调用`asc_copy_l12l0a`、`asc_copy_l12l0b`、`asc_copy_l12l0a_transpose`或`asc_copy_l12l0b_transpose`。

阅读本章时可先按[表1](#表1)确定场景，再按[表3](#表3)-[表6](#表6)选择对齐值。上述C API重点关注以下参数：

- 接口名：无`_transpose`后缀的接口执行非转置搬运，带`_transpose`后缀的接口执行转置搬运。
- `m_step`、`k_step`：分别表示本次搬运覆盖的row方向、col方向小分形个数。转置搬运时，B8 / B32需要按方型分形组合搬运，相关步长需满足`fractal_num`要求。
- `src_stride`、`dst_stride`：分别表示L1源矩阵和L0目标矩阵row方向相邻小分形之间的间隔。
- `m_start_position`、`k_start_position`：表示本次搬运在L1源矩阵中的起始小分形位置。场景7中通过循环修改`m_start_position`，分段搬运A矩阵。

#### 5.1. A矩阵 L1 -> L0A 不转置

L1 -> L0A不转置时，B8 / B16 / B32三种数据类型的参数配置基本相同，仅`fractal_shape`不同，可参考[表2](#表2)。以int8_t为例展示图示。

<div align="center">
  <img src="figures/B8_A_l1_l0A_copy_l12l0.png" width="800"><br>
  图1: int8_t数据类型下，L1 -> L0A不转置，调用asc_copy_l12l0a数据排布示意图
</div>

参数配置要点：

- `m_step = ceil_div(m_align_l1, fractal_shape[0])`，表示row方向搬运m轴对应的小分形个数。
- `k_step = ceil_div(ka_align_l1, fractal_shape[1])`，表示col方向搬运k轴对应的小分形个数。
- 调用`asc_copy_l12l0a`，表示只完成L1 Nz到L0A Nz的排布搬运，不做转置。

```cpp
m_align_l1 = ceil_align(m, fractal_shape[0]);  // 48
ka_align_l1 = ceil_align(k, fractal_shape[1]); // 96
m_align_l0 = ceil_align(m, fractal_shape[0]);  // 48
ka_align_l0 = ceil_align(k, fractal_shape[1]); // 96
asc_copy_l12l0a(a_l0, a_l1, 0, 0,
                ceil_div(m_align_l1, fractal_shape[0]),  // m_step = 3
                ceil_div(ka_align_l1, fractal_shape[1]), // k_step = 3
                ceil_div(m_align_l1, fractal_shape[0]),  // src_stride = 3
                ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
```

#### 5.2. A矩阵 L1 -> L0A 转置

L1 -> L0A需要转置时调用`asc_copy_l12l0a_transpose`。该接口会按方型分形组合完成小分形转置和L0A Nz排布写入；不同数据类型的方型分形组合方式不同，下面按B8 / B16 / B32分别说明。

##### 5.2.1. B8输入数据类型

B8输入数据类型分形为16 * 32，L1 -> L0转置时会按照方型将row方向的2个16 * 32分形拼成1个32 * 32方型进行转置。下面以int8_t为例说明单次调用和for循环调用两种方式。

**单次调用**

调用一次`asc_copy_l12l0a_transpose`完成L1 -> L0A搬运和转置的图示如下：

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_copy_l12l0.png" width="800"><br>
  图2: int8_t数据类型下，L1 -> L0A转置，调用一次asc_copy_l12l0a_transpose数据排布示意图
</div>

本样例`m = 40`，B8转置搬运时`m_align_l1 = ceil_align(m, fractal_shape[1]) = 64`，`m_align_l1 - m = 24 > 16`。单次调用`asc_copy_l12l0a_transpose`时，m方向会多搬1个无效分形；`asc_mmad`计算时需要设置`m = ceil_align(m, fractal_shape[0] * fractal_num)`让该分形参与计算，最后由`asc_copy_l0c2gm`通过`m_size = m`只搬出有效区域。

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 96
m_align_l1 = ceil_align(m, fractal_shape[1]);                // 64
m_align_l0 = ceil_align(m, fractal_shape[0] * fractal_num);  // 64
ka_align_l0 = ceil_align(k, fractal_shape[1]);               // 96
asc_copy_l12l0a_transpose(a_l0, a_l1, 0, 0,
                          ceil_div(ka_align_l1, fractal_shape[0]), // m_step = 6
                          ceil_div(m_align_l1, fractal_shape[1]),  // k_step = 2
                          ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 6
                          ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 4
```

**for循环调用**

for循环调用多次`asc_copy_l12l0a_transpose`完成L1 -> L0A搬运和转置的图示如下：

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_for_copy_l12l0.png" width="800"><br>
  图3: int8_t数据类型下，L1 -> L0A转置，for循环调用多次asc_copy_l12l0a_transpose数据排布示意图
</div>

for循环调用时，沿L1 row方向（A矩阵k轴）分段搬运，每次搬运k轴方向2个分形、m轴方向`ceil_div(m_align_l0, fractal_shape[1])`个分形。`dst_stride`按m方向有效数据向`fractal_shape[0]`对齐配置，写入L0A时跳过转置多读的m方向脏数据分形，使`asc_mmad`计算时m方向没有额外脏数据分形参与。

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 96
m_align_l1 = ceil_align(m, fractal_shape[1]);                // 64
m_align_l0 = ceil_align(m, fractal_shape[0]);                // 48
ka_align_l0 = ceil_align(k, fractal_shape[1]);               // 96
// 输入为int8类型，A矩阵[k,m]转置输入，L1 -> L0A需要转置
// for循环调用asc_copy_l12l0a_transpose，以k轴方向做for循环，每次循环在L1的k方向搬运2个分形，在L0A上跳过m方向尾脏数据分形，m方向多搬运数据不超过1个分形
uint16_t a_loop_num = ceil_div(ka_align_l0, fractal_shape[0] * fractal_num); // 3
uint32_t dst_offset = 0;
for (uint16_t loopIdx = 0; loopIdx < a_loop_num; ++loopIdx) {
    asc_copy_l12l0a_transpose(a_l0 + dst_offset, a_l1,
                              int8_m_step_align * loopIdx, 0,          // m_start_position, k_start_position
                              int8_m_step_align,                       // m_step = 2
                              ceil_div(m_align_l0, fractal_shape[1]),  // k_step = 2
                              ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 6
                              ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
    dst_offset += ceil_align(m_align_l0, fractal_shape[0]) * fractal_shape[1]; // 1536
}
```

##### 5.2.2. B16输入数据类型

B16输入数据类型分形为16 * 16，一个分形即一个方型，L1 -> L0转置时会按照小分形进行转置，调用一次`asc_copy_l12l0a_transpose`即可完成L1 -> L0A数据搬运及转置。本样例以half为例展示图示。

<div align="center">
  <img src="figures/B16_A_l1_l0A_trans_copy_l12l0.png" width="800"><br>
  图4: half数据类型下，L1 -> L0A转置，调用一次asc_copy_l12l0a_transpose数据排布示意图
</div>

参数配置要点：

- `m_step = ceil_div(ka_align_l1, fractal_shape[0])`，表示row方向搬运k轴对应的小分形个数。
- `k_step = ceil_div(m_align_l1, fractal_shape[1])`，表示col方向搬运m轴对应的小分形个数。
- 调用`asc_copy_l12l0a_transpose`，表示搬运到L0A时完成转置。

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 80
m_align_l1 = ceil_align(m, fractal_shape[1]);                // 48
m_align_l0 = ceil_align(m, fractal_shape[0] * fractal_num);  // 48
ka_align_l0 = ceil_align(k, fractal_shape[1]);               // 80
asc_copy_l12l0a_transpose(a_l0, a_l1, 0, 0,
                          ceil_div(ka_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(m_align_l1, fractal_shape[1]),  // k_step = 3
                          ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
```

##### 5.2.3. B32输入数据类型

B32输入数据类型分形为16 * 8，L1 -> L0转置时会按照方型将col方向的2个16 * 8分形拼成1个16 * 16方型进行转置。本样例以float为例展示图示。

<div align="center">
  <img src="figures/B32_A_l1_l0A_trans_copy_l12l0.png" width="800"><br>
  图5: float数据类型下，L1 -> L0A转置，调用一次asc_copy_l12l0a_transpose数据排布示意图
</div>

本样例`m = 40`。B32转置搬运时需要将col方向2个分形拼成方型，**`k_step`必须是2的倍数**，因此L1上col方向（m方向）会多读1个无效分形。写入L0A后，k方向尾部会多1个无效分形；由于L0A为Nz排布，`asc_mmad`配置`k = matrix_k`即可只让有效k轴数据参与计算。

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0]);               // 80
m_align_l1 = ceil_align(m, fractal_shape[1] * fractal_num);  // 48
m_align_l0 = ceil_align(m, fractal_shape[0]);                // 48
ka_align_l0 = ceil_align(k, fractal_shape[1] * fractal_num); // 80
asc_copy_l12l0a_transpose(a_l0, a_l1, 0, 0,
                          ceil_div(ka_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(m_align_l1, fractal_shape[1]),  // k_step = 6
                          ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
```

#### 5.3. B矩阵 L1 -> L0B 不转置

L1 -> L0B不转置时，B8 / B16 / B32三种数据类型的参数配置基本相同，仅`fractal_shape`不同，可参考[表2](#表2)。以float为例展示图示。

<div align="center">
  <img src="figures/B32_B_l1_l0B_copy_l12l0.png" width="1100"><br>
  图6: float数据类型下，L1 -> L0B不转置，调用一次asc_copy_l12l0b数据排布示意图
</div>

参数配置要点：

- `m_step = ceil_div(n_align_l1, fractal_shape[0])`，表示row方向搬运n轴对应的小分形个数。
- `k_step = ceil_div(kb_align_l1, fractal_shape[1])`，表示col方向搬运k轴对应的小分形个数。
- 调用`asc_copy_l12l0b`，表示只完成L1 Nz到L0B Zn的排布搬运，不做转置。

```cpp
n_align_l1 = ceil_align(n, fractal_shape[0]);  // 64
kb_align_l1 = ceil_align(k, fractal_shape[1]); // 72
kb_align_l0 = ceil_align(k, fractal_shape[1]); // 72
n_align_l0 = ceil_align(n, fractal_shape[0]);  // 64
asc_copy_l12l0b(b_l0, b_l1, 0, 0,
                ceil_div(n_align_l1, fractal_shape[0]),  // m_step = 4
                ceil_div(kb_align_l1, fractal_shape[1]), // k_step = 9
                ceil_div(n_align_l1, fractal_shape[0]),  // src_stride = 4
                ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
```

#### 5.4. B矩阵 L1 -> L0B 转置

L1 -> L0B需要转置时调用`asc_copy_l12l0b_transpose`。该接口会按方型分形组合完成小分形转置和L0B Zn排布写入；不同数据类型的方型分形组合方式不同，下面按B8 / B16 / B32分别说明。

##### 5.4.1. B8输入数据类型

B8输入数据类型分形为16 * 32，L1 -> L0转置时会按照方型将row方向的2个16 * 32的分形拼成1个32 * 32的方型进行转置。本样例以int8_t为例展示图示。

<div align="center">
  <img src="figures/B8_B_l1_l0B_trans_copy_l12l0.png" width="1100"><br>
  图7: int8_t数据类型下，L1 -> L0B转置，调用一次asc_copy_l12l0b_transpose数据排布示意图
</div>

本样例`k = 70`。B8转置搬运时需要将row方向2个分形拼成方型，**`m_step`必须是2的倍数**，因此L1上row方向（k方向）会多读1个无效分形。

```cpp
kb_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 96
n_align_l1 = ceil_align(n, fractal_shape[1]);                // 64
kb_align_l0 = ceil_align(k, fractal_shape[1]);               // 96
n_align_l0 = ceil_align(n, fractal_shape[0] * fractal_num);  // 64
asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0,
                          ceil_div(kb_align_l1, fractal_shape[0]), // m_step = 6
                          ceil_div(n_align_l1, fractal_shape[1]),  // k_step = 2
                          ceil_div(kb_align_l1, fractal_shape[0]), // src_stride = 6
                          ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
```

##### 5.4.2. B16输入数据类型

B16输入数据类型分形为16 * 16，一个分形即一个方型，L1 -> L0转置时会按照小分形进行转置，调用一次`asc_copy_l12l0b_transpose`即可完成L1 -> L0B数据搬运及转置。本样例以half为例展示图示。

<div align="center">
  <img src="figures/B16_B_l1_l0B_trans_copy_l12l0.png" width="800"><br>
  图8: half数据类型下，L1 -> L0B转置，调用一次asc_copy_l12l0b_transpose数据排布示意图
</div>

参数配置要点：

- `m_step = ceil_div(kb_align_l1, fractal_shape[0])`，表示row方向搬运k轴对应的小分形个数。
- `k_step = ceil_div(n_align_l1, fractal_shape[1])`，表示col方向搬运n轴对应的小分形个数。
- 调用`asc_copy_l12l0b_transpose`，表示搬运到L0B时完成转置。

```cpp
kb_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 80
n_align_l1 = ceil_align(n, fractal_shape[1]);                // 64
kb_align_l0 = ceil_align(k, fractal_shape[1]);               // 80
n_align_l0 = ceil_align(n, fractal_shape[0] * fractal_num);  // 64
asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0,
                          ceil_div(kb_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(n_align_l1, fractal_shape[1]),  // k_step = 4
                          ceil_div(kb_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
```

##### 5.4.3. B32输入数据类型

B32输入数据类型分形为16 * 8，L1 -> L0转置时会按照方型将col方向的2个16 * 8的分形拼成1个16 * 16的方型进行转置。本样例以float为例展示图示。

<div align="center">
  <img src="figures/B32_B_l1_l0B_trans_copy_l12l0.png" width="1100"><br>
  图9: float数据类型下，L1 -> L0B转置，调用一次asc_copy_l12l0b_transpose数据排布示意图
</div>

本样例`n = 50`。B32转置搬运时需要将col方向2个分形拼成方型，**`k_step`必须是2的倍数**，因此L1上col方向（n方向）会多读1个无效分形。写入L0B后，k方向尾部会多1个无效分形；由于L0B为Zn排布，`asc_mmad`配置`k = matrix_k`即可只让有效k轴数据参与计算。

```cpp
kb_align_l1 = ceil_align(k, fractal_shape[0]);               // 80
n_align_l1 = ceil_align(n, fractal_shape[1] * fractal_num);  // 64
kb_align_l0 = ceil_align(k, fractal_shape[1] * fractal_num); // 80
n_align_l0 = ceil_align(n, fractal_shape[0]);                // 64
asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0,
                          ceil_div(kb_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(n_align_l1, fractal_shape[1]),  // k_step = 8
                          ceil_div(kb_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
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
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenario_num=$SCENARIO_NUM output/output.bin output/golden.bin
  ```
  使用NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`-`7` | 场景编号：不同数据类型和转置组合 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
