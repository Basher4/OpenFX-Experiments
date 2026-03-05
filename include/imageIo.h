#pragma once

#include "imageEffectImageRef.h"

#include <cstdint>
#include <filesystem>
#include <opencv2/core/mat.hpp>

namespace image {

struct Pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

class Image {
public:
    // Skip the other bagilion of constructors
    explicit Image(const cv::Mat& img);

    [[nodiscard]] uint32_t width() const { return this->m_width; }
    [[nodiscard]] uint32_t height() const { return this->m_height; }
    [[nodiscard]] cv::Mat& mat() { return this->m_img; }
    [[nodiscard]] Pixel* data() const { return reinterpret_cast<Pixel*>(this->m_img.data); }
    [[nodiscard]] Pixel* at(const int32_t x, const int32_t y) { return &this->data()[y * this->width() + x]; }

    void show_blocking() const;

    ImageEffectImageRef as_ofx_ref() const { return ImageEffectImageRef(*this); }

    void save(const std::filesystem::path& path) const;

    // FIXME: Does this function shallow copy Image? What about destructor?
    static Image load(const std::filesystem::path& path);

private:
    cv::Mat m_img;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

}