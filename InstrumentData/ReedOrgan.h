/*
 * ReedOrgan.h
 *
 *  Created on: Apr 22, 2018
 *      Author: Alec
 */

#ifndef INSTRUMENTDATA_REEDORGAN_H_
#define INSTRUMENTDATA_REEDORGAN_H_

const int16 reed_organ_fsharp4_smpls[128] = {
    1324, 5499, 9943, 13541, 16401, 19225, 22587, 25971,
    28513, 29904, 30722, 31458, 32077, 32089, 31443, 30482,
    29984, 30563, 32032, 32767, 30982, 26915, 21812, 16238,
    10129, 4020, -939, -3800, -4603, -4669, -5270, -6571,
    -7768, -8563, -9299, -10106, -10717, -10939, -10905, -10778,
    -10817, -11273, -12031, -12842, -13601, -14402, -15409, -16675,
    -17773, -18189, -17987, -17440, -16392, -14199, -10529, -5426,
    -163, 3757, 6364, 8660, 10853, 11955, 11281, 9407,
    7726, 7050, 6678, 5631, 3878, 2127, 987, 61,
    -1531, -3822, -5820, -6587, -6033, -4652, -2711, -210,
    3044, 7278, 12155, 16173, 17375, 15770, 12423, 7953,
    2759, -2480, -6706, -9259, -10174, -10644, -11555, -12751,
    -13291, -12945, -12292, -12045, -12331, -12960, -13518, -13807,
    -13954, -14184, -14311, -14013, -13415, -12999, -13148, -14004,
    -15185, -16070, -16544, -16605, -16279, -15014, -12523, -8784,
    -5293, -3822, -4313, -4926, -4708, -3768, -3004, -1887
};

const EnvelopeDeltaPair reed_organ_fsharp4_deltas[5] = {
     {4.2667e-04, 4.2667e-04}, {-1.2688e-04, 1.0667e-03}, {0.0000e+00, 0.0000e+00},
     {-3.7591e-04, 4.2667e-04}, {0.0000e+00, 0.0000e+00}
 };

const Envelope reed_organ_fsharp4_env = { &reed_organ_fsharp4_deltas[0], 3, 4 };

const NoteSample reed_organ_fsharp4 = {
    128,
    127,
    &reed_organ_fsharp4_smpls[0],
    &reed_organ_fsharp4_env
};
//------------------------------------------------------------------

const int16 reed_organ_fsharp6_smpls[64] = {
    543, -2223, -3208, -925, 4145, 11202, 14122, 11225,
    4219, -2387, -6437, -9219, -10753, -11525, -12335, -13456,
    -14293, -14698, -14702, -15735, -17606, -19077, -18752, -15354,
    -12070, -10674, -9849, -8066, -3346, 3214, 9806, 16216,
    21433, 25280, 27939, 29417, 31379, 32767, 29997, 22699,
    13287, 6028, 2211, -235, -2121, -3956, -5419, -6266,
    -7268, -8806, -11082, -13886, -16138, -16669, -14351, -8291,
    -1279, 4739, 8670, 9233, 8564, 6953, 4971, 2462
};

const EnvelopeDeltaPair reed_organ_fsharp6_deltas[5] = {
     {5.3333e-04, 5.3333e-04}, {0.0000e+00, 2.1333e-03}, {0.0000e+00, 0.0000e+00},
     {-4.2667e-04, 4.2667e-04}, {0.0000e+00, 0.0000e+00}
 };

const Envelope reed_organ_fsharp6_env = {&reed_organ_fsharp6_deltas[0], 3, 4 };

const NoteSample reed_organ_fsharp6 = {
    64,
    63,
    &reed_organ_fsharp6_smpls[0],
    &reed_organ_fsharp6_env
};
//-------------------------------------------------------------------

const int16 reed_organ_c8_smpls[32] = {
    -722, -4736, -10509, -14443, -13792, -8211, -263, 5593,
    6123, 3114, 1843, 4633, 7659, 6034, -89, -6641,
    -11086, -14231, -17218, -19548, -20202, -18986, -15720, -9558,
    10, 11741, 22930, 30709, 32767, 27893, 17570, 6600
};

const EnvelopeDeltaPair reed_organ_c8_deltas[5] = {
     {4.8485e-04, 4.8485e-04}, {0.0000e+00, 2.1333e-03}, {0.0000e+00, 0.0000e+00},
     {-4.2667e-04, 4.2667e-04}, {0.0000e+00, 0.0000e+00}
 };

const Envelope reed_organ_c8_env = { &reed_organ_c8_deltas[0], 3, 4 };

const NoteSample reed_organ_c8 = {
    32,
    31,
    &reed_organ_c8_smpls[0],
    &reed_organ_c8_env
};

//----------------------------------------------------------------------

const NoteSample * reed_organ_note_samples[4] =
{
    &reed_organ_fsharp4, &reed_organ_fsharp4, &reed_organ_fsharp6, &reed_organ_c8
};

const Instrument reed_organ = { 5, 3, &reed_organ_note_samples[0] };

#endif /* INSTRUMENTDATA_REEDORGAN_H_ */
