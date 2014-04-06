#pragma once
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

}
#include <memory>
#include <memory>
#include <functional>

typedef std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> AVFramePtr;

class CSample
{
private: 
	AVFramePtr m_frame;
public:
	CSample(AVFrame* pFrame) 
	{
		m_frame = AVFramePtr(pFrame, [](AVFrame* f) {  av_frame_free(&f); });
	}
	virtual ~CSample()
	{
	}

	AVFrame* get() { return m_frame.get(); }
};

class CFileWriter
{

	std::unique_ptr<AVOutputFormat, std::function<void(AVOutputFormat *)>> m_avOutputFormat;
	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> m_avFormatContext;

public:
	AVFormatContext* GetContext() { return m_avFormatContext.get(); }
	CFileWriter();
	virtual ~CFileWriter();
	void InitFile(std::string fileName);
	void Start();
};


class CEncoder
{
protected:
	std::shared_ptr<CFileWriter> m_pfileWriter;
	CEncoder(std::shared_ptr<CFileWriter> fileWriter) : m_pfileWriter(fileWriter) {}
};

class CVideoEncoder : CEncoder
{
public:
	CVideoEncoder(std::shared_ptr<CFileWriter> fileWriter) : CEncoder(fileWriter) {}
protected:

	std::unique_ptr<AVCodec, std::function<void(AVCodec *)>> m_videoCodec;
	std::unique_ptr<SwsContext, std::function<void(SwsContext *)>> m_swsContext;
	std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> m_pictureYUV;
	std::unique_ptr<AVStream, std::function<void(AVStream *)>> m_videoStream;
public:
	static AVFrame* CreateVideoFrame(BYTE *pBuffer, int width, int height, int stride, AVPixelFormat format);
	void AddVideoStream(int width, int height, int bitrate);
	void Encode(AVFrame* frame);
};


class CAudioEncoder : CEncoder
{
public:
	CAudioEncoder(std::shared_ptr<CFileWriter> fileWriter) : CEncoder(fileWriter) {}
protected:
	int m_frameIndex;

	std::unique_ptr<AVCodec, std::function<void(AVCodec *)>> m_audioCodec;
	std::unique_ptr<AVStream, std::function<void(AVStream *)>> m_audioStream;
public:
	static AVFrame* CreateAudioFrame(BYTE *pBuffer, int sampleInBytes);
	void AddAudioStream(); 
	void Encode(AVFrame* inputSample);
};



class CAudioCapture
{
	std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>> m_inputDevice;
	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> m_avFormatContext;
public:
	void Init();

	AVFrame* ReadPacket();
};