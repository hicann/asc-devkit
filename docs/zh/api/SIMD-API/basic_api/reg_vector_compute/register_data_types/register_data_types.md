# 寄存器数据类型<a name="ZH-CN_TOPIC_0000001952211245"></a>

Reg矢量计算API操作的基础寄存器数据类型介绍如下：

-   **[RegTensor](RegTensor.md)**  
矢量数据寄存器，Reg矢量计算基本存储单元，用于矢量计算。RegTensor的位宽是VL（VectorLength），长度为256B，可存储VL/sizeof(T)个数据（T表示数据类型）。b64或complex32类型下，支持扩展存储2VL用于复杂计算。

-   **[MaskReg](MaskReg.md)**  
掩码寄存器，用于矢量计算中选择参与计算的元素。MaskReg的位宽是VL/8，长度为32B。

-   **[UnalignRegForLoad & UnalignRegForStore](UnalignRegForLoad-UnalignRegForStore.md)**  
非对齐寄存器，作为缓冲区，用来优化UB和RegTensor之间的连续非对齐地址访问的开销。

-   **[AddrReg](AddrReg.md)**  
地址寄存器，用于存储地址偏移量的寄存器。AddrReg通过调用CreateAddrReg接口根据每层循环所设置的stride进行地址偏移自增。
