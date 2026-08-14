# Memory矢量计算编程<a name="ZH-CN_TOPIC_0000002600000004"></a>

本文介绍如何在Ascend C中使用C++ Tensor编程API编写基于Unified Buffer（以下简称UB）的矢量计算代码，该能力是Ascend C最基础、最通用的矢量编程方式。这种基于UB内存的编程方式被称为Memory矢量计算编程。[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品支持完整的Memory矢量计算UB直接编程能力；而[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)虽也支持UB操作，但推荐使用Reg矢量计算编程以获得更高性能。

## Memory矢量编程的基本步骤

Memory矢量计算的数据流为：数据搬入 → 计算 → 数据搬出。当我们描述一个常见的Vector矢量计算时，需要执行以下三个步骤：

1. 通过DMA将数据从Global Memory搬入到Local Memory
2. 进行矢量计算
3. 通过DMA将数据从Local Memory搬出到Global Memory

## Memory矢量计算内存管理

Memory矢量编程的核心是基于Local Memory（UB）完成矢量计算，要求开发者先将输入数据从Global Memory（以下简称GM）搬入UB，再通过Vector计算指令执行运算。UB的管理依赖LocalTensor对象实现，开发者可通过LocalMemAllocator分配器申请UB空间，为后续矢量计算提供存储载体。

### 基础Tensor(不带Layout)定义

通过`LocalMemAllocator`内存分配器创建，用户可指定物理存储位置和内存大小。

```cpp
__global__ __vector__ void add_custom(...)
{
    constexpr uint32_t ub_size = 256;
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<half> static_buffer = ubAllocator.Alloc<half, ub_size>();
}
```

## Memory数据搬运

GM与UB之间的数据搬运提供了多样化的接口，支持丰富的数据搬运场景。为实现高效的Vector计算，系统支持将数据从GM搬运至UB，供Vector计算单元进行向量计算。计算完成后，再将结果从UB搬回GM，完成数据闭环。

开发者通过灵活配置DataCopyParams参数，精确控制数据块数量、地址长度及间隔等关键属性，实现连续和非连续数据搬运（高维切分数据搬运），充分释放硬件的数据搬运与计算协同能力，提升向量算子的执行效率。

| 数据通路 | 模式 | 接口 | 适用场景 |
|---|---|---|---|
| GM→UB | 连续搬运 | `DataCopy(dst, src, count)` | 一维连续数据搬运 |
| GM→UB | 高维切分 | `DataCopy(dst, src, dataCopyParams)` | 多块非连续数据，可配置块数和间隔 |
| GM→UB | 非对齐搬运 | `DataCopyPad(dst, src, dataCopyExtParams, padParams)` | 地址非32B对齐，支持自定义填充 |
| UB→GM | 连续搬运 | `DataCopy(dst, src, count)` | 一维连续数据搬运 |
| UB→GM | 高维切分 | `DataCopy(dst, src, dataCopyParams)` | 多块非连续数据，可配置块数和间隔 |
| UB→GM | 非对齐搬运 | `DataCopyPad(dst, src, dataCopyExtParams)` | 地址非32B对齐 |

根据上述矢量搬运能力的描述，开发者可以根据不同的场景选择合适的接口进行处理。

**连续搬运**: 如果开发者需要将一块连续的`Global Memory`搬运到UB时，只需指定搬运数据元素个数即可。例如srcGlobal数据（shape为256，数据类型为half），需搬入到UB中，通过`DataCopy`直接调用：

![连续数据搬运](../../../../figures/cont_copy.png)

```cpp
AscendC::DataCopy(dstLocal, srcGlobal, 256);
```

**高维切分搬运**: 当需要将多块非连续的GM数据搬入单个UB时，可通过高维切分模式配置搬运数据块的数量、单个数据块长度及块间间隔。以srcGlobal数据（shape为256，数据类型为half）为例，如下场景的数据搬运可直接通过C++ Tensor接口实现：

![非连续搬运](../../../../figures/discont_copy.png)

```cpp
// A total of 6 dataBlocks are transferred, split into two rounds of 3 dataBlocks each.
uint16_t blockCount = 2;
// Length of single data block (in 32B units): value is 3
uint16_t blockLen = 3;
// Gap from previous block to next block in source data (in 32B units): value is 1
uint16_t srcGap = 1;
// Gap from previous block to next block in destination data (in 32B units): interval of one datablock between destination blocks, value is 2
uint16_t dstGap = 2;

DataCopyParams copyParams {blockCount, blockLen, srcGap, dstGap};
AscendC::DataCopy(dstLocal, srcGlobal, copyParams);
```

**非对齐搬运**: 高维切分数据搬运提供了用户进行非连续多个数据块搬入的能力，但该接口要求GM中搬运的长度和间隔都必须保证是32B对齐的，因此对于一些非对齐场景无法支持。例如需要进行如下图所示的搬运，则无法通过`DataCopy`高维切分接口完成隔行数据的搬运，此时需要突破32B的约束限制。Ascend C根据芯片能力，提供了`DataCopyPad`接口来支持该搬运形式，该接口可以指定以字节为单位的blockLen和stride信息，完成搬运操作。

![非对齐搬入能力](../../../../figures/unaligned_in_cap.png)

在上图所示场景中，需将两个数据块从GM搬运至UB：每个数据块的blockLen为54B，源操作数中相邻数据块的间隔为1B，目的操作数中相邻数据块的间隔为32B。为满足32B对齐要求，需在blockLen左侧填充2个half类型元素、右侧填充3个half类型元素；此时总长度为blockLen + leftPadding × 2B + rightPadding × 2B = 54B + 2 × 2B + 3 × 2B = 64B，符合32B对齐约束。

```cpp
// Number of consecutive data blocks: transfer two consecutive data blocks
uint16_t blockCount = 2;
// Length of single data block (in bytes): value is 54
uint32_t blockLen = 54;
// Stride from previous block to next block in source data: source data stride unit is 1B
uint32_t srcStride = 1;
// Stride from previous block to next block in destination data: UB operation constraint address must be 32B aligned, value is 32 / 32
uint32_t dstStride = 32 / 32;
// Whether to fill with user-defined data: true means use custom pad value, false means fill with random values by default
bool isPad = true;
// Left padding of transfer result (element count): value is 4 / sizeof(half) = 2
uint8_t leftPadding = 2;
// Right padding of transfer result (element count): value is 6 / sizeof(half) = 3
uint8_t rightPadding = 3;

AscendC::DataCopyExtParams copyParams {blockCount, blockLen, srcStride, dstStride, 0}; 
AscendC::DataCopyPadExtParams<half> padParams {isPad, leftPadding, rightPadding, 0};

AscendC::DataCopyPad(dstLocal, srcGlobal, copyParams, padParams);
```

> 📌 该接口主要用于提升用户数据搬运的易用性，由软件层实现，底层基于高维切分接口封装。

如上完成了GM到UB的搬入，UB搬出到GM采用同样的接口处理。

更多搬运接口请参考API手册。

## Memory数据计算

Memory矢量计算提供连续计算和高维切分两种计算模式，并可通过掩码实现数据选择性参与计算。
以Add接口为例，两种计算模式的调用方式示意图如下。其中连续计算简单直观，适合一维Tensor连续数据处理；高维切分计算灵活可控，支持迭代执行和地址间隔配置。开发者可根据实际业务需求选择合适的计算方式。

![Memory矢量计算模式示意图](../../../../figures/mem_vec.png)

### 高维切分计算

高维切分能力主要是支持对计算接口设置`repeatTime`、`stride`和`mask`参数等，能精确控制指令的迭代执行、操作数的地址间隔和迭代内的掩码。

#### 迭代控制

Vector计算单元每个迭代会从UB中取出8个DataBlock（每个DataBlock内部地址连续，长度为32Byte）进行计算，并将结果写入对应的8个DataBlock中。

![DataBlock迭代示意图](../../../../figures/db_iter.png)

矢量计算API支持通过配置repeatTime参数控制指令的迭代执行次数。若将repeatTime设为2，矢量计算单元会执行2次迭代计算，可处理的数据量为2 × 8（单次迭代8个DataBlock） × 32Byte = 512Byte；若数据类型为half（2B / 元素），则对应计算256个元素。下图为2次迭代Exp计算的示意图。
注意：受硬件限制，repeatTime的取值范围为[1, 255]，该约束适用于所有支持Memory矢量计算的产品。

![2次迭代Exp计算](../../../../figures/iter2_exp.png)

#### 地址间隔配置

矢量计算单元还支持带地址间隔的向量计算能力，可通过dataBlockStride和repeatStride两个参数精准配置：
- dataBlockStride：单次迭代内不同DataBlock间的地址步长，取值不得超过UB的空间大小限制；
- repeatStride：相邻迭代间相同DataBlock的地址步长（下文详细说明）。

连续计算，`dataBlockStride`设置为1，对同一迭代内的8个DataBlock数据连续进行处理。
非连续计算，`dataBlockStride`值大于1（如取2），同一迭代内不同DataBlock之间在读取数据时出现一个DataBlock的间隔，如下图所示。

![dataBlockStride示例](../../../../figures/db_stride.png)

> 📌 若dst_block_stride == 0，等同于dst_block_stride = 1；若src_block_stride = 0，则源操作数将始终复用第一个dataBlock。

下图给出了单次迭代内源操作数与目的操作数在UB空间的读写示例。示例中源操作数的`dataBlockStride`配置为2，表示单次迭代内不同DataBlock间地址步长为2个DataBlock；目的操作数的`dataBlockStride`配置为1，表示单次迭代内地址连续。

![单次迭代内源和目的操作数读写示例](../../../../figures/iter_rw.png)

其中**repeatStride**是指相邻迭代间相同DataBlock的地址步长，可通过设置`repeatStride`来灵活控制不同的场景。

| 场景 | repeatStride取值 | 数据读取特点 | 适用场景 |
|------|------------------|-------------|--------|
| 连续计算 | 8 | 每轮迭代读取连续8个dataBlock，完成所有输入数据的计算 | ![repeatStride连续计算场景](../../../../figures/rep_cont.png) |
| 非连续计算 | >8 (如10) | 相邻迭代间存在dataBlock间隔，地址不连续 | ![repeatStride非连续计算场景](../../../../figures/rep_discont.png) |
| 反复计算 | 0 | 对首个连续8个dataBlock反复读取和计算 | ![repeatStride反复计算场景](../../../../figures/rep_repeat.png) |
| 部分重复计算 | (0, 8) | 相邻迭代间部分数据重复读取，一般场景不涉及 | ![repeatStride部分重复计算](../../../../figures/rep_partial.png) |

当 `repeatTime`>1 时，通过多次迭代完成计算。repeatStride表示相邻迭代间相同位置DataBlock的起始地址间隔（以DataBlock为单位）。例如 `repeatStride` = 9 时，第一次迭代的第1个DataBlock与第二次迭代的第1个DataBlock间隔9个DataBlock。

![多次迭代非连续场景示意图](../../../../figures/multi_iter.png)

通过C++ Tensor接口配置 `repeatStride` 和 `dataBlockStride` 实现上述多次迭代功能。

```cpp
// 上述示例repeat间隔为9，则通过如下配置可完成对应的操作
uint16_t mask = 128;
uint8_t repeatTime = 2;
uint8_t dstBlockStride = 1;
uint8_t src0BlockStride = 1;
uint8_t src1BlockStride = 1;
uint8_t dstRepeatStride = 9;
uint8_t src0RepeatStride = 9;
uint8_t src1RepeatStride = 9;

AscendC::Add(dstLocal, src0Local, src1Local, mask, repeatTime, { dstBlockStride, src0BlockStride, src1BlockStride, dstRepeatStride, src0RepeatStride, src1RepeatStride });
```

#### 掩码设置

针对同一个迭代中的数据，可以通过mask参数进行掩码操作来控制实际参与计算的个数。下图为进行Abs计算时通过mask逐比特模式按位控制哪些元素参与计算的示意图，1表示参与计算，0表示不参与计算。

![掩码操作示意图](../../../../figures/mask_op.png)

每一位掩码对应数据中的一个元素的位置，通过有效位和无效位标记，实现对数据操作的精细化开关控制。
掩码由固定位宽的数值表示：有效位（通常为1）：对应元素正常参与计算。无效位（通常为0）：对应元素被屏蔽，不执行操作。

掩码设置提供了两种模式：**Counter模式**和**Normal模式**。C++ Tensor矢量计算接口通过模板参数`isSetMask`控制掩码由开发者手动设置还是接口内部自动设置。以如下`Adds`接口为例，默认`isSetMask = true`表示由接口内部设置Counter或Normal模式；若开发者需自行控制两种模式的切换，则将`isSetMask`设为`false`。后续示例均以开发者自行控制切换为例。
```cpp
template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
__aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
```

Counter模式用于计算连续N个元素。该模式下，Mask寄存器仅低64bit有效，表示参与计算的元素数量；repeatTime参数被忽略，系统根据元素数量自动计算迭代次数。
例如需计算200个half类型元素时，此时使用Counter模式，可仅针对200个有效元素执行计算，忽略剩余56个元素的无效空间，避免多余计算。

![Count模式计算](../../../../figures/count_mode.png)

```cpp
half scalarValue = 1.0;

// 1. Direct transfer processing (temporarily omitted)
// 2. Use high-dimensional segmentation API for computation
// 2.1 Set mask counter mode
AscendC::SetMaskCount();
// 2.2 Set Mask value to 200, indicating this computation processes 200 elements
AscendC::SetVectorMask(0, static_cast<uint64_t>(200)); 

AscendC::Adds<half, false>(dstLocal, src0Local, scalarValue, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 8, 8 });

// 3. Default needs to reset to Normal mode, if subsequent still requires Counter mode then no need to reset to Normal mode
AscendC::SetMaskNormal();
```

Normal计算模式下，MASK寄存器作为每个Repeat的掩码使用，通过SetVectorMask设置mask值，控制单次迭代内参与计算的元素个数。该模式仅高维切分支持，区分为单次迭代内连续模式和单次迭代内逐比特模式。
> 📌 Normal为默认模式。若未发生模式切换，则无需重复配置该模式。

- 单次迭代内连续模式：表示前n个连续元素参与计算。取值范围和操作数的数据类型有关，数据类型不同，每次迭代内能够处理的元素个数最大值不同。

> **mask范围推导**：Vector计算单元单次迭代固定处理32B × 8 block = 256B数据，因此mask最大值 = 256B / sizeof(DataType)。当操作数为16位（sizeof=2B）时，mask∈[0, 128]（256/2=128）；当操作数为32位（sizeof=4B）时，mask∈[0, 64]（256/4=64）；当操作数为64位（sizeof=8B）时，mask∈[0, 32]（256/8=32）。此推导规则可推广至其他数据类型（如bfloat16=16bit→mask上限128，fp32=32bit→mask上限64）。

连续模式下，mask指定单次迭代内参与计算的前n个连续元素，超范围按对应数据类型mask最大值计算，mask为0视为NOP（空操作），负值产生未定义行为。例如对shape为256、数据类型为half的src数据（需2次迭代、每次128个元素），设置mask = 100时，第1次迭代计算0, 99、第2次迭代计算128, 227范围的元素，其余元素不参与计算。

![单次迭代内连续计算](../../../../figures/iter_cont.png)

```cpp
AscendC::LocalTensor<half> dstLocal;
AscendC::LocalTensor<half> src0Local;
half scalarValue = 1.0;

// 1. Data transfer (temporarily omitted)
// 2. Use high-dimensional segmentation API for computation
// 2.1 Set mask Normal mode
AscendC::SetMaskNormal();
// 2.2 Set Mask value to 100, indicating single iteration computes first 100 elements
AscendC::SetVectorMask(0, static_cast<uint64_t>(100)); 
AscendC::Adds<half, false>(dstLocal, src0Local, scalarValue, AscendC::MASK_PLACEHOLDER, 2, { 1, 1, 8, 8 });
```

- 单次迭代内逐比特模式：可以按位控制哪些元素参与计算，bit位的值为1表示参与计算，0表示不参与。分为maskHigh（高位mask）和maskLow（低位mask）。参数取值范围和操作数的数据类型有关，数据类型不同，每次迭代内能够处理的元素个数最大值不同。当操作数为16位时，maskLow、maskHigh∈[0, 2^64-1]，并且不同时为0；当操作数为32位时，maskHigh为0，maskLow∈(0, 2^64-1]；当操作数为64位时，maskHigh为0，maskLow∈(0, 2^32-1]。

  该模式下，可以指定maskHigh和maskLow的值，来处理各种掩码操作。如需进行交错计算，则mask设置成01010101...或者10101010...模式即可。以src数据（shape为256，数据类型为half）为例，通过设置逐bit的掩码，可在单迭代中选取特定bit位的数值参与计算：

  ![单次迭代内逐bit计算](../../../../figures/iter_bit.png)

  ```cpp
  AscendC::LocalTensor<half> dstLocal;
  AscendC::LocalTensor<half> src0Local;
  AscendC::LocalTensor<half> src1Local;

  // 1. Data transfer (temporarily omitted)
  // 2. Use high-dimensional segmentation API for computation
  // 2.1 Set mask Normal mode
  AscendC::SetMaskNormal();
  // 2.2 Set Mask to bit-wise mode (0101...), implement interleaved computation
  AscendC::SetVectorMask(0x000000000000AAAA, 0x0000000000000000); 
  AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 2, { 1, 1, 1, 8, 8, 8 });
  ```

### 连续计算

连续计算是C++ Tensor编程提供的简化计算接口，该能力由软件层面实现。开发者使用该接口无需配置mask掩码，连续计算针对源操作数的连续n个数据进行计算并连续写入目的操作数，解决一维数据的连续计算问题。

以Add接口为例，当开发者需要计算4096长度的向量加法，C++ Tensor编程通过如下形式调用：

```cpp
AscendC::Add(dstLocal, src0Local, src1Local, 4096);
```

## Memory矢量计算同步控制

AI Core内部执行单元（如MTE2搬运单元、Vector计算单元等）采用异步并行方式运行，当不同单元读写同一存储资源时，易产生数据依赖问题。因此，Memory矢量计算需通过流水同步接口协调执行顺序，确保计算流程正确。Memory矢量计算流程较Cube矩阵计算更为简洁，主要包含三步：数据搬入（Global Memory → UB）、计算（UB）、数据搬出（UB → Global Memory）。三个步骤分别对应PIPE_MTE2、PIPE_V、PIPE_MTE3流水线，需通过核内同步接口协调执行顺序，确保各步骤按序完成。

当开发者进行一个矢量加法计算时（[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品），可通过`Mutex::Lock`&`Mutex::Unlock`的方式完成对这三个步骤的同步。[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品可使用`SetFlag`&`WaitFlag`实现相同同步效果，详见[同步机制](../基于Tensor的CPP编程/CPPTensor编程概述.md#同步机制)。

```cpp
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, 48>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, 48>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, 48>();
    
    AscendC::GlobalTensor<float> xGlobal, yGlobal, zGlobal;
    xGlobal.SetGlobalBuffer((__gm__ float*)x);
    yGlobal.SetGlobalBuffer((__gm__ float*)y);
    zGlobal.SetGlobalBuffer((__gm__ float*)z);
    
    // ...
    // 1. Step 1: Data transfer in, execution pipeline is PIPE_MTE2
    AscendC::Mutex::Lock<PIPE_MTE2>(mutex_id);
    AscendC::DataCopy(xLocal, xGlobal, 48);
    AscendC::DataCopy(yLocal, yGlobal, 48);
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutex_id);

    // 2. Step 2: Compute, execution pipeline is PIPE_V
    AscendC::Mutex::Lock<PIPE_V>(mutex_id);
    AscendC::Add(zLocal, xLocal, yLocal, 48);
    AscendC::Mutex::Unlock<PIPE_V>(mutex_id);
    
    // 3. Step 3: Data transfer out, execution pipeline is PIPE_MTE3
    AscendC::Mutex::Lock<PIPE_MTE3>(mutex_id);
    AscendC::DataCopy(zGlobal, zLocal, 48);
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutex_id);
}
```

## Memory矢量计算示例

结合上文介绍的Memory矢量数据搬运与计算能力，开发者可基于C++ Tensor编程接口实现完整的Memory矢量计算算子。具体开发流程与代码示例可参考[cpp_api_add样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md)，该样例完整展示了矢量加法算子的开发、编译与验证流程。
