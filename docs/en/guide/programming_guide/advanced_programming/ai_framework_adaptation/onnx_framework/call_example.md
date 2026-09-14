# Call Example<a name="ZH-CN_TOPIC_0000001741451164"></a>

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-08-26T12:09:45.284Z pushedAt=2026-09-03T07:35:30.367Z -->

After completing the development of the ONNX framework adaptation plugin, you can call Ascend C custom operators from the ONNX framework. The following uses an ONNX framework network that contains only a LeakyRelu operator as an example (the LeakyRelu operator in this network is mapped to a custom LeakyRelu operator through the adaptation plugin) to demonstrate the inference process using an inference tool. The purpose is to help you quickly experience the process of calling custom operators in a network during inference.

Before performing the following steps, you need to complete the kernel-side and host-side development of the custom LeakyRelu operator, the development of the ONNX adaptation plugin, and the compilation and deployment of the operator by referring to the preceding content.

For the complete sample of the LeakyRelu operator implementation, click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op). For the complete sample of ONNX framework call, click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/00_framework/02_onnx).

1.  Obtain the ONNX framework network model by running the following command. As an example, this model contains only one LeakyRelu operator.

    ```
    wget https://obs-9be7.obs.cn-east-2.myhuaweicloud.com/AscendC/leaky_relu.onnx
    ```

2.  Run the following command to generate an offline model. (The directories and files used in the following command are examples. Use the actual ones in your code.)

    ```
    atc --model=$HOME/module/leaky_relu.onnx --framework=5 --soc_version=<soc_version> --output=$HOME/module/out/leaky_relu --input_shape="X:8,16,1024" --input_format=ND
    ```

    The key parameters are described as follows:

    -   **--model**: path of the ONNX framework network model file (\*.onnx).
    -   **--framework**: type of the original framework. **5** indicates ONNX.
    -   **--output**: path and file name of the converted offline model. Note that you need to record the path where the OM model file is saved, which is required for subsequent application development.
    -   **--soc\_version**: model of the AI processor.

    -   **--input\_shape**: shape of the model input data. Set it based on the shape range supported by the operator and the actual usage scenario. Here, the input X is set to a fixed shape \[8,16,1024\].
    -   **--input\_format**: format of the model input data. Set it based on the formats supported by the operator and the actual usage scenario. Here, it is set to ND.

3.  Run **export ASCEND\_GLOBAL\_LOG\_LEVEL=1** to change the log level to **INFO**. If the following information is displayed, the Ascend C custom operator compilation process has been entered and the model conversion is successful.

    ```
    ...
    start compile Ascend C operator LeakyReluCustom. kernel name is leaky_relu_custom
    compile Ascend C operator: LeakyReluCustom success!
    ...
    ATC run success
    ```

    After the command is successfully executed, you can view the offline model (for example, leaky\_relu.om) in the path specified by the **--output** parameter.

4.  Load the model and perform inference in the application by calling APIs such as **aclmdlExecute**.
