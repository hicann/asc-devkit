# Operator Implementation<a name="ZH-CN_TOPIC_0000002532228177"></a>

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-09T10:29:01.156Z pushedAt=2026-11-19T06:32:19.730Z nl=2 -->

The following uses the implementation of the Matmul+LeakyRelu fused operator as an example to describe the design and implementation process of a Mix fused operator.

The operator design process consists of three parts: **operator analysis, data flow analysis, and tiling strategy design**.

## Operator Analysis<a name="zh-cn_topic_0000001644252364_section161962036133819"></a>

Operator analysis refers to clarifying the mathematical expression, input, and output of the operator, as well as the name of the kernel function.

1.  Clarify the mathematical expression and computation logic of the operator. The computation logic of this operator is to first perform a cube multiplication, and then perform a LeakyRelu operation on the cube multiplication result with an alpha parameter. The mathematical expression is as follows:

    ```
    c = LeakyRelu(a * b + bias, alpha);
    ```

2.  Clarify the input and output.
    -   The Matmul+LeakyRelu operator input consists of a, b, and bias, and the output is c. alpha, as the coefficient of the LeakyRelu activation function, is a fixed value and can be directly used as a constant in the operator implementation.
    -   In this example, the data type supported by operator inputs a and b is half (float16), the data type supported by operator input bias is float32, and the data type of operator output c is float32.
    -   Input cube a has the shape of \[M, K\], input cube b has the shape of \[K, N\], output cube c has the shape of \[M, N\], and input bias has the shape of \[1, N\].
    -   The data format supported by the operator input and output is ND.

3.  Determine the name and parameters of the kernel function.
    -   You can customize the kernel function name. In this example, the kernel function is named **matmul\_leakyrelu\_custom**.
    -   Based on the analysis of the operator input and output, the parameters of the kernel function are determined as a, b, bias, and c. a, b, and bias are the memory addresses of the inputs in Global Memory, and c is the memory address of the output in Global Memory.

Based on the preceding analysis, the design specifications of the Ascend C Matmul+LeakyRelu operator are as follows:

-   Operator type (OpType): **MATMUL\_LEAKYRELU**
-   Operator input/output:

    **Table 1**  MATMUL\_LEAKYRELU operator input and output specifications<a name="table4934296305"></a>

    <table><thead align="left"><tr id="row59358913304"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p5503181819300"><a name="p5503181819300"></a><a name="p5503181819300"></a><strong id="b1850331853010"><a name="b1850331853010"></a><a name="b1850331853010"></a>Name</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1550381833017"><a name="p1550381833017"></a><a name="p1550381833017"></a><strong id="b7503171811309"><a name="b7503171811309"></a><a name="b7503171811309"></a>Shape</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1950391883014"><a name="p1950391883014"></a><a name="p1950391883014"></a><strong id="b2503111803020"><a name="b2503111803020"></a><a name="b2503111803020"></a>Data Type</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p14503218133015"><a name="p14503218133015"></a><a name="p14503218133015"></a><strong id="b8503141818301"><a name="b8503141818301"></a><a name="b8503141818301"></a>Format</strong></p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1950331810308"><a name="p1950331810308"></a><a name="p1950331810308"></a>a (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p534594615339"><a name="p534594615339"></a><a name="p534594615339"></a>[M, K]</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p342205120334"><a name="p342205120334"></a><a name="p342205120334"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p19503131815305"><a name="p19503131815305"></a><a name="p19503131815305"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p75031182305"><a name="p75031182305"></a><a name="p75031182305"></a>b (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p8345164693317"><a name="p8345164693317"></a><a name="p8345164693317"></a>[K, N]</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p3423513338"><a name="p3423513338"></a><a name="p3423513338"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503918103012"><a name="p1503918103012"></a><a name="p1503918103012"></a>ND</p>
    </td>
    </tr>
    <tr id="row19393173583315"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p6393235143314"><a name="p6393235143314"></a><a name="p6393235143314"></a>bias (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p2345346163314"><a name="p2345346163314"></a><a name="p2345346163314"></a>[1, N]</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p24285119339"><a name="p24285119339"></a><a name="p24285119339"></a>float32</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p173947358335"><a name="p173947358335"></a><a name="p173947358335"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1450316186305"><a name="p1450316186305"></a><a name="p1450316186305"></a>c (output)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p143457461338"><a name="p143457461338"></a><a name="p143457461338"></a>[M, N]</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p542175112332"><a name="p542175112332"></a><a name="p542175112332"></a>float32</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503101813012"><a name="p1503101813012"></a><a name="p1503101813012"></a>ND</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Kernel name: **matmul\_leakyrelu\_custom**

