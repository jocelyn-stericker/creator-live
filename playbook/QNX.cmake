INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME QNX)

# specify the cross compiler
CMAKE_FORCE_C_COMPILER(qcc GNU)
CMAKE_FORCE_CXX_COMPILER(QCC GNU)

SET(CMAKE_C_FLAGS "-fPIC -fstack-protector -fstack-protector-all -Vgcc_ntoarmv7le")
SET(CMAKE_CXX_FLAGS "-fPIC -fstack-protector -fstack-protector-all -Vgcc_ntoarmv7le")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /hddhome/joshua/bbndk-2.0.1/target/qnx6/ /usr/local/Trolltech/QtLighthouse-4.8.2-arm)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(QNX TRUE)
