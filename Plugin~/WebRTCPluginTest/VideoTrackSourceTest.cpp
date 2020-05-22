#include "pch.h"
#include "GraphicsDeviceTestBase.h"
#include "../WebRTCPlugin/Codec/EncoderFactory.h"
#include "../WebRTCPlugin/Codec/IEncoder.h"
#include "../WebRTCPlugin/Context.h"
#include "../WebRTCPlugin/VideoCaptureTrackSource.h"
#include "UnityVideoTrackSource.h"

namespace unity
{
namespace webrtc
{

class MockVideoSink : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
public:
    MOCK_METHOD1(OnFrame, void(const webrtc::VideoFrame&));

    //MOCK_METHOD1(OnFrame, void(const webrtc::VideoFrame&));
};

class VideoTrackSourceTest : public GraphicsDeviceTestBase
{
public:
    VideoTrackSourceTest()
        : track_source_(new rtc::RefCountedObject<UnityVideoTrackSource>(
            /*is_screencast=*/false,
            /*needs_denoising=*/absl::nullopt)) {
        track_source_->AddOrUpdateSink(&mock_sink_, rtc::VideoSinkWants());
    }
    ~VideoTrackSourceTest() override {
        track_source_->RemoveSink(&mock_sink_);
    }
protected:
    std::unique_ptr<IEncoder> encoder_;
    const int width = 256;
    const int height = 256;
    std::unique_ptr<Context> context;

    MockVideoSink mock_sink_;
    rtc::scoped_refptr<UnityVideoTrackSource> track_source_;

    void SetUp() override {
        GraphicsDeviceTestBase::SetUp();
        EXPECT_NE(nullptr, m_device);

        encoder_ = EncoderFactory::GetInstance().Init(width, height, m_device, encoderType);
        EXPECT_NE(nullptr, encoder_);

        context = std::make_unique<Context>();
    }

    void SendTestFrame()
    {
        
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
    auto videoSource = rtc::scoped_refptr<rtc::AdaptedVideoTrackSource>();
    /*
    rtc::scoped_refptr<WebRtcVideoTrackSource>(
        new rtc::RefCountedObject<WebRtcVideoTrackSource>(is_screencast,
            needs_denoising));
    */
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSourceProxy =
        webrtc::VideoTrackSourceProxy::Create(
            signalingThread.get(),
            workerThread.get(), videoSource);
}

INSTANTIATE_TEST_CASE_P(GraphicsDeviceParameters, VideoTrackSourceTest, testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity
