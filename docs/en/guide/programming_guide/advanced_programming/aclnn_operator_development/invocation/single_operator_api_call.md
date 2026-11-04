# Single-Operator API Call

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T14:02:48.166Z pushedAt=2026-11-03T07:05:13.413Z nl=2 -->

This document covers the basics and describes single-operator API invocation, that is, the process of performing heterogeneous computing by calling aclnn APIs through C/C++. The aclnn APIs and their implementation are delivered with the custom operator package, so you must deploy the custom operator package before invoking them. A usable NPU environment is required at runtime. The caller is responsible for allocating and releasing the resources used during computation, such as input and output tensors, device memory, workspace, and streams.

## Pre-call Checks

Before calling a single-operator API, ensure that the following conditions are met:

-   The runtime environment for operator invocation has an NPU device.
-   The custom operator project has been created.
-   The operator prototype definition, kernel-side implementation, and host-side Tiling implementation are complete.
-   For the operator package compilation scenario, see [Operator Project Compilation and Deployment](../compilation_and_deployment/basic_process.md) to compile and deploy the operator package. The operator binary compilation switch `ENABLE_BINARY_PACKAGE` is enabled by default. If it is explicitly set to `False` in the project, change it to `True` and recompile and redeploy.

    After the operator is compiled and deployed, the header file `aclnn_<op>.h` and the dynamic library `libcust_opapi.so` for single-operator calls are generated in the `op_api` directory under the operator package installation directory.

    Taking the default installation scenario as an example, the directory structure where the header file and dynamic library for single-operator calls are located is as follows:

    ```
    ├── opp    // Operator library directory
    │   ├── vendors     // Directory where custom operators are located
    │       ├── config.ini
    │       └── vendor_name   // Stores the custom operator deployed by the corresponding vendor. This name is the vendor_name configured when compiling the custom operator installation package. If not configured, the default value is customize
    │           ├── op_api
    │           │   ├── include
    │           │   │  └── aclnn_xx.h
    │           │   └── lib
    │           │       └── libcust_opapi.so
    ...
    ```
-   For the operator dynamic library or static library compilation scenario, see [Operator Dynamic Library and Static Library Compilation](../compilation_and_deployment/dynamic_static_lib_compilation.md) to complete the compilation and installation of the operator. `CMAKE_INSTALL_PREFIX` is the installation path configured by the developer in the cmake file.
    -   Dynamic library path: `${CMAKE_INSTALL_PREFIX}/lib/libcust_opapi.so`
    -   Static library path: `${CMAKE_INSTALL_PREFIX}/lib/lib${vendor_name}.a`
    -   Header file path: `${CMAKE_INSTALL_PREFIX}/include`

## Basic Principles

After the custom operator is compiled, the operator project automatically generates the single operator API header file `aclnn_<op>.h` based on the operator prototype definition, and compiles and generates the single operator API dynamic library `libcust_opapi.so`. After importing the header file and linking the dynamic library during invocation, you can directly call the custom operator.

A single operator API is generally a "two-stage API":

```c
aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
aclnnStatus aclnnXxx(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor, aclrtStream stream);
```

Where:

- `aclnnXxxGetWorkspaceSize` is the first-stage API, which calculates the workspace size required for this invocation and applies for NPU memory based on workspaceSize.
- `aclnnXxx` is the second-stage API, which uses the `workspaceSize` and `executor` obtained from the first-stage API and the workspace allocated by the caller to perform the actual computation on the specified stream.
- `Xxx` comes from the operator type passed in during operator prototype registration. For example, the APIs generated for the AddCustom operator are `aclnnAddCustomGetWorkspaceSize` and `aclnnAddCustom`.

## Preparing the Call Project

This section uses the [AddCustom custom operator](../aclnn_quick_start.md) invocation as an example to describe how to write the code logic for single operator invocation. The invocation logic of other operators is similar to that of the AddCustom operator. Modify the code as needed. The project contains two files:

```text
├── CMakeLists.txt    // Build rule file.
├── main.cpp          // Main process implementation file for single operator invocation.
```

The following code examples show the key steps. They are for reference only and cannot be directly copied, compiled, and run. After calling an API, add branches for exception handling and record error logs and prompt logs. These are not listed one by one here.

## Single Operator Call Process

The single-operator API execution process is as follows:

**Figure 1** Single operator API execution API call process.
![](../../../../figures/single_operator_api_execution_interface_call_flow.png "Single operator API execution API call process")

### Including Header Files and Preparing Check Macros

The calling program needs to include the CANN runtime header file, the aclnn base header file, and the operator API header file:

```cpp
#include "acl/acl.h"
#include "aclnn/aclnn_base.h"
#include "aclnn_add_custom.h"
```

`acl/acl.h` provides runtime APIs such as `aclrtMalloc`, `aclrtMemcpy`, and `aclrtSetDevice`; `aclnn/aclnn_base.h` provides aclnn base types and APIs such as `aclnnInit`, `aclnnFinalize`, and `aclOpExecutor`; `aclnn_add_custom.h` is the single operator API header file generated by the AddCustom operator project.

