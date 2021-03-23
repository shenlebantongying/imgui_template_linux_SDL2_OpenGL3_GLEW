// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#include <SDL.h>
#include <glad/glad.h>



typedef float t_mat4x4[16];

static inline void mat4x4_ortho(t_mat4x4 out, float right, float bottom, float top, float znear, float zfar) {
#define T(a, b) (a * 4 + b)

  out[T(0,0)] = 2.0f / (right - 0.0);
  out[T(0,1)] = 0.0f;
  out[T(0,2)] = 0.0f;
  out[T(0,3)] = 0.0f;

  out[T(1,1)] = 2.0f / (top - bottom);
  out[T(1,0)] = 0.0f;
  out[T(1,2)] = 0.0f;
  out[T(1,3)] = 0.0f;

  out[T(2,2)] = -2.0f / (zfar - znear);
  out[T(2,0)] = 0.0f;
  out[T(2,1)] = 0.0f;
  out[T(2,3)] = 0.0f;

  out[T(3,0)] = -(right + 0.0) / (right - 0.0);
  out[T(3,1)] = -(top + bottom) / (top - bottom);
  out[T(3,2)] = -(zfar + znear) / (zfar - znear);
  out[T(3,3)] = 1.0f;

#undef T
}

static const char * vertex_shader =
    "#version 130\n"
    "in vec2 i_position;\n"
    "in vec4 i_color;\n"
    "out vec4 v_color;\n"
    "uniform mat4 u_projection_matrix;\n"
    "void main() {\n"
    "    v_color = i_color;\n"
    "    gl_Position = u_projection_matrix * vec4( i_position, 0.0, 1.0 );\n"
    "}\n";

static const char * fragment_shader =
    "#version 130\n"
    "in vec4 v_color;\n"
    "out vec4 o_color;\n"
    "void main() {\n"
    "    o_color = v_color;\n"
    "}\n";

typedef enum t_attrib_id
{
  attrib_position,
  attrib_color
} t_attrib_id;

// Main code
int main(int, char**)
{

  static const int width = 1280;
  static const int height = 720;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
    // ES 3.0 "#version 300 es"   = WebGL 2.0
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gladLoadGL() == 0;

    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImFont* font = io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != nullptr);

    // [Custom SDL codes]
    // ------------------------------------------------

  GLuint vs, fs, program;

  vs = glCreateShader( GL_VERTEX_SHADER );
  fs = glCreateShader( GL_FRAGMENT_SHADER );

  int length = strlen( vertex_shader );
  glShaderSource( vs, 1, ( const GLchar ** )&vertex_shader, &length );
  glCompileShader( vs );

  GLint status;
  glGetShaderiv( vs, GL_COMPILE_STATUS, &status );
  if( status == GL_FALSE )
  {
    fprintf( stderr, "vertex shader compilation failed\n" );
    return 1;
  }

  length = strlen( fragment_shader );
  glShaderSource( fs, 1, ( const GLchar ** )&fragment_shader, &length );
  glCompileShader( fs );

  glGetShaderiv( fs, GL_COMPILE_STATUS, &status );
  if( status == GL_FALSE )
  {
    fprintf( stderr, "fragment shader compilation failed\n" );
    return 1;
  }

  program = glCreateProgram();
  glAttachShader( program, vs );
  glAttachShader( program, fs );

  glBindAttribLocation( program, attrib_position, "i_position" );
  glBindAttribLocation( program, attrib_color, "i_color" );
  glLinkProgram( program );

  glUseProgram( program );

  glDisable( GL_DEPTH_TEST );
  glClearColor( 0.5, 0.0, 0.0, 0.0 );
  glViewport( 0, 0, width, height );

  GLuint vao, vbo;

  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glEnableVertexAttribArray( attrib_position );
  glEnableVertexAttribArray( attrib_color );

  glVertexAttribPointer( attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 6, nullptr );
  glVertexAttribPointer( attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 6, ( void * )(4 * sizeof(float)) );

  const GLfloat g_vertex_buffer_data[] = {
      /*  R, G, B, A, X, Y  */
      1, 0, 0, 1, 0, 0,
      0, 1, 0, 1, width, 0,
      0, 0, 1, 1, width, height,

      1, 0, 0, 1, 0, 0,
      0, 0, 1, 1, width, height,
      1, 1, 1, 1, 0, height
  };

  glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_STATIC_DRAW );

  t_mat4x4 projection_matrix;
  mat4x4_ortho(projection_matrix, (float) width, (float) height, 0.0f, 0.0f, 100.0f);
  glUniformMatrix4fv( glGetUniformLocation( program, "u_projection_matrix" ), 1, GL_FALSE, projection_matrix );

    // Our state
    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.1f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // Rendering
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glBindVertexArray( vao );
      glDrawArrays( GL_TRIANGLES, 0, 6 );
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
