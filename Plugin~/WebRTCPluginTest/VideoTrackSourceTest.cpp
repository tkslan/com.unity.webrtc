#include "pch.h"
#include "GraphicsDeviceTestBase.h"
#include "../WebRTCPlugin/Codec/EncoderFactory.h"
#include "../WebRTCPlugin/Codec/IEncoder.h"
#include "../WebRTCPlugin/Context.h"
#include "../WebRTCPlugin/VideoCaptureTrackSource.h"
#include "UnityVideoTrackSource.h"
//#include "test/frame_generator.h"

//using testing::_;
//using testing::Invoke;
//using testing::Mock;

namespace unity
{
namespace webrtc
{
class MockVideoSink : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    MOCK_METHOD1(OnFrame, void(const webrtc::VideoFrame&));
};

class VideoTrackSourceTest : public GraphicsDeviceTestBase
{
public:
    VideoTrackSourceTest()
        : m_trackSource(new rtc::RefCountedObject<UnityVideoTrackSource>(
            /*is_screencast=*/ false,
            /*needs_denoising=*/ absl::nullopt))
    {
        m_trackSource->AddOrUpdateSink(&mock_sink_, rtc::VideoSinkWants());
    }
    ~VideoTrackSourceTest() override
    {
        m_trackSource->RemoveSink(&mock_sink_);
    }
protected:
    std::unique_ptr<IEncoder> encoder_;
    const int width = 256;
    const int height = 256;
    std::unique_ptr<Context> context;

    MockVideoSink mock_sink_;
    rtc::scoped_refptr<UnityVideoTrackSource> m_trackSource;

    void SetUp() override
    {
        GraphicsDeviceTestBase::SetUp();
        EXPECT_NE(nullptr, m_device);

        encoder_ = EncoderFactory::GetInstance().Init(width, height, m_device, encoderType);
        EXPECT_NE(nullptr, encoder_);

        context = std::make_unique<Context>();
    }

    webrtc::VideoFrame* CreateTestFrame(int width, int height)
    {
        /*
        std::unique_ptr<webrtc::test::FrameGenerator> generator =
            webrtc::test::FrameGenerator::CreateSquareGenerator( width, height,
            webrtc::test::FrameGenerator::OutputType::kI420, 1);

        return generator->NextFrame();
        */
        return nullptr;
    }

    void SendTestFrame()
    {
        webrtc::VideoFrame* frame =
                CreateTestFrame(width, height);
            m_trackSource->OnFrameCaptured(*frame);
    }

    void TearDown() override {
        GraphicsDeviceTestBase::TearDown();
    }
};

TEST_P(VideoTrackSourceTest, CreateVideoSourceProxy)
{
    std::unique_ptr<rtc::Thread> workerThread = rtc::Thread::Create();
    workerThread->Start();
    std::unique_ptr<rtc::Thread> signalingThread = rtc::Thread::Create();
    signalingThread->Start();

    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSourceProxy =
        webrtc::VideoTrackSourceProxy::Create(
            signalingThread.get(),
            workerThread.get(), m_trackSource);
}

TEST_P(VideoTrackSourceTest, SendTestFrame)
{
    SendTestFrame();
//    EXPECT_CALL(mock_sink_, OnFrame(testing::_));
}

INSTANTIATE_TEST_CASE_P(
    GraphicsDeviceParameters,
    VideoTrackSourceTest,
    testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity
