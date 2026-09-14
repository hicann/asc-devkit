# Operator Compilation Migration Guide<a name="ZH-CN_TOPIC_0000002503428219"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T12:47:52.582Z pushedAt=2026-09-11T06:51:16.184Z -->

When compiling operators, developers need to be aware of different architectures and different AI processor models.

- In heterogeneous compilation scenarios where developers compile using the command line or a CMake file, they need to manually modify the NPU architecture version number or the AI processor model. Taking modifying the NPU architecture version number as an example, change the **--npu-arch** configuration in the compilation command line or in the **CMakeLists.txt** file of the compilation project. The following is an example:

    ```
    ...
    
    target_compile_options(demo PRIVATE
        // Replace dav-xxxx with the corresponding NPU architecture version number.
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-xxxx>
    )
    ```

- For a standard custom operator project generated using the msOpGen tool, the compilation configuration file **CMakePresets.json** is automatically generated in the operator project directory, and the **ASCEND\_COMPUTE\_UNIT** field is automatically filled in. When defining the operator prototype, developers need to register the AI processor models supported by the operator and the related configuration information through the AddConfig API. The AddConfig API prototype is as follows: the **soc** parameter indicates the AI processor model, and **aicore\_config** indicates other configuration information.

    ```
    void AddConfig(const char *soc);
    void AddConfig(const char *soc, OpAICoreConfig &aicore_config);
    ```

    The following is an example of registering an AI processor model through this API. For the rules for filling in ascendxxx, see the **ASCEND\_COMPUTE\_UNIT** field in the compilation configuration file **CMakePresets.json** in the operator project directory. The value of this field is automatically generated when the project is created using msOpGen.

    ```
    ...
    
    namespace ops {
    class AddCustom : public OpDef {
    public:
        AddCustom(const char* name) : OpDef(name)
        {
            ...
            // Replace ascendXXX with the corresponding chip version.
            this->AICore().AddConfig("ascendxxx");
            
        }
    };
    OP_ADD(AddCustom);
    } // namespace ops
    ```
