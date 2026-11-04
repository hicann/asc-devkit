# Channel Split of Cube Multiplication Output<a name="ZH-CN_TOPIC_0000002532228167"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:07:52.959Z pushedAt=2026-11-03T07:05:13.302Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002264134832_section310824820358"></a>

Channel split of the cube multiplication output is also called ChannelSplit. It refers to the case where the format of the cube C computed by Matmul is [NZ](../basics.md#zh-cn_topic_0000001622194138_section1453415011), and the cube C is stored in a fractal manner. For details about the NZ format, see [Data Format](../basics.md#zh-cn_topic_0000001622194138_section1453415011). When the physical layout format of the cube C is NZ and the data type is float, each fractal contains 16\*16 elements by default, that is, the size of the fractal is 16\*16. ChannelSplit splits each 16\*16 fractal of the cube C in this scenario into 16\*8 fractals, so that the cube C is stored as 16\*8 fractals.

Since the size of one float data is 4 bytes, the 16\*8 fractal satisfies 32-byte alignment on the inner axis, and the amount of data on the inner axis is consistent with the data unit processed by one NPU vector computation instruction, which facilitates subsequent computations. ChannelSplit is disabled by default. To enable it, set the **isEnableChannelSplit** parameter in [MatmulConfig](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md) to true.

**Figure 1**  ChannelSplit function diagram<a name="zh-cn_topic_0000002264134832_fig38211632121711"></a>  
![](../../../../figures/chan_split.png "ChannelSplit function diagram")

## Scenarios<a name="zh-cn_topic_0000002264134832_section118051016163613"></a>

For an NZ-format, float-type cube C that needs to be stored in 16\*8 fractals, use this feature.

## Constraints<a name="zh-cn_topic_0000002264134832_section14160134220363"></a>

To enable ChannelSplit, the following conditions must be met:

-   The data layout of the cube C is CubeFormat::NZ.
-   The data type of the cube C is float.
-   The logical memory location of the cube C is Global Memory.
-   The data type of the cube multiplication result L0C Buffer (CO1) is float.

## Example<a name="zh-cn_topic_0000002264134832_section15486294368"></a>

For the complete operator sample, see [matmul\_channelsplit operator sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_channelsplit_output).

```
// Specify the MatmulConfig template to obtain and modify
constexpr static MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
// Set the template parameter isEnableChannelSplit to true to enable the ChannelSplit function of this MatmulConfig template
constexpr static MatmulFuncParams funcParamsChannelSplit{
    false, false, false, false, 0, IterateOrder::ORDER_M, ScheduleType::INNER_PRODUCT, true, false, false, false, true/*isEnableChannelSplit*/
};
constexpr static MatmulConfig MM_CFG = GetMMConfig<configMode>(funcParamsChannelSplit);
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> mm;

// Perform a regular Matmul computation, with the final output fractal being 16*8
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
