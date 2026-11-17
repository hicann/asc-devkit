# 基于Tensor API实现的QuantMatmul样例

## 概述

本样例基于Tensor API编程方式实现QuantMatmul计算。样例采用`__mix__`核函数模式，AIC（Cube核）和两个AIV（Vector核）并行运行在同一个物理核上。AIC执行矩阵乘法，AIV执行数据类型转换、per-channel反量化、per-token反量化和激活函数FastGelu计算，两者通过`sync_block_arrive`/`sync_block_wait`实现同步。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.3.0 |

## 目录结构介绍

```text
├── quant_matmul
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py            // 真值对比文件
│   ├── CMakeLists.txt                  // 编译工程文件
│   ├── data_utils.h                    // 数据读取写出函数
│   ├── quant_matmul.asc                // Ascend C样例实现与调用样例
│   ├── README.md                       // 中文样例说明文档
│   └── README_en.md                    // 英文样例说明文档
```

## 样例描述

- 样例功能：
  本样例实现QuantMatmul计算，并对结果进行激活函数FastGelu计算。

  QuantMatmul的计算公式为：

  $$
  T = x \times y^T
  $$

  $$
  D_{i,j} = T_{i,j} \times \mathrm{channel\_scale}[j] \times \mathrm{token\_scale}[i]
  $$

  $$
  C_{i,j} = \operatorname{FastGelu}\left(D_{i,j}\right)
  $$

  其中，`channel_scale[j]`对应矩阵乘结果的第j列，`token_scale[i]`对应矩阵乘结果的第i行。

  FastGelu的计算公式为：

  $$
  \operatorname{FastGelu}(x) = \frac{x}{1 + e^{-1.702x}}
  $$

  - x：左矩阵，形状为[m, k]，数据类型为int8，ND排布格式；
  - y：右矩阵，逻辑形状为[n, k]，实际按转置形式存储，参与计算时等价于$y^T$，数据类型为int8，DN排布格式；
  - channel_scale：右矩阵的per-channel反量化scale，形状为[n]，数据类型为float。`channel_scale[j]`用于矩阵乘结果的第j列；
  - token_scale：左矩阵的per-token反量化scale，形状为[m]，数据类型为float。`token_scale[i]`用于矩阵乘结果的第i行；
  - z：输出矩阵，存放反量化和激活后的结果，形状为[m, n]，数据类型为float。

  矩阵乘结果`T[i, j]`的反量化方式为：先将`T[i, j]`从int32转换为float，再乘以`channel_scale[j]`和`token_scale[i]`，恢复量化前的数值尺度。

- 样例规格：

  默认shape为m=1024、n=1024、k=256，Kernel启动的Block数量为32。

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">QuantMatmul</td></tr>
<tr><td rowspan="5" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x（矩阵A）</td><td align="center">[m, k]</td><td align="center">int8</td><td align="center">ND</td></tr>
<tr><td align="center">y（矩阵B，转置存储）</td><td align="center">[n, k]</td><td align="center">int8</td><td align="center">DN</td></tr>
<tr><td align="center">channel_scale（per-channel反量化scale）</td><td align="center">[n]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">token_scale（per-token反量化scale）</td><td align="center">[m]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z（矩阵C）</td><td align="center">[m, n]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">quant_matmul_custom</td></tr>
</table>

- 调用实现：
  Host侧根据总shape和单核切分计算Block数量，并使用内核调用符<<<>>>调用核函数。

### 参数说明

编译期参数默认值：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `base_m` | 128 | M维度基础分块大小 |
| `base_n` | 128 | N维度基础分块大小 |
| `base_k` | 64 | K维度基础分块大小 |
| `step_m` | 1 | 左矩阵在L1中M方向缓存的base_m块数 |
| `step_n` | 1 | 右矩阵在L1中N方向缓存的base_n块数 |
| `step_k` | 4 | 矩阵在L1中K方向缓存的base_k块数 |
| `vector_len` | 64 | AIV一次Vector计算的最大元素数 |
| `pipe_depth` | 2 | UB slot数量 |

