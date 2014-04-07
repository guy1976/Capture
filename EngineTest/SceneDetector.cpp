#include "stdafx.h"
#include "SceneDetector.h"

#if _DEBUG
#pragma comment(lib, "opencv_core248d.lib")
#pragma comment(lib, "opencv_highgui248d.lib")
#pragma comment(lib, "opencv_imgproc248d.lib")
#pragma comment(lib, "opencv_contrib248d.lib")
#pragma comment(lib, "opencv_calib3d248d.lib")
#pragma comment(lib, "opencv_features2d248d.lib")
#pragma comment(lib, "opencv_flann248d.lib")
#pragma comment(lib, "opencv_nonfree248d.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "libjpegd.lib")
#pragma comment(lib, "IlmImfd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "libtiffd.lib")
#else
#pragma comment(lib, "opencv_core248.lib")
#pragma comment(lib, "opencv_highgui248.lib")
#pragma comment(lib, "opencv_imgproc248.lib")
#pragma comment(lib, "opencv_contrib248.lib")
#pragma comment(lib, "opencv_calib3d248.lib")
#pragma comment(lib, "opencv_features2d248.lib")
#pragma comment(lib, "opencv_nonfree248.lib")
#pragma comment(lib, "opencv_flann248.lib")
#pragma comment(lib, "zlib.lib")
#endif

#pragma comment(lib,"comctl32.lib")


char* window_name = "Edge Map";
char* window_name2 = "Edge Map2";

CSceneDetector::CSceneDetector()
{

}


CSceneDetector::~CSceneDetector()
{
}



int const max_lowThreshold = 100;

int ratio = 3;
int kernel_size = 3;


void CSceneDetector::ProcessImage(CSample *pSample)
{
	auto time = std::chrono::steady_clock::now();

	if (time < m_lastCaptureTime + std::chrono::seconds(1))
		return;


	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	auto t0 = clock();
	cv::Mat a = PrepareAnalysisImage(pSample->get()->width, pSample->get()->height, pSample->get()->data[0]);
	cv::Mat canny = Canny(a);


	cv::Mat res;
	//if (!m_lastCapture.empty())
		// compute difference
	//	cvAbsDiff(&canny, &m_lastCapture, &res);

	double diff = 0;// m_lastCapture.empty() ? 0 : cv::matchShapes(canny, m_lastCapture, CV_CONTOURS_MATCH_I3, 0);

	auto t1 = clock();
	printf("time to  process video (%d) %lf\n", t1 - t0, diff);
	m_lastCaptureTime = time;
	
	cv::imshow(window_name, canny);
	cv::waitKey(1);

	m_lastCapture = canny;
}


cv::Mat CSceneDetector::PrepareAnalysisImage(int width, int height, unsigned char* pData)
{
	cv::Mat src(height, width,CV_8UC4, pData);
	
	cv::Mat src_gray, resized;
	cvtColor(src, src_gray, CV_RGB2GRAY);

	cv::resize(src_gray, resized, cv::Size(width / 4, height / 4), 0, 0, cv::INTER_NEAREST);

	return resized;
}

cv::Mat CSceneDetector::Canny(const cv::Mat& source)
{
	int lowThreshold = 0;

	cv::Mat detected_edges, dst;

	/// Reduce noise with a kernel 3x3
	blur(source, detected_edges, cv::Size(3, 3));

	/// Canny detector
	cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = cv::Scalar::all(0);

	source.copyTo(dst, detected_edges);
	return dst;
}