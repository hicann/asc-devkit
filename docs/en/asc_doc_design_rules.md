# Ascend C Documentation Design Principles

## 1 Overview

### 1.1 Objective

This specification aims to unify the design requirements for Ascend C documentation, improve documentation quality, and help developers efficiently complete the entire process from learning about Ascend C and getting started to proficiently using Ascend C for operator development.

### 1.2 Documentation System Architecture

The Ascend C documentation system consists of **five** core documents, connected through cross-references to form a navigable documentation network.

The Documentation system architecture is shown below:

<img src="figures/asc_docs_architecture.png" alt="Documentation system architecture diagram"  width="850px" height="580px">

**Core Navigation Logic**: Each document is both a **link initiator** (when first mentioning content managed by another document, it links to it) and a **link receiver** (when another document mentions content this document is responsible for, it links back).

**Five-document Positioning and Navigation Principles**:

| Document | Responsible for Clarifying | What Other Documents Get from Here | Outbound Link Directions |
|----------|---------------------------|-----------------------------------|-------------------------|
| **Getting Started Tutorial** | Ascend C overview, environment setup, quick-start hands-on (HelloWorld, first operator) | Other documents can link to the tutorial as a zero-to-one entry point | In-depth programming concepts → link to Programming Guide; first API mention → link to API Reference Manual |
| **Programming Guide** | Core concepts: programming models, programming paradigms, compilation & execution, hardware architecture, advanced programming | Other documents **link back** to the Programming Guide when encountering programming concepts | First API mention → link to API Reference Manual; first optimization/practice topic → link to Operator Practice Reference; architecture version differences → link to Cross-generation Migration Guide |
| **API Reference Manual** | Each interface's parameter definitions, usage constraints, code examples, inter-API relationships | The Programming Guide and Operator Practice Reference **link to** the API Reference Manual for interface details when mentioning an API | Prerequisite concepts → link back to Programming Guide; cross-version API differences → link to Cross-generation Migration Guide |
| **Operator Practice Reference** | How to write operators, how to optimize performance, how to debug, typical cases | Practice and optimization content mentioned briefly in the Programming Guide **links to** the Operator Practice Reference for expansion | First API usage → link to API Reference Manual; programming concepts → link back to Programming Guide; architecture-dependent optimizations → link to Cross-generation Migration Guide |
| **Cross-generation Migration & Compatibility Guide** | API compatibility strategies, inter-architecture changes, specific migration steps | Any document involving version differences, API deprecation/addition **links to** the Migration Guide for migration paths | Concept definitions → link back to Programming Guide; post-migration new APIs → link to API Reference Manual |

**Supplementary Notes**:

- The sample repository (`asc-devkit/examples/`) **is not part of** the Ascend C documentation system, but code examples in the five documents may link to the sample repository
- Technical appendices (glossary, principles, syntax restrictions) are included in the Programming Guide as shared knowledge infrastructure
- The Getting Started Tutorial is an independent document, located at `docs/zh/guide/getting_started/`, containing overview, environment setup, and quick start (SIMD/SIMT)

### 1.3 Design Requirements

This design document defines Ascend C documentation requirements across three dimensions:

| Dimension | Core Question | Target State |
|-----------|--------------|-------------|
| **Discoverability** | Can developers find the information they need within 3 steps? | From "search → guess → trial-and-error" to "navigate → locate → understand" |
| **Readability** | Once found, can it be understood without ambiguity? | From "repeated inference + cross-validation" to "read once and understand" |
| **Completeness** | After understanding, is the information sufficient to complete the task? | From "documentation only covers half, the rest requires trial-and-error" to "follow documentation to complete the task directly" |

### 1.4 Clause Organization

The clauses in this specification are organized as follows:

- **Clause ID**: Prefixed with "DOC-" and numbered by section, such as DOC-DISC-00, DOC-READ-00, and DOC-COMP-00.
- **Clause title**: Concisely describes the clause requirement.
- **[Type]**: The clause type, categorized as either "Principle" or "Specification." A "Principle" is a fundamental, directional guideline; a "Specification" is a specific, actionable behavioral requirement.
- **[Description]**: A detailed explanation of the clause requirements.
- **[Positive example]**: A writing example that complies with the requirements.
- **[Negative example]**: A writing example that does not comply with the requirements.

---

## 2 Discoverability

### DOC-DISC-00 Three-layer Navigation System

[Level] Principle

[Description] Documents provide three levels of navigation to cover different usage scenarios:

| Navigation Layer | Format | Applicable Scenario | Design Requirement |
|-----------------|--------|--------------------|--------------------|
| **Global Layer** | Directory tree | Users know what they are looking for | Directory depth ≤ 5 levels |
| **Decision Layer** | Decision trees / comparison tables / selection guides | Know the requirement but not which option | Every multi-choice fork must have a decision tree or comparison table |
| **Association Layer** | BLinks/specific introductions | Found A but need to understand related B | Five documents interlinked to form a navigable network |

**Specific Requirements**:

- The global layer is the backbone of the documentation system. A directory tree must be used to present modules and their subordinate categories, submodules, and specific documents, ensuring that users can locate target content progressively from a module entry point. The directory hierarchy must not exceed five levels. Entries at the same level must be organized according to a consistent dimension, and their names must accurately reflect document topics.
- The decision layer provides structured comparisons and recommendations as navigation tools when users face a choice among multiple options. A decision tree, comparison table, or selection guide must be provided. The content must cover key differences affecting the choice, applicable scenarios, and recommendation conditions. A default recommendation must be provided when necessary, ensuring that users can make a choice without reading each page individually.
- The association layer establishes a reference network between documents. Related concepts mentioned in a document must be specifically introduced. If a concept is mentioned by multiple documents, it must be extracted into a standalone document, with reference relationships established through links.

[Positive example]

**Global Layer**:

The following document content progressively organizes a directory tree. Entries at the same level use consistent categorization dimensions, the hierarchy is clear, and names accurately reflect document topics, allowing users to quickly locate target interfaces from a module entry point:

``` text
-   AI-Core-SIMD Programming
    -   Pointer-based C Programming
        -   C Programming Overview
        -   Memory Vector Computation Programming
        -   Register Vector Computation Programming
        -   Cube Matrix Computation Programming
    -   Tensor-based C++ Programming
        -   C Programming Overview
        -   Memory Vector Computation Programming
        -   Register Vector Computation Programming
        -   Cube Matrix Computation Programming
        -   Static Tensor Programming
    -   TPipe-TQue Framework Programming
-   AI-Core-SIMT Programming
-   AI-CPU Programming
```

**Decision Layer**:

The following document content provides a comparison table and recommended usage scenarios, enabling users to make a choice without reading each page individually:

| API level | Language | Characteristics | Primary purpose | Recommended usage scenario |
|----------|------|------|----------|----------|
| **TPipe/TQue framework programming API** | **C++** | **Tensor**-based programming that automatically manages memory transfers and synchronization through **TPipe/TQue**, hiding low-level details. | Use the framework to automatically orchestrate data transfers and computation, improving programming usability and development efficiency. | Prefer **C++ Tensor programming & automatic synchronization/memory management & high programming usability** |
| **Basic API** | **C++** | **Tensor**-based programming that provides **complete C++ programming capabilities**. Tensors are allocated through `LocalMemoryAllocator` and similar mechanisms, while developers manage synchronization independently. | Independently manage synchronization and memory layouts, accommodate C\+\+ Tensor development practices, expose all low-level hardware capabilities, and support fine-grained tuning and ultimate performance. | Prefer **C++ Tensor programming & independent synchronization/memory management** |
| **Language extension layer SIMD API** | **C** | **Pointer**-based programming that provides **complete C programming capabilities**. Local memory is managed by declaring static arrays, while developers manage synchronization independently. | Independently manage synchronization and memory, accommodate C development practices, expose all low-level hardware capabilities, and support fine-grained tuning and ultimate performance. | Prefer **pointer programming** |

**Association Layer**:

The following content from the Operator Practice Reference describes a basic vector operator, mentions the vector programming paradigm, and provides a hyperlink to the Programming Guide:

``` text
The process of implementing a basic vector operator kernel in Ascend C is as follows.

-   Operator analysis: Analyze the operator's mathematical expression, inputs, outputs, and computational logic, and identify the Ascend C interfaces that need to be called.
-   Kernel definition: Define the Ascend C operator entry function.
-   Implement the operator class according to the [vector programming paradigm](../../../programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md#section116515238815): Complete the kernel implementation, including the three basic tasks: CopyIn, Compute, and CopyOut.
```

[Negative example]

**Global Layer**:

In the following directory tree, `GEMV` is at directory level 6, which does not meet the requirement:

``` text
-   SIMD API
    -   Basic API
        -   Matrix Computation
            -   Mmad Computation
                -   Key Feature Description
                    -   GEMV
```

**Decision Layer**:

The following comparison table does not provide recommendation conditions, preventing users from quickly selecting the required model:

| Programming model | Supported scope | Chip support |
|----------|----------|----------|
| **SIMD (primary)** | Vector, matrix, and fused computation | All Ascend series |
| **SIMT (auxiliary)** | Vector computation only | Ascend 950PR/Ascend 950DT only |
| **SIMD+SIMT hybrid** | Vector, matrix, and fused computation | Ascend 950PR/Ascend 950DT only |

**Association Layer**:

The following content from the Operator Practice Reference mentions the kernel function but does not explain the rules for kernel functions or provide a link:

``` text
# Kernel Function Definition

Define the kernel function according to the rules described in the kernel function section.
```

### DOC-DISC-01 Five-document Link Interconnection

[Level] Principle

[Description] The five core documents are connected through links to form a navigable network. **Link direction follows the principle of "whoever mentions content managed by another document adds the link"**:

**Link Relationships (9 link rules)**:

| Link Rule | Initiator | Links To | Trigger Condition | Description |
|-----------|-----------|----------|-------------------|-------------|
| L0 | Getting Started Tutorial | Programming Guide | Tutorial mentions programming concepts requiring depth | Tutorial only covers quick-start; in-depth concepts link to Programming Guide |
| L1 | Programming Guide | API Reference Manual | First introduction of new API name | Add link where API name first appears in concept explanation |
| L2 | Programming Guide | Operator Practice Reference | First introduction of practice/optimization topic | Programming Guide mentions briefly, links to Practice Reference for expansion |
| L3 | Programming Guide | Cross-generation Migration Guide | Mentioning architecture version differences / deprecated APIs | See Cross-generation Migration & Compatibility Guide |
| L4 | Operator Practice Reference | API Reference Manual | First use of API in practice case | Add link where API first appears in sample code |
| L5 | Operator Practice Reference | Programming Guide | First introduction of programming concept | Link to authoritative explanation when practice involves programming model concepts |
| L6 | Operator Practice Reference | Cross-generation Migration Guide | Mentioning cross-architecture optimization differences | Link when optimization approach varies by architecture version |
| L7 | API Reference Manual | Programming Guide | First introduction of programming concept | "Prerequisites" section links to concept introduction chapter |
| L8 | API Reference Manual | Cross-generation Migration Guide | Annotating API version differences / deprecation information | API page annotates version range and links |

**Link Rules**:

- Links are added on the **first occurrence** of a concept/API; subsequent occurrences are not re-linked
- Concept references in API pages use inline links near the reference (not centralized at page bottom)
- **Note direction**: Operator Practice Reference → API Reference Manual (to look up interface details); the API Reference Manual does not need reverse links to Operator Practice Reference
- All complete code examples in the documentation reference the [sample repository](../../examples)

[Positive example]

**L0**: The following Getting Started Tutorial mentions the concept of a kernel function and links its details to the Programming Guide.

```text
3. **Launch the NPU compute task**: Call the [kernel function](../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md) written in advance on the Kernel side, and the NPU starts parallel computation.
```

**L1**: The following Programming Guide introduces the CrossCoreSetFlag and CrossCoreWaitFlag APIs for the first time and adds the corresponding hyperlinks.

