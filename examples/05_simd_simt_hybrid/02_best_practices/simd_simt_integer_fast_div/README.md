# SIMD与SIMT混合编程实现快速除法算子样例

## 概述

本样例以固定除数的整数除法计算为例，介绍SIMD与SIMT混合编程场景下的整数除法优化方法。样例展示SIMT线程内直接使用普通除法，以及通过预计算参数后使用乘法和移位替代普通除法的性能差异。

**优化路径**：

| Case | SCENARIO_NUM | 实现方式 | 说明 |
|:---|:---:|:---|:---|
| Case 0 | 0 | 普通除法 | SIMT线程内直接使用 `/` 完成整数除法，作为固定除数除法优化前的基线版本。 |
| Case 1 | 1 | 快速除法 | 针对固定除数预计算 `magic` 和 `shift`，SIMT线程内使用乘法和移位替代普通除法。 |

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.2.0

## 目录结构介绍

```
├── simd_simt_integer_fast_div
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── integer_div.asc         // 整数除法样例实现
│   └── README.md
```

## 样例描述

**样例功能**：

样例实现固定除数的uint32整数除法计算，并对比普通除法和快速除法两种SIMT线程内计算方式的性能差异。计算公式如下：

```cpp
output[i] = input[i] / divisor
```

- input为输入数据，形状为[N]，数据类型为uint32，取值范围为[0, INT32_MAX]
- divisor为固定除数，数据类型为uint32
- output为输出数据，形状为[N]，数据类型为uint32

**样例规格**：

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">integer_div</td></tr>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[8388608]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td align="center">divisor</td><td align="center">标量</td><td align="center">uint32</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[8388608]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center"><code>integer_div_kernel</code></td></tr>
</table>

## 样例实现

两个Case采用相同的数据搬运流程：先将当前线程块负责的数据从GM搬运到UB，SIMT VF在UB上完成除法计算并将结果写入UB，最后将结果从UB写回GM。这样可以避免数据搬运差异影响性能对比，使性能差异主要来自普通除法和快速除法本身。

两个Case共用的数据搬运和SIMT调用流程如下：

```cpp
asc_copy_gm2ub_align(input_buf, input + block_offset, 1, blk_length, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);

if ASC_IS_AIV {
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
}

if constexpr (scenario == 0) {
    asc_vf_call<simt_normal_div>(dim3(THREAD_COUNT), input_buf, output_buf, divisor, total_length);
} else {
    asc_vf_call<simt_fast_div>(dim3(THREAD_COUNT), input_buf, output_buf, magic, shift, total_length);
}

if ASC_IS_AIV {
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
}

asc_copy_ub2gm_align(output + block_offset, output_buf, 1, blk_length, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
```

### 性能指标说明

| 字段名 | 字段含义 |
|:---|:---|
| Task Duration(μs) | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。 |
| aiv_time(μs) | Task 在 AI Vector Core 上的执行时间。 |
| aiv_vec_time(μs) | 计算指令耗时。SIMT VF调用中的线程计算也体现在该类统计中。 |
| aiv_vec_ratio | 计算指令cycle数在total cycle数中的占用比。 |
| aiv_scalar_time(μs) | scalar类型指令耗时。 |
| aiv_scalar_ratio | scalar类型指令cycle数在total cycle数中的占用比。 |
| aiv_mte2_time(μs) | mte2类型指令耗时，主要对应GM到UB的搬运。 |
| aiv_mte2_ratio | mte2类型指令cycle数在total cycle数中的占用比。 |
| aiv_mte3_time(μs) | mte3类型指令耗时，主要对应UB到GM的搬运。 |
| aiv_mte3_ratio | mte3类型指令cycle数在total cycle数中的占用比。 |

除Task Duration外，其余指标均为所有Thread Block上的平均值。

### Case 0：普通除法版本

**样例目标**：实现基础整数除法功能，作为快速除法版本的耗时对比基线

**核心实现**：

- 每个SIMT线程从UB读取1个输入元素，直接使用 `/` 完成整数除法。

**关键代码**：

`simt_normal_div()` 函数中对当前线程处理的输入元素 `value` 执行普通除法：

```cpp
uint32_t result = value / divisor;
```

**性能数据**：

| Case | 实现方式 | 数据量 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Case 0 | 普通除法 | 8388608 | 117.229 | 1.277 | 0.497 | 0.390 | 0.128 | 0.099 | 0.423 | 0.331 | 0.222 | 0.174 |

**性能数据分析**：

- Case 0的 `Task Duration` 为117.229μs，是快速除法版本的对比基线。
- Case 0的 `aiv_time` 为1.277μs，其中 `aiv_vec_time` 为0.497μs，可作为普通除法版本在AI Vector Core上执行耗时和计算指令耗时的基线。

---

### Case 1：快速除法版本

**优化目标**：针对除数固定的整数除法场景，使用乘法和移位替代普通除法，降低SIMT线程内除法计算开销

**核心实现**：

