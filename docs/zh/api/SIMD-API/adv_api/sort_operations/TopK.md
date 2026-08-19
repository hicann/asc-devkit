# TopK

## 产品支持情况

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas 训练系列产品：不支持
<!-- end id18 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/TopK_res.md#id1 -->

## 功能说明

获取最后一个维度的前k个最大值或最小值及其对应的索引。

如果输入是向量，则在向量中找到前k个最大值或最小值及其对应的索引；如果输入是矩阵，则沿最后一个维度计算每行中前k个最大值或最小值及其对应的索引。**本接口最多支持输入为二维数据，不支持更高维度的输入。**

如下图所示，对shape为\(4, 32\)的二维矩阵进行排序，k设置为1，输出结果为\[\[32\] \[32\] \[32\] \[32\]\]。

![](../../../figures/nz_reduce_48.png)

-   **必备概念**

    基于如上样例，我们引入一些必备概念：行数称之为**外轴长度（outter）**，每行实际的元素个数称之为**内轴的实际长度（n）**。本接口要求输入的内轴长度为32的整数倍，所以当n不是32的整数倍时，需要开发者将其向上补齐到32的整数倍，补齐后的长度称之为**内轴长度（inner）**。比如，如下的样例中，每行的实际长度n为31，不是32的整数倍，向上补齐后得到inner为32，图中的padding代表补齐操作。n和inner的关系如下：当n是32的整数倍时，inner=n；否则，inner \> n。

    ![](../../../figures/nz_reduce_49.png)

-   **接口模式**

    本接口支持两种模式：**Normal模式**和**Small模式**。Normal模式是通用模式；Small模式是为内轴长度固定为32（单位：元素个数）的场景提供的高性能模式。因为Small模式inner固定为32，可以进行更有针对性的处理，所以相关的约束较少，性能较高。内轴长度inner为32时建议使用Small模式。

-   **附加功能**：本接口支持开发者指定某些行的排序是无效排序。通过传入finishLocal参数值来控制，finishLocal对应行的值为true时，表示该行排序无效，此时排序后输出的dstIndexLocal的k个索引值会全部被置为无效索引n。

    ![](../../../figures/nz_reduce_50.png)

## 实现原理

TopK提供了两种不同的排序算法，MERGE\_SORT算法和RADIX\_SELECT算法，两种算法在执行速度、时间复杂度和算法稳定性上表现不同。MERGE\_SORT算法的时间复杂度是O\(![](../../../figures/zh-cn_formulaimage_0000002108561704.png)\)，是一种稳定的排序算法，执行速度相对较慢；RADIX\_SELECT算法的时间复杂度是O\(n\)，通常不关心稳定性，执行速度较快。

