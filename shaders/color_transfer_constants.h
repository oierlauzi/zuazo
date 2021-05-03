//Constants specified for a hypothetic 16bit system. Unaccurate for higher precisions
const float ITU_NARROW_MIN_Y = 16.0f*256.0f / 65535.0f;
const float ITU_NARROW_SPAN_Y = 219.0f*256.0f / 65535.0f;
const float ITU_NARROW_MIN_C = 16.0f*256.0f / 65535.0f;
const float ITU_NARROW_SPAN_C = 224.0f*256.0f / 65535.0f;

const float BT1886_GAIN = 4.5f;
const float BT1886_ALPHA = 1.09929682680944f;
const float BT1886_BETA = 0.018053968510807f;
const float BT1886_GAMMA = 1.0f/0.45f; //2.222222...

const float IEC61966_2_1_GAIN = 12.92f;
const float IEC61966_2_1_ALPHA = 1.055f;
const float IEC61966_2_1_BETA = 0.0031308f;
const float IEC61966_2_1_GAMMA = 2.4;

const float SMPTE240M_GAIN = 4.0f;
const float SMPTE240M_ALPHA = 1.1115f;
const float SMPTE240M_BETA =  0.0228f;
const float SMPTE240M_GAMMA = 1.0f/0.45; //2.222222

const float SMPTE2084_C1 = 107.0f / 128.0f;
const float SMPTE2084_C2 = 2413.0f / 128.0f;
const float SMPTE2084_C3 = 2392.0f / 128.0f;  //ct_SMPTE2084_C1 + ct_SMPTE2084_C2 - 1;
const float SMPTE2084_M1 = 1305.0f / 8192.0f;
const float SMPTE2084_M2 = 2523.0f / 32.0f;

const float ARIB_STD_B67_A = 0.17883277f;
const float ARIB_STD_B67_B = 0.28466892f; //1 - 4a
const float ARIB_STD_B67_C = 0.55991073f; //0.5 - a ln(4a)