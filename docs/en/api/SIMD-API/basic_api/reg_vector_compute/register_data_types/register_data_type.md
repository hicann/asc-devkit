# Register Data Types<a name="ZH-CN_TOPIC_0000001952211245"></a>

<!-- md-trans-meta sourceCommit=beaf23cb6ee4b4b8b8ecc3e4442efbf552421ffc translatedAt=2026-08-27T21:57:32.125Z -->

The basic register data types operated by the Reg vector computation APIs are described as follows:

-   **[RegTensor](RegTensor.md)**  
Vector data register, the basic storage unit for Reg vector computation, used for vector computation. The bit width of RegTensor is VL (VectorLength), with a length of 256B, and it can store VL/sizeof(T) data elements (where T denotes the data type). For the b64 or complex32 type, extended storage of 2VL is supported for complex computation.

-   **[MaskReg](MaskReg.md)**  
Mask register, used to select the elements that participate in computation during vector computation. The bit width of MaskReg is VL/8, with a length of 32B.

-   **[UnalignRegForLoad & UnalignRegForStore](UnalignRegForLoad-UnalignRegForStore.md)**  
Unaligned register, used as a buffer to optimize the overhead of consecutive unaligned address access between UB and RegTensor.

-   **[AddrReg](AddrReg.md)**  
Address register, used to store address offsets. AddrReg performs address offset auto-increment based on the stride set for each loop level by calling the CreateAddrReg API.
