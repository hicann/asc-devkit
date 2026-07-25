# Matmul使用说明

Ascend C提供一组Matmul高阶API，方便用户快速实现Matmul矩阵乘法的运算操作。

Matmul的计算公式为：C = A \* B + Bias，其示意图如下。

-   A、B为源操作数，A为左矩阵，形状为\[M, K\]；B为右矩阵，形状为\[K, N\]。
-   C为目的操作数，存放矩阵乘结果的矩阵，形状为\[M, N\]。
-   Bias为矩阵乘偏置，形状为\[1, N\]。对A\*B结果矩阵的每一行都采用该Bias进行偏置。

**图1**  Matmul矩阵乘示意图  
![](../../../../figures/Matmul矩阵乘示意图.png "Matmul矩阵乘示意图")

> [!NOTE]说明
>下文中提及的M轴方向，即为A矩阵纵向；K轴方向，即为A矩阵横向或B矩阵纵向；N轴方向，即为B矩阵横向；尾轴，即为矩阵最后一个维度。

Kernel侧实现Matmul矩阵乘运算的步骤概括为：

1.  创建Matmul对象。
2.  初始化操作。
3.  设置左矩阵A、右矩阵B、Bias。
4.  完成矩阵乘操作。
5.  结束矩阵乘操作。

使用Matmul API实现矩阵乘运算的具体步骤如下：

