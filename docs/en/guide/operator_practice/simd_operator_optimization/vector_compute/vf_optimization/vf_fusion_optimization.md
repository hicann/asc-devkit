# VF Fusion Optimization<a name="ZH-CN_TOPIC_0000002530248741"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:56:40.630Z pushedAt=2026-11-03T07:05:13.357Z nl=2 -->

[Priority] High

[Description] VF fusion combines multiple VF functions in the code into a single VF function, effectively improving performance. VF fusion is an optimization feature. VF automatic fusion uses the Loop Fuse algorithm to convert VFs into loop form, fuses control-flow-equivalent VFs, and then restores the VFs. The compiler first performs a legality check before fusion to determine whether the two VFs are equivalent, whether the intermediate code on the Main side can be executed within the VF, and whether fusion can produce positive benefits (without causing parameter register overflow or excessively large VF code). If the VF fusion conditions are met, the compiler automatically performs VF fusion optimization. To ensure that the execution logic and semantics of the fused VF are consistent with those before fusion, synchronization instructions are conservatively inserted between the original two VFs. The compiler also attempts to hoist and merge instructions in the fused VF to optimize the VF code. The fusion strategy is to fuse as much as possible. Users who write code following the patterns that satisfy the fusion legality check can increase the opportunities for VF fusion.

> [!NOTE] Note
>
> A longer VF does not necessarily mean more fusion or better results. The positive benefits of VF fusion include:
> - Eliminating redundant VF startup overhead;
> - Canceling redundant Load/Store instruction pairs;
> - Hardware loop fusion;
>
> When negative benefits exist, the fusion conditions are not met:
> - Registers exceed the limit, and writing data back to the Unified Buffer (UB) causes performance degradation;
> - Too many instructions with dependencies exist in the loop, reducing the probability of dual-issue, causing instructions in the queue to be swapped out, and lowering the Icache hit rate.

## VF Fusion Principles<a name="section46641606486"></a>

VF fusion optimization can be divided into three stages: VF shallow fusion, VF deep fusion, and automatic synchronization within VF:

**VF shallow fusion**: The compiler first analyzes whether the control flows of two VFs are equivalent, and builds a Cost Model to analyze whether there is a positive benefit. If the VF fusion condition is met, the control flow outside the VF is merged into the VF, the Software Loop outside the VF is hardened into a Hardware Loop inside the VF, and then the basic capability of VF automatic fusion is enabled to fuse two VFs into one VF, providing a basis for subsequent VF deep fusion.

![](../../../../figures/vf_fusion.png)

**VF deep fusion**: VF deep fusion continues to fuse the Hardware Loops inside the VF, thereby reducing the startup overhead of Hardware Loops, greatly reducing redundant Load/Store operations, and fully reusing registers.

![](../../../../figures/vf_fusion_2.png)

**Automatic synchronization within VF**: The compiler precisely inserts necessary synchronization instructions and removes redundant synchronization instructions, greatly releasing the hardware OOO (Out of Order) capability. Users do not need to manually insert synchronization instructions, which greatly reduces the coding difficulty for users.

## VF Fusion Programming Guide<a name="section1631753714477"></a>

1.  Automatic fusion of multiple VF functions: If multiple VF functions have control-flow-equivalent logic and all of them are [Hardware Loop](vf_loop_optimization.md#section11326136133217) loops, the compiler performs VF fusion optimization.

    [Positive example] The VF functions DivVF and AddVF are fused by the compiler into a single VF function, and redundant Load/Store instructions are optimized.

    ```cpp
    template<typename T>
    __simd_vf__ inline void DivVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t repeatTime, uint32_t oneRepNum){
        AscendC::Reg::MaskReg mask;
        AscendC::Reg::RegTensor<T> reg0, reg1, reg2;
        constexpr float num = 1.0f;
        for(uint16_t j = 0; j < repeatTime; ++j){
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(reg0, srcAddr + j * oneRepNum);
            AscendC::Reg::Duplicate(reg1, num, mask);
            AscendC::Reg::Div(reg2, reg1, reg0, mask);
            AscendC::Reg::StoreAlign(dstAddr + j * oneRepNum, reg2, mask);
        }
    }
    template<typename T>
    __simd_vf__ inline void AddVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t repeatTime, uint32_t oneRepNum){
        AscendC::Reg::MaskReg mask;
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        constexpr float num = 1.0f;
        for(uint16_t j = 0; j < repeatTime; ++j){
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + j * oneRepNum);
            AscendC::Reg::Adds(dstReg, srcReg, num, mask);
            AscendC::Reg::StoreAlign(dstAddr + j * oneRepNum, dstReg, mask);
        }
    }
    template<typename T>
    class Kernel {
        public:
        __aicore__ inline Kernel() = default;
        __aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, uint32_t count, AscendC::TPipe* pipeIn){
            // ... 
     
        }
        __aicore__ inline void CopyIn(){
            // ... 
        }
        __aicore__ inline void Compute(){
            AscendC::LocalTensor<T> xLocal = inQueueX.DeQue<T>();
            AscendC::LocalTensor<T> yLocal = outQueueY.AllocTensor<T>();
            AscendC::DataCopy(yLocal, xLocal, count);
            __ubuf__ T* srcAddr = reinterpret_cast<__ubuf__ T*>(xLocal.GetPhyAddr());
            __ubuf__ T* dstAddr = reinterpret_cast<__ubuf__ T*>(yLocal.GetPhyAddr());
            constexpr uint32_t oneRepNum = 256 / sizeof(T);
            uint32_t repeatTime =  count / oneRepNum;
            DivVF(dstAddr, srcAddr, count, repeatTime, oneRepNum);
            AddVF(dstAddr, dstAddr, count, repeatTime, oneRepNum);
            outQueueY.EnQue<T>(yLocal);
        }
        __aicore__ inline void CopyOut(){
            // ... 
        }
        __aicore__ inline void Process(){
            CopyIn();
            Compute();
            CopyOut();
        }
        private:
        AscendC::TPipe* pipe = nullptr;
        uint32_t count;
        AscendC::GlobalTensor<T> xGm;
        AscendC::GlobalTensor<T> yGm;
        AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueX;
        AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueY;
    };
    ```

2.  Use the continuous computation mode of basic APIs: Basic APIs abstract hardware capabilities, expose chip capabilities, and ensure completeness and compatibility. Based on how they operate on data, basic APIs fall into two categories:

    -   Continuous computation APIs: Support computation on the first n data items of a tensor. They compute the consecutive n data items of the source operand and write the results consecutively to the destination operand, addressing the continuous computation of one-dimensional tensors.
    -   High-dimensional tiling APIs: Support Repeat and Stride. These flexible computation APIs provide programming capabilities fully equivalent to those of Builtin APIs, fully leverage hardware advantages, and support operations on parameters such as DataBlock Stride, Repeat Stride, and Mask of each operand.

    In VF fusion optimization, it is recommended to write operators using the continuous computation mode of basic APIs, which can fully unleash the capability of VF fusion optimization. Compared with high-dimensional tiling APIs, continuous computation APIs allow the compiler to better analyze VF fusion optimization and more easily satisfy the VF fusion conditions. Using the continuous computation mode of basic APIs produces operators with better performance.

    [Negative example] When operators are written using the high-dimensional tiling mode of basic APIs, the compiler is affected by the complex computation logic when analyzing VF fusion and cannot perform VF fusion optimization on the Add and Mul APIs.

    ```cpp
    template<typename T>
    class Kernel {
        public:
        // ...
        __aicore__ inline void Compute(){
            AscendC::LocalTensor<T> xLocal = inQueueX.DeQue<T>();
            AscendC::LocalTensor<T> yLocal = outQueueY.AllocTensor<T>();
            AscendC::DataCopy(yLocal, xLocal, inner * outter);
            uint64_t mask = 128;
            AscendC::Add(yLocal, xLocal, xLocal, mask, 4, { 1, 1, 1, 8, 8, 8 });
            AscendC::Mul(yLocal, yLocal, xLocal, mask, 4, { 1, 1, 1, 8, 8, 8 });
            outQueueY.EnQue<T>(yLocal);
        }
        // ...
    };
    ```

    [Positive example] When the continuous computation mode of basic APIs is used, the compiler analyzes the Add and Mul functions, finds that they meet the VF fusion requirements, and fuses Add and Mul into a single VF function.

    ```cpp
    template<typename T>
    class Kernel {
        public:
        // ...
        __aicore__ inline void Compute(){
            AscendC::LocalTensor<T> xLocal = inQueueX.DeQue<T>();
            AscendC::LocalTensor<T> yLocal = outQueueY.AllocTensor<T>();
            AscendC::DataCopy(yLocal, xLocal, inner * outter);
            AscendC::Add(yLocal, xLocal, xLocal, count);
            AscendC::Mul(yLocal, yLocal, xLocal, count);
            outQueueY.EnQue<T>(yLocal);
        }
        // ...
    };
    ```
