# Runtime Selection Process Based on the Simplified Key

<!-- md-trans-meta sourceCommit=cd8a8acfc954f81d728ff781832dc0bf48b7e693 translatedAt=2026-10-23T14:02:26.607Z pushedAt=2026-11-03T07:05:13.411Z nl=2 -->

This document describes the runtime process after `GenSimplifiedKey` is configured. This mechanism is used to select, from multiple precompiled kernel function binaries of the same operator, the one that matches the conditions of this call. The operator developer is responsible for providing the Callback and the compilation configuration JSON; the runtime framework is responsible for invoking the Callback, assembling the complete simplified key, and searching for and selecting the binary.

For a complete runnable example, see [Custom Simplified Key Compilation Example](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_simplified_key).

For the implementation of the Callback, see [GenSimplifiedKey Callback Implementation](../design_and_implementation/gen_simplified_key.md). For the JSON provisioning and compilation artifacts, see [Simplified Key Binary Configuration JSON](../compilation_and_deployment/simplified_key_json_configuration.md).

## Runtime Process

During the `aclnnXxxGetWorkspaceSize` phase, the runtime processes in the following order:

1. Load the Host-side operator implementation and `binary_info_config.json` from the operator package, and obtain the `binaryList` and `GenSimplifiedKey` callback of the OpType.
2. Create or reuse a `TilingContext` based on the inputs, outputs, attributes, and target AI processor of this call.
3. Invoke the `GenSimplifiedKey` callback registered by the operator. The callback reads the `TilingContext` and writes a custom simplified key fragment, for example, `custom_fp16`.
4. The runtime framework fills in the common dimensions such as operator type, determinism, and implementation mode, and generates the complete simplified key, for example, `AddCustomTemplate/d=0,p=1/custom_fp16`.
5. The runtime uses the complete key to search the `binaryList` in `binary_info_config.json`, obtains the matching `.o` and the accompanying `.json` paths, and loads the corresponding binary.
6. The runtime invokes the registered `TilingFunc` to compute the TilingData, workspace, BlockDim, and TilingKey; it then creates the executor and returns it to the caller.
7. During the `aclnnXxx` execution phase, the runtime uses the selected binary and the tiling results prepared in the previous phase to dispatch the kernel function.

Simplified key selection occurs in the binary matching phase before tiling. `GenSimplifiedKey` does not replace `TilingFunc`: the former is used to select which precompiled binary to use, while the latter is used to compute the runtime parameters required for executing that binary in this call.

## AddCustomTemplate Example

`AddCustomTemplate` precompiles two binaries, one for FP16 and one for FP32. Its callback generates a fragment based on the dtype of the input `x`, and the runtime completes the subsequent selection:

**Table 1** AddCustomTemplate runtime selection example

| This Call | Callback Result | Complete Runtime Key Example | Selected Binary |
|---|---|---|---|
| `x` is FP16, and `y` and `z` are FP16 | `custom_fp16` | `AddCustomTemplate/d=0,p=1/custom_fp16` | `AddCustomTemplate_FP16.o` |
| `x` is FP32, and `y` and `z` are FP32 | `custom_fp32` | `AddCustomTemplate/d=0,p=1/custom_fp32` | `AddCustomTemplate_FP32.o` |

Here, `d=...` and `p=...` are handled by the runtime according to the common call conditions; the operator developer only maintains the final `custom_fp16` or `custom_fp32` fragment. In actual configurations, the same binary can correspond to multiple complete keys to cover different determinism or implementation mode combinations.

## Correspondence That Developers Must Ensure

The runtime can select the correct binary only if both sides use the same set of rules:

**Table 2** Correspondence between the compile side and the runtime callback side

| Compile Side | Runtime Callback Side |
|---|---|
| The JSON declares a unique `bin_filename` and `simplified_key` for each supported combination. | `GenSimplifiedKey` generates the same `simplified_key` for the same call combination. |
| The input/output dtype, format, shape, and attribute combinations are consistent with the prototype and the kernel function implementation. | Read all inputs, outputs, shapes, and attributes that affect binary selection, without omitting any combination dimension. |
| The compilation artifacts write the complete Key, the `.o` path, and the `.json` path into `binary_info_config.json`. | Do not concatenate the complete Key prefix, do not load files, and do not select a binary. |

For example, if a binary is determined by the dtypes of two inputs, the callback must read both inputs. Generating the Key based only on the first input would cause different combinations to fall into the same fragment, leading to incorrect lookup or failure to find the binary.

## Failure Handling

The following situations cause this call to fail to match a precompiled binary through the custom simplified key:

- The `GenSimplifiedKey` callback is not registered, or the callback is empty.
- The callback returns a failure, for example, the input description is empty or an unsupported dtype is encountered.
- The fragment generated by the callback is empty, too long, or inconsistent with the `simplified_key` in the JSON.
- The JSON does not compile this call combination, or the corresponding `.o`, `.json`, and `binary_info_config.json` are missing during deployment.

Therefore, when adding a binary combination, you must complete three tasks simultaneously: add a JSON entry, update the callback branch, and recompile and deploy the operator package. Changing only one of them breaks the consistency between the compilation artifacts and the runtime selection rules.

## Relationship with Two-Stage Call

simplified key selection is a preparatory step in the `aclnnXxxGetWorkspaceSize` stage. When the parameter cache is hit, the runtime can reuse the saved binary and Tiling information. Only when the cache is missed does the runtime execute the callbacks, lookup, and Tiling process described above. The `aclnnXxx` execution stage does not regenerate the key; instead, it uses the binary and Tiling results already prepared in the executor.

## Related Documents

- [Runtime Loading Mechanism](./runtime_loading_mechanism.md): Learn about the two-phase call of a single operator, caching, and general binary matching behavior.
- [GenSimplifiedKey Callback Implementation](../design_and_implementation/gen_simplified_key.md): Learn about the callback signature, registration method, and writing requirements.
- [Simplified Key Binary Configuration JSON](../compilation_and_deployment/simplified_key_json_configuration.md): Learn how the complete key and binary path are written into the compilation artifacts.
