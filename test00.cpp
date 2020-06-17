#include <zuazo/Includes.h> 

#include <iostream>
#include <fstream>
#include <mutex>

class TestSource final
    : public Zuazo::ZuazoBase
{
    Zuazo::Graphics::Uploader uplo;
    int j;

public:
    TestSource(Zuazo::Instance& inst)
        : ZuazoBase(inst, "Test Source", PADS)
        , uplo( inst.getVulkan(),
			    Zuazo::Resolution(1280, 720),
			    Zuazo::AspectRatio(1, 1),
			    Zuazo::ColorSubsampling::NONE,
			    Zuazo::ColorFormat::R8G8B8A8,
			    Zuazo::ColorRange::FULL,
			    Zuazo::ColorTransferFunction::LINEAR,
			    Zuazo::ColorModel::RGB,
			    Zuazo::ColorPrimaries::BT709 )
        , j(0)
    {
        setUpdateCallback(std::bind(&TestSource::update, std::ref(*this)));
        enableRegularUpdate(Zuazo::Instance::INPUT_PRIORITY);
    }

    ~TestSource() {
        disableRegularUpdate();
    }

    void update() {
		const auto frame = uplo.acquireFrame();

		const auto& pixels = frame->getPixelData();
		for(size_t i = 0; i < pixels[0].size(); i+=4){
			pixels[0][i + 0] = static_cast<std::byte>(j % 0x100);
			pixels[0][i + 1] = static_cast<std::byte>(j % 0x100);
			pixels[0][i + 2] = static_cast<std::byte>(j % 0x100);
			pixels[0][i + 3] = static_cast<std::byte>(0xff);
		}
        j++;

		frame->flush();

        auto& output = findPad(std::get<0>(PADS));
        output.push(frame);
    }

    void open() final {}
    void close() final {}

    static inline const auto PADS = std::make_tuple(Zuazo::Signal::Output<Zuazo::Video>("videoOut0"));

};


int main(){
    Zuazo::Instance::ApplicationInfo appInfo;
    Zuazo::Instance instance(std::move(appInfo));

    std::unique_lock<Zuazo::Instance> lock(instance);
    Zuazo::Outputs::Window window(instance, "Window");
    TestSource test(instance);
    
    window.open();

    lock.unlock();
    getchar();
    lock.lock();

    window.getPad(std::get<0>(Zuazo::Outputs::Window::PADS)) << test.getPad(std::get<0>(TestSource::PADS));

    lock.unlock();
    getchar();
    lock.lock();
    return 0;
}
