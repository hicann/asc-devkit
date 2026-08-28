# Custom simplified key compilation sample

## Overview

This sample uses `AddCustomTemplate` to demonstrate how a Host callback generates a custom `simplifiedKey`, and how `BINARY_JSON` selects different Kernel Binaries for different input data types.

This is an independent custom operator project. It does not change the compilation behavior of the existing `custom_op` sample.

## Directory structure

```text
├── custom_op_simplified_key
│   ├── CMakeLists.txt
│   ├── op_host
│   │   ├── CMakeLists.txt
│   │   ├── add_custom_template
│   │   └── config
│   │       └── ascend910b
│   │           └── add_custom_template_binary.json
│   ├── op_kernel
│   │   ├── CMakeLists.txt
│   │   └── add_custom_template
│   └── README_en.md
```

## Sample description

`AddCustomTemplate` computes `z = x + y` and supports `float16` and `float32` inputs and outputs.

The Host-side `GenSimplifiedKeyFunc` returns a key according to the first input `dtype`:

| Input dtype | simplified key | Kernel Binary |
| --- | --- | --- |
| `float16` | `custom_fp16` | `AddCustomTemplate_FP16.o` |
| `float32` | `custom_fp32` | `AddCustomTemplate_FP32.o` |

`op_host/config/ascend910b/add_custom_template_binary.json` describes the two Kernel Binaries and their input/output `dtype`, `format`, and `shape`. The Kernel compilation option passes this file through `--kernel-json-file`.

## Build

Configure the CANN environment:

```bash
source ${install_path}/cann/set_env.sh
```

Run the following commands in the sample root directory:

```bash
rm -rf build && mkdir build && cd build
cmake ..
make AddCustomTemplate_ascend910b
```

To build the complete binary and installation package, run:

```bash
make -j binary package
```

## Build result verification

After compilation, the `build` directory should contain:

- `AddCustomTemplate_FP16.o` and `AddCustomTemplate_FP32.o`;
- operator JSON files containing `simplifiedKey` and `simplifiedKeyMode`;
- `binary_info_config.json`, which summarizes the Kernel Binary lookup information.

Use the following command to inspect the generated files:

```bash
find . -name 'AddCustomTemplate_*.o' -o -name 'binary_info_config.json'
```
