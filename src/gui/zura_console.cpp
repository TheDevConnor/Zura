// #include <ctype.h>
// #include <stdio.h>
// #include <stdlib.h>

// #include <GLFW/glfw3.h>

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

// #include "../common.hpp"
// #include "../debug/debug.h"  // Debugging utilities
// #include "../helper/flags.h" // Command-line flags parsing
// #include "../parser/chunk.h" // Chunk data structure and parsing functions
// #include "../vm/vm.h"        // Virtual machine implementation

// #include "zura_console.hpp"

// // TODO Remove the use of these default values, deferring
// // to using gl or glfw to get the actual pixel dimensions.
// #define SCREEN_WIDTH  400
// #define SCREEN_HEIGHT 600

// // GL 3.0 + GLSL 130
// #define ZURA_GLSL_VERSION "#version 130"

// // ImGui::ShowDemoWindow() is our reference on how to use ImGui features.
// #if IMGUI_DEMO_WINDOW
// bool imgui_show_window = true;
// #else
// bool imgui_show_window = false;
// #endif

// struct ZuraConsole {

//     char                  InputBuf[256];
//     ImVector<char*>       Items;
//     ImVector<const char*> Commands;
//     ImVector<char*>       History;
//     int                   HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
//     ImGuiTextFilter       Filter;
//     bool                  AutoScroll;
//     bool                  ScrollToBottom;

//     int (*pfn_text_edit_callback)(ZuraConsole*, ImGuiInputTextCallbackData*);
// };

// struct ZuraWindow {

//     GLFWwindow* window;

//     // Physical Monitor, pixels
//     int monitor_w;
//     int monitor_h;

//     // Application Window, pixels
//     int window_w;
//     int window_h;

//     ImVec4 clear_color; // four floats
// };

// int zura_gui_main(int argc, char* argv[])
// {
//     // Init Zura
//     flags(argc, argv);
//     init_vm();

//     ZuraWindow* zurawindow = create_zura_window();

//     while (!glfwWindowShouldClose(zurawindow->window)) {

//         glfwPollEvents();
//         start_imgui_frame();

//         // Current window takes priority over the demo window.
//         if(imgui_show_window){
//             ImGui::ShowDemoWindow(&imgui_show_window);
//         }

//         draw_zura_console(zurawindow);

//         // Rendering
//         zura_render(zurawindow);
//     }

//     // Run Zura
//     run_file(argv[1]);

//     // Cleanup Zura
//     free_vm();
//     cleanup_imgui();
//     close_zura_window(zurawindow);
//     zurawindow->window = NULL;
//     free(zurawindow);
//     zurawindow = NULL;

//     return int(OK);
// }

// ZuraConsole* init_ZuraConsole()
// {
//     ZuraConsole* zc = (ZuraConsole*)malloc(sizeof(ZuraConsole));
//     if (!zc) {
//         fprintf(stderr, "ERROR: Unable to allocate memory in %s.", __func__);
//         ZuraExit(UNABLE_TO_INIT_GUI);
//     }
//     zc_ClearLog(zc);
//     memset(zc->InputBuf, 0, sizeof(zc->InputBuf));

//     zc->pfn_text_edit_callback = &zc_TextEditCallback;
//     zc->HistoryPos             = -1;

//     // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to
//     // "CL" and display multiple matches.
//     zc->Commands.push_back("HELP");
//     zc->Commands.push_back("HISTORY");
//     zc->Commands.push_back("CLEAR");
//     zc->Commands.push_back("CLASSIFY");
//     zc->AutoScroll     = true;
//     zc->ScrollToBottom = false;
//     zc_AddLog(zc, "Welcome to Dear ImGui!");

//     return zc;
// }

// void close_ZuraConsole(ZuraConsole* zc)
// {
//     zc_ClearLog(zc);
//     for (int i = 0; i < zc->History.Size; i++)
//         free(zc->History[i]);
// }

// void zc_ClearLog(ZuraConsole* zc)
// {
//     for (int i = 0; i < zc->Items.Size; i++) {
//         free(zc->Items[i]);
//     }
//     zc->Items.clear();
// }

// void zc_AddLog(ZuraConsole* zc, const char* fmt, ...) //  IM_FMTARGS(2)
// {

