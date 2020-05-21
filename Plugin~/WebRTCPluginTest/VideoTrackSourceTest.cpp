#include "pch.h"
#include "GraphicsDeviceTestBase.h"
#include "../WebRTCPlugin/GraphicsDevice/ITexture2D.h"
#include "../WebRTCPlugin/Codec/EncoderFactory.h"
#include "../WebRTCPlugin/Codec/IEncoder.h"
#include "../WebRTCPlugin/Context.h"
#include "../WebRTCPlugin/VideoCaptureTrackSource.h"

namespace unity
{
namespace webrtc
{

class VideoTrackSourceTest : public GraphicsDeviceTestBase
{
protected:
    std::unique_ptr<IEncoder> encoder_;
    const int width = 256;
    const int height = 256;
    std::unique_ptr<Context> context;

    void SetUp() override {
        GraphicsDeviceTestBase::SetUp();
        EXPECT_NE(nullptr, m_device);

        encoder_ = EncoderFactory::GetInstance().Init(width, height, m_device, encoderType);
        EXPECT_NE(nullptr, encoder_);

        context = std::make_unique<Context>();
    }
    void TearDown() override {
        GraphicsDeviceTestBase::TearDown();
    }
};

TEST_P(VideoTrackSourceTest, Constructor)
{
    std::unique_ptr<rtc::Thread> workerThread = rtc::Thread::Create();
    workerThread->Start();
    std::unique_ptr<rtc::Thread> signalingThread = rtc::Thread::Create();
    signalingThread->Start();
    //auto videoCapturer = std::make_unique<NvVideoCapturer>();
    //auto videoCapturer = rtc::scoped_refptr<webrtc::VideoTrackSource>();
    auto videoCapturer = rtc::scoped_refptr<rtc::AdaptedVideoTrackSource>();
    /*
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> source(
        webrtc::VideoCapturerTrackSource::Create(workerThread.get(), std::move(videoCapturer), true));
     */
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> source =
        webrtc::VideoTrackSourceProxy::Create(
            signalingThread.get(), workerThread.get(), videoCapturer);
}

INSTANTIATE_TEST_CASE_P(GraphicsDeviceParameters, VideoTrackSourceTest, testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity
