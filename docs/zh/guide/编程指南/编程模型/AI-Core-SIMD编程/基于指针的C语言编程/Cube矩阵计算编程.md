# Cube矩阵计算编程<a name="ZH-CN_TOPIC_0000002600000002"></a>

本文介绍基于Ascend C语言扩展API，依托L1 Buffer、L0系列片上缓存实现矩阵计算的开发方法，该方式是Ascend C中最基础、通用的矩阵编程方案。AI Core中的Cube单元部署在AIC核，负责矩阵计算，拥有专属片上缓存：L0A Buffer存放左矩阵、L0B Buffer存放右矩阵、L0C Buffer用于保存初始累加值与矩阵运算结果。

## Cube矩阵计算核心步骤

Cube矩阵计算整体分为数据搬入、数据加载、矩阵计算、数据搬出四个阶段，完整流程如下：
1. 通过DMA将数据从Global Memory搬运至L1 Buffer；
2. 将L1 Buffer中的数据加载至L0A Buffer、L0B Buffer；
3. 执行矩阵计算，结果存入L0C Buffer；
4. 通过DMA将计算结果从L0C Buffer搬运回Global Memory。

## 矩阵计算内存管理

### 矩阵计算内存申请

矩阵计算主要依托L0 Buffer完成运算。开发者需要先将输入数据从Global Memory搬运至L1 Buffer，再加载到L0 Buffer，最后调用Cube计算指令完成运算。L1 Buffer与L0 Buffer均通过数组形式声明，本指南主要介绍静态内存申请方式。

- 静态申请：分配一段指定大小的内存空间，空间大小在编译时确定，不可动态修改。

  ```c
  __global__ __cube__ void add_custom(...)
  {
      constexpr uint32_t m_size = 128;
      constexpr uint32_t n_size = 128;
      constexpr uint32_t k_size = 256;
      // Allocate L1A/B Buffer
      __cbuf__ half static_l1a_buffer[m_size * k_size];
      __cbuf__ half static_l1b_buffer[n_size * k_size];
      // Allocate L0A/B/C Buffer
      __ca__ half static_l0a_buffer[m_size * k_size];
      __cb__ half static_l0b_buffer[n_size * k_size];
      __cc__ half static_l0c_buffer[m_size * n_size];

      // ... Use Buffer for matrix computation
  }
  ```

### 矩阵计算内存布局

Cube单元采用分块计算逻辑，硬件最小计算粒度为分形块，可并行处理多个分形。分形块大小取决于数据类型：对于half类型，分形形状为16×16（32B/sizeof(half)=16）；对于int8类型，分形形状为16×32或32×16（32B/sizeof(int8)=32）。传统行主序、列主序等线性存储模式下，读取分形块时会产生大量非连续内存访问，大幅降低访存效率。
为解决该问题，昇腾引入矩阵分形存储格式。软件层会对数据重新排布，让每个分形块在物理内存中连续存储，硬件单次读取即可加载整块数据，消除零散寻址带来的开销，有效提升数据吞吐能力。
分形粒度是Cube单元的基础计算规范，[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)、[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)全系列产品均遵循该规范，但具体分形形状因数据类型而异。昇腾Cube单元为极致发挥算力，采用不同于通用CPU的存储规则，输入、输出数据都需要匹配指定分形格式。使用Ascend C的Mmad接口开发算子时，掌握各类分形格式，是保证数据搬运、分块逻辑正确以及性能优化的关键。

为便于理解与记忆，文档采用“大Y小x”的直观命名法：
- 大Y（Z/N）：表示分形矩阵之间的排列顺序（Z为行主序，N为列主序）。
- 小x（z/n）：表示分形矩阵内部元素的排列顺序（z为行主序，n为列主序）。

### 关键分形格式详解

矩阵分形格式用于定义多维张量在内存中的排布规则。受硬件读取逻辑影响，不同产品型号对矩阵乘法 C = A × B 的格式要求存在差异：

- 针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，矩阵乘法C = A × B要求：左矩阵A使用Zz格式，右矩阵B使用Zn格式，结果矩阵C使用Nz格式。如图1所示：

<img src="../../../../figures/矩阵乘法场景设计的数据格式.png" alt="矩阵乘法场景设计的数据格式" width="800" />

- 针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，矩阵乘法C = A × B要求：左矩阵A使用Nz格式，右矩阵B使用Zn格式，结果矩阵C使用Nz格式，如图2所示：

<img src="../../../../figures/矩阵乘法场景涉及的数据格式.png" alt="矩阵乘法场景涉及的数据格式" width="800" />

#### ND 格式(N-Dimension)

ND格式是通用的N维张量格式，数据在内存中连续线性存放，通常存在于`Global Memory (GM)`中。在进行矩阵运算前，通常需要将其转换为Nz格式以适配Cube计算单元。

#### Nz 格式
Nz格式主要用于L1 Buffer和L0C Buffer中存放数据。针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，L0A Buffer也采用Nz分形格式。该格式采用大N（外部列主序）+ 小z（内部行主序）的方式排布。

<img src="../../../../figures/Nz数据排布格式.png" alt="Nz数据排布格式" width="400" />


Nz是Ascend架构的中间格式，介于线性数据(ND)和计算专用数据(Zz/Zn)之间，便于格式转换。大N排布（列主序Block）的设计通常是为了配合多核并行或后续算子（如Vector向量计算）在处理通道（Channel）维度数据时的便利性。

> 📌 Nz格式在不同Buffer中的应用场景：
> - **L0C Buffer**: 存储矩阵乘法结果。分形形状取决于数据类型：half为16×16（256元素），int8为16×32或32×16。适合Cube计算单元高效运算。
> - **L1 Buffer**: 存储中间数据。分形形状为16×(32B/sizeof(DataType))，便于转换为Zz和Zn格式。对于half类型，大小为16×16=256元素（512字节）；对于int8类型，大小为16×32或32×16=512元素（512字节）。
> - **L0A Buffer（仅[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)）**: 存储左矩阵。

#### Zz 格式

针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，L0A Buffer使用此格式（[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)使用Nz格式）。该格式采用大Z（外部行主序）+ 小z（内部行主序）。

<img src="../../../../figures/Zz格式-以half类型为例.png" alt="Zz 格式(以half类型为例)" width="400" />


在矩阵乘法中，A矩阵逐行读取数据。Zz格式的行主序特性使同一行元素物理地址连续，与计算访问模式匹配，便于Cube单元左侧通道通过DMA连续读取，避免跨步访问，提升数据搬运效率。

#### Zn 格式

Zn格式主要用于L0B Buffer中存放数据，如果离线处理好的右矩阵数据直接为Zn格式，则在`Global Memory`和`L1 Buffer`上存储该格式。该格式采用大Z（外部行主序）+ 小n（内部列主序）。

<img src="../../../../figures/Zn格式-以half类型为例.png" alt="Zn 格式(以half类型为例)" width="400" />


矩阵乘法需读取B矩阵列向量。普通行主序存储导致列数据地址跳跃。Zn格式在分形内部按列存储，使列数据在L0B中紧凑排列，便于Cube单元右侧通道线性读取完整列向量，避免跨步访问。

### 格式转换

Global Memory中的原始数据为ND通用线性格式，而L1 Buffer要求使用Nz格式，因此数据搬运过程中需要完成ND到Nz的格式转换。

<img src="../../../../figures/ND2NZ格式转换.png" alt="ND2NZ格式转换" width="800" />

以上图为例，图中展示了一个具体的M=40, K=56的矩阵（数据类型为half），在从Global Memory（GM）搬运至L1 Buffer并转换为Nz格式时的内存排布变化：
- 原始数据 (ND 格式)：
  左侧GM中存储的是原始矩阵，shape为40×56。
  数据呈线性连续排布（图中长箭头所示），遵循行主序（Row Major）规则。
- 对齐与填充 (Padding)：
  由于Cube单元的分形（Fractal）要求，目标矩阵必须能够被16×16的Block整除（假设数据类型为half，即32/sizeof(half)=16）。
  M轴填充：原始M=40不能被16整除，需向上补齐至48（16×3）。
  K轴填充：原始K=56不能被16整除，需向上补齐至64（16×4）。
  图中灰色区域即为补齐产生的无效数据（Padding），白色区域为原始有效数据。
