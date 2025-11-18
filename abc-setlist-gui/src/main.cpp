#include "SetlistManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <cstdio>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <shlobj.h>
#endif

// Global state
setlistgui::SetlistManager g_setlistManager;
int g_paddingSeconds = 5;
int g_introSeconds = 10;
int g_draggedIndex = -1;
std::vector<std::string> g_droppedFiles;
char g_editingTitle[256] = "";
int g_editingIndex = -1;
char g_exportFolderPath[512] = "";
std::string g_exportMessage = "";
float g_exportMessageTimer = 0.0f;
bool g_addNumbering = true;  // Toggle for adding numbering to exported files
int g_editingInstrumentsSongIndex = -1;  // Which song's title lines are being edited
char g_editingTitleLine[512] = "";  // Buffer for editing full title line
int g_editingTitleLineIndex = -1;

// GLFW drop callback
void drop_callback(GLFWwindow* window, int count, const char** paths) {
    for (int i = 0; i < count; i++) {
        std::string path = paths[i];
        // Check if it's an .abc file
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".abc") {
            g_droppedFiles.push_back(path);
        }
    }
}

#ifdef _WIN32
// Windows folder browser dialog
std::string OpenFolderDialog(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);

    BROWSEINFOA bi = {};
    bi.hwndOwner = hwnd;
    bi.lpszTitle = "Select Export Folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

    if (pidl != nullptr) {
        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        CoTaskMemFree(pidl);
    }

    return "";
}
#else
// Placeholder for non-Windows platforms
std::string OpenFolderDialog(GLFWwindow* window) {
    return "";
}
#endif

// Render a song card
void RenderSongCard(size_t index, const setlistgui::SongCard& card) {
    ImGui::PushID(static_cast<int>(index));

    // Card background
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.17f, 1.0f));
    ImGui::BeginChild("card", ImVec2(-1, 120), true, ImGuiWindowFlags_NoScrollbar);

    // Drag source for reordering
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
        g_draggedIndex = static_cast<int>(index);
    }

    // Title (editable on double-click)
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.6f, 1.0f), "%d. ", static_cast<int>(index) + 1);
    ImGui::SameLine();

    if (g_editingIndex == static_cast<int>(index)) {
        ImGui::SetKeyboardFocusHere();
        if (ImGui::InputText("##title", g_editingTitle, sizeof(g_editingTitle), ImGuiInputTextFlags_EnterReturnsTrue)) {
            g_setlistManager.updateSongTitle(index, g_editingTitle);
            g_editingIndex = -1;
        }
        if (ImGui::IsItemDeactivated()) {
            g_editingIndex = -1;
        }
    } else {
        ImGui::Text("%s", card.title.c_str());
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            g_editingIndex = static_cast<int>(index);
            strncpy(g_editingTitle, card.title.c_str(), sizeof(g_editingTitle) - 1);
            g_editingTitle[sizeof(g_editingTitle) - 1] = '\0';
        }
    }

    // Show edited indicator
    if (card.titleEdited) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "*");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Title edited - changes will be saved on export");
        }
    }

    ImGui::PopFont();

    // Duration
    showtimecalc::domain::Duration duration(card.durationSeconds);
    ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Duration: %s", duration.toString().c_str());

    // Instruments
    ImGui::Text("Instruments: ");
    ImGui::SameLine();
    for (size_t i = 0; i < card.instruments.size(); ++i) {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "%s", card.instruments[i].c_str());
        if (i < card.instruments.size() - 1) {
            ImGui::SameLine();
            ImGui::Text(",");
            ImGui::SameLine();
        }
    }
    if (card.titleLines.size() > 1) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
        if (ImGui::SmallButton("Edit Parts")) {
            g_editingInstrumentsSongIndex = static_cast<int>(index);
        }
        ImGui::PopStyleColor(2);
    }

    // Filename (small text)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::TextWrapped("%s", card.filename.c_str());
    ImGui::PopStyleColor();

    // Remove button (top right)
    ImGui::SameLine(ImGui::GetWindowWidth() - 80);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Remove")) {
        g_setlistManager.removeSong(index);
    }
    ImGui::PopStyleColor(2);

    ImGui::EndChild();
    ImGui::PopStyleColor();

    // Handle drop for reordering
    if (g_draggedIndex >= 0 && g_draggedIndex != static_cast<int>(index) && ImGui::IsItemHovered()) {
        if (ImGui::IsMouseReleased(0)) {
            g_setlistManager.reorderSong(static_cast<size_t>(g_draggedIndex), index);
            g_draggedIndex = -1;
        }
    }

    ImGui::PopID();
}

int main(int argc, char** argv) {
    // Initialize GLFW
    if (!glfwInit()) {
        return 1;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window
    GLFWwindow* window = glfwCreateWindow(900, 720, "ABC Setlist Manager", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup drag and drop callback
    glfwSetDropCallback(window, drop_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Process dropped files
        if (!g_droppedFiles.empty()) {
            for (const auto& file : g_droppedFiles) {
                g_setlistManager.addSongFromFile(file);
            }
            g_droppedFiles.clear();
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("ABC Setlist Manager", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

        // Header
        ImGui::PushFont(io.Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "ABC Setlist Manager");
        ImGui::PopFont();
        ImGui::Separator();

        // Control panel
        ImGui::Text("Drag and drop .abc files onto this window to add songs");
        ImGui::Spacing();

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 450);

        // Left column: Controls
        ImGui::BeginChild("controls", ImVec2(0, 100), true);

        ImGui::Text("Padding between songs:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##pad", &g_paddingSeconds)) {
            if (g_paddingSeconds < 0) g_paddingSeconds = 0;
        }
        ImGui::SameLine();
        ImGui::Text("seconds");

        ImGui::Text("Intro time:          ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##intro", &g_introSeconds)) {
            if (g_introSeconds < 0) g_introSeconds = 0;
        }
        ImGui::SameLine();
        ImGui::Text("seconds");

        ImGui::EndChild();

        // Right column: Total duration
        ImGui::NextColumn();

        ImGui::BeginChild("total", ImVec2(0, 100), true);
        ImGui::PushFont(io.Fonts->Fonts[0]);

        auto totalDuration = g_setlistManager.getTotalDuration(g_paddingSeconds, g_introSeconds);
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "TOTAL DURATION");
        ImGui::PopFont();

        ImGui::PushFont(io.Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "%s", totalDuration.toString().c_str());
        ImGui::PopFont();

        ImGui::Text("Songs: %zu", g_setlistManager.getSongs().size());

        ImGui::EndChild();

        ImGui::Columns(1);

        // Export section
        ImGui::Spacing();
        ImGui::Text("Export Folder:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(350);
        ImGui::InputText("##exportfolder", g_exportFolderPath, sizeof(g_exportFolderPath));
        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            std::string selectedFolder = OpenFolderDialog(window);
            if (!selectedFolder.empty()) {
                strncpy(g_exportFolderPath, selectedFolder.c_str(), sizeof(g_exportFolderPath) - 1);
                g_exportFolderPath[sizeof(g_exportFolderPath) - 1] = '\0';
            }
        }
        ImGui::SameLine();
        ImGui::Checkbox("Add numbering (01_, 02_, etc.)", &g_addNumbering);

        if (ImGui::Button("Export to Folder")) {
            if (strlen(g_exportFolderPath) > 0) {
                if (g_setlistManager.exportToFolder(g_exportFolderPath, g_addNumbering)) {
                    std::string numberingMsg = g_addNumbering ? " with numbering" : "";
                    g_exportMessage = "Successfully exported " + std::to_string(g_setlistManager.getSongs().size()) +
                                     " files to folder" + numberingMsg + "!";
                    g_exportMessageTimer = 3.0f;  // Show for 3 seconds
                } else {
                    g_exportMessage = "ERROR: Failed to export files. Check folder path.";
                    g_exportMessageTimer = 5.0f;
                }
            } else {
                g_exportMessage = "ERROR: Please enter a folder path first.";
                g_exportMessageTimer = 3.0f;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear All")) {
            g_setlistManager.clear();
        }

        // Show export message if active
        if (g_exportMessageTimer > 0.0f) {
            ImVec4 color = (g_exportMessage.find("ERROR") == 0) ?
                          ImVec4(1.0f, 0.3f, 0.3f, 1.0f) :
                          ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
            ImGui::TextColored(color, "%s", g_exportMessage.c_str());
            g_exportMessageTimer -= ImGui::GetIO().DeltaTime;
        }

        ImGui::Separator();
        ImGui::Spacing();

        // Song list
        ImGui::Text("Songs in Setlist (double-click title to edit, drag to reorder):");
        ImGui::BeginChild("songlist", ImVec2(0, 0), false);

        const auto& songs = g_setlistManager.getSongs();
        if (songs.empty()) {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                             "No songs yet. Drop .abc files here to get started!");
        } else {
            for (size_t i = 0; i < songs.size(); ++i) {
                RenderSongCard(i, songs[i]);
                ImGui::Spacing();
            }
        }

        // Release drag on mouse up
        if (ImGui::IsMouseReleased(0)) {
            g_draggedIndex = -1;
        }

        ImGui::EndChild();

        ImGui::End();

        // Edit Instruments Modal
        if (g_editingInstrumentsSongIndex >= 0 &&
            g_editingInstrumentsSongIndex < static_cast<int>(g_setlistManager.getSongs().size())) {
            ImGui::OpenPopup("Edit Instrument Parts");
        }

        if (ImGui::BeginPopupModal("Edit Instrument Parts", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            const auto& song = g_setlistManager.getSongs()[g_editingInstrumentsSongIndex];

            ImGui::Text("Song: %s", song.title.c_str());
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Edit each part's title line:");
            ImGui::Spacing();

            // Display all title lines with full editing capability
            for (size_t i = 0; i < song.titleLines.size(); ++i) {
                const auto& titleLine = song.titleLines[i];

                ImGui::PushID(static_cast<int>(i));
                ImGui::Text("Part %d:", static_cast<int>(i) + 1);
                ImGui::SameLine();

                if (g_editingTitleLineIndex == static_cast<int>(i)) {
                    ImGui::SetKeyboardFocusHere();
                    ImGui::SetNextItemWidth(500.0f);  // Wider input for full titles
                    if (ImGui::InputText("##titleline", g_editingTitleLine, sizeof(g_editingTitleLine),
                                        ImGuiInputTextFlags_EnterReturnsTrue)) {
                        g_setlistManager.updateTitleLine(g_editingInstrumentsSongIndex, i, g_editingTitleLine);
                        g_editingTitleLineIndex = -1;
                    }
                    if (ImGui::IsItemDeactivated()) {
                        g_editingTitleLineIndex = -1;
                    }
                } else {
                    ImGui::Text("%s", titleLine.fullTitle.c_str());
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        g_editingTitleLineIndex = static_cast<int>(i);
                        strncpy(g_editingTitleLine, titleLine.fullTitle.c_str(), sizeof(g_editingTitleLine) - 1);
                        g_editingTitleLine[sizeof(g_editingTitleLine) - 1] = '\0';
                    }
                    if (titleLine.titleEdited) {
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "*");
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Title edited - changes will be saved on export");
                        }
                    }
                }

                ImGui::PopID();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Tip: Double-click title line to edit");

            ImGui::Spacing();
            if (ImGui::Button("Done", ImVec2(120, 0))) {
                g_editingInstrumentsSongIndex = -1;
                g_editingTitleLineIndex = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
