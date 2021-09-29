#include "raylib.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <deque>
#include "pystring.hpp"
std::string digitToHex(uint8_t data);
inline std::string colorToHex(Color data);

inline int hexToInt(std::string data);
inline uint8_t hexToByte(std::string data);
Color hexToColor(std::string data);

std::deque<std::deque<Color>> getSignPlaceHolder();

enum scalingMethod {
    SCALE_BICUBIC = 0,
    SCALE_NEAREST = 1
};

Image loadRGBAImage(const char *path, int width=-1, int height=-1, scalingMethod method=SCALE_BICUBIC);

std::string generateDirectives(char *path, int targetSize, scalingMethod method=SCALE_BICUBIC);

bool saveDrawableToFile(std::string path, std::string drawable);
