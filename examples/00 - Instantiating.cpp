/*
 * This example shows how instantiate (or initialize) the library
 * 
 * How to compile:
 * c++ 00\ -\ Instantiating.cpp -std=c++17 -Wall -Wextra -lzuazo -ldl -lpthread
 */

#include <zuazo/Instance.h>

int main() {
	/*
	 * First of all create an application info struct which contains
	 * all required information to properly instantiate Zuazo
	 */
	Zuazo::Instance::ApplicationInfo appInfo(
		"Example 00",								//Application's name
		Zuazo::Version(0, 1, 0),					//Application's version
		Zuazo::Verbosity::GEQ_INFO,					//Verbosity 
		{}											//Modules that are going to be used
	);

	/*
	 * If you're too lazy you can leave it default initialized
	 */
	Zuazo::Instance::ApplicationInfo defaultInitAppInfo;
	ZUAZO_IGNORE_PARAM(defaultInitAppInfo); //This line is here only to avoid warnings

	/*
	 * Finally instantiate the library. 
	 * Tip: Move the appinfo in order to save memory allocations.
	 */
	Zuazo::Instance instance(std::move(appInfo));

	/*
	 * For now on, you're free to use Zuazo objects and functions.
	 * The instance will be destroyed at the end of the scope.
	 */

	getchar();
}