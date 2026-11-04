# Memory Vector Computation Programming<a name="ZH-CN_TOPIC_0000002600000004"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-28T08:55:14.623Z pushedAt=2026-11-03T07:08:03.559Z nl=2 -->

This document describes how to use the C++ Tensor programming API in Ascend C to write vector computation code based on the Unified Buffer (UB). This capability is the most basic and most general vector programming mode of Ascend C. This UB-memory-based programming mode is called Memory vector computation programming. Products with [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) support the complete Memory vector computation UB direct programming capability. Although [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) also supports UB operations, Reg vector computation programming is recommended for higher performance.

## Basic Steps of Memory Vector Programming

The data flow of Memory vector computation is: data loading → computation → data movement. To describe a common Vector vector computation, perform the following three steps:

1. Move data from Global Memory to Local Memory via DMA.
2. Perform vector computation.
3. Move data from Local Memory to Global Memory via DMA.

## Memory Vector Computation Memory Management

The core of Memory vector programming is to perform vector computation based on Local Memory (UB). Developers must first load input data from Global Memory (hereinafter referred to as GM) into UB, and then execute operations via Vector computation instructions. UB management relies on the LocalTensor object. Developers can allocate UB space via the LocalMemAllocator allocator to provide a storage carrier for subsequent vector computation.

### Basic Tensor (Without Layout) Definition

Created via the `LocalMemAllocator` memory allocator, which allows users to specify the physical storage location and memory size.

```cpp
__global__ __vector__ void add_custom(...)
{
    constexpr uint32_t ub_size = 256;
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<half> static_buffer = ubAllocator.Alloc<half, ub_size>();
}
```

## Memory Data Movement

Data movement between GM and UB provides diverse APIs that support a wide range of data movement scenarios. To achieve efficient vector computation, the system supports moving data from GM to UB for the vector compute unit to perform vector computation. After the computation is complete, the result is moved back from UB to GM, completing the data loop.

By flexibly configuring the DataCopyParams parameters, developers can precisely control key attributes such as the number of data blocks, address length, and intervals, implementing continuous and non-continuous data movement (high-dimensional partitioning data movement). This fully unleashes the hardware's data movement and computation collaboration capability, improving the execution efficiency of vector operators.

| Data Path | Mode | API | Application Scenario |
|---|---|---|---|
| GM→UB | Continuous movement | `DataCopy(dst, src, count)` | One-dimensional continuous data movement |
| GM→UB | High-dimensional partitioning | `DataCopy(dst, src, dataCopyParams)` | Multiple non-contiguous blocks, with configurable block count and intervals |
| GM→UB | Unaligned Movement | `DataCopyPad(dst, src, dataCopyExtParams, padParams)` | Address not 32B-aligned, supports custom padding |
| UB→GM | Continuous movement | `DataCopy(dst, src, count)` | One-dimensional continuous data movement |
| UB→GM | High-dimensional partitioning | `DataCopy(dst, src, dataCopyParams)` | Multiple non-contiguous blocks, with configurable block count and intervals |
| UB→GM | Unaligned Movement | `DataCopyPad(dst, src, dataCopyExtParams)` | Address not 32B-aligned |

Based on the description of the vector movement capabilities above, developers can select the appropriate API for different scenarios.

**Continuous movement**: If developers need to move a contiguous block of `Global Memory` to UB, they only need to specify the number of data elements to move. For example, srcGlobal data (with a shape of 256 and a data type of half) needs to be loaded into UB, which can be done by directly calling `DataCopy`:

![Continuous data movement](../../../../figures/cont_copy.png)

```cpp
AscendC::DataCopy(dstLocal, srcGlobal, 256);
```

**High-dimensional partitioning movement**: When multiple non-contiguous GM data blocks need to be loaded into a single UB, the high-dimensional partitioning mode can be used to configure the number of data blocks to move, the length of a single data block, and the intervals between blocks. Taking srcGlobal data (with a shape of 256 and a data type of half) as an example, the data movement in the following scenario can be implemented directly through the C++ Tensor API:

![Non-contiguous movement](../../../../figures/discont_copy.png)

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

**Unaligned movement**: High-dimensional partitioning data movement provides the capability for users to load multiple non-contiguous data blocks, but this API requires that both the length and intervals moved in GM be 32B-aligned. Therefore, it cannot support some unaligned scenarios. For example, if the movement shown in the following figure is required, the interleaved data movement cannot be completed through the `DataCopy` high-dimensional partitioning API, and the 32B constraint must be broken. Based on the chip capability, Ascend C provides the `DataCopyPad` API to support this movement form. This API can specify blockLen and stride information in bytes to complete the movement operation.

![Unaligned data loading capability](../../../../figures/unaligned_in_cap.png)

In the scenario shown in the preceding figure, two data blocks need to be moved from GM to UB: the blockLen of each data block is 54B, the interval between adjacent data blocks in the source operand is 1B, and the interval between adjacent data blocks in the destination operand is 32B. To meet the 32B alignment requirement, two half type elements need to be padded on the left side of blockLen and three half type elements on the right side. In this case, the total length is blockLen + leftPadding × 2B + rightPadding × 2B = 54B + 2 × 2B + 3 × 2B = 64B, which complies with the 32B alignment constraint.

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

> 📌 This API is mainly used to improve the ease of data movement for users. It is implemented at the software layer and encapsulated based on the high-dimensional partitioning API.

The preceding description covers data loading from GM to UB. Data unloading from UB to GM is processed using the same API.

For more movement APIs, see the API reference.

## Memory Data Computation

Memory vector computation provides two computation modes: continuous computation and high-dimensional partitioning. It also allows selective participation of data in computation via masks. Taking the Add API as an example, the following figure shows how the two computation modes are invoked. Continuous computation is simple and intuitive, and is suitable for processing continuous data of one-dimensional tensors. High-dimensional partitioning computation is flexible and controllable, and supports iterative execution and address interval configuration. Developers can select an appropriate computation mode based on actual service requirements.

![Memory vector computation mode diagram](../../../../figures/mem_vec.png)

### High-Dimensional Partitioning Computation

The high-dimensional partitioning capability mainly supports setting parameters such as `repeatTime`, `stride`, and `mask` for computation APIs, enabling precise control over the iterative execution of instructions, the address intervals of operands, and the mask within each iteration.

#### Iteration Control

In each iteration, the vector compute unit reads eight DataBlocks from the UB (each DataBlock has contiguous internal addresses and a length of 32 bytes) for computation, and writes the results to the corresponding eight DataBlocks.

![DataBlock iteration diagram](../../../../figures/db_iter.png)

The vector computation API supports controlling the number of instruction iterations by configuring the repeatTime parameter. If repeatTime is set to 2, the vector compute unit performs two iterations, and the amount of data that can be processed is 2 x 8 (eight DataBlocks per iteration) x 32 bytes = 512 bytes. If the data type is half (2 bytes per element), this corresponds to computing 256 elements. The following figure shows the diagram of an Exp computation with two iterations.
Note: Due to hardware limitations, the value range of repeatTime is [1, 255]. This constraint applies to all products that support Memory vector computation.

![Exp computation with two iterations](../../../../figures/iter2_exp.png)

#### Address Stride Configuration

The vector compute unit also supports vector computation with address strides, which can be precisely configured via the two parameters `dataBlockStride` and `repeatStride`:
- dataBlockStride: the address stride between different DataBlocks within a single iteration. Its value must not exceed the UB space size limit.
- repeatStride: the address stride of the same DataBlock between adjacent iterations (described in detail below).

For continuous computation, set `dataBlockStride` to 1 to process the 8 DataBlocks within the same iteration continuously.
For non-contiguous computation, set `dataBlockStride` to a value greater than 1 (for example, 2). In this case, a gap of one DataBlock appears between different DataBlocks within the same iteration when data is read, as shown in the following figure.

![dataBlockStride example](../../../../figures/db_stride.png)

> 📌 If dst_block_stride == 0, it is equivalent to dst_block_stride = 1. If src_block_stride = 0, the source operand always reuses the first dataBlock.

The following figure shows an example of reading and writing the source operand and destination operand in the UB space within a single iteration. In this example, `dataBlockStride` of the source operand is set to 2, indicating that the address stride between different DataBlocks within a single iteration is 2 DataBlocks. `dataBlockStride` of the destination operand is set to 1, indicating that the addresses are contiguous within a single iteration.

