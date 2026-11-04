# Compilation Process Debugging

<!-- md-trans-meta sourceCommit=fbd15d5df1f452aabb0f40f2b83dd25017ebaa8f translatedAt=2026-10-23T13:39:14.296Z pushedAt=2026-11-03T07:05:13.397Z nl=2 -->

This document is an extended topic that describes the debugging methods used during the compilation of an operator project. Compiling an operator project involves four stages: code generation, host-side compilation, kernel function side compilation, and packaging and deployment. The default output is usually insufficient for locating the root cause. The following sections describe the debugging methods for each stage and provide diagnostic methods for common errors.

## Generated Code Debugging

`npu_op_code_gen` automatically generates the aclnn call code and operator prototype definition code during the cmake configure phase. The generated artifacts are located in the `ASCEND_AUTOGEN_PATH` directory, which defaults to `<CMAKE_BINARY_DIR>/autogen`. When `build_out` is used as in the example in this document, the corresponding directory is `build_out/autogen`. These files need to be checked when the prototype definition is incorrect or the generated result does not match expectations.

### Generated Artifacts Directory

Take the AddCustom project as an example. After configuration is complete, the aclnn call code, operator prototype definition code, and several compilation configuration files are generated under `build_out/autogen/`. The following lists only the key artifacts commonly used for troubleshooting code generation issues. For a multi-operator project, a corresponding `aclnn_<op>.cpp` and `aclnn_<op>.h` are generated for each operator.

```
build_out/autogen/
├── aclnn_add_custom.cpp      // aclnn call code implementation
├── aclnn_add_custom.h        // aclnn call code header file
├── op_proto.cc               // operator prototype definition code (for graph entry)
├── op_proto.h                // operator prototype definition header file
├── aic-<soc>-ops-info.ini    // operator information for the specified chip model
├── custom_compile_options.ini
├── custom_opc_options.ini
└── ...
```


### Common Troubleshooting Scenarios

| Issue | What to Check |
|------|---------|
| The aclnn API parameters do not match the prototype definition. | Check the function signature in `aclnn_<op>.h` to confirm whether the input, output, and attribute parameters are correctly mapped. |
| The operator registration information is abnormal in the graph entry scenario. | Check the registration macro in `op_proto.cc` to confirm whether OpType and dtype/format are consistent with the Host-side definitions. |
| The call code for a certain operator is not generated. | Check whether the `SRC` parameter of `npu_op_code_gen` includes the Host-side source file of that operator. |
| The chip model related to JOIN_OP_DEF is abnormal. | When `JOIN_OP_DEF=True`, the operator build phase no longer explicitly passes `ASCEND_SOC_SERIES` or `ASCEND_COMPUTE_UNIT`. You need to check the configure logs, the AddConfig in `op_proto.cc`, and the chip model in CMakePresets.json to confirm whether the target model is covered. |

### Forcing Regeneration

After modifying files, if you suspect that residual historical artifacts are causing the issue, you can manually clean them up and then reconfigure:

```bash
rm -rf build_out/autogen
cmake -S . -B build_out --preset=default
```

## Compilation Debugging on the Host Side

During Host-side building, only summary information of the compilation targets is output by default, and the actual compilation and linking commands are not displayed. The VERBOSE mechanism can be used to print the complete command lines for troubleshooting issues such as header file paths, linked libraries, and compilation options.

### Method 1: Enabling Verbose During Build

Specify verbose in the build command to print each compilation and linking command:

```bash
cmake --build build_out --target binary package --verbose -j$(nproc)
```

Output example:

```
[ 30%] Building CXX object op_host/CMakeFiles/cust_opapi.dir/__/autogen/aclnn_add_custom.cpp.o
cd /data/asc-devkit/AddCustom/build_out/op_host && /usr/bin/c++ -D_FORTIFY_SOURCE=2 -D_GLIBCXX_USE_CXX11_ABI=0 -I/usr/local/Ascend/cann-9.1.0/include -fvisibility=hidden -fPIC -fvisibility-inlines-hidden -O2 -fstack-protector-strong -MD -MT op_host/CMakeFiles/cust_opapi.dir/__/autogen/aclnn_add_custom.cpp.o -MF CMakeFiles/cust_opapi.dir/__/autogen/aclnn_add_custom.cpp.o.d -o CMakeFiles/cust_opapi.dir/__/autogen/aclnn_add_custom.cpp.o -c /data/asc-devkit/AddCustom/build_out/autogen/aclnn_add_custom.cpp
```


Each line of output contains the complete compiler invocation, from which you can see:
- The actual compiler path (for example, `/usr/bin/c++`).
- Macro definitions (`-D`) and header file search paths (`-I`).
- Compilation options (for example, `-fvisibility=hidden`, `-g`, `-O2`).
- Linking library paths (`-L`) and linking libraries (`-l`).

### Method 2: Configuring CMAKE_VERBOSE_MAKEFILE in cmake

Enable verbose during the cmake configure phase, and all subsequent make invocations will automatically print the complete commands:

```bash
cmake -S . -B build_out --preset=default -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build build_out --target binary package -j$(nproc)
```

The effect is the same as Method 1, except that the setting is persisted in CMakeCache.txt, so you do not need to specify it again for each build.

### Common Troubleshooting Scenarios

| Issue | What to Check |
|------|---------|
| Header file not found | Whether the `-I` path includes the directory where the target header file resides. |
| Link undefined reference | Whether `-L` and `-l` include the library where the missing symbol resides. |

## Compilation Debugging on the Kernel Function Side

On the kernel function side, the compilation framework invokes the Bisheng compiler for each operator and does not retain intermediate artifacts by default. The `--save-temp-files` option can be used to retain the intermediate files of each compilation, which helps troubleshoot issues in the preprocessing, compilation, and linking stages.

### Enabling the Feature

In the CMakeLists.txt on the kernel function side, add the `--save-temp-files` option through `npu_op_kernel_options`:

```cmake
# Enable for all operators.
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files)

# Enable only for a specific operator.
npu_op_kernel_options(ascendc_kernels AddCustom OPTIONS --save-temp-files)
```

You can also add `-g` to generate debugging information for combined use:

```cmake
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files -g)
```

### Retained Intermediate Artifacts

After this option is enabled, the compilation framework retains the intermediate files generated during the compilation of each kernel function in the build directory. For example, in the AddCustom project verified on ascendxxyy in this document, the temporary files are located at:

```
build_out/op_kernel/AddCustom_ascendxxyy/kernel_*/kernel_meta_<kernel_name>/kernel_meta/
```

The actual directory name varies with the OpType, chip model, and kernel function name. Common files include:

- **Preprocessed file** (`.i`): the preprocessed source code, which can be used to check whether macro expansion is correct and whether header files are properly included.
- **Object file** (`.o`): the object file before linking, which can be used to check whether symbols are correctly exported.
- **Compilation log** (`.log`): the compilation log of a single kernel function, which can be used to view compiler invocations and diagnostic information.
- **Generated source code** (`*_kernel.cpp`, `*_tiling_data.h`, etc.): the kernel function source code and Tiling data structure files generated by the compilation framework.


### Common Troubleshooting Scenarios

| Issue | What to Check |
|------|---------|
| Macro definition does not take effect | Check the `.i` file to confirm whether the macro is correctly expanded. |
| Header file inclusion is missing | Check the `.i` file to confirm whether the content of the target header file is included. |
| Link symbol is missing | Check the symbol table of the `.o` file (using the `nm` command) to confirm whether the target symbol exists. |

### Handling Compilation Failures on Some Chips

When `ASCEND_SOC_SERIES` or `ASCEND_COMPUTE_UNIT` is configured with multiple chip models, kernel function compilation runs independently for each chip. If the kernel function compilation for a chip fails, the default behavior is to terminate the entire build process.

The `ASCEND_SKIP_FAILED_COMPUTE_UNIT` option allows you to skip the compilation failure of a single chip and continue compiling the remaining chips. Applicable scenarios:

- The operator is not yet fully adapted to some chip models, but other models can already be used normally.
- When the `JOIN_OP_DEF` parameter of `npu_op_code_gen` is set to False, the user sets the chip models through `compute_unit`, and the operator lacks adaptation for some of these models.

**Configuration method**: Set the following in CMakeLists.txt:

```cmake
set(ASCEND_SKIP_FAILED_COMPUTE_UNIT TRUE)
```

Or pass it on the cmake command line:

```bash
cmake -S . -B build_out --preset=default -DASCEND_SKIP_FAILED_COMPUTE_UNIT=TRUE
```

