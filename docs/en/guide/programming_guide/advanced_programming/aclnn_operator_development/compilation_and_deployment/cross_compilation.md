# Cross Compilation

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:39:22.423Z pushedAt=2026-11-03T07:05:13.399Z nl=2 -->

This document is extended content that describes how to perform cross compilation on an operator project when the compilation platform architecture differs from the runtime platform architecture (for example, compiling on an x86_64 development machine and then deploying to an aarch64 edge device).


## Overview

After the operator code is implemented, if the compilation platform and runtime platform have the same architecture, directly compile by referring to [Basic Process](basic_process.md). If cross compilation is required (the compilation platform architecture differs from the runtime platform architecture), refer to this document.

Typical cross compilation scenarios:
- Compile on an x86_64 development machine and deploy to an aarch64 edge device.
- Compile on an aarch64 server and deploy to an x86_64 server.

## Downloading Cross Compilation Tools

Using Ubuntu series operating systems as an example, the cross compilation tool download commands are listed in the following table. For other operating systems, replace them with the actual download commands.

| Current Platform Architecture | Runtime Environment Platform Architecture | Compilation Tool Download Command |
|---|---|---|
| x86_64 | aarch64 | `sudo apt-get install -y g++-aarch64-linux-gnu` |
| aarch64 | x86_64 | `sudo apt-get install -y g++-x86-64-linux-gnu` |

After the installation is complete, you can verify that the tools are available by running the following commands:

```bash
# x86_64 → aarch64
aarch64-linux-gnu-g++ --version

# aarch64 → x86_64
x86_64-linux-gnu-g++ --version
```

## CMakePresets.json Configuration

For cross compilation, you need to modify two configuration items in the CMakePresets.json file in the operator project directory:

### Enabling Cross Compilation

Set `ENABLE_CROSS_COMPILE` to `True`:

```json
"ENABLE_CROSS_COMPILE": {
    "type": "BOOL",
    "value": "True"
}
```

### Specifying the Cross Compilation Tool Path

Use the `which` command to find the installation path of the cross compilation tool:

```bash
which aarch64-linux-gnu-g++
```

Assuming it is installed in /usr/bin/, set `CMAKE_CROSS_PLATFORM_COMPILER` to the installation path of the cross compilation tool:

```json
"CMAKE_CROSS_PLATFORM_COMPILER": {
    "type": "PATH",
    "value": "/usr/bin/aarch64-linux-gnu-g++"
}
```

## Performing Cross Compilation

When cross-compiling the kernel binary, the compilation framework must first load a Tiling dynamic library that is executable on the native architecture to obtain the compilation parameters. Therefore, after setting `ENABLE_CROSS_COMPILE=True` and `ENABLE_BINARY_PACKAGE=True`, you need to perform two steps to complete cross compilation:

- First, generate `libcust_opmaster_rt2.0.so` in native mode.
- Then, pass it to the cross-compilation stage through `HOST_NATIVE_TILING_LIB`.


The following uses AddCustom from the quick start as an example to demonstrate generating an x86_64 target operator package on an aarch64 compilation machine:

It is recommended to use two different build directories for the native Tiling library and the cross-compilation artifacts, to avoid mixing native-architecture artifacts with target-architecture artifacts:

- `build_native_tiling`: used only to generate the Tiling dynamic library loadable on the native architecture.
- `build_out`: used to generate the operator package for the target architecture.

**Step 1: Generate the Tiling library in native mode**

```bash
source /usr/local/Ascend/cann/set_env.sh

VENDOR_NAME=customize

cmake -S . -B build_native_tiling --preset=default \
    -DENABLE_CROSS_COMPILE=False \
    -DCMAKE_INSTALL_PREFIX=${PWD}/build_native_tiling

cmake --build build_native_tiling \
    --target ${VENDOR_NAME}_ascendc_cust_optiling \
    -j$(nproc)

HOST_NATIVE_TILING_LIB=$(realpath build_native_tiling/op_host/libcust_opmaster_rt2.0.so)
```

`HOST_NATIVE_TILING_LIB` points to a dynamic library of the native architecture. For example, if the current compilation machine is aarch64, this library should be an aarch64 dynamic library. You can confirm this with the following command:

```bash
file ${HOST_NATIVE_TILING_LIB}
```

**Step 2: Perform cross compilation**

Ensure that `ENABLE_CROSS_COMPILE` is set to `True` in `CMakePresets.json` and that `CMAKE_CROSS_PLATFORM_COMPILER` points to the cross compiler corresponding to the target platform, and then execute:

```bash
cmake -S . -B build_out --preset=default \
    -DENABLE_CROSS_COMPILE=True \
    -DCMAKE_CROSS_PLATFORM_COMPILER=/usr/bin/x86_64-linux-gnu-g++ \
    -DHOST_NATIVE_TILING_LIB=${HOST_NATIVE_TILING_LIB} \
    -DCMAKE_INSTALL_PREFIX=${PWD}/build_out

cmake --build build_out --target binary package -j$(nproc)
```

After successful compilation, a custom operator installation package is generated in the `build_out` directory. You can perform a basic check using the following command:

```bash
ls build_out/custom_opp_ubuntu_*.run
file build_out/op_host/libcust_opapi.so
```

For example, when cross-compiling an x86_64 target on an aarch64 compilation machine, `libcust_opapi.so` should be displayed as an `x86-64` dynamic library.

## Deploying the Cross-Compiled Operator Package

Transfer the generated operator package to the target platform and deploy it:

```bash
# 1. Transfer the run package to the target platform by yourself, and run source /usr/local/Ascend/cann/set_env.sh

# 2. Run the installation in the run package folder on the target platform
./custom_opp_ubuntu_x86_64.run 
```
After installation, set the LD_LIBRARY_PATH environment variable as prompted by the installation log. Then the operator is invoked in the same way as in normal compilation and deployment. For details, see [Operator Package Invocation](../aclnn_quick_start.md#deploying).

## Related Documents

- [Basic Process](basic_process.md) — Standard compilation and deployment process.
- [Compilation Debugging](compilation_debug.md) — Methods for troubleshooting compilation issues.
- [Single-Operator API Call](../invocation/single_operator_api_call.md) — Calling compiled and deployed operators.