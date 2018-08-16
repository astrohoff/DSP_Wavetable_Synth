// Alec Hoffmann
// InitAIC23.c
// Lab 4
// Initialization functions for codec communications.

#include <stdint.h>
#include <DSP28x_Project.h>
#include "AIC23.h"
#include "InitAIC23.h"

/***************** Defines ***************/
#define SmallDelay() for(volatile long i = 0; i < 500000; i++)
/***************** Defines ***************/

static void SpiTransmit(uint16_t data)
{
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG == 1);
    SpiaRegs.SPITXBUF = data;
}


/***************** User Functions *****************/
/*void InitMcBSPb();
void InitSPIA();
void InitAIC23();*/
//void SpiTransmit(uint16_t data);
/***************** User Functions *****************/

void InitAIC23()
{
    uint16_t command;
    command = reset();
    SpiTransmit (command);
    SmallDelay();
    command = softpowerdown();       // Power down everything except device and clocks
    SpiTransmit (command);
    SmallDelay();
    command = linput_volctl(LIV);    // Unmute left line input and maintain default volume
    SpiTransmit (command);
    SmallDelay();
    command = rinput_volctl(RIV);    // Unmute right line input and maintain default volume
    SpiTransmit (command);
    SmallDelay();
    command = lhp_volctl(LHV);       // Left headphone volume control
    SpiTransmit (command);
    SmallDelay();
    command = rhp_volctl(RHV);       // Right headphone volume control
    SpiTransmit (command);
    SmallDelay();
    command = nomicaaudpath();      // Turn on DAC, mute mic
    SpiTransmit (command);
    SmallDelay();
    command = digaudiopath();       // Disable DAC mute, add de-emph
    SpiTransmit (command);
    SmallDelay();

    // I2S
    command = I2Sdigaudinterface(); // AIC23 master mode, I2S mode,32-bit data, LRP=1 to match with XDATADLY=1
    SpiTransmit (command);
    SmallDelay();
    command = CLKsampleratecontrol (SR48);
    SpiTransmit (command);
    SmallDelay();

    command = digact();             // Activate digital interface
    SpiTransmit (command);
    SmallDelay();
    command = nomicpowerup();      // Turn everything on except Mic.
    SpiTransmit (command);
    SmallDelay();
}

void InitMcBSPb()
{
    InitMcbspbGpio();
    EALLOW;

    McbspbRegs.SPCR2.all = 0;                  // Reset FS generator, sample rate generator & transmitter
    McbspbRegs.SPCR1.all = 0;                  // Reset Receiver, Right justify word
    McbspbRegs.SPCR1.bit.RJUST = 2;         // left-justify word in DRR and zero-fill LSBs

    McbspbRegs.MFFINT.all=0x0;               // Disable all interrupts

    McbspbRegs.SPCR1.bit.RINTM = 0;     // McBSP interrupt flag - RRDY
    McbspbRegs.SPCR2.bit.XINTM = 0;     // McBSP interrupt flag - XRDY

    // Clear Receive Control Registers
    McbspbRegs.RCR2.all = 0x0;
    McbspbRegs.RCR1.all = 0x0;

    // Clear Transmit Control Registers
    McbspbRegs.XCR2.all = 0x0;
    McbspbRegs.XCR1.all = 0x0;

    // Set Receive/Transmit to 32-bit operation
    McbspbRegs.RCR2.bit.RWDLEN2 = 5;
    McbspbRegs.RCR1.bit.RWDLEN1 = 5;
    McbspbRegs.XCR2.bit.XWDLEN2 = 5;
    McbspbRegs.XCR1.bit.XWDLEN1 = 5;

    McbspbRegs.RCR2.bit.RPHASE = 1;
    // Dual-phase frame for receive
    McbspbRegs.RCR1.bit.RFRLEN1 = 0;
    // Receive frame length = 1 word in phase 1
    McbspbRegs.RCR2.bit.RFRLEN2 = 0;
    // Receive frame length = 1 word in phase 2

    McbspbRegs.XCR2.bit.XPHASE = 1;      // Dual-phase frame for transmit
    McbspbRegs.XCR1.bit.XFRLEN1 = 0;    // Transmit frame length = 1 word in phase 1
    McbspbRegs.XCR2.bit.XFRLEN2 = 0;    // Transmit frame length = 1 word in phase 2

        // I2S mode: R/XDATDLY = 1 always
    McbspbRegs.RCR2.bit.RDATDLY = 1;
    McbspbRegs.XCR2.bit.XDATDLY = 1;

        // Frame Width = 1 CLKG period, CLKGDV must be 1 as slave
    McbspbRegs.SRGR1.all = 0x0001;
    McbspbRegs.PCR.all=0x0000;

        // Transmit frame synchronization is supplied by an external source via the FSX pin
    McbspbRegs.PCR.bit.FSXM = 0;

        // Receive frame synchronization is supplied by an external source via the FSR pin
    McbspbRegs.PCR.bit.FSRM = 0;

        // Select sample rate generator to be signal on MCLKR pin
    McbspbRegs.PCR.bit.SCLKME = 1;
    McbspbRegs.SRGR2.bit.CLKSM = 0;

        // Receive frame-synchronization pulses are active low - (L-channel first)
    McbspbRegs.PCR.bit.FSRP = 1;

        // Transmit frame-synchronization pulses are active low - (L-channel first)
    McbspbRegs.PCR.bit.FSXP = 1;

        // Receive data is sampled on the rising edge of MCLKR
    McbspbRegs.PCR.bit.CLKRP = 1;

        // Transmit data is sampled on the falling edge of CLKX
    McbspbRegs.PCR.bit.CLKXP = 0;

        // The transmitter gets its clock signal from MCLKX
    McbspbRegs.PCR.bit.CLKXM = 0;

        // The receiver gets its clock signal from MCLKR
    McbspbRegs.PCR.bit.CLKRM = 0;

        // Enable Receive Interrupt
    McbspbRegs.MFFINT.bit.RINT = 1;

        // Ignore unexpected frame sync
    //McbspbRegs.XCR2.bit.XFIG = 1;

    McbspbRegs.SPCR2.all |=0x00C0;
    // Frame sync & sample rate generators pulled out of reset
    delay_loop();
    McbspbRegs.SPCR2.bit.XRST=1;
    // Enable Transmitter
    McbspbRegs.SPCR1.bit.RRST=1;
    // Enable Receiver
    EDIS;
}

void InitSPIA()
{
    InitSpiaGpio();

    EALLOW;

    SpiaRegs.SPICCR.bit.SPISWRESET = 0;

    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiaRegs.SPICCR.bit.SPICHAR = 0xF;

    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;

    SpiaRegs.SPIBRR = 0x0E;

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;

    SpiaRegs.SPIPRI.bit.FREE = 1;
}


void DisableAnalogBypass()
{
    SpiTransmit(RESETREG | RES);
    DELAY_US(50);
    SpiTransmit(LLINVCTL | 0x17);
    SpiTransmit(RLINVCTL | 0x17);
    SpiTransmit(POWDCTL | AIC23MIC);
    DELAY_US(50);
    SpiTransmit(AAUDCTL | MICMUTE);
    DELAY_US(50);
}

void CheckCodecSpi()
{
    uint16_t command;
    command = reset();
    SpiTransmit (command);
    SmallDelay();
    command = softpowerdown();       // Power down everything except device and clocks
    SpiTransmit (command);
    SmallDelay();
    command = linput_volctl(LIV);    // Unmute left line input and maintain default volume
    SpiTransmit (command);
    SmallDelay();
    int recievedData = SpiaRegs.SPIRXBUF;
    SmallDelay();
}

