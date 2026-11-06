---
name: asc-ar-dev
description: AscendC 需求分析与实现技能。当用户需要分析 AscendC 需求、编写需求文档、实现或评审 asc-devkit host/device 代码，或规划基础 API、高阶 API、编译工程、tiling、matmul、vector pipeline、debug/dump、元数据相关验证方案时使用。每次编写或修改 kernel 代码并引入新的 AscendC API、重载或参数组合时，必须先查询当前 devkit 仓的 `docs/api` 资料并核对声明、实现与样例。触发场景疑似匹配时，先从当前 workspace、本 skill 所在仓、用户输入或环境变量收集 devkit 路径、CANN 路径、SoC 架构和任务目标，不假设私有机器或本地目录。
---

# asc-ar-dev

## 使用边界

Use this skill for reusable AscendC engineering work in `asc-devkit` or a user-provided AscendC devkit-like repository:

1. Decompose a raw task into an AscendC requirement document.
2. Implement or review AscendC host/device code.
3. Plan build, simulation, NPU, documentation, and regression validation.
4. Route specialized work to repository-local skills when a narrower skill owns the task.

Boundary with other repository skills:

- API UT generation and coverage backfill belong to [`../asc-api-ut-gen/SKILL.md`](../asc-api-ut-gen/SKILL.md).
- Chip names, `SocVersion`, `__NPU_ARCH__`, dtype facts, and architecture support boundaries belong to [`../asc-npu-arch/SKILL.md`](../asc-npu-arch/SKILL.md). This skill may reference those facts but must not maintain a duplicated full chip or dtype table.
- Before implementing or reviewing C/C++, AscendC, or Shell product code, load [`../asc-code-dev-standards/SKILL.md`](../asc-code-dev-standards/SKILL.md) and its applicable references; recheck them before delivery. ASC-28 excludes `test/` and `tests/` as product implementation evidence, while allowing tests to be run or maintained for verification.
- Markdown documentation style checks belong to `asc-doc-checker` when that skill is available.

Keep this skill generic. Do not encode user-specific paths, SSH hosts, token files, private PR links, employee IDs, or one-off debugging transcripts. If a task needs such information, ask for it at runtime or derive it from the live environment.

## Required Inputs

Before doing repository-backed work, resolve the minimum input bundle:

- `DEVKIT_PATH`: asc-devkit or target repository path.
- `CANN_PATH`: CANN package installation path.
- `SOC_ARCH`: target SoC architecture, for example `dav-2201` or another user-provided architecture.
- `TASK_GOAL`: requirement, bug, review target, or implementation objective.

For this repository, derive `DEVKIT_PATH` by walking upward from the current workspace or this skill directory until an asc-devkit root is found. A valid root normally contains `CMakeLists.txt`, `include/`, `impl/`, `tools/`, and `.agent/`. If the user explicitly targets another repository, use the user-provided path after validating it.

If fields are missing, ask only for the missing values. For `CANN_PATH`, prefer validated environment variables such as `ASCEND_HOME_PATH` or `ASCEND_TOOLKIT_HOME` before asking.

## Output Contract

Unless the user requests another format, structure requirement or implementation responses as:

```md
## 需求
## 背景简述
## 涉及领域
## 方案详述
```

For code-review or debugging tasks, lead with concise findings, then evidence, patch strategy, and verification commands.

## AscendC Call-Layer Rules

- Ordinary host or helper C++ code may launch kernels only through `<<<>>>`.
- Ordinary host or helper C++ code must not call `__aicore__` functions directly.
- `__global__` functions may call `__aicore__` functions only.
- `__global__` functions must not call another `__global__` function.
- `__aicore__` functions may call other `__aicore__` functions.
- `__aicore__` functions must not call `__global__` functions.
- Prefer a thin `__global__` entry and reusable `__aicore__ inline` helpers or `__aicore__` class members.
- Keep device-side data structures and operations on the device side: `GM_ADDR`, `GlobalTensor`, `LocalTensor`, `TPipe`, `TQue`, `DataCopy`, tiling structs, block/core APIs, and pipeline barriers.
- If a source file already includes `kernel_operator.h`, treat it as the AscendC umbrella header and avoid adding redundant AscendC headers unless required.

## Path Resolution Rules

- Resolve `DEVKIT_PATH` before inspecting repository examples, docs, builds, tests, patches, or source files.
- Resolve `CANN_PATH` before invoking CANN binaries, includes, simulator, package tools, or examples.
- Prefer `ASCEND_HOME_PATH`, then `ASCEND_TOOLKIT_HOME`; normalize toolkit subdirectories only after checking the expected CANN layout.
- Validate candidate paths by checking task-relevant files such as compiler binaries, tools, headers, simulator directories, or repository directories.
- Never fall back to a hard-coded personal path.
- Keep user-facing reports explicit about the paths and architecture actually used for this run.

