# SetMatmulConfigParams

## 功能说明

在计算Tiling时，用于自定义设置[表1](#table9646134355611)中的MatmulConfig参数。本接口中配置的参数对应的功能在Tiling与核函数（Kernel）中需要保持一致，所以本接口中的参数取值，需要与核函数（Kernel）侧对应的MatmulConfig参数值保持一致，详细MatmulConfig参数请见[表2](../Matmul_Kernel/MatmulConfig.md#matmulconfig-params)。

## 函数原型

```
void SetMatmulConfigParams(int32_t mmConfigTypeIn = 1, bool enableL1CacheUBIn = false, ScheduleType scheduleTypeIn = ScheduleType::INNER_PRODUCT, MatrixTraverse traverseIn = MatrixTraverse::NOSET, bool enVecND2NZIn = false)
```

```
void SetMatmulConfigParams(const MatmulConfigParams& configParams)
```

## 参数说明

**表1**  参数说明

<a name="table9646134355611"></a>
| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mmConfigTypeIn | 输入 | 设置Matmul的模板类型，需要与Matmul对象创建的模板一致，当前只支持配置为0或1。<br>0：代表Norm模板<br>1：代表MDL模板，默认值为1 |
| enableL1CacheUBIn | 输入 | 配置是否开启L1缓存Unified Buffer（UB）计算块；推荐开启场景：MTE3和MTE2流水串行较多的场景。<br>false：不开启L1缓存UB计算块，默认值为false<br>true：开启L1缓存UB计算块<!-- npu="A3" id1 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该参数。<!-- end id1 --><!-- npu="910b" id2 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该参数。<!-- end id2 --><!-- npu="310p" id3 --><br><br>Atlas 推理系列产品AI Core支持该参数。<!-- end id3 --><!-- npu="310b" id4 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id4 --> |
| scheduleTypeIn | 输入 | 配置Matmul数据搬运模式。参数取值如下：<br>ScheduleType::INNER_PRODUCT：默认模式，在K方向上做MTE1的循环搬运<br>ScheduleType::OUTER_PRODUCT：在M或N方向上做MTE1的循环搬运<br>ScheduleType::N_BUFFER_33：[NBuffer33](../Matmul_Kernel/MatmulPolicy.md#li194081238103913)模板的数据搬运模式，MTE2每次搬运A矩阵的1x3个基本块，直至A矩阵所有3x3个基本块全载在L1 Buffer中 |
| traverseIn | 输入 | Matmul做矩阵运算的循环迭代顺序，即一次迭代计算出[baseM, baseN]大小的C矩阵分片后，自动偏移到下一次迭代输出的C矩阵位置的偏移顺序。参数取值如下：<br><br>NOSET：0，当前无效。<br><br>FIRSTM：先往M轴方向偏移再往N轴方向偏移。<br><br>FIRSTN：先往N轴方向偏移再往M轴方向偏移。 |
| enVecND2NZIn | 输入 | 是否开启ND2NZ。 |
| configParams | 输入 | config相关参数，类型为MatmulConfigParams，结构体具体定义如下方代码所示。其中的参数说明请参考[表2](#table15780447181917)。 |

```
struct MatmulConfigParams {
    int32_t mmConfigType;
    bool enableL1CacheUB;
    ScheduleType scheduleType;
    MatrixTraverse traverse;
    bool enVecND2NZ;
    MatmulConfigParams(
        int32_t mmConfigTypeIn = 1, bool enableL1CacheUBIn = false,
        ScheduleType scheduleTypeIn = ScheduleType::INNER_PRODUCT, MatrixTraverse traverseIn = MatrixTraverse::NOSET,
        bool enVecND2NZIn = false)
    {
        mmConfigType = mmConfigTypeIn;
        enableL1CacheUB = enableL1CacheUBIn;
        scheduleType = scheduleTypeIn;
        traverse = traverseIn;
        enVecND2NZ = enVecND2NZIn;
    }
};
```

**表2**  MatmulConfigParams结构体内参数说明

<a name="table15780447181917"></a>
| 参数名称 | 含义 |
| --- | --- |
| mmConfigType | 设置Matmul的模板类型，需要与Matmul对象创建的模板一致，当前只支持配置为0或1。<br>0：代表Norm模板<br>1：代表MDL模板，默认值为1 |
| enableL1CacheUB | 配置是否开启L1缓存UB计算块；推荐开启场景：MTE3和MTE2流水串行较多的场景。<br>false：不开启L1缓存UB计算块，默认值为false<br>true：开启L1缓存UB计算块 |
| scheduleType | 配置Matmul数据搬运模式。参数取值如下：<br>ScheduleType::INNER_PRODUCT：默认模式，在K方向上做MTE1的循环搬运<br>ScheduleType::OUTER_PRODUCT：在M或N方向上做MTE1的循环搬运<br>ScheduleType::N_BUFFER_33：[NBuffer33](../Matmul_Kernel/MatmulPolicy.md#li194081238103913)模板的数据搬运模式，MTE2每次搬运A矩阵的1x3个基本块，直至A矩阵所有3x3个基本块全载在L1 Buffer中 |
| traverse | Matmul做矩阵运算的循环迭代顺序，即一次迭代计算出[baseM, baseN]大小的C矩阵分片后，自动偏移到下一次迭代输出的C矩阵位置的偏移顺序。参数取值如下：<br><br>NOSET：0，当前无效。<br><br>FIRSTM：先往M轴方向偏移再往N轴方向偏移。<br><br>FIRSTN：先往N轴方向偏移再往M轴方向偏移。 |
| enVecND2NZ | 是否开启ND2NZ |

## 返回值说明

无

## 约束说明

-   本接口必须在[GetTiling](GetTiling.md)接口前调用。
-   若Matmul对象使用NBuffer33模板策略，即MatmulPolicy为[NBuffer33MatmulPolicy](../Matmul_Kernel/MatmulPolicy.md#li194081238103913)，则在调用[GetTiling](GetTiling.md)接口生成Tiling参数前，必须通过本接口将scheduleTypeIn参数设置为ScheduleType::N\_BUFFER\_33，以启用NBuffer33模板策略的Tiling生成逻辑。

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
tiling.SetMatmulConfigParams(0); // 额外设置
// matmul_tiling::MatmulConfigParams configParams =
//     {1, false, matmul_tiling::ScheduleType::OUTER_PRODUCT, matmul_tiling::MatrixTraverse::FIRSTM};
// tiling.SetMatmulConfigParams(configParams);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
