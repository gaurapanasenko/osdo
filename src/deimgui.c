#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include "deimgui.h"
#include "window.h"
#include "conf.h"

void deimgui_init(DeImgui* deimgui, Window *win) {
    deimgui->ctx = igCreateContext(NULL);
    deimgui->io  = igGetIO();
    ImGuiIO *io = deimgui->io;
    ImFontAtlas_AddFontFromFileTTF(io->Fonts, "FantasqueSansMono-Regular.ttf", 14, NULL, NULL);
    ImFontAtlas_AddFontDefault(io->Fonts, NULL);

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window_get(win), false);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    igStyleColorsDark(NULL);
}

void deimgui_del(DeImgui* deimgui) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(deimgui->ctx);
}


void deimgui_render(UNUSED DeImgui* deimgui) {
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void deimgui_update(UNUSED DeImgui* deimgui) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    //igShowDemoWindow(NULL);
}