- 重排布 (Nz 格式)：
  右侧L1 Buffer中，数据被重组为3×4个分形块。
  Z字流动：每个16×16的小方块内部，以及方块之间，均按照特定的Z字形顺序存储（如图中折线箭头所示），完成了从“线性”到“分形”的物理映射，做好了进入Cube计算的准备。

Ascend C编程中直接提供了`Global Memory`到`L1 Buffer`中的C语言编程接口，通过该接口可以直接将`Global Memory`数据搬入`L1 Buffer`并完成ND2NZ格式转换。
C语言编程提供了`asc_set_gm2l1_nz_para`和`asc_copy_gm2l1_nd2nz`接口来完成GM2L1的搬运，并随路完成ND2NZ的格式转换处理。另外在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)上，新增了`asc_copy_gm2l1_dn2nz`的能力，支持从DN到Nz的格式转换搬运。

## 矩阵搬运能力说明

### 矩阵数据搬入

矩阵搬入接口实现Global Memory到L0A/L0B Buffer的数据传输，通常与Mmad接口配合使用。该接口支持多种搬运模式：二维搬运、三维格式转换、压缩数据解压、矩阵转置、稀疏化数据处理等。

开发者通过配置参数控制搬运路径、起始位置、长度等信息，充分利用硬件能力，最大化算子性能。每种数据通路对应特定的存储层级和访问特性。

结合上述数据流和分形知识，C语言编程提供以下矩阵搬入能力（完整接口见API参考手册）：
| 数据通路 | 功能    | C语言编程接口  | 分行支持情况 |
|---------|---------|---------------|-------------|
| Global Memory → L1 Buffer   | 随路转换ND2NZ搬运  | `asc_copy_gm2l1_nd2nz` | ![ND->Nz](../../../../figures/ND-Nz.png) |
| L1 Buffer → L0A Buffer  | 将L1 Buffer数据搬运到L0A Buffer中  | `asc_copy_l12l0a` | ![Nz->Zz](../../../../figures/Nz-Zz.png)<br>[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)上L0A分形为Nz，搬运时为Nz-Nz |
| L1 Buffer → L0A Buffer  | 将L1 Buffer数据搬运到L0B Buffer中  | `asc_copy_l12l0b` | ![Nz->Zn](../../../../figures/Nz-Zn.png)|

[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)

#### GM2L1 随路转换ND2NZ搬运

该功能用于将Global Memory中的矩阵搬运至L1 Buffer，并在搬运过程中同步完成ND到Nz的格式转换，主要依托`asc_copy_gm2l1_nd2nz`接口实现，通过配置对应参数即可完成数据搬运与格式转换。

<img src="../../../../figures/GM到L1的ND2NZ搬运示例.png" alt="GM到L1的ND2NZ搬运示例" width="700" />

以上图为例，实现A矩阵（Shape(16, 23)，数据类型为half）从GM搬运到L1的能力，针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，调用接口如下：

