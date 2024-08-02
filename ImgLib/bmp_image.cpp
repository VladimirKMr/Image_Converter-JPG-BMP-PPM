#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

// функция вычисления отступа по ширине
static int GetBMPStride(int width) {
    const int color_count = 3;
    const int align = 4;
    const int round = 3;
    return align * ((width * color_count + round) / align);
}

PACKED_STRUCT_BEGIN BitmapInfoHeader{
    // поля заголовка Bitmap Info Header
    BitmapInfoHeader() = default;
    BitmapInfoHeader(int w, int h)
        : bi_width(w), bi_height(h)
    {
        bi_sizeImage = GetBMPStride(w) * h;
    }
    uint32_t bi_size = 40;
    int32_t bi_width = 0;
    int32_t bi_height = 0;
    uint16_t bi_planes = 1;
    uint16_t bi_bitCount = 24;
    uint32_t bi_compression = 0;
    uint32_t bi_sizeImage = 0;
    int32_t bi_xResolution = 11811;
    int32_t bi_yResolution = 11811;
    int32_t bi_colorUsed = 0;
    int32_t bi_colorImportant = 0x1000000;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapFileHeader{
    // поля заголовка Bitmap File Header
    BitmapFileHeader() = default;
    BitmapFileHeader(int w, int h) {
        bf_size = GetBMPStride(w) * h + sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    }
    char bf_type[2] = {'B', 'M'};
    uint32_t bf_size = 0;
    uint32_t bf_reserved = 0;
    uint32_t bf_offBytes = 54;  // 14 BitmapFileHeader + 40 BitmapInfoHeader
}
PACKED_STRUCT_END

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    int w = image.GetWidth();
    int h = image.GetHeight();
    BitmapFileHeader file_header(w, h);
    BitmapInfoHeader info_header(w, h);

    ofstream out(file, ios::binary);
    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    int stride = GetBMPStride(w);
    std::vector<char> buff(stride);

    for (int y = h - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(reinterpret_cast<const char*>(buff.data()), stride);
    }

    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);

    BitmapFileHeader file_header;
    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    if (!ifs || file_header.bf_type[0] != 'B' || file_header.bf_type[1] != 'M') {
        //throw runtime_error("Failed to read file header");
        return {};
    }

    BitmapInfoHeader info_header;
    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));
    if (!ifs || info_header.bi_size != 40) {
        //throw runtime_error("Failed to read info header");
        return {};
    }

    int w = abs(info_header.bi_width);
    int h = abs(info_header.bi_height);

    int stride = GetBMPStride(w);
    Image result(w, h, Color::Black());
    std::vector<char> buff(stride);

    for (int y = h - 1; y >= 0; --y) {
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), stride);

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib