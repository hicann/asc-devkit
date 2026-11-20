# CubeResGroupHandle Usage Description<a name="ZH-CN_TOPIC_0000001960384796"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-19T02:44:57.533Z pushedAt=2026-11-19T05:58:49.105Z nl=2 -->

CubeResGroupHandle is used to group AI Core computation resources in separated mode. After grouping, developers can assign different computation tasks to different groups. An AI Core group can contain multiple AIVs and AICs, and the AIVs and AICs adopt a Client and Server architecture for task processing. The AIV acts as the Client, and each Cube computation task is a message. The AIV sends messages to the message queue, and the AIC acts as the Server, traverses the messages in the message queue, and executes the corresponding computation task based on the message type and content. A CubeResGroupHandle can contain one or more AICs, and the same AIC can belong to only one CubeResGroupHandle. The AIV has no such restriction, that is, the same AIV can belong to multiple CubeResGroupHandles.

As shown in the following figure, CubeResGroupHandle1 contains 2 AICs and 10 AIVs, and the AICs are Block0 and Block1. Block0 communicates with Queue0, Queue1, Queue2, Queue3, and Queue4, and Block1 communicates with Queue 5, Queue 6, Queue 7, Queue 8, and Queue9. Each message queue corresponds to one AIV, and the depth of the message queue is fixed at 4, that is, it can hold up to 4 messages at a time. The number of message queues in CubeResGroupHandle2 is 12, indicating that there are 12 AIVs. The message processing order of CubeResGroupHandle is shown by the black arrows in CubeResGroupHandle2.

**Figure 1**  AI Core computation resource grouping communication diagram based on CubeResGroupHandle<a name="fig1086562617536"></a>  
![](../../../../../figures/cuberesgrouphandle_aicore_group.png "AI-Core computation resource grouping communication diagram based on CubeResGroupHandle")

The steps to implement AI Core computation resource grouping based on CubeResGroupHandle are as follows:

1.  Create the required computation object type on the AIC.
2.  Create the communication area description [KfcWorkspace](../../inter_core_communication/KfcWorkspace/KfcWorkspace.md), which is used to record the address allocation of the communication message Msg.
3.  Define a custom message structure for communication.
4.  Define a custom callback computation structure, and implement the Init function and Call function according to actual business scenarios.
5.  Create a CubeResGroupHandle.
6.  Bind the AIV to the CubeResGroupHandle.
7.  Send and receive messages.
8.  The AIV exits the message queue.

1.  <a name="li27691150733"></a>Create the required computation object type on the AIC.

    According to actual requirements, the user defines the computation object type required by the AIC, or uses the Matmul type already provided by the high-level API. For example, create the Matmul type as follows, where the meanings of A\_TYPE, B\_TYPE, C\_TYPE, BIAS\_TYPE, CFG\_NORM, and so on, refer to [Matmul template parameters](../../../../adv_api/cube_compute/Matmul_Kernel/Matmul_template_params.md).

    ```
    // Construct A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, and CFG_NORM according to actual requirement scenarios.
    using MatmulApiType = Matmul<A_TYPE, B_TYPE, C_TYPE, C_TYPE, CFG_NORM>;
    ```

2.  Create a KfcWorkspace.

    Use [KfcWorkspace](../../inter_core_communication/KfcWorkspace/KfcWorkspace.md) to manage the division of the message communication area of different CubeResGroupHandles.

    ```
    // Before creating the KfcWorkspace object, clear this workspaceGM to zero.
    KfcWorkspace desc(workspaceGM);
    ```

