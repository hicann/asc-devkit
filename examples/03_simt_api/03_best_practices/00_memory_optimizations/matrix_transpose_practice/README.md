# Matrix Transpose性能调优样例

## 概述

本样例以矩阵转置为例，介绍Ascend C SIMT编程方式下的访存优化思路。首先，通过一维连续复制建立GM连续读写的性能基线，并用直接全局内存转置暴露非连续写带来的主要开销；然后，引入UB中转与32×32分块，将非连续GM写转移为UB内转置访问；接着，对比按tile分组启动线程块（Thread Block）与固定线程块数两种配置方式，并结合2048线程配置下的寄存器溢出分析线程数选择；随后，增加UB padding消除转置读阶段的bank冲突；最终，借助双缓冲（Double Buffer）去除循环尾部同步，完整呈现SIMT矩阵转置的调优路径。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── matrix_transpose_practice
│   ├── figures                     // README中的图片资源
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── matrix_transpose.asc        // SIMT矩阵转置优化路径实现
│   ├── README.md
│   └── README_en.md
```

## 样例描述

- 计算公式：

  ```text
  output(x, y) = input(y, x)
  ```

  - input为输入矩阵，形状为 [H, W]，数据类型为float
  - output为输出矩阵，形状为 [W, H]，数据类型为float
  - 连续复制基线（Case 0/1）不执行转置，校验output与input相等
- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">MatrixTranspose</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">copy_kernel / transpose_naive_kernel / transpose_ub_2tile_naive_kernel / transpose_ub_2tile_kernel / transpose_ub_kernel / transpose_ub_pad_kernel / transpose_ub_pad_db_kernel</td></tr>
  </table>

## 样例实现

### Case实现说明

本样例通过8个Case构成一条优化路径，各Case的核函数、线程块数量、每个线程块的线程数和主要变化如下表所示。

| Case | 核函数                              | 线程块数量          | 每个线程块的线程数 | 本步引入的变化                                                              |
| ---- | ----------------------------------- | ------------------- | ------------------ | --------------------------------------------------------------------------- |
| 0    | `copy_kernel`                     | `core`            | 2048               | 建立连续读写的耗时基线                                                      |
| 1    | `copy_kernel`                     | `core`            | 1024               | 仅降低线程数，与Case 5、Case 6、Case 7的启动配置一致                        |
| 2    | `transpose_naive_kernel`          | `core`            | 2048               | 直接在GM中完成转置                                                          |
| 3    | `transpose_ub_2tile_naive_kernel` | `ceil(tiles / 2)` | 2048               | 引入UB与32×32的tile分块，按切分后的tile数量设置Thread Block数              |
| 4    | `transpose_ub_2tile_kernel`       | `core`            | 2048               | 限制启动核数不超过物理核，单个Thread Block循环处理多个tile分组              |
| 5    | `transpose_ub_kernel`             | `core`            | 1024               | 降低线程数避免寄存器溢出，单个Thread Block循环处理多个32×32的tile           |
| 6    | `transpose_ub_pad_kernel`         | `core`            | 1024               | 增加UB padding缓解转置读阶段的bank冲突                                      |
| 7    | `transpose_ub_pad_db_kernel`      | `core`            | 1024               | 使用双缓冲减少循环尾部同步                                                  |

其中 `core`为硬件vector core数，通过 `aclrtGetDeviceInfo(ACL_DEV_ATTR_VECTOR_CORE_NUM)`在运行时查询获得。本样例中的性能数据基于 `core=64` 的测试环境采集得到。`tiles`为总tile数 `(W/32) × (H/32)`；处理1024×1024矩阵时，矩阵会被划分为32×32个tile，因此 `tiles=1024`。

#### 线程块与tile映射

从Case 3开始，矩阵按32×32的tile处理。下图左侧表示1024×1024矩阵被划分为32×32个tile，右侧表示其中一个32×32的tile内的线程映射关系。tile内列索引记为 `tx`，tile内行索引记为 `ty`。

**图1　Thread Block与tile的映射示意图**

<img src="./figures/block_mapping.png" width="60%">

以图中绿色元素为例，可以根据tile坐标和tile内局部坐标推导它在GM中的input坐标。

- 图中绿色元素所在的tile坐标是 `(2, 1)`。对于1024×1024矩阵，`tiles_x = 1024 / 32 = 32`，因此对应 `tile_id = 2 * 32 + 1 = 65`。
- 图中绿色元素在tile内的局部坐标是 `(2, 29)`，对应 `ty = 2`、`tx = 29`。

代入坐标计算：

```cpp
uint32_t tile_y = tile_id / tiles_x;                     // 65 / 32 = 2
uint32_t tile_x = tile_id - tile_y * tiles_x;             // 65 - 2 * 32 = 1
uint32_t x_index = tile_x * TILE_DIM + tx;                // 1 * 32 + 29 = 61
uint32_t y_index = tile_y * TILE_DIM + ty;                // 2 * 32 + 2  = 66
uint32_t index = x_index + width * y_index;              // 61 + 1024 * 66
```

因此该线程在GM中处理的输入元素坐标为 `input[66,61]`。

### 性能指标说明

| 指标                 | 说明                                                                                      |
| -------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration(μs)   | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间                  |
| aiv_time(μs)        | Task在AI Vector Core上的理论执行时间，单位为μs                                           |
| aiv_total_cycles     | 该Task在每个AI Vector Core计算单元上的执行cycle总数 |
| aiv_vec_time(μs)    | vec类型指令（向量类运算指令）耗时，单位为μs                                              |
| aiv_vec_ratio        | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比                           |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位为μs                                           |
| aiv_scalar_ratio     | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比                        |

除Task Duration外，其余指标均为所有Thread Block上的平均值。

### Case 0 / Case 1：一维连续复制基线

**优化目标**：实测当前硬件上纯连续GM读写的数据搬运能力，作为后续转置实现可逼近的性能上限。

**核心实现**：将矩阵视为一维数组，按一维线性地址访问input和output，使GM访存保持连续。Case 0每个Thread Block启动2048个线程，Case 1每个Thread Block启动1024个线程。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void copy_kernel(float* output, const float* input, uint32_t elements)
{
    uint32_t tid = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t stride = gridDim.x * blockDim.x;
    for (uint32_t i = tid; i < elements; i += stride) {
        output[i] = input[i];
    }
}
```

