# Register Data Types<a name="ZH-CN_TOPIC_0000001952211245"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-05T03:43:33.868Z pushedAt=2026-11-18T01:25:30.949Z nl=2 -->

The basic register data types operated by the Reg vector computation APIs are described as follows:

-   **[RegTensor](RegTensor.md)**  
Vector data register, the basic storage unit for Reg vector computation, is used for vector computation. The bit width of **RegTensor** is **VL** (VectorLength), with a length of 256B, and it can store **VL**/sizeof(T) data elements (T indicates the data type). For the b64 or complex32 type, extended storage of 2VL is supported for complex computation.

-   **[MaskReg](MaskReg.md)**  
Mask register, used to select the elements that participate in vector computation. The bit width of **MaskReg** is **VL**/8, with a length of 32B.

-   **[UnalignRegForLoad & UnalignRegForStore](UnalignRegForLoad-UnalignRegForStore.md)**  
Unaligned register, used as a buffer to optimize the overhead of consecutive unaligned address access between the Unified Buffer (UB) and **RegTensor**.

-   **[AddrReg](AddrReg.md)**  
Address register, used to store address offsets. **AddrReg** auto-increments the address offset based on the stride set for each loop level by calling the **CreateAddrReg** API.
