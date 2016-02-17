project(modelGen)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g" )

add_executable(modelgen main.cpp modelgen.cpp)
