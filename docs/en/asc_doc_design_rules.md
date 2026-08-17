# Ascend C Documentation Design Principles

## Design Goals

Ascend C documentation is the sole official entry point for developers using Ascend NPUs for operator development. Documentation quality directly impacts developer efficiency across the entire "understanding → onboarding → mastery" pipeline.

This design document defines Ascend C documentation requirements across three dimensions:

| Dimension | Core Question | Target State |
|-----------|--------------|-------------|
| **Discoverability** | Can developers find the information they need within 3 steps? | From "search → guess → trial-and-error" to "navigate → locate → understand" |
| **Readability** | Once found, can it be understood without ambiguity? | From "repeated inference + cross-validation" to "read once and understand" |
| **Completeness** | After understanding, is the information sufficient to complete the task? | From "documentation only covers half, the rest requires trial-and-error" to "follow documentation to complete the task directly" |

Additionally, the cross-version migration scenario (involving the Cross-generation Migration & Compatibility Guide and version constraint annotation conventions) simultaneously involves all three dimensions: developers must be able to find migration information (discoverability), understand migration steps (readability), and ensure no functionality regression after migration (completeness).

---

## Documentation System Architecture

The Ascend C documentation system consists of **five** core documents, connected through cross-references to form a navigable documentation network.

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

---

## Discoverability

### Three-layer Navigation System

The documentation provides three layers of navigation covering different usage scenarios:

| Navigation Layer | Format | Applicable Scenario | Design Requirement |
|-----------------|--------|--------------------|--------------------|
| **Global Layer** | Directory tree + full-text search | Know what you're looking for | Directory depth ≤ 5 levels; search covers code/parameters/concept names |
| **Decision Layer** | Decision trees / comparison tables / selection guides | Know the requirement but not which option | Every multi-choice fork must have a decision tree or comparison table |
| **Association Layer** | Bidirectional cross-references + Related APIs section | Found A but need to understand related B | Five documents interlinked to form a navigable network |

**Specific Requirements**:
- Every multi-path chapter in the Programming Guide (SIMD/SIMT selection, API level selection) must start with a decision tree or comparison table
- Every API family in the API Reference Manual (Reduce series, Matmul series, DataCopy series) must start with a selection comparison table
- Every operator in the Operator Practice Reference with multiple implementation approaches must start with an approach difference table + selection recommendations
- Each comparison table must provide a clear default recommendation ("If unsure, prefer X")
- Include "common wrong choice scenarios" (e.g., "Encountering XXX error may be because you chose A instead of B")

### Five-document Link Interconnection

The five core documents are connected through links to form a navigable network. **Link direction follows the principle of "whoever mentions content managed by another document adds the link"**:

**Link Relationships (11 link rules)**:

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
| L9 | Cross-generation Migration Guide | API Reference Manual | Migration target uses new API | Legacy API → new API mapping table links to new API details |
| L10 | Cross-generation Migration Guide | Programming Guide | Migration involves programming concept changes | Architecture change mapping links to concept explanation |

**Link Rules**:
- Links are added on the **first occurrence** of a concept/API; subsequent occurrences are not re-linked
- Concept references in API pages use inline links near the reference (not centralized at page bottom)
- **Note direction**: Operator Practice Reference → API Reference Manual (to look up interface details); the API Reference Manual does not need reverse links to Operator Practice Reference
- The Cross-generation Migration Guide is both a link initiator and receiver: it links out to the Programming Guide (concept changes) and API Reference Manual (new API details); it receives links from other documents when they involve version differences
- All complete code examples in the documentation reference the [sample repository](../../examples)

### Unified Terminology Entry

Establish a **unified terminology mapping table** (independent appendix file) shared by all documents:

| Mapping Content | Format | Location |
|----------------|--------|----------|
| Terminology Mapping Table | Four-column mapping: abstract concept ↔ hardware unit ↔ programming model ↔ interface name | Independent appendix file; overview chapter links to it |
| Product Model Mapping Table | Product name ↔ chip name ↔ architecture codename | Programming Guide overview chapter |
| Synonym Index | Multiple names for the same concept → authoritative explanation page | Appendix or index page |

**Specific Requirements**:
- Any code name / abbreviation that appears in the documentation must be annotated with its full name on first occurrence and linked to the mapping table
- The same concept may use different-level names in different documents, but must have an equivalence relationship in the terminology mapping table
- Terminology groups that must be mapped: DMA/MTE/DataCopy (three-layer name for the same transfer function), Local Memory/L1/`__cbuf__` (three-layer name for the same storage), etc.

### File Responsibility Focus

Each file has a clear single responsibility. Overview files serve as **navigation pages**:

| File Type | Responsibility | Prohibited |
|-----------|---------------|------------|
| Overview/Summary File | Navigation: topic list + 1-2 sentence summary + sub-chapter links | Expanding technical details, extensive code |
| Concept Introduction File | Explanation: concept definition + principles + constraints + relationships | Repeating content already in other files (use references instead) |
| Operation Guide File | Steps: code snippets + operation steps + caveats | Concept definitions (link to concept files) |
| API Reference Page | Reference: prototype + parameters + constraints + examples + related APIs | Programming model explanations (link to Programming Guide) |
| Migration Guide File | Steps: change list + mapping table + verification steps | Programming model explanations (link to Programming Guide), repeating existing migration content (use references) |

**Specific Requirements**:
- Overview file length: navigation tables/links should occupy ≥ 60%
- If a file covers more than 3 unrelated topics, split into multiple sub-files
- Detailed description of the same content appears only once (authoritative version); other locations use "See Chapter X for details"
- Hardware architecture description is unified into a single file; separate SIMD/SIMT versions describing different halves are not allowed

---

## Readability

### Using Terminology by Abstraction Level

Documents use terminology appropriate to the chapter's abstraction level. **Cross-level references must be explicitly linked on first occurrence**:

| Chapter Level | Terminology Used | Cross-level Handling |
|--------------|-----------------|---------------------|
| High-level (abstract hardware architecture, overview) | Abstract names: Global Memory, Local Memory, compute units | Do not abruptly mix in UB, L0A, or other low-level names |
| Low-level (programming model, data transfer) | Hardware names: UB, L1 Buffer, L0A/B/C | **On first occurrence**, parenthetical association: `UB (i.e., part of Local Memory in the abstract architecture)` |
| API Level (API Reference Manual) | API names: DataCopy, LocalTensor | **On first occurrence**, associate hardware name: `DataCopy (triggers MTE data transfer engine instructions)` |

**Anti-pattern (prohibited)**: Abstract hardware architecture document directly writes "data is transferred from UB into VEC", skipping the abstraction layer.

### Explicit Distinction of Easily Confused Concepts

Similar-named or hierarchically confusable concepts must be **explicitly distinguished** using comparison tables or relationship diagrams:

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

---

## Completeness

### Content Completeness per Document Type

| Document Type | Required Content | Common Missing Items |
|--------------|-----------------|---------------------|
| Concept Introduction File | Concept definition + principles + constraints + relationships | Constraints documented separately or missing |
| Operation Guide File | Steps + code snippets + expected results + error handling | Error handling only exposed through compiler/runtime errors |
| API Reference Page | Header file + prototype + parameters + constraints + data types + example + related APIs | Constraint combinations not documented; examples only cover happy path |
| Migration Guide | Change list + old→new mapping + verification steps + compatibility scope | Compatibility scope and limitations not annotated |

### Version Constraint Unified Annotation Format

All version constraints in the documentation use unified annotation formats to ensure consistency and clear scope:

| Constraint Type | Annotation Format | Example |
|----------------|-------------------|---------|
| Product Scope | Fixed-format table at page top | "Product Support Status" table |
| API Version Range | Inline annotation near the API | "(≥ CANN x.y.z)" or "(2201-3510)" |
| Architecture-dependent Features | Independent paragraph with architecture qualifier | "This feature is only supported on Ascend 950PR architecture" |
| Deprecated APIs | Strikethrough + deprecation notice + alternative link | "~~OldAPI~~ (Deprecated, use NewAPI instead)" |

### Sample and Example Completeness

| Requirement | Standard | Verification Method |
|------------|---------|-------------------|
| Every API has a code example | At minimum one compilable snippet per prototype | Compilation verification |
| Examples cover common scenarios | At least one example per typical usage pattern | Scenario coverage review |
| Parameter values are realistic | No placeholder or meaningless values | Technical review |
| Examples are up-to-date | Consistent with the current version of API signatures | Version consistency check |

---

## Cross-version Migration Content Specification

### Migration Guide Writing Principles

| Principle | Requirement | Common Mistake |
|-----------|-------------|---------------|
| Change-centric | Organize by API change, not by version | Organizing by version, requiring readers to cross-reference |
| Bidirectional Mapping | Both old→new and new→old mappings | Only providing old→new, readers cannot determine which old API a new one replaces |
| Verification Completeness | Include functional verification + performance verification steps | Only verifying compilation, not verifying result consistency |
| Scope Clarity | Clearly state which scenarios support coexistence of old and new APIs | Unclear coexistence scope leading to usage errors |

### Version Constraint Annotation Specification

| Requirement | Standard | Anti-pattern | Impact if Missing |
|------------|---------|-------------|------------------|
| Product scope annotation | Every API page has a unified product support table | Mentioning "supported on A2" without specifying other products | Readers cannot determine applicability to their hardware |
| Version range annotation | First mention of version-limited features annotated | Version differences not annotated | Code compiles on one platform but fails on another |
| Architecture-dependent constraints | Constraints annotated with architecture codename | Only annotating "not supported" without specifying which architecture | Migration paths missing critical constraints |
| Deprecation annotation | Deprecated APIs marked with version and alternative | Discovering deprecation only through compiler warnings | Using deprecated APIs in new code |

---

## Appendix: PR Quality Check Quick Reference

### Pre-submission Self-check Items

| Category | Check Item | Pass Criteria |
|----------|-----------|--------------|
| Terminology | Abstract/hardware terms used appropriately | High-level chapters use abstract names; low-level chapters annotate hardware names |
| Links | First-occurrence linking rule followed | API name first occurrence has link; subsequent occurrences do not |
| Links | No orphan pages (pages with no inbound links) | Every page is reachable from at least one other page |
| Constraints | Constraints appear at first concept mention | Not deferred to later chapters |
| ISA | Hardware parameters consistent with ISA specs | Buffer sizes, alignment, and value ranges match specifications |
| Examples | Code examples match current API signatures | No outdated parameter names or removed interfaces |
| Terminology | New terms added to terminology mapping table | No undocumented abbreviations or code names |