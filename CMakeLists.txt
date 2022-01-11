cmake_minimum_required(VERSION 3.20)
project(Base)

set(CMAKE_CXX_STANDARD 17)
include(Fetch.cmake)

set(Base_HDR
        Types.h
        Algorithms.h
        EnumInfo.h
        Result.h
        Variable.h
        FileFunctions.h
        )
set(Base_SRC
        EnumInfo.cpp
        Result.cpp
        Variable.cpp
        FileFunctions.cpp
        )

if(NOT NOT_FILESYSTEM)
    #Если файловая система не отключена проверяем что у нас есть
    try_compile(HAS_STD_FS
            "${CMAKE_BINARY_DIR}/temp"
            "${LIB_DIR}/FileSystem/TestStdFileSystem.cpp"
            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
            )

    try_compile(HAS_STD_EXPER_FS
            "${CMAKE_BINARY_DIR}/temp"
            "${LIB_DIR}/FileSystem/TestExperFileSystem.cpp"
            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
            LINK_LIBRARIES stdc++fs)
    #Если файловой системы нет, то добавляем собственную реализацию
    if(NOT HAS_STD_FS AND NOT HAS_STD_EXPER_FS)
        set(Base_SRC ${Base_SRC} ${LIB_DIR}/FileSystem/OwnFileSystem.cpp)
    endif()
endif()

#Объявление библиотеки
if(NOT TARGET Base)
    add_library(Base STATIC ${Base_SRC})

    if(HAS_STD_FS)
        message("Using std::filesystem")
        target_compile_definitions(Base PUBLIC FILESYSTEM_STD)
    elseif(HAS_STD_EXPER_FS)
        message("Using std::experimental::filesystem")
        target_compile_definitions(Base PUBLIC FILESYSTEM_EXPER)
        target_link_libraries(Base PUBLIC stdc++fs)
    else()
        message("Using own filesystem")
        target_compile_definitions(Base PUBLIC FILESYSTEM_OWN)
    endif()
endif()
