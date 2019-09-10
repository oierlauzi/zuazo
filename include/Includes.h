#pragma once

#include "Color.h"
#include "KHR/khrplatform.h"
#include "Math/Matrix.h"
#include "Math/Rational.h"
#include "Math/Vector.h"
#include "PixelComponent.h"
#include "PixelFormat.h"
#include "Resolution.h"
#include "Stream/AsyncSource.h"
#include "Stream/Consumer.h"
#include "Stream/Delay.h"
#include "Stream/DummyPad.h"
#include "Stream/LazySource.h"
#include "Stream/Operators.h"
#include "Stream/Source.h"
#include "Timing/EventBase.h"
#include "Utils/Buffer.h"
#include "Utils/Hasher.h"
#include "Utils/MMBuffer.h"
#include "Utils/Macros.h"
#include "Utils/PhantomClass.h"
#include "Utils/Pool.h"
#include "Videomode.h"
#include "glad/glad.h"


//Defines a shorcut for Zuazo
//If you don't want to define it, use #define DONT_DEFINE_ZZ 
//before including this file
#ifndef DONT_DEFINE_ZZ
#define zz Zuazo
#endif
