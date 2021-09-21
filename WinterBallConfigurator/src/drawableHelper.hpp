#include "raylib.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

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
std::string colorToHex(Color data) {
    return digitToHex(data.r) + digitToHex(data.g) + digitToHex(data.b) + digitToHex(data.a);
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
