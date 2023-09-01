// Including necessary C standard libraries
#include <ctype.h>
#include <stdio.h>  // Standard input/output functions
#include <stdlib.h> // Standard library functions, e.g., memory allocation
#include <string.h> // String manipulation functions

// Including language headers
#include "common.h"
#include "debug/debug.h"  // Debugging utilities
#include "helper/flags.h" // Command-line flags parsing
#include "parser/chunk.h" // Chunk data structure and parsing functions
#include "vm/vm.h"        // Virtual machine implementation

#if ZURA_GUI

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

    #include "gui/zura_console.h"

#endif

int main(int argc, char* argv[])
{

    // Init Zura
    flags(argc, argv);
    init_vm();

    ZuraWindow* zurawindow = create_zura_window();

    while (!glfwWindowShouldClose(zurawindow->window)) {

        glfwPollEvents();
        start_imgui_frame();

#if IMGUI_DEMO_WINDOW
        ImGui::ShowDemoWindow(&imgui_show_window);
#endif

        bool show_zura_console = true;
        draw_zura_console(zurawindow);

        // Rendering
        zura_render(zurawindow);
    }

    // Run Zura
    run_file(argv[1]);

    // Cleanup Zura
    free_vm();
    cleanup_imgui();
    cleanup_glfw(zurawindow->window);
    zurawindow->window = NULL;

    ZuraExit(OK);
}
