# Ascend C Documentation Contribution Guide

## Overview

The Ascend C documentation system consists of five core documents. Developers can improve and contribute to the documentation by submitting PRs:

| Document | Content | Directory |
|----------|---------|-----------|
| Getting Started Tutorial | Ascend C overview, environment setup, quick-start hands-on (HelloWorld, first operator) | `docs/zh/guide/getting_started` |
| Programming Guide | Programming models, programming paradigms, compilation & execution, hardware architecture, advanced programming | `docs/zh/guide/编程指南` |
| API Reference Manual | Interface parameters, constraints, examples, API associations | `docs/zh/api/` |
| Operator Practice Reference | Operator implementation, performance optimization, tuning cases | `docs/zh/guide/算子实践参考/` |
| Cross-generation Migration & Compatibility Guide | API compatibility strategies, architecture change mappings, migration steps | `docs/zh/guide/cross_gen_migration_guide/` |

Before reading this document, ensure you have a basic understanding of the Ascend AI processor hardware architecture. New developers are recommended to start with the Getting Started Tutorial; developers with prior experience should read the Ascend C Programming Guide.

---

## Contribution Scenarios

### Documentation Error Correction

If you find description errors, inaccurate parameter values, or missing constraints in the documentation:

1. Create a `Documentation | 文档反馈` type Issue following the community guidelines, pointing out the corresponding document issue
2. Enter `/assign` or `/assign @yourself` in the comment box to assign the Issue to yourself
3. Fix and submit a PR

### Documentation Supplementation

If you find that certain content is missing or incomplete in the documentation (such as missing examples, missing constraint explanations, missing related API descriptions):

1. Create a `Requirement | 需求建议` type Issue describing the content that needs to be supplemented
2. Complete the supplementation following the "Writing Conventions" in this guide
3. Submit a PR

### Performance Optimization Case Contribution

If you have practical experience with Ascend C operator performance optimization to share:

1. Refer to existing tuning case structures under `算子实践参考/优秀实践/`
2. Write documentation following the "Performance Optimization Case Writing Convention" in this guide
3. Submit a PR

---

## Writing Conventions

Ascend C documentation follows the three-dimension requirements defined in the "Ascend C Documentation Design Principles": discoverability, readability, and completeness. Below are specific conventions for common writing scenarios.

### General Conventions

| Rule | Requirement | Wrong Example | Correct Example |
|------|-------------|---------------|-----------------|
| Terminology Layering | High-level chapters use abstract terms; low-level chapters associate hardware terms on first occurrence | Overview document directly writes "data is moved from UB into VEC" | Overview document writes "data is moved from external storage into on-chip storage", low-level document writes "data is moved from Global Memory into UB (on-chip storage)" |
| Easily Confused Concept Distinction | Similar-named concepts must be distinguished with comparison tables, not left for readers to infer | Only mentions "four-step method" without distinguishing Tiling flow and TPipe pipeline | Use a table to distinguish: four-step = programming flow, TPipe four steps = pipeline management paradigm |
| Constraint Visibility | Hardware constraints should be annotated when concepts first appear, not deferred to later chapters or only exposed through compiler errors | 32B alignment constraint only appears in assert | Add a separate "Constraints and Limitations" paragraph before parameter descriptions |
| First-occurrence Linking | When content managed by another document is mentioned, add a link on first occurrence only, not repeated | Every mention of DataCopy gets a link | Only the first mention of DataCopy links to the API Reference Manual |
| No Reverse-order Reading | When concept B depends on concept A, A appears first or B has a clear reference link | API page directly uses TPipe concept without prior introduction | API page starts with a "Prerequisites" section linking to the corresponding Programming Guide chapter |

### Getting Started Tutorial Writing Conventions

The Getting Started Tutorial is the **zero-to-one entry point** for developers new to Ascend C, helping them quickly build a comprehensive overview and complete their first operator.

**File Responsibilities**:

