# Matmul高阶API开启IBShare模板共享A和B矩阵数据<a name="ZH-CN_TOPIC_0000002088668238"></a>

## 案例介绍<a name="section144912211504"></a>

本案例呈现了在融合算子场景中，使用Matmul高阶API进行矩阵乘法计算时，A矩阵和B矩阵同时启用IBShare对性能的提升效果。

该案例的关键优化措施包括：

-   分核逻辑：以Cube核视角分核，Matmul计算结果输出到GM，提供给Vector核进行后续计算。
-   开启IBShare：A矩阵和B矩阵同时开启IBShare。

本案例的算子规格如下：

**表1**  算子规格

<a name="table568792363119"></a>
<table><thead align="left"><tr id="row1368792319318"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p186887235312"><a name="p186887235312"></a><a name="p186887235312"></a>输入</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1268862303114"><a name="p1268862303114"></a><a name="p1268862303114"></a>Shape</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1168820237317"><a name="p1168820237317"></a><a name="p1168820237317"></a>Data type</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p66882235318"><a name="p66882235318"></a><a name="p66882235318"></a>Format</p>
</th>
</tr>
</thead>
<tbody><tr id="row1688142363117"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p186887235314"><a name="p186887235314"></a><a name="p186887235314"></a>x</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p7688152310317"><a name="p7688152310317"></a><a name="p7688152310317"></a>128,384</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p3688142393114"><a name="p3688142393114"></a><a name="p3688142393114"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p7688123143119"><a name="p7688123143119"></a><a name="p7688123143119"></a>ND</p>
</td>
</tr>
<tr id="row2688182315313"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p268817236313"><a name="p268817236313"></a><a name="p268817236313"></a>y</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p15688182363114"><a name="p15688182363114"></a><a name="p15688182363114"></a>384,256</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p86885235318"><a name="p86885235318"></a><a name="p86885235318"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p20688132373115"><a name="p20688132373115"></a><a name="p20688132373115"></a>ND</p>
</td>
</tr>
</tbody>
</table>

开启IBShare和未开启IBShare的完整样例请参考[A、B矩阵均开启IBShare样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_ibshareAB)。

## 获取性能数据<a name="section4647105095111"></a>

使用msOpProf工具获取算子的Profiling的数据，重点分析MTE2，Cube，Scalar的流水情况。

## 分析主要瓶颈点<a name="section371410542511"></a>

**图1**  优化前Profiling数据<a name="fig516161474220"></a>  
![](../../../figures/prof_before_86.png "优化前Profiling数据-86")

通过分析以上Profiling数据可以看出，算子执行多次的平均耗时为27.11us，aic\_scalar\_time的平均耗时为26.27us，当前性能瓶颈点为Cube的Scalar流水。

## 设计优化方案<a name="section7611135813517"></a>

A矩阵和B矩阵均未开启IBShare时，数据需要根据K轴、M轴或N轴进行切分计算。这里以K轴切分为例，未开启IBShare之前，算子以AIV Block为视角进行tiling切分，AIV0发起A0\*B0的计算，AIV1发起A1\*B1的计算。

**图2**  未开启IBShare<a name="fig16885185245110"></a>  
![](../../../figures/no_ibshare.png "未开启IBShare")

当A矩阵和B矩阵都启用IBShare时，可以一次性加载到L1 Buffer上，省去了切分，分开搬运的过程，同时Cube计算单元完全由AIV0单核驱动，发起一次计算，计算的结果由AIV0和AIV1共享，从而减少Cube响应的次数，减少Scalar计算。

**图3**  开启IBShare<a name="fig103191116"></a>  

![](../../../figures/mm_flow_87.png)

开启IBShare和不开启IBShare的数据交互对比示意图如下：

![](../../../figures/NoABshare.png)

通过设置A和B矩阵MatmulType的IBShare均为true，开启该优化，具体代码如下：

```
constexpr bool isABshare = true;
template <typename aType, typename bType, typename cType> class MatmulABshareKernel {
public:
    __aicore__ inline MatmulABshareKernel(){};
    __aicore__ inline void Init(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* c, __gm__ uint8_t* workspace,
                                const TCubeTiling &tiling, AscendC::TPipe *pipe);
    __aicore__ inline void Process(AscendC::TPipe *pipe);
    __aicore__ inline void CalcOffset(int32_t blockIdx, const TCubeTiling &tiling, int32_t &offsetA, int32_t &offsetB,
                                      int32_t &offsetC);
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType, false, LayoutMode::NONE, isABshare>, 
           AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType, false, LayoutMode::NONE, isABshare>,
           AscendC::MatmulType<AscendC::TPosition::VECIN, CubeFormat::ND, cType>>
        matmulObj;
    AscendC::GlobalTensor<aType> aGlobal;
    AscendC::GlobalTensor<bType> bGlobal;
    AscendC::GlobalTensor<cType> cGlobal;
    TCubeTiling tiling;
};
template <typename aType, typename bType, typename cType>
__aicore__ inline void MatmulABshareKernel<aType, bType, cType>::Init(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* c, 
                                                                __gm__ uint8_t* workspace,const TCubeTiling &tiling, AscendC::TPipe *pipe)
{
    this->tiling = tiling;
    aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ aType *>(a), tiling.M * tiling.Ka);
    bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ bType *>(b), tiling.Kb * tiling.N);
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ cType *>(c), tiling.M * tiling.N);
    int32_t offsetA, offsetB, offsetC;
    CalcOffset(AscendC::GetBlockIdx(), tiling, offsetA, offsetB, offsetC); // calculate offset
    aGlobal = aGlobal[offsetA];
    bGlobal = bGlobal[offsetB];
    cGlobal = cGlobal[offsetC];
}
template <typename aType, typename bType, typename cType>
__aicore__ inline void
MatmulABshareKernel<aType, bType, cType>::CalcOffset(int32_t blockIdx, const TCubeTiling &tiling,
                                                             int32_t &offsetA, int32_t &offsetB, int32_t &offsetC)
{
    offsetA = 0;
    offsetB = 0;
    offsetC = 0;
}
```

## 验证优化方案性能收益<a name="section8934151165215"></a>

优化后执行多次的平均耗时：22.44us，较优化前有较大提升。

**图4**  优化后Profiling数据<a name="fig1865995314535"></a>  
![](../../../figures/prof_after.png "优化后Profiling数据")

## 总结<a name="section15200958526"></a>

融合算子场景下，Matmul A矩阵和B矩阵同时开启IBShare，以Cube核视角分核，可以有效减少Cube侧的Scalar开销，提升性能。
