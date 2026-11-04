# BilinearInterpolation\(ISASI\)<a name="ZH-CN_TOPIC_0000001786742102"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:08:52.640Z pushedAt=2026-10-28T03:14:39.363Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/BilinearInterpolation_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_bilinearinterpolation_intf.h"`.

BilinearInterpolation is a binary vector computation API. Its core performs multiply-accumulate operations through a two-level loop of horizontal iteration and vertical iteration, and is applicable to vector operation scenarios with irregular data access and weighted multiply-add (data Gather, Multiply, and Reduce).

Note: This API is a software-emulated API added for compatibility with earlier products.

The computation process is shown in the following figure:

**Figure 1** BilinearInterpolation computation process

![](../../../../figures/bilinearInterpolation_1.png)

![](../../../../figures/bilinearInterpolation_2.png)

Based on the computation process shown above, the following example describes the entire BilinearInterpolation computation process.

Assume the inputs of the computation scenario are as follows:

```text
src0 = [1, 2, 3, ..., 512], a total of 512 half data elements;
src0Offset = [0, 32, 64, ..., 992], a total of 32 uint32 data items;
src1 = [2, 3, 4, ..., 33], a total of 32 half data items;
hRepeat = 2; vRepeat = 2; mask = 128; vROffset = 128.
```

The detailed computation process is broken down as follows:

- Step 1. Collect the input src0 tensor into a temporary buffer at the DataBlock granularity.

    There are hRepeat × vRepeat iterations in total. In each iteration, 8 offset values are read from the input src0Offset, and each offset value points to the starting address of a DataBlock in src0.

    Call the [Gatherb](../scatter_gather/Gatherb_ISASI.md) API. The Gatherb API accepts the input tensor (src0) and the DataBlock offset tensor (src0Offset), and collects the input tensor src0 into the temporary tensor (sharedTmpBuffer) by DataBlock (32 bytes) according to src0Offset.

    This step is processed as follows:

    - Total iteration count: hRepeat × vRepeat = 2 × 2 = 4;
    - Number of src0 elements processed in each iteration: 8 × 32 / × = 128 (8: number of DataBlocks; 32: size of each DataBlock is 32B);
    - Collection result of each iteration
        - Iteration 0: sharedTmpBuffer\[0:127\] = \[1, 2, ..., 128\];
        - Iteration 1: sharedTmpBuffer\[128:255\] = \[129, 130, ..., 256\];
        - Iteration 2: sharedTmpBuffer\[256:383\] = \[257, 258, ..., 384\];
        - Iteration 3: sharedTmpBuffer\[384:511\] = \[385, 386, ..., 512\].

- Step 2. Broadcast and pad the input tensor src1.

    Call the [Brcb](../data_padding/Brcb.md) API to take 8 numbers from the input tensor src1 in each iteration and pad them into the 8 DataBlocks of the result tensor sharedTmpBuffer (each DataBlock is 32B in size).

    This step is processed as follows:

    - The space occupied by the src0 tensor collected in step 1 = total iteration count \* number of elements processed in each iteration = 4 \* 128 = 512. Therefore, the broadcast src1 elements are stored starting from sharedTmpBuffer\[512\];
    - The Brcb instruction iteration count repeatTime = data length of src1 / 8 = 32 / 8 = 4 (the Brcb instruction takes 8 numbers in each iteration);
    - Fill the 8 data elements in the input src1 tensor into the 8 DataBlocks starting from the sharedTmpBuffer\[512\] position. The input is of the half data type, so each element is replicated 32 / sizeof\(half\) = 16 times.

        Result: sharedTmpBuffer\[512:1024\] = \[2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, ..., 33, 33\].