```c
// Allocate L1 Buffer space, m direction is 16, k direction 23 aligned up to 32
constexpr uint64_t m = 16;
constexpr uint64_t k = 32;
__cbuf__ half dst[m * k];

// Set parameters for dst matrix (L1 Buffer) via asc_set_gm2l1_nz_para interface
constexpr uint64_t dst_nz_matrix_stride = 0, dst_nz_c0_stride = 16, dst_nz_n_stride = 1, dst_nd_num = 1;
// Offset between starting addresses of adjacent Nz matrices in destination matrix
constexpr uint64_t nz_config = static_cast<uint64_t>(dst_nz_matrix_stride) << 48;
// After ND conversion to Nz format, offset between starting addresses of adjacent Z matrices in destination Nz matrix
nz_config |= static_cast<uint64_t>(dst_nz_c0_stride) << 32;
nz_config |= static_cast<uint64_t>(dst_nz_n_stride) << 16;
nz_config |= static_cast<uint64_t>(dst_nd_num);
asc_set_gm2l1_nz_para(nz_config);

// Number of elements contained in one row of source matrix
constexpr uint64_t src_d_value = 32;
// Offset between starting addresses of adjacent ND matrices in source matrix
constexpr uint64_t src_nd_matrix_stride = 0;
// L2 Cache uses NORMAL mode
constexpr uint8_t l2_cache_ctrl = 1;
// Number of rows in ND matrix of source matrix
constexpr uint16_t n_value = 16;
// Number of columns in ND matrix of source matrix
constexpr uint32_t d_value = 23;
// When dValue is less than or equal to 4, can enable SmallC0 mode, C0_SIZE will pad to 4 * sizeof(T) Bytes. In this mode, dst_n_stride parameter and dst_c0_stride parameter are invalid, no need to configure.
constexpr bool enable_small_c0 = false;
asc_copy_gm2l1_nd2nz(dst, src, src_d_value, l2_cache_ctrl, n_value, d_value, src_nd_matrix_stride, enable_small_c0);
```

#### L12L0 数据搬运

该能力主要实现将矩阵从L1 Buffer搬运至L0A/L0B Buffer并支持从Nz分型转换到Zz或Zn格式，C语言编程提供了`asc_copy_l12l0a`和`asc_copy_l12l0b`接口。其中针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)的`asc_copy_l12l0a`接口可将L1 Buffer中512B大小的矩阵搬运到L0A Buffer，支持2D搬运、转置搬运、3D搬运等模式。

<img src="../../../../figures/L12L0A非转置搬运.png" alt="L12L0A非转置搬运" width="700" />

通过如下方式调用可完成上述的`L1 Buffer`到`L0A Buffer`的搬入操作。
```c
__cbuf__ half l1_buffer[6400];
__ca__ half l0a_buffer[2304];
// m_step is 2, k_step is 3, indicating transfer of 6 data blocks
uint8_t m_step = 2;
uint8_t k_step = 3;
// mStartPosition is 2, indicates M direction transfer starting position is at M axis direction 16*2=64 elements away from source operand starting position
uint16_t m_start_position = 2;
// kStartPosition is 2, indicates K direction transfer starting position is at K axis direction 32B*2=64B position away from source operand starting position
uint16_t k_start_position = 2;
// srcStride is 5, indicates in source operand, interval between starting addresses of previous fractal and next fractal in K direction is 5
int16_t src_stride = 5;
// dstStride is 3: indicates in destination operand, interval between starting addresses of previous fractal and next fractal in K direction is 3
int16_t dst_stride = 3;
asc_copy_l12l0a(l0a_buffer, l1_buffer, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);
```

搬运到`L0B Buffer`采用`asc_copy_l12l0b`实现，接口参数及实现功能同L12L0A，可参考[cube_datamove](../../../../../api/SIMD-API/C-API/cube_datamove/矩阵数据搬运.md)查看更多内容。

当输入A矩阵（Global Memory）是转置排布(K × M)时，搬入到`L1 Buffer`为(K × N)排布的Nz分形。要使`L0A Buffer`存放(M × K)的Zz或Nz格式（[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)），需要在L12L0A时进行转置，调用`asc_copy_l12l0a_transpose`接口。

<img src="../../../../figures/需要转置的处理场景.png" alt="需要转置的处理场景" width="800" />

```c
// Use transpose interface to complete processing
asc_copy_l12l0a_transpose(l0a_buffer, l1_buffer, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);
```

### 矩阵数据搬出

矩阵搬出接口实现L0C到Global Memory或L1 Buffer的数据传输，支持随路量化、ReLU、格式转换（NZ2ND）、通道拆分合并等操作，开发者通过配置参数控制搬运路径、位置、长度等以充分利用硬件能力，通常在Mmad接口后使用。而针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)还支持L0C到UB Buffer的传输、NZ2DN格式转换以及UB双目标模式，矩阵搬出能力如下图所示：

针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)：

<img src="../../../../figures/矩阵搬出流程-AtlasA2A3.png" alt="矩阵搬出流程" width="800" />

