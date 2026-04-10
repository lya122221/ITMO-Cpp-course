add_executable(
    hamarc
    main.cpp
    argparser.cpp
    archiveoperation.cpp
    filesoperation.cpp
    hamcode.cpp
)

target_compile_features(hamarc PRIVATE cxx_std_20)