### Initializing the Runtime Environment

```cpp
CHECK_ACL(aclnnInit(nullptr));

const int32_t deviceId = 0;
CHECK_RTL(aclrtSetDevice(deviceId));

aclrtStream stream = nullptr;
CHECK_RTL(aclrtCreateStream(&stream));
```

This step completes ACL initialization, specifies the device, and creates the stream used for subsequent operator delivery.

### Preparing Input/Output Data and Creating Tensors

```cpp
// Allocate memory to store the operator input and output
CHECK_RTL(aclrtMalloc(&input0DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
CHECK_RTL(aclrtMalloc(&input1DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
CHECK_RTL(aclrtMalloc(&outputDeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
// Transfer data
CHECK_RTL(aclrtMemcpy(input0DeviceMem, bufferSize, input0Host.data(),
                      bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));
CHECK_RTL(aclrtMemcpy(input1DeviceMem, bufferSize, input1Host.data(),
                      bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));

int64_t shape[] = {count};
uint64_t shapeNum = 1;
// Create tensors
aclTensor *input0 = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                    ACL_FORMAT_ND, shape, shapeNum, input0DeviceMem);
aclTensor *input1 = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                    ACL_FORMAT_ND, shape, shapeNum, input1DeviceMem);
aclTensor *output = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                    ACL_FORMAT_ND, shape, shapeNum, outputDeviceMem);
```

### Calling the Two-Stage API

```cpp
// Calculate the workspace size and allocate memory.
uint64_t workspaceSize = 0;
aclOpExecutor *executor = nullptr;
CHECK_ACL(aclnnAddCustomGetWorkspaceSize(input0, input1, output,
                                         &workspaceSize, &executor));

void *workspaceDeviceMem = nullptr;
if (workspaceSize > 0) {
    CHECK_RTL(aclrtMalloc(&workspaceDeviceMem, workspaceSize,
                          ACL_MEM_MALLOC_HUGE_FIRST));
}
// Execute the operator.
CHECK_ACL(aclnnAddCustom(workspaceDeviceMem, workspaceSize, executor, stream));
// Wait for synchronization.
CHECK_RTL(aclrtSynchronizeStream(stream));
```

After the first-stage API returns `workspaceSize`, the calling program needs to allocate the workspace memory on the Device side as required. The second-stage API dispatches the operator computation, and then waits for the computation to complete by calling `aclrtSynchronizeStream`.

### Copying Results and Releasing Resources

```cpp
CHECK_RTL(aclrtMemcpy(outputHost.data(), bufferSize, outputDeviceMem,
                      bufferSize, ACL_MEMCPY_DEVICE_TO_HOST));

aclDestroyTensor(input0);
aclDestroyTensor(input1);
aclDestroyTensor(output);
aclDestroyAclOpExecutor(executor);

if (workspaceDeviceMem) {
    aclrtFree(workspaceDeviceMem);
}
aclrtFree(input0DeviceMem);
aclrtFree(input1DeviceMem);
aclrtFree(outputDeviceMem);

CHECK_RTL(aclrtDestroyStream(stream));
CHECK_RTL(aclrtResetDevice(deviceId));
aclnnFinalize();
```

A Tensor created by `aclCreateTensor` must be released by calling `aclDestroyTensor`; the `executor` returned by the first-phase API must be released by calling `aclDestroyAclOpExecutor`; and the Device memory and stream must also be released before the program ends.

## Compilation and Running

### Key Configuration in CMakeLists.txt

After the operator is compiled and deployed, the header file `aclnn_<op>.h` for single operator invocation and the operator API library are generated. In the operator package deployment scenario, `libcust_opapi.so` is usually linked. In the dynamic library or static library compilation scenario, see [Dynamic and Static Library Compilation of Operators](../compilation_and_deployment/dynamic_static_lib_compilation.md) to import the operator library through `find_package` or directly link it by the actual path.

When compiling the operator call program, add the header file directory for single operator calls to the header file search path **include\_directories** so that the header file can be found. In addition, link the **cust\_opapi dynamic** library and add the directory where **libcust\_opapi.so** resides to the library file search path **link\_directories**.

-   Add the header file directory for single-operator calls to the header file search path **include\_directories**. The following sample is for reference only. Set the path based on the actual directory where the header file resides.

    ```
    target_include_directories(execute_add_op PRIVATE
        $ENV{ASCEND_HOME_PATH}/include
        $ENV{ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/include
    )
    ```

-   Link the **cust\_opapi** library.

    ```
    target_link_libraries(execute_add_op PRIVATE
        cust_opapi
        nnopbase
        acl_rt
    )
    ```

-   Add the directory where **libcust\_opapi.so** resides to the library file search path **link\_directories**. The following sample is for reference only. Set the path based on the actual directory where the library file resides.

    ```
    target_link_directories(execute_add_op PRIVATE
        $ENV{ASCEND_HOME_PATH}/lib64
        $ENV{ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/lib
    )
    ```

### Compiling and Running

