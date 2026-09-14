# Transpose

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T16:48:45.271Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs data layout and reshape operations on the input data. The specific functions are as follows:

[Scenario 1: NZ2ND, axis 1 and axis 2 swap]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, N, H/N\], origin\_shape:\[B, S, N, H/N\], format:"ND", origin\_format:"ND"\}

**Figure 1**  Scenario 1 - data layout transformation
![](../../../figures/scenario1_data_layout_transformation.png "Scenario 1 - data layout transformation") 

[Scenario 2: NZ2NZ, axis 1 and axis 2 swap]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, H/N/16, N/16, 16, 16\], origin\_shape:\[B, S, N, H/N\], format:"NZ", origin\_format:"ND"\}

**Figure 2**  Scenario 2 - data layout transformation  
![](../../../figures/scenario2_data_layout_transformation.png "Scenario 2 - data layout transformation")

[Scenario 3: NZ2NZ, last-axis tiled]

Input tensor \{ shape:\[B, H / 16, S / 16, 16, 16\], origin\_shape:\[B, S, H\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, N, H/N/16, S / 16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

**Figure 3**  Scenario 3 - data layout transformation  
![](../../../figures/scenario3_data_layout_transformation.png "Scenario 3 - data layout transformation")

[Scenario 4: NZ2ND, last-axis tiled]

Input tensor \{ shape:\[B, H / 16, S / 16, 16, 16\], origin\_shape:\[B, S, H\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, N, S, H/N\], origin\_shape:\[B, N, S, H/N\], format:"ND", origin\_format:"ND"\}

**Figure 4**  Scenario 4 - data layout transformation 
![](../../../figures/scenario4_data_layout_transformation.png "Scenario 4 - data layout transformation")

[Scenario 5: NZ2ND, last-axis merge]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, H\], origin\_shape:\[B, S, H\], format:"ND", origin\_format:"ND"\}

**Figure 5**  Scenario 5 - data layout transformation  
![](../../../figures/scenario5_data_layout_transformation.png "Scenario 5 - data layout transformation")

[Scenario 6: NZ2NZ, last-axis merge]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, H/16, S/16, 16, 16\], origin\_shape:\[B, S, H\], format:"NZ", origin\_format:"ND"\}

**Figure 6**  Scenario 6 - data layout transformation  
![](../../../figures/scenario6_data_layout_transformation.png "Scenario 6 - data layout transformation")

[Scenario 7: 2D transpose]

Supports transposing a 2D tensor on the UB, where H and W in srcShape are both integer multiples of 16.

**Figure 7**  Scenario 7 - data layout transformation  
![](../../../figures/scenario7_data_layout_transformation.png "Scenario 7 - data layout transformation")

[Scenario 13: 2D or 3D transpose of the last two dimensions]

Supports transposing a 2D tensor or the last two dimensions of a 3D tensor in UB. The 2D tensor transpose is the same as the data layout transformation in Scenario 7.

**Figure 8**  Scenario 13 - 3D tensor data layout transformation  
![](../../../figures/scenario13_3D_tensor_data_layout_transformation.png "Scenario 13: 3D tensor data layout transformation")

[Scenario 14: Swap the first and second dimensions of a 3D tensor]

Supports swapping the first and second dimensions of a 3D tensor in UB.

**Figure 9**  Scenario 14 - 3D tensor data layout transformation  
![](../../../figures/scenario14_3D_tensor_data_layout_transformation.png "Scenario 14 - 3D tensor data layout transformation")

[Scenario 15: Swap the first and third dimensions of a 3D tensor]

Supports swapping the first and third dimensions of a 3D tensor in UB.

**Figure 10**  Scenario 15 - 3D tensor data layout transformation  
![](../../../figures/scenario15_3D_tensor_data_layout_transformation.png "Scenario 15 - 3D tensor data layout transformation")

[Scenario 16: Use the interleave instructions for 2D ND2NZ transpose]

