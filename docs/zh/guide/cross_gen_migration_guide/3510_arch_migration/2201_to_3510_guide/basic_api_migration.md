# 基础API迁移指导<a name="ZH-CN_TOPIC_0000002470508258"></a>

本节针对[NPU架构版本3510](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)的芯片变更对基础API兼容性产生的影响进行说明，并提供基础API的兼容性适配方案。

## 矢量计算<a name="section7364115741514"></a>

-   **3510架构默认不支持Subnormal功能。**

    **说明**：SubNormal浮点数指的是指数位全为0、尾数不为0的浮点数，用于表示比最小正常数更小的值，避免“下溢为0”。3510版本默认不支持Subnormal，Subnormal浮点数在计算中被视为0。

    **兼容方案**：通过设置config模板参数来配置Subnormal计算模式。软件模拟对Subnormal数据的处理，通过精度扩展等处理方式来避免Subnormal浮点数下溢为0。

    **表1**  涉及Subnormal的API和config参数说明

    <a name="table288510533427"></a>
    <table><thead align="left"><tr id="row388575318425"><th class="cellrowborder" align="center" valign="top" width="27%" id="mcps1.2.3.1.1"><p id="p12885175314420"><a name="p12885175314420"></a><a name="p12885175314420"></a>Ascend C基础API</p>
    </th>
    <th class="cellrowborder" align="center" valign="top" width="73%" id="mcps1.2.3.1.2"><p id="p1288585311428"><a name="p1288585311428"></a><a name="p1288585311428"></a>兼容说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row188851453184214"><td class="cellrowborder" align="center" valign="top" width="27%" headers="mcps1.2.3.1.1 "><p id="p1188545310427"><a name="p1188545310427"></a><a name="p1188545310427"></a>Exp、Ln、Reciprocal、Sqrt、Rsqrt、Div</p>
    </td>
    <td class="cellrowborder" align="left" valign="top" width="73%" headers="mcps1.2.3.1.2 "><p id="p1055013361204"><a name="p1055013361204"></a><a name="p1055013361204"></a>以Ln接口为例来进行说明。</p>
    <p id="p151399151208"><a name="p151399151208"></a><a name="p151399151208"></a>通过LnConfig结构体的参数algo来配置Subnormal计算模式。algo取值如下：</p>
    <a name="ul118851253124216"></a><a name="ul118851253124216"></a><ul id="ul118851253124216"><li>LnAlgo::INTRINSIC、LnAlgo::PRECISION_1ULP_FTZ_TRUE，使用单指令计算得出结果，所有Subnormal被近似为0。</li><li>LnAlgo::PRECISION_1ULP_FTZ_FALSE，支持Subnormal数据计算。</li></ul>
    <p id="p9885135319427"><a name="p9885135319427"></a><a name="p9885135319427"></a>该参数默认值DEFAULT_LN_CONFIG的取值如下：</p>
    <a name="screen18681133513203"></a><a name="screen18681133513203"></a><pre class="screen" codetype="Cpp" id="screen18681133513203">constexpr LnConfig DEFAULT_LN_CONFIG = { LnAlgo::INTRINSIC };</pre>
    </td>
    </tr>
    </tbody>
    </table>

    可以参考以下代码片段：

    ```
    // 定义模板参数
    constexpr AscendC::LnConfig CONFIG = {
        AscendC::LnAlgo::PRECISION_1ULP_FTZ_FALSE
    };
    constexpr uint32_t DATA_COUNT = 1024;
    constexpr uint32_t xAddr = 0;
    constexpr uint32_t yAddr = xAddr + DATA_COUNT * sizeof(half);

    // 核函数（Kernel）入口：使用__vector__显式声明Vector核函数（Kernel），搭配C++ Tensor方式编程
    __vector__ __global__ void ln_custom(__gm__ uint8_t* src, __gm__ uint8_t* dst)
    {
        // C++ Tensor需要手动调用InitSocState初始化全局状态寄存器
        AscendC::InitSocState();

        AscendC::GlobalTensor<half> srcGlobal;
        AscendC::GlobalTensor<half> dstGlobal;
        srcGlobal.SetGlobalBuffer((__gm__ half*)src, DATA_COUNT);
        dstGlobal.SetGlobalBuffer((__gm__ half*)dst, DATA_COUNT);

        // 直接构造指定地址和存储位置的LocalTensor（C++ Tensor方式）
        AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::VECCALC, xAddr, DATA_COUNT);
        AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::VECCALC, yAddr, DATA_COUNT);

        AscendC::DataCopy(srcLocal, srcGlobal, DATA_COUNT);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

        // 调用Ln基础API，传入模板参数
        AscendC::Ln<half, CONFIG>(dstLocal, srcLocal, DATA_COUNT);

        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::DataCopy(dstGlobal, dstLocal, DATA_COUNT);

        AscendC::PipeBarrier<PIPE_ALL>();
    }
    ```

