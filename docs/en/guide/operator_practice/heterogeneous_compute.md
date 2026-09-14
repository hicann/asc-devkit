# Heterogeneous Compute<a name="ZH-CN_TOPIC_0000001846719820"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T12:57:09.849Z -->

When programming with Ascend C, you need to develop code on two different platforms (host and device). This chapter briefly describes the differences between the host and device to help you understand this heterogeneous system at a macro level. It also presents the data flow related to operators. Combined with the characteristics of the heterogeneous architecture, you can further understand how to properly arrange the execution location of operator code to achieve better performance.

## Main Differences Between the Host CPU and the Device NPU<a name="section5161328183719"></a>

-   Different hardware resources

    A CPU is designed for general-purpose computing tasks, but it is inefficient at processing a large amount of parallel computation (such as matrix multiplication and batch data processing). An NPU is designed to accelerate machine learning and deep learning tasks, and it excels at executing a large amount of parallel computation. An NPU contains a large amount of dedicated hardware, for example, the Cube unit that supports matrix computation. A single core in an NPU can complete a multiply-accumulate operation with a data volume of 16 \* 16 \* 16 and the float16 data type within one clock cycle. It also contains the Vector unit that supports vector computation. A single core in an NPU can complete an addition operation with a data volume of 128 + 128 and the float16 data type within one clock cycle.

-   Different physical memory spaces

    The physical memory of the host is separated from that of the device. Data exchange is usually required between the Host-side memory and the Device-side memory.

## How to Properly Arrange Operator Code<a name="section1438410488387"></a>

When developing Ascend C operators, developers can regard the Host and Device as a collaborative heterogeneous system and assign each processing unit the tasks it excels at. On the Host side, it is recommended to execute non-compute-intensive tasks, which are generally scalar computation tasks. On the Device side, it is recommended to execute compute-intensive tasks. By leveraging the SIMD (Single Instruction Multiple Data) instructions of the NPU on the Device side, matrix operations and vector operations on batch data can be implemented efficiently.

The implementation of an Ascend C operator mainly consists of two parts:

-   Tiling implementation on the Host side

    Because the internal storage of the AI Core in the NPU cannot fully accommodate all the input and output data of an operator, a portion of the input data needs to be moved in for computation and then moved out each time, followed by moving in the next portion of input data for computation. This process is called Tiling. The algorithm for splitting data is called the Tiling algorithm or Tiling strategy. The computation program that determines the parameters related to the data splitting algorithm (such as the block size moved each time and the total number of loops) based on information such as the operator shape is called the Tiling implementation, also known as the Tiling Function. Since the Tiling implementation consists entirely of scalar computations, which the AI Core is not good at, it is separated out and executed on the Host-side CPU.

-   Kernel implementation on the Device side

    The Kernel implementation is the implementation of the operator kernel function. Inside the Kernel function, the Tiling information is obtained by parsing the Tiling structure passed in from the Host side, and the process of moving data in and out of Local Memory is controlled based on the Tiling information. The operator logic is implemented by calling APIs for computation, data transfer, memory management, and task synchronization. Its core logic is basically compute-intensive tasks, which are suitable for execution on the Device-side NPU.

## Operator Data Flow<a name="section139451246134413"></a>

Data exchange between the Host and the Device is involved during operator execution. The following describes the specific data flow for Tiling parameter transfer: the Tiling algorithm on the Host side computes the Tiling parameters based on the specific input and output information of the operator and stores them in the Tiling structure; the Tiling structure is then sent from the Host side to the Device side, where the operator obtains and parses the Tiling structure and executes the subsequent operator computation logic based on this information.

**Figure 1** Operator Tiling data transfer flow<a name="fig1623151118289"></a>  
![](../figures/operator_tiling_transfer_data_flow.png "Operator Tiling data transfer flow")