**性能数据**：

| Case | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| ---- | :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
| 0    |       6.262       |     4.971     |      8201.7      |       4.516       |     0.909     |        0.443        |      0.089      |
| 1    |       8.649       |     7.321     |     12079.8     |       6.872       |     0.939     |        0.437        |      0.059      |

**分析**：

Case 0/1只执行连续读写，不涉及坐标交换、UB中转和Thread Block内同步，主要用于实测当前硬件在该数据规模下的GM连续搬运能力。1024×1024的float矩阵复制需要从GM读取4MB数据并写入4MB数据，读写总数据量为：

$$
D = 1024 \times 1024 \times 4B \times 2 = 8.39MB
$$

Ascend 950PR的GM峰值带宽按1.6TB/s估算，连续复制的理论耗时为：

$$
T_{theory} = \frac{8.39MB}{1.6TB/s} \approx 5.243μs
$$

Case 0的Task Duration为6.262μs，对应读写混合带宽约1.34TB/s，达到理论峰值带宽的约83.7%；Case 1的Task Duration为8.649μs，对应读写混合带宽约0.97TB/s，达到理论峰值带宽的约60.6%。在连续读写的搬运场景下，2048线程配置更接近硬件连续搬运上限。未达峰值带宽的主要原因在于：实际耗时除了数据读写本身之外，还包含Task调度和其他计算指令的开销，这些非数据搬运开销摊薄了有效带宽；线程数较少时，单线程迭代次数更多，额外开销占比更高，带宽利用率相对更低。由于后续转置Case在完成坐标交换的同时还会引入UB访问和同步开销，因此Case 0的连续复制耗时可作为矩阵转置优化的性能上限参照。

---

### Case 2：直接全局内存转置

**优化目标**：在GM上直接进行坐标变换。

