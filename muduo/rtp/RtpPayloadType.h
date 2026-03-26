#ifndef MUDUO_RTP_RTPPAYLOADTYPE
#define MUDUO_RTP_RTPPAYLOADTYPE

class RtpPayloadType {
public:
    static int getClockRate(int pt);
    static TrackType getTrackType(int pt);
    static int getAudioChannelCount(int pt);
    static const char* getName(int pt);
    static AppCodecId getCodecId(int pt);
};

#endif // MUDUO_RTP_RTPPAYLOADTYPE