- Step 3. For the gathered src0 tensor and the broadcast src1 tensor, call [Mul](Mul.md) to perform element-wise multiplication, with the result overwriting sharedTmpBuffer.

    Iteration count: hRepeat \* vRepeat = 2 \* 2 = 4.

    Two computation modes are supported, controlled by the input parameter repeatMode (configured as true or false).

    - repeatMode=false
        In each iteration, src0 (data storage position is sharedTmpBuffer\[0:511\]) reads 8 DataBlocks, and each value of each DataBlock is multiplied by a single value of src1 (data storage position is sharedTmpBuffer\[512:1024\]).

        - The entire process is as follows:
            - Iteration 0 computation process: the 1st DataBlock (sharedTmpBuffer\[0:15\]) to the 8th DataBlock (sharedTmpBuffer\[112:127\]), all with multiplied by 2 of src1.

                That is, sharedTmpBuffer\[0:127\] = \[1 \* 2, ..., 128 \* 2\];

            - Iteration 1 computation process: The 1st DataBlock (sharedTmpBuffer\[128:143\]) through the 8th DataBlock (sharedTmpBuffer\[240:255\]) are all multiplied by 3 of src1.

                That is, sharedTmpBuffer\[128:255\] = \[129 \* 3, ..., 256 \* 3\];

            - Iteration 2 computation process: The 1st DataBlock (sharedTmpBuffer\[256:271\]) through the 8th DataBlock (sharedTmpBuffer\[368:383\]) are all multiplied by 4 of src1.

                That is, sharedTmpBuffer\[256:383\] = \[257 \* 4, ..., 384 \* 4\];

            - Iteration 3 computation process: The 1st DataBlock (sharedTmpBuffer\[384:399\]) through the 8th DataBlock (sharedTmpBuffer\[496:511\]) are all multiplied by 5 of src1.

                That is, sharedTmpBuffer\[384:511\] = \[385 \* 5, ..., 512 \* 5\].

    - repeatMode=true
        In each iteration, src0 (data storage position is sharedTmpBuffer\[0:511\]) reads 8 DataBlocks, and each DataBlock is multiplied by one value of src1 (storage position is sharedTmpBuffer\[512:1024\]). That is, the 8 DataBlocks of src0 correspond to the 8 values of src1 respectively.

        - The entire processing is as follows:
            - Iteration 0 computation process: The 1st DataBlock (sharedTmpBuffer\[0:15\]) is multiplied by 2 of src1, the 2nd DataBlock (sharedTmpBuffer\[16:31\]) is multiplied by 3 of src1; ... and so on, the 8th DataBlock (sharedTmpBuffer\[112:127\]) is multiplied by 9 of src1.

                That is, sharedTmpBuffer\[0:127\] = \[1 \* 2, ..., 16 \* 2, 17 \* 3, ..., 128 \* 9\];

            - Iteration 1 computation process: the first DataBlock (sharedTmpBuffer\[128:143\]) is multiplied by 10 of src1, the second DataBlock (sharedTmpBuffer\[144:159\]) is multiplied by 11 of src1; ... and so on, the eighth DataBlock (sharedTmpBuffer\[240:255\]) is multiplied by 17 of src1.

                That is, sharedTmpBuffer\[128:255\] = \[129 \* 10, ..., 144 \* 10, 145 \* 11, ..., 256 \* 17\];

            - Iteration 2 computation process: the first DataBlock (sharedTmpBuffer\[256:271\]) is multiplied by 18 of src1, the second DataBlock (sharedTmpBuffer\[272:287\]) is multiplied by 19 of src1; ... and so on, the eighth DataBlock (sharedTmpBuffer\[368:383\]) is multiplied by 25 of src1.

                That is, sharedTmpBuffer\[256:383\] = \[257 \* 18, ..., 272 \* 18, 273 \* 19, ..., 384 \* 25\];

            - Iteration 3 computation process: the first DataBlock (sharedTmpBuffer\[384:399\]) is multiplied by 26 of src1, the second DataBlock (sharedTmpBuffer\[400:415\]) is multiplied by 27 of src1; ... and so on, the eighth DataBlock (sharedTmpBuffer\[496:511\]) is multiplied by 33 of src1.

                That is, sharedTmpBuffer\[384:511\] = \[385 \* 26, ..., 400 \* 26, 401 \* 27, ..., 512 \* 33\].

- Step 4. Perform horizontal iterative accumulation and output the result to dst.

    Call the [Add](Add.md) API to complete the following processing:

    - dst\[0:127\] = sharedTmpBuffer\[0:127\] + sharedTmpBuffer\[128:255\];
    - dst\[128:255\] = sharedTmpBuffer\[256:383\] + sharedTmpBuffer\[384:511\].

## Prototype<a name="zh-cn_topic_0000002560891889_section82039854412"></a>

- mask bit-by-bit mode:

    ```cpp
    template <typename T>
    __aicore__ inline void BilinearInterpolation(const LocalTensor<T> &dst, const LocalTensor<T> &src0,
        const LocalTensor<uint32_t> &src0Offset, const LocalTensor<T> &src1, uint64_t mask[], uint8_t hRepeat,
        bool repeatMode, uint16_t dstBlkStride, uint16_t vROffset, uint8_t vRepeat,
        const LocalTensor<uint8_t> &sharedTmpBuffer);
    ```

- mask continuous mode:

    ```cpp
    template <typename T>
    __aicore__ inline void BilinearInterpolation(const LocalTensor<T> &dst, const LocalTensor<T> &src0,
        const LocalTensor<uint32_t> &src0Offset, const LocalTensor<T> &src1, uint64_t mask, uint8_t hRepeat,
        bool repeatMode, uint16_t dstBlkStride, uint16_t vROffset, uint8_t vRepeat,
        const LocalTensor<uint8_t> &sharedTmpBuffer);
    ```

## Parameters<a name="section622mcpsimp"></a>

