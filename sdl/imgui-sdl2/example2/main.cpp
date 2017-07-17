// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL example",
					  SDL_WINDOWPOS_CENTERED,
					  SDL_WINDOWPOS_CENTERED,
					  1280,
					  720,
					  SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    // Setup ImGui binding
    ImGui_ImplSdl_Init(window);

    bool show_first_window = true;
    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    static bool cb_value[16] = { false };
    static bool cb_inst_value[16] = { false };

    static float wrap_width = 200.0f;
    
    static bool no_titlebar  = true;
    static bool no_border    = true;
    static bool no_resize    = true;
    static bool no_move      = true;
    static bool no_scrollbar = true;
    static bool no_collapse  = true;
    static bool no_menu      = true;
    ImGuiWindowFlags window_flags = 0;
    
    if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
    if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
    if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
    if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
    if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;


    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdl_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }
        ImGui_ImplSdl_NewFrame(window);

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
	  ImGui::Begin("First Window", &show_first_window,window_flags);

	  ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");
	  ImGui::Text("instant value : %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		      cb_inst_value[0], cb_inst_value[1], cb_inst_value[2], cb_inst_value[3],
		      cb_inst_value[4], cb_inst_value[5], cb_inst_value[6], cb_inst_value[7],
		      cb_inst_value[8], cb_inst_value[9], cb_inst_value[10], cb_inst_value[11],
		      cb_inst_value[12], cb_inst_value[13], cb_inst_value[14], cb_inst_value[15]);
		      
	  cb_inst_value[0] =ImGui::Checkbox("A", &cb_value[0]);   ImGui::SameLine(50);
	  cb_inst_value[1] =ImGui::Checkbox("B", &cb_value[1]);   ImGui::SameLine(100);
	  cb_inst_value[2] =ImGui::Checkbox("C", &cb_value[2]);   ImGui::SameLine(150);
	  cb_inst_value[3] =ImGui::Checkbox("D", &cb_value[3]);   ImGui::SameLine(200);
	  cb_inst_value[4] =ImGui::Checkbox("E", &cb_value[4]);   ImGui::SameLine(250);
	  cb_inst_value[5] =ImGui::Checkbox("F", &cb_value[5]);   ImGui::SameLine(300);
	  cb_inst_value[6] =ImGui::Checkbox("G", &cb_value[6]);   ImGui::SameLine(350);
	  cb_inst_value[7] =ImGui::Checkbox("H", &cb_value[7]);  

	  cb_inst_value[8] =ImGui::Checkbox("I", &cb_value[8]);   ImGui::SameLine(50);
	  cb_inst_value[9] =ImGui::Checkbox("J", &cb_value[9]);   ImGui::SameLine(100);
	  cb_inst_value[10]=ImGui::Checkbox("K", &cb_value[10]);  ImGui::SameLine(150);
	  cb_inst_value[11]=ImGui::Checkbox("L", &cb_value[11]);  ImGui::SameLine(200);
	  cb_inst_value[12]=ImGui::Checkbox("M", &cb_value[12]);  ImGui::SameLine(250);
	  cb_inst_value[13]=ImGui::Checkbox("N", &cb_value[13]);  ImGui::SameLine(300);
	  cb_inst_value[14]=ImGui::Checkbox("O", &cb_value[14]);  ImGui::SameLine(350);
	  cb_inst_value[15]=ImGui::Checkbox("P", &cb_value[15]);  

	  ImGui::End();
	  
        }
        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}