# SinCosCompute性能调优样例

## 概述

本样例以sincos计算为例，介绍Ascend C SIMT编程方式下的线程配置优化思路。样例包含1个基线版本以及1个优化版本，基线版本中未设置`__launch_bounds__`，编译器按照默认值1024（即每个Block内1024个线程）进行资源分配导致寄存器溢出，优化版本通过配置`__launch_bounds__(512)`，提示编译器每个Block的最大线程数量为512，编译器根据提示在编译过程中充分利用硬件资源，从而避免寄存器溢出，展示SIMT编程方式下合理配置线程数优化性能的调优路径。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
sincos_compute/
│   ├── CMakeLists.txt              // cmake编译文件
│   ├── sincos_compute.asc          // sincos样例实现
│   └── README.md
```

## 样例描述

- 样例功能

  使用`sincosf()`函数同时计算sin和cos结果

  ```
  sincosf(input[idx], output_sin + idx, output_cos + idx)
  ```

- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SinCosCompute</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[393216]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">output_sin</td><td align="center">[393216]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">output_cos</td><td align="center">[393216]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">sincos_thread_1024 / sincos_thread_512</td></tr>
  </table>

## 样例实现

### sincos计算说明

本样例使用Ascend C提供的sincosf接口同时计算sin/cos结果，样例中设置了固定shape，每个线程计算16个输入值，计算流程如下：
1. 根据算子数据shape的切分逻辑，计算每个核的起始地址
2. 调用Ascend C提供的sincosf接口，同时计算sin/cos结果

### 线程数与寄存器关系

在SIMT编程模型中，核函数定义时配置的最大线程数直接影响每个线程可用的寄存器数量：

| 最大线程数 | 每个线程可用寄存器个数 |
|--------------|--------------------------|
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

**关键原则**：
- 配置的最大线程数越大，每个线程可用寄存器数越少
- 计算密集型算子，单个线程所需的寄存器通常较多，一般建议配置512或1024线程
- 数据搬运类算子，单个线程所需的寄存器通常较少，一般建议配置2048线程
- 当寄存器不足以存下所有的临时变量时，会出现寄存器溢出（register spill），数据会溢出到栈空间（Global Memory），导致性能下降

### 样例实现说明

本样例通过2个独立的kernel来体现`__launch_bounds__`的效果，每个kernel对应特定的Case版本。

| Case   | 实现特点                                                                 | 使用的核函数                     | 优化特性                |
|--------|----------------------------------------------------------------------|----------------------------|---------------------|
| Case 0 | 不设置launch bounds，使用默认值                                | sincos_thread_1024  | 基线版本，未配置`__launch_bounds__`          |
| Case 1 | 根据实际算子的计算规模，配置`__launch_bounds__(512)` | sincos_thread_512 | 编译器使用用户指定的配置值进行相应优化 |

---
### 性能指标说明

| 指标                  | 说明                          |
|---------------------|-----------------------------|
| Task Duration(μs)   | 整个任务执行的总时间，算子执行时间以该参数为准     |
| DCache Read GM      | DCache从Global Memory读取数据的次数 |
| DCache Read Vector  | Vector Core从DCache读取数据的次数   |
| DCache Write Vector | Vector Core向DCache写入数据的次数   |

---

### Case 0: 基线版本（寄存器溢出）

**样例目标**：不配置`__launch_bounds__`，观察寄存器溢出对性能的影响

**核心实现**：
- 默认线程数为1024，每个Thread仅可用32个寄存器
- 每个线程计算元素个数为PER\_THREAD\_LOOP，超出32个寄存器限制

**关键代码**：

```cpp
__global__ void sincos_thread_1024(float* input, float* output_sin, 
                                    float* output_cos, uint64_t total_length)
{
    int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
    
    // 每个核计算 PER_THREAD_LOOP * THREADS_PER_BLOCK 次运算
    for (int i = 0; i < PER_THREAD_LOOP; i++) {
        int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
        sincosf(input[idx], output_sin + idx, output_cos + idx);
    }
}
```

**编译信息**：

使用`--cce-res-usage`编译选项查看寄存器使用情况：

```
[BISHENG] Function properties for _Z18sincos_thread_1024PfS_S_m_simt_entry: Stack size: 32 bytes, Used register number: 32
```

**分析**：
- Stack size: 32 bytes → 存在寄存器溢出
- Used register number: 32 → 达到1024线程下的寄存器上限
- 寄存器溢出导致中间数据存储到Global Memory，增加访存开销

**性能数据**：

| Task Duration(μs) | DCache Read GM(次) | DCache Read Vector(次) | DCache Write Vector(次) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|       102.47       |       256        |         640          |          768          |

**性能瓶颈**：
- 寄存器溢出到Global Memory
- 额外的栈空间访问增加延迟
- DCache Read Vector / DCache Write Vector次数较高（640次 / 768次）

优化方向：通过`__launch_bounds__`提示编译器真实的`blockDim`，充分利用寄存器资源，避免寄存器溢出。

---

### Case 1: 优化版本（避免寄存器溢出）

**优化目标**：通过配置`__launch_bounds__(512)`避免寄存器溢出，充分利用寄存器资源，提升性能

**核心优化**：
- 指定`__launch_bounds__(512)`，每个Thread可用64个寄存器
- 每个线程计算元素个数为PER\_THREAD\_LOOP，所需的寄存器在限制范围内

**关键代码**：

```cpp
__global__ __launch_bounds__(512) void sincos_thread_512(float* input, 
                                    float* output_sin, float* output_cos,
                                    uint64_t total_length)
{
    int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
    
    // 每个核计算 PER_THREAD_LOOP * THREADS_PER_BLOCK 次运算
    for (int i = 0; i < PER_THREAD_LOOP; i++) {
        int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
        sincosf(input[idx], output_sin + idx, output_cos + idx);
    }
}
```

**编译信息**：

```
[BISHENG] Function properties for _Z17sincos_thread_512PfS_S_m_simt_entry: Stack size: 0 bytes, Used register number: 48
```

**分析**：
- Stack size: 0 bytes → 无寄存器溢出
- Used register number: 48 → 在64个寄存器限制内
- 所有中间数据保存在寄存器，避免Global Memory访问

**性能数据**：

| Task Duration(μs) | DCache Read GM(次) | DCache Read Vector(次) | DCache Write Vector(次) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|      96.22       |       256        |         512          |          256          |

**优化效果分析**：
- Task Duration从102.47μs降低到96.22μs，耗时下降约**6.1%**
- DCache Read GM保持不变，说明并没有增加额外开销
- DCache Read Vector从640减小至512， DCache Write Vector从768减小至256，说明没有寄存器溢出后，对于Data Cache的读写次数减少（stack物理位置位于Global Memory，因此寄存器溢出时对于stack的访问会体现在Data Cache的访问次数上）
- 寄存器充分利用，避免数据溢出到Global Memory

---

## 性能对比总结

### Ascend 950PR性能数据

**综合优化效果**：
- 从Case 0基线版本到Case 1优化版本，Task Duration从102.47μs降低到96.22μs，耗时下降约6.1%
- DCache Read Vector从640减小至512，DCache Write Vector从768减小至256

| Case version | Task Duration(μs) | Task Duration相对Case 0 | 优化点                   |
|--------------|-------------------|-----------------------|-----------------------|
| Case 0       | 102.47             | **1x**                | 基线版本，寄存器溢出到Global Memory |
| Case 1       | 96.22            | **0.94x**           | 配置launch bounds避免寄存器溢出 |

## 调优建议

1. **识别寄存器溢出**：使用`--cce-res-usage`编译选项查看寄存器使用情况
   - Stack size > 0：存在寄存器溢出
   - Stack size = 0：无寄存器溢出

2. **合理配置线程数**：
   - 计算密集型算子：建议512或1024线程
   - 数据搬运类算子：建议2048线程
   - 根据寄存器需求表选择合适的线程数配置

3. **使用`__launch_bounds__`提示编译器**：
   ```cpp
   __global__ __launch_bounds__(线程数) void kernel_name(...)
   ```

4. **验证优化效果**：
   - 对比优化前后的编译信息（Stack size和Used register number）
   - 对比优化前后的性能数据（Task Duration、DCache Read GM、DCache Read Vector、DCache Write Vector）

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  ```bash
  mkdir -p build && cd build;          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程
  ./sincos_compute 1024                    # 执行基线样例
  ./sincos_compute 512                     # 执行优化样例
  ```

- 编译选项说明

  | 选项                        | 可选值        | 说明                                                |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy is verification passed.
  ```

## 性能分析

使用 `msOpProf` 工具获取详细性能数据：

```bash
msopprof ./sincos_compute 1024   # 分析基线case的性能
msopprof ./sincos_compute 512    # 分析优化case的性能
```

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
├──ResourceConflictRatio.csv  # UB上的 bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```

查看具体的性能分析结果：

```bash
# 如查看算子基础信息
cat ./OPPROF_*/OpBasicInfo.csv

# 如查看内存相关数据
cat ./OPPROF_*/Memory.csv
```
