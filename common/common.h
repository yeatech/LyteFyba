; Common definitions for TestICal, monitor, monolith and BSL

InitSP		EQU		$300			; Initial value of stack pointer

; Port 1 bit masks
Piezo		EQU		1<<0			; Piezo on P1.0
TxMi		EQU		1<<1			; TA0/P1.1. Code requires TxMi bit somewhere to right of TxPl bit
Rx			EQU		1<<2			; Receive data on P1.2 (bit number >3 costs 2 words)
ActLed		EQU		1<<3			; Actvity LED (blue) inverted on P1.3
VrefP		EQU		1<<4			; Analog reference output. Sometimes changed to low digital output
TxPl		EQU		1<<5			; SCLK/P1.5. Can be made inverse of TA0 by hardware.
;PiezoPortDIR EQU	P1DIR			; Piezo output is on port 1
;PiezoPortSEL EQU	P1SEL
;PiezoPortOUT EQU	P1OUT
;SocPortDIR	EQU		P1DIR			; Soc meter PWM output is on port 1
;SocPortSEL	EQU		P1SEL
;SocPortOUT	EQU		P1OUT
;TxMiPortDIR	EQU		P1DIR			; Transmit- to CMUs output is on port 1
;TxMiPortSEL	EQU		P1SEL
;TxMiPortSEL2 EQU	P1SEL2
;TxMiPortOUT	EQU		P1OUT

; Port 2 bit masks
Bypass		EQU		1<<6			; Bypass transistor on P2.6
RelayM		EQU		1<<6			; IMU relay for HazV- test on P2.6
ErrLed		EQU		1<<7			; Error LED (red) on P2.7
RelayP		EQU		1<<7			; IMU relay for HazV+ test on P2.7
;BypPortDIR	EQU		P2DIR			; Bypass MOSFET output is on port 2
;BypPortSEL	EQU		P2SEL
;BypPortOUT	EQU		P2OUT

; ADC channel numbers
TouchVChan	EQU		$3				; Touch (insulation test) voltage (IMU only)
BoltVPlChan	EQU		$3				; Bolt+ voltage (CMU only)
VRefPChan	EQU		$4				; Vref+ out
LinkVChan	EQU		$6				; Link voltage
CellVChan	EQU		$7				; Cell voltage
TempChan	EQU		$A				; Temperature
NumSamples	EQU		16				; Number of ADC over-samples (typ. 4 or 16)
; To get n more bits of ADC resolution, add up 4^n samples and shift the result right by n bits.
; i.e. By adding up 4^n samples you get 2n more bits in the result,
; but half of them are noise and should be thrown away,
; leaving us with n bits of additional information.

#define		PROG_START	$E000		// Start of program image in flash memory. Ends at $FDFF



#define		WATCHDOG	1			// True if watchdog timer is to be used (only turn off for debugging)
									// Turning it off doesn't work because BSL will still clear and
									// restart the watchdog timer on every call to ReadByte.
#define		BSL2_START	$FE00		// Start of BSL2 image in flash memory. Ends at $FFFD

; The address BSL2 downloads to is usually the same as PROG_START,
; but when making a transition between different download sizes, the version of TestICal that does the
; update to the new BSL2 will still need to be the old size, so it can be downloaded by the old BSL2.
#define		PROG_START_FOR_BSL		PROG_START	// Where the BSL should put the images it downloads.
//#define		PROG_START_FOR_BSL		$E000	// Would be used temporarily while changing to a BSL that
										//	loads a different sized image to the one it is contained in.
										//	The password may need changing too, in both BSL2 and Monitor.

			ORG		$1010			; Stay away from manufacturer supplied data
			; Calibration data
DATAVERS		EQU		6			; This is version 6 of the CMU info-flash data structure
infoDataStart						; Used when copying between ram and info-flash
infoVoltCal		ds		2			; Voltage scale calibration word
infoTempCal		ds		1			; Temperature offset calibration for internal sensor
infoLinkCal		ds		1			; Link voltage offset calibration data
info8MHzCalD	ds		1			; 8 MHz DCO frequency calibration byte
info8MHzCalB	ds		1			; 8 MHz DCO range calibration byte
infoID			ds		1			; Cell/CMU identifier byte; first cell is 1; written by 'i' cmd
infoDataVers	ds		1			; Data Version byte (cannot move). Must be set to DATAVERS value above
; Note that xxxDataEnd is one PAST the last calibration byte, i.e. the address of the start of what
;	comes after the calibration data
infoDataEnd							; Used when copying between ram and info-flash

; To allow moving old calibration data from the end of the A segment to the new D segment location
oldInfoDataStart	EQU	$10F8
oldInfoDataVers		EQU $10FF