**核心实现**：不分块、不使用UB。每个线程读取 `input(row, col)`，直接写入转置位置 `output(col, row)`。全局内存读取连续，写入地址按列跨行，为非连续写。

```cpp
uint32_t row = i / width;
uint32_t col = i - row * width;
// input(row,col) -> output(col,row)
output[col * height + row] = input[i];
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
|       36.337       |    34.022    |     56136.2     |      33.568      |     0.987     |        0.442        |      0.013      |

**分析**：

Case 2在保持GM连续读取的同时，直接将数据写入转置后的输出位置，导致同一Warp内相邻线程的写地址分布到输出矩阵的不同行。相比Case 0的连续复制基线，Task Duration从6.262μs增加到36.337μs，约为复制基线的5.8倍。由于矩阵转置本身计算量很小，这一差距主要来自GM非连续写。后续优化需要将非连续访问从GM转移到UB，使GM读写转变为连续访问模式。

---

### Case 3：UB中转转置，每个Thread Block处理两个32×32的tile

**优化目标**：引入UB与分块，将Case 2的非连续全局内存写转换为UB内访问，使全局内存读写均恢复连续；本Case按切分后的tile数量设置Thread Block数，每个Thread Block处理两个32×32的tile。

**核心实现**：将矩阵划分为32×32的tile，总tile数为 `tiles=(W/32)×(H/32)`。本Case中每个Thread Block启动2048个线程，可同时处理两个tile，因此Thread Block数量设置为 `blocks_per_grid=ceil(tiles / 2)`。线程配置参数为 `dim3(32,64,1)`，其中 `threadIdx.y=0..31`的线程处理第一个tile，`threadIdx.y=32..63`的线程处理第二个tile；代码通过 `local_tile=threadIdx.y>>5`得到当前线程在Thread Block内负责的tile编号，通过 `ty=threadIdx.y&31`得到tile内行索引。线程先将tile按原布局写入UB，经 [`asc_syncthreads()`](../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md) 同步后，再按转置方向从UB读取并连续写入GM。

UB中转的核心思想如下图所示：GM读取阶段按输入矩阵行方向连续读取至UB；GM写入阶段交换输出tile坐标，使同一个Warp向输出矩阵的一行写入数据。原本非连续的GM写被转移为UB内转置方向读取。

**图2　Case 3 UB中转转置数据流向示意图**

<img src="./figures/case3.png" width="60%">

Case 2的直接逐元素写入需要交换行列坐标，输出地址会沿列方向跨行分散，这是直接全局内存转置的主要问题。引入分块后，处理粒度从单个元素变为局部tile：线程先按输入矩阵的行方向将tile连续读入UB，再从UB中按转置方向读取数据，最后按输出矩阵的行方向连续写入GM。这样，转置带来的不连续访问被限制在UB内部，GM读写都保持连续访问模式。

tile大小选为32×32，是因为 `threadIdx.x`方向的32个线程可以对应tile内一行元素，使一个Warp的GM读写访问覆盖32个连续float；同时32×32的tile含1024个元素，便于在1024线程配置下由一个Thread Block处理一个tile，或在2048线程配置下由一个Thread Block处理两个tile。

```cpp
__ubuf__ float tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
uint32_t local_tile = threadIdx.y >> 5;                // 0或1，区分Thread Block内两个tile
uint32_t ty = threadIdx.y & (TILE_DIM - 1);
uint32_t tx = threadIdx.x;
uint32_t tiles_x = width / TILE_DIM;

uint32_t tile_id = blockIdx.x * TILES_PER_BLOCK + local_tile;
uint32_t tile_y = tile_id / tiles_x;
uint32_t tile_x = tile_id - tile_y * tiles_x;
uint32_t x_index = tile_x * TILE_DIM + tx;
uint32_t y_index = tile_y * TILE_DIM + ty;
tile[local_tile][ty][tx] = input[x_index + y_index * width];
asc_syncthreads();

x_index = tile_y * TILE_DIM + tx;
y_index = tile_x * TILE_DIM + ty;
output[x_index + y_index * height] = tile[local_tile][tx][ty];
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
|       28.190       |     2.896     |      4778.1      |       2.712       |     0.938     |        0.172        |      0.058      |

