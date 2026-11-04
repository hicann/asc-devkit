# Simplified Key Binary Configuration JSON

<!-- md-trans-meta sourceCommit=cd8a8acfc954f81d728ff781832dc0bf48b7e693 translatedAt=2026-10-23T13:46:38.470Z pushedAt=2026-11-03T07:05:13.402Z nl=2 -->

This document describes how to provide the kernel (Kernel) binary configuration JSON in a custom operator project, and how this JSON drives the compilation of multiple binaries based on a custom simplified key. This mechanism applies to scenarios where one OpType needs to precompile multiple binaries by dtype, format, shape binning, or attribute combination, and the runtime selects among them based on the `GenSimplifiedKey` callback.

For details about how to implement and register the callback, see [GenSimplifiedKey Callback Implementation](../design_and_implementation/gen_simplified_key.md). For details about the runtime selection process, see [Runtime Selection Process Based on simplified key](../invocation/simplified_key_runtime_flow.md).

For a complete runnable sample, see [Custom simplified key Compilation Sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_simplified_key).

## Preparing the Configuration Directory and Files

When `--kernel-json-file` is used, the Binary JSON is an input file provided by the user. `op_host/config/<soc>` is only a recommended storage location, and CMake does not automatically create this directory or file.

- The repository sample already provides the corresponding directory and JSON, so no creation is needed for direct compilation.
- For a custom project, you need to prepare the Binary JSON yourself and point `--kernel-json-file` to the actual path. The directory name and file name can be adjusted according to the project.
- `build/.../binary/config/<soc>` is the output directory generated during compilation and does not need to be created in advance.

## How to Provide the JSON

Place the JSON in the existing Host-side configuration directory of the operator project, and organize it by AI processor model. For example, the `AddCustomTemplate` sample uses:

```text
op_host/config/ascendxxx/add_custom_template_binary.json
```

In `op_kernel/CMakeLists.txt`, pass `--kernel-json-file` through `npu_op_kernel_options()`. This parameter is a compilation option and is not written into `npu_op_kernel_sources()`:

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustomTemplate
    KERNEL_DIR add_custom_template
    KERNEL_FILE add_custom_template.cpp
)

