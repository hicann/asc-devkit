# Feature Scenarios<a name="ZH-CN_TOPIC_0000002500548108"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T10:28:40.517Z pushedAt=2026-11-19T06:32:19.727Z nl=2 -->

## Retry<a name="zh-cn_topic_0000002400328453_section11795221469"></a>

To prevent communication interruption caused by transient hardware faults in the environment where communication tasks are executed, MC² operators can enable retry by configuring compilation macros and environment variables. After retry is enabled for an MC² operator, when the AI CPU detects an environment exception, it notifies the AI Core to re-dispatch the communication task through the mechanism shown in the following figure, thereby avoiding communication interruption caused by transient hardware faults and improving communication stability.

Currently, the support for this capability is as follows:

<!-- npu="950" id1 -->
Ascend 950PR&950DT products do not support retry for MC² operators.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 products support retry for MC² operators.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 products do not support retry for MC² operators.
<!-- end id3 -->

**Figure 1** Communication task retry mechanism<a name="zh-cn_topic_0000002400328453_fig774612491699"></a>  
![](../../../../figures/comm_reexec.png "Communication task retry mechanism")

The conditions for enabling retry are as follows:

-   The output memory address of the MC² operator is different from its input memory address.
-   The MC² operator involves only inter-server communication (a server is a computing node, generally referring to a server form composed of 8 or 16 Ascend NPU devices).

-   When compiling the operator, configure the compile macro **AICORE\_EXCEPTION\_RESTART** as follows. For details about the specific stage and method of compile macro configuration, see [Supported Custom Compile Options](../../../../programming_guide/advanced_programming/aclnn_operator_development/compilation_and_deployment/basic_process.md#custom-compile-options).


    ```
    add_ops_compile_options(ALL OPTIONS -DAICORE_EXCEPTION_RESTART)
    ```

-   Configure the HCCL retry environment variable **HCCL\_OP\_RETRY\_ENABLE** to enable the retry detection and reporting capability. For details about this environment variable, see [HCCL\_OP\_RETRY\_ENABLE](https://gitcode.com/cann/hccl/blob/master/docs/en/user_guide/hccl_env/HCCL_OP_RETRY_ENABLE.md). Set this environment variable before executing the operator. The specific configuration is as follows:

    ```
    # L1 between servers must be set to 1. Cross-supernode is not supported. L2 is set to 0.
    export HCCL_OP_RETRY_ENABLE="L1:1, L2:0" 
    ```

    Note that after retry is enabled, if the communication is interrupted after the AI Core delivers the communication task for the first time, retry is performed only once by default. To modify the number of retries or the retransmission interval, see [HCCL\_OP\_RETRY\_PARAMS](https://gitcode.com/cann/hccl/blob/master/docs/en/user_guide/hccl_env/HCCL_OP_RETRY_PARAMS.md).
