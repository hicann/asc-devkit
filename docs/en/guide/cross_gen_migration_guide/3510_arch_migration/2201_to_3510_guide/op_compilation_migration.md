# Operator Compilation Migration Guide<a name="ZH-CN_TOPIC_0000002503428219"></a>

<!-- md-trans-meta sourceCommit=6002f493cefe766494b31aed2c26800bc6ec5d60 translatedAt=2026-10-23T10:55:15.224Z pushedAt=2026-11-03T07:05:13.253Z nl=2 -->

When compiling operators, developers need to be aware of different architectures and different AI processor models.

- In heterogeneous compilation scenarios where developers compile using the command line or a CMake file, they need to manually modify the NPU architecture version number or the AI processor model. Taking modifying the NPU architecture version number as an example, change the **--npu-arch** configuration in the compilation command line or in the **CMakeLists.txt** file of the compilation project. The following is an example:

    ```
    ...
    
    target_compile_options(demo PRIVATE
        // Replace dav-xxxx with the corresponding NPU architecture version number.
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-xxxx>
    )
    ```

    After the `--npu-arch` configuration is complete, you can configure other built-in compilation macro switches as needed based on the operator implementation, target architecture, and compilation scenario during migration. For the applicable scope, usage constraints, and configuration methods of the related switches, see [Built-in Compilation Macro Switches](../../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#section57020345148).

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
