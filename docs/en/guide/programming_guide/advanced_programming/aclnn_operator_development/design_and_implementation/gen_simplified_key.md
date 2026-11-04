# GenSimplifiedKey Callback Implementation

<!-- md-trans-meta sourceCommit=cd8a8acfc954f81d728ff781832dc0bf48b7e693 translatedAt=2026-10-23T13:48:10.023Z pushedAt=2026-11-03T07:05:13.402Z nl=2 -->

This document describes how to write and register the `GenSimplifiedKey` callback. This callback is used in the scenario where multiple kernel binaries are precompiled: the operator developer generates a custom simplified key fragment based on the inputs, outputs, and attributes of the current invocation, and the runtime framework uses this fragment to look up the corresponding binary.

For a complete runnable sample, see [Custom Simplified Key Compilation Sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_simplified_key).

This document only describes how the callback generates the key. For how to provide the binary corresponding to each key at compile time, see [Simplified Key Binary Configuration JSON](../compilation_and_deployment/simplified_key_json_configuration.md). For how the runtime invokes the callback and selects the binary, see [Simplified Key-Based Runtime Selection Process](../invocation/simplified_key_runtime_flow.md).

## Scenarios and Responsibility Boundaries

For a common operator, the runtime matches a binary based on the regular support information. For a scenario where an operator needs to select multiple precompiled binaries by custom dimensions, for example, selecting by input dtype, format, shape binning, or attribute combinations, you can use `GenSimplifiedKey`.

The responsibility boundaries of `GenSimplifiedKey` are as follows:

**Table 1** Responsibilities of each participant in simplified key

| Participant | Responsibility |
|---|---|
| Operator developers | Enumerates the combinations to be compiled, declares `simplified_key` for each binary in the JSON, and writes a callback to generate the same key fragment from the current invocation information. |
| Compilation toolchain | Splits and compiles each combination according to the JSON, and writes the complete simplified key and binary path into the artifact configuration. |
| Runtime framework | Invokes the callback to generate the fragment, assembles the complete simplified key, and finds and loads the matching binary. |

The callback does not load the binary, nor does it determine the TilingKey or the kernel function entry. These tasks are still completed by the runtime framework and the registered `TilingFunc`.

## Function Signature

The function type accepted by the registration API is:

```cpp
using GenSimplifiedKeyKernelFunc = UINT32 (*)(gert::TilingContext *, ge::char_t *);
```

The actual function is usually declared as:

```cpp
static ge::graphStatus GenSimplifiedKeyFunc(
    gert::TilingContext* context, ge::char_t* simplifiedKey);
```

The parameter description is as follows:

**Table 2** GenSimplifiedKey callback parameters

| Parameter | Description |
|---|---|
| `context` | Host-side context of this call. You can read the input and output descriptions, shapes, attributes, and so on to select the dimension. |
| `simplifiedKey` | Output buffer provided by the runtime. The callback writes the custom key fragment to this buffer. |
| Return Value | Returns `ge::GRAPH_SUCCESS` on success, or a failure code when a valid fragment cannot be generated. |

The UTF-8 encoded length of the custom key fragment must be less than 256 bytes, and it must not contain the complete key prefix such as the operator type, `d=`, or `p=`. The UTF-8 encoded length of the complete simplified key must be less than 1024 bytes.

## Writing the Callback

The following example is from `AddCustomTemplate`. This sample supports two combinations, FP16 and FP32. The two inputs and one output use the same dtype, so reading only the dtype of input `x` is sufficient to distinguish the binary.

```cpp
static ge::graphStatus GenSimplifiedKeyFunc(
    gert::TilingContext* context, ge::char_t* simplifiedKey)
{
    constexpr size_t CUSTOM_SIMPLIFIED_KEY_MAX_LEN = 256U;
    if (context == nullptr || simplifiedKey == nullptr || context->GetInputDesc(0) == nullptr) {
        return ge::GRAPH_FAILED;
    }

    const char* key = nullptr;
    switch (context->GetInputDesc(0)->GetDataType()) {
        case ge::DT_FLOAT16:
            key = "custom_fp16";
            break;
        case ge::DT_FLOAT:
            key = "custom_fp32";
            break;
        default:
            return ge::GRAPH_FAILED;
    }
    return strcpy_s(simplifiedKey, CUSTOM_SIMPLIFIED_KEY_MAX_LEN, key) == EOK
        ? ge::GRAPH_SUCCESS : ge::GRAPH_FAILED;
}
```

The correspondence between this function and the JSON is as follows:

**Table 3** Correspondence between the AddCustomTemplate callback and the binary

| dtype of runtime input `x` | Fragment output by the callback | `simplified_key` in the JSON | Selected binary |
|---|---|---|---|
| `ge::DT_FLOAT16` | `custom_fp16` | `custom_fp16` | `AddCustomTemplate_FP16.o` |
| `ge::DT_FLOAT` | `custom_fp32` | `custom_fp32` | `AddCustomTemplate_FP32.o` |

For operators with multiple inputs, multiple outputs, or attributes, any dimension that changes the binary selection result must appear on both ends: the JSON must enumerate the combinations, and the callback must read the corresponding `context` information and generate a unique fragment. For example, when the binary depends on the dtypes of two inputs, read `context->GetInputDesc(0)` and `context->GetInputDesc(1)` separately; do not read only the first input and assume that the remaining inputs are the same.

## Registering the Callback

Register the callback in the `OpDef` constructor of the operator prototype, usually together with the Tiling function:

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc)
    .SetGenSimplifiedKey(optiling::GenSimplifiedKeyFunc)
    .AddConfig("ascendxxx");
```

When `OP_ADD` registers the operator prototype, this function is written into the operator implementation registration information. After the Host-side operator implementation is loaded at runtime, the callback can be obtained and invoked during the call process.

## Writing Requirements

- A Key fragment should consist only of stable information that affects binary selection. Do not use memory addresses, random numbers, timestamps, or content that changes under the same input conditions.
- The same call conditions must always generate the same fragment; different binary combinations must generate different fragments.
- The fragment returned by the callback must be byte-for-byte identical to the `simplified_key` in the JSON, including case.
- First check whether `context`, the output buffer, and the input and output descriptors to be accessed are null. For combinations that cannot be covered, return a failure code instead of writing an uncertain default Key.
- The callback should only perform lightweight descriptor reading and string writing. It should not perform file I/O, dynamic compilation, complex computation, or device-side operations.

## Related Documents

- [Operator Prototype Definition](./operator_prototype_definition.md): Learn where `SetGenSimplifiedKey` is registered in the prototype.
- [Simplified Key Binary Configuration JSON](../compilation_and_deployment/simplified_key_json_configuration.md): Learn the correspondence between JSON and compilation artifacts.
- [Runtime Selection Process Based on Simplified Key](../invocation/simplified_key_runtime_flow.md): Learn how the runtime uses the callback result to select a binary.
