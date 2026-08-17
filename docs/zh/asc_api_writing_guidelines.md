# Ascend C API写作规范

## 1 概述

### 1.1 目标和适用范围

**目标**

本规范旨在帮助Ascend C API文档开发者输出完整、准确、一致、实用、易用的API参考文档，让用户获得一本自支持度极高的工具手册。通过统一的写作标准，确保各类API文档在结构组织、内容呈现、参数说明等方面保持一致，降低用户理解成本，提升开发效率。

**适用范围**

本规范适用于所有Ascend C API参考文档的编写与评审。

文档开发者在新增、修改API参考文档时，均应遵循本规范；文档评审人员在评审时，应以本规范作为评审依据。

### 1.2 总体原则

Ascend C API文档须遵循以下总体原则：

- **准确性**：文档内容须与代码实现严格一致，包括函数原型、参数名、数据类型、约束条件等，确保用户按文档使用时不会产生歧义。
- **完整性**：每个API须覆盖产品支持情况、功能说明、函数原型、参数说明、约束说明、调用示例等必要章节，禁止缺失关键信息。
- **一致性**：同类API在章节结构、术语使用、格式呈现等方面须保持统一，降低用户在不同API间的切换成本。
- **易用性**：文档应站在开发者视角组织内容，复杂概念须给出解释或超链接，提供可运行的调用示例，让开发者能够快速上手。

### 1.3 条款组织方式

本规范中的条款按照以下结构组织：

- **条款编号**：以"API-"为前缀，按章节分类编号，如API-FUNC-00、API-PROTO-00等。
- **条款标题**：简明描述条款要求。
- **【级别】**：条款的强制程度，分为"强制"和"建议"。
- **【描述】**：条款的具体要求说明。
- **【正例】**：符合要求的写作示例。
- **【反例】**：不符合要求的写作示例。
- **【例外】**：某些特殊场景下，允许出现的不符合规范的例外情况。

## 2 章节组织规范

### API-STRUCT-00 API导航目录结构或节点标题变更时，应同步更新索引文件

【级别】强制

【描述】Ascend C API的导航目录结构（静态页面/昇腾社区文档左侧导航栏的目录结构）通过[索引文件](./api/README.md)对源码文件进行组织得到，索引文件以列表形式描述各节点间的层级关系。源码文件的目录结构与导航目录结构保持一致。导航目录结构中的每个节点均对应一个md文件：

- **目录节点**（即父节点）：对应一个与源码目录同名的索引文件（如`basic_api.md`），用于组织其下的子节点。
- **叶子节点**（即具体API）：对应一个API内容md文件。

【正例】

假设我们希望呈现如下所示的目录树导航结构（静态页面/昇腾社区文档页面展示的效果）
``` text 
|-- SIMD API  
|--|-- 基础API
|--|--|-- Add
|--|--|-- Sub
|--|-- C API
|--|--|-- asc_add
|--|--|-- asc_sub
|-- SIMT API
|--|-- acosf
|--|-- asinf
```

对应的源码文件的目录结构应组织如下：
``` text
|-- README.md  
|-- SIMD-API  
|--|-- SIMD-API.md
|--|-- basic_api
|--|--|-- basic_api.md
|--|--|-- Add.md
|--|--|-- Sub.md
|--|-- c_api
|--|--|-- c_api.md
|--|--|-- asc_add.md
|--|--|-- asc_sub.md
|-- SIMT-API
|--|-- SIMT-API.md
|--|-- acosf.md
|--|-- asinf.md
```


对应的索引文件README.md中的内容为：

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [基础API](SIMD-API/basic_api/basic_api.md)
        -   [Add](SIMD-API/basic_api/Add.md)
        -   [Sub](SIMD-API/basic_api/Sub.md)
    -   [C API](SIMD-API/c_api/c_api.md)
        -   [asc_add](SIMD-API/c_api/asc_add.md)
        -   [asc_sub](SIMD-API/c_api/asc_sub.md)
-   [SIMT API](SIMT-API/SIMT-API.md)
    -   [acosf](SIMT-API/acosf.md)
    -   [asinf](SIMT-API/asinf.md)
```

### API-STRUCT-01 导航目录中不同的节点不能对应同一个md源码文件

【级别】强制

【描述】

导航目录中不同的节点不能对应同一个md源码文件。上线昇腾社区后，官网页面的URL通过md源码文件的路径拼接而成，若多个不同节点对应同一个源码文件，会导致URL冲突。

【反例】

如下索引文件中，矩阵计算（Tensor_API）和矩阵计算目录下的GEMV节点对应同一个源码文件，会导致URL冲突。

``` text
-   [SIMD API](SIMT-API/SIMT-API.md)
    -   [基础API](SIMT-API/basic_api/basic_api.md)
        -   [矩阵计算](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad计算](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [关键特性说明](SIMD-API/basic_api/cube_compute/mmad_compute/features/features.md)
                    -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/features/GEMV.md)
        -   [矩阵计算（Tensor_API）](SIMD-API/basic_api/cube_compute_TensorAPI/cube_compute_TensorAPI.md)
            -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/features/GEMV.md)
``` 
### API-STRUCT-02 API源码目录和文件名命名规范

【级别】强制

【描述】

源码目录和文件名须以英文小写命名，多个单词以下划线（_）连接。

【例外】
- API接口名采用大写英文驼峰命名时，文件名应与API名称保持一致，采用大写风格，如基础API中的Add.md。

- 概念和术语中以大写英文命名的，可保留大写风格，如基础API中的Neg_ISASI.md（ISASI为概念术语）。


### API-STRUCT-03 导航目录层级不能超过5级

【级别】强制

【描述】

导航目录层级不能超过5级。嵌套层级过深会影响文档阅读体验，且超出该层级后文档无法上线昇腾社区官网。

【反例】

如下索引文件中，`GEMV`为6级目录，不符合要求。

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [基础API](SIMD-API/basic_api/basic_api.md)
        -   [矩阵计算](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad计算](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [关键特性说明](SIMD-API/basic_api/cube_compute/mmad_compute/features/features.md)
                    -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/features/GEMV.md)
```   

