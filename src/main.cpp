#include "hiddev/hiddevreader.h"
#include "hiddev/hiddevfinder.h"
#include "sdgyrodsu/sdhidframe.h"
#include "sdgyrodsu/presenter.h"
#include "cemuhook/cemuhookprotocol.h"
#include "cemuhook/cemuhookserver.h"
#include "sdgyrodsu/cemuhookadapter.h"
#include "log/log.h"
#include <iostream>
#include <future>
#include <thread>

using namespace kmicki::sdgyrodsu;
using namespace kmicki::hiddev;

bool showIncrement = false;

#define FRAME_LEN 64
#define SCAN_PERIOD_US 3850

#define VERSION "1.12-NEXT-DEV"

#define VID 0x28de
#define PID 0x1205

typedef HidDevReader::frame_t frame_t;

using namespace kmicki::sdgyrodsu;
using namespace kmicki::cemuhook::protocol;
using namespace kmicki::cemuhook;
using namespace kmicki::log;

static std::exception_ptr teptr = nullptr;

static bool stop = false;

void WaitForKey()
{
    std::cin.get();
    stop = true;
}

void PresenterRun(HidDevReader * reader)
{
    auto & frameServe = reader->GetServe();
    auto const& data = frameServe.GetPointer();
    int temp;
    void* tempPtr = reinterpret_cast<void*>(&temp);
    Presenter::Initialize();
    while(!stop)
    {
        auto lock = frameServe.GetConsumeLock();
        Presenter::Present(GetSdFrame(*data));
    }
    Presenter::Finish();
}

int main()
{
    { LogF() << "SteamDeckGyroDSU Version: " << VERSION; }
    // Steam Deck controls: usb device VID: 28de, PID: 1205
    int hidno = FindHidDevNo(VID,PID);
    if(hidno < 0) 
    {
        Log("Steam Deck Controls' HID device not found.");
        return 0;
    }

    { LogF() << "Found Steam Deck Controls' HID device at /dev/usb/hiddev" << hidno; }
    
    HidDevReader reader(hidno,FRAME_LEN,SCAN_PERIOD_US);

    reader.SetStartMarker({ 0x01, 0x00, 0x09, 0x40 });

    CemuhookAdapter adapter(reader);
    Server server(adapter);

    uint32_t lastInc = 0;
    int stopping = 0;

    //std::thread presenter(PresenterRun,&reader);

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        // uint32_t const& newInc = *reinterpret_cast<uint32_t const*>(reader.Frame().data()+4);
        // if(newInc == lastInc)
        // {
        //     if(reader.IsStarted() || stopping > 5)
        //     {
        //         Log("Framegrab is stuck. Aborting...");
        //         std::abort();
        //     }
        //     if(reader.IsStopping())
        //         ++stopping;
        // }
        // if(!reader.IsStarted() && !reader.IsStopping())
        //     lastInc = 0;
        // else
        //     lastInc = newInc;
        // if(!reader.IsStopping())
        //     stopping = 0;
    }

    return 0;
}