## 数据搬运<a name="section7530159122210"></a>

-   **DataCopy接口不支持L1 Buffer-\>GM通路。**

    **说明**：硬件删除L1 Buffer到GM的通路，无法将数据从L1 Buffer直接搬运到GM中。现有接口不支持L1 Buffer到GM的直接搬运。

    **兼容方案**：对于纯Cube计算场景：在GM多分配一个单位矩阵，通过Mmad矩阵乘法计算输出到L0C Buffer，再从L0C Buffer通过Fixpipe搬运到GM。对于Vector和Cube计算融合场景，可以通过L1 Buffer搬运到Unified Buffer（UB），再搬运到GM。以下以纯Cube计算场景为例进行说明，介绍算子核心流程，具体可参考[L1到GM搬运兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm)。

    1.  将矩阵A从GM搬运到L1 Buffer。

        ```
        __aicore__ inline void CopyGmToL1A(AscendC::LocalTensor<T> a1Local)
        {
            AscendC::Nd2NzParams intriParams1{1, M, K, 0, K, M, 1, 0};
            AscendC::DataCopy(a1Local, aGlobal, intriParams1);
        }
        ```

    2.  将矩阵B（矩阵B为单位矩阵）从GM搬运到L1 Buffer。

        ```
        __aicore__ inline void CopyGmToL1B(AscendC::LocalTensor<U> b1Local)
        {
            AscendC::Nd2NzParams intriParams2{1, K, N, 0, N, K, 1, 0};
            AscendC::DataCopy(b1Local, bGlobal, intriParams2);
        }
        ```

    3.  将矩阵A从L1 Buffer搬运到L0A Buffer。

        ```
        __aicore__ inline void Load2DL1AToL0A(AscendC::LocalTensor<T> a1Local, AscendC::LocalTensor<T> a2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2Local, a1Local, loadDataParams);
        }
        ```

    4.  将矩阵B从L1 Buffer搬运到L0B Buffer。

        ```
        __aicore__ inline void Load2DL1BToL0B(AscendC::LocalTensor<U> b1Local, AscendC::LocalTensor<U> b2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(b2Local, b1Local, loadDataParams);
        }
        ```

    5.  进行Mmad矩阵计算，结果输出到L0C Buffer。

        ```
        __aicore__ inline void Compute(
            AscendC::LocalTensor<S> co1Local, AscendC::LocalTensor<T> a2Local, AscendC::LocalTensor<U> b2Local)
        {
            AscendC::MmadParams mmadParams;
            ...
            AscendC::Mmad(co1Local, a2Local, b2Local, mmadParams);
        }
        ```

    6.  通过FixPipe将矩阵C从L0C Buffer拷贝到GM。

        ```
        __aicore__ inline void CopyL0CToGm(AscendC::LocalTensor<S> co1Local)
        {
            AscendC::FixpipeParamsV220 fixpipeParams;
            ...
            AscendC::Fixpipe<S, S, AscendC::CFG_ROW_MAJOR>(cGlobal, co1Local, fixpipeParams);
        }
        ```

