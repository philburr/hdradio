cmake_minimum_required (VERSION 3.5)
project (HD_RADIO)

add_executable(hdr
    sync_coarse.cpp
    sync_fine.cpp
    demod.cpp
    interleaver.cpp
    viterbi.cpp
    randomizer.cpp
    parameters.cpp
    layer2.cpp
    hdradio.cpp
    main.cpp)

target_link_libraries(hdr PRIVATE ffts)
target_compile_features(hdr PRIVATE cxx_range_for)