![Example of reading and writing source and destination operands within a single iteration](../../../../figures/iter_rw.png)

Here, **repeatStride** refers to the address stride of the same DataBlock between adjacent iterations. You can flexibly control different scenarios by setting `repeatStride`.

| Scenario | repeatStride Value | Data Read Characteristics | Application Scenario |
|------|------------------|-------------|--------|
| Continuous computation | 8 | Each iteration reads 8 consecutive dataBlocks to complete the computation of all input data | ![repeatStride continuous computation scenario](../../../../figures/rep_cont.png) |
| Non-contiguous computation | >8 (for example, 10) | There is a dataBlock gap between adjacent iterations, and the addresses are non-contiguous | ![repeatStride non-contiguous computation scenario](../../../../figures/rep_discont.png) |
| Repeated computation | 0 | The first 8 consecutive dataBlocks are read and computed repeatedly | ![repeatStride repeated computation scenario](../../../../figures/rep_repeat.png) |
| Partially repeated computation | (0, 8) | Part of the data is read repeatedly between adjacent iterations, which is generally not involved in common scenarios | ![repeatStride partially repeated computation](../../../../figures/rep_partial.png) |

When `repeatTime`>1, the computation is completed through multiple iterations. repeatStride indicates the start address interval (in dataBlocks) between DataBlocks at the same position in adjacent iterations. For example, when `repeatStride` = 9, the first DataBlock of the first iteration and the first DataBlock of the second iteration are 9 dataBlocks apart.

![Schematic diagram of the multi-iteration non-contiguous scenario](../../../../figures/multi_iter.png)

Configure `repeatStride` and `dataBlockStride` through the C++ Tensor API to implement the preceding multi-iteration functionality.