-   **不支持SetLoadDataBoundary接口。**

    **说明**：3510架构硬件删除了L1 Buffer的边界值设定相关寄存器，不再支持SetLoadDataBoundary接口。该接口用于设置LoadData（卷积数据搬运）时L1 Buffer的边界值。如果指令在处理源操作数时，源操作数在L1 Buffer上的地址超出设置的边界，则会从L1 Buffer的起始地址开始读取数据。设置为0表示无边界，可以使用整个L1 Buffer。

    **兼容方案**：

    -   [NPU架构版本2201](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)的接口参数boundaryValue设置为0时与3510架构版本等价。
    -   如果需要在L1 Buffer上循环读取操作数，需要将对应的LoadData（卷积数据搬运）接口手动拆分成多条指令，手动绕回。具体代码可参考[SetLoadDataBoundary兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary)。

    ![](../../../figures/fig_1_zn.png)

    如上图所示，以L1 Buffer到L0A Buffer的搬运为例。矩阵A为half数据类型，大小为32 \* 32的矩阵，假设边界为512B，可以重复搬运数据到L0A Buffer，在每次搬运时设置目的操作数的地址偏移量。

    1.  将矩阵A从GM搬运到L1 Buffer。

        ```
        __aicore__ inline void CopyGmToA1Nd2Nz(AscendC::LocalTensor<T>& leftMatrix)
        {
            AscendC::Nd2NzParams nd2nzParams;
            ...
            AscendC::DataCopy(leftMatrix, aGlobal, nd2nzParams);
        }
        ```

    2.  将矩阵B从GM搬运到L1 Buffer。

        ```
        __aicore__ inline void CopyGmToB1Nd2Nz(AscendC::LocalTensor<U>& rightMatrix)
        {
            AscendC::Nd2NzParams nd2nzParams;
            ...
            AscendC::DataCopy(rightMatrix, bGlobal, nd2nzParams);
        }
        ```

    3.  将矩阵A从L1 Buffer搬运到L0A Buffer。

        ```
            __aicore__ inline void Load3DA1ToL0A(AscendC::LocalTensor<T>& leftMatrix, AscendC::LocalTensor<T>& a2)
            {
                AscendC::LoadData3DParamsV2Pro loadData3dParamsPro;
                ...
        #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
                ...
                AscendC::SetLoadDataRepeat({0, 1, 0});
                AscendC::SetLoadDataBoundary(1024);
                AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
        #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
                uint16_t dstStride = AscendC::DivCeil(M / 2, 16);
                ...
                AscendC::SetLoadDataRepeatWithStride({0, 1, 0, dstStride});
                // 多次调用LoadData进行手动绕回
                AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
                AscendC::LocalTensor<T> a3 = a2[256];
                AscendC::LoadData(a3, leftMatrix, loadData3dParamsPro);
                AscendC::LocalTensor<T> a4 = a2[512];
                AscendC::LoadData(a4, leftMatrix, loadData3dParamsPro);
                AscendC::LocalTensor<T> a5 = a2[768];
                AscendC::LoadData(a5, leftMatrix, loadData3dParamsPro);
        #endif
            }
        ```

    4.  将矩阵B从L1 Buffer搬运到L0B Buffer。

        ```
            __aicore__ inline void Load3DB1ToL0B(AscendC::LocalTensor<U>& rightMatrix, AscendC::LocalTensor<U>& b2)
            {
                AscendC::LoadData3DParamsV2<U> loadData3dParams;
                ...
                uint8_t padList[AscendC::PAD_SIZE] = {0, 0, 0, 0};
                static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};
                AscendC::SetFmatrix(N, 1, padList, AscendC::FmatrixMode::FMATRIX_LEFT);
        #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
                AscendC::SetLoadDataRepeat({0, 1, 0});
                AscendC::SetLoadDataBoundary(0);
                AscendC::SetLoadDataPaddingValue(0);
                AscendC::LoadData<U, LOAD3D_CONFIG>(b2, rightMatrix, loadData3dParams);
        #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
                uint16_t dstStride = AscendC::DivCeil(N, 16);
                AscendC::SetLoadDataRepeatWithStride({0, 1, 0, dstStride});
                AscendC::SetLoadDataPaddingValue(0);
                AscendC::LoadDataWithStride<U, LOAD3D_CONFIG>(b2, rightMatrix, loadData3dParams);
        #endif
            }
        ```

    5.  矩阵计算。

        ```
        __aicore__ inline void Compute(AscendC::LocalTensor<T>& a2, AscendC::LocalTensor<U>& b2,
                                    AscendC::LocalTensor<V>& co1Local)
        {
            AscendC::MmadParams mmadParams;
            ...
            AscendC::Mmad(co1Local, a2, b2, mmadParams);
        }
        ```

    6.  将矩阵C从L0C Buffer搬运到GM。

        ```
        __aicore__ inline void CopyL0CToGm(AscendC::LocalTensor<V>& co1Local, const AscendC::GlobalTensor<S>& gm)
        {
            AscendC::FixpipeParamsV220 fixpipeParams(N, static_cast<uint16_t>(M),
                                            AscendC::DivCeil(M, AscendC::BLOCK_CUBE) * AscendC::BLOCK_CUBE, static_cast<uint16_t>(N), 0);
            ...
            AscendC::Fixpipe<S, V, AscendC::CFG_ROW_MAJOR>(gm, co1Local, fixpipeParams);
        }
        ```

