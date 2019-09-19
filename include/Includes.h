#pragma once

#include "Color.h"
#include "Includes.h"
#include "KHR/khrplatform.h"
#include "Math/Matrix.h"
#include "Math/Rational.h"
#include "Math/Vector.h"
#include "PixelComponent.h"
#include "PixelFormat.h"
#include "Resolution.h"
#include "Signal/Consumer.h"
#include "Signal/ConsumerPad.h"
#include "Signal/PadBase.h"
#include "Signal/Signal.h"
#include "Signal/Source.h"
#include "Signal/SourcePad.h"
#include "Timing/Chrono.h"
#include "Timing/EventBase.h"
#include "Timing/PeriodicEvent.h"
#include "Timing/RegularEvent.h"
#include "Timing/Scheduler.h"
#include "Utils/Buffer.h"
#include "Utils/Hasher.h"
#include "Utils/MMBuffer.h"
#include "Utils/Macros.h"
#include "Utils/Pool.h"
#include "glad/glad.h"


//Defines a shorcut for Zuazo
//If you don't want to define it, use #define DONT_DEFINE_ZZ 
//before including this file
#ifndef DONT_DEFINE_ZZ
#define zz Zuazo
#endif
