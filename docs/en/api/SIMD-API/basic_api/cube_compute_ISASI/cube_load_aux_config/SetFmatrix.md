# SetFmatrix<a name="ZH-CN_TOPIC_0000001834660673"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:52:52.288Z pushedAt=2026-10-26T07:41:44.138Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetFmatrix_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Used to set the attribute description of the FeatureMap when calling [LoadData (Convolution Data)](../cube_compute_load/LoadData_3D.md). When the template parameter **isSetFMatrix** of LoadData (Convolution Data) is set to **false**, the attributes of the FeatureMap passed to LoadData (Convolution Data) (including **l1H**, **l1W**, and **padList**; see [Table 3 Parameter description of the LoadData3DParamsV1 structure](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table679014222918) and [Table 4 Parameter description of the LoadData3DParamsV2 structure](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419) for parameter descriptions) do not take effect, and developers need to set them through this API.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFmatrix(uint16_t l1H, uint16_t l1W, const uint8_t padList[4], const FmatrixMode& fmatrixMode)
```

## Parameters<a name="section622mcpsimp"></a>

<a name="table8955841508"></a>
**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| --------- | ---------- | ------ |
| l1H | Input | Source operand height, value range: l1H∈[1, 32767]. |
| l1W | Input | Source operand width, value range: l1W∈[1, 32767]. |
| padList | Input | Padding list [padding\_left, padding\_right, padding\_top, padding\_bottom], each element value range: [0, 255]. Defaults to {0, 0, 0, 0}. |
| fmatrixMode | Input | Controls whether the **LoadData** instruction obtains information from the **left** or **right** register. **FmatrixMode** type, defined as follows. Currently only FMATRIX\_LEFT is supported, and both the left and right matrices use this configuration.<br>enum class FmatrixMode : uint8_t {<br>    FMATRIX_LEFT = 0,<br>    FMATRIX_RIGHT = 1,<br>}; |

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with [LoadData (Convolution Data)](../cube_compute_load/LoadData_3D.md) and must be called before [LoadData (Convolution Data)](../cube_compute_load/LoadData_3D.md). The **fmatrixMode** parameter must be consistent with the **fMatrixCtrl** value in the parameters of LoadData (Convolution Data).
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

For the complete calling example, see [SetLoadDataBoundary sample](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary). The following is a key code snippet from the sample:

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
    loadData3dParams.fMatrixCtrl = false;
    uint8_t padList[AscendC::PAD_SIZE] = {0, 0, 0, 0};
    static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};
    AscendC::SetFmatrix(N, 1, padList, AscendC::FmatrixMode::FMATRIX_LEFT); // Enable the FM memory layout mode and obtain information from the left register.
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    AscendC::SetLoadDataRepeat({0, 1, 0});
    AscendC::SetLoadDataBoundary(0);
    AscendC::SetLoadDataPaddingValue(0);
    AscendC::LoadData<U, LOAD3D_CONFIG>(b2, rightMatrix, loadData3dParams);
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    uint16_t dstStride = DivCeil(N, 16);
    AscendC::SetLoadDataRepeatWithStride({0, 1, 0, dstStride});
    AscendC::SetLoadDataPaddingValue(0);
    AscendC::LoadDataWithStride<U, LOAD3D_CONFIG>(b2, rightMatrix, loadData3dParams);
#endif
```