**分析**：

Case 3通过UB中转把非连续GM写转换为UB内转置读取，Task Duration从Case 2的36.337μs降至28.190μs，说明GM读写连续化已经带来收益。此时新的瓶颈主要来自Thread Block映射方式：该实现按切分后的tile分组设置线程块数量，1024个tile对应512个Thread Block，明显超过硬件vector core数，额外的线程块启动和调度开销会反映到Task Duration中。因此下一步优先优化Thread Block到tile的映射，减少需要启动和调度的Thread Block数量。

---

### Case 4：限制启动核数不超过物理核

**优化目标**：将Case 3的线程块数量从切分后的tile分组数改为硬件vector core数，每个Thread Block通过循环处理多个tile分组，并观察2048线程配置下的寄存器使用情况。

**核心实现**：每个Thread Block启动2048个线程，配置参数为 `dim3(32,64,1)`。循环每轮会同时处理两个连续的32×32的tile：`threadIdx.y=0..31`的线程处理第一个tile，`threadIdx.y=32..63`的线程处理第二个tile；下一轮按 `gridDim.x*2`处理后续tile分组。

```cpp
__ubuf__ float tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
...
tile[local_tile][ty][tx] = input[x_index + y_index * width];
asc_syncthreads();
output[x_index + y_index * height] = tile[local_tile][tx][ty];
asc_syncthreads();
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
|       27.113       |    25.043    |     41321.5     |      24.581      |     0.982     |        0.450        |      0.018      |

**分析**：

Case 4将Thread Block数固定为硬件vector core数。相比Case 3启动512个Thread Block的实现，Case 4需要启动和调度的线程块数量更少，Task Duration由28.190μs降至27.113μs。增加 `--cce-res-usage` 编译选项后，可以在编译日志中查看寄存器和栈的使用情况，Case 4的输出如下：

```text
[BISHENG] Function properties for _Z25transpose_ub_2tile_kernelPfPKfjjj_simt_entry: Stack size: 24 bytes, Used register number: 16
```

该输出说明Case 4已经出现寄存器溢出。Case 4与Case 3同为2048线程配置，但Case 4将Thread Block数固定为硬件vector core数后，需要在kernel内部通过循环处理多个tile分组，索引更新逻辑比Case 3更复杂，需要维护的中间变量更多；同时 `__launch_bounds__(2048)`限制了每个线程最多可使用16个寄存器，寄存器不足后部分变量会落到栈上，引入额外访存。寄存器溢出会抵消固定Thread Block数带来的收益，Case 5改用1024线程配置，使一个Thread Block每轮只处理一个tile，先消除寄存器溢出。

---

### Case 5：降低线程数避免寄存器溢出

**优化目标**：改为1024线程，将单轮处理粒度调整为一个32×32的tile，确定后续优化的基础配置。

**核心实现**：在Case 4的基础上改为每个Thread Block启动1024个线程，配置参数为 `dim3(32,32,1)`。一个32×32的tile包含1024个元素，因此一个Thread Block在循环每轮恰好处理一个tile；下一轮按 `gridDim.x`处理后续tile。

```cpp
__ubuf__ float tile[TILE_DIM][TILE_DIM];
...
tile[ty][tx] = input[x_index + y_index * width];
asc_syncthreads();
output[x_index + y_index * height] = tile[tx][ty];
asc_syncthreads();
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
|       23.912       |    22.745    |     37528.9     |      22.297      |     0.980     |        0.435        |      0.019      |

**分析**：

改为1024线程后，Stack size归零，没有寄存器溢出现象，Task Duration为23.912μs，优于Case 4的2048线程版本，说明1024线程更适合该转置实现。此时aiv_time已经接近Task Duration，线程块启动和调度开销占比已经较小；相对Case 2的直接转置（36.337μs），UB中转将非连续全局内存写转换为UB内访问，配合固定Thread Block数的循环处理方式，耗时下降约34%。

