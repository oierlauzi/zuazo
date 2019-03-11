#pragma once

extern "C"{
	#include <libavcodec/avcodec.h>
}

namespace Zuazo::Utils{

class Codec{
public:
	constexpr Codec();
	constexpr Codec(AVCodecID codec);
	Codec(const Codec& other)=default;
	~Codec()=default;

	constexpr int operator==(const Codec& other) const;
	constexpr int operator!=(const Codec& other) const;
	constexpr int operator<(const Codec& other) const;
	constexpr int operator>(const Codec& other) const;
	constexpr int operator<=(const Codec& other) const;
	constexpr int operator>=(const Codec& other) const;

	constexpr AVCodecID toAVCodecID() const;
private:
	AVCodecID	codecId;
};

constexpr Codec::Codec() :
		codecId(AV_CODEC_ID_NONE)
{
}
constexpr Codec::Codec(AVCodecID codec) :
		codecId(codec)
{
}

constexpr int Codec::operator==(const Codec& other) const{
	return codecId==other.codecId;
}
constexpr int Codec::operator!=(const Codec& other) const{
	return codecId!=other.codecId;
}
constexpr int Codec::operator<(const Codec& other) const{
	return codecId<other.codecId;
}
constexpr int Codec::operator>(const Codec& other) const{
	return codecId>other.codecId;
}
constexpr int Codec::operator<=(const Codec& other) const{
	return codecId<=other.codecId;
}
constexpr int Codec::operator>=(const Codec& other) const{
	return codecId>=other.codecId;
}

constexpr AVCodecID Codec::toAVCodecID() const{
	return codecId;
}

namespace Codecs{

constexpr Codec NONE	=AV_CODEC_ID_NONE;
constexpr Codec MPEG1VIDEO	=AV_CODEC_ID_MPEG1VIDEO;
constexpr Codec MPEG2VIDEO	=AV_CODEC_ID_MPEG2VIDEO; ///< preferred ID for MPEG-1/2 video decoding
constexpr Codec H261	=AV_CODEC_ID_H261;
constexpr Codec H263	=AV_CODEC_ID_H263;
constexpr Codec RV10	=AV_CODEC_ID_RV10;
constexpr Codec RV20	=AV_CODEC_ID_RV20;
constexpr Codec MJPEG	=AV_CODEC_ID_MJPEG;
constexpr Codec MJPEGB	=AV_CODEC_ID_MJPEGB;
constexpr Codec LJPEG	=AV_CODEC_ID_LJPEG;
constexpr Codec SP5X	=AV_CODEC_ID_SP5X;
constexpr Codec JPEGLS	=AV_CODEC_ID_JPEGLS;
constexpr Codec MPEG4	=AV_CODEC_ID_MPEG4;
constexpr Codec RAWVIDEO	=AV_CODEC_ID_RAWVIDEO;
constexpr Codec MSMPEG4V1	=AV_CODEC_ID_MSMPEG4V1;
constexpr Codec MSMPEG4V2	=AV_CODEC_ID_MSMPEG4V2;
constexpr Codec MSMPEG4V3	=AV_CODEC_ID_MSMPEG4V3;
constexpr Codec WMV1	=AV_CODEC_ID_WMV1;
constexpr Codec WMV2	=AV_CODEC_ID_WMV2;
constexpr Codec H263P	=AV_CODEC_ID_H263P;
constexpr Codec H263I	=AV_CODEC_ID_H263I;
constexpr Codec FLV1	=AV_CODEC_ID_FLV1;
constexpr Codec SVQ1	=AV_CODEC_ID_SVQ1;
constexpr Codec SVQ3	=AV_CODEC_ID_SVQ3;
constexpr Codec DVVIDEO	=AV_CODEC_ID_DVVIDEO;
constexpr Codec HUFFYUV	=AV_CODEC_ID_HUFFYUV;
constexpr Codec CYUV	=AV_CODEC_ID_CYUV;
constexpr Codec H264	=AV_CODEC_ID_H264;
constexpr Codec INDEO3	=AV_CODEC_ID_INDEO3;
constexpr Codec VP3	=AV_CODEC_ID_VP3;
constexpr Codec THEORA	=AV_CODEC_ID_THEORA;
constexpr Codec ASV1	=AV_CODEC_ID_ASV1;
constexpr Codec ASV2	=AV_CODEC_ID_ASV2;
constexpr Codec FFV1	=AV_CODEC_ID_FFV1;
constexpr Codec V4XM	=AV_CODEC_ID_4XM;
constexpr Codec VCR1	=AV_CODEC_ID_VCR1;
constexpr Codec CLJR	=AV_CODEC_ID_CLJR;
constexpr Codec MDEC	=AV_CODEC_ID_MDEC;
constexpr Codec ROQ	=AV_CODEC_ID_ROQ;
constexpr Codec INTERPLAY_VIDEO	=AV_CODEC_ID_INTERPLAY_VIDEO;
constexpr Codec XAN_WC3	=AV_CODEC_ID_XAN_WC3;
constexpr Codec XAN_WC4	=AV_CODEC_ID_XAN_WC4;
constexpr Codec RPZA	=AV_CODEC_ID_RPZA;
constexpr Codec CINEPAK	=AV_CODEC_ID_CINEPAK;
constexpr Codec WS_VQA	=AV_CODEC_ID_WS_VQA;
constexpr Codec MSRLE	=AV_CODEC_ID_MSRLE;
constexpr Codec MSVIDEO1	=AV_CODEC_ID_MSVIDEO1;
constexpr Codec IDCIN	=AV_CODEC_ID_IDCIN;
constexpr Codec V8BPS	=AV_CODEC_ID_8BPS;
constexpr Codec SMC	=AV_CODEC_ID_SMC;
constexpr Codec FLIC	=AV_CODEC_ID_FLIC;
constexpr Codec TRUEMOTION1	=AV_CODEC_ID_TRUEMOTION1;
constexpr Codec VMDVIDEO	=AV_CODEC_ID_VMDVIDEO;
constexpr Codec MSZH	=AV_CODEC_ID_MSZH;
constexpr Codec ZLIB	=AV_CODEC_ID_ZLIB;
constexpr Codec QTRLE	=AV_CODEC_ID_QTRLE;
constexpr Codec TSCC	=AV_CODEC_ID_TSCC;
constexpr Codec ULTI	=AV_CODEC_ID_ULTI;
constexpr Codec QDRAW	=AV_CODEC_ID_QDRAW;
constexpr Codec VIXL	=AV_CODEC_ID_VIXL;
constexpr Codec QPEG	=AV_CODEC_ID_QPEG;
constexpr Codec PNG	=AV_CODEC_ID_PNG;
constexpr Codec PPM	=AV_CODEC_ID_PPM;
constexpr Codec PBM	=AV_CODEC_ID_PBM;
constexpr Codec PGM	=AV_CODEC_ID_PGM;
constexpr Codec PGMYUV	=AV_CODEC_ID_PGMYUV;
constexpr Codec PAM	=AV_CODEC_ID_PAM;
constexpr Codec FFVHUFF	=AV_CODEC_ID_FFVHUFF;
constexpr Codec RV30	=AV_CODEC_ID_RV30;
constexpr Codec RV40	=AV_CODEC_ID_RV40;
constexpr Codec VC1	=AV_CODEC_ID_VC1;
constexpr Codec WMV3	=AV_CODEC_ID_WMV3;
constexpr Codec LOCO	=AV_CODEC_ID_LOCO;
constexpr Codec WNV1	=AV_CODEC_ID_WNV1;
constexpr Codec AASC	=AV_CODEC_ID_AASC;
constexpr Codec INDEO2	=AV_CODEC_ID_INDEO2;
constexpr Codec FRAPS	=AV_CODEC_ID_FRAPS;
constexpr Codec TRUEMOTION2	=AV_CODEC_ID_TRUEMOTION2;
constexpr Codec BMP	=AV_CODEC_ID_BMP;
constexpr Codec CSCD	=AV_CODEC_ID_CSCD;
constexpr Codec MMVIDEO	=AV_CODEC_ID_MMVIDEO;
constexpr Codec ZMBV	=AV_CODEC_ID_ZMBV;
constexpr Codec AVS	=AV_CODEC_ID_AVS;
constexpr Codec SMACKVIDEO	=AV_CODEC_ID_SMACKVIDEO;
constexpr Codec NUV	=AV_CODEC_ID_NUV;
constexpr Codec KMVC	=AV_CODEC_ID_KMVC;
constexpr Codec FLASHSV	=AV_CODEC_ID_FLASHSV;
constexpr Codec CAVS	=AV_CODEC_ID_CAVS;
constexpr Codec JPEG2000	=AV_CODEC_ID_JPEG2000;
constexpr Codec VMNC	=AV_CODEC_ID_VMNC;
constexpr Codec VP5	=AV_CODEC_ID_VP5;
constexpr Codec VP6	=AV_CODEC_ID_VP6;
constexpr Codec VP6F	=AV_CODEC_ID_VP6F;
constexpr Codec TARGA	=AV_CODEC_ID_TARGA;
constexpr Codec DSICINVIDEO	=AV_CODEC_ID_DSICINVIDEO;
constexpr Codec TIERTEXSEQVIDEO	=AV_CODEC_ID_TIERTEXSEQVIDEO;
constexpr Codec TIFF	=AV_CODEC_ID_TIFF;
constexpr Codec GIF	=AV_CODEC_ID_GIF;
constexpr Codec DXA	=AV_CODEC_ID_DXA;
constexpr Codec DNXHD	=AV_CODEC_ID_DNXHD;
constexpr Codec THP	=AV_CODEC_ID_THP;
constexpr Codec SGI	=AV_CODEC_ID_SGI;
constexpr Codec C93	=AV_CODEC_ID_C93;
constexpr Codec BETHSOFTVID	=AV_CODEC_ID_BETHSOFTVID;
constexpr Codec PTX	=AV_CODEC_ID_PTX;
constexpr Codec TXD	=AV_CODEC_ID_TXD;
constexpr Codec VP6A	=AV_CODEC_ID_VP6A;
constexpr Codec AMV	=AV_CODEC_ID_AMV;
constexpr Codec VB	=AV_CODEC_ID_VB;
constexpr Codec PCX	=AV_CODEC_ID_PCX;
constexpr Codec SUNRAST	=AV_CODEC_ID_SUNRAST;
constexpr Codec INDEO4	=AV_CODEC_ID_INDEO4;
constexpr Codec INDEO5	=AV_CODEC_ID_INDEO5;
constexpr Codec MIMIC	=AV_CODEC_ID_MIMIC;
constexpr Codec RL2	=AV_CODEC_ID_RL2;
constexpr Codec ESCAPE124	=AV_CODEC_ID_ESCAPE124;
constexpr Codec DIRAC	=AV_CODEC_ID_DIRAC;
constexpr Codec BFI	=AV_CODEC_ID_BFI;
constexpr Codec CMV	=AV_CODEC_ID_CMV;
constexpr Codec MOTIONPIXELS	=AV_CODEC_ID_MOTIONPIXELS;
constexpr Codec TGV	=AV_CODEC_ID_TGV;
constexpr Codec TGQ	=AV_CODEC_ID_TGQ;
constexpr Codec TQI	=AV_CODEC_ID_TQI;
constexpr Codec AURA	=AV_CODEC_ID_AURA;
constexpr Codec AURA2	=AV_CODEC_ID_AURA2;
constexpr Codec V210X	=AV_CODEC_ID_V210X;
constexpr Codec TMV	=AV_CODEC_ID_TMV;
constexpr Codec V210	=AV_CODEC_ID_V210;
constexpr Codec DPX	=AV_CODEC_ID_DPX;
constexpr Codec MAD	=AV_CODEC_ID_MAD;
constexpr Codec FRWU	=AV_CODEC_ID_FRWU;
constexpr Codec FLASHSV2	=AV_CODEC_ID_FLASHSV2;
constexpr Codec CDGRAPHICS	=AV_CODEC_ID_CDGRAPHICS;
constexpr Codec R210	=AV_CODEC_ID_R210;
constexpr Codec ANM	=AV_CODEC_ID_ANM;
constexpr Codec BINKVIDEO	=AV_CODEC_ID_BINKVIDEO;
constexpr Codec IFF_ILBM	=AV_CODEC_ID_IFF_ILBM;
constexpr Codec IFF_BYTERUN1 =AV_CODEC_ID_IFF_BYTERUN1;
constexpr Codec KGV1	=AV_CODEC_ID_KGV1;
constexpr Codec YOP	=AV_CODEC_ID_YOP;
constexpr Codec VP8	=AV_CODEC_ID_VP8;
constexpr Codec PICTOR	=AV_CODEC_ID_PICTOR;
constexpr Codec ANSI	=AV_CODEC_ID_ANSI;
constexpr Codec A64_MULTI	=AV_CODEC_ID_A64_MULTI;
constexpr Codec A64_MULTI5	=AV_CODEC_ID_A64_MULTI5;
constexpr Codec R10K	=AV_CODEC_ID_R10K;
constexpr Codec MXPEG	=AV_CODEC_ID_MXPEG;
constexpr Codec LAGARITH	=AV_CODEC_ID_LAGARITH;
constexpr Codec PRORES	=AV_CODEC_ID_PRORES;
constexpr Codec JV	=AV_CODEC_ID_JV;
constexpr Codec DFA	=AV_CODEC_ID_DFA;
constexpr Codec WMV3IMAGE	=AV_CODEC_ID_WMV3IMAGE;
constexpr Codec VC1IMAGE	=AV_CODEC_ID_VC1IMAGE;
constexpr Codec UTVIDEO	=AV_CODEC_ID_UTVIDEO;
constexpr Codec BMV_VIDEO	=AV_CODEC_ID_BMV_VIDEO;
constexpr Codec VBLE	=AV_CODEC_ID_VBLE;
constexpr Codec DXTORY	=AV_CODEC_ID_DXTORY;
constexpr Codec V410	=AV_CODEC_ID_V410;
constexpr Codec XWD	=AV_CODEC_ID_XWD;
constexpr Codec CDXL	=AV_CODEC_ID_CDXL;
constexpr Codec XBM	=AV_CODEC_ID_XBM;
constexpr Codec ZEROCODEC	=AV_CODEC_ID_ZEROCODEC;
constexpr Codec MSS1	=AV_CODEC_ID_MSS1;
constexpr Codec MSA1	=AV_CODEC_ID_MSA1;
constexpr Codec TSCC2	=AV_CODEC_ID_TSCC2;
constexpr Codec MTS2	=AV_CODEC_ID_MTS2;
constexpr Codec CLLC	=AV_CODEC_ID_CLLC;
constexpr Codec MSS2	=AV_CODEC_ID_MSS2;
constexpr Codec VP9	=AV_CODEC_ID_VP9;
constexpr Codec AIC	=AV_CODEC_ID_AIC;
constexpr Codec ESCAPE130	=AV_CODEC_ID_ESCAPE130;
constexpr Codec G2M	=AV_CODEC_ID_G2M;
constexpr Codec WEBP	=AV_CODEC_ID_WEBP;
constexpr Codec HNM4_VIDEO	=AV_CODEC_ID_HNM4_VIDEO;
constexpr Codec HEVC	=AV_CODEC_ID_HEVC;
constexpr Codec H265	=AV_CODEC_ID_H265;
constexpr Codec FIC	=AV_CODEC_ID_FIC;
constexpr Codec ALIAS_PIX	=AV_CODEC_ID_ALIAS_PIX;
constexpr Codec BRENDER_PIX	=AV_CODEC_ID_BRENDER_PIX;
constexpr Codec PAF_VIDEO	=AV_CODEC_ID_PAF_VIDEO;
constexpr Codec EXR	=AV_CODEC_ID_EXR;
constexpr Codec VP7	=AV_CODEC_ID_VP7;
constexpr Codec SANM	=AV_CODEC_ID_SANM;
constexpr Codec SGIRLE	=AV_CODEC_ID_SGIRLE;
constexpr Codec MVC1	=AV_CODEC_ID_MVC1;
constexpr Codec MVC2	=AV_CODEC_ID_MVC2;
constexpr Codec HQX	=AV_CODEC_ID_HQX;
constexpr Codec TDSC	=AV_CODEC_ID_TDSC;
constexpr Codec HQ_HQA	=AV_CODEC_ID_HQ_HQA;
constexpr Codec HAP	=AV_CODEC_ID_HAP;
constexpr Codec DDS	=AV_CODEC_ID_DDS;
constexpr Codec DXV	=AV_CODEC_ID_DXV;
constexpr Codec SCREENPRESSO	=AV_CODEC_ID_SCREENPRESSO;
constexpr Codec RSCC	=AV_CODEC_ID_RSCC;
constexpr Codec AVS2	=AV_CODEC_ID_AVS2;

constexpr Codec Y41P	=AV_CODEC_ID_Y41P;
constexpr Codec AVRP	=AV_CODEC_ID_AVRP;
constexpr Codec V012V	=AV_CODEC_ID_012V;
constexpr Codec AVUI	=AV_CODEC_ID_AVUI;
constexpr Codec AYUV	=AV_CODEC_ID_AYUV;
constexpr Codec TARGA_Y216	=AV_CODEC_ID_TARGA_Y216;
constexpr Codec V308	=AV_CODEC_ID_V308;
constexpr Codec V408	=AV_CODEC_ID_V408;
constexpr Codec YUV4	=AV_CODEC_ID_YUV4;
constexpr Codec AVRN	=AV_CODEC_ID_AVRN;
constexpr Codec CPIA	=AV_CODEC_ID_CPIA;
constexpr Codec XFACE	=AV_CODEC_ID_XFACE;
constexpr Codec SNOW	=AV_CODEC_ID_SNOW;
constexpr Codec SMVJPEG	=AV_CODEC_ID_SMVJPEG;
constexpr Codec APNG	=AV_CODEC_ID_APNG;
constexpr Codec DAALA	=AV_CODEC_ID_DAALA;
constexpr Codec CFHD	=AV_CODEC_ID_CFHD;
constexpr Codec TRUEMOTION2RT	=AV_CODEC_ID_TRUEMOTION2RT;
constexpr Codec M101	=AV_CODEC_ID_M101;
constexpr Codec MAGICYUV	=AV_CODEC_ID_MAGICYUV;
constexpr Codec SHEERVIDEO	=AV_CODEC_ID_SHEERVIDEO;
constexpr Codec YLC	=AV_CODEC_ID_YLC;
constexpr Codec PSD	=AV_CODEC_ID_PSD;
constexpr Codec PIXLET	=AV_CODEC_ID_PIXLET;
constexpr Codec SPEEDHQ	=AV_CODEC_ID_SPEEDHQ;
constexpr Codec FMVC	=AV_CODEC_ID_FMVC;
constexpr Codec SCPR	=AV_CODEC_ID_SCPR;
constexpr Codec CLEARVIDEO	=AV_CODEC_ID_CLEARVIDEO;
constexpr Codec XPM	=AV_CODEC_ID_XPM;
constexpr Codec AV1	=AV_CODEC_ID_AV1;
constexpr Codec BITPACKED	=AV_CODEC_ID_BITPACKED;
constexpr Codec MSCC	=AV_CODEC_ID_MSCC;
constexpr Codec SRGC	=AV_CODEC_ID_SRGC;
constexpr Codec SVG	=AV_CODEC_ID_SVG;
constexpr Codec GDV	=AV_CODEC_ID_GDV;
constexpr Codec FITS	=AV_CODEC_ID_FITS;
constexpr Codec IMM4	=AV_CODEC_ID_IMM4;
constexpr Codec PROSUMER	=AV_CODEC_ID_PROSUMER;
constexpr Codec MWSC	=AV_CODEC_ID_MWSC;
constexpr Codec WCMV	=AV_CODEC_ID_WCMV;
constexpr Codec RASC	=AV_CODEC_ID_RASC;

/* various PCM "codecs" */
constexpr Codec FIRST_AUDIO	=AV_CODEC_ID_FIRST_AUDIO;    ///< A dummy id pointing at the start of audio codecs
constexpr Codec PCM_S16LE	=AV_CODEC_ID_PCM_S16LE;
constexpr Codec PCM_S16BE	=AV_CODEC_ID_PCM_S16BE;
constexpr Codec PCM_U16LE	=AV_CODEC_ID_PCM_U16LE;
constexpr Codec PCM_U16BE	=AV_CODEC_ID_PCM_U16BE;
constexpr Codec PCM_S8	=AV_CODEC_ID_PCM_S8;
constexpr Codec PCM_U8	=AV_CODEC_ID_PCM_U8;
constexpr Codec PCM_MULAW	=AV_CODEC_ID_PCM_MULAW;
constexpr Codec PCM_ALAW	=AV_CODEC_ID_PCM_ALAW;
constexpr Codec PCM_S32LE	=AV_CODEC_ID_PCM_S32LE;
constexpr Codec PCM_S32BE	=AV_CODEC_ID_PCM_S32BE;
constexpr Codec PCM_U32LE	=AV_CODEC_ID_PCM_U32LE;
constexpr Codec PCM_U32BE	=AV_CODEC_ID_PCM_U32BE;
constexpr Codec PCM_S24LE	=AV_CODEC_ID_PCM_S24LE;
constexpr Codec PCM_S24BE	=AV_CODEC_ID_PCM_S24BE;
constexpr Codec PCM_U24LE	=AV_CODEC_ID_PCM_U24LE;
constexpr Codec PCM_U24BE	=AV_CODEC_ID_PCM_U24BE;
constexpr Codec PCM_S24DAUD	=AV_CODEC_ID_PCM_S24DAUD;
constexpr Codec PCM_ZORK	=AV_CODEC_ID_PCM_ZORK;
constexpr Codec PCM_S16LE_PLANAR	=AV_CODEC_ID_PCM_S16LE_PLANAR;
constexpr Codec PCM_DVD	=AV_CODEC_ID_PCM_DVD;
constexpr Codec PCM_F32BE	=AV_CODEC_ID_PCM_F32BE;
constexpr Codec PCM_F32LE	=AV_CODEC_ID_PCM_F32LE;
constexpr Codec PCM_F64BE	=AV_CODEC_ID_PCM_F64BE;
constexpr Codec PCM_F64LE	=AV_CODEC_ID_PCM_F64LE;
constexpr Codec PCM_BLURAY	=AV_CODEC_ID_PCM_BLURAY;
constexpr Codec PCM_LXF	=AV_CODEC_ID_PCM_LXF;
constexpr Codec S302M	=AV_CODEC_ID_S302M;
constexpr Codec PCM_S8_PLANAR	=AV_CODEC_ID_PCM_S8_PLANAR;
constexpr Codec PCM_S24LE_PLANAR	=AV_CODEC_ID_PCM_S24LE_PLANAR;
constexpr Codec PCM_S32LE_PLANAR	=AV_CODEC_ID_PCM_S32LE_PLANAR;
constexpr Codec PCM_S16BE_PLANAR	=AV_CODEC_ID_PCM_S16BE_PLANAR;

constexpr Codec PCM_S64LE	=AV_CODEC_ID_PCM_S64LE;
constexpr Codec PCM_S64BE	=AV_CODEC_ID_PCM_S64BE;
constexpr Codec PCM_F16LE	=AV_CODEC_ID_PCM_F16LE;
constexpr Codec PCM_F24LE	=AV_CODEC_ID_PCM_F24LE;
constexpr Codec PCM_VIDC	=AV_CODEC_ID_PCM_VIDC;

/* various ADPCM codecs */
constexpr Codec ADPCM_IMA_QT	=AV_CODEC_ID_ADPCM_IMA_QT;
constexpr Codec ADPCM_IMA_WAV	=AV_CODEC_ID_ADPCM_IMA_WAV;
constexpr Codec ADPCM_IMA_DK3	=AV_CODEC_ID_ADPCM_IMA_DK3;
constexpr Codec ADPCM_IMA_DK4	=AV_CODEC_ID_ADPCM_IMA_DK4;
constexpr Codec ADPCM_IMA_WS	=AV_CODEC_ID_ADPCM_IMA_WS;
constexpr Codec ADPCM_IMA_SMJPEG	=AV_CODEC_ID_ADPCM_IMA_SMJPEG;
constexpr Codec ADPCM_MS	=AV_CODEC_ID_ADPCM_MS;
constexpr Codec ADPCM_4XM	=AV_CODEC_ID_ADPCM_4XM;
constexpr Codec ADPCM_XA	=AV_CODEC_ID_ADPCM_XA;
constexpr Codec ADPCM_ADX	=AV_CODEC_ID_ADPCM_ADX;
constexpr Codec ADPCM_EA	=AV_CODEC_ID_ADPCM_EA;
constexpr Codec ADPCM_G726	=AV_CODEC_ID_ADPCM_G726;
constexpr Codec ADPCM_CT	=AV_CODEC_ID_ADPCM_CT;
constexpr Codec ADPCM_SWF	=AV_CODEC_ID_ADPCM_SWF;
constexpr Codec ADPCM_YAMAHA	=AV_CODEC_ID_ADPCM_YAMAHA;
constexpr Codec ADPCM_SBPRO_4	=AV_CODEC_ID_ADPCM_SBPRO_4;
constexpr Codec ADPCM_SBPRO_3	=AV_CODEC_ID_ADPCM_SBPRO_3;
constexpr Codec ADPCM_SBPRO_2	=AV_CODEC_ID_ADPCM_SBPRO_2;
constexpr Codec ADPCM_THP	=AV_CODEC_ID_ADPCM_THP;
constexpr Codec ADPCM_IMA_AMV	=AV_CODEC_ID_ADPCM_IMA_AMV;
constexpr Codec ADPCM_EA_R1	=AV_CODEC_ID_ADPCM_EA_R1;
constexpr Codec ADPCM_EA_R3	=AV_CODEC_ID_ADPCM_EA_R3;
constexpr Codec ADPCM_EA_R2	=AV_CODEC_ID_ADPCM_EA_R2;
constexpr Codec ADPCM_IMA_EA_SEAD	=AV_CODEC_ID_ADPCM_IMA_EA_SEAD;
constexpr Codec ADPCM_IMA_EA_EACS	=AV_CODEC_ID_ADPCM_IMA_EA_EACS;
constexpr Codec ADPCM_EA_XAS	=AV_CODEC_ID_ADPCM_EA_XAS;
constexpr Codec ADPCM_EA_MAXIS_XA	=AV_CODEC_ID_ADPCM_EA_MAXIS_XA;
constexpr Codec ADPCM_IMA_ISS	=AV_CODEC_ID_ADPCM_IMA_ISS;
constexpr Codec ADPCM_G722	=AV_CODEC_ID_ADPCM_G722;
constexpr Codec ADPCM_IMA_APC	=AV_CODEC_ID_ADPCM_IMA_APC;
constexpr Codec ADPCM_VIMA	=AV_CODEC_ID_ADPCM_VIMA;

constexpr Codec ADPCM_AFC	=AV_CODEC_ID_ADPCM_AFC;
constexpr Codec ADPCM_IMA_OKI	=AV_CODEC_ID_ADPCM_IMA_OKI;
constexpr Codec ADPCM_DTK	=AV_CODEC_ID_ADPCM_DTK;
constexpr Codec ADPCM_IMA_RAD	=AV_CODEC_ID_ADPCM_IMA_RAD;
constexpr Codec ADPCM_G726LE	=AV_CODEC_ID_ADPCM_G726LE;
constexpr Codec ADPCM_THP_LE	=AV_CODEC_ID_ADPCM_THP_LE;
constexpr Codec ADPCM_PSX	=AV_CODEC_ID_ADPCM_PSX;
constexpr Codec ADPCM_AICA	=AV_CODEC_ID_ADPCM_AICA;
constexpr Codec ADPCM_IMA_DAT4	=AV_CODEC_ID_ADPCM_IMA_DAT4;
constexpr Codec ADPCM_MTAF	=AV_CODEC_ID_ADPCM_MTAF;

/* AMR */
constexpr Codec AMR_NB	=AV_CODEC_ID_AMR_NB;
constexpr Codec AMR_WB	=AV_CODEC_ID_AMR_WB;

/* RealAudio codecs*/
constexpr Codec RA_144	=AV_CODEC_ID_RA_144;
constexpr Codec RA_288	=AV_CODEC_ID_RA_288;

/* various DPCM codecs */
constexpr Codec ROQ_DPCM	=AV_CODEC_ID_ROQ_DPCM;
constexpr Codec INTERPLAY_DPCM	=AV_CODEC_ID_INTERPLAY_DPCM;
constexpr Codec XAN_DPCM	=AV_CODEC_ID_XAN_DPCM;
constexpr Codec SOL_DPCM	=AV_CODEC_ID_SOL_DPCM;

constexpr Codec SDX2_DPCM	=AV_CODEC_ID_SDX2_DPCM;
constexpr Codec GREMLIN_DPCM	=AV_CODEC_ID_GREMLIN_DPCM;

/* audio codecs */
constexpr Codec MP2	=AV_CODEC_ID_MP2;
constexpr Codec MP3	=AV_CODEC_ID_MP3; ///< preferred ID for decoding MPEG audio layer 1; 2 or 3
constexpr Codec AAC	=AV_CODEC_ID_AAC;
constexpr Codec AC3	=AV_CODEC_ID_AC3;
constexpr Codec DTS	=AV_CODEC_ID_DTS;
constexpr Codec VORBIS	=AV_CODEC_ID_VORBIS;
constexpr Codec DVAUDIO	=AV_CODEC_ID_DVAUDIO;
constexpr Codec WMAV1	=AV_CODEC_ID_WMAV1;
constexpr Codec WMAV2	=AV_CODEC_ID_WMAV2;
constexpr Codec MACE3	=AV_CODEC_ID_MACE3;
constexpr Codec MACE6	=AV_CODEC_ID_MACE6;
constexpr Codec VMDAUDIO	=AV_CODEC_ID_VMDAUDIO;
constexpr Codec FLAC	=AV_CODEC_ID_FLAC;
constexpr Codec MP3ADU	=AV_CODEC_ID_MP3ADU;
constexpr Codec MP3ON4	=AV_CODEC_ID_MP3ON4;
constexpr Codec SHORTEN	=AV_CODEC_ID_SHORTEN;
constexpr Codec ALAC	=AV_CODEC_ID_ALAC;
constexpr Codec WESTWOOD_SND1	=AV_CODEC_ID_WESTWOOD_SND1;
constexpr Codec GSM	=AV_CODEC_ID_GSM; ///< as in Berlin toast format
constexpr Codec QDM2	=AV_CODEC_ID_QDM2;
constexpr Codec COOK	=AV_CODEC_ID_COOK;
constexpr Codec TRUESPEECH	=AV_CODEC_ID_TRUESPEECH;
constexpr Codec TTA	=AV_CODEC_ID_TTA;
constexpr Codec SMACKAUDIO	=AV_CODEC_ID_SMACKAUDIO;
constexpr Codec QCELP	=AV_CODEC_ID_QCELP;
constexpr Codec WAVPACK	=AV_CODEC_ID_WAVPACK;
constexpr Codec DSICINAUDIO	=AV_CODEC_ID_DSICINAUDIO;
constexpr Codec IMC	=AV_CODEC_ID_IMC;
constexpr Codec MUSEPACK7	=AV_CODEC_ID_MUSEPACK7;
constexpr Codec MLP	=AV_CODEC_ID_MLP;
constexpr Codec GSM_MS	=AV_CODEC_ID_GSM_MS; /* as found in WAV */
constexpr Codec ATRAC3	=AV_CODEC_ID_ATRAC3;
constexpr Codec APE	=AV_CODEC_ID_APE;
constexpr Codec NELLYMOSER	=AV_CODEC_ID_NELLYMOSER;
constexpr Codec MUSEPACK8	=AV_CODEC_ID_MUSEPACK8;
constexpr Codec SPEEX	=AV_CODEC_ID_SPEEX;
constexpr Codec WMAVOICE	=AV_CODEC_ID_WMAVOICE;
constexpr Codec WMAPRO	=AV_CODEC_ID_WMAPRO;
constexpr Codec WMALOSSLESS	=AV_CODEC_ID_WMALOSSLESS;
constexpr Codec ATRAC3P	=AV_CODEC_ID_ATRAC3P;
constexpr Codec EAC3	=AV_CODEC_ID_EAC3;
constexpr Codec SIPR	=AV_CODEC_ID_SIPR;
constexpr Codec MP1	=AV_CODEC_ID_MP1;
constexpr Codec TWINVQ	=AV_CODEC_ID_TWINVQ;
constexpr Codec TRUEHD	=AV_CODEC_ID_TRUEHD;
constexpr Codec MP4ALS	=AV_CODEC_ID_MP4ALS;
constexpr Codec ATRAC1	=AV_CODEC_ID_ATRAC1;
constexpr Codec BINKAUDIO_RDFT	=AV_CODEC_ID_BINKAUDIO_RDFT;
constexpr Codec BINKAUDIO_DCT	=AV_CODEC_ID_BINKAUDIO_DCT;
constexpr Codec AAC_LATM	=AV_CODEC_ID_AAC_LATM;
constexpr Codec QDMC	=AV_CODEC_ID_QDMC;
constexpr Codec CELT	=AV_CODEC_ID_CELT;
constexpr Codec G723_1	=AV_CODEC_ID_G723_1;
constexpr Codec G729	=AV_CODEC_ID_G729;
constexpr Codec A8SVX_EXP	=AV_CODEC_ID_8SVX_EXP;
constexpr Codec A8SVX_FIB	=AV_CODEC_ID_8SVX_FIB;
constexpr Codec BMV_AUDIO	=AV_CODEC_ID_BMV_AUDIO;
constexpr Codec RALF	=AV_CODEC_ID_RALF;
constexpr Codec IAC	=AV_CODEC_ID_IAC;
constexpr Codec ILBC	=AV_CODEC_ID_ILBC;
constexpr Codec OPUS	=AV_CODEC_ID_OPUS;
constexpr Codec COMFORT_NOISE	=AV_CODEC_ID_COMFORT_NOISE;
constexpr Codec TAK	=AV_CODEC_ID_TAK;
constexpr Codec METASOUND	=AV_CODEC_ID_METASOUND;
constexpr Codec PAF_AUDIO	=AV_CODEC_ID_PAF_AUDIO;
constexpr Codec ON2AVC	=AV_CODEC_ID_ON2AVC;
constexpr Codec DSS_SP	=AV_CODEC_ID_DSS_SP;
constexpr Codec CODEC2	=AV_CODEC_ID_CODEC2;

constexpr Codec FFWAVESYNTH	=AV_CODEC_ID_FFWAVESYNTH;
constexpr Codec SONIC	=AV_CODEC_ID_SONIC;
constexpr Codec SONIC_LS	=AV_CODEC_ID_SONIC_LS;
constexpr Codec EVRC	=AV_CODEC_ID_EVRC;
constexpr Codec SMV	=AV_CODEC_ID_SMV;
constexpr Codec DSD_LSBF	=AV_CODEC_ID_DSD_LSBF;
constexpr Codec DSD_MSBF	=AV_CODEC_ID_DSD_MSBF;
constexpr Codec DSD_LSBF_PLANAR	=AV_CODEC_ID_DSD_LSBF_PLANAR;
constexpr Codec DSD_MSBF_PLANAR	=AV_CODEC_ID_DSD_MSBF_PLANAR;
constexpr Codec A4GV	=AV_CODEC_ID_4GV;
constexpr Codec INTERPLAY_ACM	=AV_CODEC_ID_INTERPLAY_ACM;
constexpr Codec XMA1	=AV_CODEC_ID_XMA1;
constexpr Codec XMA2	=AV_CODEC_ID_XMA2;
constexpr Codec DST	=AV_CODEC_ID_DST;
constexpr Codec ATRAC3AL	=AV_CODEC_ID_ATRAC3AL;
constexpr Codec ATRAC3PAL	=AV_CODEC_ID_ATRAC3PAL;
constexpr Codec DOLBY_E	=AV_CODEC_ID_DOLBY_E;
constexpr Codec APTX	=AV_CODEC_ID_APTX;
constexpr Codec APTX_HD	=AV_CODEC_ID_APTX_HD;
constexpr Codec SBC	=AV_CODEC_ID_SBC;
constexpr Codec ATRAC9	=AV_CODEC_ID_ATRAC9;
}

}
