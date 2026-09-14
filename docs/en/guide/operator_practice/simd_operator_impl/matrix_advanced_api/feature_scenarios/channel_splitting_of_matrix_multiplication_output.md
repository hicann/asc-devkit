# Channel Split of Matrix Multiplication Output<a name="ZH-CN_TOPIC_0000002532228167"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:37:24.911Z -->

## Description<a name="zh-cn_topic_0000002264134832_section310824820358"></a>

Channel splitting of the matrix multiplication output, also known as ChannelSplit, refers to the case where the format of the C matrix resulting from the Matmul computation is [NZ](../basic_knowledge.md#zh-cn_topic_0000001622194138_section1453415011). In this case, the C matrix is stored in a fractal layout. For details about the NZ format, see [Data Format](../basic_knowledge.md#zh-cn_topic_0000001622194138_section1453415011). When the physical layout of the C matrix is NZ and the data type is float, each fractal contains 16\*16 elements by default, that is, the fractal size is 16\*16. ChannelSplit splits each 16\*16 fractal of the C matrix in this scenario into 16\*8 fractals, so that the C matrix is stored in 16\*8 fractals.

Since the size of one float data element is 4 bytes, a 16\*8 fractal satisfies 32-byte alignment on the inner axis, and the amount of data on the inner axis is consistent with the data unit processed by one NPU vector computation instruction, which facilitates subsequent computations. ChannelSplit is disabled by default. To enable it, set the isEnableChannelSplit parameter in [MatmulConfig](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) to true.

**Figure 1**  ChannelSplit function diagram<a name="zh-cn_topic_0000002264134832_fig38211632121711"></a>  
![](../../../../figures/channelsplit_function_diagram.png "ChannelSplit function diagram")

## Scenarios<a name="zh-cn_topic_0000002264134832_section118051016163613"></a>

Use this feature when the C matrix in NZ format and float type needs to be stored in 16\*8 fractals.

## Constraints<a name="zh-cn_topic_0000002264134832_section14160134220363"></a>

To enable ChannelSplit, the following conditions must be met:

-   The data layout format of the C matrix is CubeFormat::NZ.
-   The data type of the C matrix is float.
-   The logical memory location of the C matrix is Global Memory.
-   The data type of the matrix multiplication result CO1 is float.

## Calling Example<a name="zh-cn_topic_0000002264134832_section15486294368"></a>

For the complete operator sample, see [matmul\_channelsplit operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_channelsplit_output).

```
// Specify the MatmulConfig template to obtain and modify
constexpr static MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
// Modify the template parameter isEnableChannelSplit=true to enable the ChannelSplit feature of this MatmulConfig template
constexpr static MatmulFuncParams funcParamsChannelSplit{
    false, false, false, false, 0, IterateOrder::ORDER_M, ScheduleType::INNER_PRODUCT, true, false, false, false, true/*isEnableChannelSplit*/
};
constexpr static MatmulConfig MM_CFG = GetMMConfig<configMode>(funcParamsChannelSplit);
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> mm;

// Perform regular Matmul computation, with the final output fractal being 16*8
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