- 在SIMD侧根据固定除数计算快速除法所需的 `magic` 和 `shift`，计算结果可被SIMT线程复用。
- 每个SIMT线程从UB读取1个输入元素，线程内使用 `__umulhi` 和右移完成等价除法计算。

#### 快速除法转换原理

对于uint32类型整数除法，可作如下变形，将除法计算转换为乘法和移位操作：

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n}{d} \times \frac{2^s}{2^s} \right\rfloor =
\left\lfloor \frac{2^s}{d} \times \frac{n}{2^s} \right\rfloor =
\left\lfloor {m} \times \frac{n}{2^s} \right\rfloor
$$

其中 $n$ 为被除数，$d$ 为除数，$m = \frac{2^s}{d}$。$n$ 除以 $2^s$ 可通过右移 $s$ 位实现。为了避免 $m \times n$ 乘法溢出，将 $m$ 拆分为：

$$
m = (m - 2^{32}) + 2^{32}
$$

令 $magic = m - 2^{32}$，代入上式可得：

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n \times {magic} + n \times 2^{32}}{2^s} \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> (s-32) \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> shift \right\rfloor
$$

其中 `magic` 和 `shift` 表示快速除法所需的乘法魔数和移位数。
本样例中基于上述除法转换思路，在SIMD部分使用Scalar计算单元中的 `clz()`、`bcnt1()` 标量计算接口，获取快速除法所需的 `magic` 和 `shift`，供SIMT线程复用。

**关键代码**：

`calc_magic_shift()` 函数中计算快速除法的 `magic` 和 `shift`：

```cpp
int64_t pos = BIT_64_LEN - count_leading_zero(divisor);
int64_t cnt1 = get_bit_count1(divisor);
uint32_t shift = (cnt1 == 1) ? (pos - 1) : pos;
uint32_t magic = (1l << BIT_32_LEN) * ((1l << shift) - divisor) / divisor + 1;
```

`simt_fast_div()` 函数中对当前线程处理的输入元素 `value` 执行快速除法：

```cpp
uint32_t q = __umulhi(value, magic);
uint32_t result = (value + q) >> shift;
```

**性能数据**：

| Case | 实现方式 | 数据量 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Case 1 | 快速除法 | 8388608 | 103.889 | 1.068 | 0.270 | 0.254 | 0.142 | 0.132 | 0.425 | 0.398 | 0.221 | 0.208 |

**性能数据分析**：

- 相比Case 0普通除法版本，Case 1的 `Task Duration` 从117.229μs降低到103.889μs，`aiv_time` 从1.277μs降低到1.068μs，整体执行耗时下降。
- `aiv_vec_time` 从0.497μs降低到0.270μs，说明将普通除法替换为乘法和移位后，计算指令耗时明显降低。
- `aiv_scalar_time` 从Case 0的0.128μs增加到0.142μs，这与快速除法场景中额外计算 `magic` 和 `shift` 的标量操作有关；但整体 `Task Duration` 仍然下降，说明快速除法带来的收益可以覆盖这部分开销。

---

## 性能对比总结

### Ascend 950PR性能数据

**综合优化效果**：

- 通过Case 0到Case 1的快速除法优化，样例 `Task Duration` 从117.229μs降低到103.889μs，耗时下降约11.4%。
- Case 1相对Case 0性能提升约1.13倍，说明在固定除数场景下，使用乘法和移位替代普通除法可以降低端到端耗时。

| Case | 实现方式 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Case 0 | 普通除法 | 117.229 | 1.277 | 0.497 | 0.390 | 0.128 | 0.099 | 0.423 | 0.331 | 0.222 | 0.174 |
| Case 1 | 快速除法 | 103.889 | 1.068 | 0.270 | 0.254 | 0.142 | 0.132 | 0.425 | 0.398 | 0.221 | 0.208 |

### 优化要点总结

| 优化手段 | 核心原理 | 样例体现 |
|:---|:---|:---|
| 固定除数预计算 | 当除数在核函数执行期间固定时，可以提前计算 `magic` 和 `shift`，SIMT线程复用该结果完成快速除法。 | Case 1在核函数中调用 `calc_magic_shift()`，SIMT线程内不再直接执行 `/`。 |
| 乘法和移位替代普通除法 | 普通整数除法指令开销较高，使用乘法和移位操作可降低逐元素计算开销。 | Case 1相比Case 0，`Task Duration` 从117.229μs降低到103.889μs。 |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在cmake编译时通过 `-DSCENARIO_NUM=N` 指定要编译的Case，各Case说明：

  - `0`: 普通除法
  - `1`: 快速除法

  示例：

  ```bash
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=0                                                           # 选择执行场景
  mkdir -p build && cd build;                                          # 创建并进入 build 目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程
  ./integer_div                                                        # 执行样例
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |:---|:---|:---|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`、`1` | Case编号：0=普通除法版本，1=快速除法版本 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy is verification passed.
  ```

### 性能分析

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    使用 `msOpProf` 工具获取详细性能数据。
    ```bash
    msopprof ./integer_div   # 分析性能
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```
