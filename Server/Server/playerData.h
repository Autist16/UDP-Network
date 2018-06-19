#pragma once

class playerData
{
public:
	playerData();
	void linerPrediction(int serverTime);
	//setters
	void setPos(int i, int newX, int newY)
	{
		xPos[i] = newX;
		yPos[i] = newY;
	}
	void setPlayerNo(int i){
		playerNo = i;
	}
	void setMsgNo(int t)
	{
		msgNo = t;
	}
	void setLastMsgTime(int i, int msgtime)
	{
		lastMsgTime[i] = msgtime;
	}
	void setRecieved(int x)
	{
		msgRecievedTime = x;
	}
	void setLastPred(int x)
	{
		lastPrediction = x;
	}
	//getters
	int getPosX(int i)
	{
		return xPos[i];
	}
	int getPosY(int i)
	{
		return yPos[i];
	}
	int getNo()
	{
		return playerNo;
	}
	int getMsgNo()
	{
		return msgNo;
	}
	int getlastMsgTime(int i)
	{
		return lastMsgTime[i];
	}
	int getLastPred()
	{
		return lastPrediction;
	}
	int getXpre()
	{
		return x_;
	}
	int getYpre()
	{
		return y_;
	}
	int getRecieved()
	{
		return msgRecievedTime;
	}


private:
	//variables
	int x_;
	int y_;
	int msgNo;
	int lastMsgTime[3];
	int msgRecievedTime;
	int lastPrediction;
	int playerNo;
	int xPos[3];
	int yPos[3];
};