# CubeResGroupHandle Usage Guide<a name="ZH-CN_TOPIC_0000001960384796"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:34:32.547Z -->

**CubeResGroupHandle** is used to group AI Core computing resources in separated mode. After grouping, developers can assign different computation tasks to different groups. An AI Core group can contain multiple AIVs and AICs, and the AIVs and AICs adopt a Client-Server architecture for task processing. The AIV acts as the Client, and each Cube computation task is a message. The AIV sends messages to the message queue, and the AIC acts as the Server, traversing the messages in the message queue and executing the corresponding computation tasks based on the message type and content. A **CubeResGroupHandle** can contain one or more AICs, and the same AIC can belong to only one **CubeResGroupHandle**. AIVs have no such restriction, meaning the same AIV can belong to multiple **CubeResGroupHandle** instances.

As shown in the following figure, **CubeResGroupHandle1** contains two AICs and ten AIVs, where the AICs are Block0 and Block1. Block0 communicates with Queue0, Queue1, Queue2, Queue3, and Queue4, while Block1 communicates with Queue5, Queue6, Queue7, Queue8, and Queue9. Each message queue corresponds to one AIV, and the depth of the message queue is fixed at 4, meaning it can hold up to four messages at a time. The number of message queues in **CubeResGroupHandle2** is 12, indicating that there are 12 AIVs. The message processing order of **CubeResGroupHandle** is shown by the black arrows in **CubeResGroupHandle2**.

**Figure 1**  Schematic diagram of AI Core computing resource group communication based on CubeResGroupHandle<a name="fig1086562617536"></a>  
![](../../../../figures/aicore_comm_via_cube_res_group.png "Schematic diagram of AI Core computing resource group communication based on CubeResGroupHandle")

The steps for grouping AI Core computing resources based on **CubeResGroupHandle** are as follows:

1.  Create the computation object types required on the AIC.
2.  Create the communication area description [KfcWorkspace](../KfcWorkspace/KfcWorkspace.md) to record the address allocation of the communication message Msg.
3.  Define a user-defined message structure for communication.
4.  Define a user-defined callback computation structure, and implement the Init function and Call function according to the actual business scenario.
5.  Create a **CubeResGroupHandle**.
6.  Bind AIV to **CubeResGroupHandle**.
7.  Send and receive messages.
8.  Exit the message queue for AIV.

1.  <a name="li27691150733"></a>Create the computing object type required on the AIC.

    Define the computing object type required by the AIC based on actual requirements, or use the **Matmul** type already provided by the high-level API. For example, create the **Matmul** type as follows, where the meanings of **A\_TYPE**, **B\_TYPE**, **C\_TYPE**, **BIAS\_TYPE**, and **CFG\_NORM** are described in [Matmul template parameters](../../../advanced_api/matrix_compute/Matmul-Kernel/matmul_template_parameters.md).

    ```
    // Construct A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, and CFG_NORM based on the actual scenario.
    using MatmulApiType = Matmul<A_TYPE, B_TYPE, C_TYPE, C_TYPE, CFG_NORM>;
    ```

2.  Create **KfcWorkspace**.

    Use [KfcWorkspace](../KfcWorkspace/KfcWorkspace.md) to manage the division of the message communication area for different **CubeResGrouphandle** instances.

    ```
    // Clear the workspaceGM before creating the KfcWorkspace object.
    KfcWorkspace desc(workspaceGM);
    ```

3.  Define a custom message structure.

    Users need to construct the message structure [CubeMsgBody](#table189051237164018) by themselves to send communication messages from the AIV to the AIC. The constructed CubeMsgBody must be 64-byte aligned. A 2-byte CubeGroupMsgHead must be defined at the beginning of the structure to ensure that the message sending and receiving mechanism works properly. For the definition of the CubeGroupMsgHead structure, see [Table 2](#table77221554135216). Except for the 2-byte CubeGroupMsgHead, the remaining parameters are constructed by users according to their service requirements.

    **Table 1**  CubeMsgBody message structure

    <a name="table189051237164018"></a>
    <table><thead align="left"><tr id="row990543774012"><th class="cellrowborder" valign="top" width="19.6%" id="mcps1.2.3.1.1"><p id="p39051637104010"><a name="p39051637104010"></a><a name="p39051637104010"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="80.4%" id="mcps1.2.3.1.2"><p id="p1290623718401"><a name="p1290623718401"></a><a name="p1290623718401"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row19062378407"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.2.3.1.1 "><p id="p2906203720407"><a name="p2906203720407"></a><a name="p2906203720407"></a>CubeMsgBody</p>
    </td>
    <td class="cellrowborder" valign="top" width="80.4%" headers="mcps1.2.3.1.2 "><p id="p7906113754018"><a name="p7906113754018"></a><a name="p7906113754018"></a>User-defined message structure. The structure name can be customized, and the structure size must be 64-byte aligned.</p>
    <a name="screen1555172113494"></a><a name="screen1555172113494"></a><pre class="screen" codetype="Cpp" id="screen1555172113494">// The following is an example of a 64B-aligned structure. In actual use, except for CubeGroupMsgHead, the number and types of other parameters can be constructed by users.
    struct CubeMsgBody {
       CubeGroupMsgHead head;  // 2B, must be placed at the beginning of the structure. In a custom CubeMsgBody, the variable name of CubeGroupMsgHead must be set to head; otherwise, a compilation error occurs.
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
    <table><thead align="left"><tr id="row1072214548521"><th class="cellrowborder" valign="top" width="11.91%" id="mcps1.2.3.1.1"><p id="p8722115485218"><a name="p8722115485218"></a><a name="p8722115485218"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="88.09%" id="mcps1.2.3.1.2"><p id="p9723754105215"><a name="p9723754105215"></a><a name="p9723754105215"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1572395414524"><td class="cellrowborder" valign="top" width="11.91%" headers="mcps1.2.3.1.1 "><p id="p1372335418525"><a name="p1372335418525"></a><a name="p1372335418525"></a>msgState</p>
    </td>
    <td class="cellrowborder" valign="top" width="88.09%" headers="mcps1.2.3.1.2 "><p id="p972385485211"><a name="p972385485211"></a><a name="p972385485211"></a>Indicates the message state at this position. The parameter values are as follows:</p>
    <a name="ul372310546523"></a><a name="ul372310546523"></a><ul id="ul372310546523"><li>CubeMsgState::FREE: Indicates that no message has been written to this position, and <a href="AllocMessage.md">AllocMessage</a> can be executed.</li><li>CubeMsgState::VALID: Indicates that this position already contains a message sent by the AIV, waiting to be received and executed by the AIC.</li><li>CubeMsgState::QUIT: Indicates that the message at this position notifies the AIC that an AIV is about to exit the process.</li><li>CubeMsgState::FAKE: Indicates that the message at this position is a fake message. In the message merging scenario, an AIV whose processing task is skipped needs to send a fake message. For details about the message merging scenario, see <a href="PostFakeMsg.md">PostFakeMsg</a>.</li></ul>
    </td>
    </tr>
    <tr id="row472325455211"><td class="cellrowborder" valign="top" width="11.91%" headers="mcps1.2.3.1.1 "><p id="p37231654135220"><a name="p37231654135220"></a><a name="p37231654135220"></a>aivID</p>
    </td>
    <td class="cellrowborder" valign="top" width="88.09%" headers="mcps1.2.3.1.2 "><p id="p17723135445214"><a name="p17723135445214"></a><a name="p17723135445214"></a>Sequence number of the AIV that sends the message.</p>
    </td>
    </tr>
    </tbody>
    </table>

4.  Customize the callback computation structure, and implement the Init function and the Call function based on the actual service scenario.

    ```
    template<class MatmulApiCfg, class CubeMsgBody>
    struct NormalCallbackFuncs {
        __aicore__ inline static void Call(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle){
          // Implement the logic as needed.
        };
    
        __aicore__ inline static void Init(NormalCallbackFuncs<MatmulApiCfg, CubeMsgBody> &foo, MatmulApiCfg &mm, GM_ADDR tilingGM){
           // Implement the logic as needed.
        };
       
    };
    ```

    For the template parameters of the computation logic structure, see [Table 3](#table18865397406).

    **Table 3**  Template parameter description

    <a name="table18865397406"></a>
    <table><thead align="left"><tr id="row888719393401"><th class="cellrowborder" valign="top" width="20.82%" id="mcps1.2.3.1.1"><p id="p18887193944012"><a name="p18887193944012"></a><a name="p18887193944012"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="79.17999999999999%" id="mcps1.2.3.1.2"><p id="p788773984016"><a name="p788773984016"></a><a name="p788773984016"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row48872393405"><td class="cellrowborder" valign="top" width="20.82%" headers="mcps1.2.3.1.1 "><p id="p10887173915407"><a name="p10887173915407"></a><a name="p10887173915407"></a>**MatmulApiCfg**</p>
    </td>
    <td class="cellrowborder" valign="top" width="79.17999999999999%" headers="mcps1.2.3.1.2 "><p id="p988893994019"><a name="p988893994019"></a><a name="p988893994019"></a>Data type of the user-defined object required for computation on the AIC. For details, see <a href="#li27691150733">Step 1</a>. This template parameter must be filled in.</p>
    </td>
    </tr>
    <tr id="row1888739204012"><td class="cellrowborder" valign="top" width="20.82%" headers="mcps1.2.3.1.1 "><p id="p188883391405"><a name="p188883391405"></a><a name="p188883391405"></a>**CubeMsgBody**</p>
    </td>
    <td class="cellrowborder" valign="top" width="79.17999999999999%" headers="mcps1.2.3.1.2 "><p id="p1688823914010"><a name="p1688823914010"></a><a name="p1688823914010"></a><a href="#table189051237164018">User-defined message structure</a>. This template parameter must be filled in.</p>
    </td>
    </tr>
    </tbody>
    </table>

    The user-defined callback computation structure must contain the fixed **Init** function and **Call** function, whose prototypes are shown below. For the parameters of the **Init** function, see [Table 4](#zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446). For the parameters of the **Call** function, see [Table 5](#table9997952179).

    ```
    // The parameters and name of this function are in a fixed format. Implement the function based on the business logic.
    __aicore__ inline static void Init(MyCallbackFunc<MatmulApiCfg, CubeMsgBody> &myCallBack, MatmulApiCfg &mm, GM_ADDR tilingGM){
         // Implement the internal logic as needed.
    }
    ```

    **Table 4** Description of **Init** function parameters

    <a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
    <table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="19.15%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.04%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="72.81%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>**myCallBack**</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.04%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.81%" headers="mcps1.2.4.1.3 "><p id="p17175015525"><a name="p17175015525"></a><a name="p17175015525"></a>User-defined callback computation structure with <a href="#table18865397406">template parameters</a>.</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000001526206862_row1239183183016"><td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_p223953193015"><a name="zh-cn_topic_0000001526206862_p223953193015"></a><a name="zh-cn_topic_0000001526206862_p223953193015"></a>**mm**</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.04%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.81%" headers="mcps1.2.4.1.3 "><p id="p1117515154210"><a name="p1117515154210"></a><a name="p1117515154210"></a>Computing object on the AIC, mostly a **Matmul** object.</p>
    </td>
    </tr>
    <tr id="row9374154371313"><td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.2.4.1.1 "><p id="p11374343181311"><a name="p11374343181311"></a><a name="p11374343181311"></a>tilingGM</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.04%" headers="mcps1.2.4.1.2 "><p id="p146153901420"><a name="p146153901420"></a><a name="p146153901420"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.81%" headers="mcps1.2.4.1.3 "><p id="p517518151222"><a name="p517518151222"></a><a name="p517518151222"></a>**tiling** pointer passed in by the user.</p>
    </td>
    </tr>
    </tbody>
    </table>

    ```
    // The parameters and name of this function are in a fixed format, and the function implementation is customized based on the business logic.
    __aicore__ inline static void Call(MatmulApiCfg &mm, __gm__ CubeMsgBody *rcvMsg, CubeResGroupHandle<CubeMsgBody> &handle){
            // Implement the internal logic as needed.
    }
    ```

    **Table 5**  Parameters of the Call function

    <a name="table9997952179"></a>
    <table><thead align="left"><tr id="row599775171716"><th class="cellrowborder" valign="top" width="18.62813718628137%" id="mcps1.2.4.1.1"><p id="p6997858178"><a name="p6997858178"></a><a name="p6997858178"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.30916908309169%" id="mcps1.2.4.1.2"><p id="p16997165131717"><a name="p16997165131717"></a><a name="p16997165131717"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="73.06269373062693%" id="mcps1.2.4.1.3"><p id="p1799755121718"><a name="p1799755121718"></a><a name="p1799755121718"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row119971056171"><td class="cellrowborder" valign="top" width="18.62813718628137%" headers="mcps1.2.4.1.1 "><p id="p59971755174"><a name="p59971755174"></a><a name="p59971755174"></a>**mm**</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.30916908309169%" headers="mcps1.2.4.1.2 "><p id="p1599755171712"><a name="p1599755171712"></a><a name="p1599755171712"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="73.06269373062693%" headers="mcps1.2.4.1.3 "><p id="p39982520173"><a name="p39982520173"></a><a name="p39982520173"></a>Computing object on the AIC, mostly a Matmul object.</p>
    </td>
    </tr>
    <tr id="row19985531717"><td class="cellrowborder" valign="top" width="18.62813718628137%" headers="mcps1.2.4.1.1 "><p id="p16998135201718"><a name="p16998135201718"></a><a name="p16998135201718"></a>**rcvMsg**</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.30916908309169%" headers="mcps1.2.4.1.2 "><p id="p49981151174"><a name="p49981151174"></a><a name="p49981151174"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="73.06269373062693%" headers="mcps1.2.4.1.3 "><p id="p899810515175"><a name="p899810515175"></a><a name="p899810515175"></a>Pointer to the user-defined message structure.</p>
    </td>
    </tr>
    <tr id="row599815501714"><td class="cellrowborder" valign="top" width="18.62813718628137%" headers="mcps1.2.4.1.1 "><p id="p0998165161717"><a name="p0998165161717"></a><a name="p0998165161717"></a>**handle**</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.30916908309169%" headers="mcps1.2.4.1.2 "><p id="p179981351176"><a name="p179981351176"></a><a name="p179981351176"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="73.06269373062693%" headers="mcps1.2.4.1.3 "><p id="p89981571714"><a name="p89981571714"></a><a name="p89981571714"></a>Group management handle. Users call its APIs to send and receive messages, release messages, and so on.</p>
    </td>
    </tr>
    </tbody>
    </table>

    The following is a code example of the callback computation structure of an operator.

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

    Use the [CreateCubeResGroup](CreateCubeResGroup.md) API to create one or more CubeResGroupHandle objects.

    ```
    /* 
     * groupID is the user-defined groupID of CreateCubeResGroup.
     * MatmulApiType is the type of the computation object defined on the AIC.
     * MyCallbackFunc is the defined custom callback computation structure.
     * CubeMsgBody is the custom message structure.
     * desc is the communication region description initialized by the user.
     * groupID is 1, blockStart is 0, blockSize is 12, msgQueueSize is 48, and tilingGm is a pointer that stores the tiling information required by the user on the AIC.
    */
    auto handle =  AscendC::CreateCubeResGroup<groupID, MatmulApiType, MyCallbackFunc, CubeMsgBody>(desc, 0, 12, 48, tilingGM);
    ```

6.  Bind the AIV to the CubeResGroupHandle.

    Bind the AIV and the message queue index. Note: The message queue index queIdx must be smaller than the total number of message queues of this CubeGroupHandle, and each AIV must be passed a different queIdx. handle is the CubeResGroupHandle object created by CreateCubeResGroup in [Step 5](#li355132105919).

    ```
    handle.AssignQueue(queIdx);
    ```

7.  The AIV sends a message.

    Use the [AllocMessage](AllocMessage.md), [PostMessage](PostMessage.md), and other APIs to send and receive messages. Call AllocMessage to obtain the message structure pointer, send the message through PostMessage, and call [PostFakeMessage](PostFakeMsg.md) to send a fake message in the message merging scenario. The following is an example.

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

After calling **AllocMessage** to obtain the message structure pointer, call **SendQuitMsg** to send the current message queue exit.

    ```
    auto msgPtr = handle.AllocMessage();        // Obtain the message space pointer msgPtr.
    handle.SetQuit(msgPtr);              // Send the quit message.
    ```

