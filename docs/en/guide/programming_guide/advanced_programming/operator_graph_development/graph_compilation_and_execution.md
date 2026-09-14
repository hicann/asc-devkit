# Graph Compilation and Graph Execution<a name="ZH-CN_TOPIC_0000001920158120"></a>

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-08-26T11:34:12.014Z pushedAt=2026-09-01T07:36:53.521Z -->

This section uses a single-operator model execution example to introduce the graph compilation and graph execution process in graph mode. Single-operator model execution refers to executing operators based on the graph IR. First compile the operators (for example, use the ATC tool to compile the single-operator description file defined by Ascend IR into an operator OM model file), then call ACL APIs to load the operator model, and finally call ACL APIs to execute the operators.

## Environment Requirements<a name="zh-cn_topic_0000001541959061_section19582183344920"></a>

-   You have completed the installation of the CANN driver and software and configured the basic environment variables required by the CANN software by referring to [Environment Setup](../../../getting_started/environment_setup.md).

    After installing the CANN software, when you use the CANN running user to compile and run, log in to the environment as the CANN running user and run the `source ${INSTALL_DIR}/set_env.sh` command to set the environment variables. Replace `${INSTALL_DIR}` with the storage path of the CANN software after installation. For example, if the software is installed by the root user, the default storage path after installation is **/usr/local/Ascend/cann**.

-   You have completed the development and deployment of operators by referring to the [engineering-based development of Aclnn operators](../aclnn_operator_development/overview.md).

## Preparing the Verification Code Project<a name="zh-cn_topic_0000001541959061_section2021523012501"></a>

The directory structure of the code project is as follows. You can click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation) to obtain the complete sample project:

```
├── aclop_invocation
│   ├── add_custom.json                   // Operator description file, used to construct a single-operator model file.
│   ├── CMakeLists.txt
│   └── main.cpp                          // Compile the single operator into an OM file and load the OM file for execution.
```

## Generating a Single-Operator Offline Model File<a name="section17164152011141"></a>

1.  Create a static-shape single-operator description file **add\_custom\_static\_shape.json** to describe the input, output, and attributes of the operator.

    The following is an example of the description file of the AddCustom static-shape operator:

    ```
    [
        {
            "op": "AddCustom",
            "input_desc": [
                {
                    "name": "x",
                    "param_type": "required",
                    "format": "ND",
                    "shape": [8, 2048],
                    "type": "float16"
                },
                {
                    "name": "y",
                    "param_type": "required",
                    "format":"ND",
                    "shape": [8, 2048],
                    "type": "float16"
                }
            ],
            "output_desc": [
                {
                    "name": "z",
                    "param_type": "required",
                    "format":  "ND",
                    "shape": [8, 2048],
                    "type": "float16"
                }
            ]
        }
    ]
    ```

2.  Use the ATC tool to compile the operator description file into a single-operator model file (\*.om file).

    The following is an example of the ATC tool command:

    ```
    atc --singleop=op_verify/run/out/test_data/config/add_custom_static_shape.json --output=. --soc_version=<soc_version>
    ```

    Key parameters are described as follows:

    -   --singleop: path of the single-operator description file (in JSON format).
    -   --output: directory for storing the om model file.
    -   --soc\_version: model of the AI processor. Replace it with the actual value based on your environment.

    After the preceding command is executed, an offline model file with the \*.om suffix is generated in the directory specified by the output parameter.

## Writing Verification Code<a name="zh-cn_topic_0000001541959061_section1862016464513"></a>

You can refer to the following sample to write the code logic for loading and executing a single operator.

The following is a code sample of the key steps. It cannot be directly copied, compiled, and run, and is for reference only. After calling the APIs, you need to add branches for exception handling and record error logs and prompt logs, which are not listed here one by one.

```
// 1. Initialize
CHECK_ACL(aclInit(nullptr));

// 2. Apply for runtime management resources
const int32_t deviceId = 0;
CHECK_ACL(aclrtSetDevice(deviceId));

// 3. Load the single operator model file (*.om file)
CHECK_ACL(aclopSetModelDir("."));

// 4. Set the operator input, apply for memory, and then read the input data and save it to the applied memory
// ......

// 5. Create a stream
aclrtStream stream = nullptr;
aclrtCreateStream(&stream);

// 6. Execute the operator
// opType indicates the operator type name, for example, AddCustom
// inputDesc.size() indicates the number of operator inputs. For example, the AddCustom operator has two inputs
// inputDesc.data() indicates the array of input tensor descriptions of the operator, describing the format, shape, and data type of each input
// inputBuffers.data() indicates the input tensor data of the operator
// outputDesc.size() indicates the number of operator outputs. For example, the AddCustom operator has one output
// outputDesc.data() indicates the array of output tensor descriptions of the operator, describing the format, shape, and data type of each output
// outputBuffers.data() indicates the output tensor data of the operator
// opAttr indicates the operator attributes. If the operator has no attributes, you still need to call the aclopCreateAttr API to create data of the aclopAttr type
// stream is used to maintain the execution order of some asynchronous operations

CHECK_ACL(aclopExecuteV2(opType, inputDesc.size(), inputDesc.data(), inputBuffers.data(),
                             outputDesc.size(), outputDesc.data(), outputBuffers.data(), opAttr, stream));


// 7. Block the application until all tasks in the specified stream are complete.
aclrtSynchronizeStream(stream);

// 8. Process the output data after the operator is executed, for example, display it on the screen or write it to a file. Implement this based on your actual requirements.
// ......

// 9. Release the stream.
aclrtDestroyStream(stream);

// 10. Release the runtime management resources.
aclRet = aclrtResetDevice(deviceId);
aclRet = aclFinalize();

// ....
```

## Running and Verification<a name="zh-cn_topic_0000001541959061_section236513711532"></a>

1.  On the development environment, set the environment variables to configure the paths of the header files and library files that the single-operator verification program depends on for compilation. The following is an example of setting the environment variables. Replace $\{INSTALL\_DIR\} with the storage path of the CANN software after installation. For example, if you install the software as the root user, the default storage path after installation is /usr/local/Ascend/cann. {arch-os} indicates the architecture and OS of the running environment, where arch indicates the OS architecture and os indicates the OS, for example, x86\_64-linux.

    ```
    export DDK_PATH=${INSTALL_DIR}
    export NPU_HOST_LIB=${INSTALL_DIR}/{arch-os}/devlib
    ```

2.  Compile the sample project to generate the single-operator verification executable file.
    1.  Switch to the root directory of the sample project, and then run the following command in the root directory to create a directory for storing the compilation files. For example, the created directory is "build".

        **mkdir -p build**

    2.  Go to the build directory and run the cmake compilation command to generate the compilation files.

        The following is an example of the command:

        **cd build**

        **cmake ../src -DCMAKE\_SKIP\_RPATH=TRUE**

    3.  Run the following command to generate the executable file.

        **make**

        The executable file **execute\_add\_op** is generated in the output directory of the project directory.

3.  Execute the single operator.
    1.  As the running user (for example, HwHiAiUser), copy **execute\_add\_op** from the output directory of the sample project in the development environment to any directory in the running environment.

        Note: If your development environment is also the running environment, you can skip this copy operation.

    2.  In the running environment, execute the **execute\_add\_op** file to verify the single-operator model file.

        **chmod +x execute\_add\_op**

        **./execute\_add\_op**

        If "test pass" is displayed, the execution is successful.
