# TensorFlow Framework<a name="ZH-CN_TOPIC_0000001615270505"></a>

<!-- md-trans-meta sourceCommit=195cf801df3a15b430fb9afd7dda8149cb021a63 translatedAt=2026-08-26T12:08:51.867Z pushedAt=2026-09-03T08:51:40.778Z -->

> [!NOTE] Note
> For Ascend 950PR/Ascend 950DT, TensorFlow framework operator call is not supported yet.

This section describes the process of adapting TensorFlow framework operators, which maps operators of the TensorFlow framework to CANN operators (custom operators developed by developers based on the CANN framework), thereby completing the process from TensorFlow framework call to CANN operators. It also provides examples of operator call on the TensorFlow framework side to help developers understand the complete process.

The following figure shows the complete development process. The specific steps are as follows:

![](../../../figures/copy.png)

1.  Set up the environment.
    1.  For CANN software installation, see [Environment Setup](../../../getting_started/environment_setup.md).
    2.  To install the framework plugin package, see the "Environment Setup > Installing the Framework Plugin Package" section in *TensorFlow 1.15 Model Porting Guide* or *TensorFlow 2.6.5 Model Porting Guide* in [TensorFlow Model Migration](https://www.hiascend.com/document/detail/en/TensorFlowCommunity/9.1.0/index/index.html) for detailed installation steps of the framework plugin package.
    3.  <a name="li123241091016"></a>[Create an operator project](../aclnn_operator_development/operator_project_creation.md). Use the msOpGen tool to create an operator development project. In the TensorFlow framework operator adaptation scenario, specify the framework as **tf** or **tensorflow** through the framework parameter, and the tool automatically generates the framework adaptation code. Taking the custom CANN operator AddCustom as an example, the specific command for creating an operator development project using the msOpGen tool is as follows:

        ```
        ${INSTALL_DIR}/python/site-packages/bin/msopgen gen -i $HOME/sample/add_custom.json -f tf -c ai_core-<soc_version> -lan cpp -out $HOME/sample/AddCustom
        ```

2.  Implement the operator.
    -   [Define the operator prototype](../aclnn_operator_development/operator_prototype_definition.md). The prototype definition describes the operator inputs, outputs, attributes, and the implementation information of the operator on the AI processor, and associates functions such as the tiling implementation.
    -   For the kernel-side operator implementation and host-side tiling implementation, see [SIMD Operator Implementation](../../../operator_practice/simd_operator_impl/overview.md). For engineering-based operator development, developers can call the tiling API to perform tiling development based on the programming framework provided by CANN. The kernel side also provides corresponding APIs for developers to obtain tiling parameters. For details, see [Kernel-Side Operator Implementation](../aclnn_operator_development/kernel_side_operator_implementation.md) and [Host-Side Tiling Implementation](../aclnn_operator_development/host_tiling_implementation/basic_process.md). The additional constraints introduced by this approach are also described in the preceding sections.

3.  [Integrate the operator into a graph (GE Graph)](../operator_graph_development/basic_development_process.md). In the operator graph integration scenario, you need to provide the implementation of operator graph integration adaptation functions such as shape inference.
4.  Develop the TensorFlow framework adaptation plugin. For details, see [Adaptation Plugin Development](#section1820291291414).
5.  Compile and deploy the operator through the project compilation script, which supports two methods: [operator package compilation](../aclnn_operator_development/operator_package_compilation/operator_project_compilation.md) and [dynamic operator library compilation](../aclnn_operator_development/operator_dynamic_and_static_library_compilation.md).
6.  Call the operator in the TensorFlow framework. For details, see [Mapping TensorFlow Native Operators to CANN Operators](#section6342138121512) and [Developing TensorFlow Custom Operators and Mapping Them to CANN Operators](#section18276103563719). For a complete sample, click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/00_framework/02_onnx).

## Adaptation Plugin Development<a name="section1820291291414"></a>

After [creating the operator project](#li123241091016), a **framework/tf\_plugin** directory is generated in the operator project directory to store the TensorFlow framework adaptation plugin implementation files. Taking the custom CANN operator AddCustom as an example, the operator project directory is as follows:

```
AddCustom
├── build.sh             // Build entry script
├── cmake 
├── CMakeLists.txt       // CMakeLists.txt of the operator project
├── CMakePresets.json    // Build configuration items
├── framework            // Directory for framework adaptation plugin implementation files
│   ├── tf_plugin     //  Directory for TensorFlow framework adaptation plugin implementation files
│   │   ├── CMakeLists.txt    
│   │   ├── tensorflow_add_custom_plugin.cc  // TensorFlow framework adaptation plugin implementation file
│   ├── CMakeLists.txt
├── op_host                      // Host-side implementation file
├── op_kernel                    // Kernel-side implementation file
└── scripts                      // Directory for storing scripts related to custom operator project packaging
```

When a TensorFlow operator is consistent with the CANN operator prototype definition, the TensorFlow framework adaptation plugin implementation code is as follows:

```
#include "register/register.h"
namespace domi {
REGISTER_CUSTOM_OP("AddCustom")
    .FrameworkType(TENSORFLOW) 
    .OriginOpType("AddCustom")   
    .ParseParamsByOperatorFn(AutoMappingByOpFn);
}
```

When a TensorFlow operator is inconsistent with the CANN operator prototype definition, the TensorFlow framework adaptation plugin implementation code is as follows:

```
#include "register/register.h"
REGISTER_CUSTOM_OP("FlashAttentionScore")
    .FrameworkType(TENSORFLOW)
    .OriginOpType({"FlashAttentionScore"})
    .ParseParamsByOperatorFn(FlashAttentionScoreMapping)  
    .ParseOpToGraphFn(AddOptionalPlaceholderForFA);
```

-   Include the header file related to the plugin implementation functions.

    **register.h** is stored in the **include/register/** directory under the CANN software installation path. Including this header file allows you to use the operator registration related classes and call the operator registration related APIs.

-   **REGISTER\_CUSTOM\_OP**: registers a custom operator and passes the _OpType_ of the operator, which must be consistent with the _OpType_ in the operator prototype registration.
    -   **FrameworkType**: **TENSORFLOW** indicates that the original framework is TensorFlow.
    -   **OriginOpType**: type of the operator in the original framework. For a TensorFlow custom operator, you also need to complete the [development of the TensorFlow custom operator](#li312982016383). Here **OriginOpType** is the same as the operator name registered by **REGISTER\_OP**. For a TensorFlow native operator, it is the native operator name.
    -   **ParseParamsByOperatorFn**: used to register the callback function that parses operator parameters to implement the mapping relationship. You need to implement the callback function ParseParamByOpFunc. When the parameters in the original TensorFlow operator correspond one-to-one with the parameters in the CANN operator, you can directly use the automatic mapping callback function AutoMappingByOpFn to implement the mapping automatically.
    -   **ParseOpToGraphFn**: used to register the callback function that adjusts the operator prototype mapping relationship when the TensorFlow operator is inconsistent with the CANN operator prototype definition (for example, the CANN operator prototype definition has an optional input, but the TensorFlow prototype definition does not support optional input and has no optional input).

## Mapping TensorFlow Native Operators to CANN Operators<a name="section6342138121512"></a>

Take the custom operator AddCustom as an example. To map this operator to the TensorFlow built-in operator Add, you need to first modify the plugin code in the **framework/tf\_plugin** directory of the AddCustom custom operator to complete the operator name mapping:

```
#include "register/register.h"
namespace domi {
REGISTER_CUSTOM_OP("AddCustom")   // Current Ascend C custom operator name
    .FrameworkType(TENSORFLOW)    // Third-party framework type TENSORFLOW
    .OriginOpType("Add")          // Map to the TensorFlow native operator Add
    .ParseParamsByOperatorFn(AutoMappingByOpFn);
}
```

After the compilation and deployment of the operator project are complete, construct a TensorFlow 1.15 test case for a single operator to verify it.

1.  Write the test case **_"tf\_add_.py"**.
2.  Import the Python libraries.

    ```
    import logging            # Python standard library logging module
    import tensorflow as tf   # Import the TensorFlow open-source library
    from npu_bridge.estimator import npu_ops   # Import the npu_ops module from the TensorFlow open-source library
    import numpy as np    # Import the Python math base library
    ```

3.  Define the running parameters on the AI processor and CPU through **config\(\)**.

    When "execute\_type" is "ai\_core", it indicates that the single operator network runs on the AI processor, and the Ascend C operator is eventually called.

    When "execute\_type" is "cpu", it indicates that the single operator network runs on the CPU on the host side, and the TensorFlow operator is called.

    ```
    def config(execute_type):
        if execute_type == 'ai_core':
            session_config = tf.ConfigProto(
                allow_soft_placement=True,
                log_device_placement=False,)
            custom_op = session_config.graph_options.rewrite_options.custom_optimizers.add()
            custom_op.name = "NpuOptimizer"
            custom_op.parameter_map["enable_data_pre_proc"].b = True   # Enable data preprocessing to be offloaded to the device side for execution
            custom_op.parameter_map["mix_compile_mode"].b = True    
            custom_op.parameter_map["use_off_line"].b = True     # True indicates that training is executed on the AI processor
            
        elif execute_type == 'cpu':
            session_config = tf.ConfigProto(
                allow_soft_placement=True,
                log_device_placement=False)
    
        return session_config
    ```

4.  Call the main function of the single-operator network test case.

    -   Construct the operator input based on the actual number of operator inputs and the shape.
    -   Implement the calculation of the operator output by calling the relevant TensorFlow APIs based on the operator logic.

    ```
    #Set the tolerance parameters of the np.allclose comparison function.
    #Relative tolerance parameter of the np.allclose comparison function
    atol = 0.001
    #Absolute tolerance parameter of the np.allclose comparison function
    rtol = 0.001
    
    def main(unused_argv):
        shape_params = (8, 2048)
        dtype_params = np.float16
    
        # Construct the two input data of the Add operator, with shape as shape_params and random values in the range [-2, 2]
        x_data = np.random.uniform(-2, 2, size=shape_params).astype(dtype_params)
        y_data = np.random.uniform(-2, 2, size=shape_params).astype(dtype_params)
        # Create placeholders for the two input data of the Add operator respectively
        x = tf.compat.v1.placeholder(dtype_params, shape=shape_params)
        y = tf.compat.v1.placeholder(dtype_params, shape=shape_params)
        # Compute the operator output
        out = tf.math.add(x, y)
        # Run the single operator on the host CPU to obtain the expected result
        with tf.compat.v1.Session(config=config('cpu')) as session:
            result_cpu = session.run(out, feed_dict={x: x_data, y: y_data})
        # Run the single operator on the AI processor to obtain the actual result
        with tf.compat.v1.Session(config=config('ai_core')) as session:
            result_ai_core = session.run(out, feed_dict={x: x_data, y: y_data})
    
        np.array(result_ai_core).astype(dtype_params)
        np.array(result_cpu).astype(dtype_params)
        print('====================================')
       # Compare the actual result on the AI processor with the expected result on the CPU using np.allclose, where atol and rtol are the relative and absolute tolerance parameters of the np.allclose comparison function
        cmp_result = np.allclose(result_ai_core, result_cpu, atol, rtol)
        print(cmp_result)
        print('====================================')
    ```

5. Run the single-operator network.

    ```
    if __name__ == "__main__":
        tf.app.run()
    ```

## Developing a TensorFlow Custom Operator and Mapping It to a CANN Operator<a name="section18276103563719"></a>

1.  Develop the adaptation plugin code. Taking the custom operator AddCustom as an example, to map this operator to the TensorFlow custom operator AddCustom, you need to first modify the plugin code in the **framework/tf\_plugin** directory of the CANN AddCustom custom operator project to complete the operator name mapping:

    ```
    REGISTER_CUSTOM_OP("AddCustom")
      .FrameworkType(TENSORFLOW)      
      .OriginOpType("AddCustom") 
      .ParseParamsByOperatorFn(AutoMappingByOpFn);
    ```

2.  <a name="li312982016383"></a>Develop the TensorFlow custom operator. This section provides only an example. For details, see the official TensorFlow documentation.

    Create the TensorFlow prototype registration file **custom\_assign\_add\_custom.cc** with the following content:

    ```
    #include "tensorflow/core/framework/op.h"
    #include "tensorflow/core/framework/shape_inference.h"
    #include "tensorflow/core/framework/op_kernel.h"
    #include "tensorflow/core/framework/common_shape_fns.h"
    using namespace tensorflow;
    
    // Register the operator prototype through the REGISTER_OP API provided by TensorFlow.
    REGISTER_OP("AddCustom")        // TensorFlow registered operator name
        .Input("x: T")              // Operator prototype, input parameter x of type T
        .Input("y: T")              // Operator prototype, input parameter y of type T
        .Output("z: T")             // Operator prototype, input parameter z of type T
        .Attr("T: {half}")          // Supported range of type T
        .SetShapeFn(shape_inference::BroadcastBinaryOpShapeFn);  // Infer the operator shape information. BroadcastBinaryOpShapeFn is a built-in function provided by TensorFlow. The output shape information is inferred by propagating the input shape, that is, the input and output shapes remain consistent.
    
    // Implement a CPU version of the kernel function. When the TensorFlow computation graph is built, it checks whether every operator has a kernel function on any device (the NPU kernel cannot be perceived). If not, an error is reported. Here, implement a CPU kernel function that always returns an error:
    class AddCustomOp : public OpKernel {
     public:
      explicit AddCustomOp(OpKernelConstruction* context) : OpKernel(context) {}
    
      void Compute(OpKernelContext* context) override {
        OP_REQUIRES_OK(context, errors::Unimplemented("AddCustomOp is not supported on CPU")); 
      }
    };
    
    REGISTER_KERNEL_BUILDER(Name("AddCustom").Device(DEVICE_CPU), AddCustomOp);          // Register the CPU implementation kernel of the AddCustom operator. This function currently only prints a log indicating that the CPU is not supported.
    ```

    Compile the preceding code by using the following command. The output is **libcustom\_ops.so**. In the subsequent operator calling script, you can load this .so file as a Python module through the **load\_op\_library** API to call the custom operator.

    ```
    TF_CFLAGS=( $(python3 -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_compile_flags()))') )     // Obtain the TensorFlow compilation options.
    TF_LFLAGS=( $(python3 -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_link_flags()))') )        // Obtain the TensorFlow link options.
    SOURCE_FILES=custom_assign_add_custom.cc                                                                     // cc file that contains the TensorFlow operator registration and CPU kernel implementation.
    g++ -std=c++14 -shared $SOURCE_FILES -o ${Path}/libcustom_ops.so -fPIC ${TF_CFLAGS[@]} ${TF_LFLAGS[@]} -O2   // Compilation command. The output is libcustom_ops.so. TensorFlow can load this .so file as a Python module through load_op_library to call the custom operator.
    ```

3.  In the test script, load the dynamic library compiled in the previous step to call the custom operator.
    -   Example of call code for TensorFlow 1.15.0

        ```
        import os
        import tensorflow as tf
        import numpy as np
        from npu_bridge.npu_init import *
        tf.enable_resource_variables()
        #Relative tolerance parameter of the np.allclose comparison function
        atol = 0.001
        #Absolute tolerance parameter of the np.allclose comparison function
        rtol = 0.001
        def main(unused_argv):
            custom_op_lib = tf.load_op_library('./outputs/libcustom_ops.so')     # Load the .so file as a Python module
            shape_params = (8, 2048)
            dtype_params = np.float16
            x_data = np.random.uniform(-2, 2, size=shape_params).astype(dtype_params)
            y_data = np.random.uniform(-2, 2, size=shape_params).astype(dtype_params)
            x = tf.compat.v1.placeholder(dtype_params, shape=shape_params)
            y = tf.compat.v1.placeholder(dtype_params, shape=shape_params)
            tf_z = tf.math.add(x, y)                                           # Call the TensorFlow native operator
            ac_z = custom_op_lib.add_custom(x, y)                              # Call the AscendC AddCustom custom operator; add_custom is the snake_case form of AddCustom in REGISTER_OP(AddCustom), converted from UpperCamelCase
            config = tf.ConfigProto()
            custom_op = config.graph_options.rewrite_options.custom_optimizers.add()
            custom_op.name = "NpuOptimizer"   # Configure single operator execution on the AI processor
            config.graph_options.rewrite_options.remapping = RewriterConfig.OFF
            config.graph_options.rewrite_options.memory_optimization = RewriterConfig.OFF
            
            with tf.Session(config=config) as sess:
                sess.run(tf.global_variables_initializer())
                tf_golden = sess.run(tf_z, feed_dict={x: x_data, y: y_data})
            with tf.Session(config=config) as sess:
                sess.run(tf.global_variables_initializer())
                ascend_out = sess.run(ac_z, feed_dict={x: x_data, y: y_data})
            np.array(tf_golden).astype(dtype_params)
            np.array(ascend_out).astype(dtype_params)
            print('====================================')
            # Use np.allclose to compare the actual result obtained on the AI processor with the expected result obtained using the TensorFlow native operator, where atol and rtol are the relative and absolute tolerance parameters of the np.allclose comparison function.
            cmp_result = np.allclose(tf_golden, ascend_out, atol, rtol)
            print(cmp_result)
            print('====================================')
        if __name__ == "__main__":
            tf.app.run()
        ```

    -   Call code for TensorFlow 2.6.5

        ```
        import os
        import tensorflow as tf
        import numpy as np
        import npu_device
        from npu_device.compat.v1.npu_init import *
        npu_device.compat.enable_v1()
        tf.compat.v1.enable_resource_variables()
        #Relative tolerance parameter of the np.allclose comparison function
        atol = 0.001
        #Absolute tolerance parameter of the np.allclose comparison function
        rtol = 0.001
        def main(unused_argv):
            custom_op_lib = tf.load_op_library('./outputs/libcustom_ops.so')     # Load the .so file as a Python module.
            
            shape_params = (8, 2048)
            dtype_params = np.float16
            x_data = np.random.uniform(-2, 2, size=shape_params).astype(dtype_params)
            y_data = np.random.uniform(-2, 2, size=shape_params).astype(dtype_params)
            x = tf.compat.v1.placeholder(dtype_params, shape=shape_params)
            y = tf.compat.v1.placeholder(dtype_params, shape=shape_params)
            tf_z = tf.math.add(x, y)                                           # Call the TensorFlow native operator.
            ac_z = custom_op_lib.add_custom(x, y)                              # Call the AscendC AddCustom custom operator; add_custom is the underscore format converted from the PascalCase name AddCustom in REGISTER_OP(AddCustom).
            config = tf.compat.v1.ConfigProto()
            custom_op = config.graph_options.rewrite_options.custom_optimizers.add()
            custom_op.name = "NpuOptimizer"
            config.graph_options.rewrite_options.remapping = RewriterConfig.OFF
            config.graph_options.rewrite_options.memory_optimization = RewriterConfig.OFF
            
            with tf.compat.v1.Session(config=config) as sess:
                sess.run(tf.global_variables_initializer())
                tf_golden = sess.run(tf_z, feed_dict={x: x_data, y: y_data})
            with tf.compat.v1.Session(config=config) as sess:
                sess.run(tf.global_variables_initializer())
                ascend_out = sess.run(ac_z, feed_dict={x: x_data, y: y_data})
            np.array(tf_golden).astype(dtype_params)
            np.array(ascend_out).astype(dtype_params)
            print('====================================')
            # Use np.allclose to compare the actual result running on the AI processor with the expected result running with the TensorFlow native operator, where atol and rtol are the absolute tolerance and relative tolerance parameters of the np.allclose comparison function.
            cmp_result = np.allclose(tf_golden, ascend_out, atol, rtol)
            print(cmp_result)
            print('====================================')
        if __name__ == "__main__":
            tf.app.run()
        ```

## Optional Input Operator Mapping Relationship Development<a name="section41517302181"></a>

TensorFlow prototype definitions do not support optional inputs. For operators that contain optional inputs, the mapping relationship from TensorFlow to CANN does not satisfy a simple one-to-one mapping. Instead, you need to convert the inputs into optional inputs in the plugin adaptation code and adjust the prototype mapping relationship. The following uses the FlashAttentionScore operator in the CANN operator library as an example to describe how to develop the framework adaptation plugin for such operators.

1.  Develop the adaptation plugin.













    Unlike the simple one-to-one mapping described above, when developing the adaptation plugin, you need to call **ParseOpToGraphFn** for callback registration. The callback function is used to adjust the operator prototype mapping relationship. In this case:

    -   Register a callback function through **ParseParamsByOperatorFn**. In the callback function, map the TensorFlow native operator to an intermediate operator whose IR is consistent with TensorFlow (call **AutoMappingByOpFn** to complete the property mapping).
    -   Register a callback function through **ParseOpToGraphFn** to adjust the operator prototype mapping relationship and finally map the intermediate operator to the operator in the CANN operator library. Here, mapping to a graph refers to a single-operator graph composed of one operator.

    **Note**: In the callback function of **ParseParamsByOperatorFn**, you need to set the TensorFlow operator name to the **original\_type** property of the intermediate operator, so that the **ParseOpToGraphFn** callback function can be triggered later. The sample code is as follows:

    ```
    #include <string>
    #include <vector>
    #include "register/register.h"
    #include "graph/operator.h"
    #include "graph/graph.h"
    #include "graph/operator_factory.h"
    
    namespace domi {
    using namespace ge;
    
    static Status AddOptionalPlaceholderForFA(const ge::Operator &tf_op, ge::Graph &graph) {
      // 1. Create a FlashAttentionScore operator npu_fa_op.
      ge::AscendString op_name;
      tf_op.GetName(op_name);
      auto npu_fa_op = OperatorFactory::CreateOperator(op_name.GetString(), "FlashAttentionScore");
      // 2. Map the TensorFlow operator properties to npu_fa_op.
      float scale_value = 1.0;
      (void)tf_op.GetAttr("scale_value", scale_value);
      (void)npu_fa_op.SetAttr("scale_value", scale_value);
    
      float keep_prob = 1.0;
      (void)tf_op.GetAttr("keep_prob", keep_prob);
      (void)npu_fa_op.SetAttr("keep_prob", keep_prob);
    
      int32_t pre_tokens = 2147483647;
      (void)tf_op.GetAttr("pre_tokens", pre_tokens);
      (void)npu_fa_op.SetAttr("pre_tokens", pre_tokens);
    
      int32_t next_tokens = 2147483647;
      (void)tf_op.GetAttr("next_tokens", next_tokens);
      (void)npu_fa_op.SetAttr("next_tokens", next_tokens);
    
      int32_t head_num = 0;
      (void)tf_op.GetAttr("head_num", head_num);
      (void)npu_fa_op.SetAttr("head_num", head_num);
    
      std::string input_layout;
      (void)tf_op.GetAttr("input_layout", input_layout);
      (void)npu_fa_op.SetAttr("input_layout", input_layout);
    
      int32_t inner_precise = 0;
      (void)tf_op.GetAttr("inner_precise", inner_precise);
      (void)npu_fa_op.SetAttr("inner_precise", inner_precise);
    
      int32_t sparse_mode = 0;
      (void)tf_op.GetAttr("sparse_mode", sparse_mode);
      (void)npu_fa_op.SetAttr("sparse_mode", sparse_mode);
    
      int32_t pse_type = 1;
      (void)tf_op.GetAttr("pse_type", pse_type);
      (void)npu_fa_op.SetAttr("pse_type", pse_type);
    
      int32_t seed = 0;
      (void)tf_op.GetAttr("seed", seed);
      (void)npu_fa_op.SetAttr("seed", seed);
      int32_t offset = 0;
      (void)tf_op.GetAttr("offset", offset);
      (void)npu_fa_op.SetAttr("offset", offset);
      int32_t out_dtype = 0;
      (void)tf_op.GetAttr("out_dtype", out_dtype);
      (void)npu_fa_op.SetAttr("out_dtype", out_dtype);  
    
      // 3. Create the input data.
      std::vector<Operator> inputs;
      for (size_t i = 0UL; i < tf_op.GetInputsSize(); i++) {
        const std::string data_name = "Data_" + std::to_string(i);
        Operator data_op = OperatorFactory::CreateOperator(data_name.c_str(), "Data");
        (void)data_op.SetAttr("index", static_cast<int32_t>(i));
        inputs.emplace_back(data_op);
      }
    
      size_t index = 0UL;
      //4. For required inputs, directly set the data to the operator input.
      (void)npu_fa_op.SetInput("query", inputs[index++]);
      (void)npu_fa_op.SetInput("key", inputs[index++]);
      (void)npu_fa_op.SetInput("value", inputs[index++]);
    
      // 5. For optional inputs, check whether the number of type properties is 0. A non-zero value indicates that the optional input is enabled.
      std::vector<DataType> real_shift_type;
      (void)tf_op.GetAttr("real_shift_type", real_shift_type);
      if (!real_shift_type.empty()) {
        (void)npu_fa_op.SetInput("real_shift", inputs[index++]);
      }
    
      std::vector<DataType> drop_mask_type;
      (void)tf_op.GetAttr("drop_mask_type", drop_mask_type);
      if (!drop_mask_type.empty()) {
        (void)npu_fa_op.SetInput("drop_mask", inputs[index++]);
      }
    
      std::vector<DataType> padding_mask_type;
      (void)tf_op.GetAttr("padding_mask_type", padding_mask_type);
      if (!padding_mask_type.empty()) {
        (void)npu_fa_op.SetInput("padding_mask", inputs[index++]);
      }
      std::vector<DataType> atten_mask_type;
      (void)tf_op.GetAttr("atten_mask_type", atten_mask_type);
      if (!atten_mask_type.empty()) {
        (void)npu_fa_op.SetInput("atten_mask", inputs[index++]);
      }
      std::vector<DataType> prefix_type;
      (void)tf_op.GetAttr("prefix_type", prefix_type);
      if (!prefix_type.empty()) {
        (void)npu_fa_op.SetInput("prefix", inputs[index++]);
      }
      std::vector<DataType> actual_seq_qlen_type;
      (void)tf_op.GetAttr("actual_seq_qlen_type", actual_seq_qlen_type);
      if (!actual_seq_qlen_type.empty()) {
        (void)npu_fa_op.SetInput("actual_seq_qlen", inputs[index++]);
      }
      std::vector<DataType> actual_seq_kvlen_type;
      (void)tf_op.GetAttr("actual_seq_kvlen_type", actual_seq_kvlen_type);
      if (!actual_seq_kvlen_type.empty()) {
        (void)npu_fa_op.SetInput("actual_seq_kvlen", inputs[index++]);
      }
    
      std::vector<DataType> q_start_idx_type;
      (void)tf_op.GetAttr("q_start_idx_type", q_start_idx_type);
      if (!q_start_idx_type.empty()) {
        (void)npu_fa_op.SetInput("q_start_idx", inputs[index++]);
      }
    
      std::vector<DataType> kv_start_idx_type;
      (void)tf_op.GetAttr("kv_start_idx_type", kv_start_idx_type);
      if (!kv_start_idx_type.empty()) {
        (void)npu_fa_op.SetInput("kv_start_idx", inputs[index++]);
      }
      std::vector<DataType> d_scale_q_type;
      (void)tf_op.GetAttr("d_scale_q_type", d_scale_q_type);
      if (!d_scale_q_type.empty()) {
        (void)npu_fa_op.SetInput("d_scale_q", inputs[index++]);
      }
      std::vector<DataType> d_scale_k_type;
      (void)tf_op.GetAttr("d_scale_k_type", d_scale_k_type);
      if (!d_scale_k_type.empty()) {
        (void)npu_fa_op.SetInput("d_scale_k", inputs[index++]);
      }
      std::vector<DataType> d_scale_v_type;
      (void)tf_op.GetAttr("d_scale_v_type", d_scale_v_type);
      if (!d_scale_v_type.empty()) {
        (void)npu_fa_op.SetInput("d_scale_v", inputs[index++]);
      }
      std::vector<DataType> query_rope_type;
      (void)tf_op.GetAttr("query_rope_type", query_rope_type);
      if (!query_rope_type.empty()) {
        (void)npu_fa_op.SetInput("queryRope", inputs[index++]);
      }
      
      std::vector<DataType> key_rope_type;
      (void)tf_op.GetAttr("key_rope_type", key_rope_type);
      if (!key_rope_type.empty()) {
        (void)npu_fa_op.SetInput("keyRope", inputs[index++]);
      }  
      // 6. Use the output of the npu_fa_op operator to construct the output of the graph.
      std::vector<std::pair<Operator, std::vector<size_t>>> output_indexs;
      std::vector<size_t> node_output_index;
      for (size_t i = 0UL; i < npu_fa_op.GetOutputsSize(); i++) {
        node_output_index.emplace_back(i);
      }
      (void)output_indexs.emplace_back(std::make_pair(npu_fa_op, node_output_index));
      (void)graph.SetInputs(inputs).SetOutputs(output_indexs);
      return SUCCESS;
    }
    
    static Status FlashAttentionScoreMapping(const ge::Operator& op_src, ge::Operator& op_dst) {
      // 1. Call the default mapping function.
      if (AutoMappingByOpFn(op_src, op_dst) != ge::GRAPH_SUCCESS) {
        return FAILED;
      }
      // 2. Set the TensorFlow operator name to the original_type property of op_dst for triggering the subsequent ParseOpToGraphFn callback function.
      op_dst.SetAttr("original_type", "FlashAttentionScore");
      return SUCCESS;
    }
    
    REGISTER_CUSTOM_OP("FlashAttentionScore")
        .FrameworkType(TENSORFLOW)
        .OriginOpType({"FlashAttentionScore"})
        .ParseParamsByOperatorFn(FlashAttentionScoreMapping) // Register this function to implement the mapping of the operator's own properties.
        .ParseOpToGraphFn(AddOptionalPlaceholderForFA); // Register this function to convert the inputs in tf into optional inputs and change the edge connection relationships.
    }  // namespace domi
    ```

2.  Register the operator prototype definition of the FlashAttentionScore operator in the TensorFlow open-source framework. Since TensorFlow does not support optional inputs, its optional inputs need to be represented as dynamic inputs in the TensorFlow prototype, and the number of dynamic inputs is marked by a property. **These optional inputs need to be placed at the end of the prototype definition.** The following is an example (**FlashAttentionScore.cc**):

    ```
    #include <algorithm>
    #include <atomic>
    #include <map> 
    #include "tensorflow/core/framework/common_shape_fns.h"
    #include "tensorflow/core/framework/op.h"
    #include "tensorflow/core/framework/op_kernel.h" 
    using namespace tensorflow;
    using shape_inference::InferenceContext;
    using shape_inference::ShapeHandle; 
    using namespace std;
    using namespace chrono; 
    using OpKernelConstructionPtr = OpKernelConstruction*;
    using OpKernelContextPtr = OpKernelContext*;
    using InferenceContextPtr = ::tensorflow::shape_inference::InferenceContext*; 
    namespace {
    class CustOps : public OpKernel {
    public:    
         explicit CustOps(OpKernelConstructionPtr context) : OpKernel(context) {}
         void Compute(OpKernelContextPtr context) override
        {
            std::cout << "Cust Ops not installed!!" << std::endl;
        }
         ~CustOps() override = default;};
    }  // namespace 
    namespace tensorflow {
    REGISTER_OP("FlashAttentionScore")
        .Input("query: T")
        .Input("key: T")
        .Input("value: T")
        .Input("real_shift: real_shift_type")  // Optional inputs are registered as dynamic inputs in the TensorFlow prototype.
        .Input("drop_mask: drop_mask_type")
        .Input("padding_mask: padding_mask_type")
        .Input("atten_mask: atten_mask_type")
        .Input("prefix: prefix_type")
        .Input("actual_seq_qlen: actual_seq_qlen_type")
        .Input("actual_seq_kvlen: actual_seq_kvlen_type")
        .Input("q_start_idx: q_start_idx_type")
        .Input("kv_start_idx: kv_start_idx_type")
        .Input("d_scale_q: d_scale_q_type")
        .Input("d_scale_k: d_scale_k_type")
        .Input("d_scale_v: d_scale_v_type")
        .Input("query_rope: query_rope_type")
        .Input("key_rope: key_rope_type")
        .Output("softmax_max: float32")
        .Output("softmax_sum: float32")
        .Output("softmax_out: T")
        .Output("attention_out: T")
        .Attr("scale_value: float = 1.0")
        .Attr("keep_prob: float = 1.0")
        .Attr("pre_tokens: int = 2147483647")
        .Attr("next_tokens: int = 2147483647")
        .Attr("head_num: int")
        .Attr("input_layout: string")
        .Attr("inner_precise: int = 0")
        .Attr("sparse_mode: int = 0")
        .Attr("pse_type: int = 1")
        .Attr("seed: int = 0")
        .Attr("offset: int = 0")
        .Attr("out_dtype: int = 0") 
        .Attr("T: {float16, float32, bfloat16} = DT_FLOAT")
        .Attr("real_shift_type: list({float16, float32, bfloat16}) >= 0") // Mark the number of dynamic inputs by a property.
        .Attr("drop_mask_type: list({uint8}) >= 0")
        .Attr("padding_mask_type: list({float16, float32, bfloat16}) >= 0")
        .Attr("atten_mask_type: list({bool, uint8}) >= 0")
        .Attr("prefix_type: list({int64}) >= 0")
        .Attr("actual_seq_qlen_type: list({int64}) >= 0")
        .Attr("actual_seq_kvlen_type: list({int64}) >= 0")
        .Attr("q_start_idx_type: list({int64}) >= 0")
        .Attr("kv_start_idx_type: list({int64}) >= 0")
        .Attr("d_scale_q_type: list({float32}) >= 0")
        .Attr("d_scale_k_type: list({float32}) >= 0")
        .Attr("d_scale_v_type: list({float32}) >= 0")
        .Attr("query_rope_type: list({float32}) >= 0")
        .Attr("key_rope_type: list({float32}) >= 0")
        .SetShapeFn([](InferenceContext *c) {
          return Status::OK();
        });
    REGISTER_KERNEL_BUILDER(Name("FlashAttentionScore").Device(DEVICE_CPU), CustOps)}
    ```

    Use the following command to compile the preceding code. The output is **libcustom\_ops.so**. In the subsequent operator call script, you can load this .so file as a Python module through the **load\_op\_library** API to call the custom operator.

    ```
    TF_CFLAGS=( $(python3 -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_compile_flags()))') )     // Obtain the TensorFlow compilation options.
    TF_LFLAGS=( $(python3 -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_link_flags()))') )        // Obtain the TensorFlow link options.
    SOURCE_FILES=FlashAttentionScore.cc                                                                          // cc file that contains the TensorFlow operator registration and CPU kernel implementation.
    g++ -std=c++14 -shared $SOURCE_FILES -o ${Path}/libflashattention.so -fPIC ${TF_CFLAGS[@]} ${TF_LFLAGS[@]} -O2   // Compilation command. The output is libflashattention.so, where ${Path} is a custom path. Later, TensorFlow can load this .so file as a Python module through load_op_library to call the custom operator.
    ```

3.  <a name="li17250141634510"></a>Encapsulate a TensorFlow operator call API and process the optional input in this API. This script needs to load the dynamic library compiled in the previous step.

    ```
    from tensorflow.python.framework import ops
    import tensorflow as tf
    tfOpLib = tf.load_op_library("../build/tf_ops/libflashattention.so")
    # If the optional input is not enabled externally, pass an empty list to the underlying layer.
    def create_optional_input_list(input):
        input_list = []
        if not input is None:
            input_list.append(input)
        return input_list
    # flash_attention_score encapsulation function.
    def npu_flash_attention(query, key, value, head_num, input_layout, real_shift=None, drop_mask=None, padding_mask=None,
                            atten_mask=None, prefix=None, actual_seq_qlen=None, actual_seq_kvlen=None,
                            q_start_idx=None, kv_start_idx=None, d_scale_q=None,d_scale_k=None,d_scale_v=None,query_rope=None,key_rope=None,scale_value=1.0, keep_prob=1.0,
                            pre_tokens=2147483647, next_tokens=2147483647, inner_precise=0, sparse_mode=0,
                            pse_type=1,seed=0,offset=0,out_dtype=0
    ):
        output = tfOpLib.flash_attention_score(query=query, key=key, value=value,
                real_shift=create_optional_input_list(real_shift), drop_mask=create_optional_input_list(drop_mask),
                padding_mask=create_optional_input_list(padding_mask), atten_mask=create_optional_input_list(atten_mask),
                prefix=create_optional_input_list(prefix), actual_seq_qlen=create_optional_input_list(actual_seq_qlen),
                actual_seq_kvlen=create_optional_input_list(actual_seq_kvlen), q_start_idx=create_optional_input_list(q_start_idx),
                kv_start_idx=create_optional_input_list(kv_start_idx),d_scale_q=create_optional_input_list(d_scale_q),
                d_scale_k=create_optional_input_list(d_scale_k),d_scale_v=create_optional_input_list(d_scale_v),
                query_rope=create_optional_input_list(query_rope),key_rope=create_optional_input_list(key_rope),
                scale_value=scale_value, keep_prob=keep_prob, pre_tokens=pre_tokens, next_tokens=next_tokens,
                head_num=head_num, input_layout=input_layout, inner_precise=inner_precise, sparse_mode=sparse_mode,
                pse_type=pse_type,seed=seed,offset=offset,out_dtype=out_dtype
    )
        return output
    ```

4.  Implement the custom operator call in the test script. Assume that the code file in the [previous step](#li17250141634510) is saved as **ops.py**. Import the npu\_flash\_attention function from ops and use it. The call code for TensorFlow 2.6.5 is as follows:

    ```
    import sys
    from ops import npu_flash_attention
    
    import tensorflow as tf
    import numpy as np
    tf.compat.v1.disable_eager_execution()
    
    import npu_device
    from npu_device.compat.v1.npu_init import *
    npu_device.compat.enable_v1()
    
    def sess_config():
        config = tf.compat.v1.ConfigProto()
        custom_op = config.graph_options.rewrite_options.custom_optimizers.add()
        custom_op.name = "NpuOptimizer"
        config.graph_options.rewrite_options.remapping = RewriterConfig.OFF
        config.graph_options.rewrite_options.memory_optimization = RewriterConfig.OFF
        return config
    
    shape = [1, 32, 32]
    query_np = np.random.randn(*shape).astype(np.float16)
    key_np = np.random.randn(*shape).astype(np.float16)
    value_np = np.random.randn(*shape).astype(np.float16)
    
    query = tf.Variable(query_np, tf.float16)
    key = tf.Variable(key_np, tf.float16)
    value = tf.Variable(value_np, tf.float16)
    
    mask = tf.zeros(shape=(shape[0], 1, shape[1], shape[1]), dtype=tf.uint8)
    
    head_num = 1
    input_layout = "BSH"
    flash_result_t = npu_flash_attention(query, key, value, head_num, input_layout, atten_mask=mask)
    
    with tf.compat.v1.Session(config=sess_config()) as sess:
        sess.run(tf.compat.v1.global_variables_initializer())
        flash_result = sess.run(flash_result_t)
        print(flash_result)
    ```

## Dynamic Input Operator Mapping Relationship Development<a name="section108910963218"></a>

For operators with dynamic inputs/outputs, use `AutoMappingByOpFnDynamic` in the callback function `ParseParamByOpFunc` of the plugin to match TensorFlow operators with CANN operators. Use the `DynamicInputOutputInfo` structure class to describe the information about dynamic inputs/outputs, bind the names of the dynamic inputs/outputs with the property names that describe their counts, and then pass them to `AutoMappingByOpFnDynamic` for automatic matching.

Take the `ParseSingleExample` operator as an example. The plugin adaptation code is as follows:

```
#include "register/register.h"
namespace domi {
Status ParseSingleExampleMapping(const ge::Operator& op_src, ge::Operator& op) {
  std::vector<DynamicInputOutputInfo> value;
  const std::string dynamic_input_name_dense_defaults = "dense_defaults";
  const std::string dynamic_input_attr_name_dense_defaults = "Tdense";
  DynamicInputOutputInfo input(kInput, dynamic_input_name_dense_defaults.c_str(),
      dynamic_input_name_dense_defaults.size(), dynamic_input_attr_name_dense_defaults.c_str(),
      dynamic_input_attr_name_dense_defaults.size());
  value.push_back(input);
  const std::string dynamic_output_name_sparse_indices = "sparse_indices";
  const std::string dynamic_output_attr_name_sparse_indices = "num_sparse";
  DynamicInputOutputInfo output(kOutput, 
      dynamic_output_name_sparse_indices.c_str(),
      dynamic_output_name_sparse_indices.size(), dynamic_output_attr_name_sparse_indices.c_str(),
      dynamic_output_attr_name_sparse_indices.size());
  value.push_back(output);
  const std::string dynamic_output_name_sparse_values = "sparse_values";
  const std::string dynamic_output_attr_name_sparse_values = "sparse_types";
  DynamicInputOutputInfo output1(kOutput, 
      dynamic_output_name_sparse_values.c_str(),
      dynamic_output_name_sparse_values.size(), dynamic_output_attr_name_sparse_values.c_str(),
      dynamic_output_attr_name_sparse_values.size());
  value.push_back(output1);
  const std::string dynamic_output_name_sparse_shapes = "sparse_shapes";
  const std::string dynamic_output_attr_name_sparse_shapes = "sparse_types";
  DynamicInputOutputInfo output2(kOutput, 
      dynamic_output_name_sparse_shapes.c_str(),
      dynamic_output_name_sparse_shapes.size(), dynamic_output_attr_name_sparse_shapes.c_str(),
      dynamic_output_attr_name_sparse_shapes.size());
  value.push_back(output2);
  const std::string dynamic_output_name_dense_values = "dense_values";
  const std::string dynamic_output_attr_name_dense_values = "Tdense";
  DynamicInputOutputInfo output3(kOutput, 
      dynamic_output_name_dense_values.c_str(),
      dynamic_output_name_dense_values.size(), dynamic_output_attr_name_dense_values.c_str(),
      dynamic_output_attr_name_dense_values.size());
  value.push_back(output3);
  AutoMappingByOpFnDynamic(op_src, op, value);
  return SUCCESS;
}

// register ParseSingleExample op to GE
REGISTER_CUSTOM_OP("ParseSingleExample")
    .FrameworkType(TENSORFLOW)
    .OriginOpType("ParseSingleExample")
    .ParseParamsByOperatorFn(ParseSingleExampleMapping)
    }
```

>[!NOTE] Note
>Mapping of operators that have both optional inputs and dynamic inputs is not supported yet.