## Data Flow Analysis<a name="zh-cn_topic_0000001644252364_section1611503193514"></a>

Analyze the data flow of the operator: the data flows from the Cube core, where the Matmul computation is completed, to the Vector core, where the LeakyRelu computation is performed. Based on the preceding data flow and the fused operator programming paradigm, plan the parallel pipeline tasks, as shown in the following figure:

![](../../../../figures/add_op_47.png)

1.  Copy the input data from Global Memory to the Cube core.
2.  Perform the internal Matmul computation. The computation formula and diagram are as follows:

    Note: bias has the shape of \[1, N\], and this bias is applied to each row of the A\*B result cube.

    **Figure 1**  Matmul cube multiplication diagram<a name="zh-cn_topic_0000001644252364_fig194600231317"></a>  
    ![](../../../../figures/mm_diag_48.png "Matmul cube multiplication diagram-48")

3.  Copy the Matmul computation result to the Vector core.
4.  Perform vector computation. In this example, LeakyReLU computation is performed.

    Leaky ReLU (leaky rectified linear unit) is a commonly used activation function in artificial neural networks. Its mathematical expression and function graph are as follows:

    ![](../../../../figures/formulaimage_0000002501251774.png)

    ![](../../../../figures/image_0000002501411624.png)

5.  Copy the output result to Global Memory.

The content of the first three steps is encapsulated in the Matmul advanced API. In this example, it can be simplified into three stages, as shown in the following figure:

![](../../../../figures/prog_paradigm.png)

Based on the preceding analysis, the implementation uses the [Matmul advanced API](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md), [LeakyRelu vector compute API](../../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/LeakyRelu.md), [DataCopy](../../../../../api/SIMD-API/basic_api/data_move_guide/overview/data_move_concept.md), [EnQue](../../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md), and [DeQue](../../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) APIs.

## Tiling Strategy Design<a name="zh-cn_topic_0000001644252364_section317744643914"></a>

The tiling strategy design mainly includes multi-core splitting and intra-core splitting strategies.

-   Multi-core splitting: Based on the current number of cores, split M, K, and N of the input shape across multiple cores to obtain the single-core shape sizes singleCoreM, singleCoreK, and singleCoreN.
-   Intra-core splitting: Based on the size constraint of Local Memory, further split the single-core shape size to obtain the shape sizes baseM, baseN, and baseK of cubes A, B, and C that participate in one cube multiplication instruction. Note the following during splitting: If the result of **GetTensorC** is placed in the Unified Buffer (UB), the size of baseM \* baseN cannot exceed the UB limit.

The following figure shows the splitting strategy. For more details about splitting strategies, see [Data Tiling](../../matrix_advanced_api/basics.md).

![](../../../../figures/prog_paradigm_49.png)

## Operator Implementation<a name="zh-cn_topic_0000001644252364_section770213024816"></a>

In the [cube programming chapter](../../matrix_advanced_api/matrix_advanced_api.md), we learned that Ascend C provides a set of advanced Matmul APIs that encapsulate common tiling, data movement, and computation algorithm logic, making it convenient for users to quickly implement Matmul cube multiplication operations. The cube programming part of a fused operator is implemented similarly. On the host side, the developer obtains the tiling parameters by calling APIs. After these parameters are passed to the kernel function side, they are passed in during initialization, and the cube multiplication operation can be completed with a few simple APIs. Combined with the fused operator programming paradigm described above, the steps for implementing a fused operator are as follows. For the complete example, see [MatmulLeakyRelu](../../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api).

![](../../../../figures/mm_flow_50.png)

The code framework implemented on the kernel function side is as follows. After initializing the Matmul object and setting the left cube A, right cube B, and Bias, the subsequent Matmul computation, LeakyRelu computation, and CopyOut process are completed through a single **Iterate** call combined with a **while** loop.