-   **MERGE\_SORT算法**

    以float类型，ND格式，shape为\[outter,  inner\]的输入Tensor为例，描述TopK高阶API内部算法框图，如下图所示。

    **图1**  TopK算法框图  
    ![](../../../figures/topk_diagram.png "TopK算法框图")

    **图2**  TopK算法框图  
    ![](../../../figures/topk_diagram_1.png "TopK算法框图-51")

    根据TopKMode不同的模式选择，可分为两个分支。

    -   计算TopK NORMAL模式，过程如下：

        1.  模板参数isInitIndex为false，需生成0到inner - 1的索引；

            <!-- npu="950" id29 -->
            Ascend 950PR/Ascend 950DT采用方式一。
            <!-- end id29 -->

            <!-- npu="A3" id30 -->
            Atlas A3 训练系列产品/Atlas A3 推理系列产品采用方式二。
            <!-- end id30 -->

            <!-- npu="910b" id31 -->
            Atlas A2 训练系列产品/Atlas A2 推理系列产品采用方式二。
            <!-- end id31 -->

            <!-- npu="310p" id32 -->
            Atlas 推理系列产品采用方式二。
            <!-- end id32 -->

            <!-- npu="x90" id3 -->
            Kirin X90采用方式一。
            <!-- end id3 -->

            <!-- npu="9030" id4 -->
            Kirin 9030采用方式一。
            <!-- end id4 -->

            -   方式一：使用CreateVecIndex生成0到inner - 1的索引。
            -   方式二：使用Arange生成0到inner - 1的索引。

        2.  isLargest参数为false，由于Sort32指令默认为降序排序，则给数据乘以-1；
        3.  对输入数据完成全排序。

            <!-- npu="950" id33 -->
            Ascend 950PR/Ascend 950DT采用方式一。
            <!-- end id33 -->

            <!-- npu="A3" id34 -->
            Atlas A3 训练系列产品/Atlas A3 推理系列产品采用方式二。
            <!-- end id34 -->

            <!-- npu="910b" id35 -->
            Atlas A2 训练系列产品/Atlas A2 推理系列产品采用方式二。
            <!-- end id35 -->

            <!-- npu="310p" id36 -->
            Atlas 推理系列产品采用方式二。
            <!-- end id36 -->

            <!-- npu="x90" id5 -->
            Kirin X90采用方式一。
            <!-- end id5 -->

            <!-- npu="9030" id6 -->
            Kirin 9030采用方式一。
            <!-- end id6 -->

            方式一：

            使用高阶API Sort对数据完成全排序。

            方式二：

            1.  使用Sort32对数据排序，保证每32个数据是有序的。
            2.  使用MrgSort指令对所有的已排序数据块归并排序。

        4.  使用GatherMask指令提取前k个数据和索引；
        5.  finishLocal\[i\]为true时，则更新该行对应的排序结果为无效索引n；
        6.  isLargest参数为false，则给数据乘以-1还原数据。

        <!-- npu="310p" id41 -->
        注意：Atlas 推理系列产品上使用ProposalConcat基础API将data和index组合起来后，再使用RpSort16基础API对数据排序；使用MrgSort4进行归并；使用ProposalExtract基础API提取data和index。
        <!-- end id41 -->

    -   计算TopK SMALL模式，过程如下：

        1.  模板参数isInitIndex为false，需生成0到inner - 1的索引，并使用Copy指令将数据复制为outter条；

            <!-- npu="950" id37 -->
            Ascend 950PR/Ascend 950DT采用方式一。
            <!-- end id37 -->

            <!-- npu="A3" id38 -->
            Atlas A3 训练系列产品/Atlas A3 推理系列产品采用方式二。
            <!-- end id38 -->

            <!-- npu="910b" id39 -->
            Atlas A2 训练系列产品/Atlas A2 推理系列产品采用方式二。
            <!-- end id39 -->

            <!-- npu="310p" id40 -->
            Atlas 推理系列产品采用方式二。
            <!-- end id40 -->

            <!-- npu="x90" id7 -->
            Kirin X90采用方式一。
            <!-- end id7 -->

            <!-- npu="9030" id8 -->
            Kirin 9030采用方式一。
            <!-- end id8 -->

            -   方式一：使用CreateVecIndex生成0到inner - 1的索引。
            -   方式二：使用Arange生成0到inner - 1的索引。

        2.  isLargest参数为false，由于Sort32指令默认为降序排序，则给输入数据乘以-1；
        3.  使用Sort32对数据排序；
        4.  使用GatherMask指令提取前k个数据和索引；
        5.  isLargest参数为false，则给输入数据乘以-1还原数据。

        <!-- npu="310p" id42 -->
        注意：Atlas 推理系列产品上使用ProposalConcat基础API将data和index组合起来后，再使用RpSort16基础API对数据排序；由于small模式下inner为32，RpSort16排序后为每16个数据有序，因此在步骤3和步骤4之间，使用MrgSort4基础API进行一次归并排序。
        <!-- end id42 -->

-   **RADIX\_SELECT算法**

    <!-- npu="950" id43 -->
    该算法仅在Ascend 950PR/Ascend 950DT上支持。
    <!-- end id43 -->

    **图3**  TopK算法框图  
    ![](../../../figures/topk_diagram_2.png "TopK算法框图-52")

    计算过程分为如下几步：

    1.  生成索引，根据不同的TopKMode，分为：
        -   TopK NORMAL模式：模板参数isInitIndex为false，使用CreateVecIndex生成0到inner-1的索引；
        -   TopK SMALL模式：模板参数isInitIndex为false，使用CreateVecIndex生成0到inner-1的索引，并使用Copy指令将数据复制为outter条；

    2.  根据模板参数选择是否对数据进行预处理。如果数据类型是浮点类型或者有符号的整数类型，需要执行twiddle in操作（将浮点数或有符号的整数转换成无符号的整数类型）。如果TopK取最小值，对无符号整数类型的数据执行取反操作；
    3.  寻找第K大的数，并保存到kValue中；
    4.  取出大于kValue的数据和索引；
    5.  取出等于kValue的数据和索引；
    6.  根据模板参数sorted选择是否对k个数进行排序，将排序数据结果存放到临时空间中；
    7.  根据模板参数，如果TopK取最小值，执行取反操作；如果数据类型是浮点类型或者有符号的整数类型，执行twiddle out操作（将无符号的整数类型转换成浮点数或有符号的整数类型）。

## 函数原型

-   API内部申请临时空间

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL, const TopKConfig& config = defaultTopKConfig>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

-   通过tmpLocal入参传入临时空间

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL, const TopKConfig& config = defaultTopKConfig>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

由于该接口的内部实现中涉及复杂的逻辑计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**API接口申请**和开发者**通过tmpLocal入参传入**两种方式。

-   API接口内部申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过tmpLocal入参传入，使用该tensor作为临时空间进行处理，API接口内部不再申请。该方式开发者可以自行管理tmpLocal内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。临时空间大小tmpLocal的BufferSize的获取方式如下：通过[TopK Tiling](TopK-Tiling.md)中提供的GetTopKMaxMinTmpSize接口获取所需最大和最小临时空间大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 待排序数据的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li197551749192811)。 |
| isInitIndex | 是否传入输入数据的索引。<br>true表示传入，设置为true时，需要通过srcIndexLocal参数传入输入数据的索引，具体规则请参考表2中的srcIndexLocal参数说明。<br>false表示不传入，TopK API输出的索引不可用。 |
| isHasfinish | Topk接口支持开发者通过finishLocal参数来指定某些行的排序是无效排序。该模板参数用于控制是否启用上述功能，true表示启用，false表示不启用。<br>Normal模式支持的取值：true / false。<br>Small模式支持的取值：false。<br>isHasfinish参数和finishLocal的配套使用方法请参考表2中的finishLocal参数说明。<!-- npu="950" id19 --><br><br>Ascend 950PR/Ascend 950DT，对于RADIX_SELECT算法，该参数为预留参数，暂未启用，为后续的功能扩展做保留，保持默认值即可。<!-- end id19 --> |
| isReuseSrc | 是否允许修改源操作数，默认值为false。<!-- npu="950" id20 --><br>Ascend 950PR/Ascend 950DT，该参数仅在输入的数据类型为float时生效，取值如下：<br>true：开发者允许源操作数被改写，可以设置该参数取值为true开启，开启后本接口内部计算时复用srcTensor的内存空间，节省部分内存空间；<br>false：本接口内部计算时不复用srcTensor的内存空间。<!-- end id20 --><!-- npu="A3" id21 --><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，该参数预留，传入默认值false即可。<!-- end id21 --><!-- npu="910b" id22 --><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，该参数预留，传入默认值false即可。<!-- end id22 --><!-- npu="310p" id23 --><br>Atlas 推理系列产品AI Core，该参数预留，传入默认值false即可。<!-- end id23 --><!-- npu="x90" id9 --><br>Kirin X90，该参数预留，传入默认值false即可。<!-- end id9 --><!-- npu="9030" id10 --><br>Kirin 9030，该参数预留，传入默认值false即可。<!-- end id10 --> |
| topkMode | Topk的模式选择，取值如下：<br>TopKMode::TOPK_NORMAL：Normal模式。<br>TopKMode::TOPK_NSMALL：Small模式。 |
| config | <!-- npu="950" id24 -->该参数仅支持Ascend 950PR/Ascend 950DT。<br><br><!-- end id24 -->TopK计算的相关配置，包括算法选择、取最大值或最小值、是否对结果排序。此参数可选配，TopKConfig类型，具体定义如下方代码所示：<br>algo：选择的排序算法。默认为MERGE_SORT算法，当前仅支持RADIX_SELECT算法，用户需要显式指定algo为TopKAlgo::RADIX_SELECT。<br>order：表示获取前k个最大值或者获取前k个最小值，取值如下：UNSET：默认值，按照函数参数isLargest的配置实现。isLargest为true时，取前k个最大值及其对应的索引，isLargest为false，取前k个最小值及其对应的索引。LARGEST：表示取前k个最大值及其对应的索引。取值为LARGEST时，函数参数isLargest的配置不生效。SMALLEST：表示取前k个最小值及其对应的索引。取值为SMALLEST时，函数参数isLargest的配置不生效。<br>sorted：表示是否对输出结果进行排序。取值为true，对输出结果进行排序；取值为false，不对输出结果进行排序。<br><br>该参数的默认值defaultTopKConfig的取值为：TopKAlgo::MERGE_SORT、TopKOrder::UNSET、true。 |

