#include <SFML\Graphics.hpp>
#include<Windows.h>
#include "winuser.h"

class Screen
{
public:
	bool CloseScreen()
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
		{
			return true;
		}
	}
	
};