## 矩阵计算<a name="section17560113713122"></a>

-   **Cube计算单元删除int4b\_t数据类型。**

    **说明**：相较于2201架构版本，3510架构版本的Cube计算单元不支持int4b\_t。相关的基础API有LoadData、Mmad和LoadDataWithTranspose，这些接口不再支持int4b\_t。

    **兼容方案**：算子侧通过编写CV融合算子在Vector Core进行int4b\_t到int8\_t的Cast转换，再通过UB搬运到L1后进行Mmad计算。具体代码可参考[int4数据类型下Matmul兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/matmul_s4)。图层面可以在该算子前增加Cast节点进行int4b\_t到int8\_t的转换。

    1.  在Vector Core进行int4b\_t到int8\_t的Cast转换，转换后的数据保存到新的GM空间中。

        ```
        __aicore__ inline void Unzip(AscendC::GlobalTensor<int8_t>& dstGlobalTensor,
            AscendC::GlobalTensor<int8_t>& srcGlobalTensor, uint32_t count)
        {
            constexpr uint32_t oneBlockBytes = 32;
            uint32_t srcAddr = 0;
            uint32_t tmpAddr = srcAddr + AscendC::AlignUp(count * sizeof(int8_t), oneBlockBytes);
            uint32_t dstAddr = tmpAddr + AscendC::AlignUp(count * 2 * sizeof(half), oneBlockBytes);

            AscendC::LocalTensor<int8_t> srcLocalTensor(AscendC::TPosition::VECCALC, srcAddr, count);
            AscendC::LocalTensor<half> tmpTensor(AscendC::TPosition::VECCALC, tmpAddr, count * 2);
            AscendC::LocalTensor<int8_t> dstLocalTensor(AscendC::TPosition::VECCALC, dstAddr, count * 2);

            AscendC::DataCopy(srcLocalTensor, srcGlobalTensor, count);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

            AscendC::LocalTensor<AscendC::int4b_t> int4SrcLocalTensor =
                srcLocalTensor.ReinterpretCast<AscendC::int4b_t>();
            AscendC::Cast<half, AscendC::int4b_t>(tmpTensor, int4SrcLocalTensor, AscendC::RoundMode::CAST_NONE, count * 2);
            AscendC::PipeBarrier<PIPE_V>();
            AscendC::Cast<int8_t, half>(dstLocalTensor, tmpTensor, AscendC::RoundMode::CAST_CEIL, count * 2);

            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::DataCopy(dstGlobalTensor, dstLocalTensor, count * 2);
        }
        ```

    2.  进行int8\_t数据类型的矩阵计算。

        ```
        __aicore__ inline void RunMatmul()
        {
            ...
            int offsetA = 0;
            int offsetB = 0;
            int offsetC = 0;
            ...
            auto gmA = aGlobal[offsetA];
            auto gmB = bGlobal[offsetB];
            auto gmC = cGlobal[offsetC];

            AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
            mm.SetSubBlockIdx(0);
            mm.Init(&tiling, pipe);
            mm.SetTensorA(gmA, false);
            mm.SetTensorB(gmB, false);
            mm.IterateAll(gmC);
        }
        ```

