#include "raygui.h"
#include "gui_textbox_extended.h"
#include <cstdio>
#include <filesystem>
namespace fs = std::filesystem;

const char theme[] =
"#                                                             \n"
"# Colors generated on https://materialpalettes.com/ using:    \n"
"#   Primary color - #FFD3DC                                   \n"
"#   Secondary color - #121212                                 \n"
"#                                                             \n"
"p 00 19 0x121212ff    BACKGROUND_COLOR                        \n"
"p 00 00 0x505050ff    DEFAULT_BORDER_COLOR_NORMAL             \n"
"p 00 01 0x121212ff    DEFAULT_BASE_COLOR_NORMAL               \n"
"p 00 02 0x8a8a8aff    DEFAULT_TEXT_COLOR_NORMAL               \n"
"p 00 03 0xffd3dcff    DEFAULT_BORDER_COLOR_FOCUSED            \n"
"p 00 04 0x505050ff    DEFAULT_BASE_COLOR_FOCUSED              \n"
"p 00 05 0xabababff    DEFAULT_TEXT_COLOR_FOCUSED              \n"
"p 00 06 0xffd3dcff    DEFAULT_BORDER_COLOR_PRESSED            \n"
"p 00 07 0x505050ff    DEFAULT_BASE_COLOR_PRESSED              \n"
"p 00 08 0xabababff    DEFAULT_TEXT_COLOR_PRESSED              \n"
"p 00 09 0xffedf2ff    DEFAULT_BORDER_COLOR_DISABLED           \n"
"p 00 10 0xabababff    DEFAULT_BASE_COLOR_DISABLED             \n"
"p 00 11 0xe2e2e2ff    DEFAULT_TEXT_COLOR_DISABLED             \n"
"                                                              \n"
"p 12 09 0x505050ff    LISTVIEW_BORDER_COLOR_DISABLED          \n"
"p 12 10 0x121212ff    LISTVIEW_BASE_COLOR_DISABLED            \n"
"p 12 11 0x8a8a8aff    LISTVIEW_TEXT_COLOR_DISABLED            \n";

void applyTheme() {
	const char* tmpPath = (const char*)(fs::temp_directory_path()/tmpnam(nullptr)).c_str();

	FILE* f = fopen(tmpPath, "wb");
	fwrite(theme, sizeof(char), sizeof(theme), f);
	fclose(f);

	GuiLoadStyle(tmpPath);

	fs::remove(tmpPath);
}