### API-STRUCT-04 不建议导航目录下只有一个子节点

【级别】建议

【描述】

不建议导航目录下只有一个子节点，否则阅读体验不佳且层次嵌套过深。

【反例】

关键特性说明目录下只有一个节点，无需增加这一层级。

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [基础API](SIMD-API/basic_api/basic_api.md)
        -   [矩阵计算](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad计算](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [关键特性说明](SIMD-API/basic_api/cube_compute/mmad_compute/feature/feature.md)
                    -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/feature/GEMV.md)
``` 

【正例】
``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [基础API](SIMD-API/basic_api/basic_api.md)
        -   [矩阵计算](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad计算](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [GEMV关键特性说明](SIMD-API/basic_api/cube_compute/mmad_compute/GEMV_feature.md)
```   


### API-STRUCT-05 复杂API须按功能场景拆分章节

【级别】建议

【描述】复杂API若存在不同的功能场景，须按功能场景拆分章节。

  - 不同芯片版本支持的API功能不同时，应按芯片版本拆分章节。
  - 同一芯片版本支持的API功能不同时，应按功能场景拆分章节。


【正例】

C API中的asc_copy_gm2ub接口在2201版本和3510版本差异较大，在一个md文件中呈现会导致结构混乱，且需要增加过多的芯片过滤标签，此时可以将接口说明拆分为两个文件，使用架构版本号为后缀来进行区分。

```text
            -   [asc_copy_gm2ub](SIMD-API/C-API/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md)
                -   [asc_copy_gm2ub_arch_2201](SIMD-API/C-API/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md)
                -   [asc_copy_gm2ub_arch_3510](SIMD-API/C-API/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md)
```

【正例】

基础API中DataCopy和LoadData接口，每个接口都对应了多种搬运场景，拆分成多个md进行组织。

```text
            -   [矩阵计算的搬入](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_load.md)
                -   [总体说明](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/overall_description.md)
                -   [矩阵计算输入搬运约束](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_input_move_constraint.md)
                -   [L1 Buffer/L0A Buffer/L0B Buffer内存结构介绍](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/L1_L0A_B_memory_structure_intro.md)
                -   [LoadData（GMToL1-2D矩阵搬运）](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2D.md)
                -   [LoadData（GMToL1-2D矩阵搬运V2）](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md)
                -   [DataCopy（GMToL1连续数据搬运）](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md)
                -   [DataCopy（GMToL1高维切分数据搬运）](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md)
```


### API-STRUCT-06 每个接口API内部一级标题名须与导航结构中的标题名一致

【级别】强制

【描述】每个接口API内部一级标题名须与导航结构中的标题名一致，即与索引文件中链接文本（[]内的内容）保持一致。

【反例】

Neg接口的导航标题为Neg（ISASI），但Neg_ISASI.md中的标题名为Neg，两者不一致。

索引文件内容如下：

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [基础API](SIMD-API/basic_api/basic_api.md)
        -   [Add](SIMD-API/basic_api/Add.md)
        -   [Neg（ISASI）](SIMD-API/basic_api/Neg_ISASI.md)
    -   [C API](SIMD-API/c_api/c_api.md)
        -   [asc_add](SIMD-API/c_api/asc_add.md)
        -   [asc_sub](SIMD-API/c_api/asc_sub.md)
```   

Neg_ISASI.md的内容如下：

``` text
# Neg

## 产品支持情况
...

