/*
 * InstrumentData.h
 *
 *  Created on: Apr 21, 2018
 *      Author: Alec
 */

#ifndef INSTRUMENTDATA_H_
#define INSTRUMENTDATA_H_

#include "InstrumentData/P200Piano.h"
#include "InstrumentData/ReedOrgan.h"
#include "InstrumentData/SteelAcoustic.h"
#include "InstrumentData/FingerBass.h"
#include "InstrumentData/Strings.h"
#include "InstrumentData/Trombone.h"
#include "InstrumentData/Flute.h"
#include "InstrumentData/SquareWave.h"
#include "InstrumentData/PizzicatoStrings.h"
#include "InstrumentData/Harp.h"

const Instrument *instrument_set[128] = {
    // Piano (0-7)
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    // Chromatic percussion (8-15)
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    // Organ (16-23)
    &reed_organ, &reed_organ, &reed_organ, &reed_organ,
    &reed_organ, &reed_organ, &reed_organ, &reed_organ,
    // Guitar (24-31)
    &Steel_Acu, &Steel_Acu, &Steel_Acu, &Steel_Acu,
    &Steel_Acu, &Steel_Acu, &Steel_Acu, &Steel_Acu,
    // Bass (32-39)
    &Finger_Bass, &Finger_Bass, &Finger_Bass, &Finger_Bass,
    &Finger_Bass, &Finger_Bass, &Finger_Bass, &Finger_Bass,
    // Strings (40-47)
    &Strings, &Strings, &Strings, &Strings,
    &Strings, &pizzicatoStrings, &Harp, &Strings,
    // Ensemble (48-55)
    &Strings, &Strings, &Strings, &Strings,
    &Strings, &Strings, &Strings, &Strings,
    // Brass (56-63)
    &trombone, &trombone, &trombone, &trombone,
    &trombone, &trombone, &trombone, &trombone,
    // Reed (64-71)
    &trombone, &trombone, &trombone, &trombone,
    &trombone, &trombone, &trombone, &trombone,
    // Pipe (72-79)
    &Flute, &Flute, &Flute, &Flute,
    &Flute, &Flute, &Flute, &Flute,
    // Synth lead (80-87)
    &Square_Wave, &Square_Wave, &Square_Wave, &Square_Wave,
    &Square_Wave, &Square_Wave, &Square_Wave, &Square_Wave,
    // Synth pad (88-95)
    &Square_Wave, &Square_Wave, &Square_Wave, &Square_Wave,
    &Square_Wave, &Square_Wave, &Square_Wave, &Square_Wave,
    // Synth effects (96-103)
    &Square_Wave, &Square_Wave, &Square_Wave, &Square_Wave,
    &Square_Wave, &Square_Wave, &Square_Wave, &Square_Wave,
    // Ethnic (104-111)
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    // Percussive (112-119)
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    // Sound effects (120-127)
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano,
    &P200_Piano, &P200_Piano, &P200_Piano, &P200_Piano
};

#endif /* INSTRUMENTDATA_H_ */