运行时参数默认值如下：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `m` | 1024 | 矩阵A的行数 |
| `n` | 1024 | 矩阵B的列数 |
| `k` | 256 | 矩阵A的列数/矩阵B的行数 |
| `single_core_m` | 256 | 单核M维度大小 |
| `single_core_n` | 128 | 单核N维度大小 |
| `single_core_k` | 256 | 单核K维度大小 |
| `num_blocks` | 32 | Kernel启动的Block数量，由M/N切分自动计算 |

参数约束如下：

```text
m % single_core_m == 0
single_core_m % base_m == 0
n % single_core_n == 0
single_core_n % base_n == 0
k % base_k == 0
base_m % 2 == 0
base_n % vector_len == 0
```

其中`m % single_core_m == 0`和`single_core_m % base_m == 0`表示本样例只支持完整`base_m`分块，`n % single_core_n == 0`和`single_core_n % base_n == 0`表示只支持完整`base_n`分块，`k % base_k == 0`表示只支持完整`base_k`分块。`base_m % 2 == 0`来自`dual_dst_mode::split_m`：每个输出tile的M维需要平均拆给两个AIV。`base_n % vector_len == 0`保证AIV按`vector_len`处理列方向数据时不会产生不完整的Vector块。

## 样例实现

### 总体数据流

本样例的数据通路如下：

```text
GM（A/B）                         int8
  |  copy(GM -> L1)                                     MTE2
L1（A/B）                         int8
  |  copy(L1 -> L0A/L0B)                                MTE1
L0A / L0B                         int8
  |  mmad                                               M
L0C                               int32
  |  copy(L0C -> UB，split_m)                           FIX
UB输出slot（每个AIV接收M半块）      int32
  |  load(UB slot，int32)         int32 -> reg            V
  |  cast_int32_to_float       int32 -> float             V
UB middle_vec                      float
  |  copy(GM -> UB，channel_scale/token_scale)         MTE2
  |  compute_quant_fast_gelu    反量化和FastGelu          V
UB output_vec                      float
  |  copy(UB -> GM)                                     MTE3
GM z                               float
```

其中，AIV通过`load`从当前UB slot中按坐标直接读取一行int32数据到寄存器，无需先复制到单独的UB缓冲区。同一轮MTE2搬运会把GM中对应的`channel_scale`片段复制到UB，`token_scale`在行循环开始前复制到UB。

### Cube侧矩阵乘流程

1. 根据Kernel运行时传入的`m`、`n`、`k`创建GM Tensor视图。
2. 按`base_m`、`base_n`划分输出tile，按`base_k`在K维循环。
3. 每当`k_iter % step_k == 0`，一次性把`step_k`个K分块的A/B数据从GM搬入L1，减少GM到L1的搬运次数。
4. 每轮K迭代将当前A/B分块从L1搬入L0A/L0B。
5. 第一次K迭代前L0C还没有本输出tile的结果，设置`init_with_zero=true`，让`mmad`先把L0C清零再写入首个K分块的乘加结果。
6. 第二次及后续K迭代设置`init_with_zero=false`，保留L0C中已累加的部分结果，继续执行`C = A * B + C`。
7. K循环结束后，通过`copy`把L0C结果搬运到两个AIV的UB。

关键`mmad`调用如下：

```cpp
mmad(mm.with(mmad_params(base_m, base_n, base_k, unit_flag_mode::disable, k_iter == 0)),
     l0_c, l0_a, l0_b);
```

上述调用中最后一个实参`k_iter == 0`映射为`mmad_params::init_with_zero`：第一轮K迭代为`true`，后续迭代为`false`。

### L0C到UB的双目标搬运

L0C到UB搬运使用自定义trait：