1.  Set the environment variables in the running environment to configure the header file and library file paths that the single operator verification program depends on for compilation:

    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    # Replace <vendor_name> with the vendor_name in the operator project CMakePresets.json
    export VENDOR_NAME=<vendor_name>
    # Configure the environment variables of the custom operator package
    # If the run package is used
    export LD_LIBRARY_PATH=${ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/lib/:${LD_LIBRARY_PATH}
    # If a dynamic library or static library is used, run the following command, where ${CMAKE_INSTALL_PREFIX} must be replaced with the absolute path of the custom operator package
    export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib/:${LD_LIBRARY_PATH}
    ```

2.  Compile the sample project to generate the single-operator verification executable file.
    1.  Switch to the root directory of the sample project, and then run the following command in the root directory to create a directory for storing compilation files, for example, `build`.

        ```
        mkdir -p build
        ```

    2.  Enter the `build` directory and run the cmake compilation command to generate the compilation files.

        The following is an example of the command:

        ```
        cd build
        cmake ../ -DCMAKE_SKIP_RPATH=TRUE
        ```

    3.  Run the following command to generate the executable file.

        ```
        make
        ```

        The location of the generated executable file is determined by the CMakeLists.txt of the calling project. If no output directory is set, the executable file is usually generated in the `build` directory. If the calling project sets `RUNTIME_OUTPUT_DIRECTORY`, find the executable file in the configured path.

3.  Configure the runtime dependencies in the running environment, and then execute the `execute_add_op` file:

        ```
        # The following path is only an example. Replace it with the actual path of the generated executable file.
        ./build/execute_add_op
        ```

        If "test pass" is displayed, the execution is successful.

## FAQs

**Cannot Find `aclnn_add_custom.h`**

First check whether `VENDOR_NAME` is consistent with `vendor_name` in the `CMakePresets.json` file of the operator project, and then check whether the operator package has been installed:

```bash
ls $ASCEND_OPP_PATH/vendors/${VENDOR_NAME}/op_api/include/aclnn_add_custom.h
```

If you install it to a specified directory, check `<path>/vendors/${VENDOR_NAME}/op_api/include/aclnn_add_custom.h`.

**`libcust_opapi.so: cannot open shared object file` Is Reported at Runtime**

This indicates that the custom operator API library is not in the dynamic library search path at runtime. Set the following:

```bash
export LD_LIBRARY_PATH=${ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/lib/:${LD_LIBRARY_PATH}
```

## Advanced: API Parameter Naming Rules

When you run the AddCustom invocation for the first time, you can skip this section. The input parameters of the aclnn API are generated by converting the operator prototype registration information. Prototype definitions such as inputs, outputs, and attributes are mapped to parameters in the C API. For the specific parameter types and names, refer to the actually generated `aclnn_<op>.h`. When developing operators that contain optional inputs, multiple outputs, attributes, or data-dependent inputs, you can refer to the following rules to confirm the parameter names of the generated API.

The generation rules for the input and output parameters of the **aclnn_Xxx_GetWorkspaceSize** API are as follows:

-   The name of an optional input is suffixed with **Optional**. In the following example, **x** is an optional input.

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *xOptional, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   If the input and output have the same name and are carried by the same tensor, only the input parameter is retained in the generated aclnn API, the const modifier of input is removed, and Ref is used as the suffix. In the following example, both input and output are defined as x in the prototype, and xRef serves as both the input and the output.

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(aclTensor *xRef, ..., uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   If there is only one output, the output parameter is named out. If there are multiple outputs, each output is suffixed with **Out**.

    ```
    // Only one output
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    // Multiple outputs
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *yOut, aclTensor *y1Out, ..., uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   If the operator contains attributes, the attribute parameters are placed between the inputs and outputs. In the following example, x is the operator input, negativeSlope is the operator attribute, and out is the operator output.

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *x, double negativeSlope, aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

When the **ValueDepend** API is used during operator prototype registration to identify an input as a data-dependent input, an additional API is generated. This API supports one-stage computation in which the input data is empty in value-dependent scenarios.

```
aclnnStatus aclnnXxxTensorGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
```

In **aclnn_Xxx_TensorGetWorkspaceSize**, the data types of the **aclnn_Xxx_GetWorkspaceSize** parameters (aclIntArray, aclFloatArray, and aclBoolArray) are converted to the aclTensor data type. The generation rules for other input and output parameters are the same as those for **aclnn_Xxx_GetWorkspaceSize**. In the following example, x0, x1, and x2 are inputs declared as data-dependent by the operator, with data types DT\_INT64, DT\_BOOL, and DT\_FLOAT respectively, and **out** is the operator output.

```
aclnnStatus aclnnXxxGetWorkspaceSize(const aclIntArray *x0, const aclBoolArray *x1, const aclFloatArray *x2, aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
aclnnStatus aclnnXxxTensorGetWorkspaceSize(const aclTensor *x0, const aclTensor *x1, const aclTensor *x2, aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
```
## Related Documents

- [Quick Start](../aclnn_quick_start.md) — Create and run the AddCustom operator project.
- [Operator Project Compilation](../compilation_and_deployment/basic_process.md) — Deploy the operator package and view the deployment directory.
