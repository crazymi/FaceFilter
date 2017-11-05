
#include <opencv2/mat.hpp>

cv::Mat nv12Tomat(unsigned char* yuv_data, unsigned int width, unsigned int height)
{
    cv::Mat nv12_img = cv::Mat(height * 3/2, width, CV_8UC1, yuv_data);
    cv::Mat rgb_img;
    cv::cvtColor(nv12_img, rgb_img, CV_YUV2RGB_NV12);

    return rgb_img;
}
