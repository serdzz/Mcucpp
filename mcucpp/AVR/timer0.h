//*****************************************************************************
//
// Author		: Konstantin Chizhov
// Date			: 2013
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
// Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer.

// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************

#pragma once
	
namespace Mcucpp
{
	namespace Timers
	{

	#ifdef TIMSK0
	#define InterruptMaskReg TIMSK0
	#else
	#define InterruptMaskReg TIMSK
	#endif

	#ifdef TIFR0
	#define InterruptFlagsReg TIFR0
	#else
	#define InterruptFlagsReg TIFR
	#endif

	#ifdef TCCR0A
	#define ControlRegA TCCR0A
	#else
	#define ControlRegA TCCR0
	#define ControlRegB TCCR0
	#endif

	#ifdef TCCR0B
	#define ControlRegB TCCR0B
	#endif


		class BaseTimer0
		{
			public:
			typedef uint8_t DataT;
			enum {MaxValue = 255};
#if defined(__ATmega64__) || defined(__AVR_ATmega64__)
			enum ClockDivider
			{
				DivStop		= (0<<CS22) | (0<<CS21) | (0<<CS20), 
				Div1 		= (0<<CS22) | (0<<CS21) | (1<<CS20),
				Div8 		= (0<<CS22) | (1<<CS21) | (0<<CS20),
				Div32 		= (0<<CS22) | (1<<CS21) | (1<<CS20),
				Div64 		= (1<<CS22) | (0<<CS21) | (0<<CS20),
				Div128 		= (1<<CS22) | (0<<CS21) | (1<<CS20),
				Div256 		= (1<<CS22) | (1<<CS21) | (0<<CS20),
				Div1024 	= (1<<CS22) | (1<<CS21) | (1<<CS20)
			};
#else
			enum ClockDivider
			{
				DivStop=0, 
				Div1 		= (1<<CS00), 
				Div8 		= (1<<CS01), 
				Div64 		= (1<<CS00) | (1<<CS01), 
				Div256 		= (1<<CS02), 
				Div1024 	= (1<<CS02) | (1<<CS00), 
				ExtFalling	= (1<<CS02) | (1<<CS01), 
				ExtRising	= (1<<CS02) | (1<<CS01) | (1<<CS00)
			};
#endif

			enum {ClockDividerMask = ~((1<<CS02) | (1<<CS01) | (1<<CS00))};

			template<unsigned Number> struct Divider;
		
			static void Set(DataT val)
			{
				TCNT0 = val;
			}

			static DataT Get()
			{
				return TCNT0;
			}

			static void Stop()
			{
				ControlRegB = 0;
			}

			static void Clear()
			{
				TCNT0 = 0;
			}
			/*
			static void ClearPrescaller()
			{
				SFIOR |= (1 << PSR10);
			}*/

			static void Start(ClockDivider divider)
			{
				ControlRegB = (ControlRegB & ClockDividerMask) | divider;
			}

			static void EnableInterrupt()
			{
				InterruptMaskReg |= (1 << TOIE0);
			}

			static bool IsInterrupt()
			{
				return InterruptFlagsReg & (1<<TOV0);
			}
			
			static void ClearInterruptFlag()
			{
				InterruptFlagsReg |= (1<<TOV0);
			}
		};

		template<> struct BaseTimer0::Divider <0> { static const ClockDivider value = Div1;		enum {Div = 1}; };
		template<> struct BaseTimer0::Divider <1> { static const ClockDivider value = Div8;		enum {Div = 8}; };
		template<> struct BaseTimer0::Divider <2> { static const ClockDivider value = Div64;	enum {Div = 64}; };
		template<> struct BaseTimer0::Divider <3> { static const ClockDivider value = Div256;	enum {Div = 256}; };
		template<> struct BaseTimer0::Divider <4> { static const ClockDivider value = Div1024;	enum {Div = 1024}; };
		

	//mega16, mega32
	#if defined (WGM00) && !defined(WGM02) 

		class Timer0 : public BaseTimer0
		{		
			public:
		