| File Type | Responsibility | Prohibited |
|-----------|---------------|------------|
| Overview & Learning Path | Panoramic introduction: What is Ascend C, recommended learning path | Deep technical details (link to Programming Guide) |
| Environment Setup | Hands-on steps: installation, configuration, environment verification | Duplicating compilation & execution details from the Programming Guide |
| Quick Start / Heterogeneous System & Programming Model | Entry-level concepts: Host/Device, AI Core, SIMD/SIMT selection | In-depth programming model principles (link to Programming Guide) |
| Quick Start / SIMD Programming | Hands-on: HelloWorld + first operator (Add) | Duplicating the complete programming paradigm from the Programming Guide |
| Quick Start / SIMT Programming | Hands-on: HelloWorld + first operator (Gather) | Duplicating the complete programming paradigm from the Programming Guide |

**Directory Structure Mapping** (corresponds to Programming Guide chapters, excluding Technical Appendix):

| Tutorial Directory | Corresponding Programming Guide Chapter | Tutorial Purpose |
|--------------------|---------------------------------------|-----------------|
| `getting_started/` | Programming Model / Overview | Overview & learning path |
| `environment_setup.md` | Compilation & Execution | Quick environment setup; detailed compilation instructions link to Programming Guide |
| `quick_start/heterogeneous_system_and_programming_model.md` | Programming Model / Heterogeneous System + Programming Model / Overview | Entry-level concepts + SIMD/SIMT selection; in-depth links to Programming Guide |
| `quick_start/simd_programming/` | Programming Model / AI-Core-SIMD | HelloWorld + Add operator quick start |
| `quick_start/simt_programming/` | Programming Model / AI-Core-SIMT | HelloWorld + Gather operator quick start |

**Link Direction**:
- First mention of in-depth programming concepts → link to corresponding Programming Guide chapter
- First mention of an API name → link to API Reference Manual
- No need to link to Operator Practice Reference or Cross-generation Migration Guide (not relevant at the beginner stage)

### Programming Guide Writing Conventions

The Programming Guide is the **concept authority source**. Other documents link back to the Programming Guide when encountering concept issues.

**File Responsibilities**:

| File Type | Responsibility | Prohibited |
|-----------|---------------|------------|
| Overview/Summary File | Navigation page: list sub-topics + one-sentence summary + links | Expanding technical details |
| Concept Introduction File | Define concepts, explain principles, provide constraints | Repeating content from other files (use references instead) |
| Operation Guide File | Code snippets + operation steps + notes | Repeating concept definitions (link to concept files) |

**Programming Guide link directions to other documents**:
- First mention of an API name → link to API Reference Manual
- First mention of an optimization/practice topic → link to Operator Practice Reference
- Mention of architecture version differences → link to Cross-generation Migration Guide

### API Reference Page Writing Conventions

The API Reference Manual is the **interface detail authority source**. Each API page must include the following elements (in order):

#### Page Structure Overview (in order of appearance)

| # | Section Title | Required | Description |
|---|---------------|----------|-------------|
| 1 | Product Support Status | ✅ Required | At the top of the page, list the support status for each product series |
| 2 | Function Description | ✅ Required | Header file path + summary paragraph + mathematical formula/diagram (all 3 elements required) |
| 3 | Matrix/Tensor Computation Description Table | Conditionally Required | Required for matrix computation APIs; separate tables by product series |
| 4 | Function Prototypes | ✅ Required | All overloaded prototypes, each in a separate code block |
| 5 | Parameter Description | ✅ Required | Main parameter table + nested struct parameter table |
| 6 | Data Types | Conditionally Required | Required for matrix/vector computation APIs; not required for utility/configuration APIs |
| 7 | Return Value Description | Conditionally Required | Required for APIs with return values |
| 8 | Constraints | ✅ Required | Single-parameter constraints inline; multi-parameter/constraint combinations in separate paragraphs |
| 9 | Key Feature Description | Conditionally Required | Required for matrix/vector computation APIs involving key hardware features (HF32, GEMV, UnitFlag, etc.); simple APIs may omit |
| 10 | Theoretical Performance (Appendix Reference) | Conditionally Recommended | Reference the theoretical performance appendix instead of inline cycle formulas |
| 11 | Code Example | ✅ Required | Code snippet + link to sample repository (if a corresponding sample exists; if no sample is available, the link may be omitted) |
| 12 | Related APIs | Conditionally Required | Mutually exclusive APIs + functionally similar APIs within the same version |