```text
Operators can be classified into three types based on their computational characteristics: Cube operators (matrix computation), Vector operators (vector computation), and CV fusion operators (hybrid matrix and vector computation). The operator type determines the selection of the inter-core synchronization method and group configuration mode. For different operator scenarios, C++ Tensor programming combines the [CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) interfaces to implement inter-core synchronization and satisfy diverse operator development requirements.
```

**L2**: The following Programming Guide mentions VF fusion optimization and VF loop optimization and links to the corresponding documents in the Operator Practice Reference.

```text
**Tip**: A larger VF fusion scope is not necessarily better. An excessively large fusion granularity may cause register overflow and instead reduce performance. For more optimization methods, see [VF Fusion Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md) and [VF Loop Optimization](../../../../operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md).
```

**L3**: The following Programming Guide mentions architectural version differences and links to the corresponding document in the Cross-Generation Migration Compatibility Guide.

```text
Compared with NPU architecture version 2201, the main differences in NPU architecture version 3510 are as follows:

**Table 2**  Differences between Membase and Regbase

| Computation method | Data staging location | Characteristics | Applicable scenario |
| --- | --- | --- | --- |
| Membase | Local Memory (UB) | Writes each computation result back to UB | NPU architecture version 2201 |
| Regbase | Register (VF Reg) | Intermediate results can be staged in registers, reducing UB reads and writes | NPU architecture version 3510 |

>[!NOTE]Description
>- For detailed architectural changes, see [Architectural Changes from 2201 to 3510](../../../cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md).
```

**L4**: SetGlobalBuffer is used for the first time in the following Operator Practice Reference document and links to the documentation for that interface.

```text
The allocation scheme in this example is as follows: The overall data length TOTAL\_LENGTH is 1 \* 2048. The [SetGlobalBuffer](../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetGlobalBuffer.md) interface of the GlobalTensor class is used to set the start address and length of Global Memory on this core.
```

**L5**: The following Operator Practice Reference document mentions the vector programming paradigm for the first time and links to the authoritative explanation of this programming concept.

```text
The process for implementing a basic vector operator kernel function using Ascend C is as follows.
-   Operator analysis: Analyze the operator's mathematical expression, inputs, outputs, and implementation of the computation logic, and identify the Ascend C interfaces that must be called.
-   Kernel function definition: Define the Ascend C operator entry function.
-   Implement the operator class according to the [vector programming paradigm](../../../programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md#section116515238815): Complete the internal implementation of the kernel function, including three basic tasks: CopyIn, Compute, and CopyOut.
```

**L6**: In the following Operator Practice Reference document, performance optimization solutions differ by architectural version, so a link to the Cross-Generation Migration Guide is required to present the architectural differences.

```text
It can be seen that bank conflict scenarios are closely related to the specifications of the Unified Buffer. Specification changes usually result in changes to bank conflict scenarios. For specific differences between architectural versions, see [Architectural Changes from 2201 to 3510](../../../../cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md).

-   Because the bank group in NPU architecture version 3510 has two groups of read ports and write ports, two read operations accessing different banks in the same bank group do not cause a conflict.
-   Assume that the address used by the read instruction is 0x0000 (bank0) and the address used by the write instruction is 0x10000. In NPU architecture version 2201, address 0x10000 (bank16) does not cause a read-write conflict, whereas in NPU architecture version 3510, address 0x10000 (bank0) causes a read-write conflict.
```

**L7**: The following API Reference document introduces the concept of C programming for the first time and links to the corresponding programming concept introduction section.

```text
The C API is the **language extension layer SIMD API** in the three-level graduated Ascend C programming interface. It is positioned as the lowest-level C interface, uses pointer-based programming, and provides complete C programming capabilities. The C API can map directly to NPU hardware instructions. Developers independently manage memory transfers and synchronization (unlike TPipe/TQue, which automatically manages memory and synchronization), with access to all low-level hardware capabilities. It accommodates C development practices and is suitable for operator development scenarios with high performance and controllability requirements. It is the core path for pursuing ultimate performance and fully unleashing the potential of NPU hardware.

For details, see [C Programming Overview](../../../guide/programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md).
```

**L8**: In the following API Reference document, Exp has a corresponding function prototype on the 3510 architecture. This difference is annotated in product support, with a link to the corresponding section in the Cross-Generation Migration Guide.

```text
# Exp

## Product Support

### Prototype without config

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training series products/Atlas A3 inference series products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training series products/Atlas A2 inference series products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference series products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference series products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training series products: Supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030: Supported
<!-- end id9 -->

### Prototype with config

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training series products/Atlas A3 inference series products: Not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training series products/Atlas A2 inference series products: Not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference series products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference series products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training series products: Not supported
<!-- end id16 -->
<!-- npu="x90" id17 -->
- Kirin X90: Not supported
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030: Not supported
<!-- end id18 -->

>[!NOTE]Description
>- [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) does not support the Subnormal feature by default. Therefore, this interface provides a function prototype that accepts config. For details, see [Basic API Migration](../../../../..//guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_guide/basic_api_migration.md).
```

### DOC-DISC-02 Unified Terminology Entry

[Level] Specification

[Description] Establish a **unified terminology mapping table** (independent appendix file) shared by all documents:

| Mapping Content | Format | Location |
|----------------|--------|----------|
| Terminology mapping table | Clearly define each term individually. If similar or related concepts are involved, also explain their intrinsic relationships and key differences to help readers accurately understand and distinguish them | Standalone appendix file, linked from the overview chapter |

**Specific requirements**:

- When a code name/abbreviation appears in any document, annotate its full name in parentheses and link to the terminology mapping table at its first occurrence.
- If terms in the glossary involve similar or related concepts, explain their intrinsic relationships and key differences together to help readers accurately understand and distinguish them.
- Subsequent references to terms must remain consistent, avoiding multiple expressions for the same concept.

[Positive example]

**Terminology mapping table**:

| Term/abbreviation | Meaning |
|---|---|
| Global Memory/GM | The main memory on the device side and external storage of the AI Core. It is used to store large-scale data, but access patterns must be optimized to improve performance. |
| Local Memory | Internal storage of the AI Core, including storage units such as L1 Buffer, L0A Buffer, L0B Buffer, L0C Buffer, and UB. |
| DMA | Direct Memory Access unit.<br>Responsible for data transfers, including transfers between Global Memory and Local Memory and between different levels of Local Memory. It includes transfer units such as MTE2 and MTE3.|

