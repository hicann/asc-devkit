# 适配插件开发<a name="ZH-CN_TOPIC_0000001741610328"></a>

<!-- npu="950,A3" id1 -->
>[!NOTE]说明 
><!-- npu="A3" id2 -->
>针对Atlas A3系列产品，暂不支持ONNX框架算子调用。
><!-- end id2 -->
>
><!-- npu="950" id3 -->
>针对Ascend 950PR&950DT系列产品，暂不支持ONNX框架算子调用。
><!-- end id3 -->
<!-- end id1 -->

您可以参考本章节进行算子适配插件的开发，将ONNX框架的算子映射成适配AI处理器的算子（下文简称CANN算子），从而完成从ONNX框架调用Ascend C自定义算子的过程。如果只涉及插件开发，也可以不依赖自定义算子工程，参考[LINK](https://gitcode.com/cann/cann-samples/tree/master/Samples/0_Introduction)中的custom_op_in_graph样例生成算子插件so后，通过ASCEND_CUSTOM_OPP_PATH指定so所在路径进行使用。

按[快速入门](../../aclnn_operator_development/aclnn_quick_start.md#create-project)手工创建算子工程后，在工程根目录执行以下命令，新建ONNX框架插件目录：

```bash
mkdir -p framework/onnx_plugin
```

在该目录创建`leaky_relu_custom_plugin.cc`，按下文编写插件源码，并创建各层CMakeLists.txt。完成后的工程结构如下：

```text
LeakyReluCustom
├── CMakeLists.txt
├── framework
│   ├── CMakeLists.txt
│   └── onnx_plugin
│       ├── CMakeLists.txt
│       └── leaky_relu_custom_plugin.cc
├── op_host
│   ├── CMakeLists.txt
│   └── ...
└── op_kernel
    ├── CMakeLists.txt
    └── ...
```

**1. 接入顶层构建**：保留已有的包声明和Host/Kernel子目录，在工程根目录的CMakeLists.txt中、`npu_op_package`之后添加：

```cmake
add_subdirectory(framework)
```

**2. 创建framework/CMakeLists.txt**，保存以下内容：

```cmake
add_subdirectory(onnx_plugin)
```

**3. 创建framework/onnx_plugin/CMakeLists.txt**，保存以下内容，编译插件并将其加入与算子相同的包。`vendor_name`沿用快速入门顶层文件中的变量：

```cmake
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} plugin_srcs)
# 插件源码若使用nlohmann/json（见下文头文件说明），需一并配置其下载与包含，
# 参考样例framework/onnx_plugin/CMakeLists.txt中的NLOHMANN_JSON配置
npu_op_library(cust_onnx_parsers ONNX_PLUGIN
    ${plugin_srcs}
)
npu_op_package_add(${vendor_name}
    LIBRARY cust_onnx_parsers
)
```

接口参数见[CMake函数参考](../../aclnn_operator_development/appendix/cmake_function_reference.md#npu_op_library)。框架调用还需要入图原型库，请按[编译与部署基本流程](../../aclnn_operator_development/compilation_and_deployment/basic_process.md#graph-library)在`op_host/CMakeLists.txt`中补充GRAPH库配置，然后重新编译安装算子包。

下文主要讲解ONNX框架适配插件实现文件（leaky\_relu\_custom\_plugin.cc ）的开发流程。

```
#include "register/register.h"
#include "graph/operator.h"
#include <nlohmann/json.hpp>
namespace domi {
    Status ParseParamByOpFunc(const ge::Operator& op_src, ge::Operator& op_dest) {
        //...
    }
    REGISTER_CUSTOM_OP("OpType")
        .FrameworkType(ONNX) 
        .OriginOpType("OriginOpType")
        .ParseParamsByOperatorFn(ParseParamByOpFunc)   //用来注册解析算子属性的函数
        .ImplyType(ImplyType::TVM);  // Ascend C算子实现类型设置为TVM
}
```

1.  包含所需头文件。
    -   register.h，存储在CANN软件安装后文件存储路径的“include/register/”目录下，包含该头文件，可使用算子注册相关类，调用算子注册相关的接口。
    -   operator.h（可选），存储在CANN软件安装后文件存储路径的“include/graph/”目录下，包含该头文件，可以使用Operator类相关接口，获取算子输入输出及属性等算子信息。
    -   json.hpp，用于进行ONNX数据定义的解析，将String类型的算子参数定义转换为json格式。涉及json解析时，nlohmann/json的下载与包含配置请参考[样例的framework/onnx_plugin/CMakeLists.txt](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/framework/onnx_plugin/CMakeLists.txt)。

2.  使用REGISTER\_CUSTOM\_OP宏，完成CANN算子和ONNX框架的算子映射关系注册。使用方法如下：
    -   REGISTER\_CUSTOM\_OP：注册自定义算子，OpType为算子类型名称，需要与[算子原型注册](../../aclnn_operator_development/design_and_implementation/operator_prototype_definition.md)中的OpType保持一致。
    -   FrameworkType：ONNX代表原始框架为ONNX。
    -   OriginOpType：算子在原始框架中的类型。例如自定义算子_OpTypeA_，对应ONNX算子库版本opset\_version=11，应传入“ai.onnx::11::_OpTypeA_”，当前支持的ONNX版本范围为9\~15。
    -   ParseParamsByOperatorFn\(_ParseParamByOpFunc_\)：用来注册解析算子参数实现映射关系的回调函数，需要用户自定义实现回调函数ParseParamByOpFunc。具体实现方式参考[3](#li213610403113)。
    -   ImplyType：指定算子的实现方式。Ascend C算子实现类型设置为TVM。

3.  <a name="li213610403113"></a>实现回调函数ParseParamByOpFunc。其函数声明如下所示：

    ```
    Status ParseParamByOpFunc(const ge::Operator& op_src, ge::Operator& op_dest)
    ```

    -   _ParseParamByOpFunc_：函数名称，用户自定义。
    -   op\_src：ONNX框架定义的Operator类对象，包含ONNX模型中自定义的算子属性信息，定义来源于ONNX框架的原始模型文件。
    -   op\_dest：CANN算子数据结构，保存算子信息。

    开发者需要在回调函数中实现属性的解析和映射，具体实现方式如下：

    ONNX原始模型中，属性为repeated message类型，对于repeated message类型的参数，可使GetAttr\(const char \*name, ge::AscendString &attr\_value\)接口获取其属性值，然后将AscendString类型的属性值转换为String类型，再将其转换为json格式进行属性字段的解析。

    实现如下所示：

    ```
    Status ParseOnnxParamsLeakyReluCustom(const ge::Operator& op_src, ge::Operator& op_dest) {
      // trans op_src to op_dest
      // if op_src get required attr failed, need to return Failed
      // if op_src get optional attr failed, need to return Failed or set a default value
      float negative_slope = 0.01f;
      string negative_slope_str;
      AscendString attrs_string;
      // 使用固定属性名称“attribute”获取ONNX算子中的属性，并赋值给AscendString类型对象
      if (ge::GRAPH_SUCCESS == op_src.GetAttr("attribute", attrs_string)) {
        // 转换为json格式
        json attrs = json::parse(attrs_string.GetString());
        for (json attr : attrs["attribute"]) {
          if (attr["name"] == "alpha" && attr["type"] == kTypeFloat) {
            negative_slope_str = attr["f"];  // float type in json has accuracy loss, so we use string type to store it
            negative_slope = atof(negative_slope_str.c_str());
          }
        }
      }
      op_dest.SetAttr("negative_slope", negative_slope);
      return SUCCESS;
    }
    ```

    >[!NOTE]说明 
    >- 当前版本GetAttr与SetAttr接口不支持对原始文件中数据类型为double和uint64的字段进行解析。
    >- 使用ATC工具执行模型转换时，对属性的获取情况不会进行强校验。所以进行算子适配插件实现时，若用户调用GetAttr失败，建议根据算子实际情况增加相应的处理逻辑，例如，针对必选属性，可返回失败，针对可选属性，可设置默认值。