### Template Parameters and API Parameters

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operands.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>The data type of the two source operands must be consistent with that of the destination operand. |
| src0Offset | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| mask[]/mask | Input | mask controls the elements that participate in the computation within each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| hRepeat | Input | Number of horizontal iterations, with a value range of [1, 255]. |
| repeatMode | Input | Iteration mode:<br>&bull; false: In each iteration, each of the 8 DataBlocks read from src0 is multiplied by a single value from src1.<br>&bull; true: In each iteration, each DataBlock of src0 is multiplied by one value from src1, that is, one iteration consumes 8 DataBlocks from src0 and 8 elements from src1. |
| dstBlkStride | Input | Address stride between different DataBlocks of the destination operand within a single iteration, in units of 32B. |
| vROffset | Input | Address offset of the destination operand between vertical iterations, in units of elements, with a value range of [128, 65535). vROffset * sizeof(T) must ensure 32-byte alignment. |
| vRepeat | Input | Number of vertical iterations, with a value range of [1, 255]. |
| sharedTmpBuffer | Input | Temporary space. For details about the space, see [sharedTmpBuffer Space Description](#sharedtmpbuffer-space-description) below. |

### sharedTmpBuffer space description

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, no temporary space needs to be allocated.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 products, you must ensure that at least src0.GetSize() * 32 + src1.GetSize() * 32 bytes of space are allocated.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 products, you must ensure that at least src0.GetSize() * 32 + src1.GetSize() * 32 bytes of space are allocated.
<!-- end id12 -->
<!-- npu="310p" id13 -->
- For Atlas inference products AI Core, you must ensure that at least src0OffsetLocal.GetSize() * sizeof(uint32_t) bytes of space are allocated.
<!-- end id13 -->

## Data Types

The supported data type is half.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- Address overlap is not allowed between src0, src1, and src0Offset, and address overlap is not allowed between the destination addresses of two vertical repeats.
- The value range of vROffset is \[128, 65535\).
- The horizontal iteration count has a value range of \[1, 255\].
- The vertical iteration count has a value range of \[1, 255\].
- For the input temporary space sharedTmpBuffer, see [sharedTmpBuffer space description](#sharedtmpbuffer-space-description) for the space size requirement.
- The value requirements for src0Offset are as follows:
    - The value must ensure bit-width alignment of the src0 element type.
    - The offset address must be 32-byte aligned.
    - The offset address must not exceed the range of the Unified Buffer (UB) size data.
    - Value range of the address offset: must not exceed the range of uint32\_t.
- When the repeatMode parameter is set to true, ensure that the valid data length of src1 * 16 ≥ the data length of src0.

## Example<a name="section642mcpsimp"></a>

- Example - mask continuous mode

    ```cpp
    AscendC::LocalTensor<half> dstLocal, src0Local, src1Local; // repeatMode = false. The data length of src1Local can be 16.
    AscendC::LocalTensor<uint32_t> src0OffsetLocal;
    AscendC::LocalTensor<uint8_t> tmpLocal;
    uint64_t mask = 128;        // mask continuous mode
    uint8_t hRepeat = 2;        // Iterate twice horizontally.
    bool repeatMode = false;    // Iteration mode
    uint16_t dstBlkStride = 1;  // Data is written continuously within a single iteration.
    uint16_t vROffset = 128;    // Data is written continuously between adjacent iterations.
    uint8_t vRepeat = 2;        // Vertical iteration, 2 times.
    AscendC::BilinearInterpolation(dstLocal, src0Local, src0OffsetLocal, src1Local, mask, hRepeat, repeatMode,
                dstBlkStride, vROffset, vRepeat, tmpLocal);
    ```

- API example - mask per-bit mode

    ```cpp
    AscendC::LocalTensor<half> dstLocal, src0Local, src1Local;
    AscendC::LocalTensor<uint32_t> src0OffsetLocal;
    AscendC::LocalTensor<uint8_t> tmpLocal;
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX }; // mask per-bit mode
    uint8_t hRepeat = 2;        // Horizontal iteration, 2 times.
    bool repeatMode = false;    // Iteration mode.
    uint16_t dstBlkStride = 1;  // Data is written continuously within a single iteration.
    uint16_t vROffset = 128;    // Data is written continuously between adjacent iterations.
    uint8_t vRepeat = 2;        // Vertical iteration, 2 times.
    AscendC::BilinearInterpolation(dstLocal, src0Local, src0OffsetLocal, src1Local, mask, hRepeat, repeatMode,
                dstBlkStride, vROffset, vRepeat, tmpLocal);
    ```

The result is as follows:

```text
Input data (src0Local, half): [1,2,3,...,512]
Input data (src1Local, half): [2,3,4,...,17]
Input data(src0OffsetLocal,uint32_t): [0,32,64,...,992]
Output data(dstLocal,half): [389, 394, 399, 404, ...,4096]
```