1.  创建Matmul对象。

    创建Matmul对象的示例如下：

    -   默认为MIX模式（包含矩阵计算和矢量计算），该场景下通常不定义ASCENDC\_CUBE\_ONLY宏，如果在程序中使用了ASCENDC\_CUBE\_ONLY宏，则必须使用ASCEND\_IS\_AIC宏和ASCEND\_IS\_AIV宏将Cube计算和Vector计算隔离开。
    -   纯Cube模式（只有矩阵计算）场景下，建议在代码中定义ASCENDC\_CUBE\_ONLY宏，避免额外的性能开销。

    ```
    // 纯cube模式（只有矩阵计算）场景下，需要设置该代码宏，并且必须在#include "lib/matmul_intf.h"之前设置
    // #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    AscendC::Matmul<aType, bType, cType, biasType> mm;
    ```

    创建对象时需要传入A、B、C、Bias的参数类型信息，类型信息通过[MatmulType](#table1188045714378)来定义，包括：内存逻辑位置、数据格式、数据类型、数据来源的内存逻辑位置。

    ```
    template <
        AscendC::TPosition POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false,
        LayoutMode LAYOUT = LayoutMode::NONE, bool IBSHARE = false, TPosition SRCPOS = TPosition::GM>
    struct MatmulType {
        constexpr static AscendC::TPosition pos = POSITION;
        constexpr static CubeFormat format = FORMAT;
        using T = TYPE;
        constexpr static bool isTrans = ISTRANS;
        constexpr static LayoutMode layout = LAYOUT;
        constexpr static bool ibShare = IBSHARE;
        constexpr static TPosition srcPos = SRCPOS;
    };
    ```

    **表1**  MatmulType参数说明

    <a name="table1188045714378"></a>
    | 参数 | 说明 |
    | --- | --- |
    | POSITION | 内存逻辑位置。<!-- npu="950" id15 --><br><br>针对Ascend 950PR/Ascend 950DT：<br>    A矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>B矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>Bias可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>C矩阵可设置为TPosition::GM，TPosition::VECIN<br> 注意，A矩阵、B矩阵或Bias矩阵设置为TPosition::VECOUT或TPosition::TSCM时，对应矩阵用于单核计算的数据必须全部在Unified Buffer或L1 Buffer上，具体样例请参考[matmul_vecout样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_vecout)、[自定义数据来源为VECOUT的TSCM输入的Matmul算子样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm_src_vecout)、[自定义数据来源为GM的TSCM输入的Matmul算子样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm)。<!-- end id15 --><!-- npu="A3" id16 --><br><br>针对Atlas A3 训练系列产品/Atlas A3 推理系列产品：<br>    A矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>B矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>Bias可设置为TPosition::GM，TPosition::VECOUT<br>C矩阵可设置为TPosition::GM，TPosition::VECIN, TPosition::CO1<br>   注意，C矩阵设置为TPosition::CO1时，C矩阵的数据排布格式仅支持CubeFormat::NZ，C矩阵的数据类型仅支持float、int32_t。<!-- end id16 --><!-- npu="910b" id17 --><br>   <br>针对Atlas A2 训练系列产品/Atlas A2 推理系列产品：<br>    A矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>B矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>Bias可设置为TPosition::GM，TPosition::VECOUT<br>C矩阵可设置为TPosition::GM，TPosition::VECIN, TPosition::CO1<br>   注意，C矩阵设置为TPosition::CO1时，C矩阵的数据排布格式仅支持CubeFormat::NZ，C矩阵的数据类型仅支持float、int32_t。<!-- end id17 --><!-- npu="310p" id18 --><br><br>针对Atlas 推理系列产品AI Core：<br>    A矩阵可设置为TPosition::GM，TPosition::VECOUT<br>B矩阵可设置为TPosition::GM，TPosition::VECOUT<br>Bias可设置为TPosition::GM，TPosition::VECOUT<br>C矩阵可设置为TPosition::GM，TPosition::VECIN<!-- end id18 --><!-- npu="310b" id19 --><br>    <br>针对Atlas 200I/500 A2 推理产品：<br>    A矩阵可设置为TPosition::GM<br>B矩阵可设置为TPosition::GM<br>Bias可设置为TPosition::GM<br>C矩阵可设置为TPosition::GM<!-- end id19 --><!-- npu="x90" id1 --><br>    <br>针对Kirin X90:<br>    A矩阵可设置为TPosition::GM，TPosition::VECOUT，TPosition::TSCM<br>B矩阵可设置为TPosition::GM，TPosition::VECOUT, TPosition::TSCM<br>Bias可设置为TPosition::GM，TPosition::VECOUT<br>C矩阵可设置为TPosition::GM，TPosition::CO1<!-- end id1 --><!-- npu="9030" id2 --><br>    <br>针对Kirin 9030：<br>    A矩阵可设置为TPosition::TSCM<br>B矩阵可设置为TPosition::TSCM<br>Bias可设置为TPosition::GM<br>C矩阵可设置为TPosition::GM<!-- end id2 --> |
    | FORMAT | 数据的物理排布格式。<!-- npu="950" id20 --><br><br>针对Ascend 950PR/Ascend 950DT：<br>    A矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::COLUMN_MAJOR，CubeFormat::VECTOR<br>B矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::COLUMN_MAJOR<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::ND_ALIGN，CubeFormat::COLUMN_MAJOR<br>    <br>针对Ascend 950PR/Ascend 950DT，请注意：<br>    仅在非MxMatmul场景中，A、B、C矩阵Format支持CubeFormat::COLUMN_MAJOR。当Format为CubeFormat::COLUMN_MAJOR时，对应矩阵仅支持内存逻辑位置为TPosition::GM。<br>输入A矩阵或B矩阵设置为TPosition::TSCM时，对应的Format仅支持CubeFormat::NZ。<br>C矩阵设置为TPosition::VECIN，CubeFormat::ND时，要求尾轴32字节对齐，比如数据类型是half的情况下，N要求是16的倍数。<!-- end id20 --><!-- npu="A3" id21 --><br>    <br>针对Atlas A3 训练系列产品/Atlas A3 推理系列产品：<br>    A矩阵可设置为CubeFormat::ND，CubeFormat::NZ, CubeFormat::VECTOR<br>B矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::ND_ALIGN<!-- end id21 --><!-- npu="910b" id22 --><br>    <br>针对Atlas A2 训练系列产品/Atlas A2 推理系列产品：<br>    A矩阵可设置为CubeFormat::ND，CubeFormat::NZ, CubeFormat::VECTOR<br>B矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::ND_ALIGN<!-- end id22 --><!-- npu="310p" id23 --><br>    <br>针对Atlas 推理系列产品AI Core：<br>    A矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>B矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::ND_ALIGN<br> 注意：针对Atlas 推理系列产品AI Core，C矩阵设置为CubeFormat::ND时，要求尾轴32字节对齐，比如数据类型是half的情况下，N要求是16的倍数。<!-- end id23 --><!-- npu="310b" id24 --><br>    <br>针对Atlas 200I/500 A2 推理产品：<br>    A矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>B矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND，CubeFormat::NZ<!-- end id24 --><!-- npu="x90" id3 --><br>    <br>针对Kirin X90:<br>    A矩阵可设置为CubeFormat::ND，CubeFormat::NZ, CubeFormat::VECTOR<br>B矩阵可设置为CubeFormat::ND，CubeFormat::NZ<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND，CubeFormat::NZ，CubeFormat::ND_ALIGN<!-- end id3 --><!-- npu="9030" id4 --><br>    <br>针对Kirin 9030：<br>A矩阵可设置为CubeFormat::NZ，CubeFormat::VECTOR<br>B矩阵可设置为CubeFormat::NZ<br>Bias可设置为CubeFormat::ND<br>C矩阵可设置为CubeFormat::ND<!-- end id4 --><!-- npu="310b" id25 --><br><br>注意: 针对Atlas 200I/500 A2 推理产品，C矩阵设置为TPosition::VECIN或者TPosition::TSCM，CubeFormat::ND时，要求尾轴32字节对齐，比如数据类型是half的情况下，N要求是16的倍数；C矩阵设置为TPosition::VECIN或者TPosition::TSCM，CubeFormat::NZ时，N要求是16的倍数。<!-- end id25 --><br>    <br>关于CubeFormat::NZ格式的A矩阵、B矩阵、C矩阵的对齐约束，请参考[表3](#table98851538118)。 |
    | TYPE | 数据类型。<!-- npu="950" id26 --><br><br>针对Ascend 950PR/Ascend 950DT：<br>    1、非MxMatmul场景：<br>A矩阵可设置为half、float、bfloat16_t 、int8_t、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t<br>B矩阵可设置为half、float、bfloat16_t 、int8_t、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t<br>Bias可设置为half、float、int32_t、bfloat16_t<br>C矩阵可设置为half、float、bfloat16_t、int32_t、int8_t、fp8_e4m3fn_t、hifloat8_t<br>    2、MxMatmul场景：<br>A矩阵可设置为fp8_e4m3fn_t、fp8_e5m2_t、fp4x2_e2m1_t、fp4x2_e1m2_t<br>B矩阵可设置为fp8_e4m3fn_t、fp8_e5m2_t、fp4x2_e2m1_t、fp4x2_e1m2_t<br>Bias可设置为half、float、bfloat16_t<br>C矩阵可设置为half、float、bfloat16_t<!-- end id26 --><!-- npu="A3" id27 --><br>    <br>针对Atlas A3 训练系列产品/Atlas A3 推理系列产品：<br>A矩阵可设置为half、float、bfloat16_t 、int8_t、int4b_t<br>B矩阵可设置为half、float、bfloat16_t 、int8_t、int4b_t<br>Bias可设置为half、float、int32_t<br>C矩阵可设置为half、float、bfloat16_t、int32_t、int8_t<!-- end id27 --><!-- npu="910b" id28 --><br>    <br>针对Atlas A2 训练系列产品/Atlas A2 推理系列产品：<br>A矩阵可设置为half、float、bfloat16_t 、int8_t、int4b_t<br>B矩阵可设置为half、float、bfloat16_t 、int8_t、int4b_t<br>Bias可设置为half、float、int32_t<br>C矩阵可设置为half、float、bfloat16_t、int32_t、int8_t<!-- end id28 --><!-- npu="310p" id29 --><br><br>针对Atlas 推理系列产品AI Core：<br>    A矩阵可设置为half、int8_t<br>B矩阵可设置为half、int8_t<br>Bias可设置为float、int32_t<br>C矩阵可设置为half、float、int8_t、int32_t<!-- end id29 --><!-- npu="310b" id30 --><br>    <br>针对Atlas 200I/500 A2 推理产品：<br>    A矩阵可设置为half、float、bfloat16_t 、int8_t<br>B矩阵可设置为half、float、bfloat16_t 、int8_t<br>Bias矩阵可设置为half、float、int32_t<br>C矩阵可设置为half、float、bfloat16_t、int32_t<!-- end id30 --><!-- npu="x90" id5 --><br>    <br>针对Kirin X90:<br>    A矩阵可设置为half、int8_t<br>B矩阵可设置为half、int8_t<br>Bias可设置为half、int32_t<br>C矩阵可设置为half、int32_t、int8_t<!-- end id5 --><!-- npu="9030" id6 --><br>    <br>针对Kirin 9030：<br>    A矩阵可设置为half<br>B矩阵可设置为half<br>Bias可设置为half<br>C矩阵可设置为half<!-- end id6 --><br>注意：除fp8_e4m3fn_t/fp8_e5m2_t两种数据类型、B矩阵为int8_t数据类型外，A矩阵和B矩阵数据类型需要一致，具体数据类型组合关系请参考[表2](#table1996113269499)。A矩阵和B矩阵为int4b_t数据类型时，矩阵内轴的数据个数必须为偶数。例如，A矩阵为int4b_t数据类型且不转置时，[singleCoreK](../Matmul-Tiling类/TCubeTiling结构体.md#p11899125875617)必须是偶数。 |
    | ISTRANS | 是否开启支持矩阵转置的功能。<br>    true：开启支持矩阵转置的功能，运行时可以分别通过[SetTensorA](SetTensorA.md)和[SetTensorB](SetTensorB.md)中的isTransposeA、isTransposeB参数设置A、B矩阵是否转置。若设置A、B矩阵转置，Matmul会认为A矩阵形状为[K, M]，B矩阵形状为[N, K]。<br>false：默认值，不开启支持矩阵转置的功能，通过[SetTensorA](SetTensorA.md)和[SetTensorB](SetTensorB.md)不能设置A、B矩阵的转置情况。Matmul会认为A矩阵形状为[M, K]，B矩阵形状为[K, N]。<br>    <br>注意，由于L1 Buffer上的矩阵数据有分形对齐的约束，A、B矩阵转置和不转置时所需的L1空间可能不相同，在开启支持矩阵转置功能时，必须保证按照[Matmul Tiling参数](../Matmul-Tiling类/TCubeTiling结构体.md)申请的L1空间不超过L1 Buffer的规格，判断方式为(depthA1*Ceil(baseM/c0Size)*baseK + depthB1*Ceil(baseN/c0Size)*baseK) * db * sizeof(dtype) < L1Size，db表示L1是否开启double buffer，取值1（不开启double buffer）或2（开启double buffer），其余参数的含义请参考[表1](../Matmul-Tiling类/TCubeTiling结构体.md#table1563162142915)。<!-- npu="9030" id7 --><br>    <br>Kirin 9030不支持此参数。<!-- end id7 --> |
    | LAYOUT | 表征数据的排布。<br>    <br>NONE：默认值，表示不使用BatchMatmul；其他选项表示使用BatchMatmul。<br>    <br>NORMAL：BMNK的数据排布格式，具体可参考[IterateBatch](IterateBatch.md#li536045110115)中对该数据排布的介绍。<br>    <br>BSNGD：原始BSH shape做reshape后的数据排布，具体可参考[IterateBatch](IterateBatch.md#li298041002213)中对该数据排布的介绍。<br>    <br>SBNGD：原始SBH shape做reshape后的数据排布，具体可参考[IterateBatch](IterateBatch.md#li6785191319227)中对该数据排布的介绍。<br>    <br>BNGS1S2：一般为前两种数据排布进行矩阵乘的输出，S1S2数据连续存放，一个S1S2为一个batch的计算数据，具体可参考[IterateBatch](IterateBatch.md#li1922441712222)中对该数据排布的介绍。<!-- npu="9030" id8 --><br>    <br>Kirin 9030不支持此参数。<!-- end id8 --> |
    | IBSHARE<a name="p84551411817"></a> | 是否开启IBShare（IntraBlock Share）。IBShare的功能是能够复用L1 Buffer上相同的A矩阵或B矩阵数据，复用的矩阵必须在L1 Buffer上全载。A矩阵和B矩阵仅有一个开启IBShare的场景，与[IBShare模板](MatmulConfig.md#table6981133810309)配合使用，具体参数设置详见[表2](MatmulConfig.md#table1761013213153)。<br>    <br>注意，A矩阵和B矩阵同时开启IBShare的场景，表示L1 Buffer上的A矩阵和B矩阵同时复用，需要满足：<br>    同一算子中其它Matmul对象的A矩阵和B矩阵也必须同时开启IBShare；<!-- npu="910b" id90 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，获取矩阵计算结果时，只支持调用[IterateAll](IterateAll.md)接口，且只支持输出到GlobalTensor，即计算结果放置于Global Memory的地址。<!-- end id90 --><!-- npu="A3" id91 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，获取矩阵计算结果时，只支持调用[IterateAll](IterateAll.md)接口，且只支持输出到GlobalTensor，即计算结果放置于Global Memory的地址。<!-- end id91 --><!-- npu="950" id92 --><br><br>Ascend 950PR/Ascend 950DT，获取矩阵计算结果时，支持输出到GlobalTensor和LocalTensor。输出到LocalTensor，即计算结果放置于Local Memory的场景，默认实现策略与[SplitM模板策略](MatmulPolicy.md)相同，且仅支持输出数据类型为float，仅支持[Norm模板](MatmulConfig.md)。<!-- end id92 --><!-- npu="950" id31 --><br>    <br>Ascend 950PR/Ascend 950DT支持该参数。<!-- end id31 --><!-- npu="A3" id32 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id32 --><!-- npu="910b" id33 --><br>    <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id33 --><!-- npu="310p" id34 --><br>    <br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id34 --><!-- npu="310b" id35 --><br>    <br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id35 --><!-- npu="x90" id9 --><br>    <br>Kirin X90不支持此参数。<!-- end id9 --><!-- npu="9030" id10 --><br>    <br>Kirin 9030不支持此参数。<!-- end id10 --> |
    | SRC_POSITION | <!-- npu="950" id36 -->该参数仅支持Ascend 950PR/Ascend 950DT。<br>    <br><!-- end id36 -->A/B矩阵的POSITION参数配置为TPosition::TSCM时，必须要设置TSCM中矩阵数据的来源的内存逻辑位置，默认为TPosition::GM。<!-- npu="950" id37 --><br><br>针对Ascend 950PR/Ascend 950DT：<br>    A矩阵可设置为TPosition::GM，TPosition::VECOUT<br>B矩阵可设置为TPosition::GM，TPosition::VECOUT<!-- end id37 --> |

    **表2**  Matmul输入输出数据类型的支持列表

    <a name="table1996113269499"></a>
    | A矩阵 | B矩阵 | Bias | C矩阵 | 支持平台 |
    | --- | --- | --- | --- | --- |
    | float | float | float/half | float/half/bfloat16_t | <!-- npu="950" id38 -->Ascend 950PR/Ascend 950DT<!-- end id38 --><!-- npu="A3" id39 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id39 --><!-- npu="910b" id40 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id40 --><!-- npu="310b" id41 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id41 --> |
    | half | half | float | float | <!-- npu="950" id42 -->Ascend 950PR/Ascend 950DT<!-- end id42 --><!-- npu="A3" id43 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id43 --><!-- npu="910b" id44 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id44 --><!-- npu="310p" id45 --><br>    <br>Atlas 推理系列产品AI Core<!-- end id45 --><!-- npu="310b" id46 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id46 --> |
    | half | half | half | float/bfloat16_t | <!-- npu="950" id47 -->Ascend 950PR/Ascend 950DT<!-- end id47 --><!-- npu="A3" id48 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id48 --><!-- npu="910b" id49 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id49 --><!-- npu="310b" id50 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id50 --> |
    | int8_t | int8_t | int32_t | int32_t/half | <!-- npu="950" id93 -->Ascend 950PR/Ascend 950DT<!-- end id93 --><!-- npu="A3" id94 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id94 --><!-- npu="910b" id95 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id95 --><!-- npu="310p" id96 --><br>    <br>Atlas 推理系列产品AI Core<!-- end id96 --><!-- npu="310b" id97 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id97 --><!-- npu="x90" id11 --><br>    <br>Kirin X90<!-- end id11 --> |
    | int4b_t | int4b_t | int32_t | int32_t/half | <!-- npu="A3" id51 -->Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id51 --><!-- npu="910b" id52 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id52 --> |
    | bfloat16_t | bfloat16_t | float | float | <!-- npu="950" id53 -->Ascend 950PR/Ascend 950DT<!-- end id53 --><!-- npu="A3" id54 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id54 --><!-- npu="910b" id55 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id55 --><!-- npu="310b" id56 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id56 --> |
    | bfloat16_t | bfloat16_t | half | float/half | <!-- npu="950" id57 -->Ascend 950PR/Ascend 950DT<!-- end id57 --><!-- npu="A3" id58 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id58 --><!-- npu="910b" id59 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id59 --> |
    | half | half | float | int8_t | <!-- npu="950" id60 -->Ascend 950PR/Ascend 950DT<!-- end id60 --><!-- npu="A3" id61 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id61 --><!-- npu="910b" id62 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id62 --> |
    | bfloat16_t | bfloat16_t | float | int8_t | <!-- npu="950" id63 -->Ascend 950PR/Ascend 950DT<!-- end id63 --><!-- npu="A3" id64 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id64 --><!-- npu="910b" id65 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id65 --> |
    | int8_t | int8_t | int32_t | int8_t | <!-- npu="950" id66 -->Ascend 950PR/Ascend 950DT<!-- end id66 --><!-- npu="A3" id67 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id67 --><!-- npu="910b" id68 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id68 --><!-- npu="310p" id69 --><br>    <br>Atlas 推理系列产品AI Core<!-- end id69 --><!-- npu="x90" id12 --><br>    <br>Kirin X90<!-- end id12 --> |
    | half | half | float | half/bfloat16_t | <!-- npu="950" id70 -->Ascend 950PR/Ascend 950DT<!-- end id70 --><!-- npu="A3" id71 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id71 --><!-- npu="910b" id72 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id72 --><!-- npu="310p" id73 --><br>    <br>Atlas 推理系列产品AI Core<!-- end id73 --><!-- npu="310b" id74 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id74 --> |
    | half | half | half | half/bfloat16_t | <!-- npu="950" id98 -->Ascend 950PR/Ascend 950DT<!-- end id98 --><!-- npu="A3" id99 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id99 --><!-- npu="910b" id100 --<br>   ><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id100 --><!-- npu="310b" id101 --<br>    ><br>Atlas 200I/500 A2 推理产品<!-- end id101 --><!-- npu="x90" id13 --><br>    <br>Kirin X90<!-- end id13 --><!-- npu="9030" id14 --><br>    <br>Kirin 9030<!-- end id14 --> |
    | bfloat16_t | bfloat16_t | float | bfloat16_t/half | <!-- npu="950" id75 -->Ascend 950PR/Ascend 950DT<!-- end id75 --><!-- npu="A3" id76 --><br>    <br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<!-- end id76 --><!-- npu="910b" id77 --><br>   <br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id77 --><!-- npu="310b" id78 --><br>    <br>Atlas 200I/500 A2 推理产品<!-- end id78 --> |
    | half | int8_t | float | float | <!-- npu="310p" id79 -->Atlas 推理系列产品AI Core<!-- end id79 --> |
    | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e4m3fn_t/fp8_e5m2_t | float/half/bfloat16_t | fp8_e4m3fn_t/half/bfloat16_t/float | <!-- npu="950" id80 -->Ascend 950PR/Ascend 950DT<!-- end id80 --> |
    | hifloat8_t | hifloat8_t | float/half/bfloat16_t | hifloat8_t/half/bfloat16_t/float | <!-- npu="950" id81 -->Ascend 950PR/Ascend 950DT<!-- end id81 --> |
    | float | float | bfloat16_t | float/half/bfloat16_t | <!-- npu="950" id82 -->Ascend 950PR/Ascend 950DT<!-- end id82 --> |
    | bfloat16_t | bfloat16_t | bfloat16_t | float/half/bfloat16_t | <!-- npu="950" id83 -->Ascend 950PR/Ascend 950DT<!-- end id83 --> |
    | half | half | bfloat16_t | float/half/bfloat16_t | <!-- npu="950" id84 -->Ascend 950PR/Ascend 950DT<!-- end id84 --> |
    | int8_t | int8_t | int32_t | bfloat16_t | <!-- npu="950" id85 -->Ascend 950PR/Ascend 950DT<!-- end id85 --> |

2.  初始化操作。
<a name="li5878185413338"></a>

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // 初始化matmul对象，参数含义请参考REGIST_MATMUL_OBJ章节
    ```

3.  设置左矩阵A、右矩阵B、Bias。

    ```
    mm.SetTensorA(gm_a);    // 设置左矩阵A
    mm.SetTensorB(gm_b);    // 设置右矩阵B
    mm.SetBias(gm_bias);    // 设置Bias
    ```

    <!-- npu="310p" id86 -->
    Atlas 推理系列产品AI Core上需要额外调用[SetLocalWorkspace](SetLocalWorkspace.md)接口设置计算所需的UB空间。
    ```
    mm.SetLocalWorkspace(usedUbBufLen);
    ```
    <!-- end id86 -->

4.  完成矩阵乘操作。

    用户可以选择以下三种调用方式之一。

    -   调用[Iterate](Iterate.md#li135771283591)完成单次迭代计算，叠加while循环完成单核全量数据的计算。Iterate方式，可以自行控制迭代次数，完成所需数据量的计算，方式比较灵活。

        ```
        // API接口内部会进行循环结束条件判断处理
        while (mm.Iterate()) {
            mm.GetTensorC(gm_c);
        }
        ```

    -   调用[IterateAll](IterateAll.md)完成单核上所有数据的计算。IterateAll方式，无需循环迭代，使用比较简单。

        ```
        mm.IterateAll(gm_c);
        ```

    <a id="user-managed-co1"></a>
    
    -   用户申请用于存放矩阵乘结果的L0C Buffer（CO1）内存，调用一次或多次[Iterate](Iterate.md#li4843165185812)完成单次或多次迭代计算，在需要搬出计算结果时，调用[Fixpipe](../../../基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)接口完成L0C Buffer（CO1）上计算结果的搬运，然后释放申请的L0C Buffer（CO1）内存。该方式下，用户可以灵活控制计算和搬运的节奏，根据实际需要，一次计算对应一次结果的搬出，或者将多次计算结果缓存在L0C Buffer（CO1）内存中，再一次性搬出计算结果。

        在此种调用方式下，创建Matmul对象时，必须定义C矩阵的内存逻辑位置为TPosition::CO1、数据排布格式为CubeFormat::NZ、数据类型为float或int32\_t。

          <!-- npu="950" id87 -->
        - Ascend 950PR/Ascend 950DT暂不支持该方式。
          <!-- end id87 -->
          <!-- npu="310p" id88 -->
        - Atlas 推理系列产品AI Core暂不支持该方式。
          <!-- end id88 -->
          <!-- npu="310b" id89 -->
        - Atlas 200I/500 A2 推理产品暂不支持该方式。
          <!-- end id89 -->

        ```
        // 定义C矩阵的类型信息
        typedef AscendC::MatmulType<AscendC::TPosition::CO1, CubeFormat::NZ, float> cType;
        // 创建Matmul对象
        AscendC::Matmul<aType, bType, cType, biasType> mm;

        // 用户提前申请L0C Buffer（CO1）的内存l0cTensor
        TQue<TPosition::CO1, 1> CO1_;
        // 128 * 1024为申请的L0C Buffer（CO1）内存大小
        GetTPipePtr()->InitBuffer(CO1_, 1, 128 * 1024);
        // L0cT为C矩阵的数据类型。
        // A矩阵数据类型是int8_t或int4b_t时，C矩阵的数据类型是int32_t。
        // A矩阵数据类型是half、float或bfloat16_t时，C矩阵的数据类型是float。
        LocalTensor<L0cT> l0cTensor = CO1_.template AllocTensor<L0cT>();

        // 将l0cTensor作为入参传入Iterate，矩阵乘结果输出到用户申请的l0cTensor上
        mm.Iterate(false, l0cTensor);

        // 调用Fixpipe接口将L0C Buffer（CO1）上的计算结果搬运到GM
        FixpipeParamsV220 params;
        params.nSize = nSize;
        params.mSize = mSize;
        params.srcStride = srcStride;
        params.dstStride = dstStride;
        CO1_.EnQue(l0cTensor);
        CO1_.template DeQue<L0cT>();
        Fixpipe<cType, L0cT, CFG_ROW_MAJOR>(gm[dstOffset], l0cTensor, params);

        // 释放L0C Buffer（CO1）内存
        CO1_.FreeTensor(l0cTensor);
        ```

5.  结束矩阵乘操作。

    ```
    mm.End();
    ```

**表3**  CubeFormat::NZ格式的矩阵对齐要求

<a name="table98851538118"></a>
| 源/目的操作数 | 外轴 | 内轴 |
| --- | --- | --- |
| A矩阵/B矩阵 | 16的倍数 | C0_size的倍数 |
| C矩阵 | 16的倍数 | 16的倍数 |
| C矩阵（开启channel_split功能） | 16的倍数 | C0_size的倍数 |
| C矩阵（不开启channel_split功能） | 16的倍数 | float/int32_t：16的倍数<br><br>half/bfloat16_t/int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t：C0_size的倍数 |

对于上表中相关参数和概念的补充说明如下：

-   float/int32\_t数据类型的C0\_size为8，half/bfloat16\_t数据类型的C0\_size为16，int8\_t/fp8\_e4m3fn\_t/fp8\_e5m2\_t/hifloat8\_t数据类型的C0\_size为32，int4b\_t/fp4x2\_e2m1\_t/fp4x2\_e1m2\_t数据类型的C0\_size为64。
-   channel\_split功能通过[MatmulConfig](MatmulConfig.md#table1761013213153)中的isEnableChannelSplit参数配置，具体内容请参考[MatmulConfig](MatmulConfig.md#table1761013213153)。

## 需要包含的头文件

```
#include "lib/matmul/matmul_intf.h"
```

## 实现原理

以输入矩阵A \(GM, ND, half\)、矩阵B\(GM, ND, half\)，输出矩阵C \(GM, ND, float\)，无Bias场景为例，其中\(GM, ND, half\)表示数据存放在GM上，数据格式为ND，数据类型为half，描述Matmul高阶API典型场景的内部算法框图，如下图所示。

**图2**  Matmul算法框图  
![](../../../../figures/Matmul算法框图.png "Matmul算法框图")

计算过程分为如下几步：

1.  数据从GM搬到L1 Buffer（A1）：DataCopy每次从矩阵A中搬出一个stepM\*baseM\*stepKa\*baseK的矩阵块a1，循环多次完成矩阵A的搬运；数据从GM搬到L1 Buffer（B1）：DataCopy每次从矩阵B中搬出一个stepKb\*baseK\*stepN\*baseN的矩阵块b1，循环多次完成矩阵B的搬运；
2.  数据从L1 Buffer（A1）搬到L0A Buffer（A2）：LoadData每次从矩阵块a1，搬出一个baseM \* baseK的矩阵块a0；数据从L1 Buffer（B1）搬到L0B Buffer（B2），并完成转置：LoadData每次从矩阵块b1，搬出一个baseK \* baseN的矩阵块，并将其转置为baseN \* baseK的矩阵块b0；
3.  矩阵乘：每次完成一个矩阵块a0 \* b0的计算，得到baseM \* baseN的矩阵块co1；
4.  数据从矩阵块co1搬到矩阵块co2:DataCopy每次搬运一块baseM \* baseN的矩阵块co1到singleCoreM \* singleCoreN的矩阵块co2中；
5.  重复2-4步骤，完成矩阵块a1 \* b1的计算；
6.  数据从矩阵块co2搬到矩阵块C：DataCopy每次搬运一块singleCoreM \* singleCoreN的矩阵块co2到矩阵块C中；
7.  重复1-6步骤，完成矩阵A \* B = C的计算。
