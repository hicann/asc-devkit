# Ascend C API Writing Guidelines

## 1 Overview

### 1.1 Objectives and Scope

**Objectives**

These guidelines aim to help Ascend C API documentation developers produce complete, accurate, consistent, practical, and user-friendly API reference documentation, enabling users to access a highly self-supporting tool manual. By unifying writing standards, these guidelines ensure that all API documentation remains consistent in structure organization, content presentation, and parameter description, reducing the user learning curve and improving development efficiency.

**Scope**

These guidelines apply to the writing and review of all Ascend C API reference documentation.

Document developers must follow these guidelines when adding or modifying API reference documentation. Document reviewers must use these guidelines as the review basis.

### 1.2 General Principles

Ascend C API documentation must follow these general principles:

- **Accuracy**: Documentation content must be strictly consistent with the code implementation, including function prototypes, parameter names, data types, and constraint conditions, ensuring that users do not encounter ambiguity when using the documentation.
- **Completeness**: Each API must cover necessary sections such as product support, function description, function prototype, parameter description, constraints, and usage examples. Missing key information is prohibited.
- **Consistency**: APIs of the same type must maintain uniformity in section structure, terminology, and format presentation, reducing the cost of switching between different APIs.
- **Usability**: Documentation must be organized from the developer perspective. Complex concepts must be explained or linked. Runnable examples must be provided to help developers get started quickly.

### 1.3 Clause Organization

Clauses in these guidelines are organized in the following structure:

- **Clause ID**: Prefixed with "API-", numbered by chapter, such as API-FUNC-00, API-PROTO-00.
- **Clause Title**: A concise description of the requirement.
- **[Level]**: The enforceability of the clause, classified as "Mandatory" or "Recommended".
- **[Description]**: The specific requirement description.
- **[Positive Example]**: A writing example that meets the requirement.
- **[Negative Example]**: A writing example that does not meet the requirement.
- **[Exception]**: Exceptions that are allowed in specific scenarios where the guidelines do not apply.

## 2 Chapter Organization

### API-STRUCT-00 Synchronize Index File When Navigation Directory Structure or Node Title Changes

[Level] Mandatory

[Description] The Ascend C API navigation directory structure (the directory structure in the left navigation panel of static pages or Ascend community documentation) is organized through an [index file](../zh/api/README.md) that maps source files. The index file describes the hierarchical relationship between nodes in a list format. The source file directory structure must be consistent with the navigation directory structure. Each node in the navigation directory structure corresponds to an md file:

- **Directory node** (parent node): Corresponds to an index file with the same name as the source directory (such as `basic_api.md`), used to organize child nodes.
- **Leaf node** (specific API): Corresponds to an API content md file.

[Positive Example]

Assume we want to present the following directory tree navigation structure (the effect displayed on static pages or Ascend community documentation pages):
``` text 
|-- SIMD API  
|--|-- Basic API
|--|--|-- Add
|--|--|-- Sub
|--|-- C API
|--|--|-- asc_add
|--|--|-- asc_sub
|-- SIMT API
|--|-- acosf
|--|-- asinf
```

The corresponding source file directory structure should be organized as follows:
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

The content of the corresponding index file README.md is:

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [Basic API](SIMD-API/basic_api/basic_api.md)
        -   [Add](SIMD-API/basic_api/Add.md)
        -   [Sub](SIMD-API/basic_api/Sub.md)
    -   [C API](SIMD-API/c_api/c_api.md)
        -   [asc_add](SIMD-API/c_api/asc_add.md)
        -   [asc_sub](SIMD-API/c_api/asc_sub.md)
-   [SIMT API](SIMT-API/SIMT-API.md)
    -   [acosf](SIMT-API/acosf.md)
    -   [asinf](SIMT-API/asinf.md)
```

### API-STRUCT-01 Different Nodes Must Not Correspond to the Same md Source File

[Level] Mandatory

[Description]

Different nodes in the navigation directory must not correspond to the same md source file. After the documentation is published to the Ascend community, the URL of the official page is constructed from the path of the md source file. If multiple different nodes correspond to the same source file, a URL conflict occurs.

[Negative Example]

In the following index file, both Cube Compute (Tensor_API) and the GEMV node under Cube Compute correspond to the same source file, causing a URL conflict.

``` text
-   [SIMD API](SIMT-API/SIMT-API.md)
    -   [Basic API](SIMT-API/basic_api/basic_api.md)
        -   [Cube Compute](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad Compute](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [Key Features](SIMD-API/basic_api/cube_compute/mmad_compute/features/features.md)
                    -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/features/GEMV.md)
        -   [Cube Compute (Tensor_API)](SIMD-API/basic_api/cube_compute_TensorAPI/cube_compute_TensorAPI.md)
            -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/features/GEMV.md)
``` 
### API-STRUCT-02 API Source Directory and File Naming Conventions

[Level] Mandatory

[Description]

Source directories and file names must use lowercase English letters, with multiple words connected by underscores (_).

[Exception]
- When the API name uses uppercase English camel case, the file name should be consistent with the API name in uppercase style, such as Add.md in the basic API.

- For concepts and terms named in uppercase English, the uppercase style may be retained, such as Neg_ISASI.md in the basic API (ISASI is a concept term).


### API-STRUCT-03 Navigation Directory Levels Must Not Exceed 5

[Level] Mandatory

[Description]

The navigation directory levels must not exceed 5. Excessive nesting affects the reading experience, and content beyond this level cannot be published to the Ascend community official website.

[Negative Example]

In the following index file, `GEMV` is a level-6 directory, which does not meet the requirement.

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [Basic API](SIMD-API/basic_api/basic_api.md)
        -   [Cube Compute](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad Compute](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [Key Features](SIMD-API/basic_api/cube_compute/mmad_compute/features/features.md)
                    -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/features/GEMV.md)
```   

### API-STRUCT-04 Single Child Node Under a Navigation Directory Is Not Recommended

[Level] Recommended

[Description]

A navigation directory with only one child node is not recommended, as it results in a poor reading experience and unnecessary nesting.

