# SetLoopModePara<a name="ZH-CN_TOPIC_0000002414107833"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:54:44.531Z pushedAt=2026-10-28T03:14:39.510Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: not supported
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

During DataCopy and DataCopyPad, this API enables loop mode and sets the loop mode parameters. After data movement is complete, reset the loop mode parameters through [ResetLoopModePara](ResetLoopModePara.md).

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoopModePara(const LoopModeParams& loopParams, DataCopyMVType type)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| loopParams | Input | Loop mode parameters of the LoopModeParams type, defined as follows. For details about the parameters, see [Table 2](#table_setloop_2).<br>struct LoopModeParams {<br>        loop1Size = 0;<br>        loop2Size = 0;<br>        loop1SrcStride = 0;<br>        loop1DstStride = 0;<br>        loop2SrcStride = 0;<br>        loop2DstStride = 0 ;<br>}; |
| type | Input | Data movement mode. DataCopyMVType is an enumeration type, defined as follows. For details about the parameters, see [Table 3](#table1166074612214).<br>enum class DataCopyMVType : uint8_t {<br>    UB_TO_OUT = 0,<br>    OUT_TO_UB = 1,<br>}; |

**Table 2**  LoopModeParams structure parameter description<a name="table_setloop_2"></a>

| Parameter | Description |
| --- | --- |
| loop1Size | Used to set the number of iterations of the inner loop. The data type is uint32_t, and the value range is [0, 2^21). |
| loop2Size | Used to set the number of iterations of the outer loop. The data type is uint32_t, and the value range is [0, 2^21). |
| loop1SrcStride | Used to set the interval between data blocks of the source operand in adjacent iterations of the inner loop. The unit is byte, and the data type is uint64_t.<br>&bull;When the data movement mode is UB_TO_OUT, the value range is [0, 2^21), and loop1SrcStride must be 32-byte aligned.<br>&bull;When the data movement mode is OUT_TO_UB, the value range is [0, 2^40). |
| loop1DstStride | Used to set the interval between data blocks of the destination operand in adjacent iterations of the inner loop. The unit is byte, and the data type is uint64_t.<br>&bull;When the data movement mode is UB_TO_OUT, the value range is [0, 2^40).<br>&bull;When the data movement mode is OUT_TO_UB, the value range is [0, 2^21), and loop1DstStride must be 32-byte aligned. |
| loop2SrcStride | Used to set the interval between data blocks of the source operand in adjacent iterations of the outer loop. The unit is byte, and the data type is uint64_t.<br>&bull;When the data movement mode is UB_TO_OUT, the value range is [0, 2^21), and loop2SrcStride must be 32-byte aligned.<br>&bull;When the data movement mode is OUT_TO_UB, the value range is [0, 2^40). |
| loop2DstStride | Used to set the interval between data blocks of the destination operand in adjacent iterations of the outer loop. The unit is byte, and the data type is uint64_t.<br>&bull;When the data movement mode is UB_TO_OUT, the value range is [0, 2^40).<br>&bull;When the data movement mode is OUT_TO_UB, the value range is [0, 2^21), and loop2DstStride must be 32-byte aligned. |

**Table 3**  DataCopyMVType structure parameter description <a name="table1166074612214"></a>

| Parameter | Description |
| --- | --- |
| UB_TO_OUT | Path for moving data from UB to GM. |
| OUT_TO_UB | Path for moving data from GM to UB. |

The following sample shows how to use SetLoopModePara.

- In the sample, when the data type is int8\_t, the data block size is 384. DataCopyPad is configured with the Compact data movement mode, blockLen = 48, blockCount = 2, indicating that each contiguous transfer data block contains 48 bytes and there are two contiguous transfer data blocks. srcStride = 0, dstStride = 0, isPad = false, indicating that there is no gap between adjacent data blocks of the source operand and no user-defined data padding is required.
- Then set the LoopModeParams parameters in SetLoopModePara: loop1Size = 2, loop2Size = 2, loop1SrcStride = 96, loop2SrcStride = 192, loop1DstStride = 128, loop2DstStride = 288, and DataCopyMVType is OUT\_TO\_UB, indicating that the inner loop and outer loop each iterate twice, the data block gaps between adjacent iterations of the source operand in the inner loop and outer loop are 96 bytes and 192 bytes respectively, the data block gaps between adjacent iterations of the destination operand in the inner loop and outer loop are 128 bytes and 288 bytes respectively, and the path moves data from GM to Unified Buffer (UB).
- With the preceding configuration, call SetLoopModePara and then DataCopyPad to enable the loop mode of DataCopyPad and complete the data movement of a 384-byte data block with the data type int8\_t. The detailed diagram is as follows:

**Figure 1**  Source operand transfer scenario example<a name="fig6671114911311"></a>  
![](../../../../figures/source_operand_transfer_scenario.png "Source operand transfer scenario example")

**Figure 2**  Destination operand transfer scenario example<a name="fig226181311513"></a>  
![](../../../../figures/destination_operand_transfer_scenario.png "Destination operand transfer scenario example")

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The start addresses of the source operand and destination operand must be 32-byte aligned.
- The data of the destination operand must not overlap. If overlap occurs, the hardware does not report an error or alarm, and the correctness of the overlapped data cannot be guaranteed. However, different iterations can be interleaved. For example, the gap between data blocks of adjacent iterations of the destination operand in the inner loop can be smaller than the gap between data blocks of adjacent contiguous destination operands.
- After each time loop mode is enabled and its parameters are set, reset the register through ResetLoopModePara. Otherwise, the next data movement on the corresponding path will be affected, causing an exception.

## Examples<a name="section1227835243314"></a>

- SetLoopModePara enables loop mode, Compact mode

    ```cpp
    // Scenario 4: Use SetLoopModePara to enable loop mode for data movement (supported only on Ascend 950PR&950DT products).
    // Compact mode: Each inner loop moves 80B and then pads 16B to achieve 96-byte alignment.
    // Set the padding value to -1.
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- SetLoopModePara enables loop mode, Normal mode

    ```cpp
    // Scenario 5: Use SetLoopModePara to enable loop mode for data movement (supported only on Ascend 950PR&950DT products).
    // Normal mode: Each block moves 40B and then pads 24B to achieve 64-byte alignment.
    // Set the padding value to -1.
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- Five-dimensional data movement

    ```cpp
    // Scenario 6: Five-dimensional data movement GM[2, 4, 3, 128, 126]int8 -> UB[512, 128]int8.
    // Normal mode, using loop mode.
    // Movement specification: [2, 2, 2, 64, 126], with 2 bytes of padding added to each 126 bytes to reach 128 bytes.
    // The final UB is stored contiguously as [512, 128].

    // Stride parameter description:
    // - loop1SrcStride=128*126: Stride of dimension 2. Each loop1 iteration skips 128*126 bytes in the source address.
    // - loop1DstStride=64*128: Each loop1 iteration skips 64*128 bytes in the destination address (64 rows x 128 bytes/row).
    // - loop2SrcStride=3*128*126: Stride of dimension 1. Each loop2 iteration skips 3*128*126 bytes in the source address.
    // - loop2DstStride=2*64*128: Each loop2 iteration skips 2*64*128 bytes in the destination address.

    AscendC::LoopModeParams loopParam2Ub {
        2,  // loop1Size=2 (dimension 2 is moved twice)
        2,  // loop2Size=2 (move twice in dimension 1)
        128 * 126,  // loop1SrcStride (stride of dimension 2)
        64 * 128,   // loop1DstStride
        3 * 128 * 126,  // loop2SrcStride (stride of dimension 1)
        2 * 64 * 128    // loop2DstStride
    };
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);

    AscendC::DataCopyExtParams copyParams{
        64,  // blockCount=64 (move 64 times in dimension 3)
        126,  // blockLen=126 (move 126 bytes in dimension 4, padded to 128)
        0, 0, 0  // srcStride=0, dstStride=0, rsv=0
    };
    AscendC::DataCopyPadExtParams<int8_t> padParams{true, 0, 0, 0};

    // The dimension-0 for loop moves twice.
    for (uint32_t dim0 = 0; dim0 < 2; dim0++) {
        constexpr uint32_t dim0SrcStride = 4 * 3 * 128 * 126;  // source stride of dimension 0
        constexpr uint32_t dim0DstStride = 2 * 2 * 64 * 128;   // destination stride of dimension 0
        uint32_t srcOffset = dim0 * dim0SrcStride;
        uint32_t dstOffset = dim0 * dim0DstStride;

        AscendC::DataCopyPad<int8_t>(srcLocal[dstOffset], srcGlobal[srcOffset], copyParams, padParams);
    }

    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

For the complete sample, see [DataCopyPad sample scenarios 4, 5, and 6](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
