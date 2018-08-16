/*
 * Flute.h
 *
 *  Created on: Apr 23, 2018
 *      Author: Alec
 */

#ifndef INSTRUMENTDATA_FLUTE_H_
#define INSTRUMENTDATA_FLUTE_H_

const int16 Flute_E4_smpls[128] = {
    471, 3261, 6388, 9315, 11637, 12923, 13292, 12487,
    10383, 7137, 3478, -523, -4875, -9209, -12708, -15136,
    -16495, -16815, -15900, -14119, -12087, -10168, -8502, -7110,
    -5338, -2954, 9, 3854, 8006, 10933, 12073, 11767,
    10133, 7237, 3471, -996, -6280, -11885, -17156, -22117,
    -26594, -29906, -31913, -32768, -32268, -30449, -27674, -24015,
    -19516, -14782, -10231, -5760, -1457, 2348, 5718, 8885,
    11996, 14926, 17715, 20471, 22885, 24763, 26479, 27857,
    28602, 29173, 29872, 30425, 30938, 31529, 31970, 32214,
    32387, 32427, 32049, 31204, 29982, 28406, 26312, 23778,
    21068, 18338, 15461, 12306, 9101, 5913, 2556, -794,
    -3825, -6775, -9708, -12457, -14880, -16930, -18649, -19912,
    -20307, -20053, -19415, -18159, -16125, -13554, -10584, -7472,
    -4725, -2793, -1677, -1452, -2297, -3770, -5568, -7425,
    -8947, -9962, -10708, -10851, -10391, -9810, -9625, -9343,
    -9044, -8925, -8873, -8141, -7189, -6110, -4678, -2462
};

const EnvelopeDeltaPair Flute_E4_deltas[4] = {
    {2.1333e-04, 2.1333e-04}, {0.0000e+00, 0.0000e+00}, {-2.1333e-03, 2.1333e-03}, {0.0000e+00, 0.0000e+00}
};

const Envelope Flute_E4_env = { &Flute_E4_deltas[0], 2, 3 };

const NoteSample Flute_E4 = {
    128,
    127,
    &Flute_E4_smpls[0],
    &Flute_E4_env
};

//-----------------------------------------------

const int16 Flute_B4_smpls[128] = {
    201, 925, 1531, 1570, 1331, 1064, 512, -520,
    -1967, -3489, -4797, -6043, -7566, -9425, -11398, -13283,
    -14885, -16151, -17135, -17889, -18339, -18362, -17845, -16827,
    -15452, -13815, -11912, -9703, -7175, -4488, -1956, 250,
    2274, 4299, 6301, 7960, 9128, 9939, 10636, 11263,
    11740, 12070, 12324, 12560, 12806, 13113, 13583, 14206,
    14904, 15633, 16516, 17690, 19188, 20854, 22485, 24059,
    25666, 27370, 29048, 30483, 31551, 32275, 32690, 32767,
    32471, 31762, 30577, 28862, 26626, 23976, 21033, 17825,
    14409, 10888, 7376, 3945, 611, -2565, -5482, -8047,
    -10143, -11598, -12337, -12435, -12123, -11458, -10486, -9193,
    -7832, -6653, -5810, -5238, -4881, -4694, -4752, -5160,
    -5891, -6809, -7751, -8710, -9789, -11114, -12572, -13948,
    -15207, -16313, -17281, -18091, -18729, -19154, -19327, -19117,
    -18551, -17880, -17157, -16191, -14941, -13504, -12141, -11133,
    -10334, -9508, -8665, -7609, -6151, -4568, -2952, -1224
};

const EnvelopeDeltaPair Flute_B4_deltas[4] = {
    {4.2667e-04, 4.2667e-04}, {0.0000e+00, 0.0000e+00}, {-2.1333e-03, 2.1333e-03}, {0.0000e+00, 0.0000e+00}
 };

const Envelope Flute_B4_env = { &Flute_B4_deltas[0], 2, 3 };

const NoteSample Flute_B4 = {
    128,
    127,
    &Flute_B4_smpls[0],
    &Flute_B4_env
};
//-----------------------------------------------

const int16 Flute_Fsharp6_smpls[32] = {
    735, 6766, 12958, 17728, 22093, 25061, 28081, 30272,
    31526, 31474, 30558, 29324, 26615, 22364, 16260, 9445,
    2215, -4857, -11237, -16952, -21506, -25454, -28536, -31229,
    -32546, -32768, -32081, -29798, -26860, -21451, -15778, -8053
};

const NoteSample Flute_Fsharp6 = {
    32,
    31,
    &Flute_Fsharp6_smpls[0],
    &Flute_B4_env
};

//----------------------------------------------

const NoteSample * Flute_note_samples[4] = {
    &Flute_E4, &Flute_B4, &Flute_Fsharp6, &Flute_Fsharp6
};

const Instrument Flute = { 5, 3, &Flute_note_samples[0] };

#endif /* INSTRUMENTDATA_FLUTE_H_ */
