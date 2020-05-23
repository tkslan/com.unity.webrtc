#include "pch.h"
#include "GraphicsDeviceTestBase.h"
#include "../WebRTCPlugin/Codec/EncoderFactory.h"
#include "../WebRTCPlugin/Codec/IEncoder.h"
#include "../WebRTCPlugin/Context.h"
#include "../WebRTCPlugin/VideoCaptureTrackSource.h"
#include "UnityVideoTrackSource.h"

using testing::_;
using testing::Invoke;
using testing::Mock;

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

    webrtc::VideoFrame::Builder  CreateBlackFrameBuilder(int width, int height)
    {
        rtc::scoped_refptr<webrtc::I420Buffer> buffer =
            webrtc::I420Buffer::Create(width, height);

        webrtc::I420Buffer::SetBlack(buffer);
        return webrtc::VideoFrame::Builder().set_video_frame_buffer(buffer);
    }

    void SendTestFrame(int width, int height)
    {
        webrtc::VideoFrame frame =
            CreateBlackFrameBuilder(width, height).build();
            m_trackSource->OnFrameCaptured(frame);
    }

    void TearDown() override
    {
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
    int width = 256;
    int height = 256;
    EXPECT_CALL(mock_sink_, OnFrame(_))
        .WillOnce(Invoke([width, height](const webrtc::VideoFrame& frame) {
            EXPECT_EQ(width, frame.width());
            EXPECT_EQ(height, frame.height());
    }));
    SendTestFrame(width, height);
}

INSTANTIATE_TEST_CASE_P(
    GraphicsDeviceParameters,
    VideoTrackSourceTest,
    testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity
