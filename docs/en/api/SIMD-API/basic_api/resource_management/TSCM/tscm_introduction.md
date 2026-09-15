# TSCM Introduction<a name="ZH-CN_TOPIC_0000002161314637"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T15:00:54.675Z -->

Vector and Cube complete inter-task communication and synchronization through queues. TSCM is a data structure used to manage queue-related operations and resources when the destination of the data path is at the TSCM Position. TSCM, TQueBind, and TQue are of the same type structure. TSCM is defined as follows:

```
template <TPosition pos, int32_t depth, auto mask = 0>
using TSCM = TQueBind<pos, TPosition::TSCM, depth, mask>;
```

**Table 1** Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="13.63%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.37%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="13.63%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>pos</p></td>
<td class="cellrowborder" valign="top" width="86.37%" headers="mcps1.2.3.1.2 "><p id="p35011591693"><a name="p35011591693"></a><a name="p35011591693"></a>Logical position of the queue, which can be <span>VECIN or GM</span>. <span>For details about TPosition, see</span><a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
<tr id="row12501859799"><td class="cellrowborder" valign="top" width="13.63%" headers="mcps1.2.3.1.1 "><p id="p1650113599915"><a name="p1650113599915"></a><a name="p1650113599915"></a>depth</p></td>
<td class="cellrowborder" valign="top" width="86.37%" headers="mcps1.2.3.1.2 "><p id="p8510214185418"><a name="p8510214185418"></a><a name="p8510214185418"></a>The depth of the queue indicates the number of consecutive enqueue/dequeue operations that the queue can perform. During code execution, if there are n consecutive EnQue operations on the same queue (with no DeQue in between), the depth of the queue needs to be set to n.</p>
<p id="p7674135716528"><a name="p7674135716528"></a><a name="p7674135716528"></a>Note that the queue depth here is unrelated to double buffer. The queue mechanism is used to implement pipeline parallelism, and double buffer further improves the pipeline utilization on this basis. Even if the queue depth is 1, double buffer can still be enabled.</p>
<p id="p1334591855410"><a name="p1334591855410"></a><a name="p1334591855410"></a>When the queue depth is set to 1, the compiler performs special optimization for this scenario, and the performance is usually better. <strong id="b0597122135416"><a name="b0597122135416"></a><a name="b0597122135416"></a>It is recommended to set it to 1</strong>.</p>
<a name="ul26757572527"></a><a name="ul26757572527"></a><ul id="ul26757572527"><li>In the following sample, the queue has no consecutive enqueue operations, so the queue depth is set to 1.<a name="screen17724335125812"></a><a name="screen17724335125812"></a><pre class="screen" codetype="Cpp" id="screen17724335125812">a1 = que.AllocTensor(); 
que.EnQue(a1);
a1 = que.DeQue();
que.FreeTensor(a1);</pre>
</li></ul>
<a name="ul12675135785214"></a><a name="ul12675135785214"></a><ul id="ul12675135785214"><li>In the following sample, the queue has two consecutive enqueue operations, so the queue depth should be set to 2. This is used only in very rare preload scenarios (for example, loading two copies of data consecutively, processing one copy, loading another copy after completion, and then processing the copy loaded in advance...). In all other cases, depth &gt;= 2 is not recommended.<a name="screen11973229582"></a><a name="screen11973229582"></a><pre class="screen" codetype="Cpp" id="screen11973229582">a1 = que.AllocTensor(); 
a2 = que.AllocTensor();
que.EnQue(a1);
que.EnQue(a2);
a1 = que.DeQue();
a2 = que.DeQue(); 
que.FreeTensor(a1);
que.FreeTensor(a2);</pre>
</li></ul></td>
</tr>
<tr id="row3501135910920"><td class="cellrowborder" valign="top" width="13.63%" headers="mcps1.2.3.1.1 "><p id="p6501175912914"><a name="p6501175912914"></a><a name="p6501175912914"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="86.37%" headers="mcps1.2.3.1.2 "><a name="ul4317543497"></a><a name="ul4317543497"></a><ul id="ul4317543497"><li>When mask is of the int type, information is expressed using bits: <a name="ul196262325012"></a><a name="ul196262325012"></a><ul id="ul196262325012"><li>Bit 0 is a reserved parameter.</li><li>Bit 1 is a reserved parameter.</li><li>When bit 2 is 0, the memory locations of VECTOR0 and VECTOR1 mapped to CUBE are staggered; when it is 1, the memory locations of VECTOR0 and VECTOR1 mapped to CUBE are the same.<p id="p8524124174715"><a name="p8524124174715"></a><a name="p8524124174715"></a><a name="image147520519109"></a><a name="image147520519109"></a><span><img class="eddx" id="image147520519109" src="../../../../figures/cube_prog_logic_pos.png"></span></p>
</li></ul>
</li><li>When mask is of the const TQueConfig* type, the TQueConfig structure definition and parameter description are as follows.<a name="screen13896155731115"></a><a name="screen13896155731115"></a><pre class="screen" codetype="Cpp" id="screen13896155731115">struct TQueConfig {
    bool nd2nz = false;  // Not supported. Defaults to false.
    bool nz2nd = false;  // Not supported. Defaults to false.
    bool scmBlockGroup = false;  // TSCM-related parameter. When it is false, the memory locations of VECTOR0 and VECTOR1 mapped to CUBE are staggered; when it is true, the memory locations of VECTOR0 and VECTOR1 mapped to CUBE are the same.
    uint32_t bufferLen = 0;  // Keep consistent with the len parameter passed to InitBuffer. This enables compile-time performance optimization. Passing 0 means resource allocation is performed in InitBuffer.
    uint32_t bufferNumber = 0;  // Keep consistent with the num parameter passed to InitBuffer. This enables compile-time performance optimization. Passing 0 means resource allocation is performed in InitBuffer.
    uint32_t consumerSize = 0;  // Reserved parameter.
    TPosition consumer[8] = {}; // Reserved parameter.
    bool enableStaticEvtId = false; // Reserved parameter.
    bool enableLoopQueue = false;   // Reserved parameter.
};</pre>
</li></ul></td>
</tr>
</tbody>
</table>

