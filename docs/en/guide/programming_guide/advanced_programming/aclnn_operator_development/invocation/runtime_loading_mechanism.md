# Runtime Loading Mechanism

<!-- md-trans-meta sourceCommit=fbd15d5df1f452aabb0f40f2b83dd25017ebaa8f translatedAt=2026-10-23T13:59:58.094Z pushedAt=2026-11-03T07:05:13.410Z nl=2 -->

This document is supplementary content that describes how the compilation artifacts are loaded and executed after kernel function development is complete, and how design decisions made during the development phase affect runtime behavior. From the perspective of a kernel function developer, this document outlines the basic model of runtime calls and the impact of code writing style on runtime behavior.

## 1. How Operators Are Called

### Two-Stage API

A single-operator API uses a two-stage call: `aclnnXxxGetWorkspaceSize` and `aclnnXxx`.

**Stage 1: GetWorkspaceSize**

```cpp
aclnnXxxGetWorkspaceSize(input0, input1, output, &workspaceSize, &executor);
```

This stage mainly completes pre-execution preparation on the Host side:
1. Based on information such as OpType, SoC, input/output dtype/format/shape, and attributes, it first attempts to match the aclnn parameter cache. On a cache hit, the saved binary and Tiling execution information are reused, skipping subsequent binary matching and TilingFunc calls.
2. On a cache miss, it matches the available kernel function binary based on the operator registration information and current parameters, or calls the TilingFunc registered by the operator to compute the TilingData, TilingKey, and BlockDim corresponding to the current input.
3. Based on the matched binary configuration and TilingKey, it determines the kernel function entry for this execution.
4. It returns workspaceSize (the size of Device memory to be allocated) and the executor (which carries the scheduling context).

Time-consuming operations that may be triggered in this stage:
- **TilingFunc call**: Executes a Host-side C++ function.
- **Binary lookup**: Searches for the .o file in the file system, involving file I/O.

**Phase 2: Execute**

```cpp
aclnnXxx(workspace, workspaceSize, executor, stream);
```

In this phase, all information prepared in the first phase is packaged and submitted to the stream for execution.

### Why Split into Two Phases

The purpose of the two-phase API is to separate pre-execution preparation from execution dispatch: the caller can first obtain `workspaceSize` and `executor`, then choose to allocate per-operator, use a unified memory pool, or reuse an existing workspace based on its own resource management approach, and complete the memory budget before submitting the kernel function. This reduces the overhead of frequent memory allocation and memory fragmentation.

### Binary Matching Mechanism

The runtime framework matches precompiled kernel function binaries based on information such as OpType, target chip model, input/output dtype/format, supported shape range, attributes, and TilingKey/template parameter combinations.

The priority order for binary lookup is as follows:

- The runtime first loads the operator package from the path configured by `ASCEND_CUSTOM_OPP_PATH`, and then searches for the operator package in the default CANN path.
- In the scenario where the operator package is installed by default to `$ASCEND_OPP_PATH/vendors/<vendor_name>`, the operator package priority is determined by `load_priority` in `opp/vendors/config.ini`. Operator packages listed earlier have higher priority.
- The loading method for dynamic/static library integration scenarios differs from that of the RUN package. When an application directly links a dynamic library or configures it through `ASCEND_CUSTOM_OPP_PATH`, the priority rules in [Operator Dynamic and Static Library Compilation](../compilation_and_deployment/dynamic_static_lib_compilation.md) should be followed.

## 2. How Code Writing Affects Runtime Behavior

The choices developers make when writing operator code directly affect the form of the compilation artifacts and the runtime behavior. They are classified into three categories by the phase they affect.

### Code Patterns That Affect the Compiled Artifact Form

#### Number of Branches in TilingKey / Kernel Template