//     // FIXME-OPT
//     char    buf[1024];
//     va_list args;
//     va_start(args, fmt);
//     vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
//     buf[IM_ARRAYSIZE(buf) - 1] = 0;
//     va_end(args);
//     zc->Items.push_back(zc_strdup(buf));
// }

// void zc_Draw(ZuraConsole* zc, ZuraWindow* zw, const char* title, bool* p_open)
// {
//     int display_w, display_h;
//     // TODO Get window size from external variable that is updated in the main loop
//     glfwGetFramebufferSize(zw->window, &display_w, &display_h);
//     ImVec2 window_dims = ImVec2((float)display_w, (float)display_h);
//     ImVec2 window_pos  = ImVec2(0, 0);

//     ImGui::SetNextWindowFocus();
//     ImGui::SetWindowPos(title, window_pos, 0);   // set named window position.
//     ImGui::SetWindowSize(title, window_dims, 0); // set named window size. set axis to 0.0f to
//                                                  // force an auto-fit on this axis.

//     if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
//         ImGui::End();
//         ZuraExit(FAILED_TO_START_IMGUI);
//     }

//     // As a specific feature guaranteed by the library, after calling Begin()
//     // the last Item represent the title bar. So e.g. IsItemHovered() will
//     // return true when hovering the title bar. Here we create a context menu
//     // only available from the title bar.
//     if (ImGui::BeginPopupContextItem()) {
//         if (ImGui::MenuItem("Close Console")) {
//             *p_open = false;
//         }
//         ImGui::EndPopup();
//     }

//     ImGui::TextWrapped(
//         "This example implements a console with basic coloring, completion "
//         "(TAB key) and history (Up/Down keys). A more elaborate "
//         "implementation may want to store entries along with extra data such "
//         "as timestamp, emitter, etc.");
//     ImGui::TextWrapped("Enter 'HELP' for help.");

//     // TODO: display items starting from the bottom

//     if (ImGui::SmallButton("Add Debug Text")) {
//         zc_AddLog(zc, "%d some text", zc->Items.Size);
//         zc_AddLog(zc, "some more text");
//         zc_AddLog(zc, "display very important message here!");
//     }
//     ImGui::SameLine();
//     if (ImGui::SmallButton("Add Debug Error")) {
//         zc_AddLog(zc, "[error] something went wrong");
//     }
//     ImGui::SameLine();
//     if (ImGui::SmallButton("Clear")) {
//         zc_ClearLog(zc);
//     }
//     ImGui::SameLine();
//     bool copy_to_clipboard = ImGui::SmallButton("Copy");
//     // static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t =
//     // ImGui::GetTime(); AddLog("Spam %f", t); }

//     ImGui::Separator();

//     // Options menu
//     if (ImGui::BeginPopup("Options")) {
//         ImGui::Checkbox("Auto-scroll", &zc->AutoScroll);
//         ImGui::EndPopup();
//     }

//     // Options, Filter
//     if (ImGui::Button("Options")) {
//         ImGui::OpenPopup("Options");
//     }
//     ImGui::SameLine();
//     zc->Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
//     ImGui::Separator();

//     // Reserve enough left-over height for 1 separator + 1 input text
//     const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
//     if (ImGui::BeginChild("ScrollingRegion",
//             ImVec2(0, -footer_height_to_reserve), false,
//             ImGuiWindowFlags_HorizontalScrollbar)) {
//         if (ImGui::BeginPopupContextWindow()) {
//             if (ImGui::Selectable("Clear")) {
//                 zc_ClearLog(zc);
//             }
//             ImGui::EndPopup();
//         }

