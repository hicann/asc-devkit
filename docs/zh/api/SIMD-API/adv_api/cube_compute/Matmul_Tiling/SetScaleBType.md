# SetScaleBType

## 功能说明

MxMatmul场景，设置scaleB矩阵的位置、数据格式、是否转置等信息，这些信息需要和Kernel侧的设置保持一致。如果不调用本接口，scaleB矩阵的信息将与[SetBType](SetBType.md)中设置的B矩阵的信息保持一致。

## 函数原型

```
int32_t SetScaleBType(TPosition scalePos, CubeFormat scaleType, bool isScaleTrans = true)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| scalePos | 输入 | scaleB矩阵的内存逻辑位置。<!-- npu="950" id1 --><br><br>针对Ascend 950PR/Ascend 950DT，scaleB矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM。<!-- end id1 --> |
| scaleType | 输入 | scaleB矩阵的物理排布格式。<!-- npu="950" id2 --><br><br>针对Ascend 950PR/Ascend 950DT，scaleB矩阵可设置为CubeFormat::ND，CubeFormat::NZ。<!-- end id2 --> |
| isScaleTrans<a name="p2934103115919"></a> | 输入 | scaleB矩阵是否转置。参数支持的取值如下：<br>true：默认值，scaleB矩阵转置；<br>false：scaleB矩阵不转置。 |

## 返回值说明

-1表示设置失败； 0表示设置成功。

## 约束说明

无

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

// 设置scaleB矩阵，buffer位置为GM，数据格式为ND，转置
tiling.SetScaleBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, true);
```
