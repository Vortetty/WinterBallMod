#include "raylib.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <deque>
#include "pystring.hpp"

/*
 * 
 * Ported from JS to C++ by vortetty
 * Original code can be found here: https://github.com/Silverfeelin/Starbound-Hatter/blob/master/scripts/drawables.js
 * 
 * function getSignPlaceHolder() {
 *   var colors = [];
 * 
 *   for(var i = 0; i < 32; i++) {
 *     colors[i] = [];
 *     var x = i;
 * 
 *     // Compensate for missing hexadecimal values (A/F).
 *     if (i >= 9)
 *       x += 6;
 *     if (i >= 19)
 *       x += 6;
 *     if (i >= 29)
 *       x += 6;
 * 
 *     for (var j = 0; j < 8; j++) {
 *       colors[i][j] = [x+1,0,j+1,1];
 *     }
 *   }
 * 
 *   return colors;
 * }
 * 
 * function digitToHex(digit) {
 *   const h = Number(digit).toString(16);
 *   return h.length === 1 ? `0${h}` : h;
 * }
 * 
 * 
 * function colorToHex(color) {
 *   var colors = [];
 *   colors.push(digitToHex(color[0]));
 *   colors.push(digitToHex(color[1]));
 *   colors.push(digitToHex(color[2]));
 *   colors.push(digitToHex(color[3]));
 *   if (colors[3] === 'ff') { delete colors[3] }
 * 
 *   const long = colors.some((s) => s[0] !== s[1]);
 *   return long ? colors.join('') : `${colors[0][0]}${colors[1][0]}${colors[2][0]}${colors[3] ? colors[3][0] : ''}`;
 * }
 * 
 * function generateDirectives(image, imageOptions) {
 *   if (image == null)
 *     return;
 * 
 *   if (imageOptions === undefined || imageOptions == null)
 *     imageOptions = {};
 * 
 *   var width = imageOptions.crop ? 43 : image.width,
 *       height = imageOptions.crop ? 43 : image.height;
 * 
 *   // Fetch color codes for the signplaceholder asset.
 *   var colors = getSignPlaceHolder();
 * 
 *   // Draw the selected image on a canvas, to fetch the pixel data.
 *   var canvas = document.createElement("canvas");
 *   canvas.width = width;
 *   canvas.height = height;
 * 
 *   var canvasContext = canvas.getContext("2d");
 * 
 *   // Flip image upside down, to compensate for the 'inverted' y axis.
 *   if (!imageOptions.crop) {
 *     canvasContext.translate(0, image.height);
 *     canvasContext.scale(1,-1);
 *     canvasContext.drawImage(image, 0, 0, width, height);
 *     canvasContext.scale(1,1);
 *   } else {
 *     canvasContext.translate(0, 43);
 *     canvasContext.scale(1,-1);
 *     canvasContext.drawImage(image, 43, 0, 43, 43, 0, 0, 43, 43);
 *     canvasContext.scale(1,1);
 *   }
 * 
 *   const r = digitToHex(width - 1);
 *   const t = digitToHex(height - 1);
 * 
 *   const bottomRight = `${r}010000`;
 *   const topLeft = `0001${t}00`;
 *   const topRight = `${r}01${t}00`;
 * 
 *   drawables = "?setcolor=fff?replace;fff0=fff?crop;0;0;2;2" +
 *     "?blendmult=/items/active/weapons/protectorate/aegisaltpistol/beamend.png;0;0" +
 *     `?replace;a355c0a5=00010000;a355c07b=${bottomRight};ffffffa5=${topLeft};ffffff7b=${topRight}` +
 *     `?scale=${width};${height}` +
 *     `?crop;1;1;${width + 1};${height + 1}` +
 *     "?replace";
 * 
 *   for (let x = 0; x < width; x++) {
 *     for (let y = 0; y < height; y++) {
 *       const c = canvasContext.getImageData(x, y, 1, 1).data;
 *       if (c[3] <= 1) continue;
 *       drawables += `;${digitToHex(x)}01${digitToHex(y)}00=${colorToHex(c)}`;
 *     }
 *   }
 * 
 *   return drawables;
 * }
 */

std::string digitToHex(uint8_t data) {
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)data;
    return ss.str();
}
inline std::string colorToHex(Color data) {
    return digitToHex(data.r) + digitToHex(data.g) + digitToHex(data.b) + digitToHex(data.a);
}

