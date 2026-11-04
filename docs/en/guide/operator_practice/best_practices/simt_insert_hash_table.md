# Case of Implementing the InsertHashTable Operator Using SIMT<a name="ZH-CN_TOPIC_0000002530604958"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T11:22:22.843Z pushedAt=2026-11-03T07:05:13.269Z nl=2 -->

## Case Introduction<a name="section1618932742516"></a>

This case introduces the [InsertHashTable operator](../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table), demonstrating an operator practice based on SIMT for large-scale multithreading concurrent access to Global Memory.

## Background<a name="section111061628366"></a>

A hash table (also called a scatter table) is an efficient data structure that maps a key to a specific position in a fixed-size array (bucket array) through a hash function, thereby enabling fast lookup, insertion, and deletion operations.

Hash function: a function that takes a key as input and computes an integer (hash value), which is then mapped to an index of the array through a certain operation (usually modulo).

Bucket array: an array in which each element (called a "bucket") is used to store key-value pairs \(Key-Value Pair\).

The basic process of inserting a key-value pair into a hash table is as follows:

-   Compute the hash value of the key using the hash function.
-   Map the hash value to the index of the bucket array (usually by taking the remainder of the hash table capacity).
-   Store the key-value pair at the bucket\[index\] position of the array.

**Figure 1** Inserting an element into a hash table<a name="fig7419123381911"></a>  
![](../../figures/hash_insert.png "Inserting an element into a hash table")

Hash collision: because the hash table capacity (bucket array size) is much smaller than the output range of the hash function, different keys may be mapped to the same index.

A common method for resolving hash collisions, open addressing: when a collision occurs, the algorithm probes linearly for the next "empty" bucket to store the data.

Take the following figure as an example: the hash value calculated from the key yields a position index of 3. When a hash collision occurs with the existing data in the bucket bucket\[3\], the algorithm probes backward one by one until it finds an available bucket. If no available bucket is found after traversing the entire bucket array, the insertion fails.

**Figure 2** Open addressing<a name="fig8305155715170"></a>  
![](../../figures/open_addr.png "Open addressing")

Because the storage location of each key-value pair is determined by the calculated hash value, which is usually random and discrete, and because hash collisions exist, multiple conditional checks are required before data is actually written. Therefore, a hash table is not suitable for implementation based on the SIMD programming model. In contrast, in SIMT programming, each thread can independently handle branch decisions and supports discrete memory access, making it more advantageous for implementing a hash table.

## Algorithm Analysis<a name="section163451294312"></a>

SIMT can efficiently process large volumes of data through the concurrent execution of a large number of threads, but it also introduces two problems: multithreading write conflicts and inter-core data synchronization.

-   Multithreading write conflict

    When multiple threads operate on the same memory block, resource conflicts are inevitable. When the keys inserted by two threads cause a hash collision, multiple threads attempt to write data to the same position in the bucket array. Therefore, a mechanism is required to ensure that only one thread can obtain the write permission of the bucket. On a CPU, a mutex lock is usually used to solve this problem. However, locks cannot be used in the SIMT architecture, so lock-free programming must be implemented through atomic operations.

    For example, in the figure, the keys processed by thread 0 and thread 256 cause a hash collision, and both compute a position index of 3. In this case, only one thread should be allowed to obtain the write permission of the bucket bucket\[3\], while the other thread finds the next available bucket through open addressing.

    **Figure 3**  Multithreading hash collision<a name="fig162521813558"></a>  
    ![](../../figures/mt_hash.png "Multithreading hash collision")

    During program implementation, a flag bit "flag" is added to the Bucket structure to mark the write permission of the current bucket. A thread modifies the flag through an atomic instruction [asc\_atomic\_cas](../../../api/SIMT-API/atomic_operations/asc_atomic_cas.md) to obtain the write permission of the bucket.

    ```cpp
    inline int32_t asc_atomic_cas(int32_t *address, int32_t compare, int32_t val)
    ```

    The **asc\_atomic\_cas** API is used to perform an atomic compare-and-swap operation on the data at a specified address. If the value at address equals the specified value compare, address is assigned the specified value val; otherwise, the value at address remains unchanged. This API ensures that when multiple threads concurrently modify the value at the same address, only one thread can modify it successfully. By modifying the flag value of the bucket through this API, the thread that succeeds in the modification obtains the write permission of the bucket and can directly write data; the thread that fails finds the next available bucket through open addressing.

    In the implementation, a flag bit "flag" is added to the Bucket structure to identify the write permission of the bucket. The values of flag include **DEFAULT\_FLAG**, **HOLD\_FLAG**, and **WRITE\_FLAG**, which indicate that the bucket is empty, the bucket contains data, and the bucket is being written, respectively. The **asc\_atomic\_cas** API is used to modify flag. When the compare parameter is **DEFAULT\_FLAG** and the **val** parameter is **WRITE\_FLAG**, it indicates an attempt to obtain the write permission of an empty bucket; when the compare parameter is **HOLD\_FLAG** and the **val** parameter is **WRITE\_FLAG**, it indicates an attempt to obtain the write permission of a non-empty bucket. Whether the write permission is successfully obtained is determined based on the return value of the API.

-   Inter-core data synchronization

    When a hash collision occurs, a thread needs to determine whether the key stored in the current bucket is the same as the key to be inserted. This requires the current thread to be able to read data written by other threads and to ensure data integrity. SIMT is implemented based on the weak memory model, in which the order of a thread's memory operations is not deterministic to other threads, and consistency between the Data Cache and Global Memory is not guaranteed. This means that data written by a thread in one core to Global Memory may not be immediately readable by another core. Therefore, when multiple cores operate on the same Global Memory, a mechanism is required to ensure data synchronization between cores.

    The weak memory model allows the hardware to reorder memory access instructions to pursue higher performance. Consider the following code snippet, assuming that A and flag are shared global variables with initial values of 0.

    ```cpp
    // core 0
    A = 1;    // store A
    flag = 1; // store flag
    
    // core 1
    while (flag != 1); // load flag (spin until flag=1)
    int r1 = A;        // load A
    ```

    Under the weak memory model, the store A and store flag operations of core 0 may be reordered, causing other cores to see flag=1 first while the update of A=1 has not yet reached the shared cache. In this case, core 1 may read r1=0, which is inconsistent with the sequential execution logic of the program. A common solution is to use a memory barrier to enforce that memory operations on both sides of the barrier are not reordered. The [asc\_threadfence\(\)](../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md) API is used to implement this function.

    During program implementation, a flag bit "state" is added to the Bucket structure to indicate the write status of the key value. In the write thread, the state flag is set to 1 after the key is written, and the asc\_threadfence\(\) API is called between the two operations to ensure that the key write operation is complete when state is set to 1. In the read thread, a while loop polls the state value until state is set to 1, and then reads the key value to determine whether the current key is consistent with the key in the bucket.

    ```cpp
    // Write thread
    bucket->key = key;
    asc_threadfence();
    bucket->state = 1;
    
    // Read thread
    while (*reinterpret_cast<volatile uint32_t*>(&bucket->state) != 1) {}
    if (key == *reinterpret_cast<volatile int64_t*>(&bucket->key))
    ...
    ```

    Because consistency between the Data Cache and Global Memory is not guaranteed, data written by core 0 to Global Memory is updated to the L2 Cache. However, when core 1 reads data, if the Data Cache hits, it directly uses the value in the Data Cache instead of obtaining the value from the L2 Cache. Therefore, when data written by other cores to Global Memory needs to be read, the volatile keyword should be used to ensure that each read obtains the value from Global Memory rather than from the Data Cache.

    **Figure 4**  Memory model diagram<a name="fig1946111125181"></a>  
    ![](../../figures/mem_model.png "Memory model diagram")

## Operator Implementation<a name="section665410415400"></a>

This example inserts 8\*1024 key-value pairs into a hash table with a capacity of 16\*1024, where the key is an int64\_t number and the value is a float tensor of length 32.

Based on the preceding analysis, the bucket structure is designed as follows:

```cpp
struct Bucket {
    int64_t key;           // Key
    uint32_t state;        // Write status flag of the key value
    uint32_t flag;         // Atomic operation flag
    float value[32];       // Value
};
```

The pseudocode of the core operator logic is as follows.

```cpp
uint32_t hash = murmur_hash_3(key);                                             // Compute the hash value using the MurmurHash3 algorithm
uint32_t index = hash & (capacity - 1);                                         // Obtain the initial position index from the hash
Bucket* bucket = nullptr;
for (uint32_t i = 0; i < capacity; i++) {
    bucket = &buckets[index];
    int32_t old_flag = asc_atomic_cas(&bucket->flag, DEFAULT_FLAG, WRITE_FLAG); // Obtain the write permission of an empty bucket.
    if (old_flag == DEFAULT_FLAG) {                                             // Successfully obtain the write permission of an empty bucket.
        bucket->key = key;
        asc_threadfence();                                                      // Insert a memory fence between writing key and setting state to 1 to ensure the execution order.
        bucket->state = 1;
        bucket->value = value;
        bucket->flag = HOLD_FLAG;
    } else {
        while (*reinterpret_cast<volatile uint32_t*>(&bucket->state) != 1) {}   // Ensure the validity of the key value through the state flag.
        if (key == *reinterpret_cast<volatile int64_t*>(&bucket->key)) {        // The key stored in the bucket is the same as the key to be inserted.
            old_flag = asc_atomic_cas(&bucket->flag, HOLD_FLAG, WRITE_FLAG);    // Obtain the write permission of the bucket that already contains data.
            if (old_flag == HOLD_FLAG) {                                        // Successfully obtain the write permission.
                bucket->value = value;                                          // Update the value.
                bucket->flag = HOLD_FLAG;
            }
        } else {
            index = (index + 1) % capacity;                                     // Linearly probe the next available bucket.
        }
    }
}
```

The following describes the handling process when a hash collision occurs in multithreading:

When thread 0 and thread 256 calculate the initial positions of key\[0\] and key\[8\] respectively, a hash collision occurs, and the initial position index is 3 for both.

**Figure 5**  Multithreading hash collision stage 1<a name="fig13897171319340"></a>  
![](../../figures/mt_hash_1.png "Multithreading hash collision stage 1")

Thread 0 and thread 256 compete for the write permission of bucket\[3\] through the **asc\_atomic\_cas** API. Thread 0 successfully obtains the write permission and writes data to the bucket\[3\] position, while thread 256 linearly probes backward for an available bucket.

**Figure 6**  Multithreading hash collision stage 2<a name="fig850521281820"></a>  
![](../../figures/mt_hash_2.png "Multithreading hash collision stage 2")

Thread 256 queries the existing data in bucket\[4\], continues to search backward, and finally finds that bucket\[5\] is available. It then obtains the write permission and writes the data.

**Figure 7**  Multithreading hash collision stage 3<a name="fig36541916161910"></a>  
![](../../figures/mt_hash_3.png "Multithreading hash collision stage 3")

## Summary<a name="section8281219125011"></a>

This case demonstrates batch insertion of elements into a hash table based on the SIMT programming model, focusing on how to resolve multithreading write conflicts through atomic instructions and how to use the **asc\_threadfence\(\)** API to implement inter-core data synchronization.
