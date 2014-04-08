#include "FileWriter.h"
#include "AudioEncoder.h"

void CAudioEncoder::AddAudioStream()
{
	if (m_audioCodec)
	{
		//nothing to do!
		return;
	}


	m_audioCodec = std::unique_ptr < AVCodec, std::function<void(AVCodec *) >> (avcodec_find_encoder(CODEC_ID_AAC), [](AVCodec* ptr) { av_free(ptr); });
	if (!m_audioCodec)
	{
		fprintf(stderr, "Codec not found\n");
	}
	else
		printf("AAC codec found\n");

	auto pContext = m_pfileWriter->GetContext();
	m_audioStream = std::unique_ptr < AVStream, std::function<void(AVStream *) >>(avformat_new_stream(pContext, m_audioCodec.get()), [](AVStream* f) {  av_free(f); });
	m_audioStream->id = pContext->nb_streams - 1;

	AVCodecContext* cc = m_audioStream->codec;
	cc->sample_fmt = AV_SAMPLE_FMT_S16;
	cc->bit_rate = 64000;
	cc->sample_rate = 44100;
	cc->channel_layout = AV_CH_LAYOUT_STEREO;
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

	m_frame_size = m_audioStream->codec->frame_size;

	m_fifo =av_audio_fifo_alloc(AV_SAMPLE_FMT_S16,2, 1);


}
/**
* Initialize one input frame for writing to the output file.
* The frame will be exactly frame_size samples large.
*/
static int init_output_frame(AVFrame **frame,
	AVCodecContext *output_codec_context,
	int frame_size)
{
	int error;

	/** Create a new frame to store the audio samples. */
	if (!(*frame = av_frame_alloc())) {
		fprintf(stderr, "Could not allocate output frame\n");
		return AVERROR_EXIT;
	}

	/**
	* Set the frame's parameters, especially its size and format.
	* av_frame_get_buffer needs this to allocate memory for the
	* audio samples of the frame.
	* Default channel layouts based on the number of channels
	* are assumed for simplicity.
	*/
	(*frame)->nb_samples = frame_size;
	(*frame)->channel_layout = output_codec_context->channel_layout;
	(*frame)->format = output_codec_context->sample_fmt;
	(*frame)->sample_rate = output_codec_context->sample_rate;

	/**
	* Allocate the samples of the created frame. This call will make
	* sure that the audio frame can hold as many samples as specified.
	*/
	if ((error = av_frame_get_buffer(*frame, 0)) < 0) 
	{
		av_frame_free(frame);
		return error;
	}

	return 0;
}


void CAudioEncoder::Encode(AVFrame* inputSample)
{
	AVCodecContext *pContext = m_audioStream->codec;


	int error;

	if ((error = av_audio_fifo_realloc(m_fifo, av_audio_fifo_size(m_fifo) + m_frame_size)) < 0)
	{
		fprintf(stderr, "Could not reallocate FIFO\n");
		return ;
	}

	/** Store the new samples in the FIFO buffer. */
	if (av_audio_fifo_write(m_fifo, (void **)inputSample->data, inputSample->nb_samples) < m_frame_size)
	{
		fprintf(stderr, "Could not write data to FIFO\n");
		return ;
	}
	do
	{
		AVFrame *output_frame;
		int frame_size = FFMIN(av_audio_fifo_size(m_fifo), m_frame_size);
		if (frame_size < m_frame_size)
			break;

		/** Initialize temporary storage for one output frame. */
		if (init_output_frame(&output_frame, m_audioStream->codec, frame_size))
			return;

		if (av_audio_fifo_read(m_fifo, (void **)output_frame->data, frame_size) < frame_size)
		{
			fprintf(stderr, "Could not read data from FIFO\n");
			return;
		}


		AVPacket pkt;
		av_init_packet(&pkt);
		pkt.data = NULL;    // packet data will be allocated by the encoder
		pkt.size = 0;
		int got_output;

		int out_size = avcodec_encode_audio2(pContext, &pkt, output_frame, &got_output);
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
	} while (1);


}