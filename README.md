# imgur-workbench


# Tweaking

---

add this to `imgui_impl_opengl3.h` to force glad
```cpp
#include <glad/glad.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
```
---
As of 2021, SDL2's `SDL_Renderer` and `OpenGL` are mutually exclusive, thus if you wanna to use imgui, only OpenGL api are allowed.