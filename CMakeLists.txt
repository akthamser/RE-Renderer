cmake_minimum_required(VERSION 3.15)
project(Re_Renderer VERSION 1.0 LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Grab Engine Source Files (Notice the path now points inside Re-Renderer/src)
file(GLOB_RECURSE ENGINE_FILES 
    "Re-Renderer/src/*.cpp" 
    "Re-Renderer/src/*.h" 
    "Re-Renderer/src/*.hpp"
)

# 2. Grab Dependency Source Files (Glad, ImGui, stb_image)
# Note: If your Dependencies folder contains the ENTIRE Assimp source code 
# (thousands of files), let me know, because we shouldn't glob Assimp's source.
file(GLOB_RECURSE DEPENDENCY_FILES 
    "Dependencies/*.cpp" 
    "Dependencies/*.c" 
    "Dependencies/*.h" 
    "Dependencies/*.hpp"
)

# 3. Create your executable
add_executable(${PROJECT_NAME} ${ENGINE_FILES} ${DEPENDENCY_FILES})

# 4. Tell CMake where to look for #include statements
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Re-Renderer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies
    # It's common for ImGui and others to need specific include paths:
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/imGui
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include
)

# 5. Link required graphics libraries
find_package(OpenGL REQUIRED)

# 6. Pre-compiled Libraries (.lib files)
# Because you are using Assimp and likely GLFW, you probably have .lib files 
# inside a folder like Dependencies/lib. You need to tell CMake where they are:
target_link_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib # Adjust this if your .lib files are elsewhere
)

# Link everything together
target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL 
    # Uncomment and adjust these based on the exact names of your .lib files:
    # glfw3.lib
    # assimp-vc143-mt.lib 
)