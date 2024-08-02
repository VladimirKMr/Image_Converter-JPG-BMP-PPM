#include <bmp_image.h>
#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>

#include <filesystem>
#include <iostream>
#include <string_view>

#include "editing.h"

using namespace std;

enum class Format {
    BMP,
    JPEG,
    PPM,
    UNKNOWN
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".bmp"sv) {
        return Format::BMP;
    }
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }
    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    return Format::UNKNOWN;
}

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

namespace format_interfaces {

class PPM : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SavePPM(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadPPM(file);
    }
};

class JPEG : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveJPEG(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadJPEG(file);
    }
};

class BMP : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveBMP(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadBMP(file);
    }
};

}  // namespace format_interfaces

const ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
    Format format = GetFormatByExtension(path);
    if (format == Format::UNKNOWN) {
        return nullptr;
    }

    static const format_interfaces::BMP bmpInterface;
    static const format_interfaces::JPEG jpegInterface;
    static const format_interfaces::PPM ppmInterface;

    switch (format) {
        case Format::BMP:
            return &bmpInterface;

        case Format::JPEG:
            return &jpegInterface;

        case Format::PPM:
            return &ppmInterface;

        case Format::UNKNOWN:
            return nullptr;
    }

    return nullptr;
}


int main() {

    cout << "Supported formats: JPG, BMP, PPM." << endl;

    img_lib::Path executable_path = filesystem::absolute(filesystem::current_path());

    string input_filename, output_filename;

    cout << "Set input filename in current directory (example: file.jpg): ";
    cin >> input_filename;
    cout << "Set output filename in current directory (example: file.bmp): ";
    cin >> output_filename;

    img_lib::Path input_path = executable_path / input_filename;
    img_lib::Path output_path = executable_path / output_filename;

    if (input_path.empty() || output_path.empty()) {
        cerr << "Incorrect name <in_file> <out_file>"sv << endl;
        return 1;
        system("pause");
    }
    

    const ImageFormatInterface* in_format_interface = GetFormatInterface(input_path);
    if (!in_format_interface) {
        cerr << "Unknown format of the input file"sv << endl;
        system("pause");
        return 2;
    }

    const ImageFormatInterface* out_format_interface = GetFormatInterface(output_path);
    if (!out_format_interface) {
        cerr << "Unknown format of the output file"sv << endl;
        system("pause");
        return 3;
    }

    img_lib::Image image = in_format_interface->LoadImage(input_path);

    if (!image) {
        cerr << "Loading failed"sv << endl;
        system("pause");
        return 4;
    }

    //преобразования при необходимости
    int edit_variant = 0;
    cout << "Set edit variant." << endl;
    cout << "0 - Nothing" << endl;
    cout << "1 - Negate" << endl;
    cout << "2 - Vertical mirroring" << endl;
    cout << "3 - Horizontal mirroring" << endl;
    cout << "4 - Sobel's transformation" << endl;
    cin >> edit_variant;

    if (!out_format_interface->SaveImage(output_path, image)) {
        cerr << "Saving failed"sv << endl;
        system("pause");
        return 5;
    }
    else {
        switch (edit_variant)
        {
        case 1:
            edit::NegateInplace(image);
            break;
        case 2:
            edit::VMirrInplace(image);
            break;
        case 3:
            edit::HMirrInplace(image);
            break;
        case 4:
            img_lib::Image image_sobel = edit::Sobel(image);
            image = image_sobel;
            break;
        }
        out_format_interface->SaveImage(output_path, image);
    }

    cout << "Successfully converted"sv << endl;

    system("pause");
}
