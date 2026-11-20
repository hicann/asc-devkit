# Overview of Data Move-In for Cube Computation

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:55:26.725Z pushedAt=2026-10-27T13:03:35.335Z nl=2 -->

The movement APIs for cube computation mainly implement efficient movement from Global Memory to L1 Buffer and from L1 Buffer to L0A Buffer/L0B Buffer/L0ScaleA Buffer/L0ScaleB Buffer/BiasTable Buffer/Fixpipe Buffer.

For Ascend 950PR&950DT products:

**Table 1** Movement paths and storage hierarchy

<a name="tensor_api_matrix_data_in_overview_table_data_path"></a>

|Source Location|Source Address Alignment Requirement|Destination Location|Destination Address Alignment Requirement|Format Conversion|Typical Application Scenario|
|--------|--------|--------|--------|--------|--------|
|Global Memory|1 byte|L1 Buffer|32 bytes|ND2ND, ND2NZ, ND2ZN, DN2NZ, DN2ZN, NZ2NZ, ZN2ZN|Loads data such as the left cube and right cube from global memory to L1 Buffer.|
|Global Memory|1 byte|L1 Buffer|32 bytes|ScaleND2ZZ, ScaleDN2ZZ, ZZ2ZZ, ScaleND2NN, ScaleDN2NN, NN2NN|Loads the left cube scale factors and right cube scale factors from global memory to L1 Buffer.|
|L1 Buffer|32 bytes|L0A Buffer|512 bytes|NZ2NZ, ZN2NZ|Loads left cube data from L1 Buffer to L0A Buffer.|
|L1 Buffer|32 bytes|L0B Buffer|512 bytes|ZN2ZN, NZ2ZN|Loads right cube data from L1 Buffer to L0B Buffer.|
|L1 Buffer|32 bytes|L0ScaleA Buffer|32 bytes|ZZ2ZZ|Loads left cube scale factors from L1 Buffer to L0ScaleA Buffer.|
|L1 Buffer|32 bytes|L0ScaleB Buffer|32 bytes|NN2NN|Loads right cube scale factors from L1 Buffer to L0ScaleB Buffer.|
|L1 Buffer|32 bytes|BiasTable Buffer|64 bytes|ND2ND|Loads bias data from L1 Buffer to BiasTable Buffer.|
|L1 Buffer|32 bytes|Fixpipe Buffer|128 bytes|ND2ND|Loads quantization data from L1 Buffer to Fixpipe Buffer.|
