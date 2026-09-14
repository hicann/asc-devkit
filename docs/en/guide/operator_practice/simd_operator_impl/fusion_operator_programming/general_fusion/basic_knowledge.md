# Basic Knowledge<a name="ZH-CN_TOPIC_0000002532388143"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T13:46:52.056Z -->

>[!NOTE] Note
>This section describes the theoretical background and development guide of the communication-computation fusion operator. Before learning this section, ensure that you have mastered the relevant knowledge in [Matrix Programming](../../matrix_advanced_api/basic_knowledge.md) and [*HCCL Collection Communication Library*](https://gitcode.com/cann/hccl/blob/9.1.0/docs/en/user_guide/README.md).
>The communication-computation fusion operator generally supports the following product models:
>>    - Ascend 950PR/Ascend 950DT
>>    - Atlas A3 training products/Atlas A3 inference products
>>    - Atlas A2 training products/Atlas A2 inference products

## Communication-Computation Fusion Operator<a name="zh-cn_topic_0000002366728672_section842184210813"></a>

Compared with common computation or data movement operators, the communication-computation fusion operator fuses the originally serial communication and computation operations. By splitting data inside the operator, it enables parallel execution of computation and communication tasks within the operator, thereby improving operator performance. Communication-computation fusion operators are collectively referred to as MC² operators, which stands for Matrix Computation & Communication.

As shown in the following figure, the ideal execution time of serial communication and computation operators is the sum of the execution times of the two operators. In a communication-computation fusion operator obtained by fusing communication and computation tasks, the data to be communicated and computed is split, reducing the amount of data processed in a single communication and computation operation. The entire communication and computation task is performed in multiple rounds, enabling pipeline parallelism between computation and communication. As a result, the theoretical execution time is greatly shortened, bringing performance gains.

**Figure 1** Theoretical execution time comparison diagram before and after communication-computation convergence<a name="zh-cn_topic_0000002366728672_fig861111613812"></a>  
![](../../../../figures/theoretical_execution_time_comparison_diagram_before_and_after_communication_computation_convergence.png "Theoretical execution time comparison diagram before and after communication-computation convergence")

## Scenarios and Benefits<a name="zh-cn_topic_0000002366728672_section1467294217389"></a>

As model scales grow, training and inference on a single device face bottlenecks in computing capability, memory capacity, and energy efficiency. Therefore, distributed parallel computing becomes a mandatory technical path. For the communication and computing tasks in distributed training and inference of large models, they can be classified into two categories based on the dependency between communication and computing:

-   Weakly dependent computing-communication tasks

    The result of communication or computing is not immediately used by the other. Although the two are dependent, other independent computing or communication tasks can be scheduled between them. As shown in [Figure 2](#zh-cn_topic_0000002366728672_fig1840354818120), communication 1 depends on computing 1-2 and computing 4, but has no dependency on computing 1-1, computing 2-1, computing 2-2, and computing 3. Communication 2 depends on computing 2-2 and computing 4, but has no dependency on computing 2-1 and computing 3. Therefore, both communication 1 and communication 2 have a large pipeline space and can be hidden by the computing tasks that are independent of them. As shown in [Figure 3](#zh-cn_topic_0000002366728672_fig382061611816), both communication 1 and communication 2 can be hidden by independent computing tasks. In a model, such independent communication and computing can achieve task-level parallelism without operator fusion. Therefore, weakly dependent computing-communication tasks are not suitable for computing-communication convergence scenarios.

    **Figure 2**  Weakly dependent computing-communication task diagram<a name="zh-cn_topic_0000002366728672_fig1840354818120"></a>  
    ![](../../../../figures/weakly_dependent_computing_communication_task_diagram.png "weakly dependent computing-communication task diagram")

    **Figure 3**  Task scheduling simulation diagram of weakly dependent computing-communication tasks<a name="zh-cn_topic_0000002366728672_fig382061611816"></a>  
    ![](../../../../figures/weakly_dependent_computing-communication_task_scheduling_simulation_diagram.png "task scheduling simulation diagram of weakly dependent computing-communication tasks")

-   Strongly dependent computing-communication tasks

    The result of communication or computing is immediately used by the other, and there is a tight dependency between the two. As shown in the following figure, the computing-communication tasks must be executed serially. During the execution of communication 1 and communication 2, the hardware computing resources are idle. If such a communication-computing pattern appears extensively in a model, it leads to low compute utilization, and communication becomes the main performance bottleneck. Strongly dependent computing-communication tasks are suitable for fusion into communication-computation fusion operators, which leverage computing-communication convergence technology to improve performance.

    **Figure 4**  Strongly dependent computing-communication task diagram<a name="zh-cn_topic_0000002366728672_fig246345165920"></a>  
    ![](../../../../figures/strongly_dependent_computing_communication_task_diagram.png "strongly dependent computing-communication task diagram")

    **Figure 5**  Task scheduling simulation diagram of strongly dependent computing-communication tasks<a name="zh-cn_topic_0000002366728672_fig12971332388"></a>  
    ![](../../../../figures/strongly_dependent_computing_communication_task_scheduling_simulation_diagram.png "task scheduling simulation diagram of strongly dependent computing-communication tasks")

The computing-communication convergence technology is closely related to the network model structure. Generally, any task that matches the strongly dependent computing-communication task described above may achieve performance improvement through the communication-computation fusion operator.
