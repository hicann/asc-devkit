# Operator Dynamic Library and Static Library Compilation

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T13:43:11.018Z pushedAt=2026-11-03T07:05:13.400Z nl=2 -->

This document is an extension that describes how to compile a custom operator project into a dynamic library or a static library, and how to use these libraries in applications or graph mode scenarios. For regular deployment scenarios, preferentially refer to [Basic Compilation and Deployment Process](./basic_process.md) to generate the `custom_opp_*.run` operator package. Consider compiling a dynamic library or a static library only when the application needs to directly link the operator library, or when you want to integrate the operator library into other projects. Unlike the `.run` operator package, which is pre-installed in the OPP directory, the dynamic library or static library approach works as follows: after process startup, the runtime creates a temporary directory, restores the operator package structure to that temporary directory, and loads operator-related files from it. The temporary directory is deleted before the process exits.

## Operator Dynamic Library and Static Library

Compiling an operator dynamic library and static library is the process of compiling and linking the operator kernel function implementation, the host-side Tiling implementation, the graph integration adaptation files, and the automatically generated single operator call code into a library file. After compilation, the single-operator call aclnn header file and the operator prototype definition header file are generated at the same time, which are used for single operator call and operator graph integration scenarios, respectively.

Compared with the `.run` operator package, a dynamic library or static library is more suitable for being directly linked by an application project; the `.run` operator package is more suitable for being installed to the `vendors/<vendor_name>` path of OPP for use by the runtime environment. The typical usage of different artifact forms is as follows.

| Artifact Form | CMake Configuration | Main Artifact | Typical Usage |
|----------|------------|----------|--------------|
| Operator package | `TYPE RUN` | `custom_opp_*.run` | Installed to the `vendors/<vendor_name>` path of OPP for use by the runtime environment. |
| Dynamic library | `TYPE SHARED` | `libcust_opapi.so` | Dynamically linked in application or graph mode processes. |
| Static library | `TYPE STATIC` | `lib${vendor_name}.a` | Statically linked by an application project, and operators are executed through the aclnn method. |

Compiling an operator dynamic library and static library supports the following products:
<!-- npu="950" id1 -->
- Ascend 950PR&950DT products
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products
<!-- end id3 -->

Before you start, complete the following preparations:

- The operator prototype definition, host-side Tiling implementation, and kernel function implementation have been completed;
- The project uses `find_package(ASC)` and `npu_op_package` to organize compilation;
- The CANN environment variables have been configured, for example, by running `source ${install_path}/cann/set_env.sh`.
- Parameters such as `vendor_name`, `ASCEND_COMPUTE_UNIT`, and `CMAKE_INSTALL_PREFIX` have been configured in `CMakePresets.json` or in the CMake command.

## Compiling a Dynamic Library

When compiling a dynamic library, set `TYPE` of `npu_op_package` to `SHARED`. Using AddCustom in [Quick Start](../aclnn_quick_start.md) as an example, modify the `npu_op_package` configuration in the top-level CMakeLists.txt as follows:

```cmake
set(package_name ${vendor_name})
npu_op_package(${package_name}
    TYPE SHARED
    CONFIG
        INSTALL_PATH ${CMAKE_INSTALL_PREFIX}
)
```

Compile and install:

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary install -j$(nproc)
```

> [!NOTE] Note
>
> In the `TYPE RUN` scenario, the `package` target is usually executed to generate `custom_opp_*.run`, which is then run to install the package to the target OPP path. In the `TYPE SHARED` and `TYPE STATIC` scenarios, the `install` target is usually executed to directly install the header files, library files, and CMake package configuration to `${CMAKE_INSTALL_PREFIX}`, so that application projects can integrate them through `find_package` or direct linking.

After successful compilation, the main artifacts are located in `${CMAKE_INSTALL_PREFIX}`:

```text
├── include
│   ├── aclnn_*.h       // aclnn header file for single operator call
│   └── op_proto.h      // operator prototype definition header file
└── lib
    ├── cmake
    │   └── ${vendor_name}
    │       ├── ${vendor_name}-config.cmake  // installation file that defines the configuration options and variables in the project
    │       └── ${vendor_name}-targets.cmake // installation file that defines the build target ${vendor_name}::${vendor_name} of the dynamic library, making it easier for developers to use the dynamic library and automatically handle library dependencies
    └── libcust_opapi.so    // dynamic library libcust_opapi.so, used for dynamic linking
