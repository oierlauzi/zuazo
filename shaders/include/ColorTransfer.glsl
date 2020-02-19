#ifdef __cplusplus
	#define ZUAZO_IF_CPP(x) x
#else
	#define ZUAZO_IF_CPP(x)
#endif

struct ZUAZO_IF_CPP(alignas(sizeof(float) * 4)) InputTexture {
	float a;
};