```   

### API-STRUCT-07 每个接口API内部须按固定顺序组织章节

【级别】强制

【描述】每个接口的md文件须按如下顺序组织章节（均为二级标题），其中数据类型、关键特性、需要包含的头文件章节可视接口实际情况选填。例如高阶API中的Sin接口，其不同芯片支持的数据类型相同，无需单独章节描述差异，可随参数说明一并描述。例如基础API统一在功能说明中描述需要包含的头文件，可以不需要"需要包含的头文件"章节。同一类API的章节结构应保持统一。

```text
产品支持情况
功能说明
函数原型
参数说明
数据类型
返回值说明
约束说明
需要包含的头文件
关键特性说明
调用示例
```


## 3 产品支持情况规范

### API-PROD-00 产品支持情况须置于API文档首节

【级别】强制

【描述】产品支持情况为API文档的第一个章节，须位于页面最顶部，紧接一级标题之下。通过列表形式列出每个产品系列的支持状态，便于用户快速判断该API是否可用于目标硬件。

【例外】
部分接口为Host接口，不区分产品，此时可以不需要该章节。

### API-PROD-01 产品支持情况须按照新旧顺序覆盖全部产品系列

【级别】强制

【描述】产品支持情况须覆盖当前所有产品系列，逐一列出支持状态，禁止遗漏。每个产品系列独占一行，格式为`- 产品名：支持/不支持`。若同一产品系列区分AI Core和Vector Core，须分别列出。

【正例】

```text
## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
```

### API-PROD-02 产品支持情况须使用芯片过滤标签

【级别】强制

【描述】每个产品系列的支持状态须使用HTML注释形式的芯片过滤标签（`<!-- npu="..." -->`和`<!-- end -->`）包裹，以便昇腾社区官网按芯片版本动态过滤展示。过滤标签中的npu属性值须与产品系列一一对应。

【正例】

```text
## 产品支持情况

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas 训练系列产品：不支持
<!-- end id14 -->
```

## 4 功能说明规范

### API-FUNC-00 功能说明中须涵盖功能作用、用户价值、基本用法三要素

【级别】建议

【描述】功能说明须明确以下内容：

  - 功能作用：API的功能（作用、目的）是什么？
  - 用户价值：使用该API后用户体验的变化？
  - 基本用法：如何使用该API？

【正例】

```text  
设置Mask模式为Counter模式。该模式下，不需要开发者去感知迭代次数、处理非对齐的尾块等操作，可直接传入计算数据量，实际迭代次数由Vector计算单元自动推断。本接口推荐配合API中isSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Counter模式，并通过[SetVectorMask](./SetVectorMask.md)设置Counter模式下参与计算的元素个数。
```


上述接口说明中明确了功能作用、用户价值、基本用法。

- 功能作用：设置Mask模式为Counter模式。
- 用户价值：该模式下，不需要开发者去感知迭代次数、处理非对齐的尾块等操作，可直接传入计算数据量，实际迭代次数由Vector计算单元自动推断。
- 基本用法：本接口推荐配合API中isSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Counter模式，并通过SetVectorMask设置Counter模式下参与计算的元素个数。

【反例】

如下功能说明仅列出了接口的功能，未给出用户价值和基本用法：

```text
获取ICache的Preload的状态。
```

### API-FUNC-01 须提供头文件说明

【级别】强制

【描述】须提供接口所在的头文件及使用该接口时需包含的头文件。若在前文章节已按API分类有总体介绍或者有单独章节介绍，功能说明中则无需重复介绍。头文件路径用反引号包裹。

【正例】  
头文件路径为：`"basic_api/kernel_operator_vec_binary_intf.h"`。

【例外】  
- 如果有单独的章节“需要包含的头文件”单独说明需要包含的头文件，此处无需列出。
- 如果在上文章节统一说明一类接口需要包含的头文件，此处无需列出。

### API-FUNC-02 引入新概念时须增加必要的解释或者增加至相关概念的超链接

【级别】建议

【描述】进行功能说明时，若引入了本API中未介绍的概念，须增加必要的解释或者至相关概念的超链接。

【正例】
如下的功能说明中，直接给出了Counter模式的介绍。

```text
设置Mask模式为Counter模式。该模式下，不需要开发者去感知迭代次数、处理非对齐的尾块等操作，可直接传入计算数据量，实际迭代次数由Vector计算单元自动推断。本接口推荐配合API中isSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Counter模式，并通过[SetVectorMask](./SetVectorMask.md)设置Counter模式下参与计算的元素个数。
```

### API-FUNC-03 与其他API配合使用时须增加关联API的超链接

【级别】建议

【描述】若该API需要与其他API配合使用，须描述其配合关系；若与其他API功能类似，须描述API间的区别，并增加至关联API的超链接。

【正例】
SetAippFunctions接口须与LoadImageToLocal接口配合使用，增加了LoadImageToLocal接口的超链接。

```text
设置图片预处理（AIPP，AI Core pre-process）相关参数。与[LoadImageToLocal](./LoadImageToLocal.md)接口配合使用。设置后，调用LoadImageToLocal接口可在搬运过程中完成图像预处理操作：包括数据填充、通道交换、单行读取、数据类型转换、通道填充、色域转换。
```

【正例】
TransDataTo5HD接口给出了完成转置功能时与Transpose接口的区别。

```
数据格式转换，一般用于将NCHW格式转换成NC1HWC0格式。特别地，也可用于二维矩阵数据块的转置。完成转置功能时，相比于[Transpose](./Transpose.md)接口，Transpose仅支持16×16大小的矩阵转置；本接口单次repeat内可处理512Byte的数据（16个datablock），根据数据类型不同，支持不同shape的矩阵转置（如数据类型为half时，单次repeat可完成16×16大小的矩阵转置），同时还可支持多次repeat操作。
```

【反例】
SetFixPipeConfig接口与DataCopy接口配合实现的功能与Fixpipe接口等同，但SetFixPipeConfig中既未给出Fixpipe接口的链接，也未给出两种方式的对比描述。

```text
在DataCopy（CO1-\>GM）的流程中，会涉及relu和quant流程，分别用于relu和quant计算。通过该接口设置relu和quant的源操作数。
```

### API-FUNC-04 计算类接口须使用公式描述功能

【级别】建议

【描述】计算类接口，若使用公式表达可便于理解接口功能，须增加公式描述。公式中的变量名须与函数原型中的变量名保持一致。功能说明中的数学表达式应尽量复用数学符号，符合LaTex语法，保证内容简洁清晰。

【正例】
如下接口中给出了乘法的计算公式，使用\times标准写法。

```text
该接口根据mask对输入数据srcReg0、srcReg1按元素相乘操作，将结果写入dstReg。计算公式如下：

$$
dstReg_i = srcReg0_i \times srcReg1_i
$$
```


【反例】
如下接口中给出了乘法的计算公式，使用*不规范用法。

```text
该接口根据mask对输入数据srcReg0、srcReg1按元素相乘操作，将结果写入dstReg。计算公式如下：

$$
dstReg_i = srcReg0_i * srcReg1_i
$$
```

### API-FUNC-05 复杂接口功能须增加图示说明

【级别】建议

【描述】接口功能复杂或难以用文字表达时，须增加清晰的图示说明。图片绘制须遵循[图元规范](#12-图元规范)。

【正例】
`ReduceSum`接口中给出了接口求和计算的示意图。

```text
`ReduceSum`接口对所有输入数据进行求和，计算过程如下。

**图1** `ReduceSum`计算示意图<a id="fig1"></a>