```

## Compiling a Static Library

When compiling a static library, set `TYPE` of `npu_op_package` to `STATIC`.

```cmake
set(package_name ${vendor_name})
npu_op_package(${package_name}
    TYPE STATIC
    CONFIG
        INSTALL_PATH ${CMAKE_INSTALL_PREFIX}
)
```

Run the compilation and installation:

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary install -j$(nproc)
```

After the compilation succeeds, the main artifacts are located in `${CMAKE_INSTALL_PREFIX}`:

```text
├── include
│   ├── aclnn_*.h   // aclnn header file for single operator call
│   └── op_proto.h  // operator prototype definition header file
└── lib
    ├── cmake
    │   └── ${vendor_name}
    │       ├── ${vendor_name}-config.cmake  // installation file that defines the configuration options and variables in the project
    │       └── ${vendor_name}-targets.cmake // installation file that defines the build target ${vendor_name}::${vendor_name} of the static library, making it easier for developers to use the static library and automatically handling library dependencies
    └── lib${vendor_name}.a // operator static library
```

> [!NOTE] Note
>
> `TYPE SHARED` and `TYPE STATIC` are two artifact forms. The libraries actually generated depend on the `TYPE` configuration of this `npu_op_package` and the artifacts in the installation directory.

## Single-Operator Call Scenario Integration

Before calling the dynamic library or static library, add the custom operator package path to the beginning of the `LD_LIBRARY_PATH` environment variable so that the system dynamic linker can locate the operator dynamic library. Run the following command:

```bash
# Replace ${CMAKE_INSTALL_PREFIX} with the absolute path of the custom operator package
export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib/:${LD_LIBRARY_PATH}
```

After that, the caller's application project can import the operator library through `find_package` and then link it to its own executable file or service library. Taking the AddCustom call in [Quick Start](../aclnn_quick_start.md#calling-the-operator) as an example, add the `${vendor_name}` library search to CMakeLists.txt and link it. The example is as follows:

### Dynamic Library Integration

```cmake
find_package(${vendor_name} REQUIRED    # ${vendor_name} is the name of the operator package generated by compilation. Replace it with the actual operator package name.
    PATHS ${CUST_PKG_PATH}  # Replace ${CUST_PKG_PATH} with the storage path of the operator package generated by compilation.
    NO_DEFAULT_PATH
)

target_link_libraries(op_runner PRIVATE # Replace op_runner with the caller's target name.
    nnopbase
    acl_rt
    ${vendor_name}::${vendor_name}  # The related target dependencies are automatically included. Replace ${vendor_name} with the actual operator package name.
)
```

Here, \$\{CUST_PKG_PATH} is \$\{CMAKE_INSTALL_PREFIX}/lib/cmake/\$\{vendor_name}. The target name imported by `find_package(${vendor_name} ...)` is `${vendor_name}::${vendor_name}`, which points to `libcust_opapi.so` in `TYPE SHARED` mode.

### Static Library Integration

```cmake
find_package(${vendor_name} REQUIRED    # ${vendor_name} is the name of the operator package generated by compilation. Replace it with the actual operator package name.
    PATHS ${CUST_PKG_PATH}  # ${CUST_PKG_PATH} needs to be replaced by the user with the storage path of the compiled operator package.
    NO_DEFAULT_PATH
)

target_link_libraries(op_runner PRIVATE # op_runner needs to be replaced with the target name of the caller
    nnopbase
    rt2_registry
    acl_rt
    ${vendor_name}::${vendor_name}  # The related target dependencies are automatically included. Replace ${vendor_name} with the actual operator package name.
)
```
`${vendor_name}::${vendor_name}` points to `lib${vendor_name}.a` in `TYPE STATIC` mode.
For a complete example of static library integration, see [custom_op_static_lib](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib).

### Mixed Use of Static and Dynamic Libraries

If an application integrates multiple operator libraries at the same time, you can specify the package search path for each library separately:

```cmake
find_package(${vendor_name1} REQUIRED     # ${vendor_name1} is the name of the operator package generated by compilation
    PATHS ${CUST_PKG_PATH_1}              # ${CUST_PKG_PATH_1} is the storage path of the operator package generated by compilation
    NO_DEFAULT_PATH
)
find_package(${vendor_name2} REQUIRED     # ${vendor_name2} is the name of the operator package generated by compilation
    PATHS ${CUST_PKG_PATH_2}              # ${CUST_PKG_PATH_2} is the storage path of the operator package generated by compilation
    NO_DEFAULT_PATH
)

target_link_libraries(op_runner PRIVATE
    ${vendor_name1}::${vendor_name1}    # The related static library target dependencies are automatically included
    ${vendor_name2}::${vendor_name2}    # The related static library target dependencies are automatically included
)
```

## Configuring Operator Graph Integration

In the operator graph integration scenario, you need to configure the ASCEND\_CUSTOM\_OPP\_PATH environment variable and add the directory where the dynamic library libcust_opapi.so resides to the beginning of this environment variable. The GE framework searches for and uses the operator dynamic library based on this environment variable during graph compilation and execution. The earlier the path appears in the environment variable, the higher the priority of the operator.

```bash
export ASCEND_CUSTOM_OPP_PATH=${CMAKE_INSTALL_PREFIX}/lib/:${ASCEND_CUSTOM_OPP_PATH}
```

When dynamic library compilation and [operator package compilation](basic_process.md) are used together, the dynamic library generated by the former has a higher priority.
In the following example, path1 and path3 are the specified installation directories of the operator package, and path2 and path4 are the `CMAKE_INSTALL_PREFIX` values used during dynamic library compilation. The priority of the compilation artifacts is 2>4>1>3.

```bash
ASCEND_CUSTOM_OPP_PATH=<path1>/vendors/vendor_name1:<path2>/lib/:<path3>/vendors/vendor_name3:<path4>/lib/
```

If developers perform graph development through Ascend Graph, in addition to configuring the environment variable, they can also directly link the dynamic library libcust_opapi.so in the compilation file of the application. For details about Ascend Graph graph development, see [Graph Development](https://gitcode.com/cann/ge/blob/9.2.0/docs/en/user_guides/graph_dev/README.md). The so loading priority of the dynamic library linking method is higher than that of the environment variable configuration method.

## Handling Multiple Vendors and Same-Name Dynamic Libraries

The dynamic libraries generated by multiple operator projects are named `libcust_opapi.so` by default. When operator libraries from multiple vendors are integrated into one project, if they are only located and linked through different installation paths, you can distinguish them by using different `CUST_PKG_PATH` values.

```cmake
# Specify the operator library paths of different vendors separately
find_package(${vendor_name1} REQUIRED
    PATHS ${CUST_PKG_PATH_1}              # Point to the operator library installation path of vendor 1
    NO_DEFAULT_PATH
)
find_package(${vendor_name2} REQUIRED
    PATHS ${CUST_PKG_PATH_2}              # Point to the operator library installation path of vendor 2
    NO_DEFAULT_PATH
)

target_link_libraries(op_runner PRIVATE
    ...
    ${vendor_name1}::${vendor_name1}
    ${vendor_name2}::${vendor_name2}
    ...
)
```

If you need to integrate multiple dynamic libraries into the same common dynamic library, or avoid file name conflicts in the same application, you can set different output names for the dynamic libraries in the top-level CMakeLists.txt before compilation, and then compile and install the dynamic libraries. Add set_target_properties after npu_op_package to modify them:

```cmake
npu_op_package(${package_name}
    TYPE SHARED
    CONFIG
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)

set_target_properties(${package_name} PROPERTIES
    OUTPUT_NAME xxxxxxx
)
```

## Legacy Project Template Description

Some legacy project templates control the compilation of dynamic and static libraries through `ASCEND_PACK_SHARED_LIBRARY=True`:

```json
"ASCEND_PACK_SHARED_LIBRARY": {
    "type": "BOOL",
    "value": "True"
}
```

If the project already uses `npu_op_package(... TYPE RUN/SHARED/STATIC ...)`, the artifact form is determined by the `TYPE` parameter and is no longer controlled by `ASCEND_PACK_SHARED_LIBRARY`.

## Related Documents

- [Basic Compilation and Deployment Process](./basic_process.md) — Basic compilation configuration and usage of `npu_op_package`.
- [Single-Operator API Call](../invocation/single_operator_api_call.md) — Call operators using the generated aclnn APIs.
