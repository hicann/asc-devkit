---
okftype: reference.macro
description: "Two conditional macros in AIC and AIV separation mode, which respectively determine whether the current code path runs on the Cube Core or the Vector Core, and are used as if statement conditions to isolate cube and vector code. They are C API aliases of ASCEND_IS_AIC and ASCEND_IS_AIV."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md
    kind: related
    condition: "The body text references this page to explain AIC and AIV separation mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "ASC_IS_AIC/ASC_IS_AIV were migrated from impl/c_api/instr_impl/npu_arch_2201/utils_impl/utils_impl.h to the public header file c_api/defs/macro.h, which users can include directly. The definition form is unified as aliases of ASCEND_IS_AIC/ASCEND_IS_AIV (in 9.0.0-beta.1, the two were g_c_api_core_type comparison expressions, and were changed to aliases starting from 9.0.0), with unchanged semantics."
---
# ASC_IS_AIC/ASC_IS_AIV

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:15:20.961Z pushedAt=2026-11-18T08:15:34.406Z nl=2 -->

## Description

The header file path is: `"c_api/defs/macro.h"`.

`ASC_IS_AIC` and `ASC_IS_AIV` are used for conditional compilation and execution of the code paths of Cube Core (AIC) and Vector Core (AIV) in [separation mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), and can be used as the condition of an `if` statement. When compiling or running the AIC code path, the `ASC_IS_AIC` condition holds; when compiling or running the AIV code path, the `ASC_IS_AIV` condition holds.

These two macros are the C API aliases of `ASCEND_IS_AIC` and `ASCEND_IS_AIV`, respectively, and the corresponding macros have the same functionality. When developing a `__mix__` fusion operator based on the AIC and AIV separation mode, you can use these two macros to isolate code such as cube computation and cube data movement that is executed only by AIC, as well as code such as vector computation and vector data movement that is executed only by AIV.

## Macro Definition

```cpp
#define ASC_IS_AIC ASCEND_IS_AIC
#define ASC_IS_AIV ASCEND_IS_AIV
```

## Value Description

| Current Code Path | `ASC_IS_AIC` Condition Result | `ASC_IS_AIV` Condition Result |
| --- | --- | --- |
| AIC | `true` | `false` |
| AIV | `false` | `true` |

## Constraints

- These two macros apply only to [separation mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md).
- In a fusion operator in AIC and AIV separation mode, AIC and AIV execute different types of instructions. Code that can be executed only on AIC must be placed within the `ASC_IS_AIC` conditional branch; code that can be executed only on AIV must be placed within the `ASC_IS_AIV` conditional branch.