-   **L0A Buffer分形改变，从ZZ转换为ZN格式。**

    **说明**：涉及的API有LoadData、Mmad和LoadDataWithTranspose。

    -   2201架构版本，参与矩阵乘计算（A \* B = C）时， ABC矩阵的数据排布格式分别为Zz，Zn，Nz。A、B、C矩阵分别位于L0A Buffer、L0B Buffer、L0C Buffer。

        矩阵A：每个分形矩阵内部是行主序，分形矩阵之间是行主序。分形Shape为16 x \(32B/sizeof\(AType\)\)，大小为512Byte。

        矩阵B：每个分形矩阵内部是列主序，分形矩阵之间是行主序。分形Shape为 \(32B/sizeof\(BType\)\) x 16，大小为512Byte。

        矩阵C：每个分形矩阵内部是行主序，分形矩阵之间是列主序。分形Shape为16 x 16，大小为256个元素。

        ![](../../../figures/fig_2_zn.png)

    -   3510架构版本，参与矩阵乘计算（A \* B = C）时， ABC矩阵的数据排布格式分别为Nz，Zn，Nz。

        矩阵A：每个分形矩阵内部是行主序，分形矩阵之间是列主序。其Shape为16 x \(32B/sizeof\(AType\)\)，大小为512Byte。

        矩阵B：每个分形矩阵内部是列主序，分形矩阵之间是行主序。其Shape为 \(32B/sizeof\(BType\)\) x 16，大小为512Byte。

        矩阵C：每个分形矩阵内部是行主序，分形矩阵之间是列主序。其Shape为16 x 16，大小为256个元素。

        ![](../../../figures/fig_3_zn.png)

    **兼容方案**：非L0A Buffer切分的场景兼容2201版本，L0A Buffer切分的场景需要根据新的分形重新适配。具体代码可参考[pattern_transformation兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/pattern_transformation)。

    在2201架构中，矩阵计算要求左矩阵为ZZ分形（3510中为NZ），右矩阵为ZN分形，由于L1 Buffer的数据分形为NZ，所以2201架构下将左矩阵从L1 Buffer搬运到L0A Buffer需要额外做NZ分形到ZZ分形的转换，3510架构下则不用转换分形。

    ![](../../../figures/fig_4_zn.png)

    分形变化带来的变动主要体现在L1 Buffer到L0A Buffer的搬运过程，以下代码片段进行展示：

    ```
        __aicore__ inline void DataLoadA(AscendC::LocalTensor<T> a1, AscendC::LocalTensor<T> a2)
        {
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
            constexpr uint32_t mBlocks = M / CUBE_BLOCK;
            constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
            int srcOffset = 0;
            int dstOffset = 0;
            for (uint32_t i = 0; i < mBlocks; ++i) {
                AscendC::LoadData2DParams loadDataParams;
                loadDataParams.repeatTimes = kBlocks;
                loadDataParams.srcStride = mBlocks;
                loadDataParams.ifTranspose = false;
                AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
                srcOffset += CUBE_BLOCK * CUBE_BLOCK;
                dstOffset += K * CUBE_BLOCK;
            }
    #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
            AscendC::LoadData2DParamsV2 loadDataParams;
            loadDataParams.mStartPosition = 0;
            loadDataParams.kStartPosition = 0;
            loadDataParams.mStep = AscendC::DivCeil(M, CUBE_BLOCK);
            loadDataParams.kStep = AscendC::DivCeil(K * sizeof(T), C0_SIZE);
            loadDataParams.srcStride = AscendC::DivCeil(M, CUBE_BLOCK);
            loadDataParams.dstStride = AscendC::DivCeil(M, CUBE_BLOCK);
            loadDataParams.sid = 0;
            loadDataParams.ifTranspose = false;
            AscendC::LoadData(a2, a1, loadDataParams);
    #endif
        }
    ```