## New API Documentation Gate

### Kernel API Scope

Kernel API classification is call-site based. For this gate, a kernel API is any public devkit symbol referenced from device-side kernel code or a device helper called by that kernel. It includes function calls and operators, and also newly introduced public types, parameter structures, tensor/buffer/queue objects, memory movement, compute, synchronization, debug, and core/block interfaces.

Host runtime, launch, tiling-generation, operator-registration, file-I/O, Python, CMake, and build-tool APIs are not kernel APIs unless the code being written is itself the corresponding device-side kernel. Internal `impl/` helpers are implementation evidence, not public interfaces for user kernel code.

Use [`references/08_api_lookup.md`](references/08_api_lookup.md#what-counts-as-a-kernel-api) for the complete inclusions, exclusions, and ambiguous-symbol rule.

Before writing or editing kernel code, identify every newly introduced AscendC API symbol. “Newly introduced” includes an API not already used by the target code, a different overload, a changed dtype or tensor position, a new parameter structure, or use on another SoC architecture.

For each such API:

1. Start from `${DEVKIT_PATH}/docs/zh/api/README.md` and `${DEVKIT_PATH}/docs/zh/api/Ascend-C-API列表.md`, then locate the exact SIMD basic/high-level API, C API, SIMT API, AI CPU API, or Utils API document.
2. Read the API document sections for product support, header path, function prototype, parameters, supported data types, constraints, synchronization requirements, and calling examples.
3. Cross-check the public declaration under `${DEVKIT_PATH}/include`, the implementation under `${DEVKIT_PATH}/impl`, and relevant non-test examples. Documentation selects the supported usage; source inspection confirms the exact repository revision and overload.
4. Keep a compact API evidence log in the working notes: `API -> documentation path -> declaration path -> selected overload -> target SoC/dtype/TPosition -> key constraints`.

Do not write the API call until this lookup is complete. If the exact API document is absent, do not guess from a similarly named interface or a bundled snippet. Inspect the declaration, implementation, and non-test examples, explicitly record that documentation is missing, and state any remaining uncertainty before using the API.

Follow [`references/08_api_lookup.md`](references/08_api_lookup.md) for the search order and evidence checklist.

## Workflow

1. Collect the input bundle and inspect repository context.
2. Classify the task:
   - basic API or high-level API
   - compile-engineering or build-system work
   - host/kernel bootstrap
   - vector pipeline or queue-based kernel
   - tiling or matmul kernel
   - debug/dump/log path
   - `.ascend.meta`, TLV, or compiler DFX metadata
   - documentation or guide update
   - code review or refactor
3. Load only relevant references:
   - [`references/README.md`](references/README.md) for reference navigation.
   - [`references/00_format_template.md`](references/00_format_template.md) for requirement output shape.
   - [`references/01_call_rules.md`](references/01_call_rules.md) for host/global/aicore call hierarchy.
   - [`references/02_local_environment.md`](references/02_local_environment.md) for environment discovery and validation.
   - [`references/03_devkit_snippets.md`](references/03_devkit_snippets.md) for generic code patterns.
   - [`references/04_debug_dump.md`](references/04_debug_dump.md) for debug and dump constraints.
   - [`references/05_build_meta.md`](references/05_build_meta.md) for metadata and build artifacts.
   - [`references/06_task_goal_example.md`](references/06_task_goal_example.md) for a neutral input bundle example.
   - [`references/07_requirement_type_routing.md`](references/07_requirement_type_routing.md) for routing by requirement type.
   - [`references/08_api_lookup.md`](references/08_api_lookup.md) before kernel code introduces a new API, overload, dtype, tensor position, parameter structure, or architecture-specific use.
4. Implement or review the smallest correct change consistent with official docs and repository style.
5. Verify with the narrowest relevant build/test first, then broader validation if available.

## Verification Checklist

Before finalizing AscendC code changes:

- Host code launches kernels through `<<<>>>`.
- No host code directly calls `__aicore__` functions.
- No `__global__` function calls another `__global__` function.
- `__aicore__` functions do not call `__global__` functions.
- Buffer positions and tensor storage locations match the target API.
- Pipeline barriers/events protect real producer-consumer dependencies.
- Error handling covers failed file I/O, allocation, stream creation, and runtime API calls when host code is involved.
- Every newly introduced kernel API has an API evidence log backed by the current devkit documentation and source tree.
- Verification commands and their outputs are recorded accurately.

## Privacy and Portability Rule

Do not write private machine names, SSH commands, personal home paths, token paths, private PR links, or one-off project debugging notes into this skill. Convert recurring lessons into environment-neutral patterns, or keep personal details outside the repository-local skill.
