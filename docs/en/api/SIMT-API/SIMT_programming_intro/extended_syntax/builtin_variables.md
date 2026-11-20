# Built-in Variable

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-27T03:50:27.991Z pushedAt=2026-10-27T09:21:55.735Z nl=2 -->

The following built-in variables of the dim3 type are currently provided and are available only on the Device:

-   <a id="blockdim"></a>blockDim

    A built-in global variable that can be directly used in kernel functions to obtain the three-dimensional hierarchy of threads configured in a thread block. blockDim.x, blockDim.y, and blockDim.z respectively represent the number of threads in the three dimensions of the thread block.

-   <a id="griddim"></a>gridDim

    A built-in global variable that can only be used in kernel functions. It indicates how many thread blocks constitute the entire computation task in each dimension.

-   blockIdx

    A built-in global variable that can only be used in kernel functions. It is used to obtain the block index, indicating the position coordinates of the thread block where the current thread resides within the entire grid.

-   threadIdx

    A built-in global variable that can be directly used in kernel functions to obtain the index of the current thread within the thread block. threadIdx.x, threadIdx.y, and threadIdx.z respectively represent the index of the current thread in the three dimensions. The range of threadIdx.x is \[0, blockDim.x\), the range of threadIdx.y is \[0, blockDim.y\), and the range of threadIdx.z is \[0, blockDim.z\). The thread index within the thread block is calculated as follows:

    -   For a one-dimensional thread block, the thread index within the thread block is **threadIdx.x**.
    -   For a two-dimensional thread block, the thread index within the thread block is \(threadIdx.x + **threadIdx.y** \* blockDim.x\).
    -   For a three-dimensional thread block, the thread index within the thread block is \(threadIdx.x + **threadIdx.y** \* **blockDim.x** + **threadIdx.z** \* **blockDim.x** \* blockDim.y\).

The following built-in variables of the int type are available only on the Device:

-   warpSize

    A const int32_t variable that indicates the number of threads in a warp, which is currently fixed at 32.
