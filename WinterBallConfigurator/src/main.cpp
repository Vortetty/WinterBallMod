
#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#pragma warning( push, 0 )
#pragma warning( disable : 4576 )
#pragma warning( disable : 2290 )
#include "hideConsole.hpp"
bool consoleHidden = hideConsole();
#endif

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_RICONS
#define GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
#define SUPPORT_FILEFORMAT_BMP
#define SUPPORT_FILEFORMAT_PNG
#define SUPPORT_FILEFORMAT_TGA
#define SUPPORT_FILEFORMAT_JPG
#define SUPPORT_FILEFORMAT_GIF
#define SUPPORT_FILEFORMAT_PSD
#define SUPPORT_FILEFORMAT_PIC
#define SUPPORT_FILEFORMAT_HDR
#define SUPPORT_FILEFORMAT_DDS
#define SUPPORT_FILEFORMAT_PKM
#define SUPPORT_FILEFORMAT_KTX
#define SUPPORT_FILEFORMAT_PVR
#define SUPPORT_FILEFORMAT_ASTC
#include "raylib.h"
#include "raygui.h"
#include "gui_textbox_extended.h"
#undef RAYGUI_IMPLEMENTATION

typedef Vector2 Vec2;

#include "json.hpp"
using json = nlohmann::json;


#include "tinyfiledialogs.h"
#include <limits.h>
#include <vector>
#include <iostream>
#include <thread>
#include <filesystem>
#include <future>
#include <iostream>
#include <fstream>
#include <chrono>
#include "drawableHelper.hpp"
#include "theme.hpp"
namespace fs = std::filesystem;

#ifdef _WIN32
typedef fs::filesystem_error fs_error;

void busy_wait_nop() {
}
#else
typedef fs::__cxx11::filesystem_error fs_error;
void busy_wait_nop() {
    asm(" ");
}
#endif

#define fitToRange(x, min, max) (x < min ? min : (x > max ? max : x))

/*
Config format (JSONC):

{
        "scale": <number>, // Scale in blocks, by default 1 block
        "frameCount": <number>,
        "drawables": [
            <drawable strings>
        ],
        "allowInterract": true // Allows player to interract with blocks while in ball form when enabled
}

*/

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

struct drawableListEntry {
    char path[512] = "";
    bool beingEdited = false;
};

char* get_homedir(void) {
    char homedir[512] = "";
#ifdef _WIN32
    snprintf(homedir, 512, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
    //snprintf(homedir, 512, "%s", getenv("HOME"));
#endif
    return strdup(homedir);
}

void openChoosePathGui(char const *title, char const *acceptableFiletypes[], int fileTypesCount, char const *fileTypeComment, char path[512]) {
    std::string fp;

    if (!std::filesystem::exists(path)) {
        fp = get_homedir();
    } else if (std::filesystem::is_directory(path)) {
        fp = path;
    } else {
        std::filesystem::path p = std::filesystem::path(path);
        fp = p.string();
    }

    std::cout << "Opening gui in " << fp << std::endl;

    char const *fFile = tinyfd_openFileDialog( // there is also a wchar_t version
        title, // title
        fp.c_str(), // optional initial directory
        fileTypesCount, // number of filter patterns
        acceptableFiletypes, // char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        fileTypeComment, // optional filter description
        0 // forbid multiple selections
    );
    
    if (fFile != NULL) 
        strcpy(path, fFile);

    return;
}
void callChoosePathGui(char const* title, char const* acceptableFiletypes[], int fileTypesCount, char const* fileTypeComment, char path[512]) {
#ifdef _WIN32
    openChoosePathGui(title, acceptableFiletypes, fileTypesCount, fileTypeComment, path);
#else
    std::thread t(openChoosePathGui, title, acceptableFiletypes, fileTypesCount, fileTypeComment, path);
    while (!t.joinable()) busy_wait_nop();
    t.join();
#endif
}

void openSaveGui(char const *title, const char userHomePath[512], std::promise<std::string> &&promise) {
    char const * validFiles[] = { "*.jsonc" };

    std::cout << "Opening gui in " << userHomePath << std::endl;

    char const *fFile = tinyfd_saveFileDialog( // there is also a wchar_t version
        title, // title
        (const char*)(fs::path(userHomePath)/"config.jsonc").c_str(), // optional initial directory
        1, // number of filter patterns
        validFiles, // char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        "WinterBall tech config" // optional filter description
    );

    if (fFile != NULL) promise.set_value(std::string(fFile));
}

inline void addDesc(Vec2 pos, char *desc) {
    GuiSetStyle(TEXTBOX, BORDER_COLOR_DISABLED, GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED,  GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL,   GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED,  GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
    GuiTextBoxMulti({pos.x+145, pos.y, 285, 30}, desc, 10, false);
    GuiSetStyle(TEXTBOX, BORDER_COLOR_DISABLED, GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED,  GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL,   GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED,  GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED));
}

inline void addSpinner(Vec2 pos, char const *label, char *description, int &value, int &bufValue, int min, int max, bool &editEnabled) {
    GuiLabel({pos.x, pos.y, 100, 10}, label);
    addDesc(pos, description);
    if(GuiSpinner({pos.x, pos.y+10, 125, 20}, NULL, &bufValue, min, max, editEnabled)) editEnabled = !editEnabled;
    value = fitToRange(value, min, max);
    bufValue = fitToRange(bufValue, min, max);
    if(!editEnabled && value > 0)
        value = bufValue;
}

inline void addCheckbox(Vec2 pos, char const *label, char *description, bool &checked) {
    GuiLabel({pos.x, pos.y, 100, 10}, label);
    addDesc(pos, description);
    checked = GuiCheckBox({pos.x, pos.y+10, 100, 20}, "", checked);
    GuiDrawText(GuiIconText(checked ? RICON_OK_TICK : RICON_CROSS, ""), GetTextBounds(LABEL, {pos.x+105, pos.y+15-(checked?1:0), 10, 10}), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade((checked ? (Color){0x4b, 0xff, 0x7b, 0xff} : (Color){0xff, 0x4b, 0x54, 0xff}), guiAlpha));
}

inline void addComboBox(Vec2 pos, char const *label, char *description, const char *items, int &optionNumber) {
    GuiLabel({pos.x, pos.y, 100, 10}, label);
    addDesc(pos, description);
    optionNumber = GuiComboBox({pos.x, pos.y+10, 125, 20}, items, optionNumber);
}

int main(int argc, char* argv[]) {
    InitWindow(800, 600, "WinterBall Tech Config Generator");

    //GuiLoadStyle("assets/dark+pink.rgs");
    applyTheme();

    int frameCount = 0;
    int frameCountBuf = 0;
    int frameMax = 10000;
    bool frameCountEditEnabled = false;

    int ballScale = 1;
    int ballScaleBuf = 1;
    int scaleMax = 10;
    bool ballScaleEditEnabled = false;

    int animSpeedDivisor = 1;
    int animSpeedDivisorBuf = 1;
    int animSpeedDivisorMax = 100;
    bool animSpeedDivisorEditEnabled = false;

    bool allowInterract = false;

    int scaleMethodSelection = 0;

    Rectangle frameScrollerRec = {445, 5, 350, 550};
    Rectangle frameScrollerContentRec = frameScrollerRec;
    frameScrollerContentRec.width -= 5;
    Vec2 frameScrollerPos = {0, 0};
    char const *acceptableFiletypes[] = { "*.bmp", "*.png", "*.tga", "*.jpg", "*.psd", "*.pic", "*.hdr", "*.dds", "*.pkm", "*.ktx", "*.pvr", "*.astc", "*.BMP", "*.PNG", "*.TGA", "*.JPG", "*.PSD", "*.PIC", "*.HDR", "*.DDS", "*.PKM", "*.KTX", "*.PVR", "*.ASTC" };
    char const *acceptableAnimFiletypes[] = { "*.gif", "*.GIF" };
    char const *acceptableConfigFiletypes[] = { "*.jsonc" };
    bool SaveGUIEnabled = false;
    bool flashInvalidPaths = false;
    bool invalidPathsExist = false;
    char dummyPath[512] = "";

    std::vector<drawableListEntry> drawableList;

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        char const *homeDirectory = strcat(getenv("HOMEDRIVE"), getenv("HOMEPATH"));
    #elif __APPLE__ || __linux__ || __unix__
        char const *homeDirectory = strcat(getenv("HOME"), "/");
    #else
        #warning "Unknown platform, the default file picker directory will be up to the os."
        char const *homeDirectory = "";
    #endif

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground({0x12, 0x12, 0x12, 0xFF});
            if(!IsCursorOnScreen()) GuiSetState(GUI_STATE_NORMAL);
            if (!SaveGUIEnabled) {    
                GuiTextBox({1000, 1000, 0, 0}, dummyPath, 256, false); // Dummy textbox to get the blank textbox value
                
                //
                // Frame count
                // 
                    addSpinner({5, 5}, "Animation frame Count:", (char*)"How many frames are in the animation", frameCount, frameCountBuf, 1, frameMax, frameCountEditEnabled);
                    if(!frameCountEditEnabled && frameCount > 0) {
                        frameScrollerContentRec.height = frameScrollerRec.height + (frameCount * 76 - (frameScrollerRec.height - 5));
                        drawableList.resize(frameCount);
                    }

                //
                // Ball scale
                //
                    addSpinner({5, 40}, "Ball scale:", (char*)"Ball diameter in blocks", ballScale, ballScaleBuf, 1, scaleMax, ballScaleEditEnabled);

                //
                // Block interractons
                //
                    addCheckbox({5, 75}, "Allow interractions:", (char*)"If enabled, allows you to interact with things while in ball mode", allowInterract);

                //
                // Animation speed divisor
                //
                    addSpinner({5, 110}, "Anim speed divisor:", (char*)"Higher values make the speed of the animation slower", animSpeedDivisor, animSpeedDivisorBuf, 1, animSpeedDivisorMax, animSpeedDivisorEditEnabled);

                //
                // Select image scale method
                //
                    addComboBox({5, 145}, "Scaling method:", (char*)"Scaling method used to resize images for frames to  the proper size (if required)", "Bicubic;Nearest", scaleMethodSelection);

                //
                // Save button
                //
                    if (GuiButton({5, 575, 215, 20}, GuiIconText(RICON_FILE_SAVE, "Save config"))) {
                        bool invalidPath = false;
                        for (int i = 0; i < frameCount; i++) {
                            if (!std::filesystem::exists(drawableList[i].path)) invalidPath = true;
                        }
                        if (invalidPath) {
                            flashInvalidPaths = true;
                        } else {
                            SaveGUIEnabled = true;
                        }
                    }

                //
                // Load button
                //
                    if (GuiButton({225, 575, 215, 20}, GuiIconText(RICON_FILE_OPEN, "Load config"))) {
                        char path[512];
                        callChoosePathGui(TextFormat("Select image to load from"), acceptableConfigFiletypes, 1, "GIF Files", path);

                        if (std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
                            std::cout << "Loading config from " << path << std::endl;

                            json j;
                            std::ifstream i(path);
                            j = json::parse(i, nullptr, true, true);

                            // j["frameCount"] = frameCount;
                            // j["scale"] = ballScale;
                            // j["allowInterract"] = allowInterract;
                            // j["animationSpeedDivisor"] = animSpeedDivisor;
                            // j["drawables"] = json::array();
                            // 
                            // for (int i = 0; i < frameCount; i++) {
                            //     j["drawables"].push_back(generateDirectives(drawableList[i].path, 8*ballScale-1, (scalingMethod)scaleMethodSelection));
                            // }

                            fs::remove_all("gifLoaderTempFiles");
                            fs::create_directory("gifLoaderTempFiles");

                            frameCount = j.value("frameCount", 0);
                            frameCountBuf = frameCount;

                            ballScale = j.value("scale", 1);
                            ballScaleBuf = ballScale;

                            allowInterract = j.value("allowInterract", false);

                            animSpeedDivisor = j.value("animationSpeedDivisor", 1);
                            animSpeedDivisorBuf = animSpeedDivisor;

                            drawableList.resize(frameCount);
                            for (int i = 0; i < frameCount; i++) {
                                saveDrawableToFile(TextFormat("gifLoaderTempFiles/frame_%i.png", i), j["drawables"][i]);
                                strcpy(drawableList[i].path, TextFormat("gifLoaderTempFiles/frame_%i.png", i));
                            }
                        }
                    }

                //
                // Load from gif button
                //
                    if (GuiButton({445, 575, 350, 20}, GuiIconText(RICON_FILE_OPEN, "Load frames from gif"))) {
                        char path[512];
                        callChoosePathGui(TextFormat("Select image to load frames from"), acceptableAnimFiletypes, 2, "GIF Files", path);

                        if (std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
                            std::cout << "Loading GIF from " << path << std::endl;
                            int gifFrameCount = 0;
                            Image im = LoadImageAnim(path, &gifFrameCount); // Don't need to convert the gif to RGBA, it's already in that format (https://github.com/raysan5/raylib/blob/master/src/textures.c#L276)
                            Image imBuf = GenImageColor(im.width, im.height, {0, 0, 0, 0});
                            std::cout << "Loaded " << gifFrameCount << " frames" << std::endl;

                            frameCountBuf = gifFrameCount;
                            frameCount = gifFrameCount;
                            drawableList.resize(frameCount);

                            fs::remove_all("gifLoaderTempFiles");
                            fs::create_directory("gifLoaderTempFiles");

                            for (int frame = 0; frame < gifFrameCount; frame++) {
                                for (int x = 0; x < im.width; x++) {
                                    for (int y = 0; y < im.height; y++) {
                                        ((uint8_t*)imBuf.data)[(y*imBuf.width+x)*4] = ((uint8_t*)im.data)[(y*im.width+x+(im.width*im.height*frame))*4];
                                        ((uint8_t*)imBuf.data)[(y*imBuf.width+x)*4+1] = ((uint8_t*)im.data)[(y*im.width+x+(im.width*im.height*frame))*4+1];
                                        ((uint8_t*)imBuf.data)[(y*imBuf.width+x)*4+2] = ((uint8_t*)im.data)[(y*im.width+x+(im.width*im.height*frame))*4+2];
                                        ((uint8_t*)imBuf.data)[(y*imBuf.width+x)*4+3] = ((uint8_t*)im.data)[(y*im.width+x+(im.width*im.height*frame))*4+3];
                                    }
                                }
                                ExportImage(imBuf, TextFormat("gifLoaderTempFiles/frame_%i.png", frame));
                                strcpy(drawableList[frame].path, TextFormat("gifLoaderTempFiles/frame_%i.png", frame));
                            }
                        }
                    }

                //
                // Divider bar
                //
                    DrawRectangle(140, 5, 1, 565, Fade(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)), guiAlpha));

                //
                // Frame list
                //
                    GuiDrawRectangle({445, 554, 350, 15}, 1, Fade(GetColor(GuiGetStyle(SCROLLBAR, BORDER_COLOR_NORMAL)), guiAlpha), Fade(GetColor(GuiGetStyle(SCROLLBAR, BASE_COLOR_NORMAL)), guiAlpha));
                    GuiLabel({450, 557, 325, 10}, TextFormat("Frames should be %dx%dpx | %s", 8*ballScale-1, 8*ballScale-1, (invalidPathsExist ? "Some paths are invalid" : "All paths are valid")));

                    Rectangle view = GuiScrollPanel(frameScrollerRec, frameScrollerContentRec, &frameScrollerPos);
                    BeginScissorMode(view.x, view.y, view.width, view.height);
                        invalidPathsExist = false;
                        for (int i = 0; i < frameCount; i++) {
                            try {
                                if (!std::filesystem::exists(drawableList[i].path) || std::filesystem::is_directory(drawableList[i].path)) {
                                    invalidPathsExist = true;
                                    if (flashInvalidPaths) { GuiDisable(); GuiLock(); }
                                    GuiDrawText(TextFormat("Frame %d image path (%s path):", i+1, strcmp(drawableList[i].path, dummyPath) ? "invalid" : "empty"), GetTextBounds(LABEL, {450, i * 35 + 9 + frameScrollerPos.y, 300, 10}), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade({0xff, 0x4b, 0x54, 0xff}, guiAlpha));
                                    if (flashInvalidPaths) { GuiEnable(); GuiUnlock(); }
                                } else {
                                    GuiLabel({450, i * 76 + 9 + frameScrollerPos.y, 300, 10}, TextFormat("Frame %d image path:", i+1));
                                }
                            } catch (fs_error &e) {
                                GuiLabel({450, i * 76 + 9 + frameScrollerPos.y, 300, 10}, TextFormat("Frame %d image path:", i+1));
                            }
                            if (GuiButton({450, i * 76 + 20 + frameScrollerPos.y, 20, 60}, GuiIconText(RICON_FILE_OPEN, ""))) {
                                callChoosePathGui(TextFormat("Select image for frame %d", i+1), acceptableFiletypes, 24, "Image files", drawableList[i].path);
                            }
                            // I disabled the extended text box because it's not working properly
                            //if (GuiTextBoxEx({475, i * 76 + 20 + frameScrollerPos.y, 290, 30}, drawableList[i].path, 512, drawableList[i].beingEdited)) 
                            //    drawableList[i].beingEdited = !drawableList[i].beingEdited;
                            if (GuiTextBoxMulti({475, i * 76 + 20 + frameScrollerPos.y, 290, 60}, drawableList[i].path, 512, drawableList[i].beingEdited)) 
                                drawableList[i].beingEdited = !drawableList[i].beingEdited;
                        }
                    EndScissorMode();

                    if (guiState == GUI_STATE_DISABLED) {
                        SetTargetFPS(0);
                        EndDrawing();
                        BeginDrawing();
                        SetTargetFPS(60);
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        GuiEnable();
                        GuiUnlock();
                    }
                } else {
                    DrawText("Saving config", 800/2-(MeasureText("Saving config", 40)/2), 600/2-40, 40, GetColor(GuiGetStyle(LABEL, TEXT)));

                    SetTargetFPS(0);
                    EndDrawing();
                    BeginDrawing();
                    SetTargetFPS(60);

                    std::promise<std::string> pathPromise;
                    auto pathPromiseFuture = pathPromise.get_future();
#ifdef _WIN32
                    openSaveGui("Save configuration file", get_homedir(), std::move(pathPromise));
#else
                    std::thread t(openSaveGui, "Save configuration file", ".", std::move(pathPromise));
                    while (!t.joinable()) busy_wait_nop();
                    t.join();
#endif
                    std::string path;
                    try { path = pathPromiseFuture.get(); }
                    catch (std::future_error &e) { path = "NULL"; }

                    if (path != "NULL") {
                        std::cout << "Saving to " << path << std::endl;

                        json j;

                        j["frameCount"] = frameCount;
                        j["scale"] = ballScale;
                        j["allowInterract"] = allowInterract;
                        j["animationSpeedDivisor"] = animSpeedDivisor;
                        j["drawables"] = json::array();
                        
                        for (int i = 0; i < frameCount; i++) {
                            j["drawables"].push_back(generateDirectives(drawableList[i].path, 8*ballScale-1, (scalingMethod)scaleMethodSelection));
                        }

                        std::ofstream jsonOut;
                        jsonOut.open(path);
                        jsonOut << j.dump(4) << "\n";
                        jsonOut.close();
                    }

                    SaveGUIEnabled = false;
                }
        EndDrawing();
    }

    fs::remove_all("gifLoaderTempFiles");

    CloseWindow();

    return 0;
}

#ifdef _WIN32
#pragma warning( pop, 0 )
#endif
