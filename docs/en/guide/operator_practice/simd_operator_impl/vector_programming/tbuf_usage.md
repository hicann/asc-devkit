# Using TBuf<a name="ZH-CN_TOPIC_0000002500548088"></a>

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T12:25:49.841Z pushedAt=2026-11-03T07:05:13.315Z nl=2 -->

In most operator development scenarios, the kernel function computation process requires temporary memory to store intermediate results of operations. These intermediate results are represented by temporary variables, and the memory occupied by temporary variables can be managed using the TBuf data structure. For details, see [TBuf](../../../../api/SIMD-API/basic_api/resource_management/TBuf/TBuf.md). The following uses the Add operator, whose input data type is bfloat16\_t and which runs on a single core, as an example to describe how to use TBuf. For the complete code of the operator described in this document, see [Add Operator Sample Using Temporary Memory](../../../../../../examples/01_simd_cpp_api/03_basic_api/04_memory_management/tmp_buffer).

On Atlas A2 products, the [Add](../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md) API does not support summation of source operands of the bfloat16\_t data type. Therefore, the data type of the operator input must first be converted to a data type supported by the Add API before the computation is performed. To ensure computation precision, call the [Cast](../../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md) API to convert the input bfloat16\_t type to the float type, then perform the Add computation, and convert the float type back to the bfloat16\_t type after the computation is complete.

Based on the preceding analysis, the design specifications of the Ascend C Add operator are as follows:

-   Operator type (OpType): Add
-   Operator input and output:

    **Table 1**  Add operator input and output specifications

    <a name="table4934296305"></a>
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
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1950331810308"><a name="p1950331810308"></a><a name="p1950331810308"></a>x (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1950321863013"><a name="p1950321863013"></a><a name="p1950321863013"></a>(1, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1050613178322"><a name="p1050613178322"></a><a name="p1050613178322"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p19503131815305"><a name="p19503131815305"></a><a name="p19503131815305"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p75031182305"><a name="p75031182305"></a><a name="p75031182305"></a>y (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p16503111873010"><a name="p16503111873010"></a><a name="p16503111873010"></a>(1, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1350691713217"><a name="p1350691713217"></a><a name="p1350691713217"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503918103012"><a name="p1503918103012"></a><a name="p1503918103012"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1450316186305"><a name="p1450316186305"></a><a name="p1450316186305"></a>z (output)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1150361853017"><a name="p1150361853017"></a><a name="p1150361853017"></a>(1, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p15506017153218"><a name="p15506017153218"></a><a name="p15506017153218"></a>bfloat16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503101813012"><a name="p1503101813012"></a><a name="p1503101813012"></a>ND</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Kernel function name: tmp\_buffer\_custom
-   Main APIs used:
    -   DataCopy: data movement API
    -   Cast: vector precision conversion API
    -   Add: vector base arithmetic API
    -   EnQue, DeQue, and other APIs: Queue management APIs

-   Operator implementation file name: **tmp\_buffer.asc**

## Operator Class Implementation<a name="zh-cn_topic_0000002201317266_section824984034911"></a>

In this sample, the CopyIn and CopyOut tasks are the same as those of the [basic vector operator](basic_vector_operator.md). The specific process of the Compute task is shown in the following figure.

**Figure 1**  Add computation process with bfloat16\_t type input<a name="zh-cn_topic_0000002201317266_fig816618211471"></a>  
![](../../../figures/bf16_add_flow.png "Add computation process with bfloat16_t type input")

During computation, the temporary variables that represent the Cast conversion result and the Add computation result must be stored in temporary memory. Compared with the KernelAdd operator class of the [base vector operator implementation](basic_vector_operator.md), this sample adds two TBuf member variables, tmpBuf0 and tmpBuf1, to manage the temporary memory used during computation. Therefore, in addition to the original steps, the initialization phase needs to call the [InitBuffer](../../../../api/SIMD-API/basic_api/resource_management/TPipe/InitBuffer.md) API to allocate memory for the TBuf variables. The specific initialization phase code is as follows:

```
// Init phase
AscendC::GlobalTensor<bfloat16_t> xGm;
AscendC::GlobalTensor<bfloat16_t> yGm;
AscendC::GlobalTensor<bfloat16_t> zGm;
xGm.SetGlobalBuffer((__gm__ bfloat16_t*)x, totalLength);
yGm.SetGlobalBuffer((__gm__ bfloat16_t*)y, totalLength);
zGm.SetGlobalBuffer((__gm__ bfloat16_t*)z, totalLength);

AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueX;
AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueY;
AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueZ;
pipe.InitBuffer(inQueueX, 1, totalLength * sizeof(bfloat16_t));
pipe.InitBuffer(inQueueY, 1, totalLength * sizeof(bfloat16_t));
pipe.InitBuffer(outQueueZ, 1, totalLength * sizeof(bfloat16_t));

// Initialize the TBuf temporary buffer to store the float data after type conversion
AscendC::TBuf<AscendC::TPosition::VECCALC> tmpBuf0;
AscendC::TBuf<AscendC::TPosition::VECCALC> tmpBuf1;
pipe.InitBuffer(tmpBuf0, totalLength * sizeof(float));
pipe.InitBuffer(tmpBuf1, totalLength * sizeof(float));
```

Based on the vector programming paradigm, the kernel function needs to implement three basic tasks: CopyIn, Compute, and CopyOut. As with the [base vector operator implementation](basic_vector_operator.md#zh-cn_topic_0000002201157438_section10423482111), the kernel function performs CopyIn, Compute, and CopyOut in sequence. Among them, CopyIn and CopyOut are implemented in the same way as [CopyIn of the base vector operator](basic_vector_operator.md#copyin-implementation) and [CopyOut of the base vector operator](basic_vector_operator.md#copyout-implementation), so they are not described in detail here. The implementation steps of Compute are as follows:

1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to obtain a LocalTensor from the Queue of the Unified Buffer (UB, VECIN).
2.  Use TBuf.[Get](../../../../api/SIMD-API/basic_api/resource_management/TBuf/Get.md) to obtain a Tensor of the full length from TBuf as temporary memory.
3.  Use the [Cast](../../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md) API to convert the LocalTensor to the float type and store it in the temporary memory.
4.  Use the [Add](../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md) API to complete the vector computation and store the computation result in the temporary memory.
5.  Use the [Cast](../../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md) API to convert the computation result in the temporary memory to the bfloat16\_t type.
6.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to place the LocalTensor of the bfloat16\_t type into the Queue of UB (VECOUT).
7.  Use [FreeTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/FreeTensor.md) to release the LocalTensor that is no longer used.

```
// Compute phase
xLocal = inQueueX.DeQue<bfloat16_t>();
yLocal = inQueueY.DeQue<bfloat16_t>();
AscendC::LocalTensor<bfloat16_t> zLocal = outQueueZ.AllocTensor<bfloat16_t>();
AscendC::LocalTensor<float> tmpTensor0 = tmpBuf0.Get<float>();
AscendC::LocalTensor<float> tmpTensor1 = tmpBuf1.Get<float>();
// Use the Cast API to convert bfloat16_t to the float type and store the result in the TBuf temporary buffer.
AscendC::Cast(tmpTensor0, xLocal, AscendC::RoundMode::CAST_NONE, totalLength);
AscendC::Cast(tmpTensor1, yLocal, AscendC::RoundMode::CAST_NONE, totalLength);
AscendC::Add(tmpTensor0, tmpTensor0, tmpTensor1, totalLength);
AscendC::Cast(zLocal, tmpTensor0, AscendC::RoundMode::CAST_RINT, totalLength);
outQueueZ.EnQue<bfloat16_t>(zLocal);
inQueueX.FreeTensor(xLocal);
inQueueY.FreeTensor(yLocal);
```