**Terminology reference**:

```text
The [**DMA (Direct Memory Access) transfer unit**](../../../technical_appendix/concepts_and_terms/glossary.md) is responsible for data transfers, including transfers into and out of Global Memory and Local Memory, as well as data movement between different levels of Local Memory.
```

### DOC-DISC-03 File Responsibility Focus

[Level] Principle

[Description] Each file has a clear single responsibility:

| File Type | Responsibility | Prohibited |
|-----------|---------------|------------|
| Overview/Summary File | Navigation: topic list + 1-2 sentence summary + sub-chapter links | Expanding technical details, extensive code |
| Concept Introduction File | Explanation: concept definition + principles + constraints + relationships | Repeating content already in other files (use references instead) |
| Operation Guide File | Steps: code snippets + operation steps + caveats | Concept definitions (link to concept files) |
| API Reference Page | Reference: prototype + parameters + constraints + examples + related APIs | Programming model explanations (link to Programming Guide) |
| Migration Guide File | Steps: change list + verification steps | Programming model explanations (link to Programming Guide), repeating existing migration content (use references) |

**Specific Requirements**:

- If a file covers more than 3 unrelated topics, split into multiple sub-files
- Detailed description of the same content appears only once (authoritative version); other locations use "See Chapter X for details"
- Hardware architecture description is unified into a single file; separate SIMD/SIMT versions describing different halves are not allowed

[Positive example] The following is an overview file for vector programming. It serves only as a navigation page: it lists topics, provides brief descriptions, and links to subchapters without expanding implementation details:

```text
# Overview

This section uses the Add operator as an example to help you quickly build an Ascend C vector operator program and learn typical scenarios and approaches for vector operator development. The scenarios include:

-   [Basic Vector Operator](basic_vector_operator.md): Develop a simple Add vector operator.
-   [Using TBuf](tbuf_usage.md): Use temporary storage to store intermediate results during operator computation.
-   [Multi-core Tiling](multi_core_tiling/overview.md): When an operator requires multi-core parallel computation or supports dynamic shapes, split and block the input data and assign different data blocks to multiple AI Cores.
    -   [Even Split of Main Blocks](multi_core_tiling/main_block_even_split.md): The operator runs on multiple cores of the AI processor, with an equal amount of computation on each core and 32-byte alignment.
    -   [Even Split of Tail Blocks](multi_core_tiling/tail_block_even_split.md): The operator runs on multiple cores of the AI processor with an equal amount of computation on each core. Except for the last data block (tail block) on each core, the remaining data blocks have equal sizes, and each core processes its tail block.
    -   [Tail-core Split](multi_core_tiling/tail_core_split.md): The data cannot be evenly distributed among the cores. The cores are divided into full cores and tail cores; the computation amounts are equal among full cores and among tail cores.
    -   [Tail-core and Tail-block Split](multi_core_tiling/tail_core_tail_block_split.md): The data cannot be evenly distributed among the cores, and the data within each core cannot be evenly divided. Except for the last data block (tail block), the remaining data blocks have equal sizes, and each core processes its tail block separately.
-   [Double-buffering Scenario](double_buffer_scenario.md): Enable double buffering so that multiple pipelines in the operator execute in parallel.
-   [Broadcast Scenario](broadcast_scenario.md): The shapes of the two operator inputs are different. Broadcast one input to match the other before performing the computation.
-   [Unaligned Scenario](unaligned_scenario.md): More solutions for handling data that is not 32-byte aligned.
```

[Negative example] The following is an overview file for vector programming that expands on technical details and contains extensive code, exceeding the responsibilities of an overview file:

````text
# Overview

## Basic Vector Operator
The process of implementing a basic vector operator kernel using Ascend C is shown in the following figure...... (details of the basic vector operator are expanded here)

## Using TBuf
During kernel computation for most operators, temporary memory is required to store intermediate results. These results are represented by temporary variables, and the memory occupied by them can be managed using the TBuf data structure...... (details of TBuf are expanded here)

```cpp
// Compute stage
xLocal = inQueueX.DeQue<bfloat16_t>();
yLocal = inQueueY.DeQue<bfloat16_t>();
AscendC::LocalTensor<bfloat16_t> zLocal = outQueueZ.AllocTensor<bfloat16_t>();
AscendC::LocalTensor<float> tmpTensor0 = tmpBuf0.Get<float>();
AscendC::LocalTensor<float> tmpTensor1 = tmpBuf1.Get<float>();
// Use the Cast interface to convert bfloat16_t to float and store the result in the temporary TBuf
AscendC::Cast(tmpTensor0, xLocal, AscendC::RoundMode::CAST_NONE, totalLength);
AscendC::Cast(tmpTensor1, yLocal, AscendC::RoundMode::CAST_NONE, totalLength);
AscendC::Add(tmpTensor0, tmpTensor0, tmpTensor1, totalLength);
AscendC::Cast(zLocal, tmpTensor0, AscendC::RoundMode::CAST_RINT, totalLength);
outQueueZ.EnQue<bfloat16_t>(zLocal);
inQueueX.FreeTensor(xLocal);
inQueueY.FreeTensor(yLocal);
```
````

## 3 Readability

### DOC-READ-00 Explicitly Distinguish Easily Confused Concepts

[Level] Principle

[Description] Similar-named or hierarchically confusable concepts must be **explicitly distinguished** using comparison tables or relationship diagrams:

**Required concept groups for distinction**:

| Concept Group | Distinction Dimension | Distinction Format |
|--------------|----------------------|-------------------|
| SPMD vs SIMD vs SIMT | SPMD=programming model, SIMD=instruction execution mode, SIMT=thread execution mode | Hierarchy diagram |
| Four-step (Tiling→Transfer→Compute→Transfer) vs TPipe four steps (Alloc→EnQue→...) | Programming flow vs pipeline management paradigm | Comparison table |
| DMA vs MTE vs DataCopy | Three-layer name for the same operation | Terminology mapping table |
| MemBase (Basic API) vs RegBase (VF fusion API) | Different compute locations (UB vs register), different Load/Store counts | Comparison table + scenario recommendation |
| Block vs CTA | Ascend C programming unit vs CUDA equivalent concept | Competitive mapping table |
| `LocalTensor` vs `GlobalTensor` vs `TBuf` | Compute buffer / external buffer / temporary buffer | Comparison table + scenario recommendation |
| `__ubuf__` vs `__cbuf__` vs `__gm__` | UB space / L1 space / GM space address qualifiers | Comparison table |
| `asc_` prefix vs `Ascend C::` prefix vs `cce::` prefix | C API / C++ API / legacy API naming conventions | Comparison table |

**Specific requirements**:

- Provide the distinction at the **first location where confusion may arise**, rather than postponing it.
- Do not use a code example for one concept to imply that it is equivalent to another concept.

[Positive example] Use the following comparison table to distinguish address space qualifiers such as `__ubuf__`, `__cbuf__`, and `__gm__`:

| Address space qualifier | AI Core physical storage space |
|----------------|---------------------|
| \_\_gm\_\_ | Device-side memory GM |
| \_\_ubuf\_\_ | Vector Unified Buffer |
| \_\_ca\_\_ | Cube L0A Buffer |
| \_\_cb\_\_ | Cube L0B Buffer |
| \_\_cc\_\_ | Cube L0C Buffer |
| \_\_cbuf\_\_ | Cube L1 Buffer |
| \_\_fbuf\_\_ | Fixpipe Buffer |
| \_\_ssbuf\_\_ | SSBuffer |

### DOC-READ-01 Present Constraints Prominently and Centrally

[Level] Specification

[Description] All hardware-level/platform-level constraints must be prominently annotated where the concept is **first defined**, rather than relying on later sections or runtime errors to reveal them:

**Constraint information elements**:

- Constraint type: address alignment/data type restriction/element count range/format restriction/read-only semantics/timing requirement
- Constraint value: specific value or range (such as "32-byte alignment" or "≥128 elements for the half type")
- Consequence of violation: compilation error/runtime out-of-bounds access/incorrect result/performance degradation

**Formatting requirement**: List constraints centrally in an independent paragraph rather than dispersing them in small print across parameter descriptions.

**Placement**:

- API reference page: Independent "Constraints and Limitations" section
- Programming Guide: Provide constraints together with the concept introduction rather than adding them later

[Positive example]

**The DataCopy (continuous data transfer between GM and UB) API reference page contains an independent constraints section**:

```text
## Constraints

- Addresses in Global Memory must be aligned to the number of bytes occupied by the corresponding data type, and addresses in the Unified Buffer must be 32-byte aligned.
- When calling a continuous transfer interface, count \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the transfer amount is rounded down to a 32-byte-aligned value.
- If multiple DataCopy instructions must be executed and their destination addresses overlap, insert a synchronization instruction by calling [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to serialize the DataCopy instructions and prevent abnormal data. In the diagram on the left below, two DataCopy instructions are executed and their destination Global Memory addresses overlap. `PipeBarrier<PIPE_MTE3>()` must be called between the two transfer instructions to add synchronization for the MTE3 transfer-out pipeline. As shown in the diagram on the right below, the destination Unified Buffer addresses overlap, so `PipeBarrier<PIPE_MTE2>()` must be called between the two transfer instructions to add synchronization for the MTE2 transfer-in pipeline.

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id19 -->
- For the following product models:
    - Atlas A2 training series products/Atlas A2 inference series products
    - Atlas A3 training series products/Atlas A3 inference series products

    In cross-device communication operator development scenarios, DataCopy interfaces support cross-device data transfers over HCCS physical links only; other paths are not supported. During development, developers must pay attention to the physical paths involved in inter-device communication. Run the `npu-smi info -t topo` command to query HCCS physical links.
<!-- end id19 -->
```

**The constraints are provided together with the introduction of the repeat_time concept in the Memory data computation section of the Programming Guide**:

```text
#### Iteration Control

In a single iteration, the vector compute unit reads eight consecutive DataBlocks (32 bytes each) from UB and writes the computation results to the eight corresponding DataBlocks in the destination UB.

If repeat_time (the number of iterations) is set to 2, the unit performs two iterations, processing a total of 2 × 8 × 32 bytes = 512 bytes. If the data type is half (2 bytes/element), this corresponds to 256 elements.

> 📌 Hardware constraint: The value range of repeat_time is 1–255. This constraint applies to all products with [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) and [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).
```

### DOC-READ-02 Visualize Parameter Semantics

[Level] Specification

[Description] Use diagrams instead of prose alone for complex parameters (involving memory layouts, dimension mappings, and stride calculations):

| Parameter type | Diagram form |
|---------|---------|
| stride/offset types | Memory layout comparison diagram (labeling each offset) |
| Dimension mapping types | Dimension transformation mapping diagram (such as [K,N] → [N,K] transpose) |
| Format types (ND/NZ) | Memory layout comparison diagram for the two formats |
| mask types | Element-to-bit-mask mapping diagram |
| Data flow types | Input → processing → output flow diagram |

**Decision criterion**: If explaining a parameter clearly in prose requires more than three sentences, a diagram must be provided. The diagram and textual description must be consistent.

[Positive example] Use a diagram to help explain scenarios where dataBlockStride is set to 1 and where it is set to a value greater than 1:

```text
#### Address Interval Configuration

The vector compute unit also supports vector computation with address intervals, which can be precisely configured using the dataBlockStride and repeatStride parameters:
- dataBlockStride: Address stride between different DataBlocks within a single iteration. Its value must not exceed the UB space size limit.
- repeatStride: Address stride of the same DataBlock between adjacent iterations (described in detail below).

For continuous computation, set `dataBlockStride` to 1 to process the eight DataBlocks within the same iteration continuously.
For non-continuous computation, set `dataBlockStride` to a value greater than 1 (for example, 2). When data is read, there is an interval of one DataBlock between different DataBlocks within the same iteration, as shown below.

![dataBlockStride example](../../../../figures/db_stride.png)
```

[Negative example] A complex parameter is explained only in prose, using more than three sentences.

### DOC-READ-03 Keep Code Snippets Concise and Focused

[Level] Specification

[Description] Provide a concise code snippet and a link to a complete example for each programming paradigm/key process:

| Requirement | Description |
|------|------|
| Concise and focused | Show only code related to the current explanation and use `// ... Other initialization` to omit irrelevant parts |
| Understandable | Accompany each snippet with two or three lines explaining "what this code does" |
| Traceable | Link to the complete example in the example repository at the end of the snippet |
| Keyword comments | Ascend C-specific keywords (such as `__aicore__`, `__ubuf__`, and `__simd_vf__`) must have inline comments |
| Parameter coverage | Cover commonly used parameter combinations instead of showing only one usage |

[Positive example] The following Memory vector computation synchronization control section provides a concise code snippet and a complete example for the vector addition computation process:

````text
Execution units inside the AI Core (such as the MTE2 transfer unit and Vector compute unit) run asynchronously and in parallel. When different units read from and write to the same storage resource, data dependency issues can easily occur. Therefore, Memory vector computation must coordinate the execution order through pipeline synchronization interfaces to ensure that the computation process is correct. The Memory vector computation process is simpler than Cube matrix computation and mainly consists of three steps: data transfer in (Global Memory → UB), computation (UB), and data transfer out (UB → Global Memory). The three steps correspond to the PIPE_MTE2, PIPE_V, and PIPE_MTE3 pipelines, respectively. Intra-core synchronization interfaces must be used to coordinate the execution order and ensure that each step is completed sequentially.

```cpp
// Kernel implementation, decorated with __global__ to mark kernel entry point
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, 48>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, 48>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, 48>();

    AscendC::GlobalTensor<float> xGlobal, yGlobal, zGlobal;
    xGlobal.SetGlobalBuffer((__gm__ float*)x);
    yGlobal.SetGlobalBuffer((__gm__ float*)y);
    zGlobal.SetGlobalBuffer((__gm__ float*)z);

    // ...
    // 1. Step 1: Data transfer in, execution pipeline is PIPE_MTE2
    AscendC::Mutex::Lock<PIPE_MTE2>(mutex_id);
    AscendC::DataCopy(xLocal, xGlobal, 48);
    AscendC::DataCopy(yLocal, yGlobal, 48);
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutex_id);

    // 2. Step 2: Compute, execution pipeline is PIPE_V
    AscendC::Mutex::Lock<PIPE_V>(mutex_id);
    AscendC::Add(zLocal, xLocal, yLocal, 48);
    AscendC::Mutex::Unlock<PIPE_V>(mutex_id);

    // 3. Step 3: Data transfer out, execution pipeline is PIPE_MTE3
    AscendC::Mutex::Lock<PIPE_MTE3>(mutex_id);
    AscendC::DataCopy(zGlobal, zLocal, 48);
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutex_id);
}
```

By combining the Memory vector data transfer and computation capabilities described above, developers can implement complete Memory vector computation operators using the C++ Tensor programming interface. For the specific development process and code example, see the [cpp_api_add example](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md), which fully demonstrates the development, compilation, and verification process for a vector addition operator.
````

[Negative example] The code snippet has no textual explanation, and no link to a complete example in the example repository is provided at the end:

````text
```cpp
AscendC::Mutex::Lock<PIPE_MTE2>(mutex_id);
AscendC::DataCopy(xLocal, xGlobal, 48);
AscendC::DataCopy(yLocal, yGlobal, 48);
AscendC::Mutex::Unlock<PIPE_MTE2>(mutex_id);
```
````

### DOC-READ-04 Explicitly Establish Prerequisites

[Level] Principle

[Description] Provide prerequisite knowledge when each concept/API first appears to ensure that developers **do not need to read out of order**:

- If a section involves prerequisite knowledge, place the relevant conceptual explanation at the beginning of that section.
- Arrange Programming Guide sections according to the learning path: basic concepts → programming model → programming paradigm → performance optimization (rather than laying them out by functional module).
- If concept B depends on concept A, present A first or provide an explicit reference link from B to A.

[Positive example] The following operator functional design section involves prerequisite knowledge of aclnn engineering-oriented operator development and provides the relevant explanation at the beginning:

```text
# Operator Functional Design

## Prerequisite Understanding: What Components Constitute aclnn Engineering-Oriented Operator Development?

A custom aclnn operator project usually requires three types of code.

The first type of code is the **operator prototype definition**, which declares operator interface information, including inputs, outputs, attributes, and supported dtype and format values.

The second type of code is the **Host-side Tiling implementation**, which prepares runtime parameters before Kernel execution. Before the Kernel runs on the AI Core, it must know the total amount of input data, how to partition the computation across cores and how many Blocks to launch, how to continue partitioning within each Block, whether additional workspace is required, and whether launch configurations such as the scheduling mode must be set.

The third type of code is the **Kernel-side operator implementation**, which performs the actual computation on the AI Core. Based on the parameters passed by Tiling, the Kernel side transfers data from GM to UB, performs computation on UB, and writes the result back to GM.

The functional design described in this document determines what each type of code must express before these three types of code are written.
```

[Negative example] The operator functional design section does not explain prerequisite concepts:

```text
# Operator Functional Design

## Perform Functional Design Around the Operator Prototype

## Perform Functional Design Around Host-side Tiling

## Perform Functional Design Around the Kernel-side Implementation

## Extend the Kernel Implementation for Different Scenarios
```

---

## 4 Completeness

### DOC-COMP-00 Zero Omission of Constraint Information

[Level] Principle

[Description] Explicitly record all implicit constraints in the documentation rather than relying on runtime assertions or compilation errors to reveal them:

| Constraint category | Coverage requirement | Common omission |
|---------|---------|---------|
| Data type restrictions | List the complete set of supported data types for each API | Minimum element count for the half type or missing bf16 support |
| Address alignment | Specify the alignment size in bytes and the alignment direction | 32B alignment is documented only in an assertion |
| Element count range | Specify the minimum value, maximum value, and alignment granularity | The "eight-element" constraint is undocumented |
| Format restrictions | Support for formats such as ND/NZ/FRACTAL | Differences in offset calculations when switching formats |
| Usage mode restrictions | Differences between direct-call/engineering/debug modes | PipeBarrier is available only in specific modes |
| Multi-core/multi-instance restrictions | Read-only semantics within the kernel and inter-core synchronization requirements | Read-only semantics of configuration parameters are not explained |
| API combination restrictions | Mutually exclusive APIs/required companion APIs | A combination recommended by the documentation is not actually supported |

**Operational requirement**: Extract constraint information from specifications and assertion code and present it centrally where the relevant item is first defined.

[Positive example] The following DataCopy (continuous data transfer between GM and UB) API documentation lists all supported data types and complete constraint information:

```text
## Data Types

The source and destination operands must use the same data type. The Global Memory -> Unified Buffer and Unified Buffer -> Global Memory data paths support the same data types on the same product, as follows:

<!-- npu="950" id20 -->
- Ascend 950PR/Ascend 950DT supports the following data types: b8, b16, b32, and b64.
<!-- end id20 -->

<!-- npu="A3" id21 -->
- Atlas A3 training series products/Atlas A3 inference series products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id21 -->

<!-- npu="910b" id22 -->
- Atlas A2 training series products/Atlas A2 inference series products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id22 -->

<!-- npu="310b" id23 -->
- Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id23 -->

<!-- npu="310p" id24 -->
- Atlas inference series products AI Core supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id24 -->

<!-- npu="310p" id25 -->
- Atlas inference series products Vector Core supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id25 -->

<!-- npu="910" id26 -->
- Atlas training series products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id26 -->

<!-- npu="x90" id27 -->
- Kirin X90 supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id27 -->

<!-- npu="9030" id28 -->
- Kirin 9030 supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id28 -->

## Return Value

None

## Constraints

- Addresses in Global Memory must be aligned to the number of bytes occupied by the corresponding data type, and addresses in the Unified Buffer must be 32-byte aligned.
- When calling a continuous transfer interface, count \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the transfer amount is rounded down to a 32-byte-aligned value.
- If multiple DataCopy instructions must be executed and their destination addresses overlap, insert a synchronization instruction by calling [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to serialize the DataCopy instructions and prevent abnormal data. In the diagram on the left below, two DataCopy instructions are executed and their destination Global Memory addresses overlap. `PipeBarrier<PIPE_MTE3>()` must be called between the two transfer instructions to add synchronization for the MTE3 transfer-out pipeline. As shown in the diagram on the right below, the destination Unified Buffer addresses overlap, so `PipeBarrier<PIPE_MTE2>()` must be called between the two transfer instructions to add synchronization for the MTE2 transfer-in pipeline.

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id29 -->
- For the following product models:
    - Atlas A2 training series products/Atlas A2 inference series products
    - Atlas A3 training series products/Atlas A3 inference series products

    In cross-device communication operator development scenarios, DataCopy interfaces support cross-device data transfers over HCCS physical links only; other paths are not supported. During development, developers must pay attention to the physical paths involved in inter-device communication. Run the `npu-smi info -t topo` command to query HCCS physical links.
<!-- end id29 -->
```

[Negative example] The following API documentation lists only some data types and does not distinguish support across different products. The constraint description does not specify address alignment requirements for different storage locations, and omits alignment requirements for the transfer size and synchronization requirements when the source and destination addresses overlap.

```text
## Data Types

The supported data types are: half and float.

## Constraint Description

- Both the source and destination addresses must be aligned to 32 bytes.
- The transfer size has no alignment requirement.
```

### DOC-COMP-01 Layered Example Coverage

[Level] Principle

[Description] Provide examples for each API/programming paradigm:

**Example level definitions**:

| Level | Positioning | Length | Requirement |
|------|------|------|------|
| **Minimal** | Minimal runnable example | <30 lines of core code | Kernel function code snippet and link to a complete example |
| **Standard** | Typical usage example | 50–150 lines | Code snippets containing the kernel function definition and invocation |

**Specific requirements for each document**:

- API Reference: At least one typical usage example (Standard) for each API, covering common parameter combinations in the example.
- Programming Guide: At least one Minimal example and a link to a complete example in the example repository for each programming paradigm.
- Operator Practice Reference: At least one Minimal example and a link to a complete example in the example repository for each operator practice case.

[Positive example]

**Minimal**:

````text
```c
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    // ...
    // 1. Step 1: Data transfer in, execution pipeline is PIPE_MTE2
    asc_lock(PIPE_MTE2, mutex_id);
    asc_copy_gm2ub_align(x_local, x_gm, 1, 48 * sizeof(half), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_copy_gm2ub_align(y_local, y_gm, 1, 48 * sizeof(half), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_unlock(PIPE_MTE2, mutex_id);

    // 2. Step 2: Compute, execution pipeline is PIPE_V
    asc_lock(PIPE_V, mutex_id);
    asc_add(z, x, y, 4096);
    asc_unlock(PIPE_V, mutex_id);

    // 3. Step 3: Data transfer out, execution pipeline is PIPE_MTE3
    asc_lock(PIPE_MTE3, mutex_id);
    asc_copy_ub2gm_align(z_gm, z_local, 1, 48 * sizeof(int8_t), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_unlock(PIPE_MTE3, mutex_id);
}
```
For a complete project example, see the [c_api_add example](../../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add).
````

**Standard**:

````text
```cpp
template <uint32_t blockLength>
__vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    // ...
}

int32_t main(int32_t argc, char* argv[])
{
    // ...
    add_custom<blockLength><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice);
    // ...

}
```
````

### DOC-COMP-02 Complete Cross-Generation Migration Coverage

[Level] Principle

[Description] The Cross-Generation Migration Compatibility Guide must cover the complete migration path, ensuring that developers are not blocked by missing documentation during architectural upgrades:

**Migration elements that must be covered**:

| Migration element | Coverage requirement |
|---------|---------|
| Architectural change list | List all differences between the two architectural versions that affect programming |
| API compatibility policy | Explain the impact on the compatibility of each API category and provide compatibility adaptation solutions |
| Post-migration verification steps | Explain how to verify functional correctness and the performance baseline after migration |
| Compilation option changes | Differences in compilation parameters between architectural versions |

**Operational requirements**:

- Changes to programming concepts involved in architectural change descriptions (such as new execution modes and memory model differences) must link back to the corresponding concept sections in the Programming Guide.
- Optimization techniques in migration practices must link to the corresponding optimization sections in the Operator Practice Reference.

[Positive example]

**Architectural change list**:

```text
Specifically, the main changes in the 3510 architecture are shown in the following tables.

- Transfer units

    **Table 1**  Transfer unit changes
    | 3510 change | Impact | Affected APIs |
    |----------|------------|---------------|
    | The data path from L1 Buffer to GM is removed. | Existing interfaces do not support directly transferring data from L1 Buffer to GM. Developers must allocate space in L1 Buffer to store an identity matrix, use MMAD matrix multiplication to compute the output into L0C Buffer, and transfer the data from L0C Buffer to GM through [Fixpipe (L0C-to-GM data transfer)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md). | DataCopy/DumpTensor |
    | The data paths from GM to L0A Buffer and L0B Buffer are removed. | The original data transfers from GM to L0A Buffer and L0B Buffer must be split into two steps: data transfer from GM to L1 Buffer and data transfer from L1 Buffer to L0A Buffer and L0B Buffer. | LoadData |
    ...
```

**API compatibility policy**:

````text
-   **NPU architecture version 3510 removes the hardware instructions related to L0A Buffer/L0B Buffer initialization.**

    **Description**: The Fill interface initializes a LocalTensor at a specific storage location to a specified value. It does not support directly initializing L0A Buffer or L0B Buffer.

    **Compatibility solution**: First use the Fill interface to initialize L1 Buffer, and then use the LoadData interface to transfer the data in L1 Buffer to L0A Buffer and L0B Buffer. For specific code, see the [Fill compatibility example](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fill).

    Using the GM-\>L1 Buffer-\>L0A Buffer data path as an example:

    1.  Initialize L1 Buffer.

        ```cpp
        __aicore__ inline void InitConstA1(AscendC::LocalTensor<T>& a1Local)
        {
            AscendC::Fill(a1Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 32), 0, 1});
        }
        ```

    2.  Call the LoadData interface to transfer the data in L1 Buffer to L0A Buffer.

        ```cpp
        __aicore__ inline void Load2DA1ToA2(AscendC::LocalTensor<T>& a1Local, AscendC::LocalTensor<T>& a2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2Local, a1Local, loadDataParams);
        }
        ```
````

**Compilation option changes**:

````text
- In heterogeneous compilation scenarios where developers compile using the command line or a CMake file, they must manually change the NPU architecture version number or AI processor model. Using a change to the NPU architecture version number as an example, modify the --npu-arch configuration in the compilation command line or the CMakeLists.txt file of the compilation project, as follows:

    ```
    ...

    target_compile_options(demo PRIVATE
        // Replace dav-xxxx with the corresponding NPU architecture version number
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-xxxx>
    )
    ```
````

### DOC-COMP-03 Version Constraint Annotation Specifications

[Level] Principle

[Description] Content involving version differences across the five documents must be annotated, ensuring that developers can identify the applicable scope at a glance:

| Annotation scenario | Example |
|---------|------|
| Constraints differ by version | Matrix fractal formats are affected by hardware reading logic: for NPU architecture version 2201, the left matrix A uses the Zz format; for NPU architecture version 3510, the left matrix A uses the Nz format |
| Performance optimization solutions differ by version | The bank conflict avoidance solutions for NPU architecture versions 2201 and 3510 are different |

**Operational requirements**:

- In the Programming Guide, annotate specific differences in hardware parameters and constraints that vary by version.
- In the Operator Practice Reference, annotate the applicable architecture versions for version-specific optimization solutions.

[Positive example]

**Constraints differ by version**:

```text
### Detailed Description of Key Fractal Formats

Matrix fractal formats define the layout rules for multidimensional tensors in memory. Due to hardware reading logic, different product models have different format requirements for matrix multiplication C = A × B:

- For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), matrix multiplication C = A × B requires the left matrix A to use the Zz format, the right matrix B to use the Zn format, and the result matrix C to use the Nz format.

- For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), matrix multiplication C = A × B requires the left matrix A to use the Nz format, the right matrix B to use the Zn format, and the result matrix C to use the Nz format.
```

**Performance optimization solutions differ by version**:

```text
# Avoiding Bank Conflicts (NPU Architecture Version 2201)

>[!NOTE] Note
>This performance optimization recommendation applies to the following product models:
><!-- npu="A3" id30 -->
>- Atlas A3 Training Series products/Atlas A3 Inference Series products
><!-- end id30 -->
><!-- npu="910b" id31 -->
>- Atlas A2 Training Series products/Atlas A2 Inference Series products
><!-- end id31 -->
```

---

## 5 Cross-Dimensional Design

Some design requirements span multiple dimensions and must satisfy them simultaneously:

| Design requirement | Discoverability | Readability | Completeness | Specific operation |
|---------|---------|--------|--------|---------|
| **Terminology mapping table** | Unified entry point | Terminology appropriate to each level | — | Create a standalone appendix file |
| **Decision tree/comparison table** | Three-level navigation | Distinguish easily confused concepts | — | Provide one at every branch requiring a choice among multiple options |
| **Code examples** | Link to the example repository | Concise and focused | Layered coverage | Provide at least one Standard example on each API page; provide a Minimal example and complete example link in the guides and practice documents |
| **Constraint box** | — | Prominent and introduced in advance | Zero omission | Provide constraint information where the relevant item is first defined |
| **Parameter diagrams** | — | Visualization | Constraint visualization | Complex parameters such as stride/layout/mask types must include diagrams |
| **Version constraint annotations** | Identify the version scope at a glance | Do not confuse version differences | Zero omission of migration paths | Content involving version differences in any of the five documents must be annotated |
| **Cross-generation migration mapping** | — | Migration steps are understandable | Zero omission from the change list | List architectural changes, API compatibility policies, verification steps, and compilation options |
