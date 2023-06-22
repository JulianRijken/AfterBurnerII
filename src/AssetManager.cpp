#include "pch.h"
#include "AssetManager.h"

#include <utility>


#include "Log.h"
#include "Sprite.h"
#include "Font.h"

void AssetManager::Initialize()
{

	AddFont("AfterBurner", "Fonts/G7 After Burner TTF.ttf",16);

	AddSprite("Bush", "BushWhite.png", 32, Vector2f{ 0.5f,0.0f });
	AddSprite("Disk", "WaterDisk.png", 32, Vector2f{ 0.5f,0.0f });
	AddSprite("Rock", "RockWhite.png", 32, Vector2f{ 0.5f,0.0f });
	AddSprite("Enemy_00", "Enemy_00.png", 32, Vector2f{ 0.5f,0.5f });
	AddSprite("Enemy_01", "Enemy_01.png", 32, Vector2f{ 0.5f,0.5f });
	AddSprite("Enemy_02", "Enemy_02.png", 32, Vector2f{ 0.5f,0.5f });
	AddSprite("AimLocation", "AimLocation.png", 32, Vector2f{ 0.5f,0.5f });
	AddSprite("PlaneLives", "PlaneLives.png", 32, Vector2f{ 0.0f,0.0f },1,3);
	AddSprite("AmmoIcon", "AmmoIcon.png", 32, Vector2f{ 0.0f,0.0f },1,2);
	AddSprite("ShipDeck", "ShipDeck.png", 32, Vector2f{ 0.5f,0.0f });
	AddSprite("Bullet", "Bullet.png", 32, Vector2f{ 0.5f,0.5f }, 1, 2);
	AddSprite("Empty", "Empty.png", 32, Vector2f{ 0,0 });
	AddSprite("EmptyCenter", "Empty.png", 32, Vector2f{ 0.5f,0.5f });
	AddSprite("SkyPlane", "Empty.png", 32, Vector2f{ 0.5f,0.0f });
	AddSprite("GroundPlane", "GroundPlane.png", 224.0f, Vector2f{ 0,0 });
	AddSprite("GroundDetail", "GroundDetail.png", 224.0f, Vector2f{ 0,0 });
	AddSprite("player", "JetSpriteSheet.png", 32, Vector2f{ 0.5f,0.5f }, 3, 5);
	AddSprite("AimTarget", "AimTarget.png", 32, Vector2f{ 0.6f,0.6f }); // 0.6 for center to keep pixels sharp
	AddSprite("BaseUI", "UIBase.png", 32);
	AddSprite("GameOverScreen", "GameOverScreen.png", 32);
	AddSprite("gridCell", "GridCell.png", 32, Vector2f{ 0,0 });
	AddSprite("FluePlane", "FluePlane.png", 32, Vector2f{ 0.0f,1.0f });
	AddSprite("MainMenuLogo", "MainMenuLogo.png", 32, Vector2f{ 0,0 });
	AddSprite("Cursor", "Cursor.png", 32, Vector2f{ 0.5f,0.5f });

	AddSprite("Pillar", "Pillar.png", 32, Vector2f{ 0.5f,0.0f });

	AddSprite("Tower",  "Tower.png",  32, Vector2f{ 0.5f,0.0f });
	AddSprite("Truck",  "Truck.png",  32, Vector2f{ 0.5f,0.0f });
	AddSprite("Tents",  "Tents.png",  32, Vector2f{ 0.5f,0.0f });
	AddSprite("Hangar", "Hangar.png", 32, Vector2f{ 0.5f,0.0f });

	AddSprite("Coin", "Coin.png", 32, Vector2f{ 0.5f,0.5f }, 3, 3,
		{
			{"Rotate",Animation{{{0,0},{0,1},{0,2},{1,0},{1,2},{1,3},{2,0}},7}},
		});

	AddSprite("TargetVisual", "TargetLock_03.png", 32, Vector2f{ 0.5f,0.5f }, 2, 7,
		{
			{"Lock",Animation{{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6}},7}},
			{"UnLock",Animation{{{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6}},7}}
		});


	AddSprite("Cloud", "CloudTrail_01.png", 32, Vector2f{ 0.5f,0.5f }, 1, 5,
		{
			{"Idle",Animation{{{0,0},{0,1},{0,2},{0,3},{0,4}},5}}
		});


	AddSprite("Rocket", "Rocket_02.png", 32, Vector2f{ 0.5f,0.675f }, 1, 3,
		{
			{"Dropping",Animation{{{0,0}}}},
			{"FlyingAway",Animation{{{0,1}}}},
			{"FlyingTowards",Animation{{{0,2}}}},
		});


	AddSprite("Explosion", "Explosion.png", 32, Vector2f{ 0.5f,0.5f }, 1, 2,
		{
			{"Explode",Animation{{{0,0},{0,1}},2}},
		});


	AddSprite("EnemyClose_Blue", "EnemyClose_Blue.png", 32, Vector2f{ 0.5f,0.5f }, 2,7,
		{
			{"Idle",Animation{{{0,0}},0}},
			{"SwingLeft",Animation{{{0,0},{0,1},{0,2},{0,3}},4}},
			{"SwingRight",Animation{{{1,0},{1,1},{1,2},{1,3}},4}},
			{"RollLeft",Animation{{{0,0},{0,1},{0,2},{0,1},{0,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6}},11}},
			{"RollRight",Animation{{{1,0},{1,1},{1,2},{1,1},{1,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6}},11}},
		});


	AddSprite("EnemyClose_Orange", "EnemyClose_Orange.png", 32, Vector2f{ 0.5f,0.5f }, 2, 7,
		{
			{"Idle",Animation{{{0,0}},0}},
			{"SwingLeft",Animation{{{0,0},{0,1},{0,2},{0,3}},4}},
			{"SwingRight",Animation{{{1,0},{1,1},{1,2},{1,3}},4}},
			{"RollLeft",Animation{{{0,0},{0,1},{0,2},{0,1},{0,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6}},11}},
			{"RollRight",Animation{{{1,0},{1,1},{1,2},{1,1},{1,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6}},11}},
		});


	AddSprite("MissionComplete", "MissionComplete.png", 32, Vector2f{ 0.0f,0.0f }, 1, 3,
		{
			{"Won",Animation{{{0,0},{0,1},{0,2}},1.0f / 4.0f}},
		});


	AddSprite("SegaLogo", "SegaLogo.png", 32, Vector2f{ 0.5f,0.5f }, 10, 2,
		{
			{"Shine",Animation{
				{
					{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},
					{0,1},{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1},{8,1},{9,1},
				},20}},
		});
}

