# 矩阵乘输出的N方向对齐<a name="ZH-CN_TOPIC_0000002532228169"></a>

## 功能介绍<a name="zh-cn_topic_0000002265055666_section310824820358"></a>

矩阵乘输出的N方向对齐，即矩阵乘结果C矩阵按ND\_ALIGN格式输出。在Matmul矩阵乘法中，常用的矩阵数据格式有ND、NZ，相关介绍可参考[数据格式](../基础知识.md#zh-cn_topic_0000001622194138_section1453415011)章节。ND\_ALIGN是矩阵的另一种数据格式，该格式一般用于N方向非32字节对齐的矩阵乘计算中，配置结果C矩阵为ND\_ALIGN格式后，将按照N方向32字节对齐的补齐规则输出C矩阵，详细内容请见[ND\_ALIGN](../../../../技术附录/概念原理和术语/神经网络和算子/数据排布格式.md#li075920427155)。

以M=16，K=16，N=14，A、B矩阵数据类型为half的Matmul为具体示例，说明ND\_ALIGN输出功能。当配置C矩阵为ND格式并输出到Global Memory时，按照原始N方向大小非32字节对齐输出如[图1](#zh-cn_topic_0000002265055666_fig2311855162511)所示。当配置C矩阵为ND格式时，按照N方向32字节对齐输出如[图2](#zh-cn_topic_0000002265055666_fig25741012182719)所示，C矩阵的N方向最后两列由下一行的实际数据进行填充补齐，以实现N方向对齐到32字节并输出。当配置C矩阵为ND\_ALIGN格式时，Matmul API会在C矩阵的N方向上通过添加无效数据来填充最后两列，以确保N方向对齐至32字节并输出，如[图3](#zh-cn_topic_0000002265055666_fig4840114152818)所示。

**图1**  ND格式C矩阵N方向非32字节对齐示意图<a name="zh-cn_topic_0000002265055666_fig2311855162511"></a>  
![](../../../../figures/nd_no_32.png "ND格式C矩阵N方向非32字节对齐示意图")

**图2**  ND格式C矩阵N方向32字节对齐示意图<a name="zh-cn_topic_0000002265055666_fig25741012182719"></a>  
![](../../../../figures/nd_32_align.png "ND格式C矩阵N方向32字节对齐示意图")

**图3**  ND\_ALIGN格式C矩阵N方向32字节对齐示意图<a name="zh-cn_topic_0000002265055666_fig4840114152818"></a>  
![](../../../../figures/nd_align_32.png "ND_ALIGN格式C矩阵N方向32字节对齐示意图")

## 使用场景<a name="zh-cn_topic_0000002265055666_section118051016163613"></a>

Matmul计算中N方向非32字节对齐，输出C矩阵的N方向要求32字节对齐的场景。

## 约束说明<a name="zh-cn_topic_0000002265055666_section14160134220363"></a>

若配置C矩阵为ND\_ALIGN格式输出，则为C矩阵申请的Buffer空间为N向上32字节对齐后的空间大小。

## 调用示例<a name="zh-cn_topic_0000002265055666_section15486294368"></a>

完整的算子样例请参考[matmul\_nd\_align算子样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_format_nd_align)。

-   Tiling实现

    调用SetCType接口，设置C矩阵的数据格式为CubeFormat::ND\_ALIGN，其它Tiling实现与[基础场景](../算子实现.md)相同。

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform); 
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16); 
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);  
    // 设置C矩阵，buffer位置为GM，数据格式为ND_ALIGN
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND_ALIGN, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(AscendC::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ... // 其他实现内容
    optiling::TCubeTiling tilingData;   
    int ret = tiling.GetTiling(tilingData);
    ```

-   Kernel实现

    相较于[基础场景](../算子实现.md#zh-cn_topic_0000001622514006_li1032116474330)，ND\_ALIGN输出功能要求在创建Matmul对象时，设置模板参数cType的数据格式为CubeFormat::ND\_ALIGN。

    ```
    #include "lib/matmul_intf.h"
    
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
    // 设置模板参数cType的数据格式为ND_ALIGN
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND_ALIGN, float> cType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType; 
    AscendC::Matmul<aType, bType, cType, biasType> mm; 
    ```
