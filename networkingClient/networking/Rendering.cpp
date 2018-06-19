#include "Rendering.h"

void Rendering::LoadSprites()
{
		if (!playerTexture.loadFromFile("artAssets/circle.jpg"))
		{
			cout << "error\n";
		}

		playerSprite.setTexture(playerTexture);
		playerSprite.setScale(0.5, 0.5);
}

void Rendering::movePlayer(int x, int y)
{
	playerSprite.setPosition(x, y);
}
void Rendering::addPlayer(int x, int y)
{
	sf::Sprite temp = playerSprite;
	temp.setPosition(x, y);

	otherSprites.push_back(temp);

}
void Rendering::renderOthers(int x, int y, int spriteNo)
{
	//move other sprites
	otherSprites[spriteNo].setPosition(x, y);
}