#include <usart.h>using namespace Mcucpp;int main(){#if defined(MCUCPP_HAS_USART1)	Usart1::Init(9600);	while(1)	{		if(Usart1::RxReady())		{			uint8_t ch = Usart1::Read();			Usart1::Write(ch);		}	}#endif}