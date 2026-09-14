# Matmul Advanced API Enabling the IBShare Template to Share Matrix A and Matrix B Data<a name="ZH-CN_TOPIC_0000002088668238"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T13:13:59.903Z -->

## Case Introduction<a name="section144912211504"></a>

This case demonstrates the performance improvement achieved by enabling IBShare for both matrix A and matrix B simultaneously when using the Matmul advanced API for matrix multiplication in a fused operator scenario.

The key optimization measures in this case include:

-   Core division logic: divide cores from the Cube core perspective, and output the Matmul computation results to GM for subsequent computation by the Vector core.
-   Enabling IBShare: enable IBShare for both matrix A and matrix B simultaneously.

The operator specifications of this case are as follows:

**Table 1** Operator specifications

<a name="table568792363119"></a>
<table><thead align="left"><tr id="row1368792319318"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p186887235312"><a name="p186887235312"></a><a name="p186887235312"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1268862303114"><a name="p1268862303114"></a><a name="p1268862303114"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1168820237317"><a name="p1168820237317"></a><a name="p1168820237317"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p66882235318"><a name="p66882235318"></a><a name="p66882235318"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row1688142363117"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p186887235314"><a name="p186887235314"></a><a name="p186887235314"></a>x</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p7688152310317"><a name="p7688152310317"></a><a name="p7688152310317"></a>128,384</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p3688142393114"><a name="p3688142393114"></a><a name="p3688142393114"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p7688123143119"><a name="p7688123143119"></a><a name="p7688123143119"></a>ND</p></td>
</tr>
<tr id="row2688182315313"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p268817236313"><a name="p268817236313"></a><a name="p268817236313"></a>y</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p15688182363114"><a name="p15688182363114"></a><a name="p15688182363114"></a>384,256</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p86885235318"><a name="p86885235318"></a><a name="p86885235318"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p20688132373115"><a name="p20688132373115"></a><a name="p20688132373115"></a>ND</p></td>
</tr>
</tbody>
</table>

For the complete examples with IBShare enabled and not enabled, see the [example with IBShare enabled for both A and B matrices](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_ibshareAB).

## Obtaining Performance Data<a name="section4647105095111"></a>

Use the msOpProf tool to obtain the operator's Profiling data, focusing on the pipeline status of MTE2, Cube, and Scalar.

## Analyzing the Main Bottleneck<a name="section371410542511"></a>

**Figure 1**  Profiling data before optimization<a name="fig516161474220"></a>  
![](../../../figures/profiling_data_before_optimization_86.png "Profiling data before optimization-86")

By analyzing the preceding profiling data, it can be seen that the average duration of multiple operator executions is 27.11us, and the average duration of aic_scalar_time is 26.27us. The current performance bottleneck lies in the Scalar pipeline of the Cube core.

## Designing an Optimization Solution<a name="section7611135813517"></a>

When IBShare is not enabled for either matrix A or matrix B, the data needs to be divided and computed along the K, M, or N axis. Taking the K-axis division as an example, before IBShare is enabled, the operator performs tiling from the AIV Block perspective. AIV0 initiates the computation of A0\*B0, and AIV1 initiates the computation of A1\*B1.

**Figure 2**  IBShare not enabled<a name="fig16885185245110"></a>  
![](../../../figures/ibshare_not_enabled.png "IBShare not enabled")

When IBShare is enabled for both matrix A and matrix B, the data can be loaded into the L1 Buffer at once, eliminating the division and separate transfer processes. Meanwhile, the Cube computation unit is driven entirely by the single core AIV0, which initiates one computation, and the computation result is shared by AIV0 and AIV1. This reduces the number of Cube responses and the Scalar computations.

**Figure 3**  IBShare enabled<a name="fig103191116"></a>  

![](../../../figures/matmul_operator_computation_flowchart_87.png)

The following figure compares the data interaction between IBShare enabled and IBShare not enabled:

![](../../../figures/NoABshare.png)

This optimization is enabled by setting the IBShare of the MatmulType of both matrix A and matrix B to true. The specific code is as follows:

```
constexpr bool isABshare = true;
template <typename aType, typename bType, typename cType> class MatmulABshareKernel {
public:
    __aicore__ inline MatmulABshareKernel(){};
    __aicore__ inline void Init(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR workspace,
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
__aicore__ inline void MatmulABshareKernel<aType, bType, cType>::Init(GM_ADDR a, GM_ADDR b, GM_ADDR c, 
                                                                GM_ADDR workspace,const TCubeTiling &tiling, AscendC::TPipe *pipe)
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

## Verifying the Performance Gains of the Optimization Solution<a name="section8934151165215"></a>

The average execution time after optimization is 22.44us, a significant improvement over the time before optimization.

**Figure 4**  Profiling data after optimization<a name="fig1865995314535"></a>  
![](../../../figures/profiling_data_after_optimization.png "Profiling data after optimization")

## Summary<a name="section15200958526"></a>

In the fused operator scenario, enabling IBShare for both the A matrix and B matrix of Matmul simultaneously and dividing cores from the Cube core perspective can effectively reduce the Scalar overhead on the Cube side and improve performance.