在当前配置下，寄存器溢出已经消除，Thread Block启动和调度开销占比也较小，下一步需要关注UB内部访问开销。GM写入阶段需要按转置方向读取 `tile[tx][ty]`，即按列访问32×32的UB tile，容易产生bank冲突，因此后续需要优先解决转置读阶段的UB bank冲突问题。

---

### Case 6：增加UB padding缓解UB bank冲突

**优化目标**：在Case 5的基础上，增加UB padding消除转置读阶段的bank冲突。

**核心实现**：

- GM到UB阶段与Case 5完全一致。
- 差异只在UB布局，从32×32改成32×34。
- 同步后，从UB写入GM与Case 5完全一致。
- 该版本没有改变算法路径，也没有改变Thread Block和线程切分，只是调整了UB中的物理布局。

下面以Ascend 950PR/Ascend 950DT的UB划分规则为例，说明本样例中bank冲突是如何产生的，以及Case 5和Case 6理论上的冲突强度差异。

UB中的bank划分如下图所示。Ascend 950PR/Ascend 950DT的UB划分为16个bank，并组织为8个bank group；SIMT编程模式下，每个bank进一步划分为4个subbank。若同一个Warp内多个线程在同一条UB访问指令中访问同一个bank group的相同编号subbank，硬件需要排队处理，从而形成subbank冲突并增加访问延迟。

SIMT编程方式下，bank冲突为更细粒度的subbank冲突，主要有以下两类：

- **写写冲突**：多个写操作同时访问同一个bank group的相同编号subbank。
- **读读冲突**：多个读操作同时访问同一个bank group的相同编号subbank。

详细的UB地址低位交织规则以及冲突场景可参考[bank_conflict样例](../../../02_features/01_api_features/00_memory_access/bank_conflict)。

**图3　UB bank结构示意图**

<img src="./figures/bank结构示意图.png">

Case 5中UB的tile数组按照行优先存储。按照地址低位交织规则，tile数组的第一行刚好覆盖bank0～bank3，第二行覆盖bank4～bank7，第三行覆盖bank8～bank11，其余行依次类推。每行32个float数据会恰好跨越4个bank存储。Case 5中，一个Warp的线程会读取tile的一列元素并写入GM输出矩阵；访问UB时，32个线程会集中访问两个bank group的subbank0，属于读读冲突场景。

**图4　Case5 tile在UB上的排布示意图**

<img src="./figures/case5_bank.png">

Case 6中对UB中的tile数组增加两列padding，由每行32个元素改为34个元素，其在UB上的排列如下图所示。由于每行有34个元素，行跨度变为17个subbank，同一列元素会错开排布到不同的subbank。Case 6访问UB时，32个线程的访问会分布到各个bank group的每个subbank中，即同一条访问指令下每个subbank仅有一个线程访问，从而避免上述读读冲突。

**图5　Case6 tile在UB上的排布示意图**

<img src="./figures/case6_bank.png">

```cpp
constexpr int TILE_PAD = 2;                          // Case6/7的UB padding列数（32x34）
constexpr int TILE_PAD_STRIDE = TILE_DIM + TILE_PAD;
__ubuf__ float tile[TILE_DIM][TILE_PAD_STRIDE];
...
tile[ty][tx] = input[x_index + y_index * width];
asc_syncthreads();
output[x_index + y_index * height] = tile[tx][ty];
asc_syncthreads();
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
|       15.152       |    13.641    |     22507.6     |      13.176      |     0.966     |        0.453        |      0.033      |

**分析**：

相对Case 5的23.912μs，padding将Task Duration降至15.152μs，下降约37%。该优化仅调整UB物理布局，不改变GM读写坐标和算法路径，说明转置读阶段的bank冲突是Case 5的主要瓶颈。消除bank冲突后，每轮循环仍保留数据加载后和GM写入后两次同步，后续可进一步降低同步开销。

---

### Case 7：使用双缓冲减少同步开销

**优化目标**：去除Case 6每轮循环末尾的 `asc_syncthreads()`，减少同步开销。

**核心实现**：Case 6每轮循环包含两次同步：数据加载到UB后进行一次同步，确保当前tile全部写入UB后再执行转置方向读取；数据写入GM后进行一次同步，确保当前tile的转置读取完成后再进入下一轮数据加载，避免下一轮数据加载覆盖仍在被读取的UB tile。本版本使用两份带padding的UB tile，并按 `cnt`选择当前轮使用的buffer。去除循环尾部同步后，当前轮GM写入阶段可以与下一轮数据加载前的地址计算及GM读取阶段重叠执行，因此每轮仅需保留数据加载后的同步。

```cpp
__ubuf__ float tile[2][TILE_DIM][TILE_PAD_STRIDE]; // 双缓冲：轮换缓冲区，去除尾部同步
uint32_t cnt = 0;
for (uint32_t tile_id = blockIdx.x; tile_id < total_tiles; tile_id += gridDim.x) {
    ...
    tile[cnt][ty][tx] = input[x_index + y_index * width];
    asc_syncthreads(); // 保留数据加载后的同步
    output[x_index + y_index * height] = tile[cnt][tx][ty];
    cnt ^= 1; // 切换到下一份buffer
}
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
|       12.074       |    10.725    |     17696.8     |      10.285      |     0.959     |        0.428        |      0.040      |

