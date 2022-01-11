
#Если FETCH_DIR не объявлен, то считаем что это корневой проект
if(NOT DEFINED FETCH_DIR)
    #Объявляем признак что это корневой проект
    set(ROOT_PROJECT TRUE)

    #По умолчанию все библиотеки сохраняются в паке Libs корневого проекта
    set(FETCH_DIR ${CMAKE_SOURCE_DIR}/Libs)
    #Текущий каталог
    set(LIB_DIR ${CMAKE_SOURCE_DIR})
else()
    set(ROOT_PROJECT FALSE)
    #Для библиотеки переопределяем текущий каталог
    set(LIB_DIR ${FETCH_DIR}/${PROJECT_NAME})
    message("LIB_DIR for project ${PROJECT_NAME}: ${LIB_DIR}")

endif()

#Если не указано что либо другого, то тесты включены по умолчанию
if(NOT DEFINED BUILD_TEST)
    set(BUILD_TEST ON)
endif()

#include(FetchContent)

#Parameters
macro(Declare NAME REP INCL ADD)
    set(${NAME}_DIR ${FETCH_DIR}/${NAME})
    if(${INCL} STREQUAL ".")
        set(${NAME}_INCL ${${NAME}_DIR})
    else()
        set(${NAME}_INCL ${${NAME}_DIR}/${INCL})
    endif()

    message("Define ${NAME}_DIR variable with value: ${${NAME}_DIR} ")
    message("Define ${NAME}_INCL variable with value: ${${NAME}_INCL} ")

    #Вариант с иcпользованием FetchContent
    #FetchContent_Declare(${NAME}
    #        GIT_REPOSITORY https://${REP}.git
    #        GIT_TAG origin/master
    #        SOURCE_DIR ${${NAME}_DIR})
    #FetchContent_Populate(${NAME})

    #Вариант с использованием просто клонирования гит
    if(NOT EXISTS ${${NAME}_DIR})
        exec_program("git" "" ARGS "clone --depth=1 https://${REP}.git ${${NAME}_DIR}")
    endif()
    if(${ADD})
        add_subdirectory(${${NAME}_DIR} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Libs/${NAME})
    endif()
    set(${PROJECT_NAME}_DEPS_INCL ${${PROJECT_NAME}_DEPS_INCL} ${${NAME}_INCL} ${${NAME}_DEPS_INCL})
endmacro()

macro(Populate NAME REP INCL)
    Declare(${NAME} ${REP} ${INCL} OFF)
endmacro()

macro(MakeAvailable NAME REP INCL)
    Declare(${NAME} ${REP} ${INCL} ON)
endmacro()

#Макрос делает доступным родительскому проекту текущие зависимости
macro(DepsAvailable)
    if(NOT ${ROOT_PROJECT})
        set(${PROJECT_NAME}_DEPS_INCL ${${PROJECT_NAME}_DEPS_INCL} PARENT_SCOPE)
        message("Include dirs ${PROJECT_NAME}_DEPS_INCL set to parent project")
    endif()
endmacro()

macro(ShowDepsIncl)
    message("${PROJECT_NAME}_DEPS_INCL: ${${PROJECT_NAME}_DEPS_INCL}")
endmacro()

macro(GitClone NAME LINK_REP)
    if(NOT EXISTS ${NAME})
        exec_program("git" "" ARGS "clone --depth=1 https://${LINK_REP}.git ${NAME}")
    endif()
endmacro()