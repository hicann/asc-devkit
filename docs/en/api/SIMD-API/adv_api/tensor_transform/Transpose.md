# Transpose

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-02T05:01:33.920Z pushedAt=2026-11-18T04:59:35.210Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- npu="9050" id7 -->
- Kirin 9050 processor products: Supported
<!-- end id7 -->
<!-- npu="kirin1" id8 -->
- KirinDev0001 processor products: Supported
<!-- end id8 -->
<!-- npu="kirin2" id9 -->
- KirinDev0002 processor products: Supported
<!-- end id9 -->
<!-- npu="kirin3" id10 -->
- KirinDev0003 processor products: Supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/tensor_transform/Transpose_res.md#id1 -->

## Description

Performs data layout and Reshape operations on the input data. The specific functions are as follows:

Scenario 1: NZ2ND, swapping axes 1 and 2

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, N, H/N\], origin\_shape:\[B, S, N, H/N\], format:"ND", origin\_format:"ND"\}

**Figure 1**  Scenario 1 data layout transformation  
![](../../../figures/scenario1_layout_transform.png "Scenario 1 data layout transformation")

[Scenario 2: NZ2NZ, swapping axes 1 and 2]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, H/N/16, N/16, 16, 16\], origin\_shape:\[B, S, N, H/N\], format:"NZ", origin\_format:"ND"\}

**Figure 2**  Scenario 2 data layout transformation  
![](../../../figures/scenario2_layout_transform.png "Scenario 2 data layout transformation")

[Scenario 3: NZ2NZ, tail-axis split]