Supports using the [interleave instructions](../../basic_api/memory_vector_compute/data_rearrange_ISASI/Interleave.md) in UB to transpose a 2D ND tensor to NZ.

**Figure 11**  Scenario 16 - ND2NZ transpose using interleave instructions
![](../../../figures/scenario16_ND2NZ_transpose_using_interleave_instructions.png "Scenario 16 - ND2NZ transpose using interleave instructions")

## Implementation Principle

The algorithm block diagram for each of the 11 functional scenarios corresponding to Transpose is shown in the following figures.

**Figure 12**  Scenario 1 - NZ2ND, axis 1 and axis 2 swapped  
![](../../../figures/scenario1_NZ2ND_axis1_and_axis2_swap.png "Scenario 1 - NZ2ND, axis 1 and axis 2 swapped")

The computation process consists of the following steps:

Process sequentially along the H/N direction, the N direction, and the B direction in loops:

1.  First TransDataTo5HD step: Transpose S/16 consecutive 16\*16 squares along the S direction into temp, where each square is stored contiguously with the next in temp;
2.  Second TransDataTo5HD step: Transpose the S/16 16\*16 squares in temp into dst. In dst, the data is in ND format, where two consecutive data rows from the same square have an address offset of \(H/N\)\*N elements on the destination operand, and the same data row of every two squares along the H direction has an address offset of 16 elements on the destination operand.

**Figure 13**  Scenario 2 - NZ2NZ, axis 1 and axis 2 swapped  
![](../../../figures/scenario2_NZ2NZ_axis1_and_axis2_swap.png "Scenario 2 - NZ2NZ, axis 1 and axis 2 swapped")

The computation process consists of the following steps:

Process sequentially along the H/N direction, the N direction, and the B direction in loops:

1. First TransDataTo5HD step: Take S/16 consecutive 16\*16 squares along the S direction into temp, where the squares are stored contiguously one after another in temp;
2. Second TransDataTo5HD step: Transpose the S/16 16\*16 squares in temp into dst, which is in NZ format. The address offset of two consecutive rows of data from the same square on the destination operand is \(H/N\)\*N elements, and the address offset of the same row of data from every two squares along the H direction on the destination operand is N\*16 elements.

**Figure 14**  Scenario 3 - NZ2NZ, last-axis tiled  
![](../../../figures/scenario3_NZ2NZ_last_axis_tiled.png "Scenario 3 - NZ2NZ, last-axis tiled")

The computation process consists of the following steps:

Process in a loop along the H direction and then the B direction:

1. First TransDataTo5HD step: Transpose S/16 consecutive 16\*16 squares into temp1 each time;
2. DataCopy step: When H/N<=16, copy H/N\*S elements into temp2 each time; when H/N\>16, copy 16\*S elements into temp2 for the first H/N/16 times, and copy H/N%16\*S elements into temp2 for the last time;
3. Second TransDataTo5HD step: Transpose the 16\*S square in temp2 into dst, which is in NZ format. The address offset of two consecutive rows of data from the same square on the destination operand is 16 elements, and the address offset of the same row of data from every two squares along the H direction on the destination operand is S\*16 elements.

**Figure 15**  Scenario 4 - NZ2ND, last-axis tiled  
![](../../../figures/scenario4_NZ2ND_last_axis_tiled.png "Scenario 4 - NZ2ND, last-axis tiled")

The computation process consists of the following steps:

Process in a loop along the H direction first, then the B direction:

1. First **TransDataTo5HD** step: Transpose S/16 consecutive 16\*16 squares to **temp1** each time;
2. **DataCopy** step: When H/N <= 16, copy H/N\*S elements to **temp2** each time; when H/N\>16, copy 16\*S elements to **temp2** in each of the first H/N/16 times, and copy H/N%16\*S elements to **temp2** the last time;
3. Second **TransDataTo5HD** step: Transpose the data in **temp2** to **dst**, where the data is in ND format. The address offset of two consecutive rows of data from the same square in the destination operand is \(H/N+16-1\)/16\*16 elements, and the address offset of the same row of data from every two squares along the H direction in the destination operand is \(H/N+16-1\)/16\*16\*S elements.

