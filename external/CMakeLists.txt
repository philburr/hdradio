cmake_minimum_required (VERSION 3.5)
enable_language(ASM)
project(FFTS)

add_library (ffts
    ffts/src/ffts.c
    ffts/src/ffts_small.c
    ffts/src/ffts_nd.c
    ffts/src/ffts_real.c
    ffts/src/ffts_real_nd.c
    ffts/src/patterns.c
    ffts/src/codegen.c
    ffts/src/sse.s
    )

target_compile_options(ffts
    PRIVATE
    -DHAVE_SSE
    -Wno-pointer-to-int-cast
    -Wno-int-conversion
    )

target_include_directories (ffts
    PRIVATE
    ffts/
    ${FFTS_SOURCE_DIR}
    PUBLIC
    ffts/
    ffts/include
    )

