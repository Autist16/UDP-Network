#include "Player.h"
#include <iostream>

using std::cout;
using std::endl;

Player::Player()
{
	playerNo = 99;
	for (int i = 0; i < 3; i++)
	{
		xPos[i] = 5;
		yPos[i] = 5;
	}
	hasMoved = false;
	msgNo = 0;
}

void Player::controlPlayer()
{
	//moving around
	if (GetAsyncKeyState('S') & 0x0001)
	{
		hasMoved = true;
		if(yPos[0] <= 595)
		{
			yPos[0] += 5;
		}
		else
		{
			yPos[0] = 600;
		}
	}
	else if(GetAsyncKeyState('W') & 0x0001)
	{
		hasMoved = true;
		if (yPos[0] >= 5)
		{
			yPos[0] -= 5;
		}
		else
		{
			yPos[0] = 0;
		}
	}
	else if (GetAsyncKeyState('A') & 0x0001)
	{
		hasMoved = true;
		if (xPos[0] >= 5)
		{
			xPos[0] -= 5;
		}
		else
		{
			xPos[0] = 0;
		}
	}
	else if (GetAsyncKeyState('D') & 0x0001)
	{
		hasMoved = true;
		if(xPos[0] <= 595)
		{
			xPos[0] += 5;
		}
		else
		{
			xPos[0] = 600;
		}
	}
}

void Player::linearPrediction(int clientTime)
{
	float x_vel = (xPos[0] - xPos[1]) / (msgTime[0] - msgTime[1]);//veclocity/ms
	float y_vel = (yPos[0] - yPos[1]) / (msgTime[0] - msgTime[1]);

	//calc time passed since message sent
	float t = (clientTime - msgTime[0]);

	//test before including properly
	int x_ = xPos[0] + (x_vel * t);
	int y_ = yPos[0] + (y_vel * t);

	for (int i = 0; i < 3; i++)
	{
		if (i != 0)
		{
			xPos[i] = xPos[i - 1];
			yPos[i] = yPos[i - 1];
		}
		else
		{
			xPos[i] = x_;
			yPos[i] = y_;
		}
	}
}