**Figure 16**  Scenario 5 - NZ2ND, last-axis merge  
![](../../../figures/scenario5_NZ2ND_last_axis_merge.png "Scenario 5 - NZ2ND, last-axis merge")

The computation process consists of the following steps:

Process in a loop along the H direction first, then the B direction:

1. First **TransDataTo5HD** step: Transpose one S\*16 square to **temp1** each time;
2. **DataCopy** step: When H/N <= 16, copy H/N\*S elements to **temp2** each time; when H/N\>16, copy 16\*S elements to **temp2** in each of the first H/N/16 times, and copy H/N%16\*S elements to **temp2** the last time;
3. Second **TransDataTo5HD** step: Transpose the 16\*S square in **temp2** to **dst**, where it is in ND format. The address offset of two consecutive rows of data from the same square in the destination operand is \(H+16-1\)/16\*16 elements, and the address offset of the same row of data from every two squares along the H direction in the destination operand is H/N\*S elements.

**Figure 17**  Scenario 6 - NZ2NZ, last-axis merge  
![](../../../figures/scenario6_NZ2NZ_last_axis_merge.png "Scenario 6 - NZ2NZ, last-axis merge")

The computation process consists of the following steps:

Loop along the H direction and then the B direction:

1.  Step 1, first TransDataTo5HD: Transpose one S\*16 square to temp1 each time.
2.  DataCopy step: When H/N<=16, move H/N\*S elements to temp2 each time; when H/N\>16, move 16\*S elements to temp2 in each of the first H/N/16 times, and move H/N%16\*S elements to temp2 the last time.
3.  Step 2, second TransDataTo5HD: Transpose the 16\*S square in temp2 to dst. In dst, the data is in NZ format. The address offset of two consecutive rows from the same square in the destination operand is 16 elements, and the address offset of the same row of every two squares along the H direction in the destination operand is S\*16 elements.

**Figure 18**  Scenario 7 - 2D transpose  
![](../../../figures/scenario7_2D_transpose.png "Scenario 7 - 2D transpose")

The computation process is as follows:

1.  Call TransDataTo5HD to transpose \[H, W\] to \[W, H\] by setting different source operand address sequences and destination operand address sequences. Both src and dst are in ND format.

**Figure 19**  Scenario 13 - 2D or 3D transpose of the last two dimensions  
![](../../../figures/scenario13_2Dtranspose_or_3Dtranspose_last_2D.png "Scenario 13 - 2D or 3D transpose of the last two dimensions")

The computation process is as follows:

1.  Invoke the internal computation logic to transpose \[H, W\] to \[W, H\], or transpose \[N, H, W\] to \[N, W, H\], by setting different source operand address sequences and writing them continuously to the destination operand addresses. Both src and dst are in ND format.

The transpose process of Scenario 14 and Scenario 15 is basically the same as the 3D transpose process in Scenario 13 described above, except that the specified transpose dimensions differ.

**Figure 20**  Scenario 16 - Using interleave instructions to perform 2D ND2NZ transpose  
![](../../../figures/scenario16_2D_ND2NZ_transpose_using_interleave_instructions.png "Scenario 16 - Using interleave instructions to perform 2D ND2NZ transpose")

The computation process is as follows:

