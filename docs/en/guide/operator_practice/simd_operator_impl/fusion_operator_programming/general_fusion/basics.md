# Basic Knowledge<a name="ZH-CN_TOPIC_0000002532388143"></a>

<!-- md-trans-meta sourceCommit=b6142b1cb0ae3dfa4b9063f8bd59db99b19fc426 translatedAt=2026-11-09T10:28:48.456Z pushedAt=2026-11-19T06:32:19.728Z nl=2 -->

> [!NOTE] Note 
> This section describes the theoretical background and development guidance of the MC² operator. Before learning this section, ensure that you have mastered the relevant knowledge in [Cube Programming](../../matrix_advanced_api/basics.md) and [HCCL Collective Communication Library](https://gitcode.com/cann/hccl/blob/master/docs/en/user_guide/README.md).
> The MC² operator generally supports the following products:
>    <!-- npu="950" id1 -->
>    - Ascend 950PR&950DT products
>    <!-- end id1 -->
>    <!-- npu="A3" id2 -->
>    - Atlas A3 products
>    <!-- end id2 -->
>    <!-- npu="910b" id3 -->
>    - Atlas A2 products
>    <!-- end id3 -->

## MC² Operator<a name="zh-cn_topic_0000002366728672_section842184210813"></a>

Compared with general computation or data movement operators, the MC² operator fuses the originally serial communication and computation operations. By partitioning data inside the operator, it enables parallel execution of computation and communication tasks within the operator, thereby improving operator performance. MC² operators refers to operators that merges compute and communication, that is, cube computation & communication.

As shown in the following figure, the ideal execution time of serial communication and computation operators is the sum of the execution times of the two operators. In an MC² operator obtained by fusing communication and computation tasks, the data to be communicated and computed is partitioned, reducing the amount of data for a single communication and computation operation. The entire communication and computation task is performed in multiple rounds, enabling pipeline parallelism between computation and communication. As a result, the theoretical execution time is greatly shortened, bringing performance gains.

**Figure 1**  Comparison of theoretical execution time before and after communication-compute fusion<a name="zh-cn_topic_0000002366728672_fig861111613812"></a>  
![](../../../../figures/comm_fusion_time.png "Comparison of theoretical execution time before and after communication-compute fusion")

## Scenarios and Benefits<a name="zh-cn_topic_0000002366728672_section1467294217389"></a>

As model scales grow, training and inference on a single device face bottlenecks in computing capability, memory capacity, and energy efficiency. Therefore, distributed parallel computing becomes a mandatory technical path. For the communication and computing tasks in distributed training and inference of large models, they can be classified into two types based on the dependency between communication and computing:

-   Weak dependency compute-communication tasks

    The result of communication or compute is not immediately used by the other party. Although the two are dependent, other independent compute or communication tasks can be scheduled between them. As shown in [Figure 2](#zh-cn_topic_0000002366728672_fig1840354818120), communication 1 has dependencies with compute 1-2 and compute 4, but has no dependency with compute 1-1, compute 2-1, compute 2-2, and compute 3. Communication 2 has dependencies with compute 2-2 and compute 4, but has no dependency with compute 2-1 and compute 3. Therefore, both communication 1 and communication 2 have large pipeline space and can be hidden by computing tasks that have no dependency with them. As shown in [Figure 3](#zh-cn_topic_0000002366728672_fig382061611816), both communication 1 and communication 2 can be hidden by independent computing tasks. In a model, such independent communication and compute can achieve task-level parallelism without operator fusion. Therefore, weak dependency compute-communication tasks are not applicable to MC² scenarios.

    **Figure 2**  Schematic diagram of weak dependency compute-communication tasks<a name="zh-cn_topic_0000002366728672_fig1840354818120"></a>  
    ![](../../../../figures/weak_task.png "Schematic diagram of weak dependency compute-communication tasks")

    **Figure 3**  Schematic diagram of scheduling simulation for weak dependency compute-communication tasks<a name="zh-cn_topic_0000002366728672_fig382061611816"></a>  
    ![](../../../../figures/weak_sched.png "Schematic diagram of scheduling simulation for weak dependency compute-communication tasks")

-   Strong dependency compute-communication tasks

    The result of communication or compute is immediately used by the other party, and there is a close dependency between the two. As shown in the following figure, compute-communication tasks must be executed serially. During the execution of communication 1 and communication 2, hardware computing resources are idle. If such communication-computing patterns appear in large numbers in a model, the computing power utilization will be low, and communication will become the main performance bottleneck. Strong dependency compute-communication tasks are suitable for fusion into MC² operators, which use MC² technology to improve performance.

    **Figure 4**  Schematic diagram of strong dependency compute-communication tasks<a name="zh-cn_topic_0000002366728672_fig246345165920"></a>  
    ![](../../../../figures/strong_task.png "Schematic diagram of strong dependency compute-communication tasks")

    **Figure 5**  Schematic diagram of scheduling simulation for strong dependency compute-communication tasks<a name="zh-cn_topic_0000002366728672_fig12971332388"></a>  
    ![](../../../../figures/strong_sched.png "Schematic diagram of scheduling simulation for strong dependency compute-communication tasks")

The MC² technology is closely related to the network model structure. Generally, compute-communication tasks that meet the preceding strong dependency conditions can achieve performance improvement through the MC² operator.
