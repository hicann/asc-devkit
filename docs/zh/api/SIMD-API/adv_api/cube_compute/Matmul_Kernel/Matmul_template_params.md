# Matmul模板参数

## 产品支持情况

<!-- npu="950" id5 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas 训练系列产品：不支持
<!-- end id10 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_template_params_res.md#id1 -->

## 功能说明

创建Matmul对象时需要传入：

-   A、B、C、Bias的参数类型信息，类型信息通过MatmulType来定义，包括：内存逻辑位置、数据格式、数据类型、是否转置、数据排布和是否开启L1复用。
-   MatmulConfig信息（可选），用于配置Matmul模板信息以及相关的配置参数。不配置默认使用Norm模板。

    <!-- npu="310b" id11 -->针对Atlas 200I/500 A2 推理产品，当前只支持使用默认的Norm模板。<!-- end id11 -->

<!-- npu="950,A3,910b,310p,310b,x90" id20 -->
<a id="matmul-callback-func"></a>

-   MatmulCallBackFunc回调函数信息（可选），用于配置A矩阵从GM拷贝到L1 Buffer（A1）、B矩阵从GM拷贝到L1 Buffer（B1）、计算结果从L0C Buffer（CO1）拷贝到GM的自定义函数。当前支持如下产品型号：

    <!-- npu="950" id12 -->
    Ascend 950PR/Ascend 950DT
    <!-- end id12 -->
    <!-- npu="A3" id13 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id13 -->
    <!-- npu="910b" id14 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id14 -->
    <!-- npu="x90" id3 -->
    Kirin X90
    <!-- end id3 -->

-   MatmulPolicy信息（可选），用于配置Matmul可拓展模块策略。不配置使用默认模板策略。当前支持如下产品型号：

    <!-- npu="950" id15 -->
    Ascend 950PR/Ascend 950DT
    <!-- end id15 -->
    <!-- npu="A3" id16 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id17 -->
    <!-- npu="310b" id18 -->
    Atlas 200I/500 A2 推理产品
    <!-- end id18 -->
    <!-- npu="310p" id19 -->
    Atlas 推理系列产品AI Core
    <!-- end id19 -->
    <!-- npu="x90" id4 -->
    Kirin X90
    <!-- end id4 -->
<!-- end id20 -->

## 函数原型

Matmul模板参数如下：
```
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM, class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
```

-   A\_TYPE、B\_TYPE、C\_TYPE类型信息通过[MatmulType](Matmul_usage.md#table1188045714378)来定义。
-   auto类型的参数MM\_CFG（可选）：
    -   支持MatmulConfig类型：

        Matmul模板信息，具体内容见[MatmulConfig](MatmulConfig.md)。

    -   支持MatmulApiStaticTiling类型：

        MatmulApiStaticTiling参数说明见[表1](#table7939847143412)。

        MatmulApiStaticTiling结构体中包括一组常量化Tiling参数和MatmulConfig结构。这种类型参数的定义方式为，通过调用[MatmulConfig](MatmulConfig.md)章节中介绍的获取模板的接口，指定\(singleM, singleN, singleK, baseM, baseN, baseK\)参数，获取自定义模板；将该模板传入[GetMatmulApiTiling](GetMatmulApiTiling.md)接口，得到常量化的参数。这种常量化的方式将得到MatmulApiStaticTiling结构体中定义的一组常量化参数，可以优化Matmul计算中的Scalar计算。当前支持定义为MatmulApiStaticTiling常量化的Tiling参数的模板有：Norm、IBShare、MDL模板。MxMatmul场景支持定义为MatmulApiStaticTiling常量化的Tiling参数的模板有：Norm、MDL模板。

-   MM\_CB（可选），用于支持不同的搬入搬出需求，实现定制化的搬入搬出功能。具体内容见[MatmulCallBackFunc](MatmulCallBackFunc.md)。
-   MATMUL\_POLICY\_DEFAULT\_OF\(MatmulPolicy\)（可选），用于配置Matmul可拓展模块的策略。当前支持不配置该参数（使用默认模板策略）或者配置1个MatmulPolicy参数。

    MATMUL\_POLICY\_DEFAULT\_OF定义如下，用于简化MATMUL\_POLICY的类型声明。该模板参数的详细使用方式请参考[MatmulPolicy](MatmulPolicy.md)。

    ```
    #define MATMUL_POLICY_DEFAULT_OF(DEFAULT)      \
    template <const auto& = MM_CFG, typename ...>  \
            class MATMUL_POLICY = AscendC::Impl::Detail::DEFAULT
    ```

## 参数说明

**表1**  MatmulApiStaticTiling常量化Tiling参数说明

<a name="table7939847143412"></a>
| 参数 | 数据类型 | 说明 |
| --- | --- | --- |
| M, N, Ka, Kb,<br><br>singleCoreM, singleCoreN, singleCoreK,<br><br>baseM, baseN, baseK,<br><br>depthA1, depthB1,<br><br>stepM， stepN，stepKa，stepKb,<br><br>isBias,<br><br>transLength,<br><br>iterateOrder,<br><br>dbL0A, dbL0B,<br><br>dbL0C,<br><br>shareMode,<br><br>shareL1Size,<br><br>shareL0CSize,<br><br>shareUbSize,<br><br>batchM,<br><br>batchN,<br><br>singleBatchM,<br><br>singleBatchN,<br><br>mxTypePara | int32_t | 与[TCubeTiling](../Matmul_Tiling/TCubeTiling_struct.md)结构体中各同名参数含义一致。本结构体中的参数是常量化后的常数值。 |
| cfg | [MatmulConfig](MatmulConfig.md#matmulconfig-params) | Matmul模板的参数配置。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
// 用户自定义回调函数
void DataCopyOut(
    const __gm__ void* gm, const LocalTensor<int8_t>& co1Local, const void* dataCopyOutParams, const uint64_t tilingPtr,
    const uint64_t dataPtr);
void CopyA1(
    const AscendC::LocalTensor<int8_t>& aMatrix, const __gm__ void* gm, int row, int col, int useM, int useK,
    const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(
    const AscendC::LocalTensor<int8_t>& bMatrix, const __gm__ void* gm, int row, int col, int useK, int useN,
    const uint64_t tilingPtr, const uint64_t dataPtr);

// 定义创建对象时需要传入的A、B、C、Bias参数类型信息
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

// 使用MDL模板，创建Matmul实例
AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL> mm1;

AscendC::MatmulConfig mmConfig{false /*不使用Norm模板*/,          true /*使用BasicBlock模板*/,
                               false /*不使用MDL模板*/,           128 /*Matmul计算时base块M轴长度*/,
                               128 /*Matmul计算时base块N轴长度*/, 64 /*Matmul计算时base块K轴长度*/};
mmConfig.enUnitFlag = false; // 不开启UnitFlag功能
// 使用自定义的mmConfig，创建Matmul实例
AscendC::Matmul<aType, bType, cType, biasType, mmConfig> mm2;

// 使用NORM模板、自定义的mmConfig和自定义的回调函数，创建Matmul实例
AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, AscendC::MatmulCallBackFunc<DataCopyOut, CopyA1, CopyB1>> mm3;
```
