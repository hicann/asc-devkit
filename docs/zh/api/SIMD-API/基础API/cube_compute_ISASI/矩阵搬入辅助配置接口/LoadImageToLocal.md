# LoadImageToLocal<a name="ZH-CN_TOPIC_0000001945534165"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

将图像数据从Global Memory搬运到Local Memory。搬运过程中可以完成图像预处理操作：包括图像翻转，改变图像尺寸（抠图，裁边，缩放，伸展），以及色域转换，类型转换等。图像预处理的相关参数通过[SetAippFunctions](SetAippFunctions.md)进行配置。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadImageToLocal(const LocalTensor<T>& dst, const LoadImageToLocalParams& loadDataParams)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>LocalTensor的起始地址需要保证32字节对齐。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadImageToLocalParams。<br>具体定义请参考：`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>参数说明参考[表2](#table8955841508)。 |

<a name="table8955841508"></a>
**表2** LoadImageToLocalParams结构体内参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| horizSize | 输入 | 从源图中加载图片的水平宽度，单位为像素，取值范围：horizSize∈[2, 4095]。 |
| vertSize | 输入 | 从源图中加载图片的垂直高度，单位为像素，取值范围：vertSize∈[2, 4095]。 |
| horizStartPos | 输入 | 加载图片在源图片上的水平起始地址，单位为像素，取值范围：horizStartPos∈[0, 4095]。默认为0。<br>**注意：当输入图片为YUV420SP、XRGB8888、RGB888、YUV400格式时，该参数需要是偶数。** |
| vertStartPos | 输入 | 加载图片在源图片上的垂直起始地址，单位为像素，取值范围：vertStartPos∈[0, 4095]。默认为0。<br>**注意：当输入图片为YUV420SP格式时，该参数需要是偶数。** |
| srcHorizSize | 输入 | 源图像水平宽度，单位为像素，取值范围：srcHorizSize∈[2, 4095]。<br>**注意：当输入图片为YUV420SP格式时，该参数需要是偶数。** |
| topPadSize | 输入 | 目的图像顶部填充的像素数，取值范围：topPadSize∈[0, 32]，默认为0。进行数据填充时使用，需要先调用SetAippFunctions通过AippPaddingParams配置填充的数值，再通过topPadSize、botPadSize、leftPadSize、rightPadSize配置填充的大小范围。 |
| botPadSize | 输入 | 目的图像底部填充的像素数，取值范围：botPadSize∈[0, 32]，默认为0。 |
| leftPadSize | 输入 | 目的图像左边填充的像素数，取值范围：leftPadSize∈[0, 32]，默认为0。 |
| rightPadSize | 输入 | 目的图像右边填充的像素数，取值范围：rightPadSize∈[0, 32]，默认为0。 |
| sid | 输入 | 预留参数。为后续的功能做保留，开发者暂时无需关注，使用默认值即可。 |

## 数据类型

<!-- npu="950" id8 -->Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、half。<!-- end id8 --><br>
<!-- npu="A3" id9 -->Atlas A3训练系列产品/Atlas A3推理系列产品，支持数据类型：int8_t、half。<!-- end id9 --><br>
<!-- npu="910b" id10 -->Atlas A2训练系列产品/Atlas A2推理系列产品，支持数据类型：int8_t、half。<!-- end id10 --><br>
<!-- npu="310b" id11 -->Atlas 200I/500 A2 推理产品，支持数据类型为：int8_t、uint8_t、half。<!-- end id11 --><br>
<!-- npu="310p" id12 -->Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、half。<!-- end id12 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 加载到dst的图片的大小加padding的大小必须小于等于所在存储空间的大小。
- 当通过[SetAippFunctions](SetAippFunctions.md)配置padding模式为块填充模式或者镜像块填充模式时，因为padding的数据来自于抠出的图片，左右padding的长度（leftPadSize、rightPadSize）必须小于或等于抠图的水平长度（horizSize），上下padding的长度（topPadSize、botPadSize）必须小于或等于抠图的垂直的长度（vertSize）。
- 支持的物理存储位置为：L1 Buffer（TPosition: A1/B1）。
    <!-- npu="950" id13 -->
    - 特别针对Ascend 950PR/Ascend 950DT，支持物理存储位置为：VECIN、VECCALC、VECOUT。
    <!-- end id13 -->

## 调用示例<a name="section22811728184217"></a>

<!-- npu="310p" id14 -->
该调用示例支持的运行平台为Atlas 推理系列产品AI Core，示例图片格式为YUV420SP。

```cpp
constexpr uint16_t imageWidth = 32;
constexpr uint16_t imageHeight = 32;
constexpr uint32_t yPlaneSize = imageWidth * imageHeight;
constexpr uint32_t inputSize = yPlaneSize * 3 / 2;
constexpr uint32_t outputChannels = 32;
constexpr uint32_t dstElemCount = imageWidth * imageHeight * outputChannels;

// 源操作数：GM上一张32x32 YUV420SP图片，前1024B为Y平面，后512B为UV平面。
AscendC::GlobalTensor<uint8_t> fmGlobal;
fmGlobal.SetGlobalBuffer((__gm__ uint8_t *)src, inputSize);

// 目的操作数：L1 Buffer。
AscendC::LocalTensor<int8_t> featureMapA1(AscendC::TPosition::A1, a1Addr, dstElemCount);

// format = YUV420SP_U8，表示输入为uint8_t类型的YUV420 Semi-Planar图片。
AscendC::AippInputFormat inputFormat = AscendC::AippInputFormat::YUV420SP_U8;

AscendC::AippParams<int8_t> aippConfig;
// 本示例不做HW padding、通道交换、单行读取、均值/缩放和色域转换，这些子参数使用结构体默认值。
// 输出U = int8_t时将通道padding到32通道，即每个像素输出32B。
aippConfig.cPaddingParams.cPaddingMode = 0;
// 通道padding补0，目的Tensor额外通道写入0。
aippConfig.cPaddingParams.cPaddingValue = static_cast<int8_t>(0);

// src0为Y平面起始地址；src1为UV平面起始地址，YUV420SP 32x32的UV偏移为32 * 32 = 1024个uint8_t。
AscendC::SetAippFunctions(fmGlobal, fmGlobal[yPlaneSize], inputFormat, aippConfig);

AscendC::LoadImageToLocalParams loadParams;
// 从源图加载32像素宽，等于整张32x32示例图的宽度。
loadParams.horizSize = 32;
// 从源图加载32像素高，等于整张32x32示例图的高度。
loadParams.vertSize = 32;
// 水平起始位置为第0个像素；YUV420SP场景要求为偶数，0满足约束。
loadParams.horizStartPos = 0;
// 垂直起始位置为第0行；YUV420SP场景要求为偶数，0满足约束。
loadParams.vertStartPos = 0;
// 源图每行宽度为32像素；YUV420SP场景要求为偶数，32满足约束。
loadParams.srcHorizSize = 32;
// 顶部不做HW方向padding。
loadParams.topPadSize = 0;
// 底部不做HW方向padding。
loadParams.botPadSize = 0;
// 左侧不做HW方向padding。
loadParams.leftPadSize = 0;
// 右侧不做HW方向padding。
loadParams.rightPadSize = 0;
// 预留参数，固定配置为0。
loadParams.sid = 0;

AscendC::LoadImageToLocal(featureMapA1, loadParams);
```
<!-- end id14 -->
