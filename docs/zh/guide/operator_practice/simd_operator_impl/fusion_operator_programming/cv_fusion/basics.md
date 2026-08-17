# 基础知识<a name="ZH-CN_TOPIC_0000002500468250"></a>

>[!NOTE]说明
>学习融合算子编程之前，请确保已经掌握[矩阵编程](../../matrix_advanced_api/basics.md)相关知识。

## CV融合算子<a name="zh-cn_topic_0000001644411628_section204271251142411"></a>

融合算子由多个独立的小算子融合而成，其功能与多个小算子的功能等价，性能方面通常优于独立的小算子。用户可以根据实际业务场景诉求，按照具体算法自由融合向量（Vector）、矩阵（Cube）算子以达到性能上的收益。融合了Cube计算、Vector计算的算子统称为CV融合算子。

比如对于LLM大模型中最核心的一个融合算子Flash Attention，其核心实现如下图。图中的Matmul算子（Cube）、Scale算子（Vector）、Mask算子（Vector）、SoftMax算子（Vector）融合为一个大的算子Flash Attention。

**图1**  Flash Attention核心实现<a name="zh-cn_topic_0000001644411628_fig980125772817"></a>  
![](../../../../figures/flash_attn.png "Flash-Attention核心实现")

## 使用场景和优势<a name="zh-cn_topic_0000001644411628_section162803512544"></a>

针对有数据依赖的矢量算子和矩阵算子，可以通过融合算子编程的方式，将矢量算子和矩阵算子进行融合，通过一个算子Kernel函数来承载，由此来获得性能上的收益。下图展示了独立矢量算子和矩阵算子、Mix融合算子的执行耗时对比，由此可以看出为什么开发Mix融合算子会带来性能上的收益。

**图2**  独立矢量算子和矩阵算子、Mix融合算子的执行耗时对比<a name="zh-cn_topic_0000001644411628_fig141121413259"></a>  
![](../../../../figures/mix_fusion_time.png "独立矢量算子和矩阵算子-Mix融合算子的执行耗时对比")

-   **独立的矢量算子和矩阵算子实现**：矩阵计算后的结果需要搬运到Global Memory上，然后由Global Memory搬运到Local Memory，再进行矢量算子的计算，计算和搬运都是串行执行，另外多个算子的调度执行，会增加算子的调度耗时。
-   **融合算子的实现方法**：可以对数据进行切片，再通过流水的设计，使得矢量计算单元和矩阵计算单元实现并行计算；另外相比于不融合的单算子，减少了算子的调度耗时。

除了有效提升算子性能，充分发挥AI处理器的算力，融合算子还有如下优势：

-   减少计算量：融合算子可以将多个算子合并为一个，简化计算过程，减少计算量，提高计算效率。
-   减少内存占用：融合算子可以将多个算子的中间结果合并为一个，从而减少内存占用，提高内存利用率。
-   优化数据流：融合算子可以优化数据流，减少数据在不同算子之间的传输，从而提高数据处理效率。
-   简化代码实现：融合算子可以简化代码实现，减少代码量，提高代码可读性和可维护性。

**总之，融合算子是一种优化计算的有效手段，可以提高计算效率和内存利用率，优化数据流，简化代码实现。**

## 编程范式<a name="zh-cn_topic_0000001644411628_section18752329256"></a>

Ascend C提供**融合算子的编程范式**，方便开发者基于该范式表达融合算子的数据流，快速实现自定义的融合算子。

**融合算子数据流**指融合算子的输入输出在各存储位置间的流向。以一个典型的Cube和Vector融合算子为例，存储位置间的数据流向如下图所示：

-   Cube的输出可以作为Vector的输入：GM/UB（CO2）-\>UB（VECIN）
-   Vector的输出可以作为Cube的输入：UB（VECOUT）-\>L1 Buffer（A1）-\>L0A Buffer（A2）、UB（VECOUT）-\>L1 Buffer（B1）-\>L0B Buffer（B2）

![](../../../../figures/mat_pos_45.png)

基于Matmul高阶API的融合算子编程范式，对上述数据流简化表达如下：

**图3**  融合算子编程范式<a name="zh-cn_topic_0000001644411628_fig321783243811"></a>  
![](../../../../figures/fusion_paradigm_46.png "融合算子编程范式-46")

1.  初始化一个Matmul对象。
2.  进行Matmul内部的计算。
3.  将Matmul的计算结果搬运到Vector核上。
4.  进行Vector矢量计算。
5.  将输出结果搬运到Global Memory上。

整个过程的示例代码如下（伪代码）。完整样例请参考[MatmulLeakyRelu](../../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api)。

```
// kernel入口函数，mix场景，AIC:AIV=1:2
__global__ __mix__(1, 2) void matmul_leakyrelu_custom(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* bias,
    __gm__ uint8_t* c, __kfc_workspace__ __gm__ uint8_t* workspace, AscendC::tiling::TCubeTiling tiling)
{
    AscendC::TPipe pipe;
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    // 设置Matmul的输入（包括左矩阵、右矩阵、bias）
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);
    // 调用matmul iterate获取一块[baseM, baseN]的计算结果
    while (matmulObj.template Iterate<true>())
    {
        // GetTensorC：获取Matmul中间结果;
        // LeakyReluCompute：对Matmul中间结果执行Vector计算;
        // DataCopy：将融合计算结果搬出到GM;
    }
    matmulObj.End();
}
```
