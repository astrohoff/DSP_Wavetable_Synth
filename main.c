#include <DSP28x_Project.h>
#include "InitAIC23.h"
#include <math.h>

// Not sure why this isn't closer to 48000, but this is the interrupt
// frequency that the DAD reports and it sounds much better.
#define SMPL_RATE       46875
// Max # of simultaneous notes.
#define POLYPHONY_CNT   24
#define ONE_OVER_127    (1.0 / 127)
#define INV_FS          (1.0 / SMPL_RATE)
#define MSG_QUEUE_SIZE  32

// Values used to control changes in amplitude and time for envelope.
typedef struct EnvelopeDeltaPair
{
    const float32 dAmplitude;
    const float32 dIndex;
} EnvelopeDeltaPair;

typedef struct Envelope{
    const EnvelopeDeltaPair * deltas;
    const int16 releaseIndex;
    const int16 deadIndex;
} Envelope;

// Wavetable + info used to synthesize a note.
typedef struct NoteSample{
    // How many audio samples.
    const int16 dataCount;
    // Precomputed mask for wavetable wrapping.
    const int16 dataIndexMask;
    // Audio samples.
    const int16 * data;
    const Envelope * envelope;
} NoteSample;

// Collection of NoteSamples.
typedef struct Instrument{
    // MIDI note number must be shifted and masked to get index into
    // NoteSample array.
    const int16 noteNumShift;
    const int16 noteNumMsk;
    const NoteSample ** noteSmpls;
} Instrument;

#include "InstrumentData.h"

// Set of persistent info used while synthesizing a note.
typedef struct WaveGenerator {
    int16 noteNum;
    int16 chanNum;
    const NoteSample * noteSmpl;
    // Index into wavetable.
    float32 sampleIndex;
    float32 volume;
    // Envelope position and level.
    float32 envIndex;
    float32 envLevel;
} WaveGenerator;

typedef struct MidiChannel{
    float32 volume;
    int16 instrumentNum;
}MidiChannel;

// Left or right.
volatile int16 codecChanNum;
// Output sample to codec, used for both codec channels.
int32 outSmpl;
// Last completed synthesis output sample.
volatile int32 synthOutSmpl;
// Does a new sample need to be synthesized.
int16 doSynth;
// MIDI note to normalized frequency mapping.
// Computed at startup.
float32 nrmFreqLookup[128];
WaveGenerator wavGens[POLYPHONY_CNT];
MidiChannel midiChannels[16];
// When a complete midi message (2 or 3 bytes) is received, it is buffered here.
int16 msgQueue[MSG_QUEUE_SIZE][4];
int16 queueReadIndex, queueWriteIndex, msgFragNum;

void InitVars();
interrupt void McBspSampleISR();
interrupt void SciReceiveISR();
void MySciInit();
void NoteOff(int16 chanNum, int16 noteNum, int16 velocity);
void NoteOn(int16 chanNum, int16 noteNum, int16 velocity);
void NoteAftertouch(int16 chanNum, int16 noteNum, int16 pressure);
void ControlChange(int16 chanNum, int16 ctrlNum, int16 ctrlVal);
void ProgramChange(int16 chanNum, int16 pgrmNum);
void ChannelAftertouch(int16 chanNum, int16 pressure);
void PitchBendChange(int16 chanNum, int16 lsbs, int16 msbs);

int main(void)
{
    DisableDog();
    InitPll(10, 3);
    InitPeripheralClocks();

    InitVars();

    // Interrupts.
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    EALLOW;
    // Codec McBSP interface.
    PieVectTable.MRINTB = McBspSampleISR;
    PieCtrlRegs.PIEIER6.bit.INTx3 = 1;
    IER |= M_INT6;
    // SCIB.
    PieVectTable.SCIRXINTB = SciReceiveISR;
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;
    IER |= M_INT9;
    EnableInterrupts();

    EALLOW;
    // Timing monitoring pins.
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;

    // Init peripherals.
    InitSPIA();
    InitMcBSPb();
    InitAIC23();
    MySciInit();

    float32 indexFraction, synthNoteSmpl;
    float32 synthSum;
    int16 intIndex;
    WaveGenerator * wg;
    const NoteSample * ns;

	while(1)
	{
	    // Synthesize new sample if needed.
	    if(doSynth)
	    {
            GpioDataRegs.GPADAT.bit.GPIO0 = 1;
	        synthSum = 0;
	        for(int16 i = 0; i < POLYPHONY_CNT; i++)
	        {
	            wg = &wavGens[i];
	            // Don't render dead notes.
	            if(wg->envIndex < wg->noteSmpl->envelope->deadIndex)
	            {
	                // Temporary variables used to reduce repetition and indirection.
                    intIndex = (int16)wg->sampleIndex;
                    indexFraction = wg->sampleIndex - intIndex;
                    ns = wg->noteSmpl;
                    intIndex &= ns->dataIndexMask;

                    // Sample from wavetable with interpolation.
                    synthNoteSmpl = ns->data[intIndex] * (1-indexFraction) +  ns->data[(intIndex + 1) & ns->dataIndexMask] * indexFraction;
                    // Scale sample by note and channel volume.
                    synthNoteSmpl *= wg->volume * midiChannels[wg->chanNum].volume;

                    // Scale by envelope level.
                    wg->envLevel += ns->envelope->deltas[(int16)wg->envIndex].dAmplitude;
                    // Silence if reached envelope dead zone.
                    //wg->envLevel *= wg->envIndex < ns->envelope->deadIndex;
                    synthNoteSmpl *= wg->envLevel;

                    // Add note sample to sum.
                    synthSum += synthNoteSmpl;

                    // Increment wavetable and enveloep indices.
                    wg->sampleIndex = intIndex + indexFraction + nrmFreqLookup[wg->noteNum] * ns->dataCount;
                    wg->envIndex += ns->envelope->deltas[(int16)wg->envIndex].dIndex;
	            }

	        }
	        // Amplify and sum as synth output.
	        synthOutSmpl = synthSum * 2000;
	        // Mark that synthesis is complete.
	        doSynth = 0;
            GpioDataRegs.GPADAT.bit.GPIO0 = 0;
	    }
	    // Process next MIDI message if queue not empty.
	    if(queueReadIndex != queueWriteIndex)
	    {
	        // Set message components to parameters for readability.
	        int16 mEvent = msgQueue[queueReadIndex][0] & 0xF0;
            int16 chanNum = msgQueue[queueReadIndex][0] & 0x0F;
            int16 param1 = msgQueue[queueReadIndex][1];
            int16 param2 = msgQueue[queueReadIndex][2];

            // Call appropriate function if applicable.
            if(mEvent == 0x80)
                NoteOff(chanNum, param1, param2);
            else if (mEvent == 0x90)
                NoteOn(chanNum, param1, param2);
            else if (mEvent == 0xA0)
                NoteAftertouch(chanNum, param1, param2);
            else if(mEvent == 0xB0)
                ControlChange(chanNum, param1, param2);
            else if(mEvent == 0xC0)
                ProgramChange(chanNum, param1);
            else if(mEvent == 0xD0)
                ChannelAftertouch(chanNum, param1);
            else if(mEvent == 0xE0)
                PitchBendChange(chanNum, param1, param2);

            queueReadIndex = (queueReadIndex + 1) & (MSG_QUEUE_SIZE - 1);
	    }
	}
}

void InitVars()
{
    // Init normalized frequency lookup.
    for(int i = 0; i < 128; i++)
    {
        float32 noteFreq = pow(2, (i - 69)/12.0) * 440;
        nrmFreqLookup[i] = noteFreq / SMPL_RATE;
    }
    // Init MIDI channels.
    for(int16 i = 0; i < 16; i++)
    {
        //midiChannels[i].noteSample = &noteSamples[0];
        midiChannels[i].instrumentNum = 0;
        midiChannels[i].volume = 100 * ONE_OVER_127;
    }
    // Init wave generators.
    for(int16 i = 0; i < POLYPHONY_CNT; i++)
    {
        wavGens[i].chanNum = 0;
        wavGens[i].noteNum = i;
        wavGens[i].noteSmpl = instrument_set[0]->noteSmpls[(i >> instrument_set[0]->noteNumShift) & instrument_set[0]->noteNumMsk];
        wavGens[i].envIndex = wavGens[i].noteSmpl->envelope->deadIndex;
        wavGens[i].envLevel = 0;
        wavGens[i].sampleIndex = 0;
        wavGens[i].volume = 0;
    }
    // Init message queue.
    for(int i = 0; i < MSG_QUEUE_SIZE; i++)
    {
        for(int j = 0; j < 4; j++)
            msgQueue[i][j] = 0;
    }
    queueWriteIndex = 0;
    queueReadIndex = 0;
    msgFragNum = 0;
    // Init other vars.
    codecChanNum = 0;
    synthOutSmpl = 0;
    outSmpl = 0;
    doSynth = 0;
}

interrupt void McBspSampleISR()
{
    long inSmpl = ((long)McbspbRegs.DRR2.all << 16) | McbspbRegs.DRR1.all;

    if(codecChanNum == 0)
    {
        outSmpl = synthOutSmpl;
        doSynth = 1;
    }

    //outSmpl = inSmpl;
    McbspbRegs.DXR2.all = outSmpl >> 16;
    McbspbRegs.DXR1.all = outSmpl & 0xFFFF;

    codecChanNum = (codecChanNum + 1) & 1;

    PieCtrlRegs.PIEACK.bit.ACK6 = 1;
}

interrupt void SciReceiveISR()
{
    // Place byte in message buffer.
    int16 r = ScibRegs.SCIRXBUF.bit.RXDT;
    // Bit 7 set = status byte; start of new message.
    if(r & 0x80)
    {
        msgFragNum = 0;
    }
    msgQueue[queueWriteIndex][msgFragNum] = r;
    int16 msgDone = ((msgFragNum == 1) && ((msgQueue[queueWriteIndex][0] & 0xE0) == 0xC0)) ||
                    ((msgFragNum == 2) && ((msgQueue[queueWriteIndex][0] & 0xF0) != 0xF0));
    if(msgDone)
    {
        queueWriteIndex = (queueWriteIndex + 1) & (MSG_QUEUE_SIZE - 1);
        msgFragNum = 0;
    }
    else
    {
        msgFragNum = (msgFragNum + 1) & 3;
    }
    PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

void NoteOff(int16 chanNum, int16 noteNum, int16 velocity)
{
    chanNum &= 0x0F;
    noteNum &= 0x7F;
    velocity &= 0x7F;
    // Search for wave generator to turn off.
    WaveGenerator * wg = &wavGens[0];
    for(int16 i = 0; i < POLYPHONY_CNT; i++)
    {
        // Check that channel and note match, and that note hasn't already been released.
        if(wg->chanNum == chanNum && wg->noteNum == noteNum && wg->envIndex < wg->noteSmpl->envelope->releaseIndex)
        {
            wg->envIndex = wg->noteSmpl->envelope->releaseIndex;
        }
        wg++;
    }
}

void NoteOn(int16 chanNum, int16 noteNum, int16 velocity)
{
    chanNum &= 0x0F;
    noteNum &= 0x7F;
    velocity &= 0x7F;
    // Ignore drums.
    if(chanNum == 9)
        return;
    WaveGenerator * wg = &wavGens[0];
    int16 mostReleasedWgIndex = -1;
    int16 matchIndex = -1;
    float32 mostReleasedAmnt = 0;
    float32 releaseAmnt;
    // Find a wave generator to use.
    for(int16 i = 0; i < POLYPHONY_CNT; i++)
    {
        releaseAmnt = wg->envIndex - wg->noteSmpl->envelope->releaseIndex;
        if(releaseAmnt >= mostReleasedAmnt)
        {
            mostReleasedWgIndex = i;
            mostReleasedAmnt = releaseAmnt;
        }
        if(wg->chanNum == chanNum && wg->noteNum == noteNum)
            matchIndex = i;
        wg++;
    }
    if(matchIndex != -1)
    {
        wg = &wavGens[matchIndex];
    }
    else if(mostReleasedWgIndex != -1)
    {
        wg = &wavGens[mostReleasedWgIndex];
    }
    else
    {
        return;
    }
    wg->noteNum = noteNum;
    wg->sampleIndex = 0;
    wg->chanNum = chanNum;
    wg->volume = velocity * ONE_OVER_127;
    const Instrument * instr = instrument_set[midiChannels[chanNum].instrumentNum];
    wg->noteSmpl = instr->noteSmpls[(noteNum >> instr->noteNumShift) & instr->noteNumMsk];
    if(velocity == 0)
    {
        wg->envIndex = wg->noteSmpl->envelope->deadIndex;
    }
    else
    {
        wg->envIndex = 0;
    }
    wg->envLevel = 0;
}

void NoteAftertouch(int16 chanNum, int16 noteNum, int16 pressure)
{

}

void ControlChange(int16 chanNum, int16 ctrlNum, int16 ctrlVal)
{
    chanNum &= 0x0F;
    ctrlNum &= 0x7F;
    ctrlVal &= 0x7F;
    // Channel volume
    if(ctrlNum == 7)
    {
        midiChannels[chanNum].volume = ctrlVal * ONE_OVER_127;
    }
    // All sound off.
    else if(ctrlNum == 120 && ctrlVal == 0)
    {
        //float32 envDeadIndex = midiChannels[chanNum].noteSample->envelope->deadIndex;
        for(int i = 0; i < POLYPHONY_CNT; i++)
        {
            if(wavGens[i].chanNum == chanNum)
            {
                wavGens[i].envIndex = wavGens[i].noteSmpl->envelope->deadIndex;
            }
        }
    }
}

void ProgramChange(int16 chanNum, int16 pgrmNum)
{
    chanNum &= 0x0F;
    pgrmNum &= 0x7F;
    midiChannels[chanNum].instrumentNum = pgrmNum;
}

void ChannelAftertouch(int16 chanNum, int16 pressure)
{

}

void PitchBendChange(int16 chanNum, int16 lsbs, int16 msbs)
{

}

void MySciInit()
{
    // GPIO.
    EALLOW;
   // Enable internal pull-up for the selected pins
   // Pull-ups can be enabled or disabled disabled by the user.
   // This will enable the pullups for the specified pins.
   // SCITXDB
   GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;
   // SCIRXDB
   GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;

   // Set qualification for selected pins to asynch only
   // This will select asynch (no qualification) for the selected pins.
   GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;

   // Configure SCI-B pins using GPIO regs
   // This specifies which of the possible GPIO pins will be SCI functional
   // pins.
   GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;
   GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;
   EDIS;

   // SCI config.

   ScibRegs.SCICCR.bit.STOPBITS = 0;
   ScibRegs.SCICCR.bit.PARITYENA = 0;
   ScibRegs.SCICCR.bit.LOOPBKENA = 0;
   ScibRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
   ScibRegs.SCICCR.bit.SCICHAR = 7;

   ScibRegs.SCICTL1.bit.RXERRINTENA = 1;
   ScibRegs.SCICTL1.bit.TXENA = 0;
   ScibRegs.SCICTL1.bit.RXENA = 1;

   // 9600
   //ScibRegs.SCIHBAUD = 0x01;
   //ScibRegs.SCILBAUD = 0xE7;
   // 19200
   //ScibRegs.SCIHBAUD = 0x00;
   //ScibRegs.SCILBAUD = 0xF2;
   // 57600
   ScibRegs.SCIHBAUD = 0;
   ScibRegs.SCILBAUD = 80;
   // 114329 (not working)
   //ScibRegs.SCIHBAUD = 0x00;
   //ScibRegs.SCILBAUD = 0x40;

   ScibRegs.SCICTL2.bit.TXINTENA =0;
   ScibRegs.SCICTL2.bit.RXBKINTENA =1;

   ScibRegs.SCICTL1.bit.SWRESET = 1;
}