npu_op_kernel_options(ascendc_kernels AddCustomTemplate
    OPTIONS
        --kernel-json-file=${CMAKE_CURRENT_SOURCE_DIR}/../op_host/config/ascendxxx/add_custom_template_binary.json
)
```

During the build configuration phase, this option is recorded into the automatically generated `custom_opc_options.ini`, from which the subsequent binary compilation reads it. For example:

```text
AddCustomTemplate@@--kernel-json-file=<project path>/op_host/config/ascendxxx/add_custom_template_binary.json
```

## JSON Format

The outermost level of the JSON file contains two fields: `op_type` and `op_list`. `op_type` specifies the operator type, and each element in `op_list` describes one binary to be compiled. The following is a complete example of `AddCustomTemplate`, which generates two binaries for FP16 and FP32:

```json
{
  "op_type": "AddCustomTemplate",
  "op_list": [
    {
      "bin_filename": "AddCustomTemplate_FP16",
      "simplified_key": "custom_fp16",
      "inputs": [
        {
          "name": "x",
          "index": 0,
          "dtype": "float16",
          "format": "ND",
          "shape": [-2]
        },
        {
          "name": "y",
          "index": 1,
          "dtype": "float16",
          "format": "ND",
          "shape": [-2]
        }
      ],
      "outputs": [
        {
          "name": "z",
          "index": 0,
          "dtype": "float16",
          "format": "ND",
          "shape": [-2]
        }
      ]
    },
    {
      "bin_filename": "AddCustomTemplate_FP32",
      "simplified_key": "custom_fp32",
      "inputs": [
        {
          "name": "x",
          "index": 0,
          "dtype": "float32",
          "format": "ND",
          "shape": [-2]
        },
        {
          "name": "y",
          "index": 1,
          "dtype": "float32",
          "format": "ND",
          "shape": [-2]
        }
      ],
      "outputs": [
        {
          "name": "z",
          "index": 0,
          "dtype": "float32",
          "format": "ND",
          "shape": [-2]
        }
      ]
    }
  ]
}
```

The fields are described as follows:

**Table 1**  Field description of the simplified key binary configuration JSON

| Field | Description |
|---|---|
| `op_type` | Mandatory. It must be consistent with `OP_TYPE` in `npu_op_kernel_sources()`. |
| `op_list` | Mandatory non-empty array. Each item in the array corresponds to an independently compiled binary. |
| `bin_filename` | Mandatory, without a directory. It is used to generate the `.o` file name and the matching `.json` file name. After removing the extension, it must be equal to `${op_type}` or start with `${op_type}_`, where `${op_type}` is the operator name in upper camel case. File names must not be duplicated. |
| `simplified_key` | Mandatory non-empty string. It is the custom fragment returned by the `GenSimplifiedKey` callback and must be byte-for-byte consistent with the callback. The length of the string after UTF-8 encoding must be less than 256 bytes. |
| `inputs`, `outputs` | Each input and output contains `name`, `index`, `dtype`, `format`, and `shape`, describing the combination supported by this binary. |

`name` and `index` must be consistent with the input and output order defined in the operator prototype. `dtype` and `format` must fall within the supported range declared by the prototype. `shape` should express the shape range covered by this binary. JSON parsing checks the basic format, including the outermost structure, `op_type`, non-empty `op_list`, `bin_filename`, and `simplified_key`. The subsequent `asc_opc` compilation also processes specific combinations based on the input and output descriptions. Therefore, the combinations in the JSON must be consistent with the prototype definition, the Kernel implementation, and the callback branches.

## Impact on the Compilation Process

After the JSON is configured, the compilation pipeline works in the following order:

1. CMake writes `--kernel-json-file` into `custom_opc_options.ini`.
2. The binary build reads the JSON and splits it into single-combination JSONs according to `op_list`; each item uses its own `bin_filename` as the output name.
3. For each single-combination JSON, `asc_opc` is invoked to compile the Kernel source code, producing the corresponding `.o` and the `.json` that describes that `.o`.
4. When a non-empty `simplified_key` exists in the JSON, `asc_opc` uses custom simplified key mode 2 and combines the operator type, the runtime common dimensions, and this fragment into a complete simplified key.
5. In the aggregation phase, the operator configuration JSON and `binary_info_config.json` are generated, recording the `.o` and `.json` paths associated with each complete Key.

Taking the FP16 item as an example, the main artifacts are as follows:

```text
binary/ascendxxx/add_custom_template/AddCustomTemplate_FP16.o
binary/ascendxxx/add_custom_template/AddCustomTemplate_FP16.json
binary/config/ascendxxx/add_custom_template.json
binary/config/ascendxxx/binary_info_config.json
```

Among them, `AddCustomTemplate_FP16.json` records the Kernel metadata of a single `.o`; `add_custom_template.json` aggregates the support information of each combination; `binary_info_config.json` provides the runtime with a lookup table from the complete simplified key to the binary path. An example is as follows:

```json
{
  "AddCustomTemplate": {
    "simplifiedKeyMode": 2,
    "binaryList": [
      {
        "simplifiedKey": ["AddCustomTemplate/d=0,p=1/custom_fp16"],
        "binPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP16.o",
        "jsonPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP16.json"
      },
      {
        "simplifiedKey": ["AddCustomTemplate/d=0,p=1/custom_fp32"],
        "binPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP32.o",
        "jsonPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP32.json"
      }
    ]
  }
}
```

The actual artifacts may write multiple complete Keys for determinism and implementation modes; they share the same custom fragment and binary path. Operator developers should not manually concatenate prefixes such as `AddCustomTemplate/d=0,p=1/` in the callback.

## Relationship with Standard Compilation

When `--kernel-json-file` is not configured and `GenSimplifiedKey` is not registered, the operator continues to use the original compilation and binary matching process. Only when both conditions are met—the JSON provides a non-empty `simplified_key` for each combination, and the Host side registers the corresponding callback—does the runtime have the information required on both ends for custom simplified key selection.

## Related Documents

- [Basic Compilation and Deployment Process](./basic_process.md): Learn the general project build, packaging, and deployment process.
- [GenSimplifiedKey Callback Implementation](../design_and_implementation/gen_simplified_key.md): Learn how the callback return value corresponds to the `simplified_key` in the JSON.
- [Runtime Selection Process Based on simplified key](../invocation/simplified_key_runtime_flow.md): Learn how the runtime uses `binary_info_config.json` to find a binary.
