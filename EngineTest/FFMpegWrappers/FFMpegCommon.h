#pragma once

#define snprintf _snprintf 
#define _CRT_SECURE_NO_WARNINGS

extern "C"
{
	#include "libavcodec\avcodec.h"
	#include "libavutil\frame.h"
	#include "libavutil\imgutils.h"
	#include "libavutil\timestamp.h"
	#include "libavutil\opt.h"
	#include "libswscale/swscale.h"
	#include "libavformat/avformat.h"
	#include "libavdevice/avdevice.h"
	#include "libavutil/time.h"
}

#include <memory>
#include <functional>



typedef std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> AVFramePtr;

class CSample
{
private:
	AVFramePtr m_frame;
protected:
	CSample()
	{
	}
public:
	CSample(AVFrame* frame)
	{
		Attach(frame);
	}
	virtual ~CSample()
	{
	}
	void Attach(AVFrame* frame)
	{
		m_frame = AVFramePtr(frame, [](AVFrame* f) {  av_frame_free(&f); });
	}

	AVFrame* get() { return m_frame.get(); }
};



AVFrame* CreateVideoFrame(uint8_t *pBuffer, int width, int height, int stride, AVPixelFormat format);
AVFrame* CreateAudioFrame(uint8_t *pBuffer, int bufferSize);