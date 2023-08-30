
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

// Define ZURA_GUI with compiler invocation.
#if ZURA_GUI

    // TODO Remove the use of these default values, deferring
    // to using gl or glfw to get the actual pixel dimensions.
    #define SCREEN_WIDTH 400
    #define SCREEN_HEIGHT 600
   
    // GL 3.0 + GLSL 130
    #define ZURA_GLSL_VERSION "#version 130"

    #include <GLFW/glfw3.h>

    #include <imgui/imgui.h>
    #include <imgui/imgui_impl_glfw.h>
    #include <imgui/imgui_impl_opengl3.h>

typedef struct {

    GLFWwindow* window;

    // Physical Monitor, pixels
    int monitor_w; 
    int monitor_h;

    // Application Window, pixels
    int window_w;
    int window_h;

    // floats
    ImVec4 clear_color;

} ZuraWindow;

typedef struct ZuraConsole {
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ZuraConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to
        // "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll     = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear ImGui!");
    }

    ~ZuraConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int Stricmp(const char* s1, const char* s2)
    {
        int d;
        while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
            s1++;
            s2++;
        }
        return d;
    }
    static int Strnicmp(const char* s1, const char* s2, int n)
    {
        int d = 0;
        while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
            s1++;
            s2++;
            n--;
        }
        return d;
    }
    static char* Strdup(const char* s)
    {
        IM_ASSERT(s);
        size_t len = strlen(s) + 1;
        void*  buf = malloc(len);
        IM_ASSERT(buf);
        return (char*)memcpy(buf, (const void*)s, len);
    }
    static void Strtrim(char* s)
    {
        char* str_end = s + strlen(s);
        while (str_end > s && str_end[-1] == ' ')
            str_end--;
        *str_end = 0;
    }

    void ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
    }

    void AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char    buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void Draw(const char* title, ZuraWindow* zurawindow, bool* p_open)
    {
        int display_w, display_h;
        // TODO Get window size from external variable that is updated in the main loop
        glfwGetFramebufferSize(zurawindow->window, &display_w, &display_h);
        ImVec2 window_dims = ImVec2(display_w, display_h);
        ImVec2 window_pos = ImVec2(0,0);

        ImGui::SetNextWindowFocus();
        ImGui::SetWindowPos(title, window_pos, 0);      // set named window position.
        ImGui::SetWindowSize(title, window_dims, 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.

        if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)){
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin()
        // the last Item represent the title bar. So e.g. IsItemHovered() will
        // return true when hovering the title bar. Here we create a context menu
        // only available from the title bar.
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion "
            "(TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such "
            "as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Debug Text")) {
            AddLog("%d some text", Items.Size);
            AddLog("some more text");
            AddLog("display very important message here!");
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) {
            AddLog("[error] something went wrong");
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear")) {
            ClearLog();
        }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        // static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t =
        // ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options")) {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion",
                ImVec2(0, -footer_height_to_reserve), false,
                ImGuiWindowFlags_HorizontalScrollbar)) {
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::Selectable("Clear"))
                    ClearLog();
                ImGui::EndPopup();
            }

            // Display every line as a separate entry so we can change their color or
            // add custom widgets. If you only want raw text you can use
            // ImGui::TextUnformatted(log.begin(), log.end()); NB- if you have
            // thousands of entries this approach may be too inefficient and may
            // require user-side clipping to only process visible items. The clipper
            // will automatically measure the height of your first item and then
            // "seek" to display only items in the visible area.
            // To use the clipper we can replace your standard loop:
            //      for (int i = 0; i < Items.Size; i++)
            //   With:
            //      ImGuiListClipper clipper;
            //      clipper.Begin(Items.Size);
            //      while (clipper.Step())
            //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            // - That your items are evenly spaced (same height)
            // - That you have cheap random access to your elements (you can access
            // them given their index,
            //   without processing all the ones before)
            // You cannot this code as-is if a filter is active because it breaks the
            // 'cheap random-access' property. We would need random-access on the
            // post-filtered list. A typical application wanting coarse clipping and
            // filtering may want to pre-compute an array of indices or offsets of
            // items that passed the filtering test, recomputing this array when user
            // changes the filter, and appending newly elements as they are inserted.
            // This is left as a task to the user until we can manage to improve this
            // example code! If your items are of variable height:
            // - Split them into same height items would be simpler and facilitate
            // random-seeking into your list.
            // - Consider using manual call to IsRectVisible() and skipping extraneous
            // decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                ImVec2(4, 1)); // Tighten spacing
            if (copy_to_clipboard)
                ImGui::LogToClipboard();
            for (int i = 0; i < Items.Size; i++) {
                const char* item = Items[i];
                if (!Filter.PassFilter(item))
                    continue;

                // Normally you would store more information in your item than just a
                // string. (e.g. make Items[] an array of structure, store color/type
                // etc.)
                ImVec4 color;
                bool   has_color = false;
                if (strstr(item, "[error]")) {
                    color     = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    has_color = true;
                } else if (strncmp(item, "# ", 2) == 0) {
                    color     = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                    has_color = true;
                }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }
            if (copy_to_clipboard)
                ImGui::LogFinish();

            // Keep up at the bottom of the scroll region if we were already at the
            // bottom at the beginning of the frame. Using a scrollbar or mouse-wheel
            // will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool                reclaim_focus    = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this)) {

            char* s = InputBuf;
            Strtrim(s);

            if (s[0]) {
                ExecCommand(s);
            }

            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus){
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        }

        ImGui::End();
    }

    void ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed
        // to the back. This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0) {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0) {
            ClearLog();
        } else if (Stricmp(command_line, "HELP") == 0) {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        } else if (Stricmp(command_line, "HISTORY") == 0) {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        } else {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding
    // callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ZuraConsole* console = (ZuraConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        // AddLog("cursor: %d, selection: %d-%d", data->CursorPos,
        // data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion: {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end   = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf) {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0) {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start),
                    word_start);
            } else if (candidates.Size == 1) {
                // Single match. Delete the beginning of the word and replace it
                // entirely so we've got nice casing.
                data->DeleteChars((int)(word_start - data->Buf),
                    (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            } else {
                // Multiple matches. Complete as much as we can..
                // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and
                // "CLASSIFY" as matches.
                int match_len = (int)(word_end - word_start);
                for (;;) {
                    int  c                      = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0) {
                    data->DeleteChars((int)(word_start - data->Buf),
                        (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0],
                        candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            } else if (data->EventKey == ImGuiKey_DownArrow) {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input
            // line along with cursor position.
            if (prev_history_pos != HistoryPos) {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        }
        return 0;
    }
} ZuraConsole;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void ShowZuraConsole(ZuraWindow* zurawindow, bool* p_open)
{
    static ZuraConsole console;
    console.Draw("Zura Console", zurawindow, p_open);
}

#endif // ZURA_GUI

ZuraWindow init_zura_window(void)
{
    ZuraWindow zurawindow = {};

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Unable to initialize GLFW.\n");
        ZuraExit(UNABLE_TO_INIT_GUI);
    }

    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    zurawindow.window = glfwCreateWindow(1280, 720, "Zura Console", nullptr, nullptr);

    if (zurawindow.window == nullptr) {
        fprintf(stderr, "ERROR: GLFW unable to create a window context.\n");
        ZuraExit(UNABLE_TO_INIT_GUI);
    }

    glfwMakeContextCurrent(zurawindow.window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    
    zurawindow.clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(zurawindow.window, true);
    // GL 3.0 + GLSL 130
    ImGui_ImplOpenGL3_Init(ZURA_GLSL_VERSION);



    return zurawindow;
}

void zura_render(const ZuraWindow* zurawindow)
{
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(zurawindow->window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(   zurawindow->clear_color.x * zurawindow->clear_color.w, 
                        zurawindow->clear_color.y * zurawindow->clear_color.w, 
                        zurawindow->clear_color.z * zurawindow->clear_color.w, 
                        zurawindow->clear_color.w
        );

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(zurawindow->window);
}

void cleanup_glfw(GLFWwindow* window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void cleanup_imgui(void)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}



int main(int argc, char* argv[])
{

    // Init Zura
    flags(argc, argv);
    init_vm();

    ZuraWindow zurawindow = init_zura_window();

    while (!glfwWindowShouldClose(zurawindow.window)) {

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

// TODO Decide to show this window in same as zura console or make
// a separate one.
// ImGui::ShowDemoWindow() is our reference on how to use ImGui features.
#if IMGUI_DEMO_WINDOW
        bool imgui_show_window = true;
        ImGui::ShowDemoWindow(&imgui_show_window);
#endif

        bool show_zura_console = true;
        ShowZuraConsole(&zurawindow, &show_zura_console);


        // Rendering
        zura_render(&zurawindow);

    }
    // Run Zura
    run_file(argv[1]);
        
    // Cleanup Zura
    free_vm();

    cleanup_imgui();
    cleanup_glfw(zurawindow.window);
    zurawindow.window = NULL;

    ZuraExit(OK);
}

