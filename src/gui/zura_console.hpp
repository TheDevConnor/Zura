#ifndef ZURA_CONSOLE_H
#define ZURA_CONSOLE_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

typedef struct ZuraWindow ZuraWindow;
typedef struct ZuraConsole ZuraConsole;

int zura_gui_main(int argc, char* argv[]);

ZuraWindow* create_zura_window(void);
void        close_zura_window(ZuraWindow* zurawindow);
void        zura_render(const ZuraWindow* zurawindow);
void        draw_zura_console(ZuraWindow* zurawindow);
void        cleanup_glfw(GLFWwindow* window);
void        cleanup_imgui(void);
void        start_imgui_frame(void);

// Portable helpers
int   zc_stricmp(const char* s1, const char* s2);
int   zc_strnicmp(const char* s1, const char* s2, int n);
char* zc_strdup(const char* s);
void  zc_strtrim(char* s);

void         close_ZuraConsole(ZuraConsole* zc);
ZuraConsole* init_ZuraConsole();

void zc_ClearLog(ZuraConsole* zc);
void zc_AddLog(ZuraConsole* zc, const char* fmt, ...);
void zc_Draw(ZuraConsole* zc, const char* title, ZuraWindow* zurawindow, bool* p_open);
void zc_ExecCommand(ZuraConsole* zc, const char* command_line);
int  zc_TextEditCallbackStub(ImGuiInputTextCallbackData* data);
int  zc_TextEditCallback(ZuraConsole* zc, ImGuiInputTextCallbackData* data);

#endif //  ZURA_CONSOLE_H