inline int hexToInt(std::string data) {
    return std::stoi(data, nullptr, 16);
}
inline uint8_t hexToByte(std::string data) {
    return (uint8_t)(std::stoi(data, nullptr, 16) & 0xff);
}
Color hexToColor(std::string data) {
    switch (data.length()) {
        case 3:
            return { 
                hexToByte(pystring::mul(data.substr(0, 1), 2)),
                hexToByte(pystring::mul(data.substr(1, 1), 2)),
                hexToByte(pystring::mul(data.substr(2, 1), 2)),
                0xff
            };
            break;
        case 4:
            return { 
                hexToByte(pystring::mul(data.substr(0, 1), 2)),
                hexToByte(pystring::mul(data.substr(1, 1), 2)),
                hexToByte(pystring::mul(data.substr(2, 1), 2)),
                hexToByte(pystring::mul(data.substr(3, 1), 2))
            };
            break;
        case 6:
            return { 
                hexToByte(data.substr(0, 2)),
                hexToByte(data.substr(2, 2)),
                hexToByte(data.substr(4, 2)),
                0xff
            };
            break;
        case 8:
            return { 
                hexToByte(data.substr(0, 2)),
                hexToByte(data.substr(2, 2)),
                hexToByte(data.substr(4, 2)),
                hexToByte(data.substr(6, 2))
            };
            break;
        
        default:
            return { 0, 0, 0, 0 };
    }
}

std::vector<std::vector<Color>> getSignPlaceHolder() {
    std::vector<std::vector<Color>> colors;
    for (int i = 0; i < 32; i++) {
        colors.push_back(std::vector<Color>());
        int x = i;
        // Compensate for missing hexadecimal values (A/F).
        if (i >= 9)
            x += 6;
        if (i >= 19)
            x += 6;
        if (i >= 29)
            x += 6;
        
        for (int j = 0; j < 8; j++) {
            colors[i][j] = {(uint8_t)(x+1), 0, (uint8_t)(j+1), 1};
        }
    }
    return colors;
}

enum scalingMethod {
    SCALE_BICUBIC = 0,
    SCALE_NEAREST = 1
};

Image loadRGBAImage(const char *path, int width=-1, int height=-1, scalingMethod method=SCALE_BICUBIC) {
    Image image = LoadImage(path);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    if (method == SCALE_BICUBIC) {
        ImageResize(&image, (width==-1 ? image.width : width), (height==-1 ? image.height : height));
    } else if (method == SCALE_NEAREST) {
        ImageResizeNN(&image, (width==-1 ? image.width : width), (height==-1 ? image.height : height));
    }
    return image;
}

std::string generateDirectives(char *path, int targetSize, scalingMethod method=SCALE_BICUBIC) {
    Image image = loadRGBAImage(path, targetSize, targetSize);
    ImageFlipVertical(&image);

    std::string rt = digitToHex(targetSize - 1);

    std::string bottomRight = rt + "010000";
    std::string topLeft = "0001" + rt + "00";
    std::string topRight = rt + "01" + rt + "00";

    //std::vector<std::vector<Color>> colors = getSignPlaceHolder();

    std::string drawables = (std::string)"?setcolor=fff?replace;fff0=fff?crop;0;0;2;2" +
        (std::string)"?blendmult=/items/active/weapons/protectorate/aegisaltpistol/beamend.png;0;0" +
        (std::string)"?replace;a355c0a5=00010000;a355c07b=" + bottomRight + ";ffffffa5=" + topLeft + ";ffffff7b=" + topRight +
        (std::string)"?scale=" + std::to_string(targetSize) + ";" + std::to_string(targetSize) +
        (std::string)"?crop;1;1;" + std::to_string(targetSize+1) + ";" + std::to_string(targetSize+1) +
        (std::string)"?replace";

    
    for (int x = 0; x < targetSize; x++) {
        for (int y = 0; y < targetSize; y++) {
            Color c = { ((uint8_t*)image.data)[(x + y * targetSize) * 4], ((uint8_t*)image.data)[(x + y * targetSize) * 4 + 1], ((uint8_t*)image.data)[(x + y * targetSize) * 4 + 2], ((uint8_t*)image.data)[(x + y * targetSize) * 4 + 3] };
            if (!(c.a <= 1)) 
                drawables += ";"+ digitToHex(x) + "01"+ digitToHex(y) + "00="+ colorToHex(c);
        }
    }

    return drawables;
}

