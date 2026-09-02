# 短向量Add算子调优样例

## 概述

本样例以向量加法为例，演示了在Ascend C SIMT编程中，基于短向量的性能调优方法。通过两个场景的对比（Case
1-2），展示从基础half类型逐元素加法到使用half2类型加法的优化过程。重点对比了原始类型（half）与短向量类型（half2）在数据处理效率上的差异。

**优化路径**：

- Case 1: half类型数据处理（基准）
- Case 2: half2类型数据处理

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```
short_vector_add
│   ├── scripts/             // 测试脚本目录
│   │   ├── gen_data.py      // 生成测试输入和golden数据
│   │   └── verify_result.py // 验证输出结果
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── data_utils.h         // 数据读写工具函数
│   └── short_vector_add.asc // Ascend C算子实现 & 调用样例
```

## 样例描述

- 样例功能：

  样例计算half类型数据，分别使用基础half类型和短向量half2类型实现向量加法运算。通过不同场景的对比，展示短向量计算和线程分配优化对性能的影响。计算公式如下：
  $$z_i = x_i + y_i$$

- 样例规格
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SIMT样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center" colspan="2">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[64 * 256 * 1024]</td><td align="center" colspan="2">half</td></tr>
  <tr><td align="center">y</td><td align="center">[64 * 256 * 1024]</td><td align="center" colspan="2">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[64 * 256 * 1024]</td><td align="center" colspan="2">half</td></tr>
  </table>

## 样例实现

### 性能指标说明

**表1 AI Core性能指标字段说明表**

|             字段名             | 字段含义                                             |
|:---------------------------:|:-------------------------------------------------|
|      Task Duration(μs)      | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。          |
|      aiv_total_cycles       | Task在Vector Core上执行所消耗的CPU周期（Cycle）总数。          |
|          aiv_time           | Task在AI Vector Core上的理论执行时间，单位为μs。               |
|      aiv_vec_time(μs)       | vec类型指令（向量类运算指令）耗时，单位μs。                         |
|        aiv_vec_ratio        | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。       |
|     aiv_scalar_time(μs)     | scalar类型指令（标量类运算指令）耗时，单位μs。                      |
|      aiv_scalar_ratio       | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。    |
|      aiv_read_hit_rate      | 读cache命中率（所在文件：L2Cache.csv）。                     |
|     aiv_write_hit_rate      | 写cache命中率（所在文件：L2Cache.csv）。                          |
| aiv_main_mem_read_bw(GB/s)  | 主存储器读取其他所有单元数据时，对应的total cycle的带宽速率（所在文件：Memory.csv）。 |
| aiv_main_mem_write_bw(GB/s) | 主存储器写入其他所有单元数据时，对应的total cycle的带宽速率（所在文件：Memory.csv）。 |

本章节性能数据在Ascend 950PR系列产品上运行得到。

### Case 1: half类型数据处理（基准程序）

**实现方式**：

基准程序实现了`half`类型的两组输入数据的加法。

**关键代码**：

```cpp
template<typename T>
__global__ void add(T* x, T* y, T* z, uint64_t size)
{
    // gridDim.x * blockDim.x线程总数 (64 * 1024)
    int32_t stride = gridDim.x * blockDim.x;
    // 每个线程从自己的全局idx开始，每次递增总线程数，直到处理完所有元素
    for (int32_t idx = blockIdx.x * blockDim.x + threadIdx.x; idx < size; idx += stride) {
        z[idx] = x[idx] + y[idx];
    }
}
```

**样例配置**：

- blockNum: 64
- threadNumPerBlock: 1024
- 总线程数: 65536
- 每线程处理元素数: 256

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_read_hit_rate | aiv_write_hit_rate | aiv_main_mem_read_bw(GB/s) | aiv_main_mem_write_bw(GB/s) |
|:-----------------:|:------------:|:----------------:|:----------------:|:-------------:|:-------------------:|:----------------:|:-----------------:|:------------------:|:--------------------------:|:---------------------------:|
|      86.478       |    86.217    |     8832432      |      85.735      |     0.995     |        0.47         |      0.005       |         0         |       0.493        |            7.77            |            6.24             |


**性能数据分析**：

- 端到端耗时：**86.478 μs**
- 写Cache命中率：**49.3%**
- 读写带宽：**7.77(GB/s) | 6.24(GB/s)**
- 性能分析：Add算子进行简单的X+Y=Z，在回写结果时命中率低于50%，说明写Cache未命中率超过50%，触发额外的内存回读（RMW）操作，被判定为写未命中。一般原因为数据未对齐或者数据类型位宽较小导致Warp不能填满。

**性能优化建议**：
> 💡 **引入half2类型**
>
> 对于16位half类型的数据，底层指令支持一次完成两个数据的计算，可以提高数据计算效率，也可以减少for循环这种scalar指令次数。

---

### Case 2: half2类型数据处理

**实现方式**：

使用`half2`类型的两组输入数据替代原有`half`类型数据。当元素总数为奇数时，需要单独处理最后一个`half`元素。