[Negative Example]

The Key Features directory has only one node, making this level unnecessary.

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [Basic API](SIMD-API/basic_api/basic_api.md)
        -   [Cube Compute](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad Compute](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [Key Features](SIMD-API/basic_api/cube_compute/mmad_compute/feature/feature.md)
                    -   [GEMV](SIMD-API/basic_api/cube_compute/mmad_compute/feature/GEMV.md)
``` 

[Positive Example]
``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [Basic API](SIMD-API/basic_api/basic_api.md)
        -   [Cube Compute](SIMD-API/basic_api/cube_compute/cube_compute.md)
            -   [Mmad Compute](SIMD-API/basic_api/cube_compute/mmad_compute/mmad_compute.md)
                -   [GEMV Key Features](SIMD-API/basic_api/cube_compute/mmad_compute/GEMV_feature.md)
```   


### API-STRUCT-05 Complex APIs Must Be Split by Functional Scenario

[Level] Recommended

[Description] If a complex API has different functional scenarios, split the content by functional scenario.

  - When different chip versions support different API functions, split by chip version.
  - When the same chip version supports different API functions, split by functional scenario.


[Positive Example]

The asc_copy_gm2ub interface in the C API has significant differences between version 2201 and version 3510. Presenting both in a single md file would result in a confusing structure and require too many chip filter tags. In this case, the interface description can be split into two files, distinguished by the architecture version number as a suffix.

```text
            -   [asc_copy_gm2ub](SIMD-API/C-API/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md)
                -   [asc_copy_gm2ub_arch_2201](SIMD-API/C-API/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md)
                -   [asc_copy_gm2ub_arch_3510](SIMD-API/C-API/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md)
```

[Positive Example]

The DataCopy and LoadData interfaces in the basic API each correspond to multiple data transfer scenarios, split into multiple md files for organization.

```text
            -   [Cube Compute Load](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_load.md)
                -   [Overall Description](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/overall_description.md)
                -   [Cube Compute Input Move Constraint](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_input_move_constraint.md)
                -   [L1 Buffer/L0A Buffer/L0B Buffer Memory Structure Introduction](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/L1_L0A_B_memory_structure_intro.md)
                -   [LoadData (GMToL1-2D Matrix Transfer)](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2D.md)
                -   [LoadData (GMToL1-2D Matrix Transfer V2)](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md)
                -   [DataCopy (GMToL1 Continuous Data Transfer)](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md)
                -   [DataCopy (GMToL1 High-Dimensional Split Data Transfer)](SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md)
```


### API-STRUCT-06 The First-Level Heading in Each API Must Match the Navigation Title

[Level] Mandatory

[Description] The first-level heading in each API md file must match the title in the navigation structure, that is, it must be consistent with the link text (content within []) in the index file.

[Negative Example]

The navigation title of the Neg interface is Neg (ISASI), but the heading in Neg_ISASI.md is Neg, which is inconsistent.

The index file content is as follows:

``` text
-   [SIMD API](SIMD-API/SIMD-API.md)
    -   [Basic API](SIMD-API/basic_api/basic_api.md)
        -   [Add](SIMD-API/basic_api/Add.md)
        -   [Neg (ISASI)](SIMD-API/basic_api/Neg_ISASI.md)
    -   [C API](SIMD-API/c_api/c_api.md)
        -   [asc_add](SIMD-API/c_api/asc_add.md)
        -   [asc_sub](SIMD-API/c_api/asc_sub.md)
```   

The content of Neg_ISASI.md is as follows:

``` text
# Neg

## Product Support
...

```   

### API-STRUCT-07 Each API Must Organize Sections in a Fixed Order

[Level] Mandatory

[Description] The md file of each interface must organize sections in the following order (all second-level headings). The Data Type, Key Features, and Required Header Files sections are optional based on the actual interface. For example, the Sin interface in the advanced API supports the same data types across different chips, so no separate section is needed to describe the differences. The description can be included with the parameter description. For another example, the basic API uniformly describes the required header files in the function description, so the "Required Header Files" section is not needed. APIs of the same type should maintain a consistent section structure.

```text
Product Support
Function Description
Function Prototype
Parameter Description
Data Type
Return Value Description
Constraints
Required Header Files
Key Features
Usage Example
```


## 3 Product Support

### API-PROD-00 Product Support Must Be the First Section

[Level] Mandatory

[Description] The product support section is the first section of the API documentation, placed at the top of the page immediately below the first-level heading. List the support status of each product series in a list format, enabling users to quickly determine whether the API is applicable to the target hardware.

[Exception]
Some interfaces are Host interfaces that do not differentiate by product. In this case, this section is not required.

### API-PROD-01 Product Support Must Cover All Product Series in Chronological Order

[Level] Mandatory

[Description] The product support section must cover all current product series, listing the support status for each one. Omissions are prohibited. Each product series occupies one line in the format `- Product Name: Supported/Not Supported`. If the same product series distinguishes between AI Core and Vector Core, list them separately.

[Positive Example]

```text
## Product Support

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 Inference Products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas Inference Series Products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas Inference Series Products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas Training Series Products: Not supported
<!-- end id7 -->
```

### API-PROD-02 Product Support Must Use Chip Filter Tags

[Level] Mandatory

[Description] The support status of each product series must be wrapped in HTML comment-style chip filter tags (`<!-- npu="..." -->` and `<!-- end -->`) so that the Ascend community official website can dynamically filter and display content by chip version. The npu attribute value in the filter tag must correspond to the product series one-to-one.

[Positive Example]

```text
## Product Support

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products: Not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products: Not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 Inference Products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas Inference Series Products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas Inference Series Products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas Training Series Products: Not supported
<!-- end id14 -->
```

## 4 Function Description

### API-FUNC-00 Function Description Must Cover Function Purpose, User Value, and Basic Usage

[Level] Recommended

[Description] The function description must clearly address the following:

  - Function Purpose: What is the function (purpose, objective) of the API?
  - User Value: What changes in the user experience after using this API?
  - Basic Usage: How to use this API?

[Positive Example]

```text  
Set the Mask mode to Counter mode. In this mode, developers do not need to be aware of iteration counts or handle unaligned tail blocks. The calculation data volume can be passed directly, and the actual iteration count is automatically inferred by the Vector compute unit. This interface is recommended for use with the isSetMask template parameter in the API. When isSetMask is false, users can call this interface to manually manage Counter mode and set the number of elements participating in the calculation in Counter mode through [SetVectorMask](./SetVectorMask.md).
```


The above interface description clearly defines the function purpose, user value, and basic usage.

- Function Purpose: Set the Mask mode to Counter mode.
- User Value: In this mode, developers do not need to be aware of iteration counts or handle unaligned tail blocks. The calculation data volume can be passed directly, and the actual iteration count is automatically inferred by the Vector compute unit.
- Basic Usage: This interface is recommended for use with the isSetMask template parameter in the API. When isSetMask is false, users can call this interface to manually manage Counter mode and set the number of elements participating in the calculation in Counter mode through SetVectorMask.

[Negative Example]

The following function description only lists the function of the interface without providing user value or basic usage:

```text
Obtain the preload status of ICache.
```

### API-FUNC-01 Header File Information Must Be Provided

[Level] Mandatory

[Description] The header file where the interface is located and the header files that need to be included when using the interface must be provided. If a general introduction has been provided in an earlier section by API category or in a separate section, the function description does not need to repeat this information. The header file path must be wrapped in backticks.

[Positive Example]  
The header file path is: `"basic_api/kernel_operator_vec_binary_intf.h"`.

[Exception]  
- If a separate "Required Header Files" section exists to describe the required header files, this section is not needed.
- If the required header files for a class of interfaces have been described in an earlier section, this section is not needed.

### API-FUNC-02 New Concepts Must Include Necessary Explanations or Hyperlinks

[Level] Recommended

[Description] When describing functions, if a concept not introduced in this API is referenced, add a necessary explanation or a hyperlink to the relevant concept.

[Positive Example]
The following function description directly provides an introduction to Counter mode.

```text
Set the Mask mode to Counter mode. In this mode, developers do not need to be aware of iteration counts or handle unaligned tail blocks. The calculation data volume can be passed directly, and the actual iteration count is automatically inferred by the Vector compute unit. This interface is recommended for use with the isSetMask template parameter in the API. When isSetMask is false, users can call this interface to manually manage Counter mode and set the number of elements participating in the calculation in Counter mode through [SetVectorMask](./SetVectorMask.md).
```

### API-FUNC-03 Cross-API References Must Include Hyperlinks to Related APIs

[Level] Recommended

[Description] If this API needs to be used in conjunction with other APIs, describe the relationship. If other APIs have similar functions, describe the differences and add hyperlinks to the related APIs.

[Positive Example]
The SetAippFunctions interface must be used in conjunction with the LoadImageToLocal interface, and a hyperlink to LoadImageToLocal is added.

```text
Set parameters related to image preprocessing (AIPP, AI Core pre-process). Used in conjunction with the [LoadImageToLocal](./LoadImageToLocal.md) interface. After setting, calling the LoadImageToLocal interface can complete image preprocessing operations during data transfer: including data padding, channel swapping, single-row reading, data type conversion, channel padding, and color space conversion.
```

[Positive Example]
The TransDataTo5HD interface describes the difference from the Transpose interface when performing transpose operations.

```
Data format conversion, generally used to convert NCHW format to NC1HWC0 format. Specifically, it can also be used for transposing two-dimensional matrix data blocks. When performing transpose operations, compared to the [Transpose](./Transpose.md) interface, Transpose only supports 16x16 matrix transposition; this interface can process 512 Byte of data (16 datablocks) in a single repeat. Depending on the data type, it supports matrix transposition of different shapes (for example, when the data type is half, a single repeat can complete a 16x16 matrix transposition), and it also supports multiple repeat operations.
```

[Negative Example]
The SetFixPipeConfig interface works in conjunction with the DataCopy interface to achieve the same function as the Fixpipe interface, but SetFixPipeConfig neither provides a link to the Fixpipe interface nor a comparison of the two approaches.

```text
In the DataCopy (CO1->GM) process, the relu and quant processes are involved, used for relu and quant calculations respectively. This interface sets the source operands for relu and quant.
```

### API-FUNC-04 Computation Interfaces Must Use Formulas to Describe Functions

[Level] Recommended

[Description] For computation interfaces, if using a formula can help users understand the interface function, add a formula description. Variable names in the formula must be consistent with those in the function prototype. Mathematical expressions in the function description should reuse mathematical symbols and conform to LaTeX syntax, ensuring concise and clear content.

[Positive Example]
The following interface provides the multiplication formula using the standard `\times` notation.

```text
This interface performs element-wise multiplication on input data srcReg0 and srcReg1 according to the mask, and writes the result to dstReg. The calculation formula is as follows:

$$
dstReg_i = srcReg0_i \times srcReg1_i
$$
```


[Negative Example]
The following interface provides the multiplication formula using the non-standard `*` notation.

```text
This interface performs element-wise multiplication on input data srcReg0 and srcReg1 according to the mask, and writes the result to dstReg. The calculation formula is as follows:

$$
dstReg_i = srcReg0_i * srcReg1_i
$$
```

### API-FUNC-05 Complex Interfaces Must Include Illustrations

[Level] Recommended

[Description] When the interface function is complex or difficult to express in text, add clear illustrations. Illustrations must follow the [Figure Guidelines](#12-figure-guidelines).

[Positive Example]
The `ReduceSum` interface includes an illustration of the summation calculation.

```text
The `ReduceSum` interface performs summation on all input data. The calculation process is as follows.

**Figure 1** `ReduceSum` Calculation Diagram<a id="fig1"></a>

![ReduceSum Calculation Diagram](api/figures/ReduceSum_basic_api.png "ReduceSum Calculation Diagram")
```

[Negative Example]
The function description of Gather does not include an illustration, making it difficult to understand through text alone.

```text
Given an input tensor and an address offset tensor, the Gather instruction collects elements from the input tensor to the result tensor based on the offset addresses.
```

## 5 Function Prototype

### API-PROTO-00 Multiple Function Prototypes Must Be Split by Structure or Presented as Unordered Lists

[Level] Recommended

[Description] If an API has multiple function prototypes, split them by different structures in the parameters or present them as unordered lists, and describe the differences between the prototypes in text. Subsequent content that needs to distinguish between different prototypes must be consistent with the introduction order of the function prototypes.

[Positive Example]

```text
- Entire tensor computation

    ```cpp
    dst = src0 + src1;
    ```

- First n elements of tensor computation

    ```cpp
    template <typename T>
    __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- High-dimensional split computation
    - Mask bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - Mask continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```
```

[Exception]
For scenarios with too many function prototypes, using only unordered lists would result in deep nesting and excessive length. A combination of unordered lists and code comments can be used for splitting.

```text
- First n elements computation
    ```cpp
    // RINT rounding mode
    __aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // ROUND rounding mode
    __aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // FLOOR rounding mode
    __aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // CEIL rounding mode
    __aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // TRUNC rounding mode
    __aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    ```

- High-dimensional split computation
    ```cpp
    // RINT rounding mode
    __aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // ROUND rounding mode
    __aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // FLOOR rounding mode
    __aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // CEIL rounding mode
    __aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // TRUNC rounding mode
    __aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```
```

### API-PROTO-01 Function Prototype Must Be Strictly Consistent with Header File

[Level] Mandatory

[Description] The function prototype must be strictly consistent with the prototype definition in the header file. The function name, parameter names (including template parameters), parameter types (including template parameters), return value data type, and function qualifiers must all be consistent. The number of prototypes must also be strictly consistent with the header file.

### API-PROTO-02 Function Prototype Must Use codetype cpp Format

[Level] Mandatory

[Description] Function prototypes must be presented in codetype cpp format without a trailing semicolon.

[Positive Example]

```text

- First n elements of tensor computation

    ```cpp
    template <typename T>
    __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

```

### API-PROTO-03 Supported Hardware Models Must Be Specified for Each Prototype

[Level] Mandatory

[Description] When different prototypes support different chip models, the supported hardware models for each prototype must be clearly specified.

```text
<!-- npu="950,A3,910b,310b,x90,9030" id15 -->
- This prototype supports the following product models:

    <!-- npu="950" id16 -->
    Ascend 950PR/Ascend 950DT 
    <!-- end id16 -->

    <!-- npu="A3" id17 -->
    Atlas A3 Training Series Products/Atlas A3 Inference Series Products 
    <!-- end id17 -->

    <!-- npu="910b" id18 -->
    Atlas A2 Training Series Products/Atlas A2 Inference Series Products 
    <!-- end id18 -->

    <!-- npu="310b" id19 -->
    Atlas 200I/500 A2 Inference Products 
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
- This prototype supports the following product models:

    <!-- npu="310p" id23 -->
    Atlas Inference Series Products AI Core 
    <!-- end id23 -->

    <!-- npu="310p" id24 -->
    Atlas Inference Series Products Vector Core 
    <!-- end id24 -->

    <!-- npu="910" id25 -->
    Atlas Training Series Products 
    <!-- end id25 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_MTE3, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id22 -->
```

## 6 Parameter Description

### API-PARAM-00 Template Parameters and Input Parameters Must Be Described in Separate Tables

[Level] Mandatory

[Description] For APIs with template parameters, the template parameters and input parameters must be described in two separate tables, in the order of template parameter description followed by parameter description. Parameter descriptions must be complete. Missing one or more parameter descriptions is prohibited.

[Positive Example]

```text
**Table 1**  Template Parameter Description

| Parameter | Description |
| --- | --- |
| T | Operand data type. |

**Table 2**  Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>Type is [LocalTensor](LocalTensor.md), supported TPosition is VECIN/VECCALC/VECOUT.<br>The starting address of LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operands.<br>Type is [LocalTensor](LocalTensor.md), supported TPosition is VECIN/VECCALC/VECOUT.<br>The starting address of LocalTensor must be 32-byte aligned.<br>The data types of both source operands must be consistent with the destination operand. |
| count | Input | Number of elements participating in the calculation. |
```

### API-PARAM-01 Parameter Names Must Be Strictly Consistent with Function Prototype

[Level] Mandatory

[Description] Parameter names must be strictly consistent with the function prototype. Parameter names must not be bolded.

### API-PARAM-02 Input/Output Must Be Clearly Specified

[Level] Recommended

[Description] Input indicates input parameters, and output indicates output parameters. Fill in the actual situation.

### API-PARAM-03 Parameter Usage Scenarios and Functions Must Be Described

[Level] Recommended

[Description] Parameter descriptions must describe the usage scenarios and functions of the parameters.

### API-PARAM-04 Parameter Values Must Be Clearly Specified

[Level] Recommended

[Description] For numeric parameters, list the unit (no space between the number and the unit; use standard units such as 16 bytes or 16B), value range, default value, and supported data types. If the unit, value range, and default value are already defined in the interface prototype and do not require special explanation, they may be omitted. Parameter values with special meanings must not use "magic numbers"; add an explanation of the meaning of the value. Parameter value information that varies by hardware model must be listed separately by model.

Parameter value data types:
- Data types that are fixed in the prototype do not need separate explanation.
- Data types with multiple possibilities must specify the supported data types.
- When the interface supports data types only related to bit width, they can be expressed by bit width: b8, b16, b32, b64. For the complete ordering and writing of data types, refer to [Data Type Guidelines](#api-software-01-data-types-must-be-arranged-in-a-fixed-order).

[Positive Example]
The unit and supported data types of the repeatTime parameter can be determined from the prototype definition, but its value range differs from the general description of the basic API. This section specifically describes its value range.

| Parameter | Input/Output | Description |
|---|---|---|
| repeatTime | Input | Number of iterations. Unlike the general parameter description, it supports a larger value range, as long as it does not exceed the maximum value of int32_t. |

[Negative Example]
The value range and unit of srcBlkStride are not provided.

| Parameter | Input/Output | Description |
|---|---|---|
| srcBlkStride | Input | Represents the address stride between different datablocks of the vector source operand within a single iteration. |

### API-PARAM-05 Data Structure Parameters Must Include Definitions and Detailed Descriptions

[Level] Recommended

[Description] For data structure parameters, list the data structure definition (if the definition is too long, it may not be included in the document, but the header file directory must be provided) and provide a detailed description (or a hyperlink to the detailed description). If a structure parameter is used by multiple APIs, and presenting it in a single API is not appropriate, extract the structure definition to a common location and add a link to the structure definition in this API.

[Positive Example]
dataCopyParams provides the header file directory of its data type and a detailed description.

```text
**Table 2**  Interface Parameter Description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md), storage location is Unified Buffer, destination address must be 32-byte aligned. |
| src | Input | Source operand, type is [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md), storage location is Global Memory, source address must be 1-byte aligned. |
| dataCopyParams | Input | Transfer parameters. DataCopyExtParams type, for specific parameter descriptions, refer to [Table 3](#table_gm2ub_pad_3). |
| padParams | Input | When transferring data from Global Memory to Local Memory, data can be padded on the left or right according to developer needs. padParams is the parameter used to control the data padding process. DataCopyPadExtParams type, for specific parameters, refer to [Table 4](#table_gm2ub_pad_4). |

The structure parameter definitions listed in the following table are available in `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the CANN software installation file storage path.

**Table 3**  DataCopyExtParams Structure Parameter Definition<a name="table_gm2ub_pad_3"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Number of data blocks to transfer, data type is uint16_t, value range: blockCount in [0, 4095], default value is 1. |
| blockLen | Length of each data block to transfer, data type is uint32_t, value range: blockLen in [0, 2097151], unit: 1B.<br>**blockLen must be an integer multiple of sizeof(T). Ensure it does not exceed the UB space size.** |
| srcStride | Interval between adjacent data blocks of the source operand (the difference between the **end address** of the previous data block and the **start address** of the next data block), data type is uint32_t, value range: srcStride in [0, 2^32 - 1], unit: 1B. The data type and supported value range of srcStride may differ across products. For details, refer to [Constraints](#section633mcpsimp). |
| dstStride | Interval between adjacent data blocks of the destination operand (the difference between the **end address** of the previous data block and the **start address** of the next data block), data type is uint32_t, value range: dstStride in [0, 2^32 - 1], unit: dataBlock (32B). The data type and supported value range of dstStride may differ across products. For details, refer to [Constraints](#section633mcpsimp). If PaddingMode is Compact mode, this parameter is invalid, default value is 0.<br>**Note: Ensure it does not exceed the UB space size.** |
| rsv | Reserved field. Data type is uint16_t, default value is 0. |

```

### API-PARAM-06 Parameter Units Must Explain Principles and Meanings

[Level] Recommended

[Description] For parameter units, explain the principles and meanings so that developers can understand why the unit is used and why different data types have different units.

[Positive Example]
Clearly states that the unit of dstRepStride is the length after one repeat reduction. When introducing specific values later, developers can understand why the unit is used and why different data types have different units.

| Parameter | Input/Output | Description |
|---|---|---|
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand. The unit is the length after one repeat reduction. Each repeat (8 DataBlocks) produces 8 elements after reduction, so when the input type is half, the RepStride unit is 16 Byte; when the input type is float, the RepStride unit is 32 Byte. |

[Negative Example]
Only lists the parameter units as 16 Bytes and 32 Bytes without the necessary principle and meaning explanation. Developers cannot understand why the unit is used and why different data types have different units, making it difficult to remember.

| Parameter | Input/Output | Description |
|---|---|---|
| dstRepStride | Input | Represents the address stride of the same datablock between adjacent iterations of the vector destination operand. When the input type is half, the RepStride unit is 16 Bytes; when the input type is float, the RepStride unit is 32 Bytes. |

### API-PARAM-07 Supported Data Types or Value Ranges Must Be Listed by Hardware Model

[Level] Recommended

[Description] When parameter supported data types differ across hardware models, list them separately.

```text
**Table 2**  Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| flagId | Input | Flag for inter-core synchronization. For the value range of flagId on different products, refer to [flagId Value Range](#flagid-value-range).


### flagId Value Range<a id="flagid-value-range"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2, the supported value range is 0-15.
<!-- npu="950" id26 -->
- For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is mode 4, the supported value range is as follows:
    - CrossCoreSetFlag operations with flagId 0-10 initiated by AIV0 correspond to CrossCoreWaitFlag operations with flagId 0-10 on AIC.
    - CrossCoreSetFlag operations with flagId 0-10 initiated by AIV1 correspond to CrossCoreWaitFlag operations with flagId 16-26 on AIC.
    - CrossCoreSetFlag operations with flagId 0-10 initiated by AIC correspond to CrossCoreWaitFlag operations with flagId 0-10 on AIV0.
    - CrossCoreSetFlag operations with flagId 16-26 initiated by AIC correspond to CrossCoreWaitFlag operations with flagId 0-10 on AIV1.
<!-- end id26 -->
```

### API-PARAM-08 Complex Parameter Configuration Methods Must Be Explained

[Level] Recommended

[Description] For complex or specially required configuration methods, provide an explanation of the configuration approach.

[Positive Example]
When isHasfinish is configured as false, finishedLocal only needs to be defined without assignment. This section provides a specific construction example for finishedLocal.

| Parameter | Input/Output | Description |
|---|---|---|
| finishedLocal | Input | Source operand. Used to specify that certain rows have invalid sorting, with shape (outter, 1). Type is LocalTensor, supported TPosition is VECIN/VECCALC/VECOUT. This parameter works with the template parameter isHasfinish. In Normal mode, isHasfinish can be configured as true/false; in Small mode, isHasfinish can only be configured as false. |

  - When isHasfinish is configured as true
    - When the value of the outter row corresponding to finishedLocal is true, the row sorting is invalid, and the k index values in the output dstIndexLocal after sorting will all be set to n.
    - When the value of the outter row corresponding to finishedLocal is false, the row sorting is valid.
  - When isHasfinish is configured as false, finishedLocal only needs to be defined without assignment. Pass the defined finishedLocal to the interface. The definition example is as follows:

```
LocalTensor<bool> finishedLocal;
```

### API-PARAM-09 Parameter Descriptions Must Include Hyperlinks to Related Concepts and APIs

[Level] Recommended

[Description] When describing parameters, if a concept not introduced in this API is referenced, add a hyperlink to the relevant concept. If this parameter needs to be used in conjunction with other APIs, describe the relationship. If other APIs/parameters have similar functions, describe the differences and add hyperlinks to the related APIs/parameters.

[Positive Example]
The IBSHARE parameter is used in conjunction with the IBShare template. The parameter description describes the relationship and adds a hyperlink to the related parameter/API.

```text
| Parameter | Description |
|---|---|
| IBSHARE | Whether to enable IBShare. The function of IBShare is to reuse the same A matrix or B matrix data on L1. It does not support reusing both A matrix and B matrix data simultaneously, that is, only one of the A matrix and B matrix can have IBShare enabled at a time. Used in conjunction with the IBShare template in [Matmul Template Parameters](./Matmul_template_params.md). For specific parameter settings, refer to Table 2. |
```

### API-PARAM-10 Constraint Information for Individual Parameters Must Be Provided Inline

[Level] Recommended

[Description] Constraint information for individual parameter descriptions must be provided inline.

> The constraints provided in parameter descriptions are typically summary descriptions. In the constraints section, provide specific constraint information such as special values, exception handling, and multi-parameter associated constraints. Some overlap between the two is acceptable.

### API-PARAM-11 Complex Parameter Functions Must Include Illustrations

[Level] Recommended

[Description] When parameter functions are complex or difficult to express in text, add clear illustrations after the parameter table. Illustrations must follow the [Figure Guidelines](#12-figure-guidelines).

[Positive Example]
The usage description of DataCopy includes an illustration of the DataCopyParams structure parameter configuration.

```text
The following example demonstrates the usage of DataCopyParams structure parameters. The example completes the transfer of 2 continuous data blocks, each containing 8 datablocks, with no interval between adjacent data blocks of the source operand, and an interval of 1 datablock between the tail and head of adjacent data blocks of the destination operand.

![DataCopyParams Structure Parameter Usage Diagram](api/figures/repeat-times.png)
```

## 7 Return Value Description

### API-RET-00 Return Value Must Be Strictly Consistent with Function Prototype

[Level] Mandatory

[Description] The return value must be strictly consistent with the return value type in the function prototype.

### API-RET-01 Return Value Units and Specific Value Meanings Must Be Listed

[Level] Mandatory

[Description] List the units and specific value meanings of the return value.

[Positive Example]

The return value is true/false. true indicates that the maximum and minimum temporary space sizes required for the internal calculation of the Topk interface were successfully obtained; false indicates that the acquisition failed.

## 8 Constraints

### API-CONST-00 Constraints Must Be Comprehensive and Reasonable

[Level] Recommended

[Description] Excessive constraints impose a burden on users and affect the documentation and software usage experience. When adding constraints, fully consider whether the software design and implementation have room for optimization, and avoid adding unreasonable constraints. Consider the following aspects when determining which constraints to add:

  - Whether there are constraints on address reuse between source and destination operands (such constraints are mandatory for basic APIs).
  - Whether operand address alignment has requirements (such constraints are mandatory for basic APIs).
  - Constraints on combinations of multiple parameters or between parameters.
  - Parameter names in the description must be consistent with those in the function prototype.
  - In terms of parameter configuration, whether there are additional considerations compared to the general API parameter introduction.
  - Whether there are aspects in API usage, parameter configuration, or calculation result layout that developers may easily misunderstand or confuse.
  - Consider the impact of the interface on the current environment: after calling this interface, whether other APIs need to be called to restore the current environment.
  - Whether the API has differences in support for special data types (such as int4_t) and what to note when using them.
  - Whether the API has constraints on development methods, and whether both Kernel direct call and project-based development are supported.
  - Whether the API has performance constraints, such as reduce fp16 performance being worse than fp32.

Interface constraints must be comprehensive to ensure documentation completeness.

[Positive Example]

```text
This interface only takes effect when the isSetMask template parameter of the vector computation API is false. After use, call [ResetMask](./ResetMask.md) to restore the mask to the default value.
```

## 9 Usage Examples

### API-EXAMPLE-00 Key Code Snippets Must Be Presented in codetype cpp Format

[Level] Recommended

[Description] Only key code snippets are needed. Complete samples must be submitted to the sample repository or presented separately. Present in codetype cpp format. If result examples help understand the API function, provide them by usage example scenario.

[Positive Example]

```cpp
AscendC::Tpipe pipe;
AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained through Host-side tiling parameters
AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor();
// Input shape is 1024, operator input data type is half, actual computation count is 512
AscendC::Cos(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

Result example:

```
Input data (srcLocal): [0.5047314 0.1864135 ... -0.73319215 0.84544605]
Output data (dstLocal): [0.8753044 0.98267525 ... 0.7430419 0.6633976]
```

### API-EXAMPLE-01 Usage Examples with Complete Samples Must Include Links to the Samples

[Level] Recommended

[Description] If a usage example has a matching complete sample, include a link to the complete sample in the usage example section (using relative paths).

[Positive Example]

```text
For a detailed example, refer to [ReduceMax Sample](../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- High-dimensional split computation sample - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all half type, srcLocal computation data volume is 8320, and continuously arranged, index values are needed, use tensor high-dimensional split computation interface, set repeatTime to 65, mask for all elements participating in the calculation
    int32_t mask = 128;
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- High-dimensional split computation sample - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all half type, srcLocal computation data volume is 8320, and continuously arranged, index values are needed, use tensor high-dimensional split computation interface, set repeatTime to 65, mask for all elements participating in the calculation
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```
```
### API-EXAMPLE-02 Sample Code Must Include Comments

[Level] Recommended

[Description] Input parameters, key interfaces, and complex logic code in sample code must include comments explaining the configuration rationale. Example code must match the function description or performance optimization approach.

### API-EXAMPLE-03 Sample Code Must Comply with Coding Standards and Interface Constraints

[Level] Mandatory

[Description] Sample code must comply with coding standards and interface constraints. The order of usage examples must be consistent with the order of function prototypes. The number, type, and order of parameters in interface calls must be accurate and consistent with the function prototype.

### API-EXAMPLE-04 Usage Examples Must Not Include Non-Public Interfaces

[Level] Mandatory

[Description] Usage examples must not include interfaces that are not publicly available.

### API-EXAMPLE-05 Usage Examples Must Not Be Strongly Coupled to a Specific Operator Development Method

[Level] Mandatory

[Description] Usage examples must not be strongly coupled to operator development methods (Kernel direct call development and project-based development). APIs that can only be used in one development method are exceptions.

 ### API-EXAMPLE-06 Sample Code Must Use Standard Namespace and Macro Definitions 
 
 
 [Level] Mandatory 
 
 
 [Description] Sample code must not include non-standard usage such as `using namespace AscendC`. When calling interfaces and enumerations, namespace qualifiers must be added.

 [Level] Mandatory

 [Positive Example] Both enumeration values and interface names have the AscendC namespace added.

```cpp
AscendC::DataCopy(src1Local, src1Global[i * tileLength], tileLength);
AscendC::DataCopy(src0Local, src0Global[i * tileLength], tileLength);

// In-loop dependency: first "DataCopy (PIPE_MTE2) writes src0Local", then "Maxs and Mins (PIPE_V) read src0Local".
// Since PIPE_V needs to wait for PIPE_MTE2, the following synchronization must be inserted.
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

AscendC::Maxs(tmpTensor1, src0Local, inputVal, tileLength);
AscendC::Mins(tmpTensor2, src0Local, inputVal, tileLength);
```

[Negative Example] The enumeration value MTE2_V does not have the AscendC namespace.

```cpp
AscendC::DataCopy(src1Local, src1Global[i * tileLength], tileLength);
AscendC::DataCopy(src0Local, src0Global[i * tileLength], tileLength);

// In-loop dependency: first "DataCopy (PIPE_MTE2) writes src0Local", then "Maxs and Mins (PIPE_V) read src0Local".
// Since PIPE_V needs to wait for PIPE_MTE2, the following synchronization must be inserted.
AscendC::SetFlag<MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<MTE2_V>(EVENT_ID0);

AscendC::Maxs(tmpTensor1, src0Local, inputVal, tileLength);
AscendC::Mins(tmpTensor2, src0Local, inputVal, tileLength);
```

## 10 Software Guidelines

### API-SOFTWARE-00 Data Type Writing Must Be Consistent with Code

[Level] Mandatory

[Description] Data type writing in the documentation must be consistent with the data type writing in the code.

### API-SOFTWARE-01 Data Types Must Be Arranged in a Fixed Order

[Level] Recommended

[Description] The writing order of data types must be consistent. The principle is: from small to large bit width, signed -> unsigned -> floating-point, floating-point sorted by exponent bit size. The complete order is as follows:

bool, int4b_t, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

### API-SOFTWARE-02 Bit-Width-Related Data Types May Use Bit Width Notation

[Level] Recommended

[Description] When the interface supports data types only related to bit width, they can be expressed by bit width: b8, b16, b32, b64.

### API-SOFTWARE-03 NAN and INF Writing Must Distinguish Between Concept and Value Scenarios

[Level] Mandatory

[Description] When referencing NAN and INF in documentation, distinguish the usage scenario: use uppercase form (NAN, INF) when describing concepts, and lowercase form (nan, inf) when describing specific values.

## 11 Terminology

### API-TERM-00 Unified Conceptual Terminology Must Be Used

[Level] Mandatory

[Description] Unified conceptual terminology must be used. For the terminology table, refer to: [LINK](../zh/guide/技术附录/概念原理和术语/术语表.md).

## 12 Figure Guidelines

### API-FIGURE-00 Figure Fonts and Sizes Must Meet Requirements

[Level] Mandatory

[Description] Chinese text must use Founder Lanting Hei Simplified, and English text must use Huawei Sans. Chinese text must not be smaller than 9pt (equivalent to 12 pixels), full-width characters; English text must not be smaller than 8pt (equivalent to 10 pixels), half-width characters.

### API-FIGURE-01 Figure Width Must Meet Requirements

[Level] Recommended

[Description] The width of figures is 840px/520px/220px. Set the width close to the figure size. There is no height limit. Scale proportionally according to the actual drawing.

### API-FIGURE-02 Figures Must Have a White Background

[Level] Mandatory

[Description] After completing the drawing, click to fit the page to the drawing, and add a white background to ensure that dark lines in the figure are still clearly visible under the dark theme of IDEs and browsers.

### API-FIGURE-03 Connections Between Figures Must Use No-Jump Line Style

[Level] Recommended

[Description] Connections between figures must use no-jump line style to avoid line crossings that cause visual confusion and affect readability.


## 13 Format Guidelines

### API-FORMAT-00 Code Blocks Under Lists Must Be Indented

[Level] Mandatory

[Description] Code blocks under lists must be indented by 2 or 4 spaces, otherwise the code block will not be correctly rendered as a child of the list.

[Negative Example] (content under list without indentation):

- First n elements of tensor computation

```cpp
template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
```

[Positive Example] (code block under list indented by 4 spaces):

- First n elements of tensor computation

    ```cpp
    template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
    __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```

### API-FORMAT-01 Markdown Simple Tables Must Be Used

[Level] Recommended

[Description] Using Markdown simple tables is recommended.

[Exception] In some scenarios where cell merging provides a better reading experience, HTML table syntax may be used.

[Negative Example]:

```text
**Table 2**  Alignment Requirements for Different Storage Units

<a name="table16278354141117"></a>
<table><thead align="left"><tr id="row1827835418116"><th class="cellrowborder" valign="top" width="24.15%" id="mcps1.2.3.1.1"><p id="p1927845481114"><a name="p1927845481114"></a><a name="p1927845481114"></a>Storage Unit</p>
</th>
...
```

[Positive Example]:

```text
**Table 1**  Alignment Requirements for Different Storage Units
|Storage Unit|Alignment Requirement|
|----------|----------|
|Global Memory|No alignment requirement.|
|Unified Buffer|32-byte alignment.|
|L1 Buffer|32-byte alignment.|
|L0A Buffer/L0B Buffer|512-byte alignment.|
|L0C Buffer|64-byte alignment.|
|BiasTable Buffer|64-byte alignment.|
|Fixpipe Buffer|64-byte alignment.|
```

### API-FORMAT-02 Line Breaks in Tables Must Use `<br>`

[Level] Recommended

[Description] Line breaks in Markdown simple tables should use `<br>`.

### API-FORMAT-03 Link References Must Use Relative Paths

[Level] Mandatory

[Description] Links between APIs, programming guides, and examples within the same repository must use relative paths.

### API-FORMAT-04 Unordered Lists Must Only Be Used When There Are Multiple Items

[Level] Recommended

[Description] When there is only one item, do not use a bullet point. Present it as plain text.

### API-FORMAT-05 Anchors Must Use Meaningful Names

[Level] Mandatory

[Description] Anchors (`<a id="...">` or `#anchor`) in the documentation must use meaningful names. Meaningless auto-generated IDs (such as `section184751024101111`) are prohibited.

[Negative Example]

```
For details, refer to [Lock Constraints](Lock.md#section184751024101111).
```

[Positive Example]

```
For supported pipelines, refer to [Hardware Pipeline Types][../core_sync_overview.md#hardware_pipeline_types].
```

## 14 Chip Version Filter Customization

Content customization refers to the ability to dynamically display different content in the same API document by chip version through chip filter tags (npu tags).

### API-CUSTOM-00 Chip Filter Tags Must Be Used in Pairs

[Level] Mandatory

[Description] Chip filter tags `<!-- npu="..." -->` and `<!-- end -->` must be used in pairs, wrapping the content to be filtered by chip version. The npu attribute value in the tag must correspond to the product series one-to-one, with multiple product series separated by commas.

[Positive Example]

```text
<!-- npu="950" id27 -->
This interface supports counter mode on Ascend 950PR/Ascend 950DT.
<!-- end id27 -->
```

### API-CUSTOM-01 IDs Must Be Unique Within the Same Document

[Level] Mandatory

[Description] Within the same md file, the id attribute values of all chip filter tags must be unique and must not be duplicated. Duplicate ids cause unknown errors during document filter publishing.

## 15 Interface Deprecation or Change Guidelines

### API-DEPRECATED-00 Interface Deprecation Must Synchronize Updates to Deprecated Interface Documentation

[Level] Mandatory

[Description] When an interface is deprecated, the interface documentation must be modified accordingly: add (Deprecated) after the title, add a link to the replacement interface (if any), and record the deprecation information in the deprecated interface list in the appendix.

[Positive Example]

The asc_get_ar_spr interface documentation example is as follows:

```text
# asc_get_ar_spr (Deprecated)

## Function Description

Header file path: `"c_api/sys_var/sys_var.h"`.

**This interface has been deprecated. Use [asc_get_squeeze_status](asc_get_squeeze_status.md) to implement this function.**
```

The deprecated interface list in the appendix example is as follows:

```text
### System Variables

- [asc_get_ar_spr](../SIMD-API/C-API/sys_var/asc_get_ar_spr_deprecated.md) interface

    This interface will be deprecated in future versions. Use the [asc_get_squeeze_status](../SIMD-API/C-API/sys_var/asc_get_squeeze_status.md) interface.
```

### API-DEPRECATED-01 Interface Documentation Must Not Reference Deprecated or Renamed Interfaces

[Level] Mandatory

[Description] When referencing other interfaces in interface documentation (including links, parameter descriptions, usage examples, and so on), use the current valid interface name. Referencing deprecated or renamed old interface names is prohibited. If the referenced interface has been renamed, update the reference to the new interface name.

## 16 Appendix

### Markdown Writing Guidelines

[Markdown writing guidelines](https://gitcode.com/cann/community/blob/master/contributor/docs/document_writing_specs.md) summarize the most commonly used writing rules and examples for quick learning and reference during writing.

### Document Low-Error Checklist

Before submitting documentation, check each item in the following checklist to ensure there are no low-level errors.

| No. | Category | Check Item |
| --- | --- | --- |
| 1 | Punctuation | Chinese descriptions must use Chinese punctuation. English punctuation marks must not appear. |
| 2 | Punctuation | Punctuation must be complete and accurate, avoiding unclosed parentheses, missing or extra punctuation. |
| 3 | Punctuation | In mixed Chinese-English text, no spaces should be added before or after English words. |
| 4 | Data Type | Data type writing in the documentation must be consistent with the code. The writing order of data types must be consistent (principle: from small to large bit width, signed -> unsigned -> floating-point, floating-point sorted by exponent bit size): bool, int4b_t, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64. When the interface supports data types only related to bit width, they can be expressed by bit width: b8, b16, b32, b64. |
| 5 | Terminology | Unified conceptual terminology must be used, such as L1 Buffer, L0A Buffer, AI Core, and so on. For the terminology table, refer to: [LINK](../zh/guide/技术附录/概念原理和术语/术语表.md). |
| 6 | Preview Format | After preview, check whether indentation and line breaks are correct, avoiding text connected to code/tables/images on the same line. |
| 7 | Sentence Logic | Sentences must be fluent and logically clear, avoiding grammatical errors or semantic ambiguity. For example, "This section mainly describes the rounding behavior during precision conversion between different data types" should not be written as "This section mainly describes the rounding behavior during precision conversion between different data types for introduction." |
| 8 | Link Reference | Link reference rule: Links between programming guides, APIs, and example samples within the same code repository must use relative links. |
| 9 | List | When there is only one item, do not use unordered list symbols. Present it as plain text. |