```cpp
constexpr asc::te::l0c_to_ub_trait copy_l0c2ub_trait = {
    asc::te::round_mode::default_round, false, false,
    asc::te::dual_dst_mode::split_m};
```

`dual_dst_mode::split_m`把一个`base_m × base_n`的L0C tile沿M轴拆成两个半块：

- AIV 0处理前`base_m / 2`行。
- AIV 1处理后`base_m / 2`行。

`dual_dst_mode::split_m`用于减少L0C到UB的数据搬运次数，并让两个AIV并行处理同一输出tile的不同M半块。

### AIV侧反量化和FastGelu流程

每个AIV通过`get_sub_block_id()`确定自己负责的M半块：AIV 0处理前`base_m / 2`行，AIV 1处理后`base_m / 2`行。默认参数下，每个输出tile为`128 × 128`，因此每个AIV处理`64 × 128`个元素。收到AIC的数据就绪通知后，AIV开始处理数据，处理过程如下：

1. 通过`load`从当前UB slot中读取当前行的`vector_len`个int32 Cube结果，直接送入Vector寄存器。
2. 从GM读取当前列片段对应的`channel_scale`片段，即`channel_scale[j * base_n + column : j * base_n + column + vector_len]`，复制到UB中的`scale_vec`。
3. 从GM读取当前行对应的`token_scale[local_row]`，复制到UB中的`token_scale_vec`。
4. 将Vector寄存器中的int32 Cube结果转换为float，写入`middle_vec`。
5. 将`middle_vec`中的float结果分别乘以`scale_vec`中的`channel_scale`和`token_scale_vec`中的`token_scale`，得到反量化结果。
6. 对反量化结果执行`FastGelu(x) = x / (1 + exp(-1.702x))`计算，得到`output_vec`。
7. 将`output_vec`写回GM输出矩阵z中当前AIV负责的行列范围。

`compute_quant_fast_gelu`的关键实现如下：

```cpp
auto value = experimental::load(src, coord).with_mask(mask);
auto channel_value = experimental::load(channel_scale, coord).with_mask(mask);
auto token_value =
    experimental::load_broadcast<experimental::broadcast_mode::elem>(token_scale, make_coord(0))
        .with_mask(mask);

auto scaled_value = value * channel_value * token_value;
auto denominator = experimental::exp(scaled_value * fast_gelu_coefficient) + 1.0f;
auto result = scaled_value / denominator;
experimental::store(dst, coord, result);
```

上述流程按行推进；每一行内按`vector_len`列分块，每个列块均为完整Vector块。

### 双缓冲与同步

输出tile按生成顺序编号，`slot = sequence % pipe_depth`。当前参数下`pipe_depth=2`，因此两个UB slot交替缓存相邻的输出tile。每个slot只缓存一个AIV对应的一个M半块；同一个输出tile的另外半个M块由另一个AIV的本地UB接收。slot编号同时用于选择对应的同步flag，避免不同tile的事件相互干扰。同步事件分为两类：

| 方向 | 事件含义 | 使用的pipe和flag |
| --- | --- | --- |
| AIC -> AIV | 当前UB slot的L0C到UB搬运完成，AIV可以读取 | `PIPE_FIX`，flag为`8`、`9` |
| AIV -> AIC | 两个AIV都完成计算和GM写回，AIC可以复用UB slot | `PIPE_MTE3`，flag为`10`、`11` |

同步流程如下：

1. AIC计算第0个输出tile时，不需要等待，直接写入slot 0。
2. AIC完成slot 0的L0C到UB搬运后，记录该slot的数据就绪事件，随后继续计算第1个输出tile并写入slot 1。
3. 每个AIV等待slot 0的就绪事件，读取并处理自己负责的M半块，写回GM后记录slot 0的消费完成事件。
4. AIC从第2个输出tile开始复用slot 0，写入前先等待slot 0的消费完成事件，确保两个AIV都已读完数据。
5. 后续tile按相同方式在slot 0和slot 1之间交替，使AIC计算下一个tile与AIV处理上一个tile可以并行进行。

