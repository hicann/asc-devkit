# Conv3D模板参数

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/Conv3D_template_params_res.md#id1 -->

## 功能说明

创建Conv3D对象时需要传入：

-   Input、Weight、Output和Bias（可选）的参数类型信息，类型信息通过[ConvType](Conv3D_usage.md#table19081115275)来定义，包括：内存逻辑位置、数据格式、数据类型。
-   Conv3dParam信息（可选），用于开启不同场景的性能优化模板。**当前暂不支持使用。**

## 函数原型

```
template <class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE = biasType, class CONV_CFG = Conv3dParam>
using Conv3D = Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| INPUT_TYPE | 输入 | ConvType类型模板参数，指定Input的参数类型信息。 |
| WEIGHT_TYPE | 输入 | ConvType类型模板参数，指定Weight的参数类型信息。 |
| OUTPUT_TYPE | 输入 | ConvType类型模板参数，指定Output的参数类型信息。 |
| BIAS_TYPE | 可选输入 | ConvType类型模板参数，指定Bias的参数类型信息。 |
| CONV_CFG | 可选输入 | ConvParam类型模板参数，用于开启不同场景的性能优化模板，当前版本只支持基础模板，不开启性能优化。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
#include "lib/conv/conv3d/conv3d_api.h"

using inputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
using weightType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::FRACTAL_Z_3D, bfloat16_t>;
using outputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
// 可选参数，如果不带Bias场景，可以不传
using biasType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::ND, float>;
struct ConvCustom : public ConvApi::ConvParam {
    __aicore__ inline ConvCustom(){};
}; // 可选参数，当前版本只支持基础模板，不开启性能优化，可以不传

Conv3dApi::Conv3D<inputType, weightType, outputType, biasType, ConvCustom> conv3dApi;
```