//         // Display every line as a separate entry so we can change their color or
//         // add custom widgets. If you only want raw text you can use
//         // ImGui::TextUnformatted(log.begin(), log.end()); NB- if you have
//         // thousands of entries this approach may be too inefficient and may
//         // require user-side clipping to only process visible items. The clipper
//         // will automatically measure the height of your first item and then
//         // "seek" to display only items in the visible area.
//         // To use the clipper we can replace your standard loop:
//         //      for (int i = 0; i < Items.Size; i++)
//         //   With:
//         //      ImGuiListClipper clipper;
//         //      clipper.Begin(Items.Size);
//         //      while (clipper.Step())
//         //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//         // - That your items are evenly spaced (same height)
//         // - That you have cheap random access to your elements (you can access
//         // them given their index,
//         //   without processing all the ones before)
//         // You cannot this code as-is if a filter is active because it breaks the
//         // 'cheap random-access' property. We would need random-access on the
//         // post-filtered list. A typical application wanting coarse clipping and
//         // filtering may want to pre-compute an array of indices or offsets of
//         // items that passed the filtering test, recomputing this array when user
//         // changes the filter, and appending newly elements as they are inserted.
//         // This is left as a task to the user until we can manage to improve this
//         // example code! If your items are of variable height:
//         // - Split them into same height items would be simpler and facilitate
//         // random-seeking into your list.
//         // - Consider using manual call to IsRectVisible() and skipping extraneous
//         // decoration from your items.
//         ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
//         if (copy_to_clipboard) {
//             ImGui::LogToClipboard();
//         }
//         for (int i = 0; i < zc->Items.Size; i++) {
//             const char* item = zc->Items[i];
//             if (!zc->Filter.PassFilter(item)) {
//                 continue;
//             }

//             // Normally you would store more information in your item than just a
//             // string. (e.g. make Items[] an array of structure, store color/type
//             // etc.)
//             ImVec4 color;
//             bool   has_color = false;
//             if (strstr(item, "[error]")) {
//                 color     = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
//                 has_color = true;
//             } else if (strncmp(item, "# ", 2) == 0) {
//                 color     = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
//                 has_color = true;
//             }
//             if (has_color) {
//                 ImGui::PushStyleColor(ImGuiCol_Text, color);
//             }
//             ImGui::TextUnformatted(item);
//             if (has_color) {
//                 ImGui::PopStyleColor();
//             }
//         }
//         if (copy_to_clipboard) {
//             ImGui::LogFinish();
//         }

//         // Keep up at the bottom of the scroll region if we were already at the
//         // bottom at the beginning of the frame. Using a scrollbar or mouse-wheel
//         // will take away from the bottom edge.
//         if (zc->ScrollToBottom || (zc->AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
//             ImGui::SetScrollHereY(1.0f);
//         }
//         zc->ScrollToBottom = false;

//         ImGui::PopStyleVar();
//     }
//     ImGui::EndChild();
//     ImGui::Separator();

//     // Command-line
//     bool reclaim_focus = false;

//     ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

//     // Arg #5 must be a fnptr to this signature
//     // ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
//     bool text_input_success = ImGui::InputText("Input", zc->InputBuf, IM_ARRAYSIZE(zc->InputBuf),
//         input_text_flags, &zc_TextEditCallbackStub, (void*)zc);

//     if (text_input_success) {

//         char* s = zc->InputBuf;
//         zc_strtrim(s);

//         if (s[0]) {
//             zc_ExecCommand(zc, s);
//         }

//         strcpy(s, "");
//         reclaim_focus = true;
//     }

//     // Auto-focus on window apparition
//     ImGui::SetItemDefaultFocus();
//     if (reclaim_focus) {
//         ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
//     }

//     ImGui::End();
// }

// // In C++11 you'd be better off using lambdas for this sort of forwarding
// // callbacks
// int zc_TextEditCallbackStub(ImGuiInputTextCallbackData* data)
// {
//     ZuraConsole* console = (ZuraConsole*)data->UserData;
//     return console->pfn_text_edit_callback(console, data);
// }

// int zc_TextEditCallback(ZuraConsole* zc, ImGuiInputTextCallbackData* data)
// {
//     // AddLog("cursor: %d, selection: %d-%d", data->CursorPos,
//     // data->SelectionStart, data->SelectionEnd);
//     switch (data->EventFlag) {
//     case ImGuiInputTextFlags_CallbackCompletion: {
//         // Example of TEXT COMPLETION

//         // Locate beginning of current word
//         const char* word_end   = data->Buf + data->CursorPos;
//         const char* word_start = word_end;
//         while (word_start > data->Buf) {
//             const char c = word_start[-1];
//             if (c == ' ' || c == '\t' || c == ',' || c == ';')
//                 break;
//             word_start--;
//         }

