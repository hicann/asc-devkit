# Adaptation Plugin Development<a name="ZH-CN_TOPIC_0000001741610328"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T12:10:18.683Z pushedAt=2026-09-03T07:43:31.233Z -->

>[!NOTE] Note
>For Atlas A3 training products/Atlas A3 inference products, ONNX framework operator call is not supported.
>For Ascend 950PR/Ascend 950DT, ONNX framework operator call is not supported.

You can refer to this section to develop an operator adaptation plugin, mapping ONNX framework operators to operators adapted to AI processors (hereinafter referred to as CANN operators), thereby completing the process of calling Ascend C custom operators from the ONNX framework.

After completing [operator project creation](../tensorflow_framework.md#li123241091016), a **framework/onnx\_plugin** directory is generated under the operator project directory to store the ONNX framework adaptation plugin implementation files. Taking the custom CANN operator LeakyReluCustom as an example, the operator project directory is as follows:

```
LeakyReluCustom
├── build.sh             // Build entry script
├── cmake 
├── CMakeLists.txt       // CMakeLists.txt of the operator project
├── CMakePresets.json    // Build configuration items
├── framework            // Directory for framework adaptation plugin implementation files
│   ├── onnx_plugin     //  Directory for ONNX framework adaptation plugin implementation files
│   │   ├── CMakeLists.txt    
│   │   ├── leaky_relu_custom_plugin.cc // ONNX framework adaptation plugin implementation file
│   ├── CMakeLists.txt
├── op_host                      // Host-side implementation file
├── op_kernel                    // Kernel-side implementation file
└── scripts                      // Directory for scripts related to custom operator project packaging
```

The following describes the development process of the ONNX framework plugin implementation file (**leaky_relu_custom_plugin.cc**).

```
#include "register/register.h"
#include "graph/operator.h"
#include "json.hpp"
namespace domi {
    Status ParseParamByOpFunc(const ge::Operator& op_src, ge::Operator& op_dest) {
        //...
    }
    REGISTER_CUSTOM_OP("OpType")
        .FrameworkType(ONNX) 
        .OriginOpType("OriginOpType")
        .ParseParamsByOperatorFn(ParseParamByOpFunc)   // Used to register the function for parsing operator attributes
        .ImplyType(ImplyType::TVM);  // Set the Ascend C operator implementation type to TVM
}
```

1.  Include the required header files.
    -   **register.h**, stored in the **include/register/** directory under the CANN software installation path. Including this header file allows you to use operator registration-related classes and call operator registration-related APIs.
    -   **operator.h** (optional), stored in the **include/graph/** directory under the CANN software installation path. Including this header file allows you to use Operator class-related APIs to obtain operator information such as inputs, outputs, and attributes.
    -   **json.hpp**, used to parse ONNX data definitions and convert operator parameter definitions of the string type into JSON format. If the **json.hpp** file is not provided in the sample project, you can download it and place it in any path that the project can find, and then include this header file. For the download path, see [json.hpp](https://github.com/nlohmann/json/blob/develop/include/nlohmann/json.hpp).

2.  Use the **REGISTER\_CUSTOM\_OP** macro to register the mapping between the CANN operator and the ONNX framework operator. The usage is as follows:
    -   **REGISTER\_CUSTOM\_OP**: registers a custom operator. **OpType** is the operator type name, which must be consistent with the **OpType** in [Operator Prototype Registration](../../aclnn_operator_development/operator_prototype_definition.md).
    -   **FrameworkType**: ONNX indicates that the original framework is ONNX.
    -   **OriginOpType**: type of the operator in the original framework. For example, for the custom operator _OpTypeA_ corresponding to ONNX operator library version opset\_version=11, pass "ai.onnx::11::_OpTypeA_". The currently supported ONNX version range is 9\~15.
    -   **ParseParamsByOperatorFn\(_ParseParamByOpFunc_\)**: registers the callback function that parses operator parameters and implements the mapping. You need to implement the callback function ParseParamByOpFunc. For details about the implementation, see [3](#li213610403113).
    -   **ImplyType**: specifies the implementation mode of the operator. For Ascend C operator implementation, set this parameter to TVM.

3.  <a name="li213610403113"></a>Implement the callback function ParseParamByOpFunc. Its function declaration is as follows:

    ```
    Status ParseParamByOpFunc(const ge::Operator& op_src, ge::Operator& op_dest)
    ```

    -   **_ParseParamByOpFunc_**: function name, user-defined.
    -   **op\_src**: Operator class object defined by the ONNX framework, which contains the custom operator attribute information in the ONNX model. It is derived from the original model file of the ONNX framework.
    -   **op\_dest**: CANN operator data structure, which stores operator information.

    Developers need to implement attribute parsing and mapping in the callback function. The specific implementation is as follows:

    In the original ONNX model, attributes are of the repeated message type. For parameters of the repeated message type, you can use the **GetAttr\(const char \*name, ge::AscendString &attr\_value\)** API to obtain the attribute value, then convert the AscendString-type attribute value to the string type, and then convert it to the JSON format for parsing the attribute fields.

    The implementation is as follows:

    ```
    Status ParseOnnxParamsLeakyReluCustom(const ge::Operator& op_src, ge::Operator& op_dest) {
      // trans op_src to op_dest
      // if op_src get required attr failed, need to return Failed
      // if op_src get optional attr failed, need to return Failed or set a default value
      float negative_slope = 0.01f;
      string negative_slope_str;
      AscendString attrs_string;
      // Use the fixed attribute name "attribute" to obtain the attribute in the ONNX operator and assign it to an AscendString object.
      if (ge::GRAPH_SUCCESS == op_src.GetAttr("attribute", attrs_string)) {
        // Convert to the JSON format.
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

    >[!NOTE] Note
    >- In the current version, the **GetAttr** and **SetAttr** APIs do not support parsing fields whose data types are double and uint64 in the original file.
    >- When the ATC tool is used to perform model conversion, the acquisition of attributes is not strictly verified. Therefore, when implementing the operator adaptation plugin, if the user fails to call **GetAttr**, it is recommended to add corresponding processing logic based on the actual situation of the operator. For example, for a mandatory attribute, a failure can be returned; for an optional attribute, a default value can be set.