针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)：

<img src="../../../../figures/矩阵搬出流程-Ascend950PR.png" alt="矩阵搬出流程" width="800" />

矩阵搬出接口支持多种随路能力的灵活组合，以L0C到Global Memory的搬运场景为例：矩阵计算结果暂存于L0C Buffer后需搬运至GM，在此过程中可协同执行随路量化、ReLU激活、格式转换及通道拆分合并等操作。下图展示了这些随路能力的有效组合、中间数据类型及完整数据路径。例如，L0C中的F32数据可通过QF322B8_PRE随路量化得到B8数据，可选使能ReLU后经NZ2NZ/NZ2ND格式转换输出至GM。图中F32→F16和F32→BF16为数据类型转换，其余路径为随路scalar/vector量化模式；针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，还额外支持NZ2DN格式转换。

针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)：

<img src="../../../../figures/L0C2GM搬运-AtlasA2A3.png" alt="L0C2GM搬运" width="800" />

针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)：

<img src="../../../../figures/L0C2GM搬运-Ascend950PR.png" alt="L0C2GM搬运" width="800" />

C语言编程提供了`asc_copy_l0c2gm`来使能发挥芯片的各种随路能力，通过直接传入配置参数可完成随路搬运的操作。
```c
__aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctrl, uint64_t quant_pre, uint8_t relu_pre, 
                        bool enable_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post, 
                        bool enable_clip_relu_post, uint8_t eltwise_op, bool enable_eltwise_antq, bool enable_c0_pad,
                        bool enable_broadcast, bool enable_nz2dn);

```

另外，对于是否开启unit_flag，nz2nd信息的设置，需配置相应寄存器参数以启用不同功能。对应的接口和处理能力如下：
```c
// 1.0 When performing inline quantization during data movement, call this interface to set vector quantization parameters in the quantization flow.
__aicore__ inline void asc_set_l0c2gm_config(uint64_t relu_pre_addr, uint64_t quant_pre_addr, bool enable_unit_flag);

// 2.0 When performing inline format conversion (NZ to ND) during data movement, call this interface to set format conversion configuration.
__aicore__ inline void asc_set_l0c2gm_nz2nd(uint64_t nd_num, uint64_t src_nd_stride, uint64_t dst_nd_stride);

// 3.0 Set the value in RELU_ALPHA register. This is a 64-bit register storing the alpha value used in Scalar ReLU during fixpipe or cube instructions.
__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(half& config);
```

详细说明可参考接口说明文档[Cube接口说明](../../../../../api/SIMD-API/C-API/cube_datamove/asc_copy_l0c2gm)。


## 矩阵计算能力说明

`asc_mmad`是Ascend C封装NPU硬件能力的核心矩阵乘加接口，广泛应用于全连接、卷积等算子，实现C = A × B + C运算。计算输入取自L0A Buffer、L0B Buffer，结果写入L0C Buffer。不同产品对应的缓存分形格式前文已说明。

![[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md) `asc_mmad`计算](../../../../figures/Ascend950PR-Ascend950DT-asc_mmad计算.png)

表 矩阵计算矩阵A、B、C解释说明([NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md))
| 矩阵计算逻辑 | 矩阵计算物理位置 | 维度 | 输入/输出数据格式 | 数据类型 |
|-------------|-----------------|------|------------------|---------|
| A | L0A Buffer | M × K | Nz | 数据类型 |
| B | L0B Buffer | K × N | Zn | 数据类型 |
| C | L0C Buffer | M × N（可支持使用偏置矩阵Bias进行初始化，维度为1 × N） | Nz | 数据类型 |

C语言编程提供了一系列`asc_mmad`重载接口，支持不同数据类型的矩阵计算。以下代码实现上图中的矩阵乘计算。

