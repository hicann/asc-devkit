# How to Develop a Dynamic Input Operator<a name="ZH-CN_TOPIC_0000002098772922"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-26T11:15:14.225Z pushedAt=2026-08-31T11:23:39.602Z -->

A dynamic input operator is an operator whose number of inputs is dynamic. For example, AddN accumulates N input tensors into one output tensor, and the number of input tensors is not fixed. The development of a dynamic input operator differs in constructing and parsing input data: the kernel function uses the ListTensorDesc structure to store input data information, and correspondingly, a TensorList structure must be constructed to store parameter information when the operator is invoked. The following describes the specific development process based on two development modes: kernel launch and engineering-based operator development.

>[!NOTE] Note
>Only code snippets are listed below. For the complete sample, see [Dynamic Input Operator Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/04_memory_management/list_tensor_desc_input).

-   Kernel launch
    -   Define the ListTensorDesc and TensorDesc structures by referring to the [ListTensorDesc](../../../../api/SIMD-API/basic_api/aux_data_structures/ListTensorDesc.md) data structure, and save the actual input data to the ListTensorDesc structure. The example is as follows:

        ptrOffset is passed in as the offset between the first address of ListTensorDesc and the first address dataPtr of the data pointer. tensorDesc stores the tensor description information of the two inputs, and dataPtr is passed in as the address pointer that stores the input data.

        ```
            constexpr uint32_t SHAPE_DIM = 2;
            struct TensorDesc {
                uint32_t dim{SHAPE_DIM};
                uint32_t index;
                uint64_t shape[SHAPE_DIM] = {8, 2048};
            };
        
            TensorDesc xDesc;
            xDesc.index = 0;
            TensorDesc yDesc;
            yDesc.index = 1;
        
            constexpr uint32_t TENSOR_DESC_NUM = 2;
            struct ListTensorDesc {
                uint64_t ptrOffset;
                TensorDesc tensorDesc[TENSOR_DESC_NUM];
                uintptr_t dataPtr[TENSOR_DESC_NUM];
            } inputDesc;

            inputDesc = {(1 + (1 + SHAPE_DIM) * TENSOR_DESC_NUM) * sizeof(uint64_t), {xDesc, yDesc}, {(uintptr_t)xDevice, (uintptr_t)yDevice}};
        ```

    -   When the kernel side calls the operator, directly pass in the input information expressed by ListTensorDesc. The example is as follows:

        ```
            // Allocate ListTensorDesc device memory
            ret = aclrtMalloc((void **)&inputDescInDevice, sizeof(ListTensorDesc), ACL_MEM_MALLOC_HUGE_FIRST);
            assert(ret == ACL_SUCCESS && "aclrtMalloc inputDescInDevice failed!");
        
            // Copy ListTensorDesc to device
            ret = aclrtMemcpy(inputDescInDevice, sizeof(ListTensorDesc), &inputDesc, sizeof(ListTensorDesc),
                                ACL_MEMCPY_HOST_TO_DEVICE);
            assert(ret == ACL_SUCCESS && "aclrtMemcpy inputDesc to device failed!");
        
            list_tensor_desc_input_custom<<<numBlocks, 0, stream>>>(inputDescInDevice, zDevice, tiling);
        ```

    -   For the operator implementation on the kernel side, the input parameter must pass in the data of the dynamic structure (srcList), and the AscendC::ListTensorDesc structure is used for structure analysis. The example is as follows:

        ```
        __global__ __vector__ void list_tensor_desc_input_custom(GM_ADDR srcList, GM_ADDR z, AddCustomTilingData tiling)
        {
            AscendC::ListTensorDesc keyListTensorDescInit((__gm__ void*)srcList);
            GM_ADDR x = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(0);
            GM_ADDR y = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(1);
        }
        ```

-   Engineering-based operator development
    -   When a single operator is called, construct a list-type tensor and pass it in.

After creating tensors by calling aclCreateTensor, call aclCreateTensorList to combine the created tensors into a list. The example is as follows.

        ```
        inputTensorList = aclCreateTensorList(inputTensor_.data(), inputTensor_.size());
        ```

The input parameter of the API for obtaining the workspace size used by the operator must also use the aclTensorList structure parameter to calculate the workspace size. The calling example is as follows.

        ```
        // Obtain the workspace size used by the operator.
        aclnnStatus aclnnAddNCustomGetWorkspaceSize(const aclTensorList *srcList, const aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
        ```

    -   In the operator prototype definition, set the parameter type of the input data to dynamic. The example is as follows.

        ```
        this->Input("srcList")
            .ParamType(DYNAMIC)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND});
        ```

    -   In the host-side operator implementation, use the corresponding dynamic API to obtain dynamic input information.

        For example, in the tiling function and InferShape function, the GetDynamicInputShape API is used to obtain the shape information of the dynamic input. In the InferDataType function, the GetDynamicInputDataType API is used to obtain the data type of the dynamic input. The example is as follows.

        ```
        namespace ge {
        static graphStatus InferShape(gert::InferShapeContext *context)
        {
            const gert::Shape *x1_shape = context->GetDynamicInputShape(0, 0);
            gert::Shape *y_shape = context->GetOutputShape(0);
            *y_shape = *x1_shape;
            return GRAPH_SUCCESS;
        }
        
        static graphStatus InferDataType(gert::InferDataTypeContext *context)
        {
            const auto inputDataType = context->GetDynamicInputDataType(0, 0);
            context->SetOutputDataType(0, inputDataType);
            return ge::GRAPH_SUCCESS;
        }
        } // namespace ge
        ```

    -   In the kernel-side operator implementation, the input parameter must pass in the data of the dynamic structure, and the AscendC::ListTensorDesc structure is used for parsing.

        The kernel function input parameter must pass in the data of the dynamic structure, for example, GM\_ADDR srcList. The example is as follows.

        ```
        extern "C" __global__ __aicore__ void addn_custom(GM_ADDR srcList, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
        ```

        For the passed-in parameter srcList, use the AscendC::ListTensorDesc structure for structure analysis to obtain the specific information of each tensor. The example is as follows.

        ```
        AscendC::ListTensorDesc keyListTensorDescInit((__gm__ void*)srcList);
        GM_ADDR x = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(0);
        GM_ADDR y = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(1);
        ```
