#include <zuazo/Consumers/Window.h>
#include <zuazo/Stream/Consumer.h>
#include <zuazo/Stream/Source.h>
#include <zuazo/Utils/Resolution.h>
#include <zuazo/Utils/VideoMode.h>
#include <zuazo/Video/VideoSourceBase.h>
#include <zuazo/Processors/Compositor.h>
#include <zuazo/Sources/FFmpeg.h>
#include <zuazo/Stream/DummyPad.h>
#include <zuazo/Zuazo.h>


#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#define RAND_IN_RANGE(min, max) ((int)(min) + (rand() % ((int)(max) - (int)(min))))




/**
 * DESCRIPTION:
 * This example shows how to combine several zuazo elements in to a single class to create your own modules
 * The final result will be a module which shows 2 video sources side-by-side over a background
 */

/**
 * HOW TO COMPILE:
 * g++ Example4.cpp -o Example4 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16
 * HOW TO RUN:
 * ./Example3 <file1> <file2> <file3>
 *
 */

class SideBySide :
		public Zuazo::Video::TVideoSourceBase<Zuazo::Video::DummyVideoPad>, //It might be useful to inherit from VideoBase, as it has methods for getting several video parameters
		public Zuazo::ZuazoBase //You also should inherit from zuazo base and override open() and close()
{
public:
	/*
	 * First of all declare the public "pads"
	 * Note that they are just references, the actual pads will be declared private afterwards
	 * This is done to protect some of the methods that shouldn't be accessible outside the module
	 * If the module has a single video input / output, it might be easier to declare the pads by inheriting
	 * However, for multiple inputs or outputs, it is compulsory to declare them manually
	 * from TVideoConsumerBase<> / TVideoSourceBase<>
	 */
	Zuazo::Video::VideoConsumer& 		leftVideoIn;
	Zuazo::Video::VideoConsumer& 		rightVideoIn;
	Zuazo::Video::VideoConsumer& 		bkgdVideoIn;
	//Zuazo::Video::VideoSource& 		videoOut; //It is defined by TVideoSourceBase

	/*
	 * Declare the constructors / destructors
	 */

	SideBySide() :
		/*
		 * Assign each private pad to the it's public reference
		 */
		leftVideoIn(m_leftVideoSourcePad),
		rightVideoIn(m_rightVideoSourcePad),
		bkgdVideoIn(m_bkgdVideoSourcePad)
	{
		//Create the 3 needed layers
		std::unique_ptr<Zuazo::Processors::Compositor::VideoLayer> leftLayer(
				new Zuazo::Processors::Compositor::VideoLayer
		);

		std::unique_ptr<Zuazo::Processors::Compositor::VideoLayer> rightLayer(
				new Zuazo::Processors::Compositor::VideoLayer
		);

		std::unique_ptr<Zuazo::Processors::Compositor::VideoLayer> bkgdLayer(
				new Zuazo::Processors::Compositor::VideoLayer
		);

		//Route each pad into each layer
		leftLayer->videoIn << m_leftVideoSourcePad;
		rightLayer->videoIn << m_rightVideoSourcePad;
		bkgdLayer->videoIn << m_bkgdVideoSourcePad;
		//Don't forget to route the output
		m_videoSourcePad << m_videoCompositor.videoOut;

		//Add the layers to the compositor
		m_videoCompositor.addLayer(std::move(leftLayer));
		m_videoCompositor.addLayer(std::move(rightLayer));
		m_videoCompositor.addLayer(std::move(bkgdLayer));
	}

	SideBySide(const Zuazo::Utils::VideoMode& videoMode) :
		SideBySide()
	{
		setVideoMode(videoMode);
	}

	SideBySide(const SideBySide& other)=delete;
	SideBySide(SideBySide&& other)=default;
	virtual ~SideBySide()
	{

	}

	/*
	 * For the seek of simplicity of this example,
	 * we are going to override only some of the setters provided by video base
	 * however, more of them could be overrided
	 */
	SUPPORTS_GETTING_VIDEOMODE
	SUPPORTS_SETTING_VIDEOMODE
	void setVideoMode(const Zuazo::Utils::VideoMode& videoMode) override{
		m_videoMode=videoMode;

		const float rectWidth=m_videoMode.res.width * 0.9 / 2;
		const float rectHeight=m_videoMode.res.height * 0.9 / 2;

		//Create the rectangle for the layers, which covers a quarter of the screen
		const Zuazo::Graphics::Rectangle rect={
				.center=Zuazo::Utils::Vec3f(rectWidth / 2, rectHeight / 2, 0),
				.size=Zuazo::Utils::Vec2f(rectWidth, rectHeight)
		};

		//Create a rectangle for the backgrund, which covers all the screren
		const Zuazo::Graphics::Rectangle bkgdRect={
				.center=Zuazo::Utils::Vec3f((float)m_videoMode.res.width / 2, (float)m_videoMode.res.height / 2, 0),
				.size=Zuazo::Utils::Vec2f(m_videoMode.res.width, m_videoMode.res.height)
		};

		const float angle=glm::radians(20.0); //The inclination of the left and right surfaces

		const float zOffset=rectWidth * sin(angle) / 2;	//The offset in the Z axis of the two
														//surfaces (so that they are not
														//behind the background due to the inclination)

		const Zuazo::Utils::Vec3f leftLayerPos(-rectWidth / 2, 0, zOffset);
		const Zuazo::Utils::Vec3f rightLayerPos(rectWidth / 2, 0, zOffset);
		const Zuazo::Utils::Vec3f leftLayerAngle(0, angle, 0);
		const Zuazo::Utils::Vec3f rightLayerAngle(0, -angle, 0);

		//Get layers by their index
		Zuazo::Processors::Compositor::VideoLayer* leftLayer=m_videoCompositor.getLayer<Zuazo::Processors::Compositor::VideoLayer>(0);
		Zuazo::Processors::Compositor::VideoLayer* rightLayer=m_videoCompositor.getLayer<Zuazo::Processors::Compositor::VideoLayer>(1);
		Zuazo::Processors::Compositor::VideoLayer* bkgdLayer=m_videoCompositor.getLayer<Zuazo::Processors::Compositor::VideoLayer>(2);

		//Stop the timings, so that all the changes happen at one frame
		std::lock_guard<Zuazo::Timing::Timings> timingLock(*Zuazo::Timing::timings);

		//Set compositor's video mode
		//m_videoCompositor.close();
		m_videoCompositor.setVideoMode(m_videoMode);
		//m_videoCompositor.open();

		//Set default viewing parameters (vary from one resolution to another)
		m_videoCompositor.setDefaultClipping();
		m_videoCompositor.setDefaultCamera();
		m_videoCompositor.setDefaultFov();

		leftLayer->setRect(rect);
		leftLayer->setPosition(leftLayerPos);
		leftLayer->setRotation(leftLayerAngle);

		rightLayer->setRect(rect);
		rightLayer->setPosition(rightLayerPos);
		rightLayer->setRotation(rightLayerAngle);

		bkgdLayer->setRect(bkgdRect); //Background layer does not get changed
	}

	/*
	 * As we are inheriting from ZuazoBase, we need to define open() and close() methods
	 */

	virtual void open() override{
		//Generally you should simply open all containing zuazo classes
		m_videoCompositor.open();

		//Finally open ZuazoBase to respond correctly to isOpen()
		Zuazo::ZuazoBase::open();
	}

	virtual void close() override{
		//Generally you should simply close all containing zuazo classes
		m_videoCompositor.close();

		//You also should reset all the pads
		m_leftVideoSourcePad.reset();
		m_rightVideoSourcePad.reset();
		m_bkgdVideoSourcePad.reset();
		m_videoSourcePad.reset(); //Declared by TVideoSourceBase<>

		//Finally open ZuazoBase to respond correctly to isOpen()
		Zuazo::ZuazoBase::close();
	}
private:
	/*
	 * Here we declare the actual pads, as promised above
	 */
	Zuazo::Video::DummyVideoPad		m_leftVideoSourcePad;
	Zuazo::Video::DummyVideoPad		m_rightVideoSourcePad;
	Zuazo::Video::DummyVideoPad		m_bkgdVideoSourcePad;

	/*
	 * The video composer that will do the trick
	 */
	Zuazo::Processors::Compositor	m_videoCompositor;

	/*
	 * THATS ALL!
	 */
};

