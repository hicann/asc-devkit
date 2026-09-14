# AllocTensor/FreeTensor Failure During Running Verification<a name="ZH-CN_TOPIC_0000001737495336"></a>

<!-- md-trans-meta sourceCommit=51eb188d02e4be270d9dc56141153e3f75345dc0 translatedAt=2026-08-26T11:23:18.357Z pushedAt=2026-08-31T10:51:30.568Z -->

## Symptoms<a name="section151611254194612"></a>

When the kernel function is verified on an NPU, the system hangs. When it is verified on a CPU, an AllocTensor/FreeTensor failure is reported. The error log and call stack are as follows:

```
[ERROR][Core_0][/usr/local/Ascend/cann/x86_64-linux/tikcpp/tikcfw/interface/kernel_tpipe.h:730][AllocEventID][321678] current size is 4, max buffer number in same queue position is 4
[ERROR][CORE_0][pid 321674] error happened! =========
SIGABRT Signal (Abort Signal from abort) catched, backtrace info:
[#0] 0x000000000001e7c0: handler(int) at /usr/local/Ascend/cann/tools/tikicpulib/lib/include/kern_fwk.h:105
[#1] 0x0000000000017c4f: signed char AscendC::TPipe::AllocEventID<(AscendC::HardEvent)5>() at /usr/local/Ascend/cann/x86_64-linux/tikcpp/tikcfw/interface/kernel_tpipe.h:733
[#2] 0x000000000001426d: AscendC::TQueBind<(AscendC::TPosition)0, (AscendC::TPosition)9, 4, 0>::FreeBuffer(unsigned char*) at /usr/local/Ascend/cann/x86_64-linux/tikcpp/tikcfw/interface/kernel_tpipe.h:1217
[#3] 0x0000000000011058: void AscendC::TQueBind<(AscendC::TPosition)0, (AscendC::TPosition)9, 4, 0>::FreeTensor<float16::Fp16T>(AscendC::LocalTensor<float16::Fp16T>&) at /usr/local/Ascend/cann/x86_64-linux/tikcpp/tikcfw/interface/kernel_tpipe.h:1237
[#4] 0x000000000000dfde: KernelAdd::Compute(int) at /home/xxxx/xxxx.cpp:59
[#5] 0x000000000000dd1c: KernelAdd::Process() at /home/xxxx/xxxx.cpp:37 (discriminator 2)
...
```

## Root Cause<a name="section417961104715"></a>

According to the log information "current size is 4, **max buffer number in same queue position** is 4", it can be determined that the issue occurs because the number of QUE Buffers on the same TPosition exceeds the limit.

For all queues on the same TPosition, the number of tensors allocated by consecutively calling the AllocTensor API is subject to a quantity constraint that varies with the AI processor model. This constraint must be satisfied when allocating buffers.

For Atlas training products, the number does not exceed 4.

For the AI Core of Atlas inference products, the number does not exceed 8.

For the Vector Core of Atlas inference products, the number does not exceed 8.

For Atlas A2 training products/Atlas A2 inference products, the number does not exceed 8.

For Atlas A3 training products/Atlas A3 inference products, the number does not exceed 8.

For Atlas 200I/500 A2 inference products, the number does not exceed 8.

If this constraint is not met, resource allocation may fail when AllocTensor/FreeTensor is used later. For example:

```
AscendC::TQue<AscendC::TPosition::VECIN, 1> que0;
AscendC::TQue<AscendC::TPosition::VECIN, 1> que1;
AscendC::TQue<AscendC::TPosition::VECIN, 1> que2;
AscendC::TQue<AscendC::TPosition::VECIN, 1> que3;
AscendC::TQue<AscendC::TPosition::VECIN, 1> que4;
AscendC::TQue<AscendC::TPosition::VECIN, 1> que5;
// For example, an operator has six inputs and needs to allocate six buffers.
// Allocate memory for them through six queues, namely que0 to que5, with each queue allocating one buffer. The total number of buffers allocated on the VECIN TPosition is six.
// Assume that the limit on the number of buffers consecutively allocated on the same position is four. If this limit is exceeded, resource allocation fails when AllocTensor/FreeTensor is used.
// On the NPU, this may manifest as abnormal behavior such as a hang. In the CPU Debug scenario, an error message is reported.
pipe.InitBuffer(que0, 1, len);
pipe.InitBuffer(que1, 1, len);
pipe.InitBuffer(que2, 1, len);
pipe.InitBuffer(que3, 1, len);
pipe.InitBuffer(que4, 1, len);
pipe.InitBuffer(que5, 1, len);

AscendC::LocalTensor<T> local1 = que0.AllocTensor<T>();
AscendC::LocalTensor<T> local2 = que1.AllocTensor<T>();
AscendC::LocalTensor<T> local3 = que2.AllocTensor<T>();
AscendC::LocalTensor<T> local4 = que3.AllocTensor<T>();
// The fifth AllocTensor fails to allocate resources because the number of tensors simultaneously allocated on the same TPosition exceeds the limit of four.
AscendC::LocalTensor<T> local5 = que4.AllocTensor<T>();
```

## Procedure<a name="section166318242419"></a>

If multiple buffers are actually used, you can combine them into a single buffer and use them through offsets. The following is an example:

```
// In this case, you are advised to use the following method:
// If multiple buffers are actually used, you can combine them into a single buffer and use them through offsets.
pipe.InitBuffer(que0, 1, len * 3);
pipe.InitBuffer(que1, 1, len * 3);
/*
 * Allocate a LocalTensor of three memory blocks. The address of local1 is the start address of the buffer in que0, 
 * the address of local2 is the address of local1 offset by len, and the address of local3 is the address of local1 offset by 
 * len * 2
 */
int32_t offset1 = len;
int32_t offset2 = len * 2;
AscendC::LocalTensor<T> local1 = que0.AllocTensor<T>();
AscendC::LocalTensor<T> local2 = local1[offset1];
AscendC::LocalTensor<T> local3 = local1[offset2];
```

