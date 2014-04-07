#include "FFMpegCommon.h"


#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "avcodec.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "swscale.lib")
#pragma comment (lib, "avdevice.lib")


class  CFFmpegInit
{
public:
	CFFmpegInit()
	{
		av_register_all();
		avcodec_register_all();
		avdevice_register_all();
		//av_log_set_level(AV_LOG_DEBUG);
	}
	~CFFmpegInit()
	{
	}

private:
} s_Initiator;


AVFrame* CreateAudioFrame(uint8_t *pBuffer, int bufferSize)
{

	auto frame = av_frame_alloc();
	frame->nb_samples = bufferSize/4;
	avcodec_fill_audio_frame(frame, 2, AVSampleFormat::AV_SAMPLE_FMT_S16, pBuffer, bufferSize, 1);

	return frame;
}

AVFrame* CreateVideoFrame(uint8_t*pBuffer, int width, int height, int stride, AVPixelFormat format)
{
	auto frame = av_frame_alloc();
	frame->data[0] = pBuffer;
	frame->linesize[0] = stride;
	frame->width = width;
	frame->height = height;
	frame->format = format;
	return frame;
}