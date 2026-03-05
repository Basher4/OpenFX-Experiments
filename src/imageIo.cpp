//
// Created by user on 6/26/2024.
//

#include "imageIo.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

image::Image::Image(const cv::Mat& img)
    : m_img(img), m_width(img.cols), m_height(img.rows)
{
}

void image::Image::show_blocking() const
{
    imshow("Display window", this->m_img);
    cv::waitKey(0); // Wait for a keystroke in the window
}

void image::Image::save(const std::filesystem::path& path) const
{
    imwrite(path.string(), this->m_img);
}

image::Image image::Image::load(const std::filesystem::path& path)
{
    cv::Mat img = imread(path.string(), cv::IMREAD_UNCHANGED);
    assert(!img.empty());
    assert(img.channels() == 3 || img.channels() == 4);
    if (img.channels() == 3)
    {
        cv::cvtColor(img, img, cv::COLOR_RGB2RGBA);
    }

    return Image(img);
}
