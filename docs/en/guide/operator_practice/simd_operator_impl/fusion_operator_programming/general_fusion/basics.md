# Basics<a name="ZH-CN_TOPIC_0000002532388143"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:51:47.569Z pushedAt=2026-11-03T07:05:13.291Z nl=2 -->

> [!NOTE] Note
> This section provides the theoretical background and development guidance for the merged computing and communication operator. Before learning this section, ensure that you have mastered the relevant knowledge in [Cube Programming](../../matrix_advanced_api/basics.md) and [HCCL Collective Communication Library](https://gitcode.com/cann/hccl/blob/9.2.0/docs/en/user_guide/README.md).
> The general fusion operator generally supports the following product models:
>>    <!-- npu="950" id1 -->
>>    - Ascend 950PR&950DT products
>>    <!-- end id1 -->
>>    <!-- npu="A3" id2 -->
>>    - Atlas A3 products
>>    <!-- end id2 -->
>>    <!-- npu="910b" id3 -->
>>    - Atlas A2 products
>>    <!-- end id3 -->

## Merged Computing and Communication Operator<a name="zh-cn_topic_0000002366728672_section842184210813"></a>

Compared with general computation or data movement operators, the merged computing and communication (MC2) operator fuses the originally serial communication and computation operations. By splitting data inside the operator, it implements parallel execution of computation and communication tasks within the operator, thereby improving operator performance. Merged computing and communication operators are collectively referred to as MC2 operators, that is, CubeComputation & Communication.

As shown in the following figure, the ideal execution time of serial communication operators and computation operators is the sum of the execution time of the two operators. In the MC2 operator obtained by fusing communication and computation tasks, the data to be communicated and computed is split, so that the amount of data for a single communication and computation is reduced. The entire communication and computation tasks are performed in multiple rounds, enabling pipeline parallelism between computation and communication. As a result, the theoretical execution time is greatly shortened, bringing performance gains.

**Figure 1** Comparison of theoretical execution time before and after communication-computation fusion<a name="zh-cn_topic_0000002366728672_fig861111613812"></a>  
![](../../../../figures/comm_fusion_time.png "Comparison of theoretical execution time before and after communication-computation fusion")

## Scenarios and Benefits<a name="zh-cn_topic_0000002366728672_section1467294217389"></a>

As model scales grow, training and inference on a single device face bottlenecks in computing capability, memory capacity, and energy efficiency. Therefore, distributed parallel computing becomes a mandatory technical path. For the communication and computing tasks in distributed training and inference of large models, they can be classified into two types based on the dependency between communication and computing:

-   Weak dependency computing and communication tasks

    The result of communication or computing is not immediately used by the other party. Although the two are dependent, other independent computing or communication tasks can be scheduled between them. As shown in [Figure 2](#zh-cn_topic_0000002366728672_fig1840354818120), communication 1 has dependencies on computing 1-2 and computing 4, but has no dependency on computing 1-1, computing 2-1, computing 2-2, and computing 3. Communication 2 has dependencies on computing 2-2 and computing 4, but has no dependency on computing 2-1 and computing 3. Therefore, both communication 1 and communication 2 have large pipeline space and can be hidden by computing tasks that are independent of them. As shown in [Figure 3](#zh-cn_topic_0000002366728672_fig382061611816), both communication 1 and communication 2 can be hidden by independent computing tasks. In a model, such independent communication and computing can achieve task-level parallelism without operator fusion. Therefore, weak dependency computing and communication tasks are not suitable for computing-network convergence scenarios.

    **Figure 2**  Diagram of weak dependency computing and communication tasks<a name="zh-cn_topic_0000002366728672_fig1840354818120"></a>  
    ![](../../../../figures/weak_task.png "Diagram of weak dependency computing and communication tasks")

    **Figure 3**  Scheduling simulation diagram of weak dependency computing and communication tasks<a name="zh-cn_topic_0000002366728672_fig382061611816"></a>  
    ![](../../../../figures/weak_sched.png "Scheduling simulation diagram of weak dependency computing and communication tasks")

-   Strong dependency computing and communication tasks

    The result of communication or computing is immediately used by the other side, and there is a tight dependency between the two. As shown in the following figure, computing and communication tasks must be executed serially. During the execution of communication 1 and communication 2, hardware computing resources are idle. If such a communication-computing pattern appears extensively in a model, it will lead to low computing power utilization, and communication becomes the main performance bottleneck. Strong dependency computing and communication tasks are suitable for fusion into an MC2 operator, which uses computing-network convergence technology to improve performance.

    **Figure 4**  Diagram of strong dependency computing and communication tasks<a name="zh-cn_topic_0000002366728672_fig246345165920"></a>  
    ![](../../../../figures/strong_task.png "Diagram of strong dependency computing and communication tasks")

    **Figure 5**  Scheduling simulation diagram of strong dependency computing and communication tasks<a name="zh-cn_topic_0000002366728672_fig12971332388"></a>  
    ![](../../../../figures/strong_sched.png "Scheduling simulation diagram of strong dependency computing and communication tasks")

MC2 technology is closely related to the network model structure. Generally speaking, computing and communication tasks that meet the strong dependency criteria described above may achieve performance improvement through MC2 operators.
