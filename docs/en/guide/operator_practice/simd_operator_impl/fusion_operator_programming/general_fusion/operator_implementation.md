# Operator Implementation<a name="ZH-CN_TOPIC_0000002500468252"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:49:27.478Z -->

In the implementation of communication-computation fusion operators, communication operations use the [Hccl high-level API](../../../../../api/SIMD-API/advanced_api/hccl_communication/HCCL-Kernel/hccl_usage.md), and Cube computation operations use the [Matmul high-level API](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_kernel_api.md). For more information about collective communication and related concepts, see [*HCCL Collective Communication Library*](https://gitcode.com/cann/hccl/blob/9.1.0/docs/zh/user_guide/README.md). The development process of communication-computation fusion operators is the same as that of general operators. However, note that communication-computation fusion operators currently do not support [Kernel direct invocation](../../../../programming_guide/appendix/completing_kernel_launch_based_on_sample_project.md) or [graph (GE graph) development](../../../../programming_guide/advanced_programming/operator_graph_development/overview.md). Only [single operator API call](../../../../programming_guide/advanced_programming/aclnn_operator_development/single_operator_api_call.md) is supported.

The following uses the implementation of the AllGatherMatmulCustom operator (referred to as AllGatherMatmul) as an example to describe the design and implementation process of communication-computation fusion operators from the aspects of operator analysis, data flow analysis, creating an operator project, prototype definition, tiling implementation, kernel implementation, and compilation and running. For the complete code of the operator in this sample, see [AllGatherMatmul sample](https://gitcode.com/cann/ops-transformer/tree/9.1.0/mc2/all_gather_matmul_v2). This sample can run only on **Atlas A2 training products/Atlas A2 inference products**.

## Operator Analysis<a name="zh-cn_topic_0000002400208581_section59611034123213"></a>

Operator analysis refers to clarifying the mathematical expression, inputs, outputs, and kernel function name of an operator.

1.  Clarify the mathematical expression and communication computation logic of the operator.

    The AllGatherMatmul operator implements the fusion of [AllGather](../../../../../api/SIMD-API/advanced_api/hccl_communication/HCCL-Kernel/AllGather.md) communication and [Matmul](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_kernel_api.md) matrix multiplication. The operator logic is as follows: perform AllGather communication on the input communication matrix a to obtain the left matrix for Matmul computation, that is, the communication result gather\_out, and then perform Matmul computation on gather\_out and the right matrix b to obtain the output c. The corresponding mathematical expression is:

    ```
    gather_out = AllGather(a)
    c = gather_out ∗ b
    ```

2.  Clarify the inputs, outputs, and attributes.
    -   a and b are source operands. a is the input matrix for communication, with a shape of \[M, K\]; b is the right matrix for Matmul, with a shape of \[K, N\]. In the sample, M, K, and N are fixed to 512, 5120, and 640, respectively.
    -   gather\_out is the destination operand that stores the AllGather communication result, with a shape of \[M \* rankDim, K\], where rankDim is the number of devices in the communication domain, fixed to 8 in the sample.
    -   c is the destination operand that stores the Matmul computation result, with a shape of \[M \* rankDim, N\].
    -   The data type of the operator inputs and outputs is float16, and the [format](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md) is ND.
    -   group is an operator attribute that indicates the name of the communication domain and specifies the communication domain in which the operator runs.

3.  Determine the kernel function name and parameters.
    -   In this sample, the kernel function is named all\_gather\_matmul\_custom.
    -   Based on the analysis of the operator inputs and outputs, determine the kernel function parameters aGM, bGM, cGM, and gatherOutGM. aGM and bGM are the memory addresses of the inputs in Global Memory, and cGM and gatherOutGM are the memory addresses of the outputs in Global Memory. Note that the kernel function parameters differ in naming from the inputs and outputs of the single operator API call. This is because the kernel function parameters are the memory addresses of the inputs and outputs in Global Memory, whereas the input and output types of the single operator API call are aclTensor, and the two are not exactly the same.

4.  Determine the interfaces required for operator implementation.
    -   The operator involves AllGather communication. Check the communication-related interfaces in the Ascend C API Reference. You need to use the [Hccl high-level API](../../../../../api/SIMD-API/high-level API/HCCL communication class/HCCL-Kernel side interface/HCCL usage instructions.md) to implement AllGather communication.
    -   The operator involves data movement of the left and right Matmul matrices between external storage and internal storage. Check the data movement interfaces in the Ascend C API Reference. You need to use [DataCopy](../../../../../api/SIMD-API/basic API/data movement guide/overview/data movement concepts.md) to implement data movement.
    -   The computation process involves Cube computation operations. Check the Cube computation-related interfaces in the Ascend C API Reference. You need to use the [Matmul high-level API](../../../../../api/SIMD-API/high-level API/Cube computation/Matmul-Kernel side interface/Matmul-Kernel side interface.md) to implement matrix multiplication computation.

**Table 1**  AllGatherMatmulCustom operator specifications

<a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_table164881913121819"></a>
<table><tbody><tr id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row1848911314188"><th class="firstcol" valign="top" id="mcps1.2.6.1.1"><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p54891613141818"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p54891613141818"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p54891613141818"></a>Operator Type (OpType)</p></th>
<td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.1.1 "><p id="zh-cn_topic_0000002400208581_p13423114244112"><a name="zh-cn_topic_0000002400208581_p13423114244112"></a><a name="zh-cn_topic_0000002400208581_p13423114244112"></a><span>AllGatherMatmulCustom</span></p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_row10188125911198"><th class="firstcol" rowspan="5" valign="top" width="19.8%" id="mcps1.2.6.2.1"><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p17188125981915"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p17188125981915"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p17188125981915"></a>Operator input and output</p></th>
<td class="cellrowborder" valign="top" width="19.189999999999998%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p11189145916192"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p11189145916192"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p11189145916192"></a><strong id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b12391113055716"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b12391113055716"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b12391113055716"></a>name</strong></p></td>
<td class="cellrowborder" valign="top" width="16.39%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p918935961919"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p918935961919"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p918935961919"></a><strong id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b9405203015715"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b9405203015715"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b9405203015715"></a>shape</strong></p></td>
<td class="cellrowborder" valign="top" width="15.09%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1618925919195"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1618925919195"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1618925919195"></a><strong id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b4418183055720"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b4418183055720"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b4418183055720"></a>data type</strong></p></td>
<td class="cellrowborder" valign="top" width="29.53%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p61891259191918"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p61891259191918"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p61891259191918"></a><strong id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b64181730165715"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b64181730165715"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_b64181730165715"></a>format</strong></p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row19489161331815"><td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p134891113131814"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p134891113131814"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p134891113131814"></a>a</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1283525118245"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1283525118245"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1283525118245"></a>[512, 5120]</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_p20241145111611"><a name="zh-cn_topic_0000002400208581_p20241145111611"></a><a name="zh-cn_topic_0000002400208581_p20241145111611"></a><span>float16</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p173195184235"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p173195184235"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p173195184235"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row1348951391811"><td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p13489191311184"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p13489191311184"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p13489191311184"></a>b</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p10959231592"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p10959231592"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p10959231592"></a>[5120, 640]</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p76241246152416"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p76241246152416"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p76241246152416"></a><span>float16</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p2795442191816"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p2795442191816"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p2795442191816"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row2489171310187"><td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p34899136188"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p34899136188"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p34899136188"></a>c</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p191831924155919"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p191831924155919"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p191831924155919"></a>[4096, 640]</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_p169522189345"><a name="zh-cn_topic_0000002400208581_p169522189345"></a><a name="zh-cn_topic_0000002400208581_p169522189345"></a><span>float16</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1752074519175"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1752074519175"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1752074519175"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_row162944819437"><td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_p96307488430"><a name="zh-cn_topic_0000002400208581_p96307488430"></a><a name="zh-cn_topic_0000002400208581_p96307488430"></a>gather_out</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_p263034894317"><a name="zh-cn_topic_0000002400208581_p263034894317"></a><a name="zh-cn_topic_0000002400208581_p263034894317"></a>[<span>4096, 5120]</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_p172861724113418"><a name="zh-cn_topic_0000002400208581_p172861724113418"></a><a name="zh-cn_topic_0000002400208581_p172861724113418"></a><span>float16</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002400208581_p18630248194310"><a name="zh-cn_topic_0000002400208581_p18630248194310"></a><a name="zh-cn_topic_0000002400208581_p18630248194310"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_row1855654151418"><th class="firstcol" valign="top" id="mcps1.2.6.7.1"><p id="zh-cn_topic_0000002400208581_p105561341101420"><a name="zh-cn_topic_0000002400208581_p105561341101420"></a><a name="zh-cn_topic_0000002400208581_p105561341101420"></a>Operator attribute</p></th>
<td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.7.1 "><p id="zh-cn_topic_0000002400208581_p1310191961513"><a name="zh-cn_topic_0000002400208581_p1310191961513"></a><a name="zh-cn_topic_0000002400208581_p1310191961513"></a>group (char*), a string that identifies the communication domain on the Host side, indicating the communication domain name.</p></td>
</tr>
<tr id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_row628260192410"><th class="firstcol" valign="top" id="mcps1.2.6.8.1"><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p22821601246"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p22821601246"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p22821601246"></a>Kernel function name</p></th>
<td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.8.1 "><p id="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1359311404260"><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1359311404260"></a><a name="zh-cn_topic_0000002400208581_zh-cn_topic_0000001514387805_p1359311404260"></a>all_gather_matmul_custom</p></td>
</tr>
</tbody>
</table>

## Data Flow Analysis<a name="zh-cn_topic_0000002400208581_section635081610514"></a>

The data of the AllGatherMatmul operator undergoes AllGather communication between cards and Matmul computation within a card. Communication and computation are performed multiple times based on the main blocks and tail blocks after data partitioning, and the pipelines overlap each other. During the analysis, assume that the communication matrix is partitioned along the M axis, with the number of main blocks (tileCnt) being 2 and the number of tail blocks (tailCnt) being 1. Then the communication-computation overlap illustration is obtained as follows.

**Figure 1**  AllGatherMatmul communication-computation overlap illustration<a name="zh-cn_topic_0000002400208581_fig17191724365"></a>  
![](../../../../figures/allgathermatmul_communication_computation_overlap_illustration.png "AllGatherMatmul communication-computation overlap illustration")

The function of AllGather is to reorder the inputs of all cards in the communication domain by card ID, concatenate them, and then send the result to all cards. Therefore, the AllGather result contains the local card data, that is, the communication matrix a input by the local card. The operator does not need to wait for the communication of this part of data to complete, nor does it need to partition the data. It can directly perform Matmul computation based on the complete communication matrix a. The AllGatherMatmul operator first performs Matmul computation on the local card data. The benefit is that the communication of main block 1 can overlap with the Matmul computation. Meanwhile, the computation of main block 1, main block 2, and tail block 1 no longer needs to include the Matmul computation on the local card data, which reduces the computation workload of subsequent main and tail blocks and increases the overlap ratio of communication and computation, thereby improving performance. Note that not all communication-computation fusion operators are suitable for performing Matmul computation on the local card data first. Because the communication of the AllGatherMatmul operator occurs before computation, performing Matmul computation on the local card data first can achieve overlap between the local card data computation and the first communication. For operators whose computation occurs before communication, such as MatmulAllReduce, it is recommended to place the computation of the local card data at the end so that it overlaps with the last communication, as shown in the following figure.

**Figure 2**  MatmulAllReduce communication-computation overlap illustration<a name="zh-cn_topic_0000002400208581_fig1392814448434"></a>  
![](../../../../figures/matmulallreduce_communication_computation_overlap_illustration.png "MatmulAllReduce communication-computation overlap illustration")

Operator logic analysis of AllGatherMatmul:

1.  The AI Core writes the communication information to be executed into the message area in Global Memory to deliver tasks. The message area is a Global Memory area at a specific address. The AI Core and AI CPU write to and poll-read from it to transfer messages between them. These operations are uniformly encapsulated in the [Hccl high-level API](../../../../../api/SIMD-API/advanced_api/hccl_communication/HCCL-Kernel/hccl_usage.md).

    **Figure 3**  Communication process illustration of the communication-computation fusion operator<a name="zh-cn_topic_0000002400208581_fig472019544819"></a>  
    ![](../../../../figures/communication_computation_fusion_operator_communication_process_illustration.png "Communication process illustration of the communication-computation fusion operator")

2.  The AI CPU reads all communication task information from the message area and starts to execute the first round of AllGather collective communication tasks over links such as HCCS (Huawei Cache Coherence System, used for high-speed interconnection between CPUs and NPUs) or RoCE (RDMA over Converged Ethernet, that is, an RDMA communication mode across Ethernet). Meanwhile, the AI Core starts Matmul computation on the local card data.

    The following figure shows the first round of communication and the local card computation when the number of communication cards is 4. tile 1 indicates that the figure shows the processing flow of the first round of communication and the matrix multiplication computation that overlaps with it. In the figure, a number in the form of X-Y in a small matrix after partitioning indicates that the data block where it resides corresponds to the Y-th block of data on the X-th card.

    **Figure 4**  Schematic diagram of the first-round communication of AllGatherMatmul and the local data matrix multiplication on rank0<a name="zh-cn_topic_0000002400208581_fig21958184612"></a>  
    ![](../../../../figures/allgathermatmul_first_round_communication_and_local_data_matrix_multiplication_on_rank0.png "Schematic diagram of the first-round communication of AllGatherMatmul and the local data matrix multiplication on rank0")

3.  After the AI CPU completes the first-round communication task, it writes a message indicating that the first-round communication task is complete to the message area, and starts executing the second-round communication task. Meanwhile, after the AI Core completes the Matmul computation on the local data, it polls the message area until it obtains the message indicating that the first-round communication task is complete, and then starts the Matmul computation on the first-round communication result, that is, main block 1.

    The following figure shows the schematic diagram of the second-round communication and the computation on rank0 when the number of communication cards is 4. tile 2 indicates that the figure illustrates the processing flow of the second-round communication and the matrix multiplication computation that overlaps with it.

    **Figure 5**  Schematic diagram of the second-round communication of AllGatherMatmul and the matrix multiplication of main block 1 on rank0<a name="zh-cn_topic_0000002400208581_fig186371833121311"></a>  
    ![](../../../../figures/allgathermatmul_second_round_communication_and_matrix_multiplication_of_main_block_1_on_rank0.png "Schematic diagram of the second-round communication of AllGatherMatmul and the matrix multiplication of main block 1 on rank0")

4.  Similar to step 3, complete the communication and computation of all remaining data blocks one by one.

## Creating an Operator Project<a name="zh-cn_topic_0000002400208581_section121335271825"></a>

Creating an operator project for a communication-computation fusion operator is the same as for a general operator. For details, see [Creating an Operator Project](../../../../programming_guide/advanced_programming/aclnn_operator_development/operator_project_creation.md). This sample creates an operator project for the AllGatherMatmul operator based on the following prototype definition JSON file by using the custom operator project generation tool msOpGen.

```
[
    {
        "op": "AllGatherMatmulCustom",
        "input_desc": [
            {
                "name": "a",
                "param_type": "required",
                "format": [
                    "ND"
                ],
                "type": [
                    "float16"
                ]
            },
            {
                "name": "b",
                "param_type": "required",
                "format": [
                    "ND"
                ],
                "type": [
                    "float16"
                ]
            }
        ],
        "output_desc":[
            {
                "name": "c",
                "param_type": "required",
                "format": [
                    "ND"
                ],
                "type": [
                    "float16"
                ]
            },
            {
                "name": "gather_out",
                "param_type": "required",
                "format": [
                    "ND"
                ],
                "type": [
                    "float16"
                ]
            }
        ],
        "attr": [
            {
                "name": "group",
                "type": "string",
                "default_value":"",
                "param_type":"required"
            }
        ]
    }
]
```

## Operator Prototype Definition<a name="zh-cn_topic_0000002400208581_section7112164101213"></a>

Compared with general operators, a communication-computation fusion operator has the following constraints when implementing the [operator prototype definition](../../../../programming_guide/advanced_programming/aclnn_operator_development/operator_prototype_definition.md):

-   An attribute representing the communication domain name of the operator must be defined. A communication domain is the context in which collective communication is executed, managing the corresponding communication entities (for example, an NPU is a communication entity) and the resources required for communication.
-   The operator must be registered as a communication-computation fusion operator through the MC2 interface in prototype registration, and the communication domain name of the operator must be configured through the HcclGroup interface.

The AllGatherMatmul operator uses the "group" attribute to represent the communication domain name of the operator, which is defined in the operator prototype as follows:

```
this->Attr("group").AttrType(REQUIRED).String(); // "group" is an attribute of the communication-computation fusion operator, representing the communication domain name. The String type in the prototype definition corresponds to the char* type in the single operator API.
...
this->MC2().HcclGroup("group"); // Configure the "group" attribute as the communication domain name of the operator.
```

The complete prototype definition of the AllGatherMatmul operator is as follows:

```
namespace ops {
class AllGatherMatmulCustom : public OpDef {
public:
    explicit AllGatherMatmulCustom(const char *name) : OpDef(name)
    {
        this->Input("a")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->Input("b")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND})
            .IgnoreContiguous();
        this->Output("c")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->Output("gather_out")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});

        this->Attr("group").AttrType(REQUIRED).String();

        this->AICore().SetTiling(AllGatherMatmulCustomTilingFunc); // Register AllGatherMatmulCustomTilingFunc as the Tiling entry function.
        this->AICore().AddConfig("ascendxxx"); // Replace ascendxxx with the corresponding AI processor model.
        this->MC2().HcclGroup("group");
    }
};
OP_ADD(AllGatherMatmulCustom);
}
```

## Tiling Implementation<a name="zh-cn_topic_0000002400208581_section59395271415"></a>

The design of the tiling strategy for a communication-computation fusion operator mainly includes the communication partitioning strategy, Matmul multi-core partitioning, and intra-core partitioning strategy.

-   Communication partitioning strategy: The size of the data block for each round of communication has a significant impact on the performance of the communication-computation fusion operator. In the sample, the M axis of communication matrix A is partitioned according to the main block M-axis length 448. For details about how to determine the partitioning strategy in specific scenarios, see [MC² Operator Performance Tuning Case](../../../best_practices/mc_operator_performance_tuning_case.md).
-   Matmul multi-core partitioning and intra-core partitioning:

    -   Multi-core partitioning: Based on the current number of cores, perform multi-core partitioning on M, K, and N of the input shape to obtain the per-core shape sizes singleCoreM, singleCoreK, and singleCoreN.
    -   Intra-core partitioning: Based on the size constraint of Local Memory, further partition the per-core shape size to obtain the shape sizes baseM, baseN, and baseK of matrices A, B, and C that participate in a single matrix multiplication instruction.

    As described above, the communication matrix is partitioned into a main block and a tail block. The communication results of the main block and tail block, as well as the local rank data, need to undergo Matmul computation separately. As shown in the following figure, the lengths of the main block, tail block, and local rank data on the M axis are tileM, tailM, and rankM, respectively. That is, the left matrix in Matmul computation has three different shapes. Therefore, it is necessary to use the sizes of the communication matrix main block, tail block, and local rank data block as the original input shapes of matrix multiplication, call the tiling API provided by the [Matmul high-level API](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_kernel_api.md), and obtain the multi-core partitioning and intra-core partitioning strategies corresponding to these three shapes. For details about the concepts and principles of singleCoreM and baseM, see [Basic Knowledge](basic_knowledge.md).

    **Figure 6**  Matrix multiplication diagram of the AllGatherMatmul operator on rank0<a name="zh-cn_topic_0000002400208581_fig189671051177"></a>  
    ![](../../../../figures/allgathermatmul_operator_matrix_multiplication_diagram_on_rank0.png "Matrix multiplication diagram of the AllGatherMatmul operator on rank0")

The key steps of the tiling implementation are as follows:

1.  Define the tiling structure of the AllGatherMatmul operator.

    The Tiling structure of a communication-computation fusion operator obtained by fusing communication and Matmul generally consists of the following three parts:

    -   [Tiling structure of the Hccl high-level API](../../../../../api/SIMD-API/advanced_api/hccl_communication/HCCL-Tiling/tilingdata_structure.md). Defines the Mc2InitTiling and Mc2CcTiling parameters. The Mc2InitTiling parameter is used to initialize the communication task configuration and must be defined as the first parameter of the operator Tiling structure. Mc2CcTiling specifies the parameter configuration of each specific communication task. Since the AllGatherMatmul operator has only one communication task, AllGather, only one Mc2CcTiling parameter needs to be defined.
    -   [TCubeTiling, the Tiling structure of the Matmul high-level API](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/tcubetiling_structure.md). Generally, the shapes of the main block, tail block, and local data differ. Since TCubeTiling can only store the result of Tiling computation for a single input shape, the Tiling structures of the main block, tail block, and local data block must be defined separately to store their multi-core partitioning and intra-core partitioning strategies.
    -   The custom structure AllGatherMatmulTiling additionally required by the AllGatherMatmul operator.

    The complete Tiling structure definition of the AllGatherMatmul operator is as follows:

    ```
    struct AllGatherMatmulTiling {
        uint32_t rankM;           // Length of the M axis of matrix A
        uint32_t rankN;           // Length of the N axis of matrix B
        uint32_t rankK;           // Length of the K axis of matrices A and B
        uint32_t tileNum;         // Number of main blocks
        uint32_t tailM;           // Length of the M axis of the tail block
        uint32_t tailNum;         // Number of tail blocks (0 or 1)
    };
    
    class AllGatherMatmulCustomTilingData {
    public:
        Mc2InitTiling mc2InitTiling;
        Mc2CcTiling mc2CcTiling;
        TCubeTiling localTiling;
        TCubeTiling tileTiling;
        TCubeTiling tailTiling;
        AllGatherMatmulTiling cfg;
    };
    ```

2.  Obtain the pointer to the Tiling structure object of the AllGatherMatmul operator.

    ```
    AllGatherMatmulCustomTilingData *tiling = context->GetTilingData<AllGatherMatmulCustomTilingData>();
    ```

    context is the object pointer of TilingContext. It is automatically passed in by the framework from the registered Tiling entry function AllGatherMatmulCustomTilingFunc and is used to save the context of the operator Tiling computation. In the Tiling implementation of the AllGatherMatmul operator, this context is used to obtain the parameters required for Tiling computation, such as the input/output shapes and input attributes, and then the Tiling results (for example, TilingKey and TilingData) are saved to the context for subsequent operator execution.

3.  Set the parameters of the custom Tiling structure of the operator.

    ```
    tiling->cfg.tileNum = rankM / TILE_M; // In the sample, TILE_M is the constant 448, indicating the length of the main block on the M axis after the communication data block is partitioned.
    tiling->cfg.tailM = rankM % TILE_M;
    tiling->cfg.tailNum = (rankM % TILE_M == 0) ? 0 : 1;
    tiling->cfg.rankM = rankM;
    tiling->cfg.rankN = rankN;
    tiling->cfg.rankK = rankK;
    ```

4.  Set the Tiling structure of the [Matmul high-level API](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_kernel_api.md).

    Obtain the TCubeTiling structure through matmul\_tiling::MultiCoreMatmulTiling. First, create the multi-core Tiling object mmTiling, then set the parameter type information of A, B, and C, the shape information of M, N, and K, and finally call the [GetTiling](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/GetTiling.md) API to obtain the Tiling information. For details, see [Matmul Tiling class](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/matmul_tiling_class.md). In the AllGatherMatmul operator, the preceding logic is encapsulated into the matmulTilingFunc function, which is then called based on the shapes of the main block, tail block, and local data to obtain the corresponding TCubeTiling parameters.

    ```
    // Encapsulate the function for setting the TCubeTiling structure as matmulTilingFunc.
    auto matmulTilingFunc = [&](int64_t m, int64_t n, int64_t k, TCubeTiling &cubeTiling) -> bool {
        matmul_tiling::MultiCoreMatmulTiling mmTiling;
        mmTiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
        mmTiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
        mmTiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
        mmTiling.SetBias(false);
        mmTiling.SetDim(aicCoreNum);
        mmTiling.SetShape(m, n, k);
        mmTiling.SetOrgShape(m, n, k);
        mmTiling.SetBufferSpace(L1_BUFFER_SIZE);
        if (mmTiling.GetTiling(cubeTiling) != 0) {
            return false;
        }
        return true;
    };
    // Set the Matmul TCubeTiling structure of the local data.
    if (!matmulTilingFunc(rankM, rankN, rankK, tiling->localTiling)) {
        ERROR_LOG("Get local matmul tiling failed");
        return ge::GRAPH_FAILED;
    }
    // Set the Matmul TCubeTiling structure of the main block.
    if (!matmulTilingFunc(TILE_M, rankN, rankK, tiling->tileTiling)) {
        ERROR_LOG("Get tile matmul tiling failed");
        return ge::GRAPH_FAILED;
    }
    // Set the Matmul TCubeTiling structure for the tail block
    if (!matmulTilingFunc(rankM % TILE_M, rankN, rankK, tiling->tailTiling)) {
        ERROR_LOG("Get tail matmul tiling failed");
        return ge::GRAPH_FAILED;
    }
    ```

5.  Set the Hccl high-level API Tiling structure.

    Based on the communication task type and algorithm configuration, create an Mc2CcTilingConfig class object, and pass the references to the mc2InitTiling and mc2CcTiling members in the operator Tiling structure to the [GetTiling](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/GetTiling.md) method to obtain the Mc2InitTiling and Mc2CcTiling parameters to be passed to the kernel side. For details about how to use the Hccl high-level API Tiling structure, see [Hccl Tiling Usage Instructions](../../../../../api/SIMD-API/advanced_api/hccl_communication/HCCL-Tiling/hccl_tiling_usage.md).

    ```
    uint32_t opType = HCCL_CMD_ALLGATHER; // Set the communication task type
    std::string algConfig = "AllGather=level0:doublering"; // Set the communication algorithm. This parameter is reserved and does not take effect after configuration.
    uint32_t reduceType = HCCL_REDUCE_SUM; // Set the Reduce operation type. This parameter is valid only for communication tasks with a reduction operation. For AllGather communication, you can directly configure the default value HCCL_REDUCE_SUM.
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(group, opType, algConfig, reduceType);
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
    mc2CcTilingConfig.SetSkipLocalRankCopy(0); // The output gatherOut must contain the A matrix of the local card, so set this parameter to 0.
    mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
    ```

## Kernel Implementation<a name="zh-cn_topic_0000002400208581_section3459452161617"></a>

In the kernel implementation of the AllGatherMatmul operator, Matmul computation needs to be performed on left matrices of three shapes: the local card data, the main communication block, and the tail communication block. To avoid duplicate code, it is necessary to abstract a general Matmul computation function that applies to different input shapes. Before designing this Matmul computation function, you need to consider the basic information required for Matmul computation, which is listed as follows:

-   Addresses of the input matrices A and B and the output matrix C.
-   TCubeTiling structure: contains information such as the shapes and data types of matrices A, B, and C, as well as the inter-core and intra-core partitioning strategies used when matrices A and B undergo Matmul computation.

In addition to the information required for Matmul computation described above, to quickly implement Matmul matrix multiplication, you can use the Matmul object in the Matmul high-level API to perform the computation. If the Matmul object is defined inside the Matmul computation function, the Matmul object will be instantiated and its resources released each time the function is called, which causes significant runtime overhead. Therefore, this object is also passed as a parameter of the Matmul computation function to enable object reuse.

In summary, the Matmul computation function defined in the kernel implementation that applies to different input shapes is as follows. The Matmul computation function is named MatmulKernel. The input parameters aGM, bGM, and cGM represent the addresses of the original input and output matrices to be computed, the input parameter tiling represents the TCubeTiling structure, and the input parameter mm corresponds to the implementation class of the Matmul high-level API. MATMUL\_TYPE is a type alias that specializes the MatmulType template.

```
using MATMUL_TYPE = MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>;

__aicore__ inline void MatmulKernel(GM_ADDR aGM, GM_ADDR bGM, GM_ADDR cGM, TCubeTiling &tiling,
                                    Matmul<MATMUL_TYPE, MATMUL_TYPE, MATMUL_TYPE> &mm)
```

The implementation steps of the MatmulKernel function are as follows.

1.  The TCubeTiling structure stores the number of cores required for Matmul computation. On cores that do not need to perform computation, the function returns directly to end the computation.

    ```
    if (GetBlockIdx() >= tiling.usedCoreNum) {
        return;
    }
    ```

2.  The Matmul high-level API requires GlobalTensor to be used as the input and output matrices. Therefore, based on the addresses of the input matrices A, B, and C in Global Memory, define three GlobalTensors: aGlobal, bGlobal, and cGlobal.

    ```
    GlobalTensor<half> aGlobal, bGlobal, cGlobal;
    aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(aGM), tiling.M * tiling.Ka);
    bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(bGM), tiling.Ka * tiling.N);
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(cGM), tiling.M * tiling.N);
    ```

3.  To implement multi-core parallelism and improve computation efficiency, the matrix data is partitioned, and the partitioned data is distributed to different cores for processing. Here, a partitioning strategy that does not partition the K axis and partitions only the M and N axes is adopted. The schematic diagram is as follows. In this scenario, each core needs to compute the offset of the matrix data to be processed relative to the original matrix, and pass the offset matrix as the input parameter when passing matrices A, B, and C. Meanwhile, to support the processing of tail block data after core partitioning, each core needs to compute the actual singleCoreM and singleCoreN sizes to be processed, and set them in the next step by calling the Matmul high-level API.

    **Figure 7** Matmul computation core partitioning schematic diagram<a name="zh-cn_topic_0000002400208581_fig1967823019367"></a>  
    ![](../../../../figures/matmul_computation_core_split_diagram.png "Matmul computation core partitioning schematic diagram")

    ```
    int mSingleBlocks = (tiling.M + tiling.singleCoreM - 1) / tiling.singleCoreM;
    int mCoreIndex = GetBlockIdx() % mSingleBlocks;
    int nCoreIndex = GetBlockIdx() / mSingleBlocks;
    // Calculate the offset of the matrix data to be processed by the current core relative to the original matrix.
    int offsetA = mCoreIndex * tiling.Ka * tiling.singleCoreM;
    int offsetB = nCoreIndex * tiling.singleCoreN;
    int offsetC = mCoreIndex * tiling.N * tiling.singleCoreM + nCoreIndex * tiling.singleCoreN;
    // Calculate the singleCoreM/singleCoreN of the current core, which are used as input parameters of the subsequent SetTail API.
    int tailM = Std::min(tiling.M - mCoreIndex * tiling.singleCoreM, tiling.singleCoreM);
    int tailN = Std::min(tiling.N - nCoreIndex * tiling.singleCoreN, tiling.singleCoreN);
    ```

4.  Call the Matmul high-level API to set the original complete shape of the Matmul computation, the addresses of the input and output matrices processed by the current core, and the actual singleCoreM and singleCoreN sizes to be computed, and then complete the matrix multiplication.

    ```
    mm.SetOrgShape(tiling.M, tiling.N, tiling.Ka, tiling.Kb);
    mm.SetTensorA(aGlobal[offsetA]);
    mm.SetTensorB(bGlobal[offsetB]);
    mm.SetTail(tailM, tailN);
    mm.IterateAll(cGlobal[offsetC]);
    ```

The kernel function of the AllGatherMatmul operator is defined as follows. The meanings of the aGM, bGM, cGM, and gatherOutGM parameters are described in [operator analysis](#zh-cn_topic_0000002400208581_section59611034123213). workspaceGM and tilingGM indicate the addresses of the workspace and tiling data in Global Memory, respectively.

```
extern "C" __global__ __aicore__ void all_gather_matmul_custom(GM_ADDR aGM, GM_ADDR bGM, GM_ADDR cGM, GM_ADDR gatherOutGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
```

The following describes the specific steps for implementing the main process of the AllGatherMatmul operator.

1.  The Matmul computation depends on the AIC core, so the control operator logic runs only on the AIC. Use the ASCEND\_IS\_AIV macro to determine whether the current core is an AIV core. If so, return directly to end the running of the current core.

    ```
    if ASCEND_IS_AIV {
        return;
    }
    ```

2.  Register the operator Tiling structure, obtain the Tiling, and initialize TPipe.

    ```
    REGISTER_TILING_DEFAULT(AllGatherMatmulCustomTilingData);
    GET_TILING_DATA(tilingData, tilingGM);
    TPipe pipe;
    ```

3.  Define and assign values to the variables required for subsequent computation.

    ```
    auto &&localTiling = tilingData.localTiling;
    auto &&tileTiling = tilingData.tileTiling;
    auto &&tailTiling = tilingData.tailTiling;
    const auto tileNum = tilingData.cfg.tileNum;                          // Number of main blocks
    const auto tailNum = tilingData.cfg.tailNum;                          // number of tail blocks
    const auto aTileEleCnt = tileTiling.M * tileTiling.Ka;                // number of elements in the main block of the communication matrix
    const auto aTileSize = tileTiling.M * tileTiling.Ka * sizeof(half);   // size of the main block of the communication matrix in bytes
    const auto cTileSize = tileTiling.M * tileTiling.N * sizeof(half);    // size of the output matrix region corresponding to the main block of the communication matrix in bytes
    const auto aTailEleCnt = tailTiling.M * tailTiling.Ka;                // number of elements in the tail block of the communication matrix
    const auto aRankEleCnt = localTiling.M * localTiling.Ka;              // number of elements in the communication matrix
    const auto aRankSize = localTiling.M * localTiling.Ka * sizeof(half); // size of the communication matrix in bytes
    const auto cRankSize = localTiling.M * localTiling.N * sizeof(half);  // size of the output matrix region corresponding to the communication matrix in bytes
    ```

4.  Initialize the hccl object and issue the AllGather communication task.

    ```
    Hccl hccl;
    GM_ADDR contextGM = GetHcclContext<HCCL_GROUP_ID_0>();
    hccl.InitV2(contextGM, &tilingData);
    hccl.SetCcTilingV2(offsetof(AllGatherMatmulCustomTilingData, mc2CcTiling));
    auto handleId = hccl.AllGather<true>(aGM, gatherOutGM, aTileEleCnt, HcclDataType::HCCL_DATA_TYPE_FP16, aRankEleCnt, tileNum);
    auto tailHandleId = hccl.AllGather<true>(aGM + tileNum * aTileSize, gatherOutGM + tileNum * aTileSize, aTailEleCnt,
                                             HcclDataType::HCCL_DATA_TYPE_FP16, aRankEleCnt, tailNum);
    ```

5.  Initialize the Matmul object and perform Matmul computation on the data of the current card.

    ```
    Matmul<MATMUL_TYPE, MATMUL_TYPE, MATMUL_TYPE> mm;
    REGIST_MATMUL_OBJ(GetTPipePtr(), GetSysWorkSpacePtr(), mm);
    mm.Init(&localTiling);
    MatmulKernel(aGM, bGM, cGM + hccl.GetRankId() * cRankSize, localTiling, mm);
    ```

6.  Wait for the communication of the main block to complete round by round, and perform Matmul computation on it.

    ```
    auto aAddr = gatherOutGM;
    auto cAddr = cGM;
    mm.Init(&tileTiling);
    for (uint32_t i = 0; i < tileNum; i++) {
        hccl.Wait(handleId);
        for (uint32_t rankId = 0; rankId < hccl.GetRankDim(); rankId++) {
            if (rankId == hccl.GetRankId())
                continue;
            MatmulKernel(aAddr + rankId * aRankSize, bGM, cAddr + rankId * cRankSize, tileTiling, mm);
        }
        aAddr += aTileSize;
        cAddr += cTileSize;
    }
    ```

7.  Wait for the communication of the tail block to complete, and perform Matmul computation on it.

    ```
    aAddr = gatherOutGM + tileNum * aTileSize;
    cAddr = cGM + tileNum * cTileSize;
    if (tailNum > 0) {
        mm.Init(&tailTiling);
        hccl.Wait(tailHandleId);
        for (uint32_t rankId = 0; rankId < hccl.GetRankDim(); rankId++) {
            if (rankId == hccl.GetRankId())
                continue;
            MatmulKernel(aAddr + rankId * aRankSize, bGM, cAddr + rankId * cRankSize, tailTiling, mm);
        }
    }
    ```

8.  Release resources.

    ```
    mm.End();
    hccl.Finalize();
    ```

Integrating the preceding code, the complete kernel code is as follows.

```
#define ASCENDC_CUBE_ONLY
#include "kernel_operator.h"
#include "lib/matmul_intf.h"
#include "all_gather_matmul_custom_tiling.h"
using namespace AscendC;
using MATMUL_TYPE = MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>;

__aicore__ inline void MatmulKernel(GM_ADDR aGM, GM_ADDR bGM, GM_ADDR cGM, TCubeTiling &tiling,
                                    Matmul<MATMUL_TYPE, MATMUL_TYPE, MATMUL_TYPE> &mm)
{
    if (GetBlockIdx() >= tiling.usedCoreNum) {
        return;
    }

    GlobalTensor<half> aGlobal, bGlobal, cGlobal;
    aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(aGM), tiling.M * tiling.Ka);
    bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(bGM), tiling.Ka * tiling.N);
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(cGM), tiling.M * tiling.N);

    int mSingleBlocks = (tiling.M + tiling.singleCoreM - 1) / tiling.singleCoreM;
    int mCoreIndx = GetBlockIdx() % mSingleBlocks;
    int nCoreIndx = GetBlockIdx() / mSingleBlocks;
    int offsetA = mCoreIndx * tiling.Ka * tiling.singleCoreM;
    int offsetB = nCoreIndx * tiling.singleCoreN;
    int offsetC = mCoreIndx * tiling.N * tiling.singleCoreM + nCoreIndx * tiling.singleCoreN;
    int tailM = Std::min(tiling.M - mCoreIndx * tiling.singleCoreM, tiling.singleCoreM);
    int tailN = Std::min(tiling.N - nCoreIndx * tiling.singleCoreN, tiling.singleCoreN);

    mm.SetOrgShape(tiling.M, tiling.N, tiling.Ka, tiling.Kb);
    mm.SetTensorA(aGlobal[offsetA]);
    mm.SetTensorB(bGlobal[offsetB]);
    mm.SetTail(tailM, tailN);
    mm.IterateAll(cGlobal[offsetC]);
}
extern "C" __global__ __aicore__ void all_gather_matmul_custom(GM_ADDR aGM, GM_ADDR bGM, GM_ADDR cGM,
                                                               GM_ADDR gatherOutGM, GM_ADDR workspaceGM,
                                                               GM_ADDR tilingGM)
{
    if ASCEND_IS_AIV {
        return;
    }
    REGISTER_TILING_DEFAULT(AllGatherMatmulCustomTilingData);
    GET_TILING_DATA(tilingData, tilingGM);
    TPipe pipe;

    auto &&localTiling = tilingData.localTiling;
    auto &&tileTiling = tilingData.tileTiling;
    auto &&tailTiling = tilingData.tailTiling;
    const auto tileNum = tilingData.cfg.tileNum;                          // Number of main blocks
    const auto tailNum = tilingData.cfg.tailNum;                          // Number of tail blocks
    const auto aTileEleCnt = tileTiling.M * tileTiling.Ka;                // Number of elements in the main block of the communication matrix
    const auto aTileSize = tileTiling.M * tileTiling.Ka * sizeof(half);   // Size of the main block of the communication matrix in bytes
    const auto cTileSize = tileTiling.M * tileTiling.N * sizeof(half);    // Size of the main block of the output matrix in bytes
    const auto aTailEleCnt = tailTiling.M * tailTiling.Ka;                // Number of elements in the tail block of the communication matrix
    const auto aRankEleCnt = localTiling.M * localTiling.Ka;              // Number of elements in the communication matrix of a single card
    const auto aRankSize = localTiling.M * localTiling.Ka * sizeof(half); // Number of bytes of the communication matrix on a single card
    const auto cRankSize = localTiling.M * localTiling.N * sizeof(half);  // Number of bytes of the output matrix on a single card

    Hccl hccl;
    GM_ADDR contextGM = GetHcclContext<HCCL_GROUP_ID_0>();
    hccl.InitV2(contextGM, &tilingData);
    hccl.SetCcTilingV2(offsetof(AllGatherMatmulCustomTilingData, mc2CcTiling));
    auto handleId = hccl.AllGather<true>(aGM, gatherOutGM, aTileEleCnt, HcclDataType::HCCL_DATA_TYPE_FP16, aRankEleCnt, tileNum);
    auto tailHandleId = hccl.AllGather<true>(aGM + tileNum * aTileSize, gatherOutGM + tileNum * aTileSize, aTailEleCnt,
                                             HcclDataType::HCCL_DATA_TYPE_FP16, aRankEleCnt, tailNum);

    Matmul<MATMUL_TYPE, MATMUL_TYPE, MATMUL_TYPE> mm;
    REGIST_MATMUL_OBJ(GetTPipePtr(), GetSysWorkSpacePtr(), mm);
    mm.Init(&localTiling);
    MatmulKernel(aGM, bGM, cGM + hccl.GetRankId() * cRankSize, localTiling, mm);

    auto aAddr = gatherOutGM;
    auto cAddr = cGM;
    mm.Init(&tileTiling);
    for (uint32_t i = 0; i < tileNum; i++) {
        hccl.Wait(handleId);
        for (uint32_t rankId = 0; rankId < hccl.GetRankDim(); rankId++) {
            if (rankId == hccl.GetRankId())
                continue;
            MatmulKernel(aAddr + rankId * aRankSize, bGM, cAddr + rankId * cRankSize, tileTiling, mm);
        }
        aAddr += aTileSize;
        cAddr += cTileSize;
    }

    aAddr = gatherOutGM + tileNum * aTileSize;
    cAddr = cGM + tileNum * cTileSize;
    if (tailNum > 0) {
        mm.Init(&tailTiling);
        hccl.Wait(tailHandleId);
        for (uint32_t rankId = 0; rankId < hccl.GetRankDim(); rankId++) {
            if (rankId == hccl.GetRankId())
                continue;
            MatmulKernel(aAddr + rankId * aRankSize, bGM, cAddr + rankId * cRankSize, tailTiling, mm);
        }
    }

    mm.End();
    hccl.Finalize();
}
```

## Compilation and Execution<a name="zh-cn_topic_0000002400208581_section17516122912424"></a>

The following briefly describes the AllGatherMatmul sample from three steps: compilation, installation, and running.

1.  **Compilation**

    Refer to the commands for generating a custom operator project and compiling the operator in the [AllGatherMatmul sample](https://gitcode.com/cann/ops-transformer/tree/9.1.0/mc2/all_gather_matmul_v2), and run the install.sh script to complete compilation.

    The sample directory structure is as follows. The AllGatherMatmulCustom directory contains the necessary operator implementation. The install.sh script uses msOpGen to create a CustomOp directory under the 21\_all\_gather\_matmul\_custom directory, copies the operator implementation files to the corresponding directory, and then calls the build.sh compilation entry script generated by msOpGen to compile the operator.

    ```
    ├── 21_all_gather_matmul_custom
    │   ├── AclNNInvocation                  // Call the AllGatherMatmulCustom operator through aclnn
    │   ├── AllGatherMatmulCustom            // AllGatherMatmulCustom operator project
    │   ├── all_gather_matmul_custom.json    // Prototype definition JSON file of the AllGatherMatmulCustom operator
    │   ├── all_gather_matmul_demo_def.h     // Parameter configuration of the AllGatherMatmulCustom operator
    │   └── install.sh                       // Script that calls msOpGen to generate a custom operator project and compile it
    ```

    The CustomOp directory structure generated by msOpGen is as follows.

    ```
    ├── CustomOp                 // AllGatherMatmul custom operator project generated by msOpGen
    │   ├── cmake 
    │   ├── op_host             // Host-side implementation files
    │   ├── op_kernel           // Kernel-side implementation files
    │   ├── scripts             // Directory containing scripts for packaging the custom operator project
    │   ├── build.sh            // Build entry script
    │   ├── CMakeLists.txt      // CMakeLists.txt of the operator project
    │   └── CMakePresets.json   // Build configuration items
    ```

2.  **Install**

    Before deploying the custom operator package, ensure that the environment variable ASCEND\_OPP\_PATH for the default deployment path of the custom operator package exists in the environment.

    ```
    # View the environment variable output.
    echo $ASCEND_OPP_PATH
    
    # If there is no output, set the environment variable. ASCEND_INSTALL_PATH is the installation path of the CANN software package.
    source [ASCEND_INSTALL_PATH]/set_env.bash 
    # For example, source /usr/local/Ascend/cann/set_env.sh
    ```

    Then run the following command to switch to the directory where the compiled custom operator installation package is located, and install the custom operator package.

    ```
    cd CustomOp/build_out
    ./custom_opp_<target os>_<target architecture>.run
    ```

    After the command is executed successfully, the related files in the custom operator package are deployed to the vendors/customize directory pointed to by the ASCEND\_OPP\_PATH environment variable.

3.  **Running**

    Switch to the AclNNInvocation directory and run the run.sh script to run the single operator sample.

    ```
    cd ../../AclNNInvocation
    bash run.sh
    ```

    The AclNNInvocation directory in the sample provides complete sample code for calling the single operator API. After the custom operator is compiled and deployed in the first two steps, the single operator API is automatically generated and can be directly called in the application. The operator API generally takes the form of a "two-stage interface", as shown below:

    ```
    // Obtain the workspace size used by the operator.
    aclnnStatus aclnnAllGatherMatmulCustomGetWorkspaceSize(
         const aclTensor *a,
         const aclTensor *b,
         char *group,
         const aclTensor *cOut,
         const aclTensor *gatherOutOut,
         uint64_t *workspaceSize,
         aclOpExecutor **executor);
    // Execute the operator.
    aclnnStatus aclnnAllGatherMatmulCustom(
         void *workspace,
         uint64_t workspaceSize,
         aclOpExecutor *executor,
         const aclrtStream stream);
    ```

    Here, aclnnAllGatherMatmulCustomGetWorkspaceSize is the first-stage API, which is mainly used to calculate the workspace memory size required during the computation of this API call. Allocate the Device-side memory based on this workspaceSize, and then call the second-stage API aclnnAllGatherMatmulCustom to perform the computation. For details, see [Single Operator API Call](../../../../programming_guide/advanced_programming/aclnn_operator_development/single_operator_api_call.md).

    In the communication-computation fusion scenario, the program that calls the single operator API needs to call the [communication domain creation and management APIs](https://gitcode.com/cann/hcomm/blob/9.1.0/docs/en/api_ref/comm_mgr_c/README.md) to create the communication domain, and execute the AllGatherMatmul operator on multiple threads. The following provides code examples of the key steps in the main function and the thread call function for reference only.

    ```
    int main(int argc, char **argv)
    {
        // 1. Initialize AscendCL
        if (aclInit(NULL) != ACL_SUCCESS) {
            ERROR_LOG("aclInit failed");
            return FAILED;
        }
        // 2. Create the communication domain
        HcclComm comms[RANK_DIM]; // RANK_DIM is the number of cards, which is 8 in this example
        int32_t devices[RANK_DIM];
        for (int32_t i = 0; i < RANK_DIM; i++) {
            devices[i] = i;
        }
        if (HcclCommInitAll(RANK_DIM, devices, comms) != HCCL_SUCCESS) {
            ERROR_LOG("Hccl comm init failed.");
            (void)aclFinalize();
            return FAILED;
        }
        // 3. Create multiple threads to call the AllGatherMatmul operator on all cards in the communication domain
        std::vector<std::unique_ptr<std::thread>> threads(RANK_DIM);
        for (uint32_t rankId = 0; rankId < RANK_DIM; rankId++) {
            threads[rankId].reset(new(std::nothrow) std::thread(&RunOp, rankId, std::ref(comms[rankId])));
        }
        for (uint32_t rankId = 0; rankId < RANK_DIM; rankId++) {
            threads[rankId]->join();
        } 
        // 4. Deinitialize AscendCL
        (void)aclFinalize();
        return SUCCESS;
    }
    ```

    In the main function, the communication domains of RANK\_DIM cards are created uniformly in the current process through the HcclCommInitAll API, where one card corresponds to one thread created later. Each thread calls the RunOp function, which is responsible for applying for runtime resources on the card and calling the two-stage APIs of the single operator API. The code example of the RunOp function is as follows.

    ```
    bool RunOp(uint32_t rankId, HcclComm &comm)
    {
        // 1. Apply for resources such as the context and stream of the current thread
        aclrtContext context;
        aclrtCreateContext(&context, rankId);
        aclrtStream stream;
        aclrtCreateStream(&stream);
        aclrtSetCurrentContext(context);
    
        // 2. Obtain the communication domain name of the card corresponding to the current thread
        char group[128] = {0};
        HcclGetCommName(comm, group);
    
        // 3. Allocate device-side memory to store the operator input and output.
        // ......
    
        // 4. Calculate the workspace size and allocate memory.
        size_t workspaceSize = 0;
        aclOpExecutor *handle = nullptr;
        auto ret = aclnnAllGatherMatmulCustomGetWorkspaceSize(a, b, group, c, gatherOut, &workspaceSize, &handle);
        void *workspace = nullptr;
        if (workspaceSize != 0) {
            aclrtMalloc(&workspace, workspaceSize);
        }
    
        // 5. Execute the operator.
        ret = aclnnAllGatherMatmulCustom(workspace, workspaceSize, handle, stream);
    
        // 6. Synchronize and wait.
        ret = aclrtSynchronizeStreamWithTimeout(stream, 10000);  // 10000 ms stream synchronization timeout.
    
        // 7. Release the device-side memory for the operator input, output, and workspace.
        // ......
    
        // 8. Release resources such as the communication domain, context, and stream.
        (void)HcclCommDestroy(comm);
        (void)aclrtDestroyStream(stream);
        (void)aclrtDestroyContext(context);
        (void)aclrtResetDevice(rankId);
        return true;
    }
    ```