![ReduceSum计算示意图](api/figures/ReduceSum_basic_api.png "ReduceSum计算示意图")
```

【反例】
Gather的功能说明中未给出图示说明，仅通过文字难以理解。

```text
给定输入的张量和一个地址偏移张量，Gather指令根据偏移地址将输入张量按元素收集到结果张量中。
```

## 5 函数原型规范

### API-PROTO-00 多函数原型须按结构体不同拆分章节或通过无序列表呈现

【级别】建议

【描述】一个API若存在多个函数原型，须按参数中结构体的不同拆分章节或通过无序列表呈现，并以文字描述函数原型间的区别。后续内容中须区分不同原型的，与函数原型中的介绍顺序保持一致。

【正例】

```text
- 整个tensor参与计算

    ```cpp
    dst = src0 + src1;
    ```

- tensor前n个数据连续计算

    ```cpp
    template <typename T>
    __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```
```

【例外】
对于函数原型过多的场景，仅使用无序列表会导致嵌套层次深、篇幅过长，可以采取无序列表+代码注释相结合的方式进行拆分。

```text
- 前n个数据计算
    ```cpp
    // RINT舍入模式
    __aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // ROUND舍入模式
    __aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    //FLOOR舍入模式
    __aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // CEIL舍入模式
    __aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // TRUNC舍入模式
    __aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    ```

- 高维切分计算
    ```cpp
    // RINT舍入模式
    __aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // ROUND舍入模式
    __aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // FLOOR舍入模式
    __aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // CEIL舍入模式
    __aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // TRUNC舍入模式
    __aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```
```

### API-PROTO-01 函数原型须与头文件严格一致

【级别】强制

【描述】函数原型须与头文件中原型定义严格保持一致。函数名、参数名（包括模板参数）、参数类型（包括模板参数）、返回值数据类型、函数限定符等均须保持一致。原型数量也须与头文件严格一致。

### API-PROTO-02 函数原型须使用codetype cpp格式呈现

【级别】强制

【描述】函数原型通过codetype cpp格式呈现，无需带末尾分号。

【正例】

```text

- tensor前n个数据连续计算

    ```cpp
    template <typename T>
    __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

```

### API-PROTO-03 须明确写明每种原型支持的硬件型号

【级别】强制

【描述】不同原型支持的芯片型号不同时，须明确写明每种原型支持的硬件型号。

```text
<!-- npu="950,A3,910b,310b,x90,9030" id15 -->
- 该原型支持如下产品型号：

    <!-- npu="950" id16 -->
    Ascend 950PR/Ascend 950DT 
    <!-- end id16 -->

    <!-- npu="A3" id17 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品 
    <!-- end id17 -->

    <!-- npu="910b" id18 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品 
    <!-- end id18 -->

    <!-- npu="310b" id19 -->
    Atlas 200I/500 A2 推理产品 
    <!-- end id19 -->

    <!-- npu="x90" id20 -->
    Kirin X90 
    <!-- end id20 -->

    <!-- npu="9030" id21 -->
    Kirin 9030 
    <!-- end id21 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id15 -->

<!-- npu="310p,910" id22 -->
- 该原型支持如下产品型号：

    <!-- npu="310p" id23 -->
    Atlas 推理系列产品AI Core 
    <!-- end id23 -->

    <!-- npu="310p" id24 -->
    Atlas 推理系列产品Vector Core 
    <!-- end id24 -->

    <!-- npu="910" id25 -->
    Atlas 训练系列产品 
    <!-- end id25 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_MTE3, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id22 -->
```

## 6 参数说明规范

### API-PARAM-00 模板参数与入参须分两个表格说明

【级别】强制

【描述】有模板参数的API，须将模板参数与入参分两个表格说明，按模板参数说明、参数说明的顺序介绍。参数说明须完备，禁止缺失某个或多个参数说明。

【正例】

```text
**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src0、src1 | 输入 | 源操作数。<br>类型为[LocalTensor](LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>两个源操作数的数据类型需要与目的操作数保持一致。 |
| count | 输入 | 参与计算的元素个数。 |
```

### API-PARAM-01 参数名须与函数原型严格一致

【级别】强制

【描述】参数名须与函数原型严格一致，参数名不加粗。

### API-PARAM-02 须明确参数的输入/输出

【级别】建议

【描述】输入表示入参，输出表示出参，须根据实际情况填写。

### API-PARAM-03 须描述参数的使用场景和功能

【级别】建议

【描述】参数说明中须描述参数的使用场景和参数功能。

### API-PARAM-04 须明确参数取值

【级别】建议

【描述】对于数值类参数，须列出单位（数字与国际单位之间不应有空格，推荐使用中文单位或国际标准单位，如16字节、16B）、取值范围、默认值及支持的数据类型。单位、取值范围、默认值若在接口原型定义中已明确，且无需特别说明的，可不给出。有特殊含义的参数取值，不应使用"魔鬼数字"，须增加解释该取值含义的说明。参数取值相关内容区分硬件型号时，须按硬件型号分别列出。

