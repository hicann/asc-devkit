# Tiling Implementation for High-Level APIs<a name="ZH-CN_TOPIC_0000002365633590"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T12:03:10.350Z pushedAt=2026-09-02T11:52:23.027Z -->

1.  Define the tiling structure first:

    ```
    namespace optiling {
    BEGIN_TILING_DATA_DEF(MyAddTilingData)  // Declare the name of the tiling structure
      TILING_DATA_FIELD_DEF_STRUCT(TCubeTiling, cubeTilingData);   // Reference the tiling structure of the high-level API
      TILING_DATA_FIELD_DEF(uint32_t, field);   // Reference structure of the structure member
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(MyAdd, MyAddTilingData)  // Register the tiling structure with the operator
    }
    ```

2.  Initialize the tiling structure through the tiling function provided with the high-level API:

    ```
    static ge::graphStatus TilingFunc(gert::TilingContext* context) {
        int32_t M = 1024;
        int32_t N = 640;
        int32_t K = 256;
        int32_t baseM = 128;
        int32_t baseN = 128;
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        MultiCoreMatmulTiling cubeTiling(ascendcPlatform);
        cubeTiling.SetDim(2);
        cubeTiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
        cubeTiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
        cubeTiling.SetCType(TPosition::LCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
        cubeTiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
        cubeTiling.SetShape(M, N, K);
        cubeTiling.SetOrgShape(M, N, K);
        cubeTiling.SetFixSplit(baseM, baseN, -1);
        cubeTiling.SetBias(true);
        cubeTiling.SetBufferSpace(-1, -1, -1);
        MyAddTilingData tiling;
        if (cubeTiling.GetTiling(tiling.cubeTilingData) == -1){
            return ge::GRAPH_FAILED;
        }
        // some code
    }
    ```