After this option is enabled, when the operator compilation for some AI processor models fails, the compilation process can skip the failed items and continue compiling other models. When it is disabled, a failure affects the compilation process of other models. If you need to locate the skipped failed items, it is recommended to also configure [ASCENDC_BUILD_LOG_DIR](#compilation-log-persistence) to save the compilation logs.

## Packaging and Deployment Debugging

The packaging stage organizes the generated artifacts into a `.run` installation package, and the deployment stage installs the operator package to the target environment. If an issue occurs during packaging or installation, check the intermediate artifacts and the packaging command to locate the cause.

### Viewing the Packaging Command

As with Host-side debugging, the VERBOSE mechanism can be used to view the actual packaging command:

```bash
cmake --build build_out --target package --verbose -j$(nproc)
```

Example output:

```
Run CPack packaging tool...
/opt/buildtools/cmake-3.20.5-linux-aarch64/bin/cpack --config ./CPackConfig.cmake
```

The cpack invocation process is visible, which can be used to troubleshoot packaging configuration issues.

### Checking the Compilation Artifact Directory

Before packaging or installation, check the compilation artifact directory to confirm that the single-operator API header files, dynamic libraries, kernel function binaries, and Tiling libraries have been generated. The directory structure varies with the compilation mode. Only the key paths commonly used for troubleshooting are listed below.

In `TYPE RUN` mode, the intermediate artifacts in the original build directory are mainly checked:

```
build_out/
├── autogen/
│   ├── aclnn_add_custom.h
│   └── ...
├── op_host/
│   ├── libcust_opapi.so
│   ├── libcust_opmaster_rt2.0.so
│   ├── libcust_opsproto_rt2.0.so
│   └── ...
├── op_kernel/ascendc_kernels/binary/
│   ├── ascendxxyy/
│   │   └── add_custom/
│   ├── config/ascendxxyy/
│   ├── dynamic/
│   └── ...
└── ...
```

After the `package` target is executed, a `vendors/<vendor_name>/` directory close to the post-installation layout appears in the CPack staging directory. You can focus on `op_api/`, `op_impl/`, and `op_proto/`:

```
build_out/_CPack_Packages/Linux/External/custom_opp_*.run/packages/vendors/<vendor_name>/
├── op_api/
│   ├── include/aclnn_<op>.h
│   └── lib/libcust_opapi.so
├── op_impl/
│   ├── ai_core/tbe/
│   └── ...
├── op_proto/
│   ├── inc/op_proto.h
│   └── lib/linux/aarch64/libcust_opsproto_rt2.0.so
└── ...
```

In `TYPE SHARED` or `TYPE STATIC` installation mode, the artifacts are usually located in `${CMAKE_INSTALL_PREFIX}/include` and `${CMAKE_INSTALL_PREFIX}/lib`.

**Key checkpoints**:
- Check whether `aclnn_<op>.h` is generated under `build_out/autogen/` or under `op_api/include/` in the staging directory.
- Check whether `libcust_opapi.so` is generated under `build_out/op_host/` or under `op_api/lib/` in the staging directory.
- Check whether the `op_impl` directory in the staging directory or the installation directory contains the kernel function binaries and Tiling dynamic libraries for the target chip model.
- If SHARED/STATIC mode is used, `ENABLE_BINARY_PACKAGE` must be set to True.

### Post-Installation Verification

After the `.run` package is installed, you can check the deployment result in the installation directory. For the default installation scenario, check the following:

```bash
ls ${INSTALL_DIR}/opp/vendors/<vendor_name>/op_api
ls ${INSTALL_DIR}/opp/vendors/<vendor_name>/op_impl
ls ${INSTALL_DIR}/opp/vendors/<vendor_name>/op_proto # Optional
```

For the custom directory installation scenario, check the following:

```bash
ls <path>/vendors/<vendor_name>/op_api
ls <path>/vendors/<vendor_name>/op_impl
ls <path>/vendors/<vendor_name>/op_proto # Optional
```

### Common Troubleshooting Scenarios

| Issue | What to Check |
|------|---------|
| The `package` target fails. | The cpack command and error information in the output of `cmake --build build_out --target package --verbose`. |
| Build or installation fails in SHARED/STATIC mode. | Whether `ENABLE_BINARY_PACKAGE` in the `npu_op_package` CONFIG is set to True. |
| The location of the generated artifacts from INSTALL_PATH is incorrect. | Whether the `INSTALL_PATH` configuration in the `npu_op_package` CONFIG points to the expected directory. |
| The operator is not recognized after installation. | Whether `source <path>/vendors/<vendor_name>/bin/set_env.bash` was executed when installing to a specified directory, and whether the priority configuration in `opp/vendors/config.ini` is correct for default installation. |

## Compilation Log Persistence

By default, intermediate logs generated during kernel function side compilation are not persisted to disk. When compilation fails, the terminal only prints the exception information. By setting the `ASCENDC_BUILD_LOG_DIR` environment variable, you can write the logs of each compilation stage to a specified directory for later review.

**How to enable**: Set the environment variable before executing cmake or build.sh:

```bash
export ASCENDC_BUILD_LOG_DIR=/home/build_log/
```

The compilation framework creates subdirectories under it based on the chip model. For example, when compiling the `ascendxxyy` and `ascendxxyy_2` chips, the log directory structure is as follows:

```
/home/build_log/
├── ascendxxyy/
└── ascendxxyy_2/
```

**Log naming rules**: After log persistence is enabled, the compilation framework adjusts the log file suffix based on the compilation result:

- **Compilation success**: The corresponding log file suffix is appended with `_success`.
- **Compilation failure**: The terminal prints the error information and the corresponding log file path, and the log file suffix is appended with `_error`.

**Locating errors from logs**:

1. Check the specific path of the failed log in the terminal error message, or search for log files with the `_error` suffix in the corresponding chip subdirectory under `ASCENDC_BUILD_LOG_DIR`.
2. Open the failed log and view the error information output by the Bisheng compiler.
3. Based on the file name, line number, or API hint in the error, return to the source code to fix it.

> [!NOTE] Note
>
> When `ASCEND_SKIP_FAILED_COMPUTE_UNIT` is enabled, it is recommended to also enable `ASCENDC_BUILD_LOG_DIR`. Using the two together can completely preserve the detailed compilation logs of each failed kernel function, facilitating one-by-one repair.
