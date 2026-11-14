# Build do cliente no Linux (mingw-w64), para rodar com Wine.
# O build do Windows continua sendo TM.sln / build.ps1.
# Use ./wyd-linux.sh build.
cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain-mingw-x64.cmake")
endif()

project(WYD LANGUAGES CXX RC)

set(WYD_RELEASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../wyd-others/Release Cliente"
    CACHE PATH "Pasta do jogo que recebe o WYD.exe e as DLLs do D3DX")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(WYD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Projects/TMProject")
set(WYD_D3DX "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/D3DX")

file(GLOB WYD_SOURCES CONFIGURE_DEPENDS "${WYD_SRC}/*.cpp")

# O Visual Studio não usa glob: um .cpp fora do vcxproj só quebraria lá, no link.
set(WYD_LINUX_ONLY crashlog.cpp mingwalloc.cpp)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${WYD_SRC}/TMProject.vcxproj")
file(READ "${WYD_SRC}/TMProject.vcxproj" WYD_VCXPROJ)
string(TOLOWER "${WYD_VCXPROJ}" WYD_VCXPROJ)
foreach(source IN LISTS WYD_SOURCES)
    get_filename_component(name "${source}" NAME)
    string(TOLOWER "${name}" name)
    string(FIND "${WYD_VCXPROJ}" "<clcompile include=\"${name}\"" position)
    if(position EQUAL -1 AND NOT name IN_LIST WYD_LINUX_ONLY)
        message(FATAL_ERROR "${name} não está em TMProject.vcxproj e quebraria o build do Windows. "
            "Adicione ao projeto, ou a WYD_LINUX_ONLY se for só do Linux.")
    endif()
endforeach()

# O .rc do Visual Studio é UTF-16 e usa "\" nos caminhos; o windres não lê nenhum dos dois.
set(WYD_RC "${CMAKE_CURRENT_BINARY_DIR}/TMProject.rc")
add_custom_command(
    OUTPUT "${WYD_RC}"
    COMMAND sh -c "iconv -f utf-16 -t cp1252 '${WYD_SRC}/TMProject.rc' | tr -d '\\r' | sed -e 's/\\\\\\\\/\\//g' -e 's/Resources\\\\/Resources\\//g' -e 's/\"resource\\.h\"/\"Resource.h\"/' > '${WYD_RC}'"
    DEPENDS "${WYD_SRC}/TMProject.rc"
    VERBATIM)

add_executable(WYD WIN32 ${WYD_SOURCES} "${WYD_RC}")
set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -c 1252")

target_include_directories(WYD PRIVATE
    "${WYD_SRC}"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommonFiles"
    "${WYD_D3DX}/Include")

target_compile_definitions(WYD PRIVATE
    D3D9b_SDK_VERSION=31
    _WINSOCK_DEPRECATED_NO_WARNINGS
    _CRT_SECURE_NO_WARNINGS
    # O strsafe.h do mingw-w64 do Ubuntu 24.04 (via dshow.h) troca strcpy e
    # sprintf por nomes inexistentes; no MSVC isso é só um aviso.
    STRSAFE_NO_DEPRECATE
    WIN32
    NDEBUG
    _WINDOWS
    _WYDCLIENT)

# O código é descompilado: desliga as otimizações que assumem C++ sem comportamento indefinido.
target_compile_options(WYD PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
    -fpermissive
    -w
    -fno-strict-aliasing
    -fwrapv
    -fno-delete-null-pointer-checks
    -fno-aggressive-loop-optimizations
    -fno-lifetime-dse
    -g1
    -gdwarf-4>)

# Basedef.cpp é UTF-8 com BOM: o MSVC grava os literais em CP1252, o GCC grava em UTF-8.
# Sem isto, a chave do serverlist.bin sai errada e os endereços viram lixo.
set_source_files_properties("${WYD_SRC}/Basedef.cpp" PROPERTIES
    COMPILE_OPTIONS "-fexec-charset=CP1252")

target_link_directories(WYD PRIVATE "${WYD_D3DX}/Lib64")
target_link_options(WYD PRIVATE -municode -static)
target_link_libraries(WYD PRIVATE
    d3dx9 d3d9 wininet wsock32 ws2_32 dxguid dsound imm32 dinput8 winmm iphlpapi bcrypt
    strmiids ole32 oleaut32 uuid gdi32 comdlg32 shell32 advapi32)

add_custom_target(deploy
    DEPENDS WYD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:WYD>"
        "${WYD_D3DX}/Runtime64/D3DX9_43.dll"
        "${WYD_D3DX}/Runtime64/D3DCompiler_43.dll"
        "${WYD_RELEASE_DIR}/"
    VERBATIM)
