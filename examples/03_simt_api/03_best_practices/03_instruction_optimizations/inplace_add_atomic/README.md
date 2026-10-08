# 原地加法指令优化

## 概述
本样例以三操作数乘加（`a[index] += b[index] * c[index]`）与两操作数加法（`a[index] += b[index]`）两种原地址累加操作为载体，展示如何用原子加进行性能优化。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | \>= CANN 9.2.0 |

## 目录结构介绍

```text
├── inplace_add_atomic
│   ├── figures                  // 样例说明文档配图
│   ├── CMakeLists.txt           // 样例构建脚本
│   ├── inplace_add_atomic.asc   // Ascend C SIMT核函数实现 & Host调用样例
│   ├── README.md                // 样例说明文档
│   └── README_en.md             // 英文样例说明文档
```

## 样例描述

本样例对`int32_t`类型的数组执行逐元素累加，包含两组对比：

- 三操作数乘加（Case 0，场景0/1）：对三个数组执行向量乘加，将`b`与`c`的逐元素乘积累加到`a`上。

  ```text
  a[index] += b[index] * c[index],  index = 0, 1, ..., element_count - 1
  ```

- 两操作数加法（Case 1，场景2/3）：对两个数组执行向量加，将`b`的元素累加到`a`上。

  ```text
  a[index] += b[index],  index = 0, 1, ..., element_count - 1
  ```

- 样例规格：

  <table>
  <tr><td align="center">样例类型（OpType）</td><td colspan="4" align="center">Atomic Optimization</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">c</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">a</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">vector_muladd_plain / vector_muladd_atomic / vector_add_plain / vector_add_atomic</td></tr>
  </table>

## 样例实现

本节以两组对照（Case 0~1）逐一分析三操作数乘加与两操作数加法两种计算形式下，累加方式对性能的影响。每组对照只改变累加方式、其余保持一致，并给出对应的`msopprof`实测数据与性能表现根因分析。

### 性能指标说明

| 指标 | 说明 |
| --- | --- |
| Task Duration（μs） | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间 |

---

### Case 0：三操作数乘加

**样例目标**：对比`a[index] += b[index] * c[index]`和`asc_atomic_add(&a[index], b[index] * c[index])`的性能差异。

**场景配置**：

| 场景 | 核函数 | 累加方式 |
|:---:|---|---|
| 0 | vector_muladd_plain | a[index] += b[index] * c[index] |
| 1 | vector_muladd_atomic | asc_atomic_add(&a[index], b[index] * c[index]) |

**核心实现**：启动64个线程块（运行时查询到的AIV核数），每块2048个线程，各线程以总线程数为步长遍历数组。两组场景仅循环体的累加语句不同：场景0为普通原地加，场景1改用`asc_atomic_add`。核函数实现如下。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_plain(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index] * c[index];
    }
}

__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_atomic(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index] * c[index]);
    }
}
```

**性能数据**：

| 场景 | 累加方式 | Task Duration（μs） | 耗时相对基线 |
|:---:|:---:|:---:|:---:|
| 0 | a[index] += b[index] * c[index] | 51.62 | 1× |
| 1 | asc_atomic_add | 47.51 | **0.92×** |

**分析**：

性能数据呈现出一个现象：改用原子加后，Task Duration下降8.0%。差异只来自累加目标`a[index]`旧值的数据路径。

本样例两种累加方式的数据流转分别如下图所示，两图的GM均存放`a`、`b`、`c`三个数组。

普通原地加的数据流转如下图：

![普通原地加（场景0）的数据流转](figures/inplace_add_atomic_dataflow_plain.png)

原子加的数据流转如下图：

![原子加（场景1）的数据流转](figures/inplace_add_atomic_dataflow_atomic.png)

- **原地加法（场景0）**：`a[index]`、`b[index]`、`c[index]`三者都经L2 Cache读入寄存器，乘加完成后，`a[index]`的新值再从寄存器写回L2 Cache。
- **原子加法（场景1）**：只有`b[index]`与`c[index]`读入寄存器并算出乘积，乘积作为原子加的操作数下发；读旧值、加法、写回三步作为一个不可分割的整体在L2 Cache侧完成，`a[index]`不进入寄存器，上述往返不再发生。

两个场景中`b`、`c`的读取与乘法完全相同，唯一的变量就是省去`a[index]`旧值进出寄存器的数据往返。8.0%的差距即由此而来。

**结论**：逐元素累加，可优先考虑由原子加在L2完成。

---

### Case 1：两操作数加法

**样例目标**：对比`a[index] += b[index]`和`asc_atomic_add(&a[index], b[index])`的性能差异。

**场景配置**：

| 场景 | 核函数 | 累加方式 |
|:---:|---|---|
| 2 | vector_add_plain | a[index] += b[index] |
| 3 | vector_add_atomic | asc_atomic_add(&a[index], b[index]) |

**核心实现**：启动64个线程块（运行时查询到的AIV核数），每块2048个线程，各线程以总线程数为步长遍历数组。两组场景仅循环体的累加语句不同：场景2为普通原地加，场景3改用`asc_atomic_add`。核函数实现如下。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_plain(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index];
    }
}

__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_atomic(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index]);
    }
}
```