> [!NOTE]Description
>-   TSCM is defined via using as an alias of TQueBind when the destination address is the TSCM Position.
>    -   The AllocTensor/EnQue/DeQue/FreeTensor APIs are supported. The complete lifecycle must be strictly executed in the order of AllocTensor-\>EnQue-\>DeQue-\>FreeTensor, and they must be used in pairs.
>    -   However, TSCM does not need to support all APIs of TQueBind. VacantInQue/HasTensorInQue/GetTensorCountInQue/HasIdleBuffer are not supported.
>-   Because the Buffer allocated by TSCM stores the synchronization event eventID, and this structure is used together with Cube high-level APIs (such as the Matmul high-level API, macro function call mode), the number of TSCM Buffers on the same TPosition is related to the hardware synchronization event eventID and the number of Matmul objects.
>    **The sum of the number of Buffer blocks initiated by TSCM from VECIN and the number of Matmul objects is at most 10.**
>    **Applying for TSCM Buffers beyond the specification limit is not allowed, as exceeding the specification may cause undefined behavior.**

The following is a simple usage example:

```
TSCM<TPosition::VECIN, 1> tscm;
for () {
    auto scmTensor = tscm.AllocTensor<float>(); // Allocate the Buffer before moving data from UB to TSCM.
    DataCopy(scmTensor, ubLocal, 1024); // Move the UB data to TSCM to prepare for Matmul computation.
    tscm.EnQue(scmTensor); //After the move is complete and before Matmul computation, call EnQue/DeQue.
    LocalTensor<float> scmLocal = tscm.DeQue<float>();
    mm.SetTensorA(scmLocal);
    mm.SetTensorB(gm_b);
    mm.IterateAll(gm_c);
    tscm.FreeTensor(scmLocal); // After Matmul computation is complete, release the tensor.
}
```

The following is an example of using it together with the high-level API Matmul:

```
{
    typedef matmul::MatmulType<AscendC::TPosition::TSCM, CubeFormat::NZ, half, true, LayoutMode::NONE, false, AscendC::TPosition::VECIN> A_TYPE;
    typedef matmul::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> B_TYPE;
    typedef matmul::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> C_TYPE;
    typedef matmul::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> BIAS_TYPE; 
    matmul::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE> mm1;
    constexpr uint32_t M = 32;
    constexpr uint32_t N = 32;
    constexpr uint32_t K = 32;
    AscendC::GlobalTensor<half> aGlobal;
    AscendC::GlobalTensor<half> bGlobal;
    AscendC::GlobalTensor<float> cGlobal;
    AscendC::GlobalTensor<float> biasGlobal;
    aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(aGM), M * K);
    bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(bGM), K * N);
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float *>(cGM), M * N);
    TCubeTiling tiling;
    AscendC::TPipe pipe;
    AscendC::TSCM<AscendC::TPosition::VECIN, 1> scm;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> qIn;
    pipe.InitBuffer(scm, 1, M * K * sizeof(half));
    pipe.InitBuffer(qIn, 1, M * K * sizeof(half));
    REGIST_MATMUL_OBJ(&pipe, workspaceGM, mm1, &tiling);
    auto scmTensor = scm.AllocTensor<half>();
    auto ubTensor = qIn.AllocTensor<half>();
    AscendC::Nd2NzParams intriParams;
    DataCopy(ubTensor, aGlobal, M * K);
    qIn.EnQue(ubTensor);
    AscendC::LocalTensor<half> ubLocal = qIn.DeQue<half>();
    AscendC::DataCopy(scmTensor, ubLocal, intriParams);
    scm.EnQue(scmTensor);
    AscendC::LocalTensor<half> scmLocal = scm.DeQue<half>();
    mm1.SetTensorA(scmLocal);
    mm1.SetTensorB(bGlobal);
    mm1.IterateAll(cGlobal);
    scm.FreeTensor(scmLocal);
    qIn.FreeTensor(ubLocal);
}
```
