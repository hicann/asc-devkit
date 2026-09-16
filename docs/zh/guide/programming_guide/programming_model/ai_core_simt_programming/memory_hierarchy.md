# 内存层级<a name="ZH-CN_TOPIC_0000002523359048"></a>

SIMT线程可访问多种内存空间。下表汇总了SIMT编程中常见内存类型的作用域及其生命周期。

| 内存类型 | 线程作用域 | 生命周期 | 物理位置 |
|---------|----------|---------|---------|
| 全局内存 | Grid | 应用程序 | Device |
| 共享内存 | Block | 核函数（Kernel） | Vector Core |
| 栈空间 | Thread | 核函数（Kernel） | Global Memory |
| 寄存器 | Thread | 核函数（Kernel） | Vector Core |

-   全局内存是所有线程均可直接访问的内存资源，即Global Memory；
-   共享内存是线程块内所有线程共享的内存，位于Unified Buffer（UB），生命周期和线程块一致。
-   每个线程拥有独立的栈空间，主要用于存放线程私有临时数据。
-   每个线程拥有独立的寄存器，用于存储局部变量。
-   在访问性能方面，寄存器具有最高的访问速度；共享内存的访问效率优于全局内存。

内存层级如下图所示：  
<img src="../../../figures/memory.png" width="50%">

## 全局内存（Global Memory）<a name="section8946131492119"></a>

Device侧的全局内存是整个Grid中所有线程均可访问的内存空间。全局内存具有持久性：通过全局内存分配的空间及其存储的数据将持续保留，直到该内存空间被释放或应用程序终止。用户需在核函数（Kernel）启动前通过Runtime API完成全局内存的分配与初始化，核函数（Kernel）执行期间SIMT每个线程均可读写全局内存，执行完毕后可将结果拷贝回Host。有关Runtime API的更多信息与细节，可以参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

运行在Device侧的核函数（Kernel）可以通过指针直接访问全局内存。下述代码展示了全局内存的简易示例。数组x、y、z均存储于全局内存中，通过以下核函数（Kernel）实现每个线程对全局内存的访问和存储。

```cpp
__global__ void add_custom(float* x, float* y, float* z, uint64_t total_length)
{
    // Calculate global thread ID
    int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    // Maps to the row index of output tensor
    if (idx >= total_length) {
        return;
    }
    z[idx] = x[idx] + y[idx];
}
```

## 共享内存<a name="section66329146410"></a>

共享内存是同一线程块内所有线程均可访问的内存空间，位于每个Vector Core（AIV）内的UB。与全局内存相比，共享内存的容量较小，但具有更高的带宽和更低的访问延迟，可视为内核执行期间由用户管理的高速缓存资源。

用户可通过动态或者静态方式申请共享内存。

1.  静态申请：分配一段指定大小的内存空间，其大小在编译时确定，不可动态修改，开发者通过数组分配申请使用。

    ```cpp
    __global__ void add_custom(...)
    {
        __ubuf__ half static_buf[1024];
        ...
    }
    ```

    默认情况下，申请到的静态内存首地址按照数据类型对齐，也支持用户使用`__align__(N)`手动指定对齐。

2.  动态申请：用户需要通过<<<\>\>\>中参数[dyn_ubuf_size](kernel_function.md#li12421238101815)指定动态内存的空间大小，其大小在运行期确定，SIMT编程中可通过以下方式申请使用动态内存。

    ```cpp
    // Device侧：声明动态共享内存
    __global__ void add_custom(...)
    {
        extern __ubuf__ char dynamic_buf[];
        ...
    }

    // Host侧：通过<<<>>>中的第三个参数指定动态共享内存大小
    int32_t main(int argc, char const *argv[])
    {
        ...
        uint32_t dyn_ubuf_size = 1024 * sizeof(char);
        add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
        ...
    }
    ```

### UB内存划分排布

如下图所示，UB内存空间总大小为256KB。除了用户申请的动静态内存，UB还需要预留一定的空间供内部使用，因此用户在配置动静态内存大小时，并不能用满全部UB空间：

![](../../../figures/ub_alloc.png)

UB按功能划分为四个主要区域，从低地址到高地址依次为静态内存、动态内存、预留空间和Data Cache，具体结构如下：

1.  静态内存和动态内存对应用户静态、动态申请方式分配的内存。用户在UB上申请的动态内存首地址固定按照32B对齐，静态内存按如下首地址对齐规则排布：
    -   默认情况下，申请到的静态内存首地址按照数据类型大小进行对齐；
    -   用户可以通过`alignas(N)`手动指定对齐方式，该配置的优先级高于默认对齐方式。
2.  预留空间：编译器和Ascend C预留空间，大小为8KB，其中6KB作为SIMT传参寄存器溢出栈空间，2KB为Ascend C接口内部预留空间。
3.  Data Cache：SIMT专有的Data Cache空间，用于SIMT线程访问全局内存时的数据缓存，Data Cache的空间可配置范围为**最小32KB、最大128KB**，实际内存大小受用户配置的静态和动态内存大小影响，具体计算公式为：

    ```
    Data Cache空间大小 = min(UB总大小（256KB） - 静态内存 - 动态内存 - 预留空间（默认8KB）, 128KB)
    ```

    Data Cache空间上限为**128KB**，即使静态与动态内存申请较少，Data Cache实际分配大小也不会超出该上限。

#### Data Cache内存空间说明

Data Cache作为访问GM内存的缓存，其大小会影响算子的访存效率。若Data Cache小于32KB，执行时会出现校验报错并退出，因此申请内存时需确保预留足够的Data Cache空间。

由于Data Cache、预留空间与用户申请的动静态内存共用同一块UB，开发者无法使用全部UB空间。Data Cache的容量由UB总量扣除动静态内存和预留空间计算所得，用户配置的共享内存越小，留给Data Cache的空间就越大，即共享内存大小实际上隐式决定了Data Cache的容量，直至其达到128KB上限。

若用户配置的内存大小不足，甚至未申请内存即直接访问UB地址，访问地址可能越过动、静态内存边界并落入Data Cache区域，属于未定义行为，非常危险。此类越界写入可能破坏SIMT全局内存访问的数据缓存，导致算子功能异常。

#### 禁用预留空间

若用户需要使用更多UB内存，可通过禁用预留空间来扩大可用UB空间。禁用方法为编译时增加如下编译选项：

- [--cce-disable-vf-stack-reserved-ubuf](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461)：关闭6KB的SIMT传参寄存器溢出栈空间。
- [--cce-disable-asc-reserved-ubuf](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461)：关闭2KB的Ascend C接口内部预留空间。

以上编译选项可独立使用，也可同时使用。开启后，编译器不再预留对应部分UB空间，该空间可作为普通UB空间使用。但开发者需要自行确认禁用预留空间的影响：

- 开启`--cce-disable-vf-stack-reserved-ubuf`后，编译器将无法使用预留UB空间作为传参寄存器溢出的栈空间，若用户函数参数过大，将会编译报错。
- 开启`--cce-disable-asc-reserved-ubuf`暂无影响，当前SIMT编程场景内部暂未使用该空间。

## 栈空间

SIMT编程中的栈空间由编译器管理，生命周期与核函数（Kernel）一致，包括以下两部分：

-   位于Global Memory（GM）上的SIMT栈空间。
-   位于UB上用于SIMT函数调用参数传递的中转栈空间（UB上8KB预留空间的一部分）。

下文分别说明这两类栈空间。

### GM上的SIMT栈空间

GM上的SIMT栈空间用于保存线程私有运行时状态，按用途分为SIMT通用栈空间和SIMT Divergence（分支）栈空间。

-   **SIMT通用栈空间**  
    用于存放线程私有临时数据，包括函数调用产生的临时数据、无法完全分配到寄存器的局部对象、[寄存器](#寄存器)溢出（register spill）产生的临时数据等。每个线程的SIMT通用栈空间默认大小为1152Byte，由于位于Global Memory，其访问延迟高于寄存器访问。局部变量较多、函数调用链较深、使用较大的线程私有数组，或最大线程数配置过大导致每个线程可用寄存器不足时，栈空间占用会相应增加，严重时可能引发栈溢出。
-   **SIMT Divergence栈空间**  
    用于存放SIMT分支发散场景下的程序计数器（PC，Program Counter）和活动掩码（Active Mask）。Warp内线程共享一个SIMT Divergence栈空间，每个Warp的栈空间默认大小为1024Byte。分支发散越频繁、控制流越复杂，该栈空间占用越高，严重时可能引发栈溢出。

开发者可通过`--cce-res-usage`编译选项查看核函数（Kernel）的栈空间使用情况。若栈空间占用较高，应优先减少线程私有临时变量、控制调试打印的数据量、简化函数调用链并优化分支发散逻辑，以减少不必要的Global Memory访问。若业务确需调整栈空间大小，可通过`aclInit()`接口加载的配置文件进行设置：配置项`simt_stack_size`用于设置每个线程的SIMT通用栈空间大小，`simt_divergence_stack_size`用于设置SIMT Divergence栈空间大小，单位均为Byte。配置示例如下，更多说明请参考[aclInit()接口配置说明](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/latest/API/runtimeapi/aclcppdevg_03_0022.html)中的“SIMT算子栈空间大小配置示例”。

```json
{
  "StackSize": {
    "simt_stack_size": 1024,
    "simt_divergence_stack_size": 512
  }
}
```

### UB上的SIMT函数调用中转栈空间

如上文[UB内存划分排布](#ub内存划分排布)所述，UB中有8KB大小的预留空间，其中6KB内存是用于SIMT函数调用参数传递的中转栈。如[传参寄存器](#传参寄存器)章节所述，硬件原生支持的函数间传参寄存器容量有限，当参数总大小超出该容量时，编译器会使用该中转栈空间完成参数传递，由此产生额外的访存开销，降低函数调用性能。因此，开发者应关注Device侧函数调用的参数规模，避免传递过大的参数对象。

## 寄存器<a name="section1137715201010"></a>

寄存器是SIMT编程中最靠近计算单元、访问速度最高的存储资源，按功能可分为通用寄存器和传参寄存器，下文分别介绍。

### 通用寄存器

每个SIMT线程拥有独立的通用寄存器，生命周期与核函数（Kernel）一致，其分配由编译器统一管理，用于线程执行期间的局部数据存储。每个Vector Core（AIV）的SIMT通用寄存器文件总容量为128KB，由运行在该AIV上的线程块内所有线程按最大线程数划分使用。每个线程实际可用的寄存器数量取决于核函数（Kernel）定义时配置的最大线程数，最大线程数越大，每个线程可用寄存器越少，对应关系见下表。

**表1**  最大线程数与每个线程可用寄存器数的对应关系

| 最大线程数 | 每个线程可用寄存器数 |
| --- | --- |
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

由上表可知，最大线程数配置越大，每个线程可用的寄存器数量越少。若最大线程数配置过大且单线程计算逻辑复杂，编译器可能因寄存器资源不足，将部分局部变量或中间结果转存至GM上的SIMT通用栈空间，即寄存器溢出（register spill），从而引入额外的Global Memory访问并影响算子性能。开发者可通过`--cce-res-usage`编译选项查看核函数（Kernel）的寄存器资源使用情况，据此调整最大线程数或寄存器配置。

开发者可使用[\_\_launch\_bounds\_\_](../../language_extension/simt_builtin_keywords.md#li23861114618)配置最大线程数，间接控制每个线程的通用寄存器数量；也可使用[\_\_maxnreg\_\_](../../language_extension/simt_builtin_keywords.md#section_maxnreg)直接配置每个线程的通用寄存器数量。

### 传参寄存器

传参寄存器专用于SIMT函数间的参数传递，去除内部占用后，用户可用的传参寄存器容量为`26 * 32bit`。当传递的参数总量超出该容量时，超出部分将溢出至栈空间，产生额外的寄存器到栈空间的数据搬运操作，降低算子性能，严重时可能导致栈溢出。因此，开发者应关注SIMT函数间传递参数的数据量，避免超出该限制。

## 编程建议

针对寄存器个数受限、栈空间有限容易引起性能劣化、甚至爆栈的问题，开发者可参考以下编程建议进行优化：

- 尽量使用位宽较小的数据类型，因为通用寄存器位宽是32bit的，一个`int64_t`数据类型的变量，需要占用2个通用寄存器；
- 减少临时变量，能用地址引用的尽量不用临时变量，可减少寄存器的使用；
- 使用关键字`__inline__`修饰SIMT函数，编译器能够优化函数调用；
- 优化分支处理逻辑，减少Warp内的分支跳转；
- 对于复杂的结构体，可以规划UB或GM内存存放结构体数据，避免传参过大。