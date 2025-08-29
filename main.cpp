#include <SDL3/SDL_main.h>
#include "engine.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace
{
    Engine engine;
}

#ifdef __EMSCRIPTEN__
void mainloop()
{
    engine.tick();
    if (!engine.m_running) {
        emscripten_cancel_main_loop();
        engine.deinit();
        SDL_Quit();
    }
}
#endif

int main(int argc, char** argv)
{
#ifdef __EMSCRIPTEN__
    engine.init();
    emscripten_set_main_loop(mainloop, 0, 0);
#else
    engine.run();
    SDL_Quit();
#endif
    
    return 0;
}
