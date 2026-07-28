# SIMT实现InsertHashTable算子案例<a name="ZH-CN_TOPIC_0000002530604958"></a>

## 案例介绍<a name="section1618932742516"></a>

本案例介绍[InsertHashTable算子](../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table)，展示基于SIMT实现的大规模线程并发访问Global Memory内存的算子实践。

## 背景知识<a name="section111061628366"></a>

Hash Table（哈希表，也称散列表）是一种高效的数据结构，它通过一个哈希函数将“键”（Key）映射到一个固定大小的数组（桶数组）的特定位置，从而实现快速的查找、插入和删除操作。

哈希函数：一个函数，它接收一个键作为输入，计算出一个整数（哈希值），该整数通过某种运算（通常是取模）被映射为数组的一个索引。

桶数组：一个数组，每个元素（称为“桶，Bucket”）用于存储键值对 \(Key-Value Pair\)。

向哈希表中插入一个键值对的基本流程是：

-   使用哈希函数计算出key的哈希值
-   将哈希值映射为桶数组的索引index（通常是对哈希表容量取余）
-   将键值对存储到数组的bucket\[index\]位置

**图1**  向哈希表中插入元素<a name="fig7419123381911"></a>  
![](../../figures/向哈希表中插入元素.png "向哈希表中插入元素")

哈希冲突：由于哈希表容量（桶数组大小）远小于哈希函数的输出范围，不同的键可能会被映射到同一个索引上。

常见的哈希冲突解决方法——开放寻址法：当发生冲突时，算法会按照线性探查寻找下一个“空”的桶来存储数据。

以下图为例：根据key计算出的哈希值，得到位置索引为3，当与桶bucket\[3\]中的已有数据发生哈希冲突时，会向后逐个探测，直到找到可用的桶。如果遍历完整个桶数组仍未找到可用的桶，则插入失败。

**图2**  开放寻址法<a name="fig8305155715170"></a>  
![](../../figures/开放寻址法.png "开放寻址法")

由于每个键值对的存储位置由计算出的哈希值决定，这些值通常是随机且离散的，加上哈希冲突的存在，导致在实际写入数据前需要进行多个条件判断，因此，哈希表不适合基于SIMD编程模型来实现。相比之下，SIMT编程中每个线程都能独立处理分支判断，并支持离散访存，因此在实现哈希表方面更具优势。

## 算法分析<a name="section163451294312"></a>

SIMT能够通过大量线程的并发执行实现高效处理大批数据，但同时也带来了两个问题：多线程写冲突和核间数据同步问题。

-   多线程写冲突问题

    在多线程操作同一块内存时，资源冲突问题在所难免。当两个线程插入的key产生哈希冲突时，多个线程会尝试向桶数组的同一位置写入数据，因此需要一种机制确保仅有一个线程能够获得桶的写入权限。在CPU上，通常使用互斥锁来解决这一问题，但在SIMT架构中无法使用锁，故需通过原子操作实现无锁编程。

    例如，图中线程0和线程256处理的key发生了哈希冲突，计算出的位置索引均为3。此时，应确保仅有一个线程能获得桶bucket\[3\]的写入权限，而另一个线程则通过开放寻址法寻找下一个可用的桶。

    **图3**  多线程哈希冲突<a name="fig162521813558"></a>  
    ![](../../figures/多线程哈希冲突.png "多线程哈希冲突")

    程序实现时，在Bucket结构体中添加一个标记位”flag”，用于标记当前桶的写入权限，线程通过原子指令[asc\_atomic\_cas](../../../api/SIMT-API/原子操作/asc_atomic_cas.md)修改flag来获取桶的写权限。

    ```cpp
    inline int32_t asc_atomic_cas(int32_t *address, int32_t compare, int32_t val)
    ```

    asc\_atomic\_cas接口用于对指定地址上的数据进行原子比较赋值操作，如果address上的数值等于指定数值compare，则将address赋值为指定的val值，否则address的数值不变。该接口确保在多个线程并发修改同一地址的值时，只有1个线程能修改成功。通过该接口修改桶的flag值，修改成功的线程获得桶的写权限，可直接写入数据；修改失败的线程则按照开放寻址法寻找下一个可用的桶。

    在实现上，Bucket结构体中添加了一个标记位“flag”，用于标识桶的写权限。flag的取值包括DEFAULT\_FLAG、HOLD\_FLAG和WRITE\_FLAG，分别表示桶为空、桶中有数据和桶正在被写入。使用asc\_atomic\_cas接口修改flag，当compare参数为DEFAULT\_FLAG且val参数为WRITE\_FLAG时，表示尝试获取空桶的写权限；当compare参数为HOLD\_FLAG且val参数为WRITE\_FLAG时，表示尝试获取非空桶的写权限。根据接口的返回值来判断是否成功获取了写权限。

-   核间数据同步问题

    当发生哈希冲突时，线程需要判断当前桶中存储的key是否与待插入的key相同，这要求当前线程能够读取其他线程写入的数据，并确保数据的完整性。SIMT基于弱内存模型（Weak Memory Model）实现，在这种模型中，线程的内存操作顺序对其他线程而言是不确定的，且Data Cache与Global Memory之间不保证一致性。这意味着一个核中的线程向Global Memory写入的数据，另一个核可能无法立即读取到，因此，当多个核操作同一块Global Memory时，需要一种机制来确保核间数据同步。

    弱内存模型允许硬件对内存访问指令进行重排序，以追求更高的性能。考虑以下代码片段，假设A和flag是共享的全局变量，初始值都为0。

    ```cpp
    // core 0
    A = 1;    // store A
    flag = 1; // store flag
    
    // core 1
    while (flag != 1); // load flag (spin until flag=1)
    int r1 = A;        // load A
    ```

    在弱内存模型下，core 0的store A和store flag可能被重排序，导致其他核先看到flag=1，而A=1的更新尚未到达共享缓存。这时core 1可能会读到r1=0，这与程序的顺序执行逻辑不符。常见的解决方法是使用内存屏障来强制保证屏障两侧的内存操作不被重排序，[asc\_threadfence\(\)](../../../api/SIMT-API/同步与内存栅栏/内存栅栏接口/asc_threadfence.md)接口用于实现此功能。

    程序实现时，在Bucket结构体中添加一个标记位“state”，用于标识key值的写入状态。写线程中，在写入key后将state标记位置为1，并在二者之间调用asc\_threadfence\(\)接口，确保当state被设置为1时，key的写入操作已经完成。读线程中，通过while循环轮询state值，直到state被设置为1，然后再读取key值，判断当前的key与桶中的key是否一致。

    ```cpp
    // 写线程
    bucket->key = key;
    asc_threadfence();
    bucket->state = 1;
    
    // 读线程
    while (*reinterpret_cast<volatile uint32_t*>(&bucket->state) != 1) {}
    if (key == *reinterpret_cast<volatile int64_t*>(&bucket->key))
    ...
    ```

    由于Data Cache与Global Memory之间不保证一致性，核0向Global Memory中写入的数据会更新到L2 Cache，但核1读取数据时，如果Data Cache命中，会直接使用Data Cache中的值，而不是从L2 Cache中获取值。因此，当需要读取其他核写入Global Memory中的数据时，应使用volatile关键字，以确保每次读取的值都是从Global Memory中获取，而不是Data Cache。

    **图4**  内存模型示意图<a name="fig1946111125181"></a>  
    ![](../../figures/内存模型示意图.png "内存模型示意图")

## 算子实现<a name="section665410415400"></a>

本例实现将8\*1024个键值对插入容量为16\*1024的哈希表中，其中key为int64\_t类型数，value为长度32的float类型的Tensor。

根据上述分析，桶结构体设计如下：

```cpp
struct Bucket {
    int64_t key;           // 键
    uint32_t state;        // key值写入状态标记
    uint32_t flag;         // 原子操作标志位
    float value[32];       // 值
};
```

算子核心逻辑的伪代码如下所示。

```cpp
uint32_t hash = murmur_hash_3(key);                                             // 采用MurmurHash3算法计算哈希值
uint32_t index = hash & (capacity - 1);                                         // 通过hash得到初始位置索引
Bucket* bucket = nullptr;
for (uint32_t i = 0; i < capacity; i++) {
    bucket = &buckets[index];
    int32_t old_flag = asc_atomic_cas(&bucket->flag, DEFAULT_FLAG, WRITE_FLAG); // 获取空桶的写权限
    if (old_flag == DEFAULT_FLAG) {                                             // 成功获取空桶写权限
        bucket->key = key;
        asc_threadfence();                                                      // 在写入key和state置1中间插入内存栅栏，保证执行顺序
        bucket->state = 1;
        bucket->value = value;
        bucket->flag = HOLD_FLAG;
    } else {
        while (*reinterpret_cast<volatile uint32_t*>(&bucket->state) != 1) {}   // 通过state标记位保证key值的有效性
        if (key == *reinterpret_cast<volatile int64_t*>(&bucket->key)) {        // 桶中存储的key与要插入的key相同
            old_flag = asc_atomic_cas(&bucket->flag, HOLD_FLAG, WRITE_FLAG);    // 获取已有数据的bucket的写权限
            if (old_flag == HOLD_FLAG) {                                        // 成功获取写权限
                bucket->value = value;                                          // 更新value
                bucket->flag = HOLD_FLAG;
            }
        } else {
            index = (index + 1) % capacity;                                     // 线性探测下一个可用桶
        }
    }
}
```

特别说明多线程中出现哈希冲突的处理流程，具体如下：

线程0和线程256分别计算key\[0\]和key\[8\]的初始位置时出现哈希冲突，初始位置索引都是3。

**图5**  多线程哈希冲突阶段1<a name="fig13897171319340"></a>  
![](../../figures/多线程哈希冲突阶段1.png "多线程哈希冲突阶段1")

线程0和线程256通过asc\_atomic\_cas接口抢占bucket\[3\]的写权限，其中线程0成功获取写权限，将数据写入bucket\[3\]位置，线程256向后线性探测可用的bucket。

**图6**  多线程哈希冲突阶段2<a name="fig850521281820"></a>  
![](../../figures/多线程哈希冲突阶段2.png "多线程哈希冲突阶段2")

线程256查询bucket\[4\]中已有数据，继续向后寻找，最终发现bucket\[5\]可用，获取写入权限后将数据写入。

**图7**  多线程哈希冲突阶段3<a name="fig36541916161910"></a>  
![](../../figures/多线程哈希冲突阶段3.png "多线程哈希冲突阶段3")

## 总结<a name="section8281219125011"></a>

本案例展示了基于SIMT编程实现向hash table中批量插入元素，重点介绍了如何通过原子指令解决多线程写冲突，以及如何使用asc\_threadfence\(\)接口实现核间数据同步。
