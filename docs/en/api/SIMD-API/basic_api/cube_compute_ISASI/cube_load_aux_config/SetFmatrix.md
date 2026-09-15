# SetFcube<a name="ZH-CN_TOPIC_0000001834660673"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T15:28:09.446Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Used to set the attribute description of the FeatureMap when calling [LoadData (Convolution Data Move)](../cube_compute_load/Load3D.md). When the template parameter **isSetFCube** of Load3D is set to **false**, the attributes of the FeatureMap passed to Load3D (including **l1H**, **l1W**, and **padList**; for parameter descriptions, see [Table 3 Parameter description of the LoadData3DParamsV1 structure](../cube_compute_load/Load3D.md#zh-cn_topic_0000002512171652_table679014222918) and [Table 4 Parameter description of the LoadData3DParamsV2 structure](../cube_compute_load/Load3D.md#zh-cn_topic_0000002512171652_table193501032193419)) do not take effect, and developers need to set them through this API.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFcube(uint16_t l1H, uint16_t l1W, const uint8_t padList[4], const FcubeMode& fcubeMode)
```

## Parameters<a name="section622mcpsimp"></a>

<a name="table8955841508"></a>
**Table 1** Parameters

| Parameter | Input/Output | Description |
| --------- | ---------- | ------ |
| l1H | Input | Source operand height. Value range: l1H∈[1, 32767]. |
| l1W | Input | Source operand width. Value range: l1W∈[1, 32767]. |
| padList | Input | Padding list [padding\_left, padding\_right, padding\_top, padding\_bottom], where each element has a value range of [0, 255]. The default value is {0, 0, 0, 0}. |
| fcubeMode | Input | Controls whether the **LoadData** instruction obtains information from the left or right register. It is of the **FcubeMode** type, defined as follows. Currently, only **FCUBE\_LEFT** is supported, and both the left and right matrices use this configuration.<br>enum class FcubeMode : uint8_t {<br>    FCUBE_LEFT = 0,<br>    FCUBE_RIGHT = 1,<br>};<br>|

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with [LoadData (Convolution Data Move)](../cube_compute_load/Load3D.md) and must be called before [LoadData (Convolution Data Move)](../cube_compute_load/Load3D.md). The **fcubeMode** parameter must be consistent with the **fCubeCtrl** value in the Load3D API parameters.
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

For the complete calling example, see the [SetLoadDataBoundary sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary). The following is a key code snippet from the sample:

```cpp
    AscendC::LoadData3DParamsV2<U> loadData3dParams;
    loadData3dParams.l1W = 1;
    loadData3dParams.l1H = K;
    loadData3dParams.channelSize = N;
    loadData3dParams.kExtension = N;
    loadData3dParams.mExtension = K;
    loadData3dParams.kStartPt = 0;
    loadData3dParams.mStartPt = 0;
    loadData3dParams.strideW = 1;
    loadData3dParams.strideH = 1;
    loadData3dParams.filterW = 1;
    loadData3dParams.filterH = 1;
    loadData3dParams.dilationFilterW = 1;
    loadData3dParams.dilationFilterH = 1;
    loadData3dParams.enTranspose = true;
    loadData3dParams.enSmallK = false;
    loadData3dParams.padValue = 0;
    loadData3dParams.filterSizeW = 0;
    loadData3dParams.filterSizeH = 0;
    loadData3dParams.fCubeCtrl = false;
    uint8_t padList[AscendC::PAD_SIZE] = {0, 0, 0, 0};
    static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};
    AscendC::SetFcube(N, 1, padList, AscendC::FcubeMode::FCUBE_LEFT); // Enable the FM memory layout mode and obtain information from the left register.
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    AscendC::SetLoadDataRepeat({0, 1, 0});
    AscendC::SetLoadDataBoundary(0);
    AscendC::SetLoadDataPaddingValue(0);
    AscendC::LoadData<U, LOAD3D_CONFIG>(b2, rightCube, loadData3dParams);
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    uint16_t dstStride = DivCeil(N, 16);
    AscendC::SetLoadDataRepeatWithStride({0, 1, 0, dstStride});
    AscendC::SetLoadDataPaddingValue(0);
    AscendC::LoadDataWithStride<U, LOAD3D_CONFIG>(b2, rightCube, loadData3dParams);
#endif
```