---

#### 1. Product Support Status

**Format**: List the support status for each product series using a list format.

**Sample**:

```
## Product Support Status

- Ascend 950PR/Ascend 950DT：Supported
- Atlas A3 Training Products/Atlas A3 Inference Products：Supported
- Atlas A2 Training Products/Atlas A2 Inference Products：Supported
- Atlas 200I/500 A2 Inference Products：Not supported
```

**Requirements**:
- One line per product series, format: `- Product Name: Supported/Not supported`
- Partial support annotated as `Supported*` with limitation conditions explained below

---

#### 2. Function Description

**Format**: All 3 elements must be present — header file path + summary paragraph + mathematical formula/diagram. The header file path appears at the very beginning.

**Sample** (using Mmad as an example):

```
## Function Description

Header file: `#include "basic_api/kernel_operator_mm_intf.h"`

Mmad is AscendC's core matrix multiply-accumulate computation interface designed for Ascend AI chips,
purpose-built for high-performance operator development. It encapsulates the matrix multiply-accumulate
computation capability of the Ascend NPU hardware...

As shown below, the Mmad interface implements Ascend NPU matrix multiplication. The mathematical formula is:
C = A × B + Bias
[Diagram/formula image]
```

**Requirements**:
- **Header file** must specify the exact include path, placed at the beginning of Function Description (not as an independent section, but as part of Function Description)
- Summary paragraph explains the API's positioning (target scenario, hardware capability encapsulated)
- Mathematical formula must be provided (not text-only description)
- If there are diagrams/data flow diagrams, embed images and label figure numbers

---

#### 3. Matrix/Tensor Computation Description Table

**Scope**: Required for matrix computation APIs (Mmad, LoadData, Fixpipe, etc.). Non-matrix computation APIs skip this section.

**Format**: Separate tables by product series, listing physical location, dimensions, and format of tensors involved in computation.

**Sample**:

```
For Atlas A2/A3 series products:
| Matrix Computation | Physical Location | Dimensions | I/O Data Format | Data Type |
|--------------------|-------------------|------------|-----------------|-----------|
| A | L0A Buffer | M x K | Zz | Data Type |
| B | L0B Buffer | K x N | Zn | Data Type |
| C | L0C Buffer | M x N | Nz | Data Type |

For Ascend 950PR/Ascend 950DT products:
| Matrix Computation | Physical Location | Dimensions | I/O Data Format | Data Type |
|--------------------|-------------------|------------|-----------------|-----------|
| A | L0A Buffer | M x K | Nz | Data Type |
| B | L0B Buffer | K x N | Zn | Data Type |
| C | L0C Buffer | M x N | Nz | Data Type |
```

**Requirements**:
- When different products use different layout formats (e.g., Zz vs Nz), **separate tables are required**
- Physical location uses Buffer names (L0A/L0B/L0C/UB, etc.)
- Format uses fractal layout names (Zz/Zn/Nz/ND, etc.)
- If a bias matrix exists, annotate in the C matrix row: "Can support bias matrix Bias initialization, dimension 1 x N"

---

#### 4. Function Prototypes

**Format**: Group all overloads by function branch. Each prototype in a separate code block with 1-2 lines of description.

**Sample**:

```cpp
// No bias: basic matrix multiply-accumulate prototype
template <typename T, typename U, typename S>
__aicore__ inline void Mmad(const LocalTensor<T>& c, const LocalTensor<U>& a,
    const LocalTensor<S>& b, const MmadParams& mmadParams)
```

```cpp
// With bias: matrix multiply-accumulate with bias prototype
template <typename T, typename U, typename S, typename V>
__aicore__ inline void Mmad(const LocalTensor<T>& c, const LocalTensor<U>& a,
    const LocalTensor<S>& b, const LocalTensor<V>& bias, const MmadParams& mmadParams)