```c
// 1. Define L0A/L0B matrix sizes
constexpr uint32_t l0_m_size = 64;
constexpr uint32_t l0_k_size = 64;
constexpr uint32_t l0_n_size = 64;
__ca__ a_matrix[l0_m_size * l0_k_size];
__cb__ b_matrix[l0_k_size * l0_n_size];
__cc__ c_matrix[l0_m_size * l0_n_size];

// 2. Configure asc_mmad parameters
// 2.1. Actual matrix sizes participating in computation (m, n, k)
// m direction: 3 fractals, actual value = 3 × 16 = 48
constexpr uint16_t m = 48;
// n direction: 3 fractals, actual value = 3 × 16 = 48
constexpr uint16_t n = 48;
// k direction: 4 fractals, actual value = 4 × 16 = 64
constexpr uint16_t k = 64;

// 2.2 Feature parameters
// unit_flag: Controls fine-grained parallelism of Mmad and Fixpipe instructions. When enabled, results are transferred out immediately after computing each fractal.
uint8_t unit_flag = 0;
// disable_gemv: Configures whether to enable GEMV mode when M=1
bool disable_gemv = false;
// enable_cube_init: Enables C matrix initial value loading. false: do not initialize L0C; true: load initial value from BT Buffer.
bool enable_cube_init = false;
// enable_cube_zero_init: Enables C matrix default zero initialization. true: initialize to 0; false: control initialization via enable_cube_init parameter.
bool enable_cube_zero_init = true;

asc_mmad(c_matrix, a_matrix, b_matrix, m, n, k, unit_flag, disable_gemv, enable_cube_init, enable_cube_zero_init);
```

## 矩阵计算的同步

AI Core内部的搬运、计算单元均为异步并行工作，多个单元访问同一片存储资源时会产生数据依赖。因此Cube计算必须借助流水同步接口管控执行时序。
矩阵计算的四个阶段分别对应PIPE_MTE2、PIPE_MTE1、PIPE_M、PIPE_MTE3四条流水线，需要使用核内同步接口保证流程串行执行。在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品中，可使用`asc_lock`与`asc_unlock`接口实现全流程同步控制。

```c
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    ...
    // 1. Step 1: Data transfer in (Global Memory → L1 Buffer), execution pipeline is PIPE_MTE2
    asc_lock(PIPE_MTE2, mutex_id);
    asc_copy_gm2l1_nd2nz(a_l1_buf, a_gm, 128, 1, BLOCK_M, BLOCK_K, 0, false);
    asc_copy_gm2l1_nd2nz(b_l1_buf, b_gm, 64, 1, BLOCK_K, BLOCK_N, 0, false);
    asc_unlock(PIPE_MTE2, mutex_id);
    ...
    // 2. Step 2: Data transfer in (L1 Buffer → L0A/B Buffer), execution pipeline is PIPE_MTE1
    asc_lock(PIPE_MTE1, mutex_id);
    asc_copy_l12l0a(l0a_buf, a_l1_buf, 0, 0, 4, 8, 1, 1);
    asc_copy_l12l0b_transpose(l0b_buf, b_l1_buf, 0, 0, 4, 8, 1, 1);
    asc_unlock(PIPE_MTE1, mutex_id);
    ...
    // 3. Step 3: Matrix computation (L0A/B Buffer → L0C Buffer), execution pipeline is PIPE_M
    asc_lock(PIPE_M, mutex_id);
    asc_mmad(l0c_buf, l0a_buf, l0b_buf, BLOCK_M, BLOCK_K, BLOCK_N, 0, false, false, false);
    asc_unlock(PIPE_M, mutex_id);
    ...
    // 4. Step 4: Data transfer out (L0C Buffer → Global Memory), execution pipeline is PIPE_MTE3
    asc_lock(PIPE_MTE3, mutex_id);
    asc_copy_l0c2gm(c_gm, l0c_buf, BLOCK_N, BLOCK_M, BLOCK_N, 16, 1, 0,
        0, QuantMode_t::NoQuant, 0, false, true, 0, 0, false, false, 0, false, false, false, false, false, false);
    asc_unlock(PIPE_MTE3, mutex_id);
}
```

## Cube矩阵计算示例

目前完整的矩阵计算工程样例仍在开发，后续版本会持续补充。开发者可查阅API文档了解[Cube接口详细说明](../../../../../api/SIMD-API/C-API)。
