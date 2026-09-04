# Ascend C Documentation Contribution Guide

## Overview

The Ascend C documentation system consists of five core documents. Developers can improve and contribute to the documentation by submitting PRs:

| Document | Content | Directory |
|----------|---------|-----------|
| Getting Started Tutorial | Ascend C overview, environment setup, quick-start hands-on (HelloWorld, first operator) | `docs/zh/guide/getting_started` |
| Programming Guide | Programming models, programming paradigms, compilation & execution, hardware architecture, advanced programming | `docs/zh/guide/programming_guide` |
| API Reference Manual | Interface parameters, constraints, examples, API associations | `docs/zh/api/` |
| Operator Practice Reference | Operator implementation, performance optimization, tuning cases | `docs/zh/guide/operator_practice/` |
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

1. Refer to existing tuning case structures under `operator_practice/best_practices/`
2. Write documentation following the "Performance Optimization Case Writing Convention" in this guide
3. Submit a PR

---

## Writing Conventions

Ascend C documentation follows the three-dimensional requirements defined in the [Ascend C Documentation Design Principles](./asc_doc_design_rules.md): discoverability, readability, and completeness. Below are specific conventions for common writing scenarios.

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

| No. | Section Title | Required | Description |
|-----|---------------|----------|-------------|
| 1 | Product Support Status | ✅ Required | Describes the API support status for each product series |
| 2 | Function Description | ✅ Required | Covers the API's purpose, user value, and basic usage |
| 3 | Function Prototypes | ✅ Required | Lists all overloads, with each prototype in a separate code block |
| 4 | Parameter Description | ✅ Required | Parameter names must exactly match the function prototypes; template parameters and function parameters must be documented in separate tables |
| 5 | Data Types | Conditionally required | If all chips support the same data types, a separate section is unnecessary; the data types may be documented together with the parameters |
| 6 | Return Value Description | ✅ Required | Specifies the return-value unit and the meaning of each possible value |
| 7 | Pipeline Type | Conditionally required | Specifies the API's pipeline type |
| 8 | Constraints | ✅ Required | Provides complete and reasonable constraints |
| 9 | Key Feature Description | Conditionally required | Matrix/vector computation APIs involving key hardware features such as HF32, GEMV, or UnitFlag must include corresponding feature descriptions; simple APIs may omit this section |
| 10 | Code Example | ✅ Required | Provides a code snippet and a link to the sample repository when a corresponding sample exists; the link may be omitted when no sample is available |

For detailed writing requirements for each section, see the [Ascend C API Writing Guidelines](./asc_api_writing_guidelines.md).

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
- Optimization approaches that vary by architecture version must be annotated with `[Applicable Version: only xxx]` and linked to Cross-generation Migration Guide

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

Refer to existing cases under `operator_practice/best_practices/` (e.g., FlashAttention, Matmul series). Recommended structure:

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
- [ ] All hardware parameter values (capacity, granularity, range) are consistent with [Architecture Specifications](../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/architecture_spec.md)
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
