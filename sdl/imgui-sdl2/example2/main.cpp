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
    static bool cb_last_value[16] = { false };

    static int  cs=0; // current step
    
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


    static const char * value_zero_sixteen[] =
      {
       "01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16"
      };
    static const char * value_zero_sixteen_s[] =
      {
       "[01]","[02]","[03]","[04]","[05]","[06]","[07]","[08]","[09]","[10]","[11]","[12]","[13]","[14]","[15]","[16"
      };

    
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

	    // press up                   it   activate A
	    // press up   a second time,  it deactivate A
	    if (event.type == SDL_KEYDOWN)
	      {
		if (event.key.keysym.sym == SDLK_UP)
		  {
		    if (cb_inst_value[cs]==0)
		      {
			cb_value[cs]=!cb_value[cs];
			cb_inst_value[cs]=1;
		      }
		  }
		if (event.key.keysym.sym == SDLK_LEFT) { cs--; if (cs<0) cs=15;  }
	      }
	    
	    if (event.type == SDL_KEYUP)
	      {
		if (event.key.keysym.sym == SDLK_UP)
		  {
		    if (cb_inst_value[cs]==1)
		      {
			cb_inst_value[cs]=0;
		      }
		  }
		if (event.key.keysym.sym == SDLK_RIGHT) { cs++; if (cs>15) cs=0;  }
	      }


	    
        }
        ImGui_ImplSdl_NewFrame(window);

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
	  ImGui::Begin("First Window", &show_first_window,window_flags);

	  ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");
	  ImGui::Text("instant value : %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		      cb_inst_value[0],  cb_inst_value[1],  cb_inst_value[2],  cb_inst_value[3],
		      cb_inst_value[4],  cb_inst_value[5],  cb_inst_value[6],  cb_inst_value[7],
		      cb_inst_value[8],  cb_inst_value[9],  cb_inst_value[10], cb_inst_value[11],
		      cb_inst_value[12], cb_inst_value[13], cb_inst_value[14], cb_inst_value[15]);

	  ImGui::Text("last value : %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		      cb_last_value[0],  cb_last_value[1],  cb_last_value[2],  cb_last_value[3],
		      cb_last_value[4],  cb_last_value[5],  cb_last_value[6],  cb_last_value[7],
		      cb_last_value[8],  cb_last_value[9],  cb_last_value[10], cb_last_value[11],
		      cb_last_value[12], cb_last_value[13], cb_last_value[14], cb_last_value[15]);

	  ImGui::Text("last value : %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		      cb_value[0],  cb_value[1],  cb_value[2],  cb_value[3],
		      cb_value[4],  cb_value[5],  cb_value[6],  cb_value[7],
		      cb_value[8],  cb_value[9],  cb_value[10], cb_value[11],
		      cb_value[12], cb_value[13], cb_value[14], cb_value[15]);


	  ImGui::Text("cs : %d ",cs);

	  for (int i=0;i<8;i++)
	    {
	      static const char * txt;
	      if (cs==i)
		txt=value_zero_sixteen_s[i];
	      else
		txt=value_zero_sixteen[i];

	      cb_last_value[i] =ImGui::Checkbox(txt, &cb_value[i]);
	      if (i<7) ImGui::SameLine((i+1)*70);
	    }
	  for (int i=8;i<16;i++)
	    {
	      static const char * txt;
	      if (cs==i)
		txt=value_zero_sixteen_s[i];
	      else
		txt=value_zero_sixteen[i];
	      
	      cb_last_value[i] =ImGui::Checkbox(txt, &cb_value[i]);
	      if (i<15) ImGui::SameLine((i-8+1)*70);
	    }

	  /*
	  cb_last_value[0] =ImGui::Checkbox("01", &cb_value[0]);   ImGui::SameLine(50);
	  cb_last_value[1] =ImGui::Checkbox("02", &cb_value[1]);   ImGui::SameLine(100);
	  cb_last_value[2] =ImGui::Checkbox("03", &cb_value[2]); ImGui::SameLine(150);
	  cb_last_value[3] =ImGui::Checkbox("04", &cb_value[3]);   ImGui::SameLine(200);
	  cb_last_value[4] =ImGui::Checkbox("05", &cb_value[4]);   ImGui::SameLine(250);
	  cb_last_value[5] =ImGui::Checkbox("06", &cb_value[5]);   ImGui::SameLine(300);
	  cb_last_value[6] =ImGui::Checkbox("07", &cb_value[6]);   ImGui::SameLine(350);
	  cb_last_value[7] =ImGui::Checkbox("08", &cb_value[7]);  
	  
	  cb_last_value[8] =ImGui::Checkbox("09", &cb_value[8]);   ImGui::SameLine(50);
	  cb_last_value[9] =ImGui::Checkbox("10", &cb_value[9]);   ImGui::SameLine(100);
	  cb_last_value[10]=ImGui::Checkbox("11", &cb_value[10]);  ImGui::SameLine(150);
	  cb_last_value[11]=ImGui::Checkbox("12", &cb_value[11]);  ImGui::SameLine(200);
	  cb_last_value[12]=ImGui::Checkbox("13", &cb_value[12]);  ImGui::SameLine(250);
	  cb_last_value[13]=ImGui::Checkbox("14", &cb_value[13]);  ImGui::SameLine(300);
	  cb_last_value[14]=ImGui::Checkbox("15", &cb_value[14]);  ImGui::SameLine(350);
	  cb_last_value[15]=ImGui::Checkbox("16", &cb_value[15]);
	  */
	  
	  ImGui::End();
	  
	  // int i;	  
	  // for (i=0;i<16;i++)
	  //   cb_value[i]=cb_last_value[i];
	  
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
