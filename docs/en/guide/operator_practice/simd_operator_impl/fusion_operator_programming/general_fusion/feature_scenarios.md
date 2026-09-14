# Feature Scenarios<a name="ZH-CN_TOPIC_0000002500548108"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T13:48:37.534Z -->

## Re-execution<a name="zh-cn_topic_0000002400328453_section11795221469"></a>

To prevent communication interruption caused by transient hardware faults in the environment where communication tasks are executed, the computing-communication convergence operator can enable the retry capability by configuring compilation macros and environment variables. After the retry capability is enabled for the computing-communication convergence operator, when the AI CPU detects an environment exception, it notifies the AI Core to re-dispatch the communication task through the mechanism illustrated in the following figure, thereby avoiding communication interruption caused by transient hardware faults and improving communication stability.

Currently, the support for this capability is as follows:

Ascend 950PR/Ascend 950DT do not support re-execution of the computing-communication convergence operator.

Atlas A2 Training Series/Atlas A2 Inference Series do not support re-execution of the computing-communication convergence operator.

Atlas A3 Training Series/Atlas A3 Inference Series support re-execution of the computing-communication convergence operator.

**Figure 1** Communication task retry mechanism<a name="zh-cn_topic_0000002400328453_fig774612491699"></a>  
![](../../../../figures/communication_task_retry_mechanism.png "Communication task retry mechanism")

The conditions for enabling retry are as follows:

-   The output memory address and input memory address of the computing-communication convergence operator are different.
-   The computing-communication convergence operator involves only inter-server communication (a server is a computing node, generally referring to a server form composed of 8-card or 16-card Ascend NPU devices).

-   When compiling the operator, configure the compilation macro AICORE\_EXCEPTION\_RESTART as follows. For details about the stage and method of configuring the compilation macro, see [Supporting Custom Compilation Options](../../../../programming_guide/advanced_programming/aclnn_operator_development/operator_package_compilation/operator_project_compilation.md#section7321165972311).


    ```
    add_ops_compile_options(ALL OPTIONS -DAICORE_EXCEPTION_RESTART)
    ```

-   Configure the HCCL retry environment variable HCCL\_OP\_RETRY\_ENABLE to enable the detection and reporting capability of retry. For details about this environment variable, see [HCCL\_OP\_RETRY\_ENABLE](https://gitcode.com/cann/hccl/blob/9.1.0/docs/en/user_guide/hccl_env/HCCL_OP_RETRY_ENABLE.md). Set this environment variable before the operator is executed. The specific configuration is as follows:

    ```
    # L1 must be set to 1 for inter-server communication. Cross-supernode is not supported. L2 is set to 0.
    export HCCL_OP_RETRY_ENABLE="L1:1, L2:0" 
    ```

    Note that after retry is enabled, if the communication is interrupted after the AI Core delivers a communication task for the first time, the task is re-executed only once by default. To change the number of re-executions or the retransmission interval, see [HCCL\_OP\_RETRY\_PARAMS](https://gitcode.com/cann/hccl/blob/9.1.0/docs/en/user_guide/hccl_env/HCCL_OP_RETRY_PARAMS.md).