Input tensor \{ shape:\[B, H / 16, S / 16, 16, 16\], origin\_shape:\[B, S, H\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, N, H/N/16, S / 16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

**Figure 3**  Scenario 3 data layout transformation  
![](../../../figures/scenario3_layout_transform.png "Scenario 3 data layout transformation")

[Scenario 4: NZ2ND, tail-axis split]

Input tensor \{ shape:\[B, H / 16, S / 16, 16, 16\], origin\_shape:\[B, S, H\], format:"NZ", origin\_format:"ND"\}

Output Tensor \{ shape:\[B, N, S, H/N\], origin\_shape:\[B, N, S, H/N\], format:"ND", origin\_format:"ND"\}

**Figure 4**  Scenario 4 data layout transformation  
![](../../../figures/scenario4_layout_transform.png "Scenario 4 data layout transformation")

[Scenario 5: NZ2ND, tail-axis merge]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output Tensor \{ shape:\[B, S, H\], origin\_shape:\[B, S, H\], format:"ND", origin\_format:"ND"\}

**Figure 5**  Scenario 5 data layout transformation  
![](../../../figures/scenario5_layout_transform.png "Scenario 5 data layout transformation")

[Scenario 6: NZ2NZ, tail-axis merge]

Input tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape:\[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output Tensor \{ shape:\[B, H/16, S/16, 16, 16\], origin\_shape:\[B, S, H\], format:"NZ", origin\_format:"ND"\}

**Figure 6**  Scenario 6 data layout transformation  
![](../../../figures/scenario6_layout_transform.png "Scenario 6 data layout transformation")

[Scenario 7: 2D transpose]

Supports transposing a 2D tensor on the Unified Buffer (UB), where both H and W in **srcShape** are integer multiples of 16.

**Figure 7**  Scenario 7 data layout transformation  
![](../../../figures/scenario7_layout_transform.png "Scenario 7 data layout transformation")

[Scenario 13: 2D transpose or transpose of the last two dimensions of a 3D tensor]

Supports transposing a 2D tensor on the UB or transposing the last two dimensions of a 3D tensor. The 2D tensor transpose follows the same data layout transformation as Scenario 7.

**Figure 8**  Scenario 13 3D tensor data layout transformation  
![](../../../figures/scenario13_3d_tensor_transform.png "Scenario 13 3D tensor data layout transformation")

[Scenario 14: Swap the first and second dimensions of a 3D tensor]

Supports swapping the first and second dimensions of a 3D Tensor on the UB.

**Figure 9**  Data layout transformation of the 3D Tensor in scenario 14  
![](../../../figures/scenario14_3d_tensor_transform.png "Data layout transformation of the 3D Tensor in scenario 14")

[Scenario 15: Swapping the first and third dimensions in 3D]

Supports swapping the first and third dimensions of a 3D Tensor on the UB.

**Figure 10**  Data layout transformation of the 3D Tensor in scenario 15  
![](../../../figures/scenario15_3d_tensor_transform.png "Data layout transformation of the 3D Tensor in scenario 15")

[Scenario 16: Using the interleave instruction to perform 2D ND2NZ transpose]

Supports using the [interleave instruction](../../basic_api/memory_vector_compute/data_rearrange_ISASI/Interleave.md) on the UB to transpose a 2D ND Tensor to NZ.

**Figure 11**  ND2NZ transpose using the interleave instruction in scenario 16  
![](../../../figures/scenario16_nd2nz_interleave_2.png "ND2NZ transpose using the interleave instruction in scenario 16")

## Implementation Principle

For the 11 functional scenarios of Transpose, the algorithm block diagram of each functional scenario is shown in the following figures.

**Figure 12**  Scenario 1: NZ2ND, axis 1 and axis 2 swap  
![](../../../figures/scenario1_nz2nd_axis12_swap.png "Scenario 1 - NZ2ND - axis 1 and axis 2 swap")

The computation process is divided into the following steps:

Process in a loop successively along the H/N direction, the N direction, and the B direction:

1.  First TransDataTo5HD step: Transpose S/16 consecutive 16\*16 squares along the S direction into temp, where each square is stored contiguously with the next square in temp;
2.  Second TransDataTo5HD step: Transpose the S/16 16\*16 squares in temp to dst. In dst, the data is in ND format. The address offset of two consecutive rows of data from the same square on the destination operand is \(H/N\)\*N elements, and the address offset of the same row of data from every two squares along the H direction on the destination operand is 16 elements.

**Figure 13**  Scenario 2: NZ2NZ, axis 1 and axis 2 swap  
![](../../../figures/scenario2_nz2nz_axis12_swap.png "Scenario 2 - NZ2NZ - axis 1 and axis 2 swap")

The computation process is divided into the following steps:

Process in a loop successively along the H/N direction, the N direction, and the B direction:

1.  First TransDataTo5HD step: Take S/16 consecutive 16\*16 squares along the S direction into temp, where each square is stored contiguously with the next in temp;
2.  Second TransDataTo5HD step: Transpose the S/16 16\*16 squares in temp to dst, where the data is in NZ format. Two consecutive rows of data from the same square have an address offset of \(H/N\)\*N elements on the destination operand, and the same row of data from every two squares along the H direction has an address offset of N\*16 elements on the destination operand.

**Figure 14**  Scenario 3: NZ2NZ, tail-axis split  
![](../../../figures/scenario3_nz2nz_tail_split.png "Scenario 3-NZ2NZ-tail-axis split")

The computation process is divided into the following steps:

Process along the H direction and then the B direction in a loop:

1.  First TransDataTo5HD step: Transpose S/16 consecutive 16\*16 squares into temp1 each time;
2.  DataCopy step: When H/N <= 16, move H/N\*S elements into temp2 each time; when H/N\ > 16, move 16\*S elements into temp2 for the first H/N/16 times, and move H/N%16\*S elements into temp2 for the last time;
3.  Second TransDataTo5HD step: Transpose the 16\*S square in temp2 into dst, where the data is in NZ format. Two consecutive rows of data from the same square have an address offset of 16 elements on the destination operand, and the same row of data from each pair of squares along the H direction has an address offset of S\*16 elements on the destination operand.

**Figure 15**  Scenario 4: NZ2ND, tail-axis split  
![](../../../figures/scenario4_nz2nd_tail_split.png "Scenario 4-NZ2ND-tail-axis split")

The computation process is divided into the following steps:

Process along the H direction and then the B direction in a loop:

1.  First TransDataTo5HD step: Transpose S/16 consecutive 16\*16 squares into temp1 each time;
2.  DataCopy step: when H/N<=16, move H/N\*S elements to temp2 each time; when H/N\>16, move 16\*S elements to temp2 in each of the first H/N/16 times, and move H/N%16\*S elements to temp2 the last time;
3.  Second TransDataTo5HD step: transpose the data in temp2 to dst, where the data is in ND format. The address offset of two consecutive rows of data from the same square on the destination operand is \(H/N+16-1\)/16\*16 elements, and the address offset of the same row of data from every two squares along the H direction on the destination operand is \(H/N+16-1\)/16\*16\*S elements.

**Figure 16**  Scenario 5: NZ2ND, tail-axis merge  
![](../../../figures/scenario5_nz2nd_tail_merge.png "Scenario 5-NZ2ND-tail-axis merge")

The computation process is divided into the following steps:

Process along the H direction and then the B direction in a loop:

1.  First TransDataTo5HD step: transpose one S\*16 square to temp1 each time;
2.  DataCopy step: when H/N<=16, move H/N\*S elements to temp2 each time; when H/N\>16, move 16\*S elements to temp2 in each of the first H/N/16 times, and move H/N%16\*S elements to temp2 in the last iteration;
3.  Second TransDataTo5HD step: transpose the 16\*S square in temp2 to dst, where the data is in ND format. The address offset of two consecutive rows of data from the same square on the destination operand is \(H+16-1\)/16\*16 elements, and the address offset of the same row of data from every two squares along the H direction on the destination operand is H/N\*S elements.

**Figure 17**  Scenario 6: NZ2NZ, tail-axis merge  
![](../../../figures/scenario6_nz2nz_tail_merge.png "Scenario 6-NZ2NZ-tail-axis merge")

The computation process is divided into the following steps:

Process along the H direction and then the B direction in a loop:

1.  First TransDataTo5HD step: transpose one S\*16 square to temp1 each time;
2.  DataCopy step: when H/N<=16, move H/N\*S elements to temp2 each time; when H/N\>16, move 16\*S elements to temp2 in each of the first H/N/16 times, and move H/N%16\*S elements to temp2 the last time;
3.  Second TransDataTo5HD step: Transpose the 16\*S squares in temp2 into dst, where the data is in NZ format. Two consecutive rows of data from the same square have an address offset of 16 elements on the destination operand, and the same row of data from every two squares along the H direction has an address offset of S\*16 elements on the destination operand.

**Figure 18**  Scenario 7: 2D transpose  
![](../../../figures/scenario7_2d_transpose.png "Scenario 7-2D transpose")

The computation process is as follows:

1.  Call TransDataTo5HD, and by setting different source operand address sequences and destination operand address sequences, transpose \[H, W\] into \[W, H\]. Both src and dst are in ND format.

**Figure 19**  Scenario 13: 2D transpose or transpose of the last two dimensions of a 3D tensor  
![](../../../figures/scenario13_2d_3d_transpose.png "Scenario 13-2D transpose or transpose of the last two dimensions of 3D")

The computation process is as follows:

1.  Invoke the internal computation logic to continuously write into the destination operand address by configuring different source operand address sequences, transposing \[H, W\] into \[W, H\], or transposing \[N, H, W\] into \[N, W, H\]. Both src and dst are in ND format.

The conversion processes of scenarios 14 and 15 are basically the same as the 3D transpose conversion process in scenario 13 above, except that the dimensions specified for transposition are different.

**Figure 20**  Scenario 16: Using the interleave instruction to perform a two-dimensional ND2NZ transpose  
![](../../../figures/scenario16_nd2nz_interleave.png "Scenario 16 - Two-dimensional ND2NZ transpose using the interleave instruction")

The computation process is as follows:

1.  Invoke the internal computation logic to continuously write into the destination operand addresses by configuring different source operand address sequences, transposing \[H, W\] from ND format into \[W1,H1,H0,W0\] NZ format, where H = H1 \* H0, W = W1 \* W0, H0 = 16, W0 = 2. src is in ND format and dst is in NZ format.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. To obtain the temporary space size **BufferSize**, use the **GetTransposeMaxMinTmpSize** API provided in [Transpose Tiling](Transpose-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, while the maximum space is used to improve performance.

Temporary space supports two methods: **applied by the API framework** and **passed in by the developer through the sharedTmpBuffer input parameter**. Therefore, the Transpose API has two prototypes:

-   Pass in the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T>
    __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, TransposeType transposeType, ConfusionTransposeTiling& tiling)
    ```

    In this method, the developer needs to allocate and manage the temporary memory space, and reuse this part of memory after the API call is completed. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization.

-   The API framework allocates the temporary space

    ```
    template <typename T>
    __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src, TransposeType transposeType, ConfusionTransposeTiling& tiling)
    ```

    In this method, the developer does not need to allocate the space, but needs to reserve the size of the temporary space.

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand.<br><br>Scenarios 1 to 7 support the following data types: int16_t, uint16_t, half, int32_t, uint32_t, float; scenarios 13 to 15 support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float; scenario 16 supports the following data types: int8_t, uint8_t. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src | Input | Source operand. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during the internal computation of the API. In this mode, developers can manage the sharedTmpBuffer memory space by themselves and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization. For how to obtain the shared buffer size, see [Transpose Tiling](Transpose-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| transposeType | Input | Type of data layout and reshape. The type is the TransposeType enumeration class, whose definition is shown in the following code. |
| tiling | Input | Tiling information required for computation. For how to obtain the tiling information, see [Transpose Tiling](Transpose-Tiling.md). |

```
enum class TransposeType : uint8_t {
    TRANSPOSE_TYPE_NONE,           // default value
    TRANSPOSE_NZ2ND_0213,          // Scenario 1: NZ2ND, swapping axes 1 and 2.
    TRANSPOSE_NZ2NZ_0213,          // Scenario 2: NZ2NZ, swapping axes 1 and 2.
    TRANSPOSE_NZ2NZ_012_WITH_N,    // Scenario 3: NZ2NZ, tail-axis split.
    TRANSPOSE_NZ2ND_012_WITH_N,    // Scenario 4: NZ2ND, tail-axis split.
    TRANSPOSE_NZ2ND_012_WITHOUT_N, // Scenario 5: NZ2ND, tail-axis merge.
    TRANSPOSE_NZ2NZ_012_WITHOUT_N, // Scenario 6: NZ2NZ, tail-axis merge.
    TRANSPOSE_ND2ND_ONLY,          // Scenario 7: 2D transpose.
    TRANSPOSE_ND_UB_GM,            // Not supported.
    TRANSPOSE_GRAD_ND_UB_GM,       // Not supported.
    TRANSPOSE_ND2ND_B16,           // Not supported.
    TRANSPOSE_NCHW2NHWC,           // Not supported.
    TRANSPOSE_NHWC2NCHW,           // Not supported.
    TRANSPOSE_ND2ND_021,           // Scenario 13: 2D transpose or transpose of the last two dimensions in 3D.
    TRANSPOSE_ND2ND_102,           // Scenario 14: Swap the first and second dimensions in 3D.
    TRANSPOSE_ND2ND_210,           // Scenario 15: Swap the first and third dimensions in 3D.
    TRANSPOSE_ND2NZ_WITH_INTLV     // Scenario 16: Use interleave instructions to perform 2D ND2NZ transpose.
};
```

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
<!-- npu="950" id11 -->
-   Scenarios 13 to 16 are supported only on the Ascend 950PR&950DT products.
-   On the Ascend 950PR&950DT products, scenarios 13 to 16 do not support space reuse between dst and src.
<!-- end id11 -->

## Examples

This example is for scenario 1 (NZ2ND, axis swap of dimensions 1 and 2):

Input Tensor \{ shape:\[B, N, H/N/16, S/16, 16, 16\], origin\_shape: \[B, N, S, H/N\], format:"NZ", origin\_format:"ND"\}

Output tensor \{ shape:\[B, S, N, H/N\], origin\_shape:\[B, S, N, H/N\], format:"ND", origin\_format:"ND"\}

B=1, N=2, S=64, H/N=32, and the input data type is half. For more complete examples, see [Transpose Example](../../../../../../examples/01_simd_cpp_api/04_advanced_api/08_transpose/transpose).

```
// dst: Output tensor.
// src: Input tensor.
// NZ2ND, swapping axes 1 and 2.
AscendC::Transpose(dst, src, AscendC::TransposeType::TRANSPOSE_NZ2ND_0213, this->tiling);
```
