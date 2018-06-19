#include "playerData.h"

playerData::playerData()
{
	for (int i = 0; i<3; i++)
	{
		xPos[i] = 0;
		yPos[i] = 0;
		lastMsgTime[i] = 0;
	}
}

void playerData::linerPrediction(int serverTime)
{
	float x_vel = (xPos[0] - xPos[1]) / (lastMsgTime[0] - lastMsgTime[1]);//veclocity/ms
	float y_vel = (yPos[0] - yPos[1]) / (lastMsgTime[0] - lastMsgTime[1]);
	
	//calc time passed since message sent
	float t = (serverTime - lastMsgTime[0]);

	//test before including properly
	x_ = xPos[0]+ (x_vel * t);
	y_ = yPos[0] + (y_vel * t);

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