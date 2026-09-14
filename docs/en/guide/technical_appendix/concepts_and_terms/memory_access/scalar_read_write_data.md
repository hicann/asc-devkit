# Scalar Read/Write Data<a name="ZH-CN_TOPIC_0000002375401226"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:33:18.790Z pushedAt=2026-09-10T11:55:51.500Z -->

In the AI Core, the Scalar compute unit is responsible for scalar data operations of various types and program flow control. According to the [hardware architecture](../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md) design, Scalar supports only read/write operations on Global Memory and Unified Buffer, and does not support access to other types of storage such as L1 Buffer, L0A Buffer, L0B Buffer, and L0C Buffer. The following sections describe how Scalar reads/writes Global Memory and Unified Buffer, and the synchronization mechanism for Scalar read/write data.

## Scalar Read/Write of Global Memory<a name="section7480536235"></a>

![](../../../figures/figure_1_ai_core_internal_parallel_compute_architecture_abstract_diagram_31.png)

As shown in the preceding figure, when the Scalar reads/writes GM data, it passes through the DataCache. The DataCache is mainly used to improve the execution efficiency of scalar memory access instructions. Each AIC/AIV core has an independent DataCache. The following uses a specific example to explain the working mechanism of the DataCache.

globalTensor1 is a Tensor located in GM:

-   After GetValue\(0\) is executed, the first eight elements of globalTensor1 enter the DataCache. Subsequent GetValue\(1\)~GetValue\(7\) calls no longer need to access GM and can directly read data from the Cache Line of the DataCache, improving the efficiency of continuous scalar access.
-   After SetValue\(8, val\) is executed, the elements with indexes 8~15 of globalTensor1 enter the DataCache. SetValue only modifies the Cache Line data in the DataCache and sets the Cache Line status to Dirty, indicating that the data in the Cache Line is inconsistent with the data in GM.

```
AscendC::GlobalTensor<int64_t> globalTensor1;
globalTensor1.SetGlobalBuffer((__gm__ int64_t *)input);
// A total of eight uint64_t values from 0 to 7. The Cache Line length of the DataCache is 64 bytes.
// After GetValue(0) is executed, GetValue(1)~GetValue(7) can be read directly from the Cache Line without accessing GM again.
globalTensor1.GetValue(0);
globalTensor1.GetValue(1);
globalTensor1.GetValue(2);
globalTensor1.GetValue(3);
globalTensor1.GetValue(4);
globalTensor1.GetValue(5);
globalTensor1.GetValue(6);
globalTensor1.GetValue(7);

// After SetValue(8) is executed, the data in GM is not modified. Only the Cache Line data in the DataCache is modified.
// At the same time, the Cache Line status is set to dirty, indicating that the Cache Line data in the DataCache is inconsistent with the data in GM.
int64_t val = 32;
globalTensor1.SetValue(8, val);
globalTensor1.GetValue(8);
```

Based on the working mechanism described above (as shown in the following figure), accessing globalTensor1 across multiple cores may cause data inconsistency. If other cores need to obtain the changes to the GM data, the developer must manually call [DataCacheCleanAndInvalid](../../../../api/SIMD-API/basic_api/cache_control/DataCacheCleanAndInvalid.md) to ensure data consistency.

![](../../../figures/figure_1_ai_core_internal_parallel_compute_architecture_abstract_diagram_32.png)

## Scalar Read/Write of Unified Buffer<a name="section8156161471119"></a>

When Scalar reads/writes the Unified Buffer, you can use the SetValue and GetValue APIs of LocalTensor. The following is an example:

```
for (int32_t i = 0; i < 16; ++i) {
    inputLocal.SetValue(i, i); // Assign the value i to the i-th position in inputLocal.
}

for (int32_t i = 0; i < srcLen; ++i) {
    auto element = inputLocal.GetValue(i); // Obtain the value at the i-th position in inputLocal.
}
```

## Synchronization When a Scalar Reads/Writes Data<a name="section554364118119"></a>

Reading/writing Global Memory and Unified Buffer by Scalar is a PIPE\_S (Scalar pipeline) operation. When you use the SetValue or GetValue API and automatic synchronization is enabled for the operator project, you do not need to manually insert synchronization events.

If automatic synchronization is disabled for the operator project, you need to manually insert synchronization events:

```
// GetValue is a Scalar operation and has a data dependency on the subsequent Duplicate operation.
// Therefore, the Vector pipeline must wait for the Scalar operation to complete.
float inputVal = srcLocal.GetValue(0);
SetFlag<HardEvent::S_V>(eventID1);
WaitFlag<HardEvent::S_V>(eventID1);
AscendC::Duplicate(dstLocal, inputVal, srcDataSize); 

// SetValue is a Scalar operation and has a data dependency on the subsequent data transfer operation.
// Therefore, the MTE3 pipeline must wait for the Scalar operation to complete.
srcLocal.SetValue(0, value);
SetFlag<HardEvent::S_MTE3>(eventID2);
WaitFlag<HardEvent::S_MTE3>(eventID2);
AscendC::DataCopy(dstGlobal, srcLocal, srcDataSize); 
```