bool saveDrawableToFile(std::string path, std::string drawable) {
    /* [markdown]
     * >
     * > # This file looks best in visual studio code with the markdown anywhere extension.
     * >
     * > Example drawable:
     * > ?setcolor=fff?replace;fff0=fff
     * >  ?crop;0;0;2;2
     * >  ?blendmult=/items/active/weapons/protectorate/aegisaltpistol/beamend.png;0;0
     * >  ?replace;a355c0a5=00010000;a355c07b=06010000;ffffffa5=00010600;ffffff7b=06010600
     * >  ?scale=7;7
     * >  ?crop;1;1;8;8
     * >  ?replace;00010200=642f00;00010300=e359b9;00010400=36261e;01010100=642f00;01010200=a46e06;01010300=ff99e6;01010400=513a2d;01010500=36261e;02010000=642f00;02010100=a46e06;02010200=a46e06;02010300=feffff;02010400=513a2d;02010500=714f3c;02010600=513a2d;03010000=642f00;03010100=a46e06;03010200=a46e06;03010300=feffff;03010400=513a2d;03010500=714f3c;03010600=513a2d;04010000=642f00;04010100=a46e06;04010200=a46e06;04010300=feffff;04010400=714f3c;04010500=976750;04010600=513a2d;05010100=642f00;05010200=a46e06;05010300=ff99e6;05010400=976750;05010500=513a2d;06010200=a46e06;06010300=e359b9;06010400=513a2d
     * > 
     * > Format:
     * > ?setcolor=fff?replace;fff0=fff
     * >  ?crop;0;0;2;2
     * >  ?blendmult=/items/active/weapons/protectorate/aegisaltpistol/beamend.png;0;0
     * >  ?replace;a355c0a5=00010000;a355c07b=`hex for image width/height-1`010000;ffffffa5=0001`hex for image width/height-1`00;ffffff7b=0601`hex for image width/height-1`00
     * >  ?scale=`image width/height`;`image width/height`
     * >  ?crop;1;1;`hex for image width/height+1`;`hex for image width/height+1`
     * >  ?replace **[`;x pos as hex`01`y pos as hex`00=`color as hex`]**(repeat for each pixel with color, all pixels not here are rgba(0,0,0,0))
     * >
     * > How it works:
     * >  1) Crop off the first chunk of the drawable, easiest way to make sure this works with hatter directives is 
     * >      to split on `?` and pop the first 6 directives off the list(there will be an empty one), that way we are left with the scale, crop, and replace directives 
     * >  2) Get the separate the directives into a list of strings
     * >  3) We can discard the crop directive, as we will be using the scale directive to get dimensions
     * >  4) Split the scale directive on `'` and store the height integer, since they are always squares we can discard the width
     * >  5) Split the replace directives on `;` and parse them all into colors
     * >   5.1) Iterate every replace directive and split it on `=` to get the color and position
     * >   5.2) split the position into 4 strings, the first and third are the x and y position respectively
     * >   5.3) Convert the x and y position to integers from hex strings
     * >   5.4) Convert the color to an int then to a Color struct
     * >   5.5) Write color to the image at the x and y position
     * >  6) Write the image to the file specified by `path`
     * >  7) Return true if succeeded, return false otherwise
     */

    //
    // Parse the drawable into directives
    //
    std::deque<std::string> directives;
    pystring::split(drawable, directives, "?");
    for (int i = 0; i < 6; i++)
        directives.pop_front();
    std::string scaleDirective = directives[0];
    std::string replaceDirective = directives[2];
    
    //
    // Parse the scale directive into an integer for size
    //
    std::deque<std::string> scaleDirectiveSplit;
    pystring::split(scaleDirective, scaleDirectiveSplit, ";");
    int size = std::stoi(scaleDirectiveSplit.back());

    //
    // Parse the replace directives into colors
    //
    std::deque<std::string> replaceDirectiveSplit;
    pystring::split(replaceDirective, replaceDirectiveSplit, ";");
    replaceDirectiveSplit.pop_front(); // Removes `replace` since it isn't a color

    Image im = GenImageColor(size, size, { 0, 0, 0, 0 });

    for (int i = 0; i < replaceDirectiveSplit.size(); i++) {
        std::deque<std::string> pixelDataSplit;
        pystring::split(replaceDirectiveSplit[i], pixelDataSplit, "=");

        int x = (hexToInt(pixelDataSplit[0]) >> (8*3)) & 0xff;
        int y = (hexToInt(pixelDataSplit[0]) >> (8*1)) & 0xff;

        Color c = hexToColor(pixelDataSplit[1]);

        ImageDrawPixel(&im, x, y, c);
    }

    ImageFlipVertical(&im);

    ExportImage(im, path.c_str());

    return true;
}
