cmake_minimum_required(VERSION 3.22)
project(ImGuiTest)
set(CMAKE_CXX_STANDARD 20)
file (GLOB SOURCES "./*.cpp" )
add_executable(${PROJECT_NAME} ${SOURCES} )
 
option(MAKEGUI "build  as a GUI rather then console app" ON)


 
if(MAKEGUI)
  set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  target_compile_definitions(${PROJECT_NAME} PUBLIC GUI)
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC CONSOLE)
endif(MAKEGUI)

unset(MAKEGUI CACHE) # <---- this is important

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE  
  /options:strict # unrecognized compiler options are an error
  /nologo #suppress copyright message    
  #/GA #optimize for Windows Application
  #/O1 # maximum optimizations (favor space)
  #/Os #  favor code space
  /favor:INTEL64 #  favor code space
  #/analyze #Enable native analysis      
  #/external:env:<var>      # location of external headers
  /external:anglebrackets # treat all headers included via <> as external
  /external:I "C://Program Files//Microsoft Visual Studio//2022//Community" #All recursive subdirectories of path are considered external
  #/Wall #enable all warnings
  /external:W0
  )
  endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ImGui)
file (GLOB IMGUISOURCES "./ImGui/*.cpp" )
add_library(ImGui_s STATIC  ${IMGUISOURCES}   )
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui_s)
target_link_libraries(${PROJECT_NAME} PUBLIC D3D11 DXGI dxguid d3dcompiler )
target_precompile_headers(${PROJECT_NAME} PUBLIC pch.hpp)
 