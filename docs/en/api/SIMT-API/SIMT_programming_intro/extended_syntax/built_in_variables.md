# Built-in Variable

<!-- md-trans-meta sourceCommit=2c97acf47d28b2c9fa27e8b745084f6e4f3eedea translatedAt=2026-09-09T02:27:55.042Z pushedAt=2026-09-09T10:00:56.673Z -->

The following built-in variables of the dim3 structure are currently provided and are available only on the Device:

-   <a id="blockdim"></a>**blockDim**

    A built-in global variable that can be directly used in kernel functions to obtain the three-dimensional hierarchy of threads configured in the thread block. blockDim.x, blockDim.y, and blockDim.z respectively represent the number of threads in the three dimensions of the thread block. The number of threads in each dimension must satisfy blockDim.x * blockDim.y * blockDim.z <= 2048.

-   <a id="griddim"></a>**gridDim**

    A built-in global variable that can only be used in kernel functions. It indicates how many thread blocks constitute the entire computation task in each dimension. The number of thread blocks in each dimension must satisfy gridDim.x * gridDim.y * gridDim.z <= 65535.

-   **blockIdx**

    A built-in global variable that can only be used in kernel functions. It is used to obtain the block index, indicating the position coordinates of the thread block where the current thread resides within the entire grid.

-   **threadIdx**

    A built-in global variable that can be directly used in kernel functions to obtain the index of the current thread within the thread block. threadIdx.x, threadIdx.y, and threadIdx.z respectively represent the index of the current thread in the three dimensions. The range of threadIdx.x is \[0, blockDim.x\), the range of threadIdx.y is \[0, blockDim.y\), and the range of threadIdx.z is \[0, blockDim.z\). The thread index within the thread block is calculated as follows:

    -   For a one-dimensional thread block, the thread index within the thread block is **threadIdx.x**.
    -   For a two-dimensional thread block, the thread index within the thread block is \(threadIdx.x + **threadIdx.y** \* blockDim.x\).
    -   For a three-dimensional thread block, the thread index within the thread block is \(threadIdx.x + **threadIdx.y** \* **blockDim.x** + **threadIdx.z** \* **blockDim.x** \* blockDim.y\).

The following built-in variables of the int type are available only on the Device:

-   **warpSize**

    A runtime variable that indicates the number of threads in a warp. It is currently a fixed value of 32.

