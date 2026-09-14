# Single-Operator API Call<a name="ZH-CN_TOPIC_0000001566407580"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T11:49:47.479Z pushedAt=2026-09-03T02:43:10.807Z -->

The single-operator API call method refers to directly calling the single-operator API to execute an operator based on the C language API. After the operator project is created, complete the operator prototype definition, kernel-side operator implementation, and host-side tiling implementation based on the project code framework, compile and deploy the operator through the project build script, and then perform the single-operator API call.

## Basic Principles<a name="section9403945674"></a>

After the custom operator is compiled, the single operator API is automatically generated and can be directly called in the application.

The single operator API is generally defined as a "two-stage API", as shown below:

```
aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
aclnnStatus aclnnXxx(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor, aclrtStream stream);
```

In the preceding example, **aclnn_Xxx_GetWorkspaceSize/aclnn_Xxx_TensorGetWorkspaceSize** is the first-stage API, which is mainly used to calculate the amount of workspace memory required for this API call. After obtaining the **workspaceSize** required for this computation, allocate NPU memory based on **workspaceSize**, and then call the second-stage API **aclnn_Xxx_** to execute the computation. *Xxx* represents the operator type passed in during [operator prototype registration](operator_prototype_definition.md).

The generation rules for the input and output parameters of the **aclnn_Xxx_GetWorkspaceSize** API are as follows:

-   The name of an optional input is suffixed with **Optional**. In the following example, **x** is an optional input.

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *xOptional, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   If the input and output have the same name and are carried by the same tensor, only the input parameter is retained in the generated aclnn API, the **const** modifier of input is removed, and **Ref** is used as the suffix. In the following example, both input and output are defined as **x** in the prototype, and **xRef** serves as both the input and the output.

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

-   If the operator contains attributes, the attribute parameters are placed between the inputs and outputs. In the following example, **x** is the operator input, **negativeSlope** is the operator attribute, and **out** is the operator output.

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

## Prerequisites<a name="section29151158429"></a>

-   Create a custom operator project by referring to [Creating an Operator Project](operator_project_creation.md).
-   Complete the preparations for kernel-side implementation by referring to [Kernel-Side Operator Implementation](kernel_side_operator_implementation.md), and complete the preparations for host-side implementation by referring to [Host-Side Tiling Implementation](host_tiling_implementation/basic_process.md) and [Operator Prototype Definition](operator_prototype_definition.md).
-   For the operator package compilation scenario, compile and deploy the operator by referring to [Operator Project Compilation](operator_package_compilation/operator_project_compilation.md) and [Operator Package Deployment](operator_package_compilation/operator_package_deployment.md). During compilation and deployment, enable the binary compilation function of the operator: modify the compilation configuration file **CMakePresets.json** in the operator project and set **ENABLE\_BINARY\_PACKAGE** to **True**. During compilation and deployment, you can deploy the operator binary to the current environment to facilitate subsequent operator calls.

    ```
    "ENABLE_BINARY_PACKAGE": {
                        "type": "BOOL",
                        "value": "True"
                    },
    ```

    After the operator is compiled and deployed, the header file **aclnn\__xx_.h** and the dynamic library **libcust\_opapi.so** for single-operator calls are generated in the **op\_api** directory under the operator package installation directory.

    Taking the default installation scenario as an example, the directory structure where the header file .h and the dynamic library **libcust\_opapi.so** for single-operator calls are located is as follows:

    ```
    ├── opp    // Operator library directory
    │   ├── vendors     // Directory where custom operators are located
    │       ├── config.ini
    │       └── vendor_name1   // Stores the custom operators deployed by the corresponding vendor. This name is the vendor_name configured when compiling the custom operator installation package. If it is not configured, the default value is customize.
    │           ├── op_api
    │           │   ├── include
    │           │   │  └── aclnn_xx.h
    │           │   └── lib
    │           │       └── libcust_opapi.so
    ...
    ```

-   For the operator dynamic library compilation scenario, compile the operator by referring to [Operator Dynamic Library and Static Library Compilation](operator_dynamic_and_static_library_compilation.md). After compilation, the header file **aclnn\__xx_.h** and the dynamic library **libcust\_opapi.so** for single-operator calls are generated in the following path, where **CMAKE\_INSTALL\_PREFIX** is the path for storing compilation outputs configured by the developer in the cmake file.
    -   Dynamic library path: $\{CMAKE\_INSTALL\_PREFIX\}/op\_api/lib/libcust\_opapi.so
    -   Header file path: $\{CMAKE\_INSTALL\_PREFIX\}/op\_api/include

## Preparing the Verification Code Project<a name="zh-cn_topic_0000001541959061_section2021523012501"></a>

The directory structure of the code project is as follows. You can click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation) to obtain the complete sample project:

```
├── CMakeLists.txt    // Compilation rule file
├── main.cpp    // Implementation file of the main process for single-operator call
```

## Single-Operator Call Process<a name="section9465185611515"></a>

The single-operator API execution process is as follows:

**Figure 1**  Single-operator API execution process<a name="fig10713450184013"></a>  
![](../../../figures/single_operator_api_call_process.png "Single-operator API execution process")

