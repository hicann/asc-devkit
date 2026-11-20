# Heterogeneous Computing<a name="ZH-CN_TOPIC_0000001846719820"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:44:28.548Z pushedAt=2026-11-19T06:32:19.847Z nl=2 -->

When developing with Ascend C, you need to write code for two different platforms: host and device. This chapter briefly describes the differences between the host and device to help you understand this heterogeneous system at a high level. It also presents the data flow related to operators. Combined with the characteristics of the heterogeneous architecture, this helps you further understand how to properly arrange the execution location of operator code for better performance.

## Key Differences Between the Host (CPU) and the Device (NPU)<a name="section5161328183719"></a>

-   Different hardware resources

    The CPU is designed for general-purpose computing tasks, but it is inefficient at processing large amounts of parallel computation (such as cube multiplication and batch data processing). The NPU is designed to accelerate machine learning and deep learning tasks, and it excels at performing large amounts of parallel computation. The NPU contains a large number of dedicated hardware units, such as the Cube unit that supports cube computation. A single core in the NPU can complete a multiply-accumulate operation on 16 \* 16 \* 16 float16 data within one clock cycle. The NPU also contains the Vector unit that supports vector computation. A single core in the NPU can complete an addition operation on 128 + 128 float16 data within one clock cycle.

-   Different physical memory spaces

    The physical memory of the host and the device is separate, and data exchange between the host memory and the device memory is usually required.

## How to Properly Arrange Operator Code<a name="section1438410488387"></a>

When developing Ascend C operators, you can regard the host and device as a collaborative heterogeneous system and assign each processing unit the work it excels at. On the host side, it is recommended to execute non-computation-intensive tasks, generally scalar computation tasks. On the device side, it is recommended to execute computation-intensive tasks. By using the SIMD \(Single Instruction Multiple Data\) instructions of the NPU on the device side, cube operations and vector operations on batch data can be efficiently implemented.

The operator implementation in Ascend C mainly consists of two parts:

-   Tiling implementation on the host side

    Because the internal storage of the AI Core in the NPU cannot fully accommodate all the input and output data of an operator, it is necessary to move a portion of the input data in for computation and then move it out, and then move the next portion of the input data in for computation. This process is called tiling. The algorithm for splitting data is called the tiling algorithm or tiling strategy. The computation program that determines the parameters related to the data splitting algorithm (such as the block size of each data movement and the total number of loops) based on information such as the operator shape is called the tiling implementation, also known as the tiling function. Since the tiling implementation consists entirely of scalar computation, which the AI Core is not good at, we separate it out and execute it on the host-side CPU.

-   Kernel implementation on the device side

    In the operator kernel function implementation, the tiling information is obtained by parsing the tiling structure passed from the host side, and the process of moving data in and out of Local Memory is controlled based on the tiling information. The operator logic is implemented by calling APIs for computation, data movement, memory management, and task synchronization. Its core logic is essentially computation-intensive tasks, which are suitable for execution on the device-side NPU.

## Operator Data Flow<a name="section139451246134413"></a>

Operator execution involves data exchange between the host and the device. The following describes the specific data flow for tiling parameter transfer: the tiling algorithm on the host side computes the tiling parameters based on the specific input and output information of the operator, and stores them in the tiling structure. The tiling structure is then sent from the host side to the device side, where the operator obtains and parses it, and executes the subsequent operator computation logic based on this information.

**Figure 1**  Data flow of operator tiling parameter passing<a name="fig1623151118289"></a>  
<img src="../figures/tiling_dataflow.png" alt="Data flow of operator tiling parameter passing" width="600px" height="556px">