参数取值数据类型：
- 原型中已固定确定的数据类型，无需单独说明。
- 数据类型存在多种可能的，须说明支持的数据类型。
- 接口支持的数据类型只与位宽有关时，可通过位宽表达：b8、b16、b32、b64。数据类型的完整排序和写法详见[数据类型规范](#api-software-01-数据类型须按固定顺序排列)。

【正例】
repeatTime参数的单位和支持的数据类型在原型定义中可以明确，但其取值范围与基础API通用说明中不同，此处特别说明了其取值范围。

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| repeatTime | 输入 | 迭代次数。与通用参数说明中不同的是，支持更大的取值范围，保证不超过int32_t最大值的范围即可。 |

【反例】
未给出srcBlkStride的取值范围和单位。

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| srcBlkStride | 输入 | 表示单次迭代内，矢量源操作数单次迭代内不同datablock间地址步长。 |

### API-PARAM-05 数据结构类参数须列出定义并给出详细介绍

【级别】建议

【描述】数据结构类参数，须列出数据结构的定义（结构定义过长可不在文档列出，但须给出所在头文件目录）并给出详细介绍（或增加至详细介绍的超链接）。若结构体参数被多个API使用，经评估在单个API中呈现不合理，须将结构体定义提炼至公共位置，并在本API内增加至该结构体定义的说明。

【正例】
dataCopyParams给出了其数据类型所在的头文件目录和详细介绍。

```text
*表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数，类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md)，存储位置为Unified Buffer，目的地址需要32字节对齐。 |
| src | 输入 | 源操作数，类型为[GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md)，存储位置为Global Memory，源地址需要1字节对齐。 |
| dataCopyParams | 输入 | 搬运参数。DataCopyExtParams类型，具体参数说明请参考[表3](#table_gm2ub_pad_3)。 |
| padParams | 输入 | 从Global Memory搬运数据至Local Memory时，可以根据开发者需要，在搬运数据左边或右边填充数据。padParams是用于控制数据填充过程的参数。DataCopyPadExtParams类型，具体参数请参考[表4](#table_gm2ub_pad_4)。 |

下文表格中列出的结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。

**表3**  DataCopyExtParams结构体参数定义<a name="table_gm2ub_pad_3"></a>

| 参数名 | 描述 |
| :--- | :--- |
| blockCount | 搬运的数据块个数，数据类型为uint16_t，取值范围：blockCount∈[0, 4095]，默认值为1。 |
| blockLen | 搬运的每个数据块长度，数据类型为uint32_t，取值范围：blockLen∈[0, 2097151]，单位：1B。<br>**blockLen必须是sizeof(T)的整数倍，需要注意不要超过UB空间大小。** |
| srcStride | 源操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），数据类型为uint32_t，取值范围：srcStride∈[0, 2^32 - 1]，单位：1B。不同产品中srcStride的数据类型和支持的取值范围可能不同，详细请参考[约束说明](#section633mcpsimp)。 |
| dstStride | 目的操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），数据类型为uint32_t，取值范围：dstStride∈[0, 2^32 - 1]，单位：dataBlock（32B）。不同产品中dstStride的数据类型和支持的取值范围可能不同，详细请参考[约束说明](#section633mcpsimp)。若PaddingMode为Compact模式，该参数无效，默认值为0，取默认值即可。<br>**注：需要注意不要超过UB空间大小**。 |
| rsv | 保留字段。数据类型为uint16_t，默认值为0。 |

```

### API-PARAM-06 参数单位须解释其原理和含义

【级别】建议

【描述】对于参数单位，须解释其原理和含义，便于开发者理解为何以该数值为单位，以及为何不同的数据类型单位不同。

【正例】
明确指出dstRepStride的参数单位是以一个repeat归约后的长度为单位。后续介绍具体数值时，开发者可理解为何以该数值为单位，以及为何不同的数据类型单位不同。

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dstRepStride | 输入 | 目的操作数相邻迭代间的地址步长。以一个repeat归约后的长度为单位。每个repeat(8个DataBlock)归约后，得到8个元素，所以输入类型为half类型时，RepStride单位为16Byte；输入类型为float类型时，RepStride单位为32Byte。 |

【反例】
仅列出参数的单位为16Bytes、32Bytes，缺少必要的原理和含义介绍。开发者无法得知为何以该数值为单位，以及为何不同的数据类型单位不同，不利于开发者记忆。

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dstRepStride | 输入 | 表示相邻迭代间，矢量目的操作数相邻迭代间相同datablock的地址步长。输入类型为half类型时，RepStride单位为16Bytes，输入类型为float类型时，RepStride单位为32Bytes。 |

### API-PARAM-07 参数支持的数据类型或者取值范围须按硬件型号分别列出

【级别】建议

【描述】参数支持的数据类型在不同硬件型号下不同时，须分别列出。

```text
**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| flagId | 输入 | 核间同步的标记。不同产品对flagId的取值范围说明请参见[flagId取值范围说明](#flagId取值范围说明)。 |


### flagId取值范围说明<a id="flagId取值范围说明"></a>

- 核间同步的模式为模式0、1、2时，支持的取值范围为0-15。
<!-- npu="950" id26 -->
- 针对Ascend 950PR/Ascend 950DT，核间同步的模式为模式4时，支持的取值范围情况如下：
    - AIV0发起的flagId 0-10的CrossCoreSetFlag操作对应AIC CrossCoreWaitFlag中flagId 0-10的操作。
    - AIV1发起的flagId 0-10的CrossCoreSetFlag操作对应AIC CrossCoreWaitFlag中flagId 16-26的操作。
    - AIC发起的flagId 0-10的CrossCoreSetFlag操作对应AIV0 CrossCoreWaitFlag中flagId 0-10的操作。
    - AIC发起的flagId 16-26的CrossCoreSetFlag操作对应AIV1 CrossCoreWaitFlag中flagId 0-10的操作。
<!-- end id26 -->
```

### API-PARAM-08 复杂参数配置方法须提供说明

【级别】建议

【描述】对于复杂或具有特殊要求的配置方法，须提供配置方式的说明。

【正例】
isHasfinish配置为false时，finishedLocal只需进行定义，无需赋值，此处给出了finishedLocal的具体构造示例。

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| finishedLocal | 输入 | 源操作数。用于指定某些行的排序是无效排序，其shape为(outter, 1)。类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。该参数和模板参数isHasfinish配合使用，Normal模式下支持isHasfinish配置为true/false，Small模式下仅支持isHasfinish配置为false。 |

  - isHasfinish配置为true
    - finishedLocal对应的outter行的值为true时，该行排序无效，排序后输出的dstIndexLocal的k个索引值会全部被置为n。
    - finishedLocal对应的outter行的值为false时，该行排序有效。
  - isHasfinish配置为false时，finishedLocal只需进行定义，不需要赋值，将定义后的finishedLocal传入接口即可。定义样例如下：

```
LocalTensor<bool> finishedLocal;
```

### API-PARAM-09 参数说明中须增加相关概念和关联API的超链接

【级别】建议

【描述】进行参数介绍时，若引入了本API中未介绍的概念，须增加至相关概念的超链接。若该参数需要与其他API配合使用，须描述其配合关系；若与其他API/参数功能类似，须描述两者间的区别，并增加至关联API/参数的超链接。

【正例】
IBSHARE参数与IBShare模板配合使用，参数说明中描述了其配合关系，并增加了至关联参数/API的超链接。

```text
| 参数名 | 描述 |
|---|---|
| IBSHARE | 是否使能IBShare。IBShare的功能是能够复用L1上相同的A矩阵或B矩阵数据，不支持同时复用A矩阵和B矩阵的数据，即A矩阵和B矩阵同时只有一个可以使能IBShare。与[Matmul模板参数](./Matmul_template_params.md)中的IBShare模板配合使用，具体参数设置详见表2。 |
```

### API-PARAM-10 单个参数的约束信息须就近填写

【级别】建议

【描述】单个参数说明的约束信息须就近填写。

> 参数说明中给出的约束多为概括性描述，在约束说明中应给出特殊值、异常值处理等具体的约束信息、多个参数的关联约束等。两者可以有适度的重复。

### API-PARAM-11 复杂参数功能须增加图示说明

【级别】建议

【描述】参数功能复杂或难以用文字表达时，须在参数表格后增加清晰的图示说明。图示绘制须遵循[图元规范](#12-图元规范)。

【正例】
DataCopy的使用说明中给出了DataCopyParams结构体参数的配置图示。

```text
以下样例呈现了DataCopyParams结构体参数的使用方法，样例中完成了2个连续传输数据块的搬运，每个数据块含有8个datablock，源操作数相邻数据块之间无间隔，目的操作数相邻数据块尾与头之间间隔1个datablock。

![DataCopyParams结构体参数使用方法示意图](api/figures/repeat-times.png)
```

## 7 返回值说明规范

### API-RET-00 返回值须与函数原型严格一致

【级别】强制

【描述】须与函数原型中的返回值类型严格保持一致。

### API-RET-01 须列出返回值的单位和具体取值含义

【级别】强制

【描述】须列出返回值的单位和具体取值的含义说明。

【正例】

返回值为true/false，true表示成功获取Topk接口内部计算需要的最大和最小临时空间大小；false表示获取失败。

## 8 约束说明规范

### API-CONST-00 约束须全面且合理

【级别】建议

【描述】过多的约束会给用户带来理解负担，影响文档和软件使用体验。添加约束时，应充分考虑软件设计与实现是否还有优化空间，避免增加不合理的约束。须从以下几个方面考虑需要添加哪些约束：

  - 源操作数与目的操作数地址复用是否有约束（基础API此类约束为必选）。
  - 操作数地址对齐是否有要求（基础API此类约束为必选）。
  - 多个参数组合使用或参数之间的约束。
  - 表述的参数名须与函数原型中的参数名保持一致。
  - 在参数配置方面，与通用API参数介绍相比有无额外需要注意的地方。
  - 在API使用、参数配置、计算结果的排布方面有无开发者容易理解错误、易混淆的地方。
  - 考虑接口对当前环境的影响：调用该接口后，是否需要调用其他API恢复当前环境。
  - API对特殊数据类型（如int4_t）的支持度有无差异，使用时有什么需要注意的地方。
  - API接口支持的开发方式有无约束，Kernel直调开发方式和工程化开发方式是否都支持。
  - API有无性能方面的约束，如reduce类fp16性能比fp32性能差。

接口的约束须全面，保证资料内容完备性。

【正例】

```text
该接口仅在矢量计算API的isSetMask模板参数为false时生效，使用完成后需要使用[ResetMask](./ResetMask.md)将mask恢复为默认值。
```

## 9 调用示例规范

### API-EXAMPLE-00 须通过codetype cpp格式呈现关键代码片段

【级别】建议

【描述】仅提供关键代码片段即可，完整样例须合入样例仓或单独呈现。通过codetype cpp格式呈现。结果示例对理解API功能有帮助的，应按调用示例的场景分别给出。

【正例】

```cpp
AscendC::Tpipe pipe;
AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize 通过Host侧tiling参数获取
AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor();
// 输入shape信息为1024, 算子输入的数据类型为half, 实际计算个数为512
AscendC::Cos(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

结果示例如下：

```
输入数据(srcLocal): [0.5047314 0.1864135 ... -0.73319215 0.84544605]
输出数据(dstLocal): [0.8753044 0.98267525 ... 0.7430419 0.6633976]
```

### API-EXAMPLE-01 调用示例配套了完整样例的，需要在调用示例章节增加到完整样例的链接。

【级别】建议

【描述】调用示例配套了完整样例的，需要在调用示例章节增加到完整样例的链接（使用相对路径）。

【正例】

```text
详细示例请参考[ReduceMax样例](../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation)。

- tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor高维切分计算接口，设定repeatTime为65，mask为全部元素参与计算
    int32_t mask = 128;
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor高维切分计算接口，设定repeatTime为65,mask为全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```
```
### API-EXAMPLE-02 样例代码须增加注释说明

【级别】建议

【描述】样例代码中的输入参数、关键接口、复杂逻辑代码须增加注释说明，描述配置原因。示例代码须与功能描述或性能优化手段相匹配。

### API-EXAMPLE-03 样例代码须符合编码规范和接口约束

【级别】强制

【描述】样例代码须符合编码规范和接口约束。调用示例的顺序须与函数原型的顺序保持一致，调用接口的参数个数、类型、顺序须准确，与函数原型保持一致。

### API-EXAMPLE-04 调用示例不应包含未对外公开的接口

【级别】强制

【描述】调用示例不应包含未对外公开的接口。

### API-EXAMPLE-05 调用示例不应与特定算子开发方式强相关

【级别】强制

【描述】调用示例不应与算子开发方式（Kernel直调开发方式和工程化开发方式）强相关，只能在某一种开发方式下使用的API属于例外场景。

 ### API-EXAMPLE-06 样例代码须使用规范的命名空间和宏定义 
 
 
 【级别】强制 
 
 
 【描述】样例代码中不应出现`using namespace AscendC`等不规范用法，在调用接口、枚举时需要加上命名空间限定。

 【级别】强制

 【正例】枚举值和接口名都加上AscendC命名空间。

```cpp
AscendC::DataCopy(src1Local, src1Global[i * tileLength], tileLength);
AscendC::DataCopy(src0Local, src0Global[i * tileLength], tileLength);

// 循环内依赖：先“DataCopy(PIPE_MTE2)写src0Local”，后“Maxs和Mins（PIPE_V）读src0Local”。
// 由于PIPE_V需要等待PIPE_MTE2，所以需要插入以下同步。
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

AscendC::Maxs(tmpTensor1, src0Local, inputVal, tileLength);
AscendC::Mins(tmpTensor2, src0Local, inputVal, tileLength);
```

【反例】枚举值MTE2_V没有加AscendC命名空间。

```cpp
AscendC::DataCopy(src1Local, src1Global[i * tileLength], tileLength);
AscendC::DataCopy(src0Local, src0Global[i * tileLength], tileLength);

// 循环内依赖：先“DataCopy(PIPE_MTE2)写src0Local”，后“Maxs和Mins（PIPE_V）读src0Local”。
// 由于PIPE_V需要等待PIPE_MTE2，所以需要插入以下同步。
AscendC::SetFlag<MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<MTE2_V>(EVENT_ID0);

AscendC::Maxs(tmpTensor1, src0Local, inputVal, tileLength);
AscendC::Mins(tmpTensor2, src0Local, inputVal, tileLength);
```

## 10 业务规范

### API-SOFTWARE-00 数据类型写法须与代码保持一致

【级别】强制

【描述】文档中的数据类型写法须与代码中的数据类型写法保持一致。

### API-SOFTWARE-01 数据类型须按固定顺序排列

【级别】建议

【描述】数据类型的写作顺序须保持一致，原则：位宽从小到大、有符号数→无符号数→浮点数、浮点数按指数位大小排序。完整顺序如下：

bool、int4b_t、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。

### API-SOFTWARE-02 位宽相关数据类型可使用位宽表达

【级别】建议

【描述】接口支持的数据类型只与位宽有关时，可通过位宽表达：b8、b16、b32、b64。

### API-SOFTWARE-03 NAN和INF写法须区分概念与数值场景

【级别】强制

【描述】文档中涉及NAN和INF时，须区分使用场景：描述概念时使用大写形式（NAN、INF），描述具体数值时使用小写形式（nan、inf）。

## 11 术语规范

### API-TERM-00 须使用统一的概念术语

【级别】强制

【描述】须使用统一的概念术语，术语表参考：[LINK](./guide/technical_appendix/concepts_and_terms/glossary.md)。

## 12 图元规范

### API-FIGURE-00 图元字体和字号须符合要求

【级别】强制

【描述】中文须使用方正兰亭黑简体，英文须使用Huawei Sans。中文不能小于9pt（相当于12像素），全角字符；英文不能小于8pt（相当于10像素），半角字符。

### API-FIGURE-01 图形宽度须符合要求

【级别】建议

【描述】图形的宽度为840px/520px/220px，可根据图形大小设置接近的宽度。高度没有限制，请根据绘图实际情况等比例缩放。

### API-FIGURE-02 图元须增加白底背景

【级别】强制

【描述】完成绘制后，须点击页面适应到绘图，并增加白底背景，以确保在IDE和浏览器的深色风格下，仍能够清晰地显示图中的深色线条。

### API-FIGURE-03 图元之间的连线应选择无跳线样式

【级别】建议

【描述】图元之间的连线应选择无跳线样式，避免线条交叉导致图形混乱，影响可读性。


## 13 格式规范

### API-FORMAT-00 列表下的代码块须缩进

【级别】强制

【描述】列表下的代码块须缩进2或4个空格，否则代码块不会被正确渲染为列表的子项。

【反例】（列表下的内容无缩进）：

- tensor前n个数据计算

```cpp
template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
```

【正例】（列表下的代码块缩进4个空格）：

- tensor前n个数据计算

    ```cpp
    template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
    __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```

### API-FORMAT-01 须使用Markdown简易表格

【级别】建议

【描述】建议使用Markdown简易表格。

【例外】部分场景，需要进行合并单元格，能够带来更优的阅读体验，可以使用html语法表格实现该功能。

【反例】：

```text
**表 2**  不同存储单元的对齐要求

<a name="table16278354141117"></a>
<table><thead align="left"><tr id="row1827835418116"><th class="cellrowborder" valign="top" width="24.15%" id="mcps1.2.3.1.1"><p id="p1927845481114"><a name="p1927845481114"></a><a name="p1927845481114"></a>存储单元</p>
</th>
...
```

【正例】：

```text
**表1**  不同存储单元的对齐要求
|存储单元|对齐要求|
|----------|----------|
|Global Memory|无对齐要求。|
|Unified Buffer|32Byte对齐。|
|L1 Buffer|32Byte对齐。|
|L0A Buffer/L0B Buffer|512Byte对齐。|
|L0C Buffer|64Byte对齐。|
|BiasTable Buffer|64Byte对齐。|
|Fixpipe Buffer|64Byte对齐。|
```

### API-FORMAT-02 表格中换行须使用`<br>`

【级别】建议

【描述】Markdown简易表格中增加换行应使用`<br>`。

### API-FORMAT-03 链接引用须使用相对路径

【级别】强制

【描述】同一个仓内的API和编程指南、example之间的链接均须使用相对路径。

### API-FORMAT-04 无序列表仅在有多个条目时使用

【级别】建议

【描述】当只有一个条目时，不应使用项目符号，直接以正文形式呈现即可。

### API-FORMAT-05 锚点须使用有意义的名称

【级别】强制

【描述】文档中的锚点（`<a id="...">`或`#锚点`）须使用有实际含义的名称，不得使用无意义的自动生成ID（如`section184751024101111`）。

【反例】

```
详细情况参考[Lock约束说明](Lock.md#section184751024101111)。
```

【正例】

```
支持的流水参考[硬件流水类型](../core_sync_overview.md#硬件流水类型)。
```

## 14 芯片版本过滤定制

内容定制是指通过芯片过滤标签（npu标签）实现同一篇API文档按不同芯片版本动态展示不同内容的能力。

### API-CUSTOM-00 芯片过滤标签须成对使用

【级别】强制

【描述】芯片过滤标签`<!-- npu="..." -->`和`<!-- end -->`须成对使用，包裹需要按芯片版本过滤的内容。标签中的npu属性值须与产品系列一一对应，多个产品系列以逗号分隔。

【正例】

```text
<!-- npu="950" id27 -->
本接口在Ascend 950PR/Ascend 950DT上支持counter模式。
<!-- end id27 -->
```

### API-CUSTOM-01 同一文档内的id须唯一

【级别】强制

【描述】同一篇md文件中，所有芯片过滤标签的id属性值须唯一，不得重复。id重复会导致文档过滤发布时产生未知错误。

## 15 接口废弃或变更规范

### API-DEPRECATED-00 接口废弃时须同步更新废弃接口文档列表

【级别】强制

【描述】当接口废弃时，须在接口文档中做相应的修改：标题后增加（废弃），增加替代接口的链接（如有），并在附录中的废弃接口列表中记录废弃信息。

【正例】

asc_get_ar_spr接口文档示例如下：

```text
# asc_get_ar_spr (废弃)

## 功能说明

头文件路径：`"c_api/sys_var/sys_var.h"`。

**该接口已废弃，请使用[asc_get_squeeze_status](asc_get_squeeze_status.md)实现此功能。**
```

附录中的废弃接口列表示例如下：

```text
### 系统变量

- [asc_get_ar_spr](../SIMD-API/C-API/sys_var/asc_get_ar_spr_deprecated.md)接口

    此接口后续版本会废弃，请使用[asc_get_squeeze_status](../SIMD-API/C-API/sys_var/asc_get_squeeze_status.md)接口。
```

### API-DEPRECATED-01 接口文档中引用其他接口时不得使用已废弃或已更名的接口

【级别】强制

【描述】接口文档中出现其他接口引用时（包括链接、参数说明、调用示例等），须使用当前有效的接口名称，不得引用已经废弃或已更名的旧接口名称。如被引用的接口已改名，须同步更新引用处为新接口名称。

## 16 附录

### markdown写作规范

[markdown写作规范](https://gitcode.com/cann/community/blob/master/contributor/docs/document_writing_specs.md)总结了最常用的写作规则和示例，便于快速学习和写作时查阅。

### 文档低错CheckList

文档提交前须按如下检查清单逐项排查，确保不存在低级错误。

| 序号 | 类别 | 检查项 |
| --- | --- | --- |
| 1 | 标点符号 | 中文描述中须使用中文标点，不得出现英文标点符号。 |
| 2 | 标点符号 | 标点符号须完整、准确，避免括号未闭合、标点缺失或多余的情况。 |
| 3 | 标点符号 | 中英文混排时，英文单词前后不加空格。 |
| 4 | 数据类型 | 文档中的数据类型写法须与代码中的数据类型写法保持一致。数据类型的写作顺序须保持一致（原则：位宽从小到大、有符号数→无符号数→浮点数、浮点数按指数位大小排序）：bool、int4b_t、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。接口支持的数据类型只与位宽有关时，可通过位宽表达：b8、b16、b32、b64。 |
| 5 | 术语 | 须使用统一的概念术语，如L1 Buffer、L0A Buffer、AI Core等。术语表参考：[LINK](./guide/technical_appendix/concepts_and_terms/glossary.md)。 |
| 6 | 预览格式 | 预览后须检查缩进、换行是否正确，避免出现文字与代码/表格/图片连在同一行等问题。 |
| 7 | 语句逻辑 | 语句须通顺、逻辑清晰，避免出现语病或语义歧义。例如，"本节主要描述不同数据类型之间精度转换时的舍入行为"不应写成"本节主要描述不同数据类型之间精度转换时的舍入行为进行介绍"。 |
| 8 | 链接引用 | 链接引用规则：同一个代码仓内编程指南和API、example样例之间的链接均使用相对链接。 |
| 9 | 列表 | 当只有一个条目时，不应使用无序列表符号，直接以正文形式呈现。 |