**分析**：

相对Case 6的15.152μs，去除每轮循环末尾的同步后Task Duration降至12.074μs，下降约20%。在1024×1024矩阵、64个Thread Block的配置下，每个Thread Block需要循环处理16个tile，双缓冲相当于每个Thread Block减少16次Thread Block内同步。该方法的代价是额外占用一份UB tile（共两份32×34），但未引入寄存器溢出。Case 7是本样例中耗时最低的SIMT转置版本，相对Case 0复制基线（6.262μs）约为1.9倍。

Case 6与Case 7的仿真指令流水图分别如图6、图7所示，其中耗时最多的SIMT_LDG和SIMT_STG分别为SIMT编程模式下从GM读取数据和向GM写入数据的指令。Case 6每轮循环受尾部 `asc_syncthreads()` 约束，当前轮GM写入阶段完成后才能进入下一轮循环，下一轮数据加载前的地址计算与GM读取阶段需要串行执行。Case 7通过双缓冲轮换两份UB tile，去除尾部同步后，当前轮GM写入阶段可与下一轮数据加载前的地址计算以及GM读取阶段重叠执行，减少数据加载阶段与地址计算阶段之间的同步等待。因此，Case 7的Task Duration相对Case 6进一步下降。

**图6　Case 6仿真指令流水图**

<img src="./figures/case6_trace.png">

**图7　Case 7仿真指令流水图**

<img src="./figures/case7_trace.png">

## 性能对比总结

### Ascend 950PR性能数据

| Case | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| ---- | :----------------: | :-----------: | :--------------: | :---------------: | :-----------: | :------------------: | :--------------: |
| 0    |       6.262       |     4.971     |      8201.7      |       4.516       |     0.909     |        0.443        |      0.089      |
| 1    |       8.649       |     7.321     |     12079.8     |       6.872       |     0.939     |        0.437        |      0.059      |
| 2    |       36.337       |    34.022    |     56136.2     |      33.568      |     0.987     |        0.442        |      0.013      |
| 3    |       28.190       |     2.896     |      4778.1      |       2.712       |     0.938     |        0.172        |      0.058      |
| 4    |       27.113       |    25.043    |     41321.5     |      24.581      |     0.982     |        0.450        |      0.018      |
| 5    |       23.912       |    22.745    |     37528.9     |      22.297      |     0.980     |        0.435        |      0.019      |
| 6    |       15.152       |    13.641    |     22507.6     |      13.176      |     0.966     |        0.453        |      0.033      |
| 7    |       12.074       |    10.725    |     17696.8     |      10.285      |     0.959     |        0.428        |      0.040      |

**综合优化效果**：

- 转置主路径Case 2 -> Case 5 -> Case 6 -> Case 7，Task Duration由36.337μs依次降至23.912μs、15.152μs、12.074μs，相对Case 2提升约3.01倍。
- 耗时最低的Case 7相对Case 0复制基线约为1.93倍，差距主要来自UB中转、Thread Block内同步以及UB访问开销。

## 调优建议

访存类算子调优时，建议先用连续复制确认当前规模下的GM搬运上限，再用最直接的实现暴露主要差距。本样例中，直接转置与复制基线的差距主要来自非连续GM写，因此优化方向不是增加计算并行度，而是重排访存：以32×32的tile为单位连续读入UB，在UB内完成转置方向读取，再按输出矩阵行方向连续写入GM。

分块后需要先确定Thread Block到tile的映射方式。按切分后的tile分组启动Thread Block实现简单，但当tile数远大于硬件vector core数时，会带来额外的启动和调度开销；将Thread Block数固定为硬件vector core数，并在kernel内循环处理多个tile，可以降低这部分开销。线程数也不能只参考连续搬运场景，高线程数会限制每个线程可使用的寄存器数量，索引逻辑变复杂后可能出现寄存器溢出，需要结合 `--cce-res-usage` 和实测耗时选择。

当GM访问已经转为连续后，瓶颈会转移到UB访问和同步。转置读UB时，如果按列访问32×32的tile，容易出现bank冲突，可以通过padding改变UB物理布局；循环中的同步也需要结合数据覆盖关系判断是否必要，双缓冲可以在不覆盖当前读取数据的前提下减少一次同步。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
  >
- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=7                                                                     # 选择执行场景，可选0-7
  mkdir -p build && cd build;                                                        # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
  ./matrix_transpose                                                        # 执行样例
  ```
- 编译选项说明

  | 选项                        | 可选值       | 说明                                                       |
  | --------------------------- | ------------ | ---------------------------------------------------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM`            | `0`-`7`  | 样例类型，默认为7                                          |


  - `--cce-res-usage`用于输出寄存器和栈等编译资源信息，便于分析资源使用。

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy verification passed.
  ```

## 性能分析

使用 `msOpProf` 工具获取详细性能数据：

```bash
msopprof ./matrix_transpose   # 分析case的性能
```

命令执行完成后，会在输出目录生成以 `OPPROF_{timestamp}_XXX`命名的文件夹，性能数据文件夹结构示例如下：

```text
├── dump                       // 原始性能数据
├── ArithmeticUtilization.csv  // cube/vector指令cycle占比
├── L2Cache.csv                // L2 Cache命中率
├── Memory.csv                 // UB、L1和主存储器读写带宽
├── MemoryL0.csv               // L0A、L0B、L0C读写带宽
├── MemoryUB.csv               // Vector和Scalar到UB的读写带宽
├── OpBasicInfo.csv            // 算子基础信息，包含Task Duration(μs)
├── PipeUtilization.csv        // 计算单元和搬运单元耗时及占比
├── ResourceConflictRatio.csv  // UB bank group、bank conflict和资源冲突率
└── visualize_data.bin         // MindStudio Insight呈现文件
```

## 仿真调优

可以使用 `msopprof simulator` 进行仿真性能分析，生成可视化的指令流水图等信息。命令如下：

```bash
SCENARIO_NUM=7                                                                     # 选择执行场景，可选0-7
mkdir -p build && cd build;                                                        # 创建并进入build目录
cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
msopprof simulator --soc-version=<soc_version> ./matrix_transpose
```

> 使用仿真调优功能前，需要在 CMakeLists.txt 中添加 `-g` 编译选项，用于生成调试信息，使仿真器能够采集指令流水图。`soc_version` 获取方式及仿真调优其他说明可参考[仿真调优样例](../../../01_utilities/07_simulator)。

命令完成后，会在当前目录下生成以 `OPPROF_{timestamp}_XXX` 命名的文件夹，产物结构如下：

```text
OPPROF_{timestamp}_XXX/
├── dump                    // 原始性能数据，用户无需关注
└── simulator
    ├── core*.veccore*/     // 各向量核的仿真指令流水图文件
    └── visualize_data.bin  // MindStudio Insight呈现文件
```

执行后，可以在 **MindStudio Insight** 中打开 `visualize_data.bin` 查看可视化指令流水图。

更多 `msOpProf` 工具使用方法，请参见[MindStudio工具调优（msOpProf）快速入门](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md)。
