#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitdbus.h"
#include "mapdownloaderdbus.h"
#include "gpsdprovider.h"
#include "testutils.h"
#include "dbuscontroller.h"
#include "mocks/speechmock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>

using namespace NXE;
extern bool runNavit;

struct NXEInstanceTest : public ::testing::Test {

    DBusController dbusController;
    INavitIPC * ipc { new NavitDBus{dbusController}};
    IMapDownloader * md { new MapDownloaderDBus{dbusController}};
    NXE::DI::Injector injector{ std::make_tuple(
                    std::shared_ptr<NXE::INavitIPC>(ipc),
                    std::shared_ptr<NXE::INavitProcess>(new NXE::NavitProcessImpl),
                    std::shared_ptr<NXE::IGPSProvider>(new GPSDProvider),
                    std::shared_ptr<NXE::IMapDownloader>(new MapDownloaderDBus{dbusController}),
                    std::shared_ptr<NXE::ISpeech>(new SpeechMock)
                    )};
    NXEInstance instance{ injector };
    bool receivedRender{ false };
    std::size_t numberOfResponses = 0;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void SetUp() override {
        instance.Initialize();
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }

    void zoom(int factor)
    {
        instance.HandleMessage<ZoomByMessageTag>(factor);
    }
};

TEST_F(NXEInstanceTest, zoomByMessage_zoomIn_correct)
{
    zoom(2);
}

// TODO: How to enable speech test?
TEST_F(NXEInstanceTest, DISABLED_speechTest)
{
    // by default each time we want to draw a
    // speech 'draw' will be triggered
    instance.Initialize();
    zoom(2);
}

TEST_F(NXEInstanceTest, zoomByMessage_zoomOut_correct)
{
    zoom(-2);
}

TEST_F(NXEInstanceTest, zoomMessage_correct)
{
    instance.HandleMessage<ZoomMessageTag>();
}

TEST_F(NXEInstanceTest, zoomInAndOut)
{
    instance.HandleMessage<ZoomByMessageTag>(2);
    instance.HandleMessage<ZoomByMessageTag>(-2);
}

TEST_F(NXEInstanceTest, renderMessage_correct)
{
    instance.HandleMessage<RenderMessageTag>();
}

TEST_F(NXEInstanceTest, moveByMessage_correct)
{
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    instance.HandleMessage<MoveByMessageTag>(10,10);
}

TEST_F(NXEInstanceTest, changeOrientation_correct)
{
    instance.HandleMessage<SetOrientationMessageTag>(-1);
    int orientation = instance.HandleMessage<OrientationMessageTag>();

    EXPECT_EQ(orientation, -1);
}

TEST_F(NXEInstanceTest, positionMessage_correct)
{
    auto pos = instance.HandleMessage<PositionMessageTag>();
}

TEST_F(NXEInstanceTest, changeOrientationMessage_incorrectValue)
{
    EXPECT_ANY_THROW(instance.HandleMessage<SetOrientationMessageTag>(100));
}

TEST_F(NXEInstanceTest, set_position_and_set_zoom)
{
    instance.HandleMessage<SetPositionByIntMessageTag>(0,0);
    instance.HandleMessage<SetZoomMessageTag>(16);
}

TEST_F(NXEInstanceTest, DISABLED_availableMessages_correct)
{
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    auto maps = instance.HandleMessage<MapsMessageTag>();
}

TEST_F(NXEInstanceTest, DISABLED_downloadMessage_incorrect_country)
{
    // Arrange
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    // Act
    instance.HandleMessage<DownloadMessageTag>("this countr does not exists");

    // Assert
}

TEST_F(NXEInstanceTest, DISABLED_downloadMessage_correct_country)
{
    // Arrange
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    // Act
    instance.HandleMessage<DownloadMessageTag>("Hawaii");

    while (true) {
        std::this_thread::sleep_for(dura);
    }

    instance.HandleMessage<CancelDownloadMessageTag>("Hawaii");

}