```

```cpp
// No bias (BitMode version): bit-field union parameter prototype
template <typename T, typename U, typename S>
__aicore__ inline void Mmad(const LocalTensor<T>& c, const LocalTensor<U>& a,
    const LocalTensor<S>& b, const MmadBitModeParams& mmadParams)
```

```cpp
// With bias (BitMode version): bit-field union + bias prototype
template <typename T, typename U, typename S, typename V>
__aicore__ inline void Mmad(const LocalTensor<T>& c, const LocalTensor<U>& a,
    const LocalTensor<S>& b, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)
```

**Requirements**:
- Each template parameter (T, U, S, V...) must be annotated with its correspondence in the parameter description
- `__aicore__` modifier must be preserved
- Parameters listed in input order: output first (c), then inputs (a, b, bias), then parameter struct
- If a BitMode version exists (bit-field union parameters), list alongside the regular version

---

#### 5. Parameter Description

**Format**: Two layers — main parameter table + nested struct parameter table.

##### Main Parameter Table

```
| Parameter | I/O | Description |
|-----------|-----|-------------|
| c | Output | Destination operand, result matrix c, type LocalTensor, physical storage L0C Buffer(TPosition:CO1). Starting address must be 1024-byte aligned. |
| a | Input | Source operand, left matrix a, type LocalTensor, physical storage L0A Buffer(TPosition:A2). Starting address must be 512-byte aligned. |
| b | Input | Source operand, right matrix b, type LocalTensor, physical storage L0B Buffer(TPosition:B2). Starting address must be 512-byte aligned. |
| bias | Input | Source operand, bias matrix, physical storage BT Buffer(TPosition:C2). Starting address must be 64-byte aligned. |
| mmadParams | Input | Matrix multiply parameters. See the table below for MmadParams details. |
```

**Requirements**:
- Description column **must include**: functional description + LocalTensor physical location (TPosition enum value) + **alignment bytes**
- Alignment requirement written in description column (not a separate row), format: "Starting address must be N-byte aligned"
- Complex parameters (struct/union) end description with "See the table below for details"

##### Nested Struct Parameter Table

```
### MmadParams Struct Parameter Description

| Parameter | Description |
|-----------|-------------|
| m | Left matrix Height, range: m∈[0, 4095]. Default: 0. |
| n | Right matrix Width, range: n∈[0, 4095]. Default: 0. |
| k | Left matrix Width / right matrix Height, range: k∈[0, 4095]. Default: 0. |
| cmatrixInitVal | Whether to enable C matrix default zero initialization. true=init to 0, false=no init (controlled by cmatrixSource). Default: true. |
| cmatrixSource | Whether C matrix initial value comes from BT Buffer. true=from BT, false=no init. Default: false. **Note**: Ineffective when bias input is present. |
| unitFlag | Controls fine-grained Mmad/Fixpipe parallelism. 0=disabled, 2=enabled no reset, 3=enabled with reset. See UnitFlag feature description. |
| disableGemv | Whether to disable GEMV mode when M=1. **Only Ascend 950PR/Ascend 950DT**. |
```

**Requirements**:
- Each parameter must specify **value range** (mathematical interval) and **default value**
- Parameters with **product differences** annotated inline: "Only xxx product" or separate columns
- **Mutual exclusion/dependency** marked with **Note** blocks (e.g., "Ineffective when bias is present")
- Deprecated parameters marked with `> **Deprecated**` and alternative provided
- BitMode parameter class (union + bit-field design) requires design rationale explanation, listing each bit field's meaning and Get/Set functions

---

#### 6. Data Types

**Format**: List type combination tables by product series. Using Mmad API as an example, separate "no bias" and "with bias" tables.

**Sample**:

```
For Atlas A2/A3 series products:

| Left Matrix A | Right Matrix B | Result Matrix C |
|--------------|----------------|-----------------|
| int8_t | int8_t | int32_t |
| half | half | float |
| float | float | float |
| bfloat16_t | bfloat16_t | float |
| int4b_t | int4b_t | int32_t |

| Left Matrix A | Right Matrix B | Bias | Result Matrix C |
|--------------|----------------|------|-----------------|
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| ... | ... | ... | ... |

For Ascend 950PR/Ascend 950DT products:
(Similar tables, including fp8/hifloat8 and other unique types)
```

**Requirements**:
- **Must separate by product series**— different chips support different type combinations (e.g., 950PR adds fp8/hifloat8)
- No bias and with bias are **different tables** (bias column has additional type constraints)
- Column headers use type names (no abbreviations), sorted by precision from low to high
- Each combination is one row; do not merge "int8_t/int4b_t"

---

#### 7. Return Value Description (Conditionally Required)

**Scope**: APIs with return values (e.g., Cast returns LocalTensor reference, GetBlockIdx returns uint32_t).

**Sample**:

```
## Return Value Description

| Return Type | Description |
|------------|-------------|
| LocalTensor<T>& | Returns reference to the destination operand, type matches template parameter T. Supports chained calls. |
```

**Requirements**:
- Table listing return value type and meaning
- If return value supports chained calls, annotate "Supports chained calls"

---

#### 8. Constraints

**Format**: Two layers. Single-parameter constraints are described inline in the parameter table (e.g., alignment, value range); multi-parameter constraints and multi-interface combination constraints in a separate "Constraints" paragraph.

**Requirements**:
- **Location constraints** must prioritize the physical storage location, followed by the TPosition enum value in parentheses
- **NOP constraints** must specify avoidance scenarios and performance impact
- **Fractal granularity** must specify minimum computation block size and invalid data layout rules
- **Synchronization constraints** must provide explicit threshold formula and code snippet
- **Special value handling** must provide CTRL register configuration method
- Each complex feature (UnitFlag/kDirectionAlign/GEMV, etc.) gets an independent subsection + link to detailed description

---

#### 9. Key Feature Description (Conditionally Required)

**Scope**: Required for matrix/vector computation APIs involving key hardware features (HF32 mode, GEMV acceleration, UnitFlag parallel control, etc.). Simple APIs (configuration/utility) may omit.

**Requirements**:
- Each feature gets an independent subsection with ### heading
- Feature description includes: what it is, when to use, how to configure, what constraints apply
- Complex features use tables to compare modes
- Each feature subsection ends with a link to detailed documentation (if available)

---

#### 10. Theoretical Performance (Appendix Reference)

**Format**: Do not inline cycle formulas in the API page. Reference the appendix instead.

**Requirements**:
- Theoretical performance data maintained centrally in the appendix
- Reference format is a link to the corresponding appendix page
- If the API has special performance considerations, supplement after the appendix reference

---

#### 11. Code Example

**Format**: Code snippet + link to sample repository.

**Requirements**:
- Link to the specific directory in the sample repository (if a corresponding sample exists). If no sample is currently available, the link may be omitted.

---

#### 12. Related APIs (Conditionally Required)

**Format**: Three layers — mutually exclusive APIs (warning box) → similar API selection guide (table) → underlying mapping (collapsible).

##### Mutually Exclusive APIs

Use `> **Warning**` block to prominently mark APIs that cannot be used simultaneously.

```
> **Warning**: The following interfaces are mutually exclusive with the current interface and cannot be used in the same Tiling strategy:
> - **MmadWithSparse**: Used when structured sparsity (4-out-of-2) is enabled; mutually exclusive with Mmad.
> - **MXMmad**: Used for MX matrix computation scenarios; mutually exclusive with Mmad.

> **Note**: Mmad's with-bias and without-bias prototypes are **not mutually exclusive**; either may be freely selected.
```

##### Similar API Selection Guide

Use a table to list functionally similar APIs with selection recommendations.

```
| Similar API | Difference | Selection Recommendation |
|-------------|-----------|------------------------|
| [MmadWithSparse](MmadWithSparse.md) | Supports 4-out-of-2 structured sparse matmul | Use when weight matrix has been pruned to sparse format |
| [MXMmad](MXMmad.md) | Supports MX format (microscaling) matmul | Use for MX mixed-precision training |
| [Iterate](Iterate.md) + [GetTensorC](GetTensorC.md) | Matmul high-level API loop iteration | Prefer high-level API when fine-grained L0A/L0B control is not needed |
| Mmad with-bias prototype | Supports bias matrix initialization of C | Use for C+=A×B+Bias; simpler than cmatrixSource |
| Mmad without bias + cmatrixSource | Configure C initial value source via parameter | Use when C initial value from BT Buffer but no Bias needed |
| Mmad + MmadBitModeParams | Bit-field union parameter, single-input passthrough | Use for extreme performance / fine-grained bit manipulation |
```

##### Underlying built-in Mapping (Optional, Collapsible)

```
<details>
<summary>Underlying built-in interface mapping (click to expand)</summary>

Mmad interface is abstracted on top of built-in interface (mad):

For Atlas A2/A3:
void mad(__cc__ float *c, __ca__ half *a, __cb__ half *b,
    uint16_t m, uint16_t k, uint16_t n, uint8_t unitFlag,
    bool kDirectionAlign, bool cmatrixSource, bool cmatrixInitVal);

For Ascend 950PR/Ascend 950DT:
void mad(__cc__ float *c, __ca__ half *a, __cb__ half *b,
    uint16_t m, uint16_t k, uint16_t n, uint8_t unit_Flag_ctrl,
    bool gemv_ctrl, bool BTbuf_ctrl, bool zero_Cmatrix_ctrl);
</details>
```

---

### Operator Practice Reference Writing Conventions

The Operator Practice Reference is the **practice case source**, responsible for expanding practice and optimization content that the Programming Guide mentions briefly.

**Standard Template**:

```markdown
# <Operator Name> Operator Practice Reference

## Prerequisites
Before reading this document, you should understand: xxx concept (link to Programming Guide), xxx API (link to API Reference).

## Operator Implementation
### Basic Version
(Code snippet + description + link to complete sample)

### Advanced Version
(If applicable: performance-optimized version, multi-data-type version, etc.)

## Performance Optimization
(If applicable: list optimization techniques, link to optimization topics)

## Common Issues
(If applicable: common pitfalls during development)
```

**Key Requirements**:
- For operators with multiple implementation approaches (e.g., MemBase vs RegBase), start with an approach difference table + selection recommendations
- First API occurrence in code → link to API Reference Manual; subsequent occurrences do not repeat
- Optimization approaches that vary by architecture version must be annotated with `[Applicable Version: 仅xxx]` and linked to Cross-generation Migration Guide

### Cross-generation Migration Guide Writing Conventions

The Cross-generation Migration Guide is the **compatibility authority source**, responsible for explaining differences and migration paths between architecture versions.

**Migration Mapping Entry Format**:

```markdown
## <Legacy API Name> → <New API Name>

### Change Description
(One sentence explaining why and when the change occurred)

### Parameter Difference Comparison

| Parameter | Old Interface | New Interface | Difference |
|-----------|--------------|---------------|------------|
| ... | ... | ... | ... |

### Constraint Differences
(List differences in alignment, data types, element count, etc.)

### Migration Code Example
(Show comparison of old vs new syntax)

### Migration Verification
(How to verify functionality and performance after migration)
```

**Key Requirements**:
- Each mapping entry must link to both old and new API details in the API Reference Manual
- Concept changes must link back to the corresponding Programming Guide chapter
- Use unified `[Applicable Version: xxx]` annotation format

---

## Code Example Writing Conventions

### Code Snippet Requirements

| Requirement | Description |
|------------|-------------|
| Focused | Only show code relevant to the current explanation; use `// ... other initialization` for unrelated parts |
| Understandable | Each snippet accompanied by 2-3 lines of text explaining what it does |
| Traceable | Snippet ends with link to complete sample in the sample repository |
| Keyword Annotated | Ascend C-specific keywords (`__aicore__`, `__ubuf__`, `__simd_vf__`, etc.) must have inline comments |
| Parameter Coverage | Cover common parameter combinations, not just a single usage |

### Keyword Annotation in Examples

```cpp
__aicore__ inline void ExampleKernel(__gm__ uint8_t* x) {  // __aicore__=kernel function modifier, __gm__=Global Memory address space
    // ...
}
```

---

## Performance Optimization Case Writing Conventions

Refer to existing cases under `算子实践参考/优秀实践/` (e.g., FlashAttention, Matmul series). Recommended structure:

```markdown
# <Operator Name> Performance Tuning Case

## Background
(What is the performance bottleneck for this operator in real-world scenarios)

## Optimization Approach
(Which dimensions to optimize: Tiling, memory access, pipeline scheduling, instruction selection, etc.)

## Optimization Implementation
### Baseline Version
(Unoptimized code snippet)

### Optimized Version
(Optimized code snippet, explaining each change)

## Performance Comparison

| Metric | Baseline | Optimized | Improvement |
|--------|----------|-----------|-------------|
| Bandwidth Utilization | ... | ... | ... |
| Compute Throughput | ... | ... | ... |

## Applicable Scope
[Applicable Version: xxx] (if optimization varies by architecture version)
```

---

## PR Submission Checklist

Before submitting a documentation PR, check each item:

**Content Accuracy**:
- [ ] All hardware parameter values (capacity, granularity, range) are consistent with Architecture Specifications
- [ ] "Fixed value" and other absolute descriptions have been verified (many "fixed values" are actually determined by configuration parameters)
- [ ] Data path descriptions match actual routes (no contradictions with other documents)

**Constraint Completeness**:
- [ ] Each API's supported data type list is complete
- [ ] Address alignment requirements are specified (bytes and direction)
- [ ] Element count range (min, max, alignment granularity) is documented
- [ ] API combination constraints (mutual exclusion, required pairing) are annotated
- [ ] Cross-version differences are annotated with `[Applicable Version: xxx]`

**Link Conventions**:
- [ ] First mention of API/concept from another document has a link
- [ ] Subsequent mentions of the same concept/API are not re-linked
- [ ] API pages have "Prerequisites" section linking to Programming Guide
- [ ] Version differences link to Cross-generation Migration Guide

**Readability**:
- [ ] Terminology matches chapter abstraction level (abstract names at high level, hardware names annotated at low level)
- [ ] Easily confused concepts distinguished with comparison tables (not text-only hints)
- [ ] Code snippets are focused, have text descriptions, and sample repository links
- [ ] Ascend C-specific keywords have inline comments

**Example Quality**:
- [ ] Each API has at least 1 minimal runnable example
- [ ] Examples can be copied, compiled, and run
- [ ] Multiple data types/parameter combinations are covered (not just single usage)

---

## Document Navigation Network

The five documents are connected through cross-references to form a navigation network, following the principle of "whoever mentions content managed by another document adds the link":

```
Getting Started Tutorial ──in-depth concepts──→ Programming Guide
                    ──first API mention──→ API Reference Manual

Programming Guide ──first API mention──→ API Reference Manual
                 ──first optimization──→ Operator Practice Reference
                 ──version differences──→ Cross-generation Migration Guide

API Reference Manual ──prerequisites──→ Programming Guide
                     ──version differences──→ Cross-generation Migration Guide

Operator Practice Reference ──first API usage──→ API Reference Manual
                           ──programming concepts──→ Programming Guide
                           ──cross-architecture optimization──→ Cross-generation Migration Guide

Cross-generation Migration Guide ──concept definitions──→ Programming Guide
                               ──new API details──→ API Reference Manual
```

The sample repository (`asc-devkit/examples/`) is not part of the documentation system, but code examples in the five documents may link to the sample repository.

---

## More Information

- Community code of conduct and CLA signing: [cann-community](https://gitcode.com/cann/community)
- Issue and PR process: See [Submit Issue / Handle Issue Tasks](https://gitcode.com/cann/community#提交Issue/处理Issue任务)
- API code contribution guides:
  - Advanced API: [asc_adv_api_contributing.md](./asc_adv_api_contributing.md)
  - Basic API: [asc_basic_api_contributing.md](./asc_basic_api_contributing.md)
  - C API: [asc_c_api_contributing.md](./asc_c_api_contributing.md)
