# 辅助矩阵分形格式详解<a name="ZH-CN_TOPIC_0000002538231118"></a>

<!-- npu="950" id1 -->
## MX系数矩阵<a name="zh-cn_topic_0000002563445163_section648585413300"></a>

针对Ascend 950PR/Ascend 950DT产品：

浮点数在科学计算、图像处理、神经网络等领域应用广泛。以AI训练为例，现有的浮点数格式或数值范围不足，或精度不高，这影响了模型的收敛速度和性能。如果要同时满足数值范围和精度的要求，将会导致内存占用过大，从而增加数据存储和传输的成本。基于此种情况，业内提出了一种新的浮点数格式——微缩放（Microscaling，MX）格式。MX格式的浮点数可以支持更低比特位宽的AI训练和推理，并且占用的内存更少。符合MX标准的数据格式在使用8位或更低比特位的情况下，能够实现稳健的AI训练和推理模型精度。

MX格式是一种块数据格式，若干个数据可以组成一个块（或者一个组），数据以块为单位。MX格式的数据由三部分构成：

- 共享缩放因子X，位宽为w bits；
- 私有元素P<sub>i</sub>，位宽为d bits；
- 块大小k，表示多少个低比特数据形成一个块；

所有k个元素P<sub>i</sub>有相同的位宽和数据类型，并且共享一个缩放因子X，每个包含k个元素的块可以使用（w+k\*d）位进行编码。元素的数据类型和缩放因子可以独立选择。

下图为MX格式的浮点数的数据结构，S、E和M分别用于表示浮点数的符号、指数和尾数字段的值。其中，共享缩放因子X是一个用于整个数据块的缩放比例因子，它决定了数据块中所有元素的动态范围。通过引入共享缩放因子，MX格式的数据能够在保持低位宽的同时，灵活地表示不同范围的数据。块大小k指的是组成一个数据块（或组）的低比特数据的数量。私有元素P<sub>i</sub>是指数据块中的每个低比特数据元素。这些元素经过缩放因子X的调整后，共同表示了一个高精度的浮点数或整数。

**图1** MX格式组成示意图<a name="zh-cn_topic_0000002563445163_fig3385175571011"></a>  
![](../../../../figures/mx_format.png "MX格式组成示意图")

MXMmad为带有量化系数X的矩阵乘法，即左矩阵和右矩阵均有对应的量化系数矩阵，左量化系数矩阵scaleA和右量化系数矩阵scaleB。计算公式为：C = \(scaleA ⊗ A\) \* \(scaleB ⊗ B\) + Bias，“⊗”表示广播乘法，左/右矩阵与左/右量化系数矩阵做乘积时，K方向上每32个元素共享一个量化因子。如下图所示，其中A为左矩阵，形状为\[M, K\]；scaleA为左量化系数矩阵，形状为\[M, K/32\]；B为右矩阵，形状为\[K, N\]；scaleB为右量化系数矩阵，形状为\[K/32, N\]。

**图2** MX矩阵乘示意图<a name="zh-cn_topic_0000002563445163_fig33303533712"></a>  
![](../../../../figures/mx_matrix_computation_demo.png "MX矩阵乘示意图")