-   **3510架构版本硬件架构删除4：2结构化稀疏功能。**<a name="li69092585134"></a>

    **说明**：LoadDataWithSparse用于将存储在L1 Buffer中的512B稠密权重矩阵搬运到L0B buffer，并同时读取128B的索引矩阵以实现稠密矩阵的稀疏化。由于3510架构版本不支持结构化稀疏功能，因此LoadDataWithSparse在此版本中并不适用。另一方面，MmadWithSparse负责执行矩阵乘加操作，其中右矩阵B为稠密矩阵，需要通过调用LoadDataWithSparse进行载入。由于3510架构不支持LoadDataWithSparse，因此MmadWithSparse也无法在3510架构版本中使用。

    **兼容方案**：在算子侧可以不调用LoadDatawithSparse进行矩阵稠密转稀疏操作，然后使用Mmad进行正常的稠密矩阵计算。稀疏矩阵相关算法可参考MmadWithSparse中的介绍。

-   **3510架构版本删除GM-\>L0A Buffer/L0B Buffer通路**

    **说明**：硬件删除GM-\>L0A Buffer/L0B Buffer通路，调用LoadData时，不再支持这些通路。

    **兼容方案**：实现GM-\>L0A Buffer/L0B Buffer搬运需拆分成两步进行，先从GM搬运到L1 Buffer，再从L1 Buffer搬运到L0A Buffer、L0B Buffer。

     以GM-\>L1 Buffer-\>L0A Buffer通路为例可以参考以下步骤：

    1.  将矩阵A从GM搬运到L1 Buffer。

        ```
        __aicore__ inline void CopyGmToA1(AscendC::LocalTensor<T>& leftMatrix)
        {
            AscendC::Nd2NzParams intriParams1{1, 64, 128, 0, 128, 64, 1, 0};
            AscendC::DataCopy(leftMatrix, aGlobal, intriParams1);
        }
        ```

    2.  将矩阵A从L1 Buffer搬运到L0A Buffer。

        ```
        __aicore__ inline void Load2DA1ToL0A(AscendC::LocalTensor<T>& a1, AscendC::LocalTensor<T>& a2)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2, a1, loadDataParams);
        }
        ```

-   **3510架构版本删除L0A Buffer/L0B Buffer初始化的相关硬件指令。**

    **说明**：Fill接口将特定存储位置的LocalTensor初始化为某一具体数值，不支持直接初始化L0A Buffer、L0B Buffer。

    **兼容方案**：先通过Fill接口初始化L1 Buffer，再通过LoadData接口将L1 Buffer上的数据搬运到L0A Buffer、L0B Buffer。具体代码可参考[Fill兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fill)。

    以GM-\>L1 Buffer-\>L0A Buffer的数据通路为例：

    1.  初始化L1 Buffer。

        ```
        __aicore__ inline void InitConstA1(AscendC::LocalTensor<T>& a1Local)
        {
            AscendC::Fill(a1Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 32), 0, 1});
        }
        ```

    2.  调用LoadData接口将L1 Buffer上的数据搬运到L0A Buffer。

        ```
        __aicore__ inline void Load2DA1ToA2(AscendC::LocalTensor<T>& a1Local, AscendC::LocalTensor<T>& a2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2Local, a1Local, loadDataParams);
        }
        ```

## 系统变量访问<a name="section34615332215"></a>

-   **不支持CheckLocalMemoryIA，3510架构版本相关寄存器删除。**

    **说明**：CheckLocalMemoryIA监视设定范围内的UB读写行为，如果监视到有设定范围的读写行为则会出现EXCEPTION报错，未监视到设定范围的读写行为则不会报错。

    **兼容方案**：该接口为调测接口，对功能无影响。

## 调测API<a name="section12611957102116"></a>

-   **L1 Buffer上不支持Tensor信息的打印。**

    **说明**：因芯片删除L1 Buffer-\>GM通路，不支持L1 Buffer-\>GM的功能。

    **兼容方案**：该接口为调测接口，对功能无影响。