这套依赖保证UB slot不会被提前覆盖，同时让下一个tile的Cube计算、L0C搬运和上一个tile的Vector计算可以交叠执行。

### copy接口使用方式

`copy`是搬运接口。本样例使用两种调用形式。

使用默认trait，并通过目的坐标、源坐标和搬运形状指定区域时，接口形式为五个入参，由源和目的Tensor的位置自动推导搬运通路：

```cpp
copy(dst, src, dst_coord, src_coord, copy_shape);
```

需要指定非默认trait，且源和目的Tensor本身已经表示完整搬运对象时，接口形式为三个入参：

```cpp
copy(atom, dst, src);
```

样例中各通路使用的形式如下：

| 数据通路 | trait | 调用形式 | 用途 |
| --- | --- | --- | --- |
| GM -> L1 | 默认 | `copy(dst, src, dst_coord, src_coord, copy_shape)` | 按K方向搬运A/B数据，一次搬运`step_k`个`base_k`分块 |
| L1 -> L0A | 默认 | `copy(dst, src, dst_coord, src_coord, copy_shape)` | 加载左矩阵 |
| L1 -> L0B | 默认 | `copy(dst, src, dst_coord, src_coord, copy_shape)` | 加载右矩阵 |
| GM -> UB | 默认 | `copy(dst, src, dst_coord, src_coord, copy_shape)` | 搬运channel_scale和token_scale |
| UB -> GM | 默认 | `copy(dst, src, dst_coord, src_coord, copy_shape)` | 写回float输出 |
| L0C -> UB | 自定义`split_m` | `copy(atom, dst, src)` | 将L0C结果按M轴拆分后写入两个AIV的UB |

L0C到UB通路先通过`make_copy`构造包含自定义trait的copy atom，再调用`copy`接口。本样例通过自定义trait开启`dual_dst_mode::split_m`：

```cpp
// L0C到UB搬运的trait配置。
constexpr asc::te::l0c_to_ub_trait copy_l0c2ub_trait = {
    // 使用默认舍入模式。
    asc::te::round_mode::default_round,
    false,
    false,
    // 开启M轴双目标模式：L0C tile按M轴拆分到两个AIV的UB。
    asc::te::dual_dst_mode::split_m};

// 自定义trait需要提供TraitType别名和编译期value，
// make_copy会通过value读取该trait配置。
struct CopyL0C2UBTraitCustom {
    using TraitType = asc::te::l0c_to_ub_trait;
    static constexpr const TraitType value = copy_l0c2ub_trait;
};

// 用L0C到UB通路操作对象和自定义trait构造copy atom。
auto l0c_to_ub = make_copy(copy_l0c_to_ub{}, CopyL0C2UBTraitCustom{});

// ub_dst通过slice()已经定位到当前slot的M半块，因此这里使用
// 完整Tensor搬运形式；copy_atom作为第一个参数传入自定义trait。
copy(l0c_to_ub, ub_dst, l0_c);
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

### 配置环境变量

请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)配置环境变量：

```bash
source ${install_path}/cann/set_env.sh
```

其中`${install_path}`为CANN包安装目录。

### 编译和执行

```bash
mkdir -p build && cd build     # 创建并进入build目录
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..  # 编译工程，默认NPU模式
make -j                        # 编译样例
python3 ../scripts/gen_data.py # 生成测试输入数据和真值数据
./demo                         # 执行样例
python3 ../scripts/verify_result.py output/output.bin output/golden.bin
```

使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

示例：

```bash
cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
```

> **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

### 编译选项说明

| 选项 | 可选值 | 说明 |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | NPU运行、NPU仿真 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应Ascend 950PR&950DT系列产品 |

本样例仅支持`dav-3510`架构。

### 执行结果

执行结果如下，说明精度对比成功：

```text
test pass!
```

