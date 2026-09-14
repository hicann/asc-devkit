# PyTorch Framework<a name="ZH-CN_TOPIC_0000001918449130"></a>

<!-- md-trans-meta sourceCommit=dc56b6592be9a41ee18c3bfdf6bd8116a9020128 translatedAt=2026-08-26T12:07:54.742Z pushedAt=2026-09-03T08:12:02.021Z -->

When training and inferring models through the PyTorch framework, many operators are invoked for computation. If developers need to integrate and deploy their custom operators into the PyTorch framework, the following methods are available:

-   Kernel direct invocation: By adapting torch.library or Pybind to register custom operators, the PyTorch framework can invoke operator kernel programs.

-   Single-operator API invocation: For the adaptation plugin development process and specific examples in this mode, see the "Development Guide > Framework Features > Custom Operator Adaptation Development > Operator Adaptation Development Based on OpPlugin" section in [*TorchNPU*](https://www.hiascend.com/document/detail/en/Pytorch/2610/index/index.html).

-   Graph mode call: For the adaptation development guide of custom operators in PyTorch graph mode, see the custom operator graph development under "TorchAir" in [*TorchNPU*](https://www.hiascend.com/document/detail/en/Pytorch/2610/index/index.html).

**Figure 1**  PyTorch framework deployment methods<a name="fig1969201074516"></a>  
![](../../../figures/pytorch_framework_deployment_method.png "PyTorch framework deployment methods")

**This section mainly provides guidance on registering custom operators through torch.library and Pybind and implementing PyTorch framework call of operator kernel programs.**

-   torch.library is a collection of APIs for extending the PyTorch core operator library. It allows developers to create new operators and provide custom implementations for them.

-   Pybind is an open-source bridging tool between C++ and Python, designed to seamlessly integrate C++ code into the Python environment.

Pybind is suitable for quickly exposing C++ functions to Python and implementing efficient API binding. However, the operators it generates cannot be recognized by the PyTorch operator system, and lack schema definition and graph tracing capabilities, so they do not support torch.compile optimization. In contrast, torch.library provides a mechanism for deep integration with the PyTorch core operator system, supporting operator registration, schema definition, and graph tracing capabilities, which is a prerequisite for supporting torch.compile. Developers can choose the appropriate method based on their requirements.

## torch.library<a name="section9528173218278"></a>

The following code uses add\_custom (the Add custom operator) as an example to describe how to call the operator kernel program through torch.library. This document describes only the core steps. For the complete sample, see the [torch.library sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/00_framework/00_pytorch/torch_library).

1.  Set up the environment.

    In addition to installing the CANN software package as described in [Environment Setup](../../../getting_started/environment_setup.md), install the following dependencies:

    [Install TorchNPU](https://www.hiascend.com/document/detail/en/Pytorch/2610/configandinstg/instg/docs/en/installation_guide/installation_description.md).

2.  Implement the custom operator on the NPU.

    This includes implementing the operator kernel and using the <<<\>\>\> API to call the operator kernel function to complete the specified computation. In the sample, the **c10\_npu::getCurrentNPUStream** API is used to obtain the current NPU stream, and its return value type is NPUStream. For usage, see "API > Custom APIs" in [*TorchNPU*](https://www.hiascend.com/document/detail/en/Pytorch/2610/index/index.html).

    Note that the memory for the inputs x and y in this sample is allocated in the outer Python call script.

    ```
    namespace ascendc_ops {
    at::Tensor ascendc_add(const at::Tensor& x, const at::Tensor& y)
    {
        // Allocate runtime resources and obtain the stream on the current NPU through the c10_npu::getCurrentNPUStream() function.
        auto aclStream = c10_npu::getCurrentNPUStream().stream(false);
        // Allocate the output memory on the device side.
        at::Tensor z = at::empty_like(x);
        uint32_t numBlocks = 8;
        uint32_t totalLength = 1;
        for (uint32_t size : x.sizes()) {
            totalLength *= size;
        }
        // Call the kernel function through the <<<>>> API to complete the specified operation
        add_custom<<<numBlocks, 0, aclStream>>>((uint8_t*)(x.mutable_data_ptr()), (uint8_t*)(y.mutable_data_ptr()), (uint8_t*)(z.mutable_data_ptr()), totalLength);
        // Copy the operation result from the device back to the host and release the allocated resources.
        return z;
    }
    } // namespace ascendc_ops
    ```

3.   Register the custom operator.

    PyTorch provides the **TORCH\_LIBRARY** macro as the core API for registering custom operators. It is used to create and initialize a custom operator library. After registration, the operator can be called on the Python side through **torch.ops.namespace.op\_name**. **TORCH\_LIBRARY\_IMPL** is used to bind the operator logic to a specific DispatchKey (a PyTorch device dispatch identifier). For NPU devices, the operator implementation must be registered to PrivateUse1, a dedicated DispatchKey.

    ```
    // Register the operator to torch.library.
    TORCH_LIBRARY(ascendc_ops, m)
    {
        m.def("ascendc_add(Tensor x, Tensor y) -> Tensor");
    }
    
    // Register the PrivateUse1 implementation for NPU devices.
    TORCH_LIBRARY_IMPL(ascendc_ops, PrivateUse1, m)
    {
        m.impl("ascendc_add", TORCH_FN(ascendc_ops::ascendc_add));
    }
    ```

4.   Compile and generate the dynamic operator library.

5.   Test using a Python test script.

    In **add\_custom\_test.py**, first load the generated custom operator library through **vtorch.ops.load\_library**, call the registered ascendc\_add function, and verify the numerical correctness of the custom operator by comparing the NPU output with the standard CPU addition result.

## Pybind<a name="section1788413437123"></a>

The following code uses the add\_custom operator as an example to introduce the process of calling a custom operator in a PyTorch script through Pybind. This document only introduces the core steps. For the complete sample, see the [Pybind sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/00_framework/00_pytorch/pybind).

1. Set up the environment.

    In addition to installing the CANN software package as described in [Environment Setup](../../../getting_started/environment_setup.md), you also need to install the following dependencies:

    - [Install TorchNPU](https://www.hiascend.com/document/detail/en/Pytorch/2610/configandinstg/instg/docs/en/installation_guide/installation_description.md).

    -   Install pybind11.

        ```
        pip3 install pybind11 expecttest
        ```

2. Implement the custom operator on the NPU.

    This includes implementing the operator kernel and using the <<<\>\>\> API to call the kernel function to complete the specified operation. The **c10\\_npu::getCurrentNPUStream** API in the sample is used to obtain the current NPU stream, and its return value type is NPUStream. For usage, see "API > Custom APIs" in [TorchNPU](https://www.hiascend.com/document/detail/en/Pytorch/2610/index/index.html).

    Note that the memory for the input x and y in this sample is allocated in the Python call script.

    ```
    // Header files required for Pybind and PyTorch calls
    #include <pybind11/pybind11.h>
    #include <torch/extension.h>
    
    #include "torch_npu/csrc/core/npu/NPUStream.h"
    // Header file required for the kernel-side implementation
    #include "kernel_operator.h" 
    ...
    namespace ascendc_ops {
    at::Tensor ascendc_add(const at::Tensor& x, const at::Tensor& y)
    {
        // Allocate runtime resources and obtain the stream on the current NPU through the c10_npu::getCurrentNPUStream() function.
        auto aclStream = c10_npu::getCurrentNPUStream().stream(false);
        // Allocate the output memory on the device side.
        at::Tensor z = at::empty_like(x);
        uint32_t numBlocks = 8;
        uint32_t totalLength = 1;
        for (uint32_t size : x.sizes()) {
            totalLength *= size;
        }
        // Call the kernel function through the <<<>>> API to complete the specified operation.
        add_custom<<<numBlocks, 0, aclStream>>>((uint8_t*)(x.mutable_data_ptr()), (uint8_t*)(y.mutable_data_ptr()), (uint8_t*)(z.mutable_data_ptr()), totalLength);
        // Copy the operation result from the device back to the host and release the allocated resources.
        return z;
    }
    } // namespace ascendc_ops
    ```

3. Define a Pybind module to encapsulate the C++ function into a Python function. PYBIND11\_MODULE is a macro in the Pybind11 library used to define a Python module. It accepts two parameters: the first is the name of the encapsulated module, and the second is a Pybind11 module object used to define functions, classes, constants, and so on in the module. By calling the **m.def\(\)** method, you can convert the function ascendc\_ops::ascendc\_add from the previous step into the Python function ascendc\_add so that it can be called in Python code.

    ```
    PYBIND11_MODULE(ascendc_ops, m)// Module name ascendc_ops, module object m
    {
        m.doc() = "add_custom pybind11 interfaces";// optional module docstring
        m.def("ascendc_add", &ascendc_ops::ascendc_add, "");// Bind the function ascendc_add to the Pybind module.
    }
    ```

4. Compile and generate the dynamic operator library.

5. In the Python call script, use the torch API to generate random input data and allocate memory. Import the encapsulated custom module ascendc\_ops and call the run\_add\_custom function in the custom module ascendc\_ops to execute the operator on the NPU.