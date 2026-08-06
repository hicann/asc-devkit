# SIMT InsertHashTable Operator Example

## Overview

This example introduces the InsertHashTable operator, demonstrating a large-scale thread concurrent Global Memory access operator implementation based on SIMT.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version
- \>= CANN 9.1.0

## Directory Structure

```text
├── insert_hash_table
│   ├── CMakeLists.txt          // CMake build file
│   ├── insert_hash_table.asc   // Ascend C operator implementation & invocation example
│   └── README.md
```

## Background

A Hash Table is an efficient data structure that maps a "key" to a specific position in a fixed-size array (bucket array) through a hash function, enabling fast lookup, insertion, and deletion operations.

- Hash Function: A function that accepts a key as input, computes an integer (hash value), and maps it to an array index through some operation.
- Bucket Array: An array where each element (called a "bucket") stores key-value pairs.

The basic flow of inserting a key-value pair into a hash table is:

- Use the hash function to compute the hash value of the key
- Map the hash value to a bucket array index (typically by taking the remainder of the hash table capacity)
- Store the key-value pair at the bucket[index] position in the array

Hash Collision: Since the hash table capacity (bucket array size) is much smaller than the hash function output range, different keys may be mapped to the same index.

A common hash collision resolution method is open addressing: when a collision occurs, the algorithm uses linear probing to find the next "empty" bucket to store data.

Since the storage location of each key-value pair is determined by the computed hash value, and these values are typically random and discrete, plus the existence of hash collisions requiring multiple conditional checks before actual data writes, hash tables are not suitable for SIMD programming model implementation. In contrast, in SIMT programming, each thread can independently handle branch judgments and supports discrete memory access, making it more advantageous for implementing hash tables.

## Struct Design

The hash table insertion process involves multiple pieces of information, such as the hash table base address, capacity, bucket size, and input key/value data. Passing these parameters one by one between functions would make interfaces verbose and increase coupling between modules. Therefore, this example uses separate structs to encapsulate the hash table context and input data, improving code readability and maintainability.

`Bucket` is the target memory structure that actually stores key-value pairs in the hash table. Here, `key` stores the key, `value` stores the corresponding value, `state` is used to synchronize key write status, and `flag` is used to control bucket write permission.

```cpp
struct Bucket {
    int64_t key;            // Key
    uint32_t state;         // Key value write status flag
    uint32_t flag;          // Atomic operation flag
    float value[32];        // Value
};
```

`KeyValueData` is used to encapsulate the data of key-value pairs to be inserted, including input keys, input values, value dimension, and the number of key-value pairs.

```cpp
struct KeyValueData {
    int64_t* keys;       // Input keys
    float* values;       // Input values
    uint32_t value_dim;  // Value dimension
    uint32_t key_num;    // Number of key-value pairs

    __aicore__ inline int64_t load_key(uint32_t kv_idx) const; // Load key
    __aicore__ inline float load_value(uint32_t kv_idx, uint32_t j) const; // Load value
};
```

`HashTableContext` stores the parameters of the hash table itself, including the hash table base address, capacity, and bucket size. Its interfaces are used to compute bucket addresses and find and occupy target buckets. When writing values, the `insert` function reads the corresponding input values through the `KeyValueData` parameter.

```cpp
struct HashTableContext {
    uint8_t* table_addr;     // Hash table base address
    uint32_t capacity;       // Hash table capacity
    uint32_t bucket_size;    // Bucket size

    __aicore__ inline Bucket* get_bucket(uint32_t bucket_idx) const; // Get bucket address
    __aicore__ inline Bucket* query(int64_t key, int32_t& success); // Find and occupy target bucket
    __aicore__ inline void insert(Bucket* bucket, const KeyValueData* input,
                                  uint32_t kv_idx, int32_t lane_id); // Write value
};
```

## Algorithm Analysis

SIMT can efficiently process large amounts of data through concurrent execution of many threads, but it also brings two problems: multi-thread write conflicts and inter-core data synchronization.

### Multi-thread Write Conflict Problem

When multiple threads operate on the same memory block, resource conflicts are inevitable. When two threads inserting keys produce a hash collision, multiple threads attempt to write data to the same position in the bucket array, so it is necessary to ensure that only one thread can obtain write permission to the bucket. In the implementation, a flag bit "flag" is added to the Bucket struct to mark the current bucket's write permission. Threads obtain bucket write permission by modifying the flag through the atomic instruction [`asc_atomic_cas()`](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_cas.md).

### Inter-core Data Synchronization Problem

When a hash collision occurs, a thread needs to determine whether the key stored in the current bucket is the same as the key to be inserted. This requires the current thread to read data written by other threads and ensure data integrity. In the implementation, a flag bit "state" is added to the Bucket struct to identify the write status of the key value. After writing the key, the write thread sets the state flag to 1, and calls the [`asc_threadfence()`](../../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md) interface between the two operations to ensure that when state is set to 1, the key write operation has already completed. In the read thread, a while loop polls the state value until state is set to 1, and then reads the key value.

## Operator Description

- Operator Function:  
  The InsertHashTable operator inserts N key-value pairs into a hash table with capacity Z, where key is an int64_t type number and value is a float type Tensor of length M.

- Operator Specifications:  
  <table>
  <tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">insert_hash_table</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">keys</td><td align="center">[1,N]</td><td align="center">int64_t</td><td align="center">ND</td></tr>
  <tr><td align="center">values</td><td align="center">[N,M]</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">table_addr</td><td align="center">[1,Z]</td><td align="center">Bucket</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">insert_hash_table</td></tr>
  </table>

- Operator Implementation:  
  The InsertHashTable operator implementation flow is: each warp processes one key-value pair. Thread 0 in the warp is responsible for finding an available bucket based on the key's hash value, and threads 0-31 are responsible for storing the value into the bucket. When thread 0 searches for an available bucket, it uses open addressing to resolve hash collisions, uses the `asc_atomic_cas()` interface to resolve multi-thread conflicts, and uses the `asc_threadfence()` interface to resolve inter-core data synchronization.

- Invocation Implementation:  
  Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # Build the project
  ./demo                        # Run the example
  ```

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following output indicates that the accuracy verification is successful.
  ```
  [Success] find all key-value in hash table.
  ```
