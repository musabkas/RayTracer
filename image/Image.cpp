#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Image.hpp"
#include "../utilities/RGBColor.hpp"
#include "../world/ViewPlane.hpp"

Image::Image(int hres, int vres) {
    this->hres = hres;
    this->vres = vres;
    this->colors = new RGBColor* [hres];
    for (int i = 0; i < hres; i++) this->colors[i] = new RGBColor[vres];
}

Image::Image(const ViewPlane &vp) : Image(vp.get_hres(), vp.get_vres()){

}

Image::~Image(){
    for (int i = 0; i < hres; i++) {
        delete[] colors[i];
    }
    delete[] colors;
}

void Image::set_pixel(int x, int y, const RGBColor& color) {
    colors[x][y] = color;
}

void Image::write_png(std::string path) const {
    const int channels = 3;
    unsigned char* byte_data = new unsigned char [hres * vres * channels];
    for (int y = 0; y < vres; ++y) {
        for (int x = 0; x < hres; ++x) {
            RGBColor col = colors[x][y]; 
            int index = (y * hres + x) * channels;
            byte_data[index + 0] = col.r * 255.0;
            byte_data[index + 1] = col.g * 255.0;
            byte_data[index + 2] = col.b * 255.0;
        }
    }

    stbi_write_png(path.c_str(), hres, vres, channels, byte_data, hres * channels);
    delete[] byte_data;
}