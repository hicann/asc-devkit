# PyTorch Framework<a name="ZH-CN_TOPIC_0000001918449130"></a>

<!-- md-trans-meta sourceCommit=4d9fe40502b3423a344d570fd1a8be343cfca4a5 translatedAt=2026-10-23T14:11:54.025Z pushedAt=2026-11-03T07:05:13.419Z nl=2 -->

When training and inferring models through the PyTorch framework, many operators are invoked for computation. If developers need to integrate and deploy their custom operators into the PyTorch framework, the following methods are available:

-   Kernel function direct invocation: By adapting torch.library or Pybind to register custom operators, the PyTorch framework can invoke operator kernel function programs.
-   Single-operator API invocation: For the adaptation plugin development process and specific examples in this mode, see [OpPlugin-Based Operator Adaptation Development](https://gitcode.com/Ascend/pytorch/blob/v2.14.0-26.2.0/docs/en/developer_notes/custom_operator_adaptation/opplugin_operator_adaptation/adaptation_overview_opplugin.md).
-   Graph mode invocation: For guidance on adapting custom operators in PyTorch graph mode, see [Custom Operator Graph Integration](https://gitcode.com/Ascend/torchair/blob/26.2.0/docs/zh/custom_op_graph/overview.md).

**Figure 1**  PyTorch framework deployment methods<a name="fig1969201074516"></a>  
![](../../../figures/pytorch_deploy.png "PyTorch framework deployment methods")

**This section mainly provides guidance on registering custom operators through torch.library and Pybind and implementing PyTorch framework invocation of operator kernel function programs.**

-   torch.library is a collection of APIs for extending the PyTorch core operator library. It allows developers to create new operators and provide custom implementations for them.
-   Pybind is an open-source bridging tool between C++ and Python, designed to seamlessly integrate C++ code into the Python environment.

Pybind is suitable for quickly exposing C++ functions to Python and implementing efficient API binding. However, the operators it generates cannot be recognized by the PyTorch operator system, and lack schema definition and graph tracing capabilities, so they do not support torch.compile optimization. In contrast, torch.library provides a mechanism for deep integration with the PyTorch core operator system, supporting operator registration, schema definition, and graph tracing capabilities, which is a prerequisite for supporting torch.compile. Developers can choose the appropriate method based on their requirements.

## torch.library<a name="section9528173218278"></a>

The following code uses add\_custom (the Add custom operator) as an example to describe how to call the operator kernel function program through torch.library. This document describes only the core steps. For the complete sample, see the [torch.library sample](../../../../../../examples/01_simd_cpp_api/02_features/00_framework/00_pytorch/torch_library).

1.  Set up the environment.

    In addition to installing the CANN software package as described in [environment setup](../../../getting_started/environment_setup.md), you also need to install the following dependencies:

    [TorchNPU installation](https://www.hiascend.com/developer/software/ai-frameworks/pytorch/download)

2.  Implement the custom operator on the NPU.

    This includes the operator kernel function implementation, and uses the <<<\>\>\> API to call the operator kernel function to complete the specified computation. The [c10\_npu::getCurrentNPUStream](https://gitcode.com/Ascend/op-plugin/blob/26.2.0/docs/en/custom_APIs/cpp/(beta)c10_npu-getCurrentNPUStream.md) API in the sample is used to obtain the current NPU stream, and its return value type is NPUStream.

    Note that the memory for the inputs x and y in this sample is allocated in the outer Python call script.

    ```
    namespace ascendc_ops {
    at::Tensor ascendc_add(const at::Tensor& x, const at::Tensor& y)
    {
        // Allocate runtime resources, and obtain the stream on the current NPU through the c10_npu::getCurrentNPUStream() function.
        auto aclStream = c10_npu::getCurrentNPUStream().stream(false);
        // Allocate the output memory on the Device side.
        at::Tensor z = at::empty_like(x);
        uint32_t numBlocks = 8;
        uint32_t totalLength = 1;
        for (uint32_t size : x.sizes()) {
            totalLength *= size;
        }
        // Use the <<<>>> API to call the kernel function to complete the specified computation.
        add_custom<<<numBlocks, 0, aclStream>>>((uint8_t*)(x.mutable_data_ptr()), (uint8_t*)(y.mutable_data_ptr()), (uint8_t*)(z.mutable_data_ptr()), totalLength);
        // Copy the computation result from the Device back to the Host and release the applied resources.
        return z;
    }
    } // namespace ascendc_ops
    ```

3.  Register the custom operator.

    PyTorch provides the **TORCH\_LIBRARY** macro as the core API for registering custom operators. It is used to create and initialize a custom operator library. After registration, the operator can be called on the Python side through **torch.ops.namespace.op\_name**. **TORCH\_LIBRARY\_IMPL** is used to bind the operator logic to a specific DispatchKey (a PyTorch device dispatch identifier). For NPU devices, the operator implementation must be registered to PrivateUse1, a dedicated DispatchKey.

    ```
    // Register the operator with torch.library.
    TORCH_LIBRARY(ascendc_ops, m)
    {
        m.def("ascendc_add(Tensor x, Tensor y) -> Tensor");
    }
    
    // Register the PrivateUse1 implementation for the NPU device.
    TORCH_LIBRARY_IMPL(ascendc_ops, PrivateUse1, m)
    {
        m.impl("ascendc_add", TORCH_FN(ascendc_ops::ascendc_add));
    }
    ```

4.  Compile and generate the dynamic operator library.
5.  Test using a Python test script.

    In **add\_custom\_test.py**, first load the generated custom operator library through **torch.ops.load\_library**, call the registered ascendc\_add function, and verify the numerical correctness of the custom operator by comparing the NPU output with the standard CPU addition result.

## Pybind<a name="section1788413437123"></a>

The following code uses the add\_custom operator as an example to introduce the process of calling a custom operator in a PyTorch script through Pybind. This document only introduces the core steps. For the complete sample, see the [Pybind sample](../../../../../../examples/01_simd_cpp_api/02_features/00_framework/00_pytorch/pybind).

1.  Set up the environment.

    In addition to installing the CANN software package as described in [Environment Preparation](../../../getting_started/environment_setup.md), you also need to install the following dependencies:

    - [TorchNPU installation](https://www.hiascend.com/developer/software/ai-frameworks/pytorch/download)

    -   Install pybind11.

        ```
        pip3 install pybind11 expecttest
        ```

2.  Implement the custom operator on the NPU.

    This includes the operator kernel function implementation, and uses the <<<\>\>\> API to call the operator kernel function to complete the specified operation. The [c10\_npu::getCurrentNPUStream](https://gitcode.com/Ascend/op-plugin/blob/26.2.0/docs/en/custom_APIs/cpp/(beta)c10_npu-getCurrentNPUStream.md) API in the sample is used to obtain the current NPU stream, and the return value type is NPUStream.

    Note that the memory for the input x and y in this sample is allocated in the Python call script.

    ```
    // Header files required for Pybind and PyTorch invocation
    #include <pybind11/pybind11.h>
    #include <torch/extension.h>
    
    #include "torch_npu/csrc/core/npu/NPUStream.h"
    // Header files required for the kernel function implementation
    #include "kernel_operator.h" 
    ...
    namespace ascendc_ops {
    at::Tensor ascendc_add(const at::Tensor& x, const at::Tensor& y)
    {
        // Allocate runtime resources, and obtain the stream on the current NPU through the c10_npu::getCurrentNPUStream() function
        auto aclStream = c10_npu::getCurrentNPUStream().stream(false);
        // Allocate the output memory on the Device side
        at::Tensor z = at::empty_like(x);
        uint32_t numBlocks = 8;
        uint32_t totalLength = 1;
        for (uint32_t size : x.sizes()) {
            totalLength *= size;
        }
        // Call the kernel function through the <<<>>> API to complete the specified operation
        add_custom<<<numBlocks, 0, aclStream>>>((uint8_t*)(x.mutable_data_ptr()), (uint8_t*)(y.mutable_data_ptr()), (uint8_t*)(z.mutable_data_ptr()), totalLength);
        // Copy the operation result from the Device back to the Host and release the allocated resources.
        return z;
    }
    } // namespace ascendc_ops
    ```

3.  Define a Pybind module to encapsulate the C++ function into a Python function. PYBIND11\_MODULE is a macro in the Pybind11 library used to define a Python module. It accepts two parameters: the first is the name of the encapsulated module, and the second is a Pybind11 module object used to define functions, classes, constants, and so on in the module. By calling the **m.def\(\)** method, you can convert the function ascendc\_ops::ascendc\_add from the previous step into the Python function ascendc\_add so that it can be called in Python code.

    ```
    PYBIND11_MODULE(ascendc_ops, m)// Module name ascendc_ops, module object m
    {
        m.doc() = "add_custom pybind11 interfaces";// optional module docstring
        m.def("ascendc_add", &ascendc_ops::ascendc_add, "");// Bind the function ascendc_add to the Pybind module
    }
    ```

4.  Compile and generate the dynamic operator library.
5.  In the Python call script, use the torch API to generate random input data and allocate memory. Import the encapsulated custom module ascendc\_ops and call the ascendc\_add function in the custom module ascendc\_ops to execute the operator on the NPU.