```
struct TopKConfig {
    TopKAlgo algo = TopKAlgo::MERGE_SORT;
    TopKOrder order = TopKOrder::UNSET;
    bool sorted = true;
};
enum class TopKAlgo { RADIX_SELECT, MERGE_SORT };
enum class TopKOrder { UNSET, LARGEST, SMALLEST };
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstValueLocal | 输出 | 目的操作数。用于保存排序出的k个值。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>Normal模式：<br>输出shape为outter * k_pad，即输出outter条数据，每条数据的长度是k_pad。k_pad是根据输入的数据类型将k向上32Byte对齐后的值。<br>开发者需要为dstValueLocal开辟k_pad * outter * sizeof(T)大小的空间。<br>输出每条数据的前k个值就是该条的前k个最大值/最小值。每条数据的k+1~k_pad个元素不填值，是一些随机值。<br>k_pad计算方式为：<br>当输入的srcLocal和dstValueLocal的类型是float时，float是4字节，因此将k向上取整设置为8的倍数k_pad，即可满足32Byte对齐。<br>k_pad = (k + 7) / 8 * 8<br>当输入的srcLocal和dstValueLocal的类型是half时，half是2字节，因此将k向上取整设置为16的倍数k_pad，即可满足32Byte对齐。<br>    k_pad = (k + 15) / 16 * 16<br><br>Small模式：<br>输出shape为outter * k，即输出outter条数据，每条数据的长度是k。<br>输出值需要k * outter * sizeof(T)大小的空间来进行保存。开发者要根据该大小和框架的对齐要求来为dstValueLocal分配实际的内存空间。 <br>**此处需要注意：遵循框架对内存申请的要求（开辟内存的大小满足32Byte对齐），即k * outter * sizeof(T)不是32Byte对齐时，需要向上进行32Byte对齐。为了对齐而多开辟的内存空间不填值，为一些随机值。** |
| dstIndexLocal | 输出 | 目的操作数。用于保存排序出的k个值对应的索引。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>Normal模式：<br>输出shape为outter *kpad_index，即输出outter条数据，每条数据的长度是kpad_index。kpad_index是根据输入的索引类型将k向上32Byte对齐后的值。<br>开发者需要为dstIndexLocal开辟kpad_index * outter * sizeof(int32_t)大小的空间。<br>其中每条数据的前k个值就是该条的前k个最大值/最小值对应的索引。每条数据的k+1~kpad_index个索引不填值，是一些随机值。<br>k_pad计算方式为：由于dstIndexLocal是int32_t类型，是4字节。因此将k向上取整设置为8的倍数kpad_index，即可满足32Byte对齐。<br>kpad_index = (k + 7) / 8 * 8<br><br>Small模式：<br>输出shape为outter *k，即输出outter条数据，每条数据的长度是k。<br>输出索引需要k * outter * sizeof(int32_t)大小的空间来进行保存。开发者要根据该大小和框架的对齐要求来为dstIndexLocal分配实际的内存空间。<br>**注意：遵循框架对内存开辟的要求（开辟内存的大小满足32Byte对齐），即k * outter * sizeof(int32_t)不是32Byte对齐时，需要向上进行32Byte对齐。为了对齐而多开辟的内存空间不填值，为一些随机值。** |
| srcLocal | 输入 | 源操作数。用于保存待排序的值。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<!-- npu="310p" id44 --><br>Atlas 推理系列产品AI Core上：<br>输入数据的shape为outter * inner。开发者需要为其开辟outter * inner * sizeof(T)大小的空间。<br>当n < inner时，开发者需要对srcLocal里outter条数据进行补齐操作，每条数据都需要从n补齐到inner长度。<br>补齐的规则：要求填充的数据不能影响整体排序。建议使用如下的填充方法：在取前k个最大值的时候，填充的值需要是**输入数据类型**的最小值；在取前k个最小值的时候，填充的值需要是**输入数据类型**的最大值。<!-- end id44 --> |
| srcIndexLocal | 输入 | 源操作数。用于保存待排序的值对应的索引。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>该参数和模板参数isInitIndex配合使用，isInitIndex为false时，srcIndexLocal只需进行定义，不需要赋值，将定义后的srcIndexLocal传入接口即可；isInitIndex为true时，开发者需要通过srcIndexLocal参数传入索引值。srcIndexLocal参数设置的规则如下：<br><br>Normal模式：<br>输入索引数据的shape为1 * inner，此处outter条数据都使用相同的索引。开发者需要为其开辟inner * sizeof(int32_t)大小的空间。<br>当n < inner时，开发者需要对索引数据进行补齐操作，将该条数据从n补齐到inner长度。<br>补齐的规则：要求填充的索引不能影响整体排序。建议使用如下的填充方法：填充的值在原始索引的基础上递增。例如，原始索引为0，1，2，...，n-1，填充后的索引为0，1，2，...， n ，n + 1，...，inner-1。<br><br>Small模式：<br>输入索引数据的shape为outter * inner。开发者需要为其开辟outter * inner * sizeof(int32_t)大小的空间。<br>当n < 32时，开发者需要对outter条数据进行补齐操作，每条数据都需要从n补齐到32的长度。<br>补齐的规则：要求填充的数据不能影响整体排序。建议使用如下的填充方法：填充的值在原始索引的基础上递增例如，原始索引为0，1，2，...，n-1，填充后的索引为0， 1，2，...， n ，n + 1，...，inner-1。 |
| finishLocal | 输入 | 源操作数。用于指定某些行的排序是无效排序，其shape为(outter, 1)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<!-- npu="950" id45 --><br><br>针对Ascend 950PR/Ascend 950DT，该参数为预留参数，暂未启用，为后续的功能扩展做保留，取值为false。<!-- end id45 --><br><br>该参数和模板参数isHasfinish配合使用，Normal模式下支持isHasfinish配置为true/false，Small模式下仅支持isHasfinish配置为false。<br>isHasfinish配置为true：<br>finishLocal对应的outter行的值为true时，该行排序无效，排序后输出的dstIndexLocal的k个索引值会全部被置为n。<br>finishLocal对应的outter行的值为false时，该行排序有效。<br><br>isHasfinish配置为false时，finishLocal只需进行定义，不需要赋值，将定义后的finishLocal传入接口即可。定义样例如下：`LocalTensor<bool> finishLocal;` |
| tmpLocal | 输入 | 临时空间。接口内部复杂计算时用于存储中间变量，由开发者提供，临时空间大小的获取方式请参考[TopK Tiling](TopK-Tiling.md)。数据类型固定uint8_t。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，**逻辑位置仅支持VECCALC，不支持其他逻辑位置。** |
| k | 输入 | 获取前k个最大值或最小值及其对应的索引。数据类型为int32_t。<br><br>k的大小应该满足：1 <= k <= n。 |
| tiling | 输入 | Topk计算所需Tiling信息，Tiling信息的获取请参考[TopK Tiling](TopK-Tiling.md)。 |
| topKInfo | 输入 | srcLocal的shape信息。TopKInfo类型，具体定义如下方代码所示，其中参数的含义为：<br>outter：表示输入待排序数据的外轴长度。<br>inner：表示输入待排序数据的内轴长度，inner必须是32的整数倍。<br>n：表示输入待排序数据的内轴的实际长度。<br>请注意：<br>topKInfo.inner必须是32的整数倍。<br>topKInfo.inner是topKInfo.n进行32的整数倍向上补齐的值，因此topKInfo.n的大小应该满足：1 <= topKInfo.n <= topKInfo.inner。<br>Small模式下，topKInfo.inner必须设置为32。<br>Normal模式下，topKInfo.inner最大值为4096。 |
| isLargest | 输入 | 类型为bool。取值为true时默认降序排列，获取前k个最大值；取值为false时进行升序排列，获取前k个最小值。 |

```
struct TopKInfo {
    int32_t outter = 1;
    int32_t inner;
    int32_t n;
};
```

## 返回值说明

无

## 约束说明

-   操作数地址偏移对齐要求请参见[通用说明和约束](../../general_description_and_constraints.md)。
-   **不支持源操作数与目的操作数地址重叠。**
-   当存在srcLocal\[i\]与srcLocal\[j\]相同时，如果i\>j，则srcLocal\[j\]将首先被选出来，排在前面。
-   inf在Topk中被认为是极大值。
-   nan在topk中排序时无论是降序还是升序，均被排在前面。
<!-- npu="310p" id46 -->
-   对于Atlas 推理系列产品AI Core：
    -   输入srcLocal类型是half，模板参数isInitIndex值为false时，传入的topKInfo.inner不能大于2048。

    -   输入srcLocal类型是half，模板参数isInitIndex值为true时，传入的srcIndexLocal中的索引值不能大于2048。
<!-- end id46 -->

-   支持的数据类型<a id="li197551749192811"></a>

    <!-- npu="950" id25 -->
    Ascend 950PR/Ascend 950DT，MERGE\_SORT算法当前支持的数据类型为half、float。RADIX\_SELECT算法当前支持的数据类型为uint8\_t、int8\_t、uint16\_t、int16\_t、uint32\_t、int32\_t、bfloat16\_t、half、float、uint64\_t、int64\_t。
    <!-- end id25 -->

    <!-- npu="A3" id26 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float。
    <!-- end id26 -->

    <!-- npu="910b" id27 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float。
    <!-- end id27 -->

    <!-- npu="310p" id28 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：half、float。
    <!-- end id28 -->

    <!-- npu="x90" id11 -->
    Kirin X90，支持的数据类型为：half。
    <!-- end id11 -->

    <!-- npu="9030" id12 -->
    Kirin 9030，支持的数据类型为：half。
    <!-- end id12 -->

## 调用示例<a name="section94691236101419"></a>

本样例实现了Normal模式代码逻辑。

```
// dstLocalValue：保存已排序出的k个值
// dstLocalIndex：保存已排序出的k个值对应的索引
// srcLocal：保存待排序的值
// srcLocalIndex：保存待排序的值对应的索引
// srcLocalFinish：用于指定某些行的排序是无效排序，其shape为(outter, 1)
// sharedTmpBuffer: 保存排序过程中临时缓存的Tensor
// 获取前k个最大值或最小值及其对应的索引
// topKTiling：存放Topk计算所需Tiling信息，可通过TopKTilingFunc接口获取
// topKInfo：srcLocal的shape信息
// isLargest：取值为true时默认降序排列，获取前k个最大值；取值为false时进行升序排列，获取前k个最小值