**性能数据**：

| 场景 | 累加方式 | Task Duration（μs） | 耗时相对基线 |
|:---:|:---:|:---:|:---:|
| 2 | a[index] += b[index] | 39.18 | 1× |
| 3 | asc_atomic_add | 36.52 | **0.93×** |

**分析**：

性能数据呈现出一个现象：改用原子加后，Task Duration下降6.8%。原理同Case 0。

**结论**：两操作数加法同样适用该优化。

---

## 性能对比总结

**全场景Task Duration汇总**：

| 场景 | 对照Case编号 | 计算形式 | 累加方式 | Task Duration（μs） | 耗时相对基线 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | Case 0 | 三操作数乘加 | a[index] += b[index] * c[index] | 51.62 | 1× |
| 1 | Case 0 | 三操作数乘加 | asc_atomic_add | 47.51 | **0.92×** |
| 2 | Case 1 | 两操作数加法 | a[index] += b[index] | 39.18 | 1× |
| 3 | Case 1 | 两操作数加法 | asc_atomic_add | 36.52 | **0.93×** |

两组对照的结论一致：使用原子加的场景均快于基线，乘加组耗时下降8.0%，加法组耗时下降6.8%。

## 调优建议

1. **逐元素原地累加优先考虑用原子加完成**：当目标地址的旧值仅用于累加、不参与其他计算时，改用原子加可使旧值的读出、加法与写回均在L2完成，省去旧值进出寄存器的数据往返。
2. **注意适用边界**：本优化仅适用于逐元素原地更新，使用前需确认各线程原子操作的目标地址互不重叠。其与归约累加（多个线程向同一地址累加，如[atomic_histogram](../atomic_histogram/README.md)样例中的直方图统计）的适用边界不同：归约累加中同一地址上的原子操作只能串行执行，存在地址竞争时直接使用原子加反而会劣化性能，应采用该样例中分块累加再合并的策略。
3. **数据类型需支持指令优化**：原子加支持的数据类型必须是支持指令优化的数据类型；对int64_t等类型，该执行路径的收益需实测确认，不使用返回值时能否生成更优原子指令与数据类型相关，参见[asc_atomic_add约束说明](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md#约束说明)。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：**`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  场景在编译期通过`SCENARIO_NUM`选择，一次编译只包含一个场景。在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build                                      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=0 ..    # 配置工程（场景0：乘加基线）
  make -j                                                         # 编译样例
  ./inplace_add_atomic                                            # 执行场景0
  ```

  切换场景时，重新配置`SCENARIO_NUM`后再编译执行：

  ```bash
  cmake -DSCENARIO_NUM=1 .. && make -j && ./inplace_add_atomic    # 场景1：乘加优化
  cmake -DSCENARIO_NUM=2 .. && make -j && ./inplace_add_atomic    # 场景2：加法基线
  cmake -DSCENARIO_NUM=3 .. && make -j && ./inplace_add_atomic    # 场景3：加法优化
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU仿真模式
  ```

  > 注意：切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

  编译选项说明：

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | CMAKE_ASC_RUN_MODE | npu/sim | 运行模式：NPU运行、NPU仿真，默认npu |
  | CMAKE_ASC_ARCHITECTURES | dav-3510 | NPU架构：本样例仅支持dav-3510（Ascend 950PR&950DT系列产品） |
  | SCENARIO_NUM | 0/1/2/3 | 场景编号：0/1为Case 0（三操作数乘加）的基线/优化，2/3为Case 1（两操作数加法）的基线/优化 |
  | SKIP_VALIDATION | ON/OFF | 是否跳过结果校验，默认OFF。使用msopprof采集性能时建议设为ON |

  执行结果如下，说明精度对比成功。

  ```text
  Scenario 0 (muladd plain update): 4194304 elements, 64 blocks, 2048 threads per block
  [Success] Case accuracy verification passed.
  ```

## 性能调试

### msOpProf工具介绍

msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

使用`msOpProf`工具获取详细性能数据。每个场景需先按该场景编译，再采集，以场景1为例：

```bash
cmake -DSCENARIO_NUM=1 -DSKIP_VALIDATION=ON ..
make -j
msopprof ./inplace_add_atomic
```

> 关于性能采集时的Validation failed：本样例的累加目标`a`在host侧分配时写入初值，kernel仅在其上累加。`msopprof`的warmup+replay会在同一块GM内存上重复执行kernel，`a`被累加多次，因此严格校验模式下会报`Validation failed`。该现象是replay机制与校验逻辑的固有冲突。采集性能时建议先以`-DSKIP_VALIDATION=ON`重新编译再执行`msopprof`，跳过校验。

命令完成后，会在默认目录下生成以"OPPROF_{timestamp}_XXX"命名的文件夹，性能数据文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率
├──Memory.csv                 # UB，L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```