**关键代码**：

```cpp
template<typename T, typename U>
__global__ void add2(T* x, T* y, T* z, uint64_t size)
{
    // 转为向量类型
    const U* x2 = reinterpret_cast<const U*>(x);
    const U* y2 = reinterpret_cast<const U*>(y);
    U* z2 = reinterpret_cast<U*>(z);

    // 此处用除法一次性算出for循环的边界值
    uint64_t vector_size = size / 2;
    int32_t stride = gridDim.x * blockDim.x;

    // 向量化循环处理偶数对（步长同样是总线程数）
    for (int32_t idx = blockIdx.x * blockDim.x + threadIdx.x; idx < vector_size; idx += stride) {
        z2[idx] = x2[idx] + y2[idx];
    }

    // 最后一个奇数元素处理（如果size是奇数，由全局第0号线程在最后处理）
    if (blockIdx.x == 0 && threadIdx.x == 0 && (size % 2 != 0)) {
        uint64_t last_idx = size - 1;
        z[last_idx] = x[last_idx] + y[last_idx];
    }
}
```

**样例配置**：

- blockNum: 64
- threadNumPerBlock: 1024
- 总线程数: 65536
- 每线程处理half2元素数: 128

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_read_hit_rate | aiv_write_hit_rate | aiv_main_mem_read_bw(GB/s) | aiv_main_mem_write_bw(GB/s) |
|:-----------------:|:------------:|:----------------:|:----------------:|:-------------:|:-------------------:|:----------------:|:-----------------:|:------------------:|:--------------------------:|:---------------------------:|
|      45.371       |    45.111    |     4584309      |      44.582      |     0.988     |        0.517        |      0.011       |         0         |       0.891        |           14.51            |            7.25             |

**优化效果分析**：

- 端到端耗时：**45.371 μs**
- 写Cache命中率：**89.1%**
- 读写带宽：**14.51(GB/s) | 7.25(GB/s)**

**原理说明**：
SIMT编程模式下，L2 Cache是以Cache Line=128字节为单位进行访问。

- Case 1（标量half）：无法填满整行导致RMW写放大
  使用标量half（2B）。单个Warp（32线程）同时发射指令时，在空间上连续拼出的总宽度为：
  $$
  32 * 2B = 64B
  $$
  同时发射写指令的总宽度仅为64字节，不满足Cache Line的要求，带宽占用率50%


- 在Case 2中，改用短向量类型half2（4B）。单个Warp（32线程）单次发射写入的总宽度达到：
  $$
  32 * 4B = 128B
  $$
  这个宽度覆盖了一整行物理Cache Line的边界，带宽占用率100%，传输效率达到最大化，在同样的物理拉取周期下，读带宽由7.77 GB/s拉升至14.51 GB/s。


**最终性能总结**：
- **耗时**：从**86.478 μs**骤降至**45.371 μs**，耗时减少了**47.5%**，换算整体性能提升达**1.91**倍。
- **cycles数**：CPU总周期数从**8,832,432**减至**4,584,309**，降幅达**48.1%**，证明指令发射密度得到精简。
- **主存读写带宽**：读写带宽均有提升，其中读带宽提升**86.7%**，从**7.77 GB/s**到**14.51 GB/s**，提升访存密集型算子对带宽的利用。

---

### 优化要点总结

| 优化手段 | 核心原理              | 适用场景                                                                                                                                                                                                                                                                                                                                               |
|:-----|:------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 短向量  | 底层指令支持一次完成两个数据的计算 | 1、half/bfloat16_t类型元素进行[基本运算](../../../../../docs/zh/guide/programming_guide/language_extension/simt_builtin_keywords.md#运算符)，更多短向量类型参见[完整类型列表](../../../../../docs/zh/api/SIMT-API/SIMT_programming_intro/extended_syntax/builtin_data_types.md#短向量类型) |

---

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  ```bash
  # 场景2：使用half2类型
  SCENARIO_NUM=2
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM; make -j;                           # 编译工程
  python3 ../scripts/gen_data.py;                                           # 生成测试输入数据
  ./demo                                                                    # 执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin;  # 验证输出结果
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行 `rm CMakeCache.txt` 后重新cmake。

- 编译选项说明

  | 参数                        | 可选值             | 说明                                                |
  |:--------------------------|:----------------|:--------------------------------------------------|
  | `SCENARIO_NUM`            | `1`（默认）、`2`     | 1: half Add计算；<br/>2: 短向量half2 Add计算；             |
  | `CMAKE_ASC_RUN_MODE`      | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真                                 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`      | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |

- 执行结果
  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```

## 性能数据获取

使用 `msOpProf` 工具获取单个组件上的性能数据：

```bash
msopprof ./demo   # 分析case的性能
```

命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹，性能数据文件夹结构示例如下：

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

查看具体的性能分析结果：

```
# 如查看L2 Cache相关数据
cat ./OPPROF_*/L2Cache.csv
```