3.  Define a custom message structure.

    The user must construct the message structure [CubeMsgBody](#table189051237164018) by themselves, which is used by the AIV to send communication messages to the AIC. The constructed CubeMsgBody must be 64-byte aligned. A 2-byte CubeGroupMsgHead must be defined at the beginning of the structure to ensure the normal operation of the message send and receive mechanism. For the CubeGroupMsgHead structure definition, refer to [Table 2](#table77221554135216). Except for the 2-byte CubeGroupMsgHead, the remaining parameters are constructed by the user according to business requirements.

    **Table 1**  CubeMsgBody message structure

    <a name="table189051237164018"></a>
    <table><thead align="left"><tr id="row990543774012"><th class="cellrowborder" valign="top" width="19.6%" id="mcps1.2.3.1.1"><p id="p39051637104010"><a name="p39051637104010"></a><a name="p39051637104010"></a>Parameter Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="80.4%" id="mcps1.2.3.1.2"><p id="p1290623718401"><a name="p1290623718401"></a><a name="p1290623718401"></a>Meaning</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row19062378407"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.2.3.1.1 "><p id="p2906203720407"><a name="p2906203720407"></a><a name="p2906203720407"></a>CubeMsgBody</p>
    </td>
    <td class="cellrowborder" valign="top" width="80.4%" headers="mcps1.2.3.1.2 "><p id="p7906113754018"><a name="p7906113754018"></a><a name="p7906113754018"></a>User-defined message structure. The structure name can be customized, and the structure size must be 64-byte aligned.</p>
    <a name="screen1555172113494"></a><a name="screen1555172113494"></a><pre class="screen" codetype="Cpp" id="screen1555172113494">// A 64B-aligned structure example is provided here. In actual use, except for CubeGroupMsgHead, the number and types of other parameters can be constructed by the user.
    struct CubeMsgBody {
       CubeGroupMsgHead head;  // 2B, must be placed at the beginning of the structure. In a custom CubeMsgBody, the variable name of CubeGroupMsgHead must be set to head, otherwise a compilation error occurs.
       uint8_t funcID;
       uint8_t skipCnt;
       uint32_t value;
       bool isTransA;
       bool isTransB;
       bool isAtomic;
       bool isLast;                 
       int32_t tailM;              
       int32_t tailN;
       int32_t tailK;               
       uint64_t aAddr;
       uint64_t bAddr;
       uint64_t cAddr;
       uint64_t aGap;
       uint64_t bGap;
    }</pre>
    </td>
    </tr>
    </tbody>
    </table>

    **Table 2**  CubeGroupMsgHead structure parameter definition

    <a name="table77221554135216"></a>
    <table><thead align="left"><tr id="row1072214548521"><th class="cellrowborder" valign="top" width="11.91%" id="mcps1.2.3.1.1"><p id="p8722115485218"><a name="p8722115485218"></a><a name="p8722115485218"></a>Parameter Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="88.09%" id="mcps1.2.3.1.2"><p id="p9723754105215"><a name="p9723754105215"></a><a name="p9723754105215"></a>Meaning</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1572395414524"><td class="cellrowborder" valign="top" width="11.91%" headers="mcps1.2.3.1.1 "><p id="p1372335418525"><a name="p1372335418525"></a><a name="p1372335418525"></a>msgState</p>
    </td>
    <td class="cellrowborder" valign="top" width="88.09%" headers="mcps1.2.3.1.2 "><p id="p972385485211"><a name="p972385485211"></a><a name="p972385485211"></a>Indicates the message state at this position. The parameter values are as follows:</p>
    <a name="ul372310546523"></a><a name="ul372310546523"></a><ul id="ul372310546523"><li>CubeMsgState::FREE: Indicates that no message has been written at this position, and <a href="AllocMessage.md">AllocMessage</a> can be executed.</li><li>CubeMsgState::VALID: Indicates that this position already contains a message sent by the AIV, waiting to be received and executed by the AIC.</li><li>CubeMsgState::QUIT: Indicates that the message at this position notifies the AIC that an AIV will exit the process.</li><li>CubeMsgState::FAKE: Indicates that the message at this position is a fake message. In the message merging scenario, the AIV whose processing task is skipped must send a fake message. For the message merging scenario, refer to the introduction in <a href="PostFakeMsg.md">PostFakeMsg</a>.</li></ul>
    </td>
    </tr>
    <tr id="row472325455211"><td class="cellrowborder" valign="top" width="11.91%" headers="mcps1.2.3.1.1 "><p id="p37231654135220"><a name="p37231654135220"></a><a name="p37231654135220"></a>aivID</p>
    </td>
    <td class="cellrowborder" valign="top" width="88.09%" headers="mcps1.2.3.1.2 "><p id="p17723135445214"><a name="p17723135445214"></a><a name="p17723135445214"></a>Sequence number of the AIV that sends the message.</p>
    </td>
    </tr>
    </tbody>
    </table>

4.  Define a custom callback computation structure, and implement the Init function and Call function according to actual business scenarios.

    ```
    template<class MatmulApiCfg, class CubeMsgBody>
    struct NormalCallbackFuncs {
        __aicore__ inline static void Call(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle){
          // The user implements the logic by themselves.
        };
    
        __aicore__ inline static void Init(NormalCallbackFuncs<MatmulApiCfg, CubeMsgBody> &foo, MatmulApiCfg &mm, GM_ADDR tilingGM){
           // The user implements the logic by themselves.
        };
       
    };
    ```

    For the template parameters of the computation logic structure, refer to [Table 3](#table18865397406).

    **Table 3**  Template parameter description

    <a name="table18865397406"></a>
    <table><thead align="left"><tr id="row888719393401"><th class="cellrowborder" valign="top" width="20.82%" id="mcps1.2.3.1.1"><p id="p18887193944012"><a name="p18887193944012"></a><a name="p18887193944012"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="79.17999999999999%" id="mcps1.2.3.1.2"><p id="p788773984016"><a name="p788773984016"></a><a name="p788773984016"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row48872393405"><td class="cellrowborder" valign="top" width="20.82%" headers="mcps1.2.3.1.1 "><p id="p10887173915407"><a name="p10887173915407"></a><a name="p10887173915407"></a>MatmulApiCfg</p>
    </td>
    <td class="cellrowborder" valign="top" width="79.17999999999999%" headers="mcps1.2.3.1.2 "><p id="p988893994019"><a name="p988893994019"></a><a name="p988893994019"></a>The data type of the object required for computation on the user-defined AIC. Refer to <a href="#li27691150733">Step 1</a>. This template parameter must be filled in.</p>
    </td>
    </tr>
    <tr id="row1888739204012"><td class="cellrowborder" valign="top" width="20.82%" headers="mcps1.2.3.1.1 "><p id="p188883391405"><a name="p188883391405"></a><a name="p188883391405"></a>CubeMsgBody</p>
    </td>
    <td class="cellrowborder" valign="top" width="79.17999999999999%" headers="mcps1.2.3.1.2 "><p id="p1688823914010"><a name="p1688823914010"></a><a name="p1688823914010"></a><a href="#table189051237164018">User-defined message structure</a>. This template parameter must be filled in.</p>
    </td>
    </tr>
    </tbody>
    </table>

    The user-defined callback computation structure must contain the fixed Init function and Call function, and the function prototypes are as follows. For the parameter description of the Init function, refer to [Table 4](#zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446), and for the parameter description of the Call function, refer to [Table 5](#table9997952179).

    ```
    // The parameters and names of this function are in a fixed format. The function implementation is implemented by the user based on business logic.
    __aicore__ inline static void Init(MyCallbackFunc<MatmulApiCfg, CubeMsgBody> &myCallBack, MatmulApiCfg &mm, GM_ADDR tilingGM){
         // The user implements the internal logic by themselves.
    }
    ```

    **Table 4**  Init function parameter description

    <a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
    <table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="19.15%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.04%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="72.81%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>myCallBack</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.04%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.81%" headers="mcps1.2.4.1.3 "><p id="p17175015525"><a name="p17175015525"></a><a name="p17175015525"></a>User-defined callback computation structure with <a href="#table18865397406">template parameters</a>.</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000001526206862_row1239183183016"><td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_p223953193015"><a name="zh-cn_topic_0000001526206862_p223953193015"></a><a name="zh-cn_topic_0000001526206862_p223953193015"></a>mm</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.04%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.81%" headers="mcps1.2.4.1.3 "><p id="p1117515154210"><a name="p1117515154210"></a><a name="p1117515154210"></a>Computation object on the AIC, mostly a Matmul object.</p>
    </td>
    </tr>
    <tr id="row9374154371313"><td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>tilingGM</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.04%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.81%" headers="mcps1.2.4.1.3 "><p id="p517518151222"><a name="p517518151222"></a><a name="p517518151222"></a>Tiling pointer passed in by the user.</p>
    </td>
    </tr>
    </tbody>
    </table>

    ```
    // The parameters and names of this function are in a fixed format. The function implementation is implemented by the user based on business logic.
    __aicore__ inline static void Call(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle){
            // The user implements the internal logic by themselves.
    }
    ```

    **Table 5**  Call function parameter description

    <a name="table9997952179"></a>
    <table><thead align="left"><tr id="row599775171716"><th class="cellrowborder" valign="top" width="18.62813718628137%" id="mcps1.2.4.1.1"><p id="p6997858178"><a name="p6997858178"></a><a name="p6997858178"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.30916908309169%" id="mcps1.2.4.1.2"><p id="p16997165131717"><a name="p16997165131717"></a><a name="p16997165131717"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="73.06269373062693%" id="mcps1.2.4.1.3"><p id="p1799755121718"><a name="p1799755121718"></a><a name="p1799755121718"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row119971056171"><td class="cellrowborder" valign="top" width="18.62813718628137%" headers="mcps1.2.4.1.1 "><p id="p59971755174"><a name="p59971755174"></a><a name="p59971755174"></a>mm</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.30916908309169%" headers="mcps1.2.4.1.2 "><p id="p1599755171712"><a name="p1599755171712"></a><a name="p1599755171712"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="73.06269373062693%" headers="mcps1.2.4.1.3 "><p id="p39982520173"><a name="p39982520173"></a><a name="p39982520173"></a>Computation object on the AIC, mostly a Matmul object.</p>
    </td>
    </tr>
    <tr id="row19985531717"><td class="cellrowborder" valign="top" width="18.62813718628137%" headers="mcps1.2.4.1.1 "><p id="p16998135201718"><a name="p16998135201718"></a><a name="p16998135201718"></a>rcvMsg</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.30916908309169%" headers="mcps1.2.4.1.2 "><p id="p49981151174"><a name="p49981151174"></a><a name="p49981151174"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="73.06269373062693%" headers="mcps1.2.4.1.3 "><p id="p899810515175"><a name="p899810515175"></a><a name="p899810515175"></a>User-defined message structure pointer.</p>
    </td>
    </tr>
    <tr id="row599815501714"><td class="cellrowborder" valign="top" width="18.62813718628137%" headers="mcps1.2.4.1.1 "><p id="p0998165161717"><a name="p0998165161717"></a><a name="p0998165161717"></a>handle</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.30916908309169%" headers="mcps1.2.4.1.2 "><p id="p179981351176"><a name="p179981351176"></a><a name="p179981351176"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="73.06269373062693%" headers="mcps1.2.4.1.3 "><p id="p89981571714"><a name="p89981571714"></a><a name="p89981571714"></a>Grouping management Handle. The user calls its APIs to send and receive messages, release messages, and so on.</p>
    </td>
    </tr>
    </tbody>
    </table>

    A code example of the callback computation structure of a certain operator is as follows.

    ```
    // User-defined callback computation logic.
    template<class MatmulApiCfg, typename CubeMsgBody>
    struct MyCallbackFunc
    {
        template<int32_t funcId>
        __aicore__ inline static typename IsEqual<funcId, 0>::Type CubeGroupCallBack(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle)
        {
            GlobalTensor<int64_t> msgGlobal;
            msgGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int64_t *> (rcvMsg) + sizeof(int64_t));
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT> (msgGlobal);
            using SrcAT = typename MatmulApiCfg::AType::T;
            auto skipNum = 0;
            for (int i = 0; i < skipNum + 1; ++i)
            {
                auto tmpId = handle.FreeMessage(rcvMsg + i); // msgPtr process is complete
            }
            handle.SetSkipMsg(skipNum);
        }
        template<int32_t funcId>
        __aicore__ inline static typename IsEqual<funcId, 1>::Type CubeGroupCallBack(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle)
        {
            GlobalTensor<int64_t> msgGlobal;
            msgGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int64_t *> (rcvMsg) + sizeof(int64_t));
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT> (msgGlobal);
            using SrcAT = typename MatmulApiCfg::AType::T;
            LocalTensor<SrcAT> tensor_temp;
            auto skipNum = 3;
            auto tmpId = handle.FreeMessage(rcvMsg, CubeMsgState::VALID);
            for (int i = 1; i < skipNum + 1; ++i)
            {
                auto tmpId = handle.FreeMessage(rcvMsg + i, CubeMsgState::FAKE);
            }
            handle.SetSkipMsg(skipNum); // notify the cube not to process
        }
        __aicore__ inline static void Call(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle)
        {
            if (rcvMsg->funcId == 0)
            {
                CubeGroupCallBack<0> (mm, rcvMsg, handle);
            }
            else if(rcvMsg->funcId == 1)
            {
                CubeGroupCallBack<1> (mm, rcvMsg, handle);
            }
        }
        __aicore__ inline static void Init(MyCallbackFunc<MatmulApiCfg, CubeMsgBody> &foo, MatmulApiCfg &mm, GM_ADDR tilingGM)
        {
            auto tempTilingGM = (__gm__ uint32_t*)tilingGM;
            auto tempTiling = (uint32_t*)&(foo.tiling);
            for (int i = 0; i < sizeof(TCubeTiling) / sizeof(int32_t); ++i, ++tempTilingGM, ++tempTiling)
            {
                *tempTiling = *tempTilingGM;
            }
            mm.SetSubBlockIdx(0);
            mm.Init(&foo.tiling, GetTPipePtr());
        }
        TCubeTiling tiling;
    };
    ```

5.  <a name="li355132105919"></a>Create a CubeResGroupHandle.

    The user uses the [CreateCubeResGroup](CreateCubeResGroup.md) API to create one or more CubeResGroupHandles.

    ```
    /* 
     * groupID is the user-defined groupID of CreateCubeResGroup.
     * MatmulApiType is the predefined type of the computation object on the AIC.
     * MyCallbackFunc is the predefined custom callback computation structure.
     * CubeMsgBody is the custom message structure.
     * desc is the communication area description initialized by the user.
     * groupID is 1, blockStart is 0, blockSize is 12, msgQueueSize is 48, and tilingGm is a pointer that stores the tiling information required by the user on the AIC.
    */
    auto handle =  AscendC::CreateCubeResGroup<groupID, MatmulApiType, MyCallbackFunc, CubeMsgBody>(desc, 0, 12, 48, tilingGM);
    ```

6.  Bind the AIV to the CubeResGroupHandle.

    Bind the AIV and the message queue sequence number. Note: The message queue sequence number queIdx must be less than the total number of message queues of this CubeGroupHandle, and each AIV must pass in a different queIdx. handle is the CubeResGroupHandle object created by CreateCubeResGroup in [Step 5](#li355132105919).

    ```
    handle.AssignQueue(queIdx);
    ```

7.  The AIV sends messages.

    The user calls APIs such as [AllocMessage](AllocMessage.md) and [PostMessage](PostMessage.md) to send and receive messages. Among them, AllocMessage is called to get the message structure pointer, PostMessage is used to send messages, and in the message merging scenario, [PostFakeMessage](PostFakeMsg.md) is called to send a fake message. An example is as follows.

    ```
    CubeGroupMsgHead head = {CubeMsgState::VALID, (uint8_t)queIdx};
    CubeMsgBody aCubeMsgBody {head, 0, 0, 0, false, false, false, false, 0, 0, 0, 0, 0, 0, 0, 0};
    CubeMsgBody bCubeMsgBody {head, 1, 0, 0, false, false, false, false, 0, 0, 0, 0, 0, 0, 0, 0};
    auto offset = 0;
    if (GetBlockIdx() == 0)
    {
        auto msgPtr = handle.template AllocMessage(); // alloc for queue space
        offset = handle.template PostMessage(msgPtr, bCubeMsgBody); // post true msgPtr
        bool waitState = handle.template Wait<true> (offset); // wait until the msgPtr is processed
    }
    else if (GetBlockIdx() < 4)
    {
        auto msgPtr = handle.AllocMessage();
        offset = handle.PostFakeMsg(msgPtr); // post fake msgPtr
        bool waitState = handle.template Wait<true> (offset); // wait until the msgPtr is processed
    }
    else
    {
        auto msgPtr = handle.template AllocMessage();
        offset = handle.template PostMessage(msgPtr, aCubeMsgBody);
        bool waitState = handle.template Wait<true> (offset); // wait until the msgPtr is processed
    }
    ```

8.  The AIV exits the message queue.

    After calling AllocMessage to get the message structure pointer, use SendQuitMsg to send the current message queue exit.

    ```
    auto msgPtr = handle.AllocMessage();        // Get the message space pointer msgPtr.
    handle.SetQuit(msgPtr);              // Send the exit message.
    ```
