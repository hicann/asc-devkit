# 内置变量

当前提供了以下仅在Device上可用的dim3类型的内置变量：

-   <a id="blockdim"></a>blockDim

    内置全局变量，在核函数中可以直接使用，用于获取线程块中配置的线程的三维层次结构。blockDim.x，blockDim.y，blockDim.z分别表示线程块中三个维度的线程数。

-   <a id="griddim"></a>gridDim

    内置全局变量，只能在核函数中使用，表示整个计算任务在各个维度上分别由多少个线程块构成。

-   blockIdx

    内置全局变量，只能在核函数中使用，用于获取块索引。表示当前线程所在的线程块在整个网格中的位置坐标。blockIdx.x的范围是0到gridDim.x - 1，blockIdx.y和blockIdx.z目前只能返回0。

-   threadIdx

    内置全局变量，在核函数中可以直接使用，用于获取当前线程在线程块内部的索引。threadIdx.x，threadIdx.y，threadIdx.z分别表示当前线程在3个维度的索引，threadIdx.x的范围为\[0, blockDim.x\)，threadIdx.y的范围为\[0, blockDim.y\)，threadIdx.z的范围为\[0, blockDim.z\)。线程块内线程的索引计算方式如下：

    -   对于一维线程块，线程块内线程的索引为threadIdx.x。
    -   对于二维线程块，线程块内线程的索引为\(threadIdx.x + threadIdx.y \* blockDim.x\)。
    -   对于三维线程块，线程块内线程的索引为\(threadIdx.x + threadIdx.y \* blockDim.x + threadIdx.z \* blockDim.x \* blockDim.y\)。

当前提供了以下仅在Device上可用的int类型的内置变量：

-   warpSize

    const int32_t类型变量，表示一个线程束（Warp）中的线程数量，当前为固定值32。
