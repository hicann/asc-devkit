# 算子编译迁移指导<a name="ZH-CN_TOPIC_0000002503428219"></a>

进行算子编译时，开发者需要感知不同架构、不同的AI处理器型号。

- 异构编译场景，开发者使用命令行或者编写Cmake文件进行编译的情况，需要手动修改NPU架构版本号或者AI处理器型号。以修改NPU架构版本号为例，更改编译命令行或编译工程CMakeLists.txt文件中的--npu-arch配置，示例如下：

    ```
    ...
    
    target_compile_options(demo PRIVATE
        // 将dav-xxxx更换为对应NPU架构版本号
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-xxxx>
    )
    ```

    完成`--npu-arch`配置后，迁移过程中还可根据算子实现、目标架构和编译场景按需配置其他内置编译宏开关。相关开关的适用范围、使用约束和配置方式请参考[内置编译宏开关](../../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#section57020345148)。

- 对于自定义算子工程的情况，开发者在CMakeLists.txt（或CMakePresets.json）中配置ASCEND\_SOC\_SERIES字段；存量工程使用ASCEND\_COMPUTE\_UNIT的方式同样有效，但两者不能同时配置。开发者需要在进行算子原型定义时，通过[AddConfig](../../../../api/Utils-API/prototype_register_management/OpAICoreDef/AddConfig.md)接口注册算子支持的AI处理器型号以及相关的配置信息。

    通过该接口注册AI处理器型号的样例如下，ascendxxx应填写构建目标标识，可参考[产品形态说明](https://www.hiascend.com/document/detail/zh/AscendFAQ/ProduTech/productform/hardwaredesc_0001.html)中的soc_series。

    ```
    ...
    
    namespace ops {
    class AddCustom : public OpDef {
    public:
        AddCustom(const char* name) : OpDef(name)
        {
            ...
            // 将ascendXXX更换为对应芯片版本
            this->AICore().AddConfig("ascendxxx");
            
        }
    };
    OP_ADD(AddCustom);
    } // namespace ops
    ```
