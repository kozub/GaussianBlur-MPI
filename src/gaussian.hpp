#ifndef _gausian_hu_
#define _gausian_hu_

#include <opencv2/highgui/highgui.hpp>

class GaussianBlur
{
private:
	cv::Mat * source;
	cv::Mat * result;

	cv::Vec3b processPixel(int i, int j);
public:
	GaussianBlur(cv::Mat * input);
	~GaussianBlur();

	cv::Mat* getResult();
	void process();
};



#endif //_gausian_hu_
