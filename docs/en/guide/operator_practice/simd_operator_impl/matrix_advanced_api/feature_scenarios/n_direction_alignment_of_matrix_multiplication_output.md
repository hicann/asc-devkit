# N-Direction Alignment of Matrix Multiplication Output<a name="ZH-CN_TOPIC_0000002532228169"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T13:37:55.591Z -->

## Description<a name="zh-cn_topic_0000002265055666_section310824820358"></a>

N-direction alignment of the matrix multiplication output means that the result matrix C of the matrix multiplication is output in the ND\_ALIGN format. In Matmul matrix multiplication, the commonly used matrix data formats are ND and NZ. For details, see [Data Format](../basic_knowledge.md#zh-cn_topic_0000001622194138_section1453415011). ND\_ALIGN is another matrix data format. It is generally used in matrix multiplication where the N direction is non-32-byte aligned. After the result matrix C is configured in the ND\_ALIGN format, matrix C is output according to the padding rule of 32-byte alignment in the N direction. For details, see [ND\_ALIGN](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md#li075920427155).

The following uses a Matmul with M=16, K=16, N=14, and the A and B matrix data type of half as an example to describe the ND\_ALIGN output function. When matrix C is configured in the ND format and output to the Global Memory, it is output in a non-32-byte aligned manner according to the original N-direction size, as shown in [Figure 1](#zh-cn_topic_0000002265055666_fig2311855162511). When matrix C is configured in the ND format, it is output in a 32-byte aligned manner in the N direction, as shown in [Figure 2](#zh-cn_topic_0000002265055666_fig25741012182719). The last two columns in the N direction of matrix C are padded with the actual data of the next row to achieve 32-byte alignment in the N direction before output. When matrix C is configured in the ND\_ALIGN format, the Matmul API pads the last two columns in the N direction of matrix C with invalid data to ensure 32-byte alignment in the N direction before output, as shown in [Figure 3](#zh-cn_topic_0000002265055666_fig4840114152818).

**Figure 1**  Non-32-byte aligned C matrix in the N direction in the ND format<a name="zh-cn_topic_0000002265055666_fig2311855162511"></a>  
![](../../../../figures/non_32_byte_aligned_c_matrix_in_n_direction_in_nd_format.png "Non-32-byte aligned C matrix in the N direction in the ND format")

**Figure 2**  32-byte aligned C matrix in the N direction in the ND format<a name="zh-cn_topic_0000002265055666_fig25741012182719"></a>  
![](../../../../figures/32_byte_aligned_c_matrix_in_n_direction_in_nd_format.png "32-byte aligned C matrix in the N direction in the ND format")

**Figure 3**  32-byte aligned C matrix in the N direction in the ND\_ALIGN format<a name="zh-cn_topic_0000002265055666_fig4840114152818"></a>  
![](../../../../figures/32_byte_aligned_c_matrix_in_n_direction_in_nd_align_format.png "32-byte aligned C matrix in the N direction in the ND_ALIGN format")

## Scenarios<a name="zh-cn_topic_0000002265055666_section118051016163613"></a>

In Matmul computation, the N direction is non-32-byte aligned, while the N direction of the output C matrix is required to be 32-byte aligned.

## Constraints<a name="zh-cn_topic_0000002265055666_section14160134220363"></a>

If the C matrix is configured to be output in the ND_ALIGN format, the buffer space allocated for the C matrix is the space size after the N dimension is aligned upward to 32 bytes.

## Calling Example<a name="zh-cn_topic_0000002265055666_section15486294368"></a>

For the complete operator sample, see [matmul_nd_align operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_format_nd_align).

-   Tiling Implementation

    Call the SetCType API to set the data format of matrix C to CubeFormat::ND_ALIGN. Other Tiling implementations are the same as those in the [basic scenario](../operator_implementation.md).

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform); 
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16); 
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);  
    // Set matrix C with the buffer at GM and the data format ND_ALIGN.
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND_ALIGN, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(AscendC::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ... // Other implementation content
    optiling::TCubeTiling tilingData;   
    int ret = tiling.GetTiling(tilingData);
    ```

-   Kernel Implementation

    Compared with the [basic scenario](../operator_implementation.md#zh-cn_topic_0000001622514006_li1032116474330), the ND_ALIGN output feature requires that when creating a Matmul object, you set the data format of the template parameter cType to CubeFormat::ND_ALIGN.

    ```
    #include "lib/matmul_intf.h"
    
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
    // Set the data format of the template parameter cType to ND_ALIGN.
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND_ALIGN, float> cType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType; 
    AscendC::Matmul<aType, bType, cType, biasType> mm; 
    ```