```cpp
// In the preceding example, the repeat interval is 9. The corresponding operation can be completed through the following configuration.
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

#### Mask Setting

For data within the same iteration, you can use the mask parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows the schematic diagram of controlling which elements participate in the computation bit by bit using the mask bit-by-bit mode during Abs computation, where 1 indicates participation in the computation and 0 indicates non-participation.

![Schematic diagram of mask operation](../../../../figures/mask_op.png)

Each mask bit corresponds to the position of one element in the data. Valid bits and invalid bits are used to mark elements, implementing fine-grained on/off control over data operations.
The mask is represented by a fixed-width value: valid bits (usually 1) indicate that the corresponding elements participate in the computation normally. Invalid bits (usually 0) indicate that the corresponding elements are masked and no operation is performed.

Mask setting provides two modes: **Counter mode** and **Normal mode**. The C++ Tensor vector computation APIs use the template parameter `isSetMask` to control whether the mask is set manually by the developer or automatically inside the API. Taking the following `Adds` API as an example, the default `isSetMask = true` indicates that the Counter or Normal mode is set inside the API. If the developer needs to control the switching between the two modes, set `isSetMask` to `false`. The subsequent examples all use the developer-controlled switching as an example.
```cpp
template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
__aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
```

Counter mode is used to compute N consecutive elements. In this mode, only the lower 64 bits of the Mask register are valid, indicating the number of elements that participate in the computation. The repeatTime parameter is ignored, and the system automatically calculates the number of iterations based on the number of elements.
For example, when 200 half-type elements need to be computed, using Counter mode allows computation to be performed only on the 200 valid elements, ignoring the invalid space of the remaining 56 elements and avoiding redundant computation.

![Count mode computation](../../../../figures/count_mode.png)

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

In Normal computation mode, the MASK register is used as the mask for each Repeat. The mask value is set via SetVectorMask to control the number of elements that participate in the computation within a single iteration. This mode is supported only by high-dimensional partitioning, and is divided into the continuous mode within a single iteration and the bit-by-bit mode within a single iteration.
> 📌 Normal is the default mode. If no mode switch occurs, there is no need to configure this mode repeatedly.

- Continuous mode within a single iteration: indicates that the first n consecutive elements participate in the computation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration.

> **Mask range derivation**: The Vector compute unit processes a fixed 32B x 8 block = 256B of data in a single iteration. Therefore, the maximum mask value = 256B / sizeof(DataType). When the operand is 16-bit (sizeof = 2B), mask is in [0, 128] (256/2 = 128). When the operand is 32-bit (sizeof = 4B), mask is in [0, 64] (256/4 = 64). When the operand is 64-bit (sizeof = 8B), mask is in [0, 32] (256/8 = 32). This derivation rule can be generalized to other data types (for example, bfloat16 = 16 bit -> mask upper limit 128, fp32 = 32 bit -> mask upper limit 64).

In continuous mode, mask specifies the first n consecutive elements that participate in computation within a single iteration. Values beyond the range are calculated based on the maximum mask value of the corresponding data type. A mask of 0 is treated as a NOP (no operation), and a negative value produces undefined behavior. For example, for src data with a shape of 256 and a data type of half (which requires 2 iterations, 128 elements each), when mask = 100 is set, the first iteration computes elements in the range [0, 99], the second iteration computes elements in the range [128, 227], and the remaining elements do not participate in computation.

![Continuous computation within a single iteration](../../../../figures/iter_cont.png)

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

- Bit-by-bit mode within a single iteration: You can control which elements participate in computation on a per-bit basis. A bit value of 1 indicates participation in computation, and 0 indicates non-participation. It is divided into maskHigh (high-order mask) and maskLow (low-order mask). The parameter value range is related to the data type of the operand. Different data types allow different maximum numbers of elements to be processed within each iteration. When the operand is 16-bit, maskLow and maskHigh are in [0, 2^64-1] and cannot both be 0. When the operand is 32-bit, maskHigh is 0 and maskLow is in (0, 2^64-1]. When the operand is 64-bit, maskHigh is 0 and maskLow is in (0, 2^32-1].

  In this mode, you can specify the values of maskHigh and maskLow to handle various mask operations. For interleaved computation, set mask to a pattern such as 01010101... or 10101010.... Taking src data (with a shape of 256 and a data type of half) as an example, by setting a bit-by-bit mask, you can select the values of specific bits to participate in computation within a single iteration:

  ![Bit-by-bit computation within a single iteration](../../../../figures/iter_bit.png)

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

### Continuous Computation

Continuous computation is a simplified computation API provided by C++ Tensor programming, and this capability is implemented at the software level. When using this API, developers do not need to configure the mask. Continuous computation performs computation on n consecutive data elements of the source operand and writes the results consecutively to the destination operand, solving the continuous computation problem of one-dimensional data.

Taking the Add API as an example, when developers need to compute vector addition of length 4096, C++ Tensor programming invokes it in the following form:

```cpp
AscendC::Add(dstLocal, src0Local, src1Local, 4096);
```

## Memory Vector Computation Synchronization Control

The execution units inside the AI Core (such as the MTE2 movement unit and the Vector compute unit) run in an asynchronous parallel manner. When different units read from and write to the same storage resource, data dependency issues are likely to occur. Therefore, Memory vector computation must coordinate the execution order via pipeline synchronization APIs to ensure a correct computation process. The Memory vector computation process is simpler than the  Cube computation process and mainly consists of three steps: data loading (Global Memory → UB), computation (UB), and data storing (UB → Global Memory). The three steps correspond to the PIPE_MTE2, PIPE_V, and PIPE_MTE3 pipelines, respectively, and their execution order must be coordinated via intra-core synchronization APIs to ensure that each step is completed in sequence.

When a developer performs a vector addition computation (on [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) products), the three steps can be synchronized via `Mutex::Lock` and `Mutex::Unlock`. On [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) products, `SetFlag` and `WaitFlag` can be used to achieve the same synchronization effect. For details, see [Synchronization Mechanism](../cpp_tensor_programming/cpp_tensor_programming_overview.md#synchronization-mechanism).

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

## Memory Vector Computation Example

Based on the Memory vector data movement and computation capabilities described above, developers can implement a complete Memory vector computation operator using the C++ Tensor programming API. For the specific development process and code example, see the [cpp_api_add sample](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md), which fully demonstrates the development, compilation, and verification process of a vector addition operator.
