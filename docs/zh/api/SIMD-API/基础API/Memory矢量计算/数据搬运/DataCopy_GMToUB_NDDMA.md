# DataCopy（GMToUB多维数据搬运NDDMA）<a name="ZH-CN_TOPIC_0000002544407954"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas 训练系列产品：不支持
<!-- end id15 -->

## 功能说明<a name="section474617392321"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

多维数据搬运接口，相比于基础数据搬运接口，可更加自由配置搬入的维度信息以及对应的Stride。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN

## 函数原型<a name="section1954364615315"></a>

- Global Memory -> Unified Buffer，支持多维度搬运

    ```cpp
    template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const NdDmaParams<T, dim>& params)
    ```

- NDDMA Cache刷新，在使用DataCopy接口进行数据搬运前，需要使用NdDmaDci接口刷新NDDMA Cache缓存保证为最新状态。

    ```cpp
    __aicore__ inline void NdDmaDci()
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 源操作数或者目的操作数的数据类型。 |
| dim | 搬运的数据维度，数据类型为uint8_t，支持的维度为[1, 5]。 |
| config | 搬运配置选项，NdDmaConfig类型，定义如下，具体参数说明请参考[表3](#table_nddma_3)。<br><pre>struct NdDmaConfig {<br>    static constexpr uint16_t unsetPad = 0xffff;<br>    bool isNearestValueMode = false;<br>    uint16_t loopLpSize = unsetPad; // Left padding size of all dimensions, must be less than 256.<br>    uint16_t loopRpSize = unsetPad; // Right padding size of all dimensions, must be less than 256.<br>    bool ascOptimize = false;       // used for Ascend C optimization on special scenario.<br>};</pre> |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数，类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)。 |
| src | 输入 | 源操作数，类型为[GlobalTensor](../../数据结构/GlobalTensor/GlobalTensor简介.md)。 |
| params | 输入 | 搬运参数NdDmaParams类型，定义如下，具体参数说明请参考[表4](#table_nddma_4)。<br><pre>template &lt;typename T, uint8_t dim&gt;<br>struct NdDmaParams  {<br>    NdDmaLoopInfo&lt;dim&gt; loopInfo;<br>    T constantValue;  // 若有左右Padding，且不使能NearestValueMode时，该值将作为Padding值填充。<br>};</pre><br>NdDmaLoopInfo类型，定义如下，具体参数说明请参考[表5](#table_nddma_5)。<br><pre>template &lt;uint8_t dim&gt;<br>struct NdDmaLoopInfo  {<br>    uint64_t loopSrcStride[dim] = {0}; // src stride info per loop.<br>    uint32_t loopDstStride[dim] = {0}; // dst stride info per loop.<br>    uint32_t loopSize[dim] = {0}; // Loop size per loop.<br>    uint8_t loopLpSize[dim] = {0}; // Left padding size per loop.<br>    uint8_t loopRpSize[dim] = {0}; // Right padding size per loop.<br>};<br>// 注意：dim的有效范围为[1,5]。</pre> |

**表3**  NdDmaConfig结构体参数定义<a name="table_nddma_3"></a>

| 参数名 | 描述 |
| :--- | :--- |
| unsetPad | 表示不设置PaddingSize，固定为0xFFFF。 |
| isNearestValueMode | 表示Padding值填取方式，类型为bool。<br>True：使能最近值填充方式，即左右Padding值会选取当前维度最左或最右的值进行填充，可参考[图1](#fig10722115123919)。<br>False：使能常数填充方式，即所有Padding值填充为固定值NdDmaParams::constantValue。<br>当数据类型为b64时，参数isNearestValueMode的值应为False。 |
| loopLpSize | 表示每个维度内的PaddingSize，当该值不为unsetPad时，则表示所有循环里的左PaddingSize为该值，且会使NdDmaLoopInfo::loopLpSize不生效。默认值为unsetPad，开发者可填的范围为默认值或[0,255]。 |
| loopRpSize | 表示每个维度内的PaddingSize，当该值不为unsetPad时，则表示所有循环里的右PaddingSize为该值，且会使NdDmaLoopInfo::loopRpSize不生效。默认值为unsetPad，开发者可填的范围为默认值或[0,255]。 |
| ascOptimize | 预留参数，暂不支持。 |

**表4**  NdDmaParams结构体参数定义<a name="table_nddma_4"></a>

| 参数名 | 描述 |
| :--- | :--- |
| loopInfo | 每维进行搬运的信息，类型为NdDmaLoopInfo&lt;dim&gt;。<br>NdDmaLoopInfo结构中数组类型的参数，其数组索引值对应实际维度信息，索引0 - 4对应1 - 5维。具体参数介绍可参考[表5](#table_nddma_5)。 |
| constantValue | 数据类型为T的数值，当存在维度左右Padding，且不使能NearestValueMode时，该值将作为Padding值填充。<br>当数据类型为b64时，参数constantValue的值应为0。 |

**表5**  NdDmaLoopInfo结构体参数定义<a name="table_nddma_5"></a>

| 参数名 | 描述 |
| :--- | :--- |
| loopSrcStride | 表示每个维度内，该源操作数元素与下一个元素间的间隔。<br>单位为元素个数。数据类型为uint64_t，loopSrcStride需在[0, 2<sup>40</sup>)。 |
| loopDstStride | 表示每个维度内，该目的操作数元素与下一个元素间的间隔。<br>单位为元素个数。数据类型为uint32_t，loopDstStride需在[0, 2<sup>20</sup>)。 |
| loopSize | 表示每个维度内，处理的元素个数（不包含Padding元素）。<br>单位为元素个数。数据类型为uint32_t，loopSize需在[0, 2<sup>20</sup>)。 |
| loopLpSize | 表示每个维度内，左侧需要补齐的元素个数。<br>单位为元素个数。数据类型为uint8_t，loopLpSize不要超出该数据类型的取值范围：[0, 255]。 |
| loopRpSize | 表示每个维度内，右侧需要补齐的元素个数。<br>单位为元素个数。数据类型为uint8_t，loopRpSize不要超出该数据类型的取值范围：[0, 255]。 |

以下以2维的例子介绍几个典型使用场景。

**图1**  2D Padding场景<a name="fig10722115123919"></a>  
![](../../../../figures/2d_padding_scenario.png "2D-Padding场景")

**图2**  2D Transpose场景<a name="fig104451143104510"></a>  
![](../../../../figures/2d_transpose_scenario.png "2D-Transpose场景")

**图3**  2D BroadCast场景<a name="fig4654326132913"></a>  
![](../../../../figures/2d_broadcast_scenario.png "2D-BroadCast场景")

**图4**  2D Slice场景<a name="fig13061435132915"></a>  
![](../../../../figures/2d_slice_scenario.png "2D-Slice场景")

## 数据类型

源操作数和目的操作数支持的数据类型保持一致。

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。
<!-- end id1 -->

## 返回值说明<a name="section7607175220218"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 一条指令所能获取的所有数据的地址范围宽度不能超过40位（1TB），即：

    源操作数的每一次循环的大小为：\(loopLpSize + loopSize + loopRpSize -1 \) \* loopSrcSize，目的操作数的每一次循环的大小为：\(loopLpSize + loopSize + loopRpSize -1 \) \* loopDstSize，所有的循环的大小加起来不超过2的40次方位。

- 当每层循环的dstStride为升序序列，则不同循环间的地址空间不能交织或者重叠。以一个2D Padding场景为例，loopSrcStride、loopDstStride第二个维度的stride值最小是3，数据3不能落在维度1的循环中。

    ![](../../../../figures/nddma_padding_2d.png)

- 该接口通过NDDMA进行数据搬运，对应的NDDMA Cache大小为32KB，在使用DataCopy接口进行数据搬运前，需要使用NdDmaDci接口刷新缓存，否则多核场景下读写同一块Global Memory地址可能会导致部分核读取数据错误。

## 调用示例<a name="section122101199486"></a>

- 场景1：2D Padding场景，常数填充方式

    ```cpp
    // xGmShape：[16, 32]，搬运8列2行数据，左Padding 15，上Padding 13，右Padding 17，下Padding 3，xLocalShape：[32, 64]。
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 32}, {1, 64}, {32, 16}, {15, 13}, {17, 3}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};  // padding的值为0
    AscendC::NdDmaDci();  // 刷新cache
    static constexpr AscendC::NdDmaConfig dmaConfig;  // 使用默认参数，也可以不传。
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- 场景2：2D Padding场景，最近值填充方式

    ```cpp
    // xGmShape：[28, 15]，搬运3列4行数据，左Padding 11，上Padding 3，右Padding 6，下Padding 1，xLocalShape：[32, 32]。
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 15}, {1, 32}, {15, 28}, {11, 3}, {6, 1}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // 刷新cache
    // isNearestValueMode = true左右Padding值会选取当前维度最左或最右的值进行填充。
    static constexpr AscendC::NdDmaConfig dmaConfig = {true};
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- 场景3：2D Transpose场景

    ```cpp
    // xGmShape：[16, 64]，搬运8列4行数据，xLocalShape：[64, 16]。
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 64}, {16, 1}, {64, 16}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // 刷新cache
    static constexpr AscendC::NdDmaConfig dmaConfig;  // 使用默认参数，也可以不传。
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- 场景4：2D BroadCast场景

    ```cpp
    // xGmShape：[1, 16]，搬运16列1行数据并进行广播，xLocalShape：[3, 16]。
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 0}, {1, 16}, {16, 3}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // 刷新cache
    static constexpr AscendC::NdDmaConfig dmaConfig;  // 使用默认参数，也可以不传。
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

- 场景5：2D Slice场景

    ```cpp
    // xGmShape：[32, 64]，将16*10的矩阵截取为一个5*7的矩阵，xLocalShape：[16, 16]。
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 64}, {1, 16}, {16, 16}, {0, 0}, {0, 0}};
    AscendC::NdDmaParams<T, 2> params{loopInfo, 0};
    AscendC::NdDmaDci();  // 刷新cache
    static constexpr AscendC::NdDmaConfig dmaConfig;  // 使用默认参数，也可以不传。
    AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
    ```

完整样例请参考：[多维数据搬运样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_nddma)。