// 通过sharedTmpBuffer入参传入临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// 接口框架申请临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

```
// dstLocalValue：保存已排序出的k个值
// dstLocalIndex：保存已排序出的k个值对应的索引
// srcLocal：保存待排序的值
// srcLocalIndex：保存待排序的值对应的索引
// srcLocalFinish：用于指定某些行的排序是无效排序，其shape为(outter, 1)
// sharedTmpBuffer: 保存排序过程中临时缓存的Tensor
// 获取前k个最大值或最小值及其对应的索引
// topKTiling：存放Topk计算所需Tiling信息，可通过TopKTilingFunc接口获取
// topKInfo：srcLocal的shape信息
// isLargest：取值为true时默认降序排列，获取前k个最大值；取值为false时进行升序排列，获取前k个最小值
// defaultTopKConfig：TopK计算的相关配置，包括算法选择、取最大值或最小值、是否对结果排序

// 通过sharedTmpBuffer入参传入临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// 接口框架申请临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

**表3**  Normal模式的样例解析

| 样例描述 | 本样例为对shape为（2，32）、数据类型为float的矩阵进行排序的示例，分别求取每行数据的前5个最小值。<br><br>使用Normal模式的接口，开发者自行传入输入数据索引，传入finishLocal来指定某些行的排序是无效排序。 |
| --- | --- |
| 输入 | 模板参数T：float；模板参数isInitIndex：true；模板参数isHasfinish：true；模板参数topkMode：TopKMode::TOPK_NORMAL；模板参数topkConfig: defaultTopKConfig。<br>输入数据finishLocal：<br>[False  True  False  False False False  False False False  False False False <br> False  False False False False False False False False False False False <br> False False False False False False False False]<br><br>**注意：DataCopy的搬运量要求为32byte的倍数，因此此处finishLocal的实际有效输入是前两位False，True，剩余的值都是进行32bytes向上补齐的值，并不实际参与计算。**<br>输入数据k：5<br>输入数据topKInfo：<br>int32_t outter = 2;<br>int32_t inner = 32;<br>int32_t n = 32;<br>输入数据isLargest：false<br>输入数据srcLocal：<br>[[-18096.555   -11389.83    -43112.895   -21344.77     57755.918<br>   50911.145    24912.621   -12683.089    45088.004   -39351.043<br>  -30153.293    11478.329    12069.15     -9215.71     45716.44<br>  -21472.398   -37372.16    -17460.414    22498.03     21194.838<br>  -51229.17    -51721.918   -47510.38     47899.11     43008.176<br>    5495.8975  -24176.97    -14308.27     53950.695     7652.6035<br>  -45169.168   -26275.518  ]<br> [ -9196.681   -31549.518    18589.23    -12427.927    50491.81<br>  -20078.11    -25606.107   -34466.773   -42512.805    50584.48<br>   35919.934   -17283.5       6488.137   -12885.134     1942.2147<br>  -50611.96     52671.477    23179.662    25814.875      -69.73492<br>   33906.797   -34662.61     46168.71    -52391.258    57435.332<br>   50269.414    40935.05     21164.176     4028.458   -29022.918<br>  -46391.133     1971.2042 ]]<br>输入数据srcIndexLocal：<br>[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br> 24 25 26 27 28 29 30 31] |
| 输出数据 | 输出数据dstValueLocal如下，每行长度是k_pad，其中每条数据的前5个值就是该条的前5个最小值。后面的三个值是随机值。<br>[[-51721.918 -51229.17  -47510.38  -45169.168 -43112.895     0.     0.   0.   ]<br> [-52391.258 -50611.96  -46391.133 -42512.805 -34662.61      0.     0.   0.   ]]<br>输出数据dstIndexLocal如下<br>每行长度是kpad_index，其中每条数据的前5个值就是该条的前5个最小值对应的索引。后面的三个值是随机值。<br>由于第二行数据对应的finishLocal为true，说明第二行数据的排序是无效的，所以其输出的索引值均为内轴实际长度32。<br>[[21 20 22 30  2  0  0  0]<br> [32 32 32 32 32  0  0  0]] |

本样例实现了Small模式的代码逻辑。

```
// dstLocalValue：保存已排序出的k个值
// dstLocalIndex：保存已排序出的k个值对应的索引
// srcLocal：保存待排序的值
// srcLocalIndex：保存待排序的值对应的索引
// srcLocalFinish：用于指定某些行的排序是无效排序，其shape为(outter, 1)
// sharedTmpBuffer: 保存排序过程中临时缓存的Tensor
// 获取前k个最大值或最小值及其对应的索引
// topKTiling：存放Topk计算所需Tiling信息，可通过TopKTilingFunc接口获取
// topKInfo：srcLocal的shape信息
// isLargest：取值为true时默认降序排列，获取前k个最大值；取值为false时进行升序排列，获取前k个最小值

// 通过sharedTmpBuffer入参传入临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// 接口框架申请临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

```
// dstLocalValue：保存已排序出的k个值
// dstLocalIndex：保存已排序出的k个值对应的索引
// srcLocal：保存待排序的值
// srcLocalIndex：保存待排序的值对应的索引
// srcLocalFinish：用于指定某些行的排序是无效排序，其shape为(outter, 1)
// sharedTmpBuffer: 保存排序过程中临时缓存的Tensor
// 获取前k个最大值或最小值及其对应的索引
// topKTiling：存放Topk计算所需Tiling信息，可通过TopKTilingFunc接口获取
// topKInfo：srcLocal的shape信息
// isLargest：取值为true时默认降序排列，获取前k个最大值；取值为false时进行升序排列，获取前k个最小值
// defaultTopKConfig：TopK计算的相关配置，包括算法选择、取最大值或最小值、是否对结果排序

// 通过sharedTmpBuffer入参传入临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// 接口框架申请临时空间
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

**表4**  Small模式的样例解析

| 样例描述 | 本样例为对shape为(4，17)、类型为float的输入数据进行排序的示例，求取每行数据的前8个最大值。<br><br>使用Small模式的接口，开发者自行传入输入数据索引。 |
| --- | --- |
| 输入 | 模板参数T：float；模板参数isInitIndex：true；模板参数isHasfinish：false；模板参数topkMode：TopKMode::TOPK_NSMALL；模板参数topkConfig: defaultTopKConfig；输入数据finishLocal: `LocalTensor<bool> finishLocal`，不需要赋值。输入数据k：8。<br>输入数据topKInfo：<br>int32_t outter = 4;<br>int32_t inner = 32;<br>int32_t n = 17;<br>输入数据isLargest：true<br>输入数据srcLocal：此处n=17，不是32的整数倍时，将其向上补齐到32，填充内容为-inf。<br>[[ 55492.18     27748.229   -51100.11     19276.926    14828.149<br>  -20771.824    57553.4     -21504.092   -57423.414      142.36443<br>   -5223.254    54669.473    54519.184    10165.924     -658.4564<br>    2264.2397  -52942.883           -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [-52849.074    57778.72     37069.496    16273.109   -25150.637<br>  -35680.5     -15823.097     4327.308   -35853.86     -7052.2627<br>   44148.117   -17515.457   -18926.059    -1650.6737   21753.582<br>   -2589.2822   39390.4             -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [-17539.186   -15220.923    29945.332    -4088.1514   28482.525<br>   29750.484   -46082.03     31141.16     23140.047     8461.174<br>   39955.844    29401.35     53757.543    33584.566    -3543.6284<br>  -38318.344    22212.41            -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [ -9970.768    -9191.963   -17903.045     2211.4912   47037.562<br>  -41114.824    13305.985    59926.07    -24316.797    -6462.8896<br>    5699.733    -5873.5015   15695.861   -38492.004    19581.654<br>  -36877.68     27090.158           -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]]<br>输入数据srcIndexLocal：<br>[[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]] |
| 输出数据 | 输出数据dstValueLocal：输出每行数据的前8个最大值。<br>[[57553.4    55492.18   54669.473  54519.184  27748.229  19276.926<br>  14828.149  10165.924 ]<br> [57778.72   44148.117  39390.4    37069.496  21753.582  16273.109<br>   4327.308  -1650.6737]<br> [53757.543  39955.844  33584.566  31141.16   29945.332  29750.484<br>  29401.35   28482.525 ]<br> [59926.07   47037.562  27090.158  19581.654  15695.861  13305.985<br>   5699.733   2211.4912]]<br>输出数据dstIndexLocal：输出每行数据的前8个最大值索引。<br>[[ 6  0 11 12  1  3  4 13]<br> [ 1 10 16  2 14  3  7 13]<br> [12 10 13  7  2  5 11  4]<br> [ 7  4 16 14 12  6 10  3]] |
