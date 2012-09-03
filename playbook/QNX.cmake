#INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME QNX)

SET(CMAKE_SYSTEM_PROCESSOR armv7)

# specify the cross compiler

#CMAKE_FORCE_C_COMPILER(qcc GNU)
#CMAKE_FORCE_CXX_COMPILER(QCC GNU)

SET(CMAKE_C_FLAGS "-fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_CXX_FLAGS "-x c++ -fPIC -fstack-protector -fstack-protector-all")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /hddhome/joshua/bbndk-2.1.0-beta1/target/qnx6/ /usr/local/Trolltech)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(QNX TRUE)

SET(CMAKE_MAKE_PROGRAM "make${HOST_EXECUTABLE_SUFFIX}"    CACHE PATH "QNX Make Program")
SET(CMAKE_SH           "sh${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX shell Program")
SET(CMAKE_AR           "nto${CMAKE_SYSTEM_PROCESSOR}-ar${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX ar Program")
SET(CMAKE_RANLIB       "nto${CMAKE_SYSTEM_PROCESSOR}-ranlib${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX ranlib Program")
SET(CMAKE_NM           "nto${CMAKE_SYSTEM_PROCESSOR}-nm${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX nm Program")
SET(CMAKE_OBJCOPY      "nto${CMAKE_SYSTEM_PROCESSOR}-objcopy${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX objcopy Program")
SET(CMAKE_OBJDUMP      "nto${CMAKE_SYSTEM_PROCESSOR}-objdump${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX objdump Program")
SET(CMAKE_LINKER       "nto${CMAKE_SYSTEM_PROCESSOR}-ld"     CACHE PATH "QNX Linker Program")
SET(CMAKE_STRIP        "nto${CMAKE_SYSTEM_PROCESSOR}-strip${HOST_EXECUTABLE_SUFFIX}"   CACHE PATH "QNX Strip Program")

SET(CMAKE_C_COMPILER qcc -Vgcc_ntoarmv7le)
SET(CMAKE_C_FLAGS_DEBUG "-g -x c++ -fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG -x c++ -fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -x c++ -fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -x c++ -fPIC -fstack-protector -fstack-protector-all")

SET(CMAKE_CXX_COMPILER QCC -Vgcc_ntoarmv7le)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -x c++ -fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -x c++ -fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -x c++ -fPIC -fstack-protector -fstack-protector-all")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -x c++ -fPIC -fstack-protector -fstack-protector-all")