```
// Kernel function entry, mix scenario, AIC:AIV=1:2
__global__ __mix__(1, 2) void matmul_leakyrelu_custom(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* bias,
    __gm__ uint8_t* c, __kfc_workspace__ __gm__ uint8_t* workspace, AscendC::tiling::TCubeTiling tiling)
{
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> mmOutQueue;
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> reluOutQueue;
    // Initialize the Matmul object.
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    // Set the Matmul inputs (including the left cube, right cube, and bias).
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);
    // Call matmul iterate to obtain a block of [baseM, baseN] computation results.
    uint32_t computeRound = 0;
    while (matmulObj.template Iterate<true>())
    {
        // Matmul computation: obtain the Matmul intermediate result;
        // LeakyRelu computation: perform vector computation on the Matmul intermediate result;
        // CopyOut: Move the fused computation result to the GM.
        computeRound++;
    }
    matmulObj.End();
}
```

The specific implementation code of Matmul computation, LeakyRelu computation, and CopyOut is as follows:

1.  Matmul computation:
    1.  Perform the internal Matmul computation on the Cube core.
    2.  Copy the Matmul computation result to the Vector core.

        ```
        // ...
        // Call matmul iterate to obtain a computation result block of [baseM, baseN].
        while (matmulObj.template Iterate<true>())
        {
            AscendC::LocalTensor<float> mmOutLocal = mmOutQueue.AllocTensor<float>();
            // Obtain the Matmul result into the LocalTensor on the vector side. This tensor is then used as the input of LeakyRelu.
            matmulObj.template GetTensorC<true>(mmOutLocal, false, true);
            // Enqueue the Matmul intermediate result to establish the data dependency between the Matmul result write and the subsequent LeakyRelu read.
            mmOutQueue.EnQue(mmOutLocal);
            // ...
    
        }
        matmulObj.End();
        ```

2.  LeakyRelu computation:

    ```
    // Call DeQue to dequeue the Matmul intermediate result from the VECIN queue for LeakyRelu to read.
    mmOutLocal = mmOutQueue.DeQue<float>();
    // Call the LeakyRelu API for computation.
    AscendC::LocalTensor<float> reluOutLocal = reluOutQueue.AllocTensor<float>();
    AscendC::LeakyRelu(reluOutLocal, mmOutLocal, static_cast<float>(alpha), tiling.baseM * tiling.baseN);
    reluOutQueue.EnQue(reluOutLocal);
    ```

3.  CopyOut: move the output result to Global Memory.

    ```
    // Move the result out to GM.
    reluOutLocal = reluOutQueue.DeQue<float>();
    // LeakyRelu has finished reading mmOutLocal. Release the Matmul intermediate result buffer in the VECIN queue.
    mmOutQueue.FreeTensor(mmOutLocal);
    // Each Iterate generates one baseM * baseN output block, which is written back to the corresponding position in the current C cube tile in each round.
    // For UB->GM, call the DataCopy API with the DataCopyParams struct parameter: {count, len, srcStrideIn, dstStrideIn}.
    AscendC::DataCopy(cGlobal[computeRound * tiling.baseM * tiling.N], reluOutLocal,
        AscendC::DataCopyParams{static_cast<uint16_t>(tiling.baseM),
            static_cast<uint16_t>(tiling.baseN * sizeof(float) / AscendC::DEFAULT_C0_SIZE), 0,
            static_cast<uint16_t>((tiling.N - tiling.baseN) * sizeof(float) / AscendC::DEFAULT_C0_SIZE)});
    // Release the LocalTensor buffer so that this queue buffer can be reused by the next Iterate round.
    reluOutQueue.FreeTensor(reluOutLocal);
    ```

The host side implements the **GenerateTiling** function, which automatically obtains the tiling parameters. The key steps are described as follows:

1.  **Create a tiling object.**

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    matmul_tiling::MultiCoreMatmulTiling tilingApi(ascendcPlatform);
    ```

    When creating the object, pass in the hardware platform information, which can be obtained through **GetInstance**.

2.  **Set the data types and formats of A, B, and Bias.**

    The following is an example of the settings. For details about **TPosition**, see [TPosition](../../../../../api/SIMD-API/basic_api/aux_data_structures/TPosition.md).

    ```
    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT16, false);
    tilingApi.SetBType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT16, false);
    tilingApi.SetCType(TPosition::VECIN, CubeFormat::ND, DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT);
    ```

3.  **Set the cube shape information.**

    ```
    tilingApi.SetOrgShape(M, N, K);
    tilingApi.SetShape(M, N, K);
    ```

4.  **Set the available space size information.**

    Set the L1 Buffer/L0C Buffer/UB space available for Matmul computation. **-1** indicates the size of the corresponding buffer on the AI processor. In the fusion scenario, LeakyRelu requires a UB cache of `baseM * baseN * sizeof(float)`, so a portion of the UB space can be deducted based on the cache requirement on the vector side.

    ```
    uint64_t ubSize = 0;
    ascendcPlatform->GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    uint64_t usedUb = static_cast<uint64_t>(baseM) * baseN * sizeof(float);
    tilingApi.SetBufferSpace(-1, -1, ubSize - usedUb);
    ```

5.  **Set other parameters as required, for example, enable bias to participate in the computation and set the Matmul computation direction to M-axis first.**

    ```
    tilingApi.SetBias(true);
    tilingApi.SetTraverse(matmul_tiling::MatrixTraverse::FIRSTM);
    ```

6.  **Obtain the tiling parameters.**

    ```
    AscendC::tiling::TCubeTiling tilingData;
    int64_t res = tilingApi.GetTiling(tilingData);
    if (res == -1) {
        std::cout << "gen tiling failed" << std::endl;
    }
    ```

7.  Perform other operations such as serializing and saving the tiling parameters.

>[!NOTE] Note <a name="zh-cn_topic_0000001644252364_li4790115115920"></a>
>- In particular, for the multi-core scenario, use the [SetDim](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDim.md) API to set the number of cores used for Matmul computation. The setting rules for the MIX mode (including cube computation and vector computation) are as follows:
>    - [Decoupled mode](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#li188191010204418): The Matmul API is initiated from the AIV side. When **Iterate** is called for computation, it only serves as a notification on the AIV side, notifying the AIC to perform cube computation. After the computation is complete, the AIC notifies the AIV that the computation is complete. In this architecture, **SetSimdNumBlocks** is set to the number of AI Cores (AIC and AIV combinations) actually used for computation, and **SetDim** is set to the number of AIVs actually used for computation. For example, **SetSimdNumBlocks** can be set to **20** to start 20 AI Cores (AIC and AIV combinations), and **SetDim** is set to **40**, indicating that the computation is split across 40 AIVs.
>    - [Coupled mode](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#li1414517184416): The number of cores loaded by **SetSimdNumBlocks** is the number of cores actually used for Matmul API computation, and the values set for **SetDim** and **SetSimdNumBlocks** are the same.
>- The advanced Matmul API requires the system workspace during its internal implementation. Developers need to:
>    - When implementing tiling on the host side, set the total workspace size (including the user workspace and the system workspace). The workspace is allocated and managed by the framework. The system workspace size is obtained through [GetLibApiWorkSpaceSize](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md).
>        ```
>        size_t userWorkspaceSize = 0;
>        size_t systemWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
>        size_t workspaceSize = userWorkspaceSize + systemWorkspaceSize;
>        ```
>- In the implementation method described above, the code isolation and inter-core synchronization between the AIC side and the AIV side are completed by the framework, and developers do not need to care about them. In addition to this method, developers can also choose the low-level coding method to implement the fused operator in decoupled mode, which is more flexible. When using the low-level coding method, note the following:
>    - Use [ASCEND\_IS\_AIV and ASCEND\_IS\_AIC](../../../../programming_guide/language_extension/simd_builtin_keywords.md#li19530175294118) to isolate the AIV and AIC code.
>    - Implement the synchronization between the AIC and AIV cores by yourself: for example, in the Matmul + LeakyRelu operator example, ensure that the AIV performs the LeakyRelu computation only after the AIC completes the cube computation.
>    - When using the advanced Matmul API, set **ASCENDC\_CUBE\_ONLY** to indicate that the Matmul API is called only on the AIC side.
>
>    For the complete example, see the [matmul_fused_manual example](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_fused_manual).
>
>    ```
>    #define ASCENDC_CUBE_ONLY // Specify that Matmul runs on the AIC.
>    if ASCEND_IS_AIC {
>        ...
>        // The AIC core performs Matmul computation.
>        // After the AIC core completes the computation, send a synchronization flag through AscendC::CrossCoreSetFlag<modeId, pipe>(flagId).
>    }
>    if ASCEND_IS_AIV {
>        ...
>        // The AIV core receives the synchronization flag through AscendC::CrossCoreWaitFlag<modeId>(flagId).
>        // The AIV core performs LeakyRelu computation.
>    } 
>    ```
>
