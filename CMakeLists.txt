cmake_minimum_required(VERSION 3.30.8)
project(codejam2025 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(codejam2025 main.cpp engine.cpp map.cpp render/renderer.cpp render/map_renderer.cpp game.cpp heightmap.cpp camera.cpp
                imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui.cpp imgui/imgui_impl_sdl3.cpp imgui/imgui_impl_sdlrenderer3.cpp) # imgui

#set(CMAKE_CXX_FLAGS "-fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

if(CMAKE_CXX_FLAGS EQUAL "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

target_include_directories(codejam2025 PRIVATE "./")

#set(LINK_OPTIONS "${CMAKE_CXX_FLAGS} -sEXPORTED_RUNTIME_METHODS=[ccall,UTF8ToString,stringToUTF8,lengthBytesUTF8]" "--shell-file ./shell_minimal.html")

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=3")
    #set(LINK_OPTIONS "${LINK_OPTIONS} -sEXPORTED_RUNTIME_METHODS=[ccall,UTF8ToString,stringToUTF8,lengthBytesUTF8] --shell-file ./shell_minimal.html")
    
    #set(EMMAKEN_CFLAGS "")
    set (CMAKE_EXECUTABLE_SUFFIX ".html")
    message("using emscripten")
else()
    find_package(SDL3 REQUIRED)
    target_link_libraries(codejam2025 PUBLIC SDL3::SDL3)    
    message("not using emscripten")
endif()