1.  Invoke the internal computation logic to transpose \[H, W\] in ND format to \[W1,H1,H0,W0\] in NZ format, where H = H1 \* H0, W = W1 \* W0, H0=16, W0=2, by setting different source operand address sequences and continuously writing them to the destination operand addresses. src is in ND format, and dst is in NZ format.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. To obtain the temporary space size **BufferSize**: use the **GetTransposeMaxMinTmpSize** API provided in [Transpose Tiling](Transpose-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

Temporary space supports two methods: **application by the API framework** and **passing by you through the sharedTmpBuffer input parameter**. Therefore, the **Transpose** API has two prototypes:

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T>
    __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t> &sharedTmpBuffer, TransposeType transposeType, ConfusionTransposeTiling& tiling)
    ```

    In this method, you must apply for and manage the temporary memory space, and reuse this memory after the API call is completed. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.

-   The API framework applies for temporary space.

    ```
    template <typename T>
    __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src, TransposeType transposeType, ConfusionTransposeTiling& tiling)
    ```

    In this method, you do not need to apply for it, but must reserve the size of the temporary space.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand.<br><br>Scenarios 1 to 7 support the following data types: int16_t, uint16_t, half, int32_t, uint32_t, and float. Scenarios 13 to 15 support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float. Scenario 16 supports the following data types: int8_t and uint8_t. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src | Input | Source operand. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during the internal computation of the API. With this approach, you can manage the memory space of sharedTmpBuffer by yourself and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, which provides high flexibility and high memory utilization. For how to obtain the shared buffer size, see [Transpose Tiling](Transpose-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| transposeType | Input | Type of the data layout and reshape. The type is the TransposeType enumeration class. The definition of TransposeType is shown in the following code. |
| tiling | Input | Tiling information required for computation. For how to obtain the tiling information, see [Transpose Tiling](Transpose-Tiling.md). |

```
enum class TransposeType : uint8_t {
    TRANSPOSE_TYPE_NONE,            // default value
    TRANSPOSE_NZ2ND_0213,           // Scenario 1: NZ2ND, axis swap between axes 1 and 2.
    TRANSPOSE_NZ2NZ_0213,           // Scenario 2: NZ2NZ, axis swap between axes 1 and 2.
    TRANSPOSE_NZ2NZ_012_WITH_N,     // Scenario 3: NZ2NZ, last-axis tiled.
    TRANSPOSE_NZ2ND_012_WITH_N,     // Scenario 4: NZ2ND, last-axis tiled.
    TRANSPOSE_NZ2ND_012_WITHOUT_N,  // Scenario 5: NZ2ND, last-axis merge.
    TRANSPOSE_NZ2NZ_012_WITHOUT_N,  // Scenario 6: NZ2NZ, last-axis merge.
    TRANSPOSE_ND2ND_ONLY,           // Scenario 7: 2D transpose.
    TRANSPOSE_ND_UB_GM,             // Currently not supported.
    TRANSPOSE_GRAD_ND_UB_GM,        // Not supported.
    TRANSPOSE_ND2ND_B16,            // Not supported.
    TRANSPOSE_NCHW2NHWC,            // Not supported.
    TRANSPOSE_NHWC2NCHW,            // Not supported.
    TRANSPOSE_ND2ND_021,            // Scenario 13: 2D or 3D transpose of the last two dimensions.
    TRANSPOSE_ND2ND_102,            // Scenario 14: Swap the first and second dimensions in 3D.
    TRANSPOSE_ND2ND_210,            // Scenario 15: Swap the first and third dimensions in 3D. 
    TRANSPOSE_ND2NZ_WITH_INTLV      // Scenario 16: Use interleave instructions to perform 2D ND2NZ transpose.             
    };
```

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   Scenarios 13 to 16 are supported only on Ascend 950PR/Ascend 950DT.
-   On Ascend 950PR/Ascend 950DT, dst and src space reuse is not supported for scenarios 13 to 16.

## Examples

This example is for scenario 1 (NZ2ND, axis swap between axes 1 and 2):

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape: \[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, N, H/N\], origin\_shape:\[B, S, N, H/N\], format:"ND", origin\_format:"ND"\}

B=1, N=2, S=64, H/N=32, and the input data type is half. For more complete examples, see the [transpose example](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/04_advanced_api/08_transpose/transpose).

```
// dst: Output tensor.
// src: Input tensor.
// NZ2ND, axis swap between axes 1 and 2.
AscendC::Transpose(dst, src, AscendC::TransposeType::TRANSPOSE_NZ2ND_0213, this->tiling);
```