//         // Build a list of candidates
//         ImVector<const char*> candidates;
//         for (int i = 0; i < zc->Commands.Size; i++)
//             if (zc_strnicmp(zc->Commands[i], word_start, (int)(word_end - word_start)) == 0)
//                 candidates.push_back(zc->Commands[i]);

//         if (candidates.Size == 0) {
//             // No match
//             zc_AddLog(zc, "No match for \"%.*s\"!\n", (int)(word_end - word_start),
//                 word_start);
//         } else if (candidates.Size == 1) {
//             // Single match. Delete the beginning of the word and replace it
//             // entirely so we've got nice casing.
//             data->DeleteChars((int)(word_start - data->Buf),
//                 (int)(word_end - word_start));
//             data->InsertChars(data->CursorPos, candidates[0]);
//             data->InsertChars(data->CursorPos, " ");
//         } else {
//             // Multiple matches. Complete as much as we can..
//             // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and
//             // "CLASSIFY" as matches.
//             int match_len = (int)(word_end - word_start);
//             for (;;) {
//                 int  c                      = 0;
//                 bool all_candidates_matches = true;
//                 for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
//                     if (i == 0)
//                         c = toupper(candidates[i][match_len]);
//                     else if (c == 0 || c != toupper(candidates[i][match_len]))
//                         all_candidates_matches = false;
//                 if (!all_candidates_matches)
//                     break;
//                 match_len++;
//             }

//             if (match_len > 0) {
//                 data->DeleteChars((int)(word_start - data->Buf),
//                     (int)(word_end - word_start));
//                 data->InsertChars(data->CursorPos, candidates[0],
//                     candidates[0] + match_len);
//             }

//             // List matches
//             zc_AddLog(zc, "Possible matches:\n");
//             for (int i = 0; i < candidates.Size; i++)
//                 zc_AddLog(zc, "- %s\n", candidates[i]);
//         }

//         break;
//     }
//     case ImGuiInputTextFlags_CallbackHistory: {
//         // Example of HISTORY
//         const int prev_history_pos = zc->HistoryPos;
//         if (data->EventKey == ImGuiKey_UpArrow) {
//             if (zc->HistoryPos == -1)
//                 zc->HistoryPos = zc->History.Size - 1;
//             else if (zc->HistoryPos > 0)
//                 zc->HistoryPos--;
//         } else if (data->EventKey == ImGuiKey_DownArrow) {
//             if (zc->HistoryPos != -1)
//                 if (++zc->HistoryPos >= zc->History.Size)
//                     zc->HistoryPos = -1;
//         }

//         // A better implementation would preserve the data on the current input
//         // line along with cursor position.
//         if (prev_history_pos != zc->HistoryPos) {
//             const char* history_str = (zc->HistoryPos >= 0) ? zc->History[zc->HistoryPos] : "";
//             data->DeleteChars(0, data->BufTextLen);
//             data->InsertChars(0, history_str);
//         }
//     }
//     }
//     return 0;
// }

// void zc_ExecCommand(ZuraConsole* zc, const char* command_line)
// {
//     zc_AddLog(zc, "# %s\n", command_line);

//     // Insert into history. First find match and delete it so it can be pushed
//     // to the back. This isn't trying to be smart or optimal.
//     zc->HistoryPos = -1;
//     for (int i = zc->History.Size - 1; i >= 0; i--)
//         if (zc_stricmp(zc->History[i], command_line) == 0) {
//             free(zc->History[i]);
//             zc->History.erase(zc->History.begin() + i);
//             break;
//         }
//     zc->History.push_back(zc_strdup(command_line));

//     // Process command
//     if (zc_stricmp(command_line, "CLEAR") == 0) {
//         zc_ClearLog(zc);
//     } else if (zc_stricmp(command_line, "HELP") == 0) {
//         zc_AddLog(zc, "Commands:");
//         for (int i = 0; i < zc->Commands.Size; i++)
//             zc_AddLog(zc, "- %s", zc->Commands[i]);
//     } else if (zc_stricmp(command_line, "HISTORY") == 0) {
//         int first = zc->History.Size - 10;
//         for (int i = first > 0 ? first : 0; i < zc->History.Size; i++)
//             zc_AddLog(zc, "%3d: %s\n", i, zc->History[i]);
//     } else {
//         zc_AddLog(zc, "Unknown command: '%s'\n", command_line);
//     }