- **MX scaleA系数矩阵分形介绍**<a name="section_mx_scalea_fractal_format"></a>

    - **物理位置：** Global Memory/L1 Buffer/Unified Buffer：保存离线/在线生成左矩阵量化系数数据。

    - **设计原理：** MX矩阵乘法中A矩阵与scaleA矩阵物理地址上一一映射，LoadData（MX矩阵搬运）接口要求在L1 Buffer中scaleA矩阵满足行读取需求，因此在L1 Buffer上scaleA矩阵为按行存储的小z大Z排布（Zz）。

    - **格式转换过程：** 假设原始A矩阵大小为（M, K），则对应的scaleA矩阵大小为（M, K/32），scaleA矩阵被分为M1 \* K1个分形，按照row major（行主序）排布，形状如Z字形；每个分形内部有M0 \* K0个元素，按照row major（行主序）排布，形状如Z字形，所以这种数据格式称为Zz格式。其中，\(M0, K0\)表示一个分形的shape，其分形需要与L0A_MX Buffer物理内存排布保持一致，且数据类型必须为fp8_e8m0_t，shape为16x2。

        通过公式表达转换过程如下：

        ```py
        (…, B, M, K/32)->pad->(…, B, M1 * M0, K1 * K0)->reshape->(…, B, M1, M0, K1, K0)->transpose->(…, B, M1, K1, M0, K0)
        ```

    - **分形大小约束：** 数据类型为fp8\_e8m0\_t，分形大小为16x2（M0，K0）

    如下图所示，若GM上scaleA矩阵为ND排布，可以通过ND2NZ.b16/DN2NZ.b16指令搬运到L1 Buffer，确保L1 Buffer上分形为Zz排布，满足LoadData（MX矩阵搬运）接口要求。

    **图3** scaleA矩阵在不同位置上的排布格式<a name="zh-cn_topic_0000002563445163_fig6871154411446"></a>  
    ![](../../../../figures/mx_scaleA_format.png "scaleA矩阵在不同位置上的排布格式")

- **MX scaleB系数矩阵分形介绍**<a name="section_mx_scaleb_fractal_format"></a>

    - **物理位置：** Global Memory/L1 Buffer/Unified Buffer：保存离线/在线生成右矩阵量化系数数据。

    - **设计原理：** MX矩阵乘法中B矩阵与scaleB矩阵物理地址上一一映射，LoadData（MX矩阵搬运）接口要求在L1 Buffer上scaleB矩阵满足列读取需求，因此在L1 Buffer上scaleB矩阵为按列存储的大N小n排布（Nn）。

    - **格式转换过程：** 假设原始B矩阵大小为（K, N），则对应的scaleB矩阵大小为（K/32, N），scaleB矩阵被分为K1 \* N1个分形，按照column major（列主序）排布，形状如N字形；每个分形内部有K0 \* N0个元素，按照column major（列主序）排布，形状如n字形，所以这种数据格式称为Nn格式。其中，（K0, N0）表示一个分形的shape，其分形需要与L0B_MX Buffer物理内存排布保持一致，且数据类型必须为fp8_e8m0_t，shape为2x16。

        通过公式表达转换过程如下：

        ```py
        (…, B, K/32, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, N1, K1, N0, K0)
        ```

    - **分形大小约束：** 数据类型为fp8\_e8m0\_t，分形大小为2x16（K0，N0）

    如下图所示，若Global Memory（GM）上scaleB矩阵为ND排布，可以通过ND2NZ.b16/DN2NZ.b16指令搬运到L1 Buffer，确保L1 Buffer上分形为Nn排布，满足LoadData（MX矩阵搬运）接口要求。

    **图4** scaleB矩阵在不同位置上的排布格式<a name="zh-cn_topic_0000002563445163_fig599218260112"></a>  
    ![](../../../../figures/mx_scaleB_format.png "scaleB矩阵在不同位置上的排布格式")

具体实现可参考：[MXmmad样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dmx_l12l0)。
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
## 4选2稀疏索引矩阵<a name="zh-cn_topic_0000002563445163_section1873692415233"></a>

针对Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品：

