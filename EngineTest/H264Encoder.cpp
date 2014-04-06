#include "stdafx.h"
#include "H264Encoder.h"

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
		av_log_set_level(AV_LOG_DEBUG);
	}
	~CFFmpegInit()
	{
	}

private:
} s_Initiator;

//std::unique_ptr<FILE, std::function<void(FILE *)>> m_fileName;

void CVideoEncoder::AddVideoStream(int width, int height, int bitrate)
{
	if (m_videoCodec)
	{
		//nothing to do!
		return;
	}
		
	
	m_videoCodec = std::unique_ptr<AVCodec, std::function<void(AVCodec *)>>(avcodec_find_encoder(CODEC_ID_H264), [](AVCodec* ptr) { av_free(ptr); });
	if (!m_videoCodec)
	{
		fprintf(stderr, "Codec not found\n");
	}
	else
		printf("H264 codec found\n");

	auto pContext = m_pfileWriter->GetContext();
	m_videoStream = std::unique_ptr<AVStream, std::function<void(AVStream *)>>(avformat_new_stream(pContext, m_videoCodec.get()), [](AVStream* f) {  av_free(f); });
	m_videoStream->id = pContext->nb_streams - 1;

	AVCodecContext* cc = m_videoStream->codec;
	cc->bit_rate = bitrate * 1000;
	cc->width = width;                                     // resolution must be a multiple of two
	cc->height = height;
	cc->time_base.num = 1;
	cc->time_base.den = 25;	
	cc->gop_size = 10;
	cc->max_b_frames = 1;
	cc->pix_fmt = PIX_FMT_YUV420P;
	cc->codec_id = CODEC_ID_H264;
	//av_opt_set(m_videoCodecContext->priv_data, "preset", "slow", 0);

	if (pContext->oformat->flags & AVFMT_GLOBALHEADER)
		cc->flags |= CODEC_FLAG_GLOBAL_HEADER;
	// Open the encoder
	if (avcodec_open2(cc, m_videoCodec.get(), NULL) < 0)
	{
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	else
		printf("H264 codec opened\n");

	SwsContext * ctx = sws_getContext(width, height,
		AV_PIX_FMT_RGB32, cc->width, cc->height,
		AV_PIX_FMT_YUV420P, 0, 0, 0, 0);

	m_swsContext = std::unique_ptr<SwsContext, std::function<void(SwsContext *)>>(ctx, [](SwsContext* ptr) { sws_freeContext(ptr); });


	m_pictureYUV=std::unique_ptr<AVFrame, std::function<void(AVFrame *)>>(av_frame_alloc(), [](AVFrame* ptr) {av_frame_free(&ptr);});


	m_pictureYUV->format = PIX_FMT_YUV420P;
	m_pictureYUV->width = cc->width;
	m_pictureYUV->height = cc->height;
	int ret = av_image_alloc(m_pictureYUV->data, m_pictureYUV->linesize, m_pictureYUV->width, m_pictureYUV->height, cc->pix_fmt, 32);



	//m_fileName = std::unique_ptr<FILE, std::function<void(FILE *)>>(fopen("c:\\1.h264", "wb"), [](FILE* f) { fclose(f); });
}

AVFrame* CVideoEncoder::CreateVideoFrame(BYTE *pBuffer, int width, int height,int stride,AVPixelFormat format)
{
	auto frame=av_frame_alloc();
	frame->data[0] = pBuffer;
	frame->linesize[0] = stride;
	frame->width = width;
	frame->height = height;
	frame->format = format;
	return frame;
}
void CVideoEncoder::Encode(AVFrame* inputFrame)
{

	sws_scale(m_swsContext.get(), inputFrame->data, inputFrame->linesize, 0, inputFrame->height, m_pictureYUV->data, m_pictureYUV->linesize);

	m_pictureYUV->pts = inputFrame->pts;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	AVCodecContext *pContext = m_videoStream->codec;
	int got_output;
	int out_size = avcodec_encode_video2(pContext, &pkt, m_pictureYUV.get(), &got_output);
	if (got_output)
	{
		pkt.stream_index = m_videoStream->index;

		pkt.pts = av_rescale_q_rnd(pkt.pts, pContext->time_base, m_videoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, pContext->time_base, m_videoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, pContext->time_base, m_videoStream->time_base);

		printf("H264 wrote %d bytes \n", pkt.size);
		auto pContext = m_pfileWriter->GetContext();
		av_interleaved_write_frame(pContext, &pkt);
		//fwrite(pkt.data, 1, pkt.size, m_fileName.get());
	}
	av_free_packet(&pkt);

}


CFileWriter::CFileWriter()
{
}
//D:\Users\Gjacubovski>ffmpeg - f gdigrab - i desktop - vcodec libx264 - preset ultrafast - b:v 200k - f mp4 - movflags  frag_keyframe  out.mp4

CFileWriter::~CFileWriter()
{
	if (m_avFormatContext)
	{
		av_write_trailer(m_avFormatContext.get());
		if (m_avFormatContext->pb)
			avio_close(m_avFormatContext->pb);
	}
}



void CFileWriter::Start()
{
	av_dump_format(m_avFormatContext.get(), 0, m_avFormatContext->filename, 1);

	/* open the output file, if needed */
	avio_open(&m_avFormatContext->pb, m_avFormatContext->filename, AVIO_FLAG_WRITE);
	avformat_write_header(m_avFormatContext.get(), NULL);
}








void CFileWriter::InitFile(std::string fileName)
{
	AVFormatContext *oc=NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, fileName.c_str());
	if (!oc)
	{
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, NULL, "mp4", fileName.c_str());

	}
	if (!oc) 
		return;

	AVOutputFormat* pFormat = oc->oformat;

	m_avFormatContext = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>>(oc, [](AVFormatContext* f) {  av_free(f); });
	int x=av_opt_set(m_avFormatContext->priv_data, "movflags", "+frag_keyframe", 0);

}



void CAudioCapture::Init()
{
	m_inputDevice = std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>>(av_find_input_format("dshow"), [](AVInputFormat* f) {  av_free(f); });
	AVFormatContext *formatC = avformat_alloc_context();
	m_avFormatContext = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>>(formatC, [](AVFormatContext* f) {  av_free(f); });
	AVDictionary* options = NULL;
	avformat_open_input(&formatC, "audio=Microphone (Realtek High Defini", m_inputDevice.get(), NULL);

	for (int i = 0; i < formatC->nb_streams; i++)
	{
		auto v = formatC->streams[i];
	}
}

AVFrame* CAudioCapture::ReadPacket()
{
	AVFrame* audioSample = NULL;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	if (0 == av_read_frame(m_avFormatContext.get(), &pkt))
	{
		audioSample = CAudioEncoder::CreateAudioFrame(pkt.data, pkt.size);
		audioSample->pts = pkt.pts;
	}
	av_free_packet(&pkt);

	return audioSample;
}






void CAudioEncoder::AddAudioStream()
{
	if (m_audioCodec)
	{
		//nothing to do!
		return;
	}


	m_audioCodec = std::unique_ptr<AVCodec, std::function<void(AVCodec *)>>(avcodec_find_encoder(CODEC_ID_AAC), [](AVCodec* ptr) { av_free(ptr); });
	if (!m_audioCodec)
	{
		fprintf(stderr, "Codec not found\n");
	}
	else
		printf("AAC codec found\n");

	auto pContext = m_pfileWriter->GetContext();
	m_audioStream = std::unique_ptr<AVStream, std::function<void(AVStream *)>>(avformat_new_stream(pContext, m_audioCodec.get()), [](AVStream* f) {  av_free(f); });
	m_audioStream->id = pContext->nb_streams - 1;

	AVCodecContext* cc = m_audioStream->codec;
	cc->sample_fmt = AV_SAMPLE_FMT_S16;
	cc->bit_rate = 64000;
	cc->sample_rate = 44100;
	cc->channels = 2;


	if (pContext->oformat->flags & AVFMT_GLOBALHEADER)
		cc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	// Open the encoder
	if (avcodec_open2(cc, m_audioCodec.get(), NULL) < 0)
	{
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	else
		printf("H264 codec opened\n");




}

AVFrame* CAudioEncoder::CreateAudioFrame(BYTE *pBuffer, int bufferSize)
{
	auto frame=av_frame_alloc();
	frame->nb_samples = bufferSize;
	avcodec_fill_audio_frame(frame, 2, AVSampleFormat::AV_SAMPLE_FMT_S16, pBuffer, bufferSize , 1);

	return frame;
}


void CAudioEncoder::Encode(AVFrame* inputSample)
{
	AVCodecContext *pContext = m_audioStream->codec;

	

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	int got_output;

	int out_size = avcodec_encode_audio2(pContext, &pkt, inputSample, &got_output);
	if (got_output)
	{
		pkt.stream_index = m_audioStream->index;

		pkt.pts = av_rescale_q_rnd(pkt.pts, pContext->time_base, m_audioStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, pContext->time_base, m_audioStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, pContext->time_base, m_audioStream->time_base);
		pkt.pos = -1;

		printf("AAC wrote %d bytes \n", pkt.size);
		auto pContext = m_pfileWriter->GetContext();
		av_interleaved_write_frame(pContext, &pkt);
		//fwrite(pkt.data, 1, pkt.size, m_fileName.get());
	}
	av_free_packet(&pkt);

}