//     // On command input, we scroll to bottom even if AutoScroll==false
//     zc->ScrollToBottom = true;
// }

// int zc_stricmp(const char* s1, const char* s2)
// {
//     int d;
//     while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
//         s1++;
//         s2++;
//     }
//     return d;
// }
// int zc_strnicmp(const char* s1, const char* s2, int n)
// {
//     int d = 0;
//     while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
//         s1++;
//         s2++;
//         n--;
//     }
//     return d;
// }
// char* zc_strdup(const char* s)
// {
//     IM_ASSERT(s);
//     size_t len = strlen(s) + 1;
//     void*  buf = malloc(len);
//     IM_ASSERT(buf);
//     return (char*)memcpy(buf, (const void*)s, len);
// }
// void zc_strtrim(char* s)
// {
//     char* str_end = s + strlen(s);
//     while (str_end > s && str_end[-1] == ' ')
//         str_end--;
//     *str_end = 0;
// }

// void glfw_error_callback(int error, const char* description)
// {
//     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
// }

// void draw_zura_console(ZuraWindow* zurawindow)
// {
//     bool               p_open;
//     static ZuraConsole console;
//     zc_Draw(&console, zurawindow, "Zura Console", &p_open);
// }

// ZuraWindow* create_zura_window(void)
// {
//     ZuraWindow* zw = (ZuraWindow*)malloc(sizeof(ZuraWindow));

//     if (!zw) {
//         fprintf(stderr, "ERROR: Unable to allocate memory in %s.", __func__);
//         ZuraExit(UNABLE_TO_INIT_GUI);
//     }

//     *zw = ZuraWindow {};

//     glfwSetErrorCallback(glfw_error_callback);
//     if (!glfwInit()) {
//         fprintf(stderr, "ERROR: Unable to initialize GLFW.\n");
//         ZuraExit(UNABLE_TO_INIT_GUI);
//     }

//     // GL 3.0 + GLSL 130
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

//     // Create window with graphics context
//     zw->window = glfwCreateWindow(1280, 720, "Zura Console", nullptr, nullptr);

//     if (!zw->window) {
//         fprintf(stderr, "ERROR: GLFW unable to create a window context.\n");
//         ZuraExit(UNABLE_TO_INIT_GUI);
//     }

//     glfwMakeContextCurrent(zw->window);
//     glfwSwapInterval(1); // Enable vsync

//     // Setup Dear ImGui context
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO();
//     (void)io;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

//     // Dear ImGui style
//     ImGui::StyleColorsDark();
//     // ImGui::StyleColorsLight();

//     zw->clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

//     // Setup Platform/Renderer backends
//     ImGui_ImplGlfw_InitForOpenGL(zw->window, true);
//     // GL 3.0 + GLSL 130
//     ImGui_ImplOpenGL3_Init(ZURA_GLSL_VERSION);

//     return zw;
// }

// void zura_render(const ZuraWindow* zurawindow)
// {
//     ImGui::Render();
//     int display_w, display_h;
//     glfwGetFramebufferSize(zurawindow->window, &display_w, &display_h);
//     glViewport(0, 0, display_w, display_h);

//     glClearColor(zurawindow->clear_color.x * zurawindow->clear_color.w,
//         zurawindow->clear_color.y * zurawindow->clear_color.w,
//         zurawindow->clear_color.z * zurawindow->clear_color.w,
//         zurawindow->clear_color.w);

//     glClear(GL_COLOR_BUFFER_BIT);
//     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//     glfwSwapBuffers(zurawindow->window);
// }

// void cleanup_imgui(void)
// {
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();
// }

// void start_imgui_frame(void)
// {
//     ImGui_ImplOpenGL3_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
//     ImGui::NewFrame();
// }

// void close_zura_window(ZuraWindow* zw)
// {
//     glfwDestroyWindow(zw->window);
//     glfwTerminate();
// }