- 定义：原始B矩阵稠密化过程中生成的索引矩阵。假设原始B矩阵每4个元素有至少2个零元素，稠密化的B矩阵是过滤掉2个零的稠密矩阵。
- 场景：通常存在于Global Memory（GM）中。在进行sparse矩阵运算前，通常需要离线生成并将其转换为大Z小n格式（Zn）以适配Cube单元。
- 生成过程：如下图所示，对于原始矩阵B中的每4个元素，将在index矩阵中生成2个2位索引，并按照以下规则进行编码。索引必须在\{0, 1, 2\}范围内。
**图5** index生成示意图<a name="zh-cn_topic_0000002563445163_fig1423214854720"></a>  
![](../../../../figures/index生成示意图.png "index生成示意图")
    - 第一个索引用于指示前3个元素中第1个非零元素的相对位置。
    - 第二个索引用于指示第2个非零元素在后3个元素中的相对位置。
    - 索引矩阵中的一个uint8的元素是由4个uint2\_t组合而成的，例如上图：生成的索引数据为1 2 0 1 ，索引数据在Global Memory（GM）地址中的排布从高位到低位为1 0 2 1 ，其中1 0 2 1（对应索引矩阵前四位1 2 0 1）为一个uint8\_t。

**图6** 稠密化B矩阵示意图<a name="zh-cn_topic_0000002563445163_fig1115556103916"></a>  
![](../../../../figures/density_B_matrix_demo.png "稠密化B矩阵示意图")

稠密化算法规则具体可参考下表。其中，“-”表示不关心该位置上的值，因为其后续Sparse Mmad计算过程中会被过滤。

<table><thead>
<tr><th>示例</th><th>B矩阵元素0</th><th>B矩阵元素1</th><th>B矩阵元素2</th><th>B矩阵元素3</th><th>索引数据0</th><th>索引数据1</th>
</tr></thead><tbody>
<tr><td rowspan="6">四个元素里面有两个非零元素</td>
<td>0</td><td>0</td><td>X</td><td>Y</td><td>2'b10</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>Y</td><td>2'b01</td><td>2'b10</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>Y</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>X</td><td>Y</td><td>-</td><td>2'b01</td><td>2'b01</td></tr>
<tr><td>X</td><td>0</td><td>Y</td><td>-</td><td>2'b00</td><td>2'b01</td></tr>
<tr><td>X</td><td>Y</td><td>-</td><td>-</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td rowspan="4">四个元素里面有一个非零元素</td>
<td>0</td><td>0</td><td>0</td><td>X</td><td>2'b00</td><td>2'b10</td></tr>
<tr><td>0</td><td>0</td><td>X</td><td>0</td><td>2'b10</td><td>2'b00</td></tr>
<tr><td>0</td><td>X</td><td>0</td><td>0</td><td>2'b01</td><td>2'b00</td></tr>
<tr><td>X</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td></tr>
<tr><td>四个元素里面没有非零元素</td>
<td>0</td><td>0</td><td>0</td><td>0</td><td>2'b00</td><td>2'b00</td>
</tr></tbody></table>

- **物理位置：** GM/L1 Buffer：保存离线生成好的索引矩阵数据。

- **设计原理：** 列读取需求：sparse矩阵乘法中B矩阵与Index矩阵物理地址上一一映射，LoadDataWithSparse指令要求在L1 Buffer上B矩阵与Index矩阵满足列读取需求，因此在GM/L1 Buffer上Index矩阵为按列存储的大Z小n排布（Zn）。

- **格式转换过程：** 假设原始B矩阵大小为（K, N），则经过4选2稠密化后的B矩阵与Index矩阵大小为（K/2, N），Index矩阵被分为K1 \* N1个分形，按照row major（行主序）排布，形状如Z字形；每个分形内部有N0 \* K0个元素，按照column major（列主序）排布，形状如N字形，所以这种数据格式称为Zn格式。其中，\(N0, K0\)表示一个分形的shape，其分形需要与B矩阵分形保持一致，B矩阵数据类型为int8\_t，其小分形shape为16\*32，Index矩阵分形中每个uint8\_t数据由4个uint2\_t元素拼接而成，其元素与B矩阵元素地址一一对应，因此分形shape也为16\*32，但大小为128Byte。

    通过公式表达转换过程如下：

    ```py
    (…, B, K/2, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, K1, N1, N0, K0)
    ```

- **分形大小约束：** 存储为uint8数据类型时，分形大小为16x8（N0，K0）。
<!-- end id2 -->