An operator supports two mutually exclusive branch strategies: [numeric TilingKey](../design_and_implementation/multi_branch_strategy.md#using-tilingkey-to-select-a-branch) or [named Kernel template parameter](../design_and_implementation/multi_branch_strategy.md#using-kernel-function-templates-to-organize-branches).

| Branch Strategy | Compilation Time | Runtime Behavior | Application Scenario |
|---------|---------|---------|---------|
| Single branch (all supported inputs share the same Kernel logic) | Very fast | Simple code, but difficult to optimize for different scenarios. | Simple operators, development phase. |
| Branching by shape (for example, ≤64 / ≤1024 / ≤64K / >64K) | Moderate | Allows selecting more suitable algorithms or tile parameters for different data scales. | Most operators. |
| Fine-grained branching by shape × dtype × parameter combinations | Longer | More thorough performance specialization, but increases the number of binaries and package size. | Production deployment, performance-sensitive scenarios. |

**Development phase recommendation**: Use fewer branches or use selective compilation options to compile only the branch currently being verified, accelerating iteration. **Production deployment**: Ensure that the published binary configuration covers the dtype/format, attributes, TilingKey, or template parameter combinations that will actually be called.

#### ASCEND_SOC_SERIES/ASCEND_COMPUTE_UNIT Scope

The offline compilation artifact must contain the kernel function binary corresponding to the chip model currently running. During the development phase, if you only verify a single chip, you can narrow down `ASCEND_SOC_SERIES` or `ASCEND_COMPUTE_UNIT`; the two cannot be configured at the same time. When cross-chip deployment is required, configure multiple series or models according to the target environment. The input of `ASCEND_SOC_SERIES` is case-insensitive; the artifact directory and runtime lookup rules remain unchanged. If the offline compilation artifact does not contain the kernel function binary of the currently running chip, the runtime will fail.

### Coding Patterns That Affect Host-Side Runtime Overhead

#### Computational Complexity of TilingFunc

When TilingFunc is called, if the function contains complex computations (such as a large number of loops, dynamic memory allocation, and file system operations), it directly increases the Host-side latency of each operator call.

**Recommendation**: Perform only the necessary scalar computations in TilingFunc (shape derivation and block parameter calculation), and avoid complex logic.

#### Number of TilingData Fields

The TilingData structure is filled in and serialized on the Host side, and then passed to the kernel function side. The more fields there are, the higher the serialization overhead.

**Recommendation**: Pass only the scalar values required by the kernel function in TilingData, and do not pass redundant information.

#### Workspace Size Setting

The workspace size set by `context->GetWorkspaceSizes()` determines the amount of additional Device memory to be allocated for each call. The larger the workspace, the greater the memory pressure.

**Recommendation**: Calculate the required workspace size precisely to avoid reserving excessive redundant space.

### Writing Practices That Affect Kernel-Side Runtime Behavior

#### TILING_KEY_IS vs. Runtime if-else

```cpp
// Method 1: TilingKey branch (recommended)
if (TILING_KEY_IS(1)) {
    ProcessSmall();    // Compile-time constant folding; unmatched branches are completely removed.
} else if (TILING_KEY_IS(2)) {
    ProcessLarge();
}

// Method 2: Runtime branch
if (tilingData.totalLength <= threshold) {
    ProcessSmall();    // Runtime condition check; both branches remain in the binary.
} else {
    ProcessLarge();
}
```

`TILING_KEY_IS` is a compile-time constant check. The compiler completely removes unmatched branches (constant folding), eliminating runtime branch-checking overhead and icache pressure. With runtime if-else, both branches remain in the binary, resulting in a larger code size.

#### Kernel Function Template Parameters

The kernel function template uses named parameters (such as `D_T_X` and `TILE_NUM`) to organize template parameter combinations, and generates and sets the TilingKey on the Host side based on the actual parameters, thereby implementing compile-time specialization:

```cpp
template<typename D_T_X, typename D_T_Y, typename D_T_Z, int TILE_NUM>
__global__ __aicore__ void add_custom(...)
{
    // D_T_X/D_T_Y/D_T_Z are determined at compile time, and type checking is completed at compile time
    // TILE_NUM is determined at compile time, and loop boundaries can be directly unrolled
}
```

Template parameters enable the compiler to optimize for specific types and constants (such as loop unrolling and constant propagation). At runtime, only the compiled template combination needs to be selected, and no overhead is incurred for determining the template parameters themselves within the kernel function.

#### tileNum Split Granularity

The tileNum in TilingData determines the number of blocks on each Block:

- If tileNum is too small, each block contains a large amount of data, so a single transfer takes longer, but the number of loop iterations is small.
- If tileNum is too large, each block contains a small amount of data, so the number of loop iterations increases and the loop overhead accounts for a larger proportion.

You need to select an appropriate tileNum based on the Unified Buffer (UB) memory capacity and the compute/transfer ratio.

## Related Documents

- [Basic Process](../compilation_and_deployment/basic_process.md) — Compilation and deployment operation guide.
- [Host-Side Tiling Implementation](../design_and_implementation/host_tiling_implementation.md) — Configuration methods for Tiling functions, TilingData, and workspace.
- [Multi-Branch Strategy](../design_and_implementation/multi_branch_strategy.md) — Configuration methods for TilingKey and kernel function templates.
- [Single-Operator API Call](./single_operator_api_call.md) — Usage of the two-stage interface.
- [Runtime Selection Flow Based on Simplified Key](./simplified_key_runtime_flow.md) — Flow of generating a simplified key through callbacks and selecting a precompiled binary.
- [Operator Dynamic and Static Library Compilation](../compilation_and_deployment/dynamic_static_lib_compilation.md) — Integration methods for dynamic/static library forms.