void AssetManager::Cleanup()
{
	for (auto const& pair : m_SpritePtrMap)
		delete pair.second;

	for (auto const& pair : m_FontPtrMap)
		delete pair.second;
}


Sprite* AssetManager::GetSprite(const std::string& name)
{
	if (m_SpritePtrMap.contains(name))
		return m_SpritePtrMap.at(name);

	Log(LogType::Warning) << "Sprite " << name << " does not exist!";
	return nullptr;
}

Font* AssetManager::GetFont(const std::string& name)
{
	if (m_FontPtrMap.contains(name))
		return m_FontPtrMap.at(name);

	Log(LogType::Warning) << "Font " << name << " does not exist!";
	return nullptr;
}

void AssetManager::AddSprite(const std::string& assetName, const std::string& path, float pixelsPerUnit, const Vector2f& pivotAlpha,
                             int rowCount, int colCount, std::map<std::string, Animation> animations)
{
	Sprite* newSprite{ new Sprite{assetName,path,pixelsPerUnit,pivotAlpha,rowCount,colCount, std::move(animations)} };
	m_SpritePtrMap.insert({ assetName, newSprite });

	Log(TextColor::BrightWhite, BackgroundColor::Green) << "+ Sprite added: " << assetName;

}

void AssetManager::AddFont(const std::string& assetName, const std::string& fontPath, const int pixelResolution)
{
	Font* newFont{ new Font{assetName,fontPath,pixelResolution} };
	m_FontPtrMap.insert({ assetName, newFont });

	Log(TextColor::BrightWhite, BackgroundColor::Green) << "+ Font added: " << assetName;

}