int main(int argc, char *argv[]){
	//Initialize zuazo
	if(Zuazo::init() != Zuazo::Errors::OK){
		std::cout << "Error initializing zuazo. Aborting" << std::endl;
		std::terminate();
	}

	if(argc != 4){
		std::cout << "You need to specify at least one input!" << std::endl;
		std::cout << "Usage: " << std::string(argv[0]) << " <file1> <file2> <file3>" << std::endl;
		std::terminate();
	}

	std::unique_ptr<Zuazo::Video::VideoSourceBase> leftSource(Zuazo::videoSourceFromFile(std::string(argv[1])));
	std::unique_ptr<Zuazo::Video::VideoSourceBase> rightSource(Zuazo::videoSourceFromFile(std::string(argv[2])));
	std::unique_ptr<Zuazo::Video::VideoSourceBase> backgroud(Zuazo::videoSourceFromFile(std::string(argv[3])));

	//Create a window for outputting the resulting mix
	//Window only cares about "res" "frameRate" parameters of the given VideoMode
	//A window title can be provided after the VideoMode
	Zuazo::Consumers::Window window(
			Zuazo::Utils::VideoMode{
				.res=Zuazo::Utils::Resolution(1280, 720),
				.frameRate=Zuazo::Utils::Rational(30.0)
			},
			"Example 4"
	);

	//Create the previously defined side-by-side generator
	SideBySide sbsGenerator(
			Zuazo::Utils::VideoMode{
				.pixFmt=Zuazo::Utils::PixelFormats::RGB32,
				.res=Zuazo::Utils::Resolution(1280, 720),
			}
	);


	//Feed the composer into the window
	window.videoIn << sbsGenerator.videoOut;

	//Feed the side by side generator's inputs
	if(leftSource)
		sbsGenerator.leftVideoIn << leftSource->videoOut;
	if(rightSource)
		sbsGenerator.rightVideoIn << rightSource->videoOut;
	if(backgroud)
		sbsGenerator.bkgdVideoIn << backgroud->videoOut;


	std::cout << "Program running... Press enter to quit" << std::endl;
	getchar();

	//You should always close the objects before calling end()
	//Deleting them is also OK
	window.close();
	sbsGenerator.close();

	Zuazo::end();
}
