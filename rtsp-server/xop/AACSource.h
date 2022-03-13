// PHZ
// 2018-5-16

#ifndef XOP_AAC_SOURCE_H
#define XOP_AAC_SOURCE_H

#include "MediaSource.h"
#include "rtp.h"

namespace xop
{

class AACSource : public MediaSource
{
public:
    static AACSource* CreateNew(uint32_t samplerate= 44100, uint8_t channels= 2, bool has_adts= true);
    ~AACSource() override;

    uint32_t GetSamplerate() const
    { return samplerate_; }

    uint32_t GetChannels() const
    { return channels_; }

    std::string GetMediaDescription(uint16_t port= 0) override;

    std::string GetAttribute() override;

    bool HandleFrame(MediaChannelId channel_id, AVFrame frame) override;

    static uint32_t GetTimestamp(uint32_t samplerate = 44100);

private:
    AACSource(uint32_t samplerate, uint8_t channels, bool has_adts);

    uint32_t samplerate_ = 44100;  
    uint8_t channels_ = 2;         
    bool has_adts_ = true;

    static constexpr size_t ADTS_SIZE = 7;
    static constexpr size_t AU_SIZE = 4;
};

}

#endif