			enum TimerMode
			{
				Normal 			= (0 << WGM01) | (0 << WGM00),
				PwmPhaseCorrect = (0 << WGM01) | (1 << WGM00),
				ClearOnMatch 	= (1 << WGM01) | (0 << WGM00),
				PwmFast 		= (1 << WGM01) | (1 << WGM00)
			};

			enum {TimerModeClearMask = ~(1 << WGM01) | (1 << WGM00)};

			static void SetMode(TimerMode mode)
			{
				ControlRegA = (ControlRegA & TimerModeClearMask) | mode;
			}

			template<int number> class OutputCompare;
		};

		template<> class Timer0::OutputCompare<0>
		{
		public:
			static void Set(DataT val)
			{
				OCR0 = val;
			}

			static DataT Get()
			{
				return OCR0;
			}

			static void EnableInterrupt()
			{
				InterruptMaskReg |= (1 << OCIE0);
			}

			static bool IsInterrupt()
			{
				return InterruptFlagsReg & (1<<OCF0);
			}
		
			static void ClearInterruptFlag()
			{
				InterruptFlagsReg |= (1<<OCF0);
			}
		};
	#elif defined(WGM02) 

		class Timer0 : public BaseTimer0
		{		
			public:
		
			enum TimerMode
			{
				Normal 			= (0 << WGM02) | (0 << WGM01) | (0 << WGM00),
				PwmPhaseCorrect = (0 << WGM02) | (0 << WGM01) | (1 << WGM00),
				ClearOnMatch 	= (0 << WGM02) | (1 << WGM01) | (0 << WGM00),
				PwmFast 		= (0 << WGM02) | (1 << WGM01) | (1 << WGM00),
				//Reserved1		= (1 << WGM02) | (0 << WGM01) | (0 << WGM00),
				PwmPhaseCorrectToOCRA = (1 << WGM02) | (0 << WGM01) | (1 << WGM00),
				//Reserved2 	= (1 << WGM02) | (1 << WGM01) | (0 << WGM00),
				PwmFastToOCRA	= (1 << WGM02) | (1 << WGM01) | (1 << WGM00),
			};

			enum {TimerModeAClearMask = ~((1 << WGM01) | (1 << WGM00))};
			enum {TimerModeAMask = ((1 << WGM01) | (1 << WGM00))};
			enum {TimerModeBClearMask = ~((1 << WGM02))};
			enum {TimerModeBMask = ~((1 << WGM02))};


			static void SetMode(TimerMode mode)
			{
				ControlRegA = (ControlRegA & TimerModeAClearMask) | (mode & TimerModeAMask);
				ControlRegB = (ControlRegB & TimerModeBClearMask) | (mode & TimerModeBMask);
			}

			template<int number> class OutputCompare;
		};

		template<> class Timer0::OutputCompare<0>
		{
		public:
			static void Set(DataT val)
			{
				OCR0A = val;
			}

			static DataT Get()
			{
				return OCR0A;
			}

			static void EnableInterrupt()
			{
				InterruptMaskReg |= (1 << OCIE0A);
			}

			static bool IsInterrupt()
			{
				return InterruptFlagsReg & (1<<OCF0A);
			}
		
			static void ClearInterruptFlag()
			{
				InterruptFlagsReg |= (1<<OCF0A);
			}
		};

		template<> class Timer0::OutputCompare<1>
		{
		public:
			static void Set(DataT val)
			{
				OCR0B = val;
			}

			static DataT Get()
			{
				return OCR0B;
			}

			static void EnableInterrupt()
			{
				InterruptMaskReg |= (1 << OCIE0B);
			}

			static bool IsInterrupt()
			{
				return InterruptFlagsReg & (1<<OCF0B);
			}
		
			static void ClearInterruptFlag()
			{
				InterruptFlagsReg |= (1<<OCF0B);
			}
		};

	#else
		class Timer0 : public BaseTimer0
		{
		};
	#endif


	#undef InterruptMaskReg 
	#undef InterruptFlagsReg 
	#undef ControlRegA
	#undef ControlRegB

	}
}
