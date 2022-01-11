cmake_minimum_required(VERSION 3.20)
project(Base)

set(CMAKE_CXX_STANDARD 17)

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

#Объявление библиотеки
if(NOT TARGET Base)
    add_library(Base STATIC ${Base_SRC})
endif()
