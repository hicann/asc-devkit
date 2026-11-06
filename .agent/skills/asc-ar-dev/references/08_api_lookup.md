# API Documentation Lookup Workflow

Use this workflow whenever kernel code introduces a new AscendC API, overload, dtype, tensor position, parameter structure, or target-architecture use. The purpose is to prevent plausible-looking but unsupported calls from being copied out of snippets or generated from memory.

## What Counts As A Kernel API

Use a call-site rule rather than a name-only rule: a kernel API is any public devkit symbol referenced from device-side kernel code or from a device helper reachable from that kernel.

It includes newly introduced functions, operator overloads, macros, types, classes, parameter structures, and objects from public kernel-facing surfaces such as:

- `include/basic_api`：SIMD basic compute, data movement, tensor, queue/pipe, synchronization, debug, and core/block APIs;
- `include/adv_api`：high-level kernel algorithms and objects such as Matmul or Softmax components;
- `include/c_api`：device-side C APIs used by kernel code;
- `include/simt_api`：SIMT and SIMD/SIMT mixed-programming device APIs;
- `include/tensor_api`：Tensor API layouts, tensors, memory pointers, and compute interfaces when used by the target kernel;
- `include/aicpu_api`：only when the code being written is an AI CPU kernel;
- `include/utils`：only when the selected utility is documented as callable from the device-side kernel context.

Kernel API scope also covers public support symbols required to call those interfaces, including `LocalTensor`, `GlobalTensor`, `Tensor`, `TPipe`, `TQue`, `TBuf`, `TPosition`, repeat/mask/copy parameter structures, memory barriers/events, and core/block query functions. A newly introduced type or parameter structure triggers the same documentation gate as a function call.

The following are not kernel APIs by default:

- Host-side runtime APIs such as `acl*` and `aclrt*`, kernel launch configuration, stream/device management, and host file I/O;
- host-side tiling generation, operator registration, platform discovery, RTC/compile tooling, Python scripts, CMake, and packaging interfaces;
- ordinary C++ helpers or project-local functions and types defined by the target code itself;
- Internal helpers under `impl/`; they are source evidence or devkit implementation details, not public interfaces to call from user kernel code. When the task is implementing devkit internals, inspect them as implementation targets without reclassifying them as public kernel APIs.

If a symbol is ambiguous, trace its declaration and documented header before writing code. If it is referenced from device-side code and classification is still unresolved, treat it as a kernel API until classified and apply the lookup gate.

## Mandatory Timing

Complete the lookup before writing or editing the API call in kernel code. An API is “new” for this gate when any of the following changes:

- the target code did not already use the API;
- a different function overload or operator form is selected;
- dtype, source/destination tensor kind, or `TPosition` changes;
- mask, repeat, tiling, copy, synchronization, or parameter-structure form changes;
- the same call is moved to another SoC architecture or core type.

Within one task, a verified API/overload combination may be reused without repeating the full lookup. Do not carry that verification across another devkit revision or a different architecture without checking again.

## API Documentation Lookup Order

1. Resolve `DEVKIT_PATH` and confirm that `${DEVKIT_PATH}/docs/api` exists.
2. Start from `${DEVKIT_PATH}/docs/zh/api/README.md` to understand the documentation hierarchy.
3. Use `${DEVKIT_PATH}/docs/zh/api/Ascend-C-API列表.md` to select the correct API family:
   - SIMD basic API;
   - SIMD high-level API;
   - SIMD C API;
   - SIMT API;
   - AI CPU API;
   - Utils API.
4. Search the selected documentation subtree for the exact API name. If several files share one API name, select by data path, storage position, programming model, and overload semantics. `DataCopy`, for example, has separate documents for different GM/UB/L1/L0 paths and transfer forms.
5. Read the exact API document rather than stopping at an index or similarly named interface.

## Required Document Sections

For each selected API document, inspect these sections when present:

- `产品支持情况`：confirm target product and core support.
- `功能说明`：confirm the operation, header path, and intended programming model.
- `函数原型`：copy the exact overload, template parameters, parameter order, and qualifiers.
- `参数说明`：confirm input/output direction, tensor type, `TPosition`, alignment, range, and parameter semantics.
- `数据类型`：confirm the exact dtype or dtype combination for the target product.
- `约束说明`：confirm overlap, alignment, count/repeat, temporary-buffer, pipeline, synchronization, and architecture-specific constraints.
- `调用示例`：confirm namespace, initialization, parameter construction, and surrounding synchronization pattern.

Do not infer support from the API name alone. A documented API may expose several overloads with different product, dtype, storage-position, or alignment constraints.

## Source Cross-Check

After reading the documentation, cross-check the current repository revision:

1. Find the public declaration under `${DEVKIT_PATH}/include` and confirm the documented header and prototype.
2. Find the target-architecture implementation under `${DEVKIT_PATH}/impl` when behavior, compile-time dispatch, or synchronization matters.
3. Inspect relevant non-test `${DEVKIT_PATH}/examples` for a call matching the selected overload and target architecture; do not use `test/` or `tests/` as product implementation evidence.
4. If documentation and source disagree, stop and report the conflict instead of silently choosing one.

Documentation defines the supported public usage. Source confirms that the selected usage matches the current checkout. A nearby call site or snippet does not replace either check.

## Missing Documentation Fallback

If no exact API document exists, search the declaration, implementation, and non-test examples. Explicitly record that the documentation is missing and identify the source-backed evidence used instead. Do not copy a similarly named overload or another architecture's implementation by analogy.

If the available evidence cannot establish the signature, target-architecture support, dtype, tensor position, or required synchronization, do not write the API call. Report the unresolved item and ask for authoritative input or a newer devkit revision.

## API Evidence Log

Keep this compact table in working notes before code generation or editing:

| API | Documentation | Declaration | Selected overload | Target | Key constraints |
|-----|---------------|-------------|-------------------|--------|-----------------|
| `AscendC::ApiName` | `docs/zh/api/.../ApiName.md` | `include/..._intf.h` | exact prototype or overload description | SoC, core, dtype, tensor/TPosition | alignment, count/repeat, buffer, synchronization |

For APIs with multiple data paths or overloads, create one row per selected form. The evidence log may stay in working notes unless the user requests it in the final report, but the final implementation summary should name the documentation paths consulted for newly introduced APIs.

## Code Review Checklist

Before accepting generated or edited kernel code, confirm:

- every newly introduced API has an evidence-log row;
- the code uses the documented header, namespace, and exact prototype;
- product, core, dtype, tensor type, and `TPosition` are supported;
- alignment, overlap, count/repeat, temporary-buffer, and synchronization constraints are satisfied;
- parameter structures use documented fields and supported values;
- non-test examples match the intended overload rather than only sharing the same API name;
- no call was justified only by a bundled snippet, memory, or another architecture's implementation.
