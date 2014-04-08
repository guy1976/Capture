#include "FileWriter.h"
#include "VideoEncoder.h"

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
	cc->time_base.den = 30;
	cc->gop_size = 100;
	cc->max_b_frames = 0;
	cc->pix_fmt = PIX_FMT_YUV420P;
	cc->codec_id = CODEC_ID_H264;
	av_opt_set(cc->priv_data, "preset", "ultrafast", 0);

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


	m_pictureYUV = std::unique_ptr<AVFrame, std::function<void(AVFrame *)>>(av_frame_alloc(), [](AVFrame* ptr) {av_frame_free(&ptr); });


	m_pictureYUV->format = PIX_FMT_YUV420P;
	m_pictureYUV->width = cc->width;
	m_pictureYUV->height = cc->height;
	int ret = av_image_alloc(m_pictureYUV->data, m_pictureYUV->linesize, m_pictureYUV->width, m_pictureYUV->height, cc->pix_fmt, 32);



	//m_fileName = std::unique_ptr<FILE, std::function<void(FILE *)>>(fopen("c:\\1.h264", "wb"), [](FILE* f) { fclose(f); });
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
	if (got_output && pkt.size>0)
	{
		pkt.stream_index = m_videoStream->index;

		pkt.pts = av_rescale_q_rnd(pkt.pts, pContext->time_base, m_videoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, pContext->time_base, m_videoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, pContext->time_base, m_videoStream->time_base);

		auto pContext = m_pfileWriter->GetContext();

		auto size = pkt.size;
		int val = av_write_frame(pContext, &pkt);
		//int val = av_interleaved_write_frame(pContext, &pkt);
	//	printf("H264 wrote %d bytes (%d)\n", size, val);
		//fwrite(pkt.data, 1, pkt.size, m_fileName.get());
	}
	av_free_packet(&pkt);

}