This section uses the **AddCustom custom operator** call as an example to describe how to write the code logic for a single-operator call. The call logic of other operators is roughly the same as that of the Add operator. Modify the code based on the actual situation.

The following code examples show the key steps. They are for reference only and cannot be directly copied, compiled, and run. After calling an API, add branches for exception handling and record error logs and prompt logs. These are not listed one by one here.

>[!NOTE] Note
>Because the single operator API execution mode automatically generates .cpp and .h files in the **build\_out/autogen** directory of the build project, include the automatically generated single-operator API execution header file when writing the single-operator call code. Example:
>```
>#include "aclnn_add_custom.h"
>```

```
// 1. Initialize.
CHECK_ACL(aclnnInit(nullptr));

// 2. Allocate runtime management resources.
const int32_t deviceId = 0;
CHECK_ACL(aclrtSetDevice(deviceId));

// 3. Allocate memory to store the operator input and output.
// ......

// 4. Transfer data.
CHECK_ACL(aclrtMemcpy(input0DeviceMem, bufferSize, input0HostData.data(), bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));
CHECK_ACL(aclrtMemcpy(input1DeviceMem, bufferSize, input1HostData.data(), bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));

// 5. Calculate the workspace size and allocate memory.
uint64_t workspaceSize = 0;
aclOpExecutor* executor = nullptr;
CHECK_ACL(aclnnAddCustomGetWorkspaceSize(input0, input1, output0, &workspaceSize, &executor));
void* workspaceDeviceMem = nullptr;
if (workspaceSize > 0) {
    CHECK_ACL(aclrtMalloc(&workspaceDeviceMem, workspaceSize, ACL_MEM_MALLOC_HUGE_FIRST));
}

// 6. Execute the operator.
CHECK_ACL(aclnnAddCustom(workspaceDeviceMem, workspaceSize, executor, stream));

// 7. Wait for synchronization.
CHECK_ACL(aclrtSynchronizeStream(stream));

// 8. Process the output data after the operator is executed, for example, display it on the screen or write it to a file. This is implemented by the user based on the actual situation.
// ......

// 9. Release the runtime management resources.
CHECK_ACL(aclrtResetDevice(deviceId));
// ....

// 10. Deinitialize.
CHECK_ACL(aclnnFinalize());
```

## CMakeLists File<a name="section1930615371323"></a>

After the operator is compiled, the header file **aclnn\__xx_.h** and the dynamic library **libcust\_opapi.so** for single operator calls are generated. For the specific paths, see [Prerequisites](#section29151158429).

When compiling the operator call program, add the header file directory for single operator calls to the header file search path **include\_directories** so that the header file can be found. In addition, link the **cust\_opapi dynamic** library and add the directory where **libcust\_opapi.so** resides to the library file search path **link\_directories**.

-   Add the header file directory for single-operator calls to the header file search path **include\_directories**. The following sample is for reference only. Set the path based on the actual directory where the header file resides.

    ```
    target_include_directories(execute_add_op PRIVATE
        $ENV{ASCEND_HOME_PATH}/include
        $ENV{ASCEND_OPP_PATH}/vendors/customize/op_api/include
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
        $ENV{ASCEND_OPP_PATH}/vendors/customize/op_api/lib
    )
    ```

## Compilation and Running<a name="section1129535885018"></a>

1.  On the development environment, set environment variables to configure the paths of the header files and library files that the single-operator verification program depends on during compilation. The following is an example of setting environment variables. Replace *$\{INSTALL\_DIR\}* with the storage path of the files after CANN software installation. For example, if you install CANN as the root user, the default storage path of the installed files is **/usr/local/Ascend/cann**. **_\{arch-os\}_** indicates the architecture and operating system of the running environment, where **_arch_** indicates the operating system architecture and **_os_** indicates the operating system, for example, **x86\_64-linux** or **aarch64-linux**.

    ```
    export DDK_PATH=${INSTALL_DIR}
    export NPU_HOST_LIB=${INSTALL_DIR}/{arch-os}/devlib
    ```

2.  Compile the sample project to generate the single-operator verification executable file.
    1.  Switch to the root directory of the sample project, and then run the following command in the root directory to create a directory for storing compilation files, for example, create a directory named **build**.

        ```
        mkdir -p build
        ```

    2.  Enter the **build** directory and run the cmake compilation command to generate the compilation files.

        The following is an example of the command:

        ```
        cd build
        cmake ../src -DCMAKE_SKIP_RPATH=TRUE
        ```

    3.  Run the following command to generate the executable file.

        ```
        make
        ```

        The executable file **execute\_add\_op** is generated in the output directory of the project directory.

3.  Execute the single operator.
    1.  As the running user (for example, **HwHiAiUser**), copy **execute\_add\_op** from the output directory of the sample project on the development environment to any directory on the operating environment.

        Note: If your development environment is also the operating environment, you can skip this copy operation.




    2.  In the operating environment, execute the **execute\_add\_op** file:

        ```
        chmod +x execute_add_op
        ./execute_add_op
        ```

        If "test pass" is displayed, the execution is successful.
