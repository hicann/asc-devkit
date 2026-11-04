# N-Direction Alignment of Cube Multiplication Output<a name="ZH-CN_TOPIC_0000002532228169"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:08:41.499Z pushedAt=2026-11-03T07:05:13.303Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002265055666_section310824820358"></a>

N-direction alignment of the cube multiplication output means that the result cube C of cube multiplication is output in the ND\_ALIGN format. In Matmul cube multiplication, the commonly used cube data formats are ND and NZ. For details, see [Data Format](../basics.md#zh-cn_topic_0000001622194138_section1453415011). ND\_ALIGN is another cube data format. This format is generally used in cube multiplication where the N direction is not 32-byte aligned. After the result cube C is configured in the ND\_ALIGN format, cube C is output according to the padding rule of 32-byte alignment in the N direction. For details, see [ND\_ALIGN](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li075920427155).

The following uses a Matmul example with M=16, K=16, N=14, and the half data type for cubes A and B to describe the ND\_ALIGN output function. When cube C is configured in the ND format and output to Global Memory, it is output without 32-byte alignment in the original N direction, as shown in [Figure 1](#zh-cn_topic_0000002265055666_fig2311855162511). When cube C is configured in the ND format, it is output with 32-byte alignment in the N direction, as shown in [Figure 2](#zh-cn_topic_0000002265055666_fig25741012182719). The last two columns in the N direction of cube C are padded with the actual data of the next row to achieve 32-byte alignment in the N direction before output. When cube C is configured in the ND\_ALIGN format, the Matmul API pads the last two columns in the N direction of cube C with invalid data to ensure 32-byte alignment in the N direction before output, as shown in [Figure 3](#zh-cn_topic_0000002265055666_fig4840114152818).

**Figure 1**  Schematic diagram of cube C in the ND format without 32-byte alignment in the N direction<a name="zh-cn_topic_0000002265055666_fig2311855162511"></a>  
![](../../../../figures/nd_no_32.png "Schematic diagram of cube C in the ND format without 32-byte alignment in the N direction")

**Figure 2**  Schematic diagram of cube C in the ND format with 32-byte alignment in the N direction<a name="zh-cn_topic_0000002265055666_fig25741012182719"></a>  
![](../../../../figures/nd_32_align.png "Schematic diagram of cube C in the ND format with 32-byte alignment in the N direction")

**Figure 3**  Schematic diagram of cube C in the ND\_ALIGN format with 32-byte alignment in the N direction<a name="zh-cn_topic_0000002265055666_fig4840114152818"></a>  
![](../../../../figures/nd_align_32.png "Schematic diagram of cube C in the ND_ALIGN format with 32-byte alignment in the N direction")

## Scenarios<a name="zh-cn_topic_0000002265055666_section118051016163613"></a>

A scenario where the N direction is not 32-byte aligned in Matmul computation, and the N direction of the output cube C is required to be 32-byte aligned.

## Constraints<a name="zh-cn_topic_0000002265055666_section14160134220363"></a>

If the cube C is configured for ND\_ALIGN format output, the Buffer space allocated for the cube C is the space size after N is aligned upward to 32 bytes.

## Example<a name="zh-cn_topic_0000002265055666_section15486294368"></a>

For the complete operator sample, see [matmul\_nd\_align operator sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_format_nd_align).

-   Tiling Implementation

    Call the SetCType API to set the data format of cube C to CubeFormat::ND\_ALIGN. Other Tiling implementations are the same as those in the [basic scenario](../operator_impl.md).

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform); 
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16); 
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);  
    // Set cube C with the buffer in GM and the data format ND_ALIGN
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND_ALIGN, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(AscendC::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ... // Other implementation content
    optiling::TCubeTiling tilingData;   
    int ret = tiling.GetTiling(tilingData);
    ```

-   Kernel Implementation

    Compared with the [basic scenario](../operator_impl.md#zh-cn_topic_0000001622514006_li1032116474330), the ND\_ALIGN output function requires that when creating a Matmul object, the data format of the template parameter cType be set to CubeFormat::ND\_ALIGN.

    ```
    #include "lib/matmul_intf.h"
    
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
    // Set the data format of the template parameter cType to ND_ALIGN
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND_ALIGN, float> cType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType; 
    AscendC::Matmul<aType, bType, cType, biasType> mm; 
    ```
