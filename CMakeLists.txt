cmake_minimum_required(VERSION 3.7)
project(tinylog)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        catagory.c
        catagory.h
        config.c
        config.h
        dict.c
        dict.h
        layout.c
        layout.h
        logdef.h
        logger.c
        logger.h
        logmsg.c
        logmsg.h
        logstr.c
        logstr.h
        snprintf.c
        snprintf.h
        testmain.c
        tick.c
        tick.h
        tinylog.c
        tinylog.h)

add_executable(tinylog ${SOURCE_FILES})