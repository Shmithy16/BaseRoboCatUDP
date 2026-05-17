#include "RoboCatPCH.hpp"
#include "PointGainTracker.hpp"

std::unique_ptr< World > World::sInstance;

void World::StaticInit()
{
	sInstance.reset(new World());
}

World::World() : mShineHolderId(-1) //Help with AI
{
}


void World::AddGameObject(GameObjectPtr inGameObject)
{
	mGameObjects.push_back(inGameObject);
	inGameObject->SetIndexInWorld(mGameObjects.size() - 1);
}


void World::RemoveGameObject(GameObjectPtr inGameObject) //Help with AI
{
	int index = inGameObject->GetIndexInWorld();

	int lastIndex = mGameObjects.size() - 1;
	if (index != lastIndex)
	{
		mGameObjects[index] = mGameObjects[lastIndex];
		mGameObjects[index]->SetIndexInWorld(index);
	}

	inGameObject->SetIndexInWorld(-1);

	mGameObjects.pop_back();
}


void World::Update()
{
	//update all game objects- sometimes they want to die, so we need to tread carefully...

	for (int i = 0, c = mGameObjects.size(); i < c; ++i)
	{
		GameObjectPtr go = mGameObjects[i];


		if (!go->DoesWantToDie())
		{
			go->Update();
		}
		//you might suddenly want to die after your update, so check again
		if (go->DoesWantToDie())
		{
			RemoveGameObject(go);
			go->HandleDying();
			--i;
			--c;
		}
	}
    CheckDashCollisions(); //Help with AI
}

void World::TransferShine(RoboCatPtr inFromPlayer, RoboCatPtr inToPlayer) //Help with AI
{
    if (!inToPlayer)
        return;

    // Stop the old holder from gaining points
    if (inFromPlayer)
    {
        inFromPlayer->SetHasShine(false);
        PointGainTracker::sInstance->StopPointGain(inFromPlayer->GetPlayerId());
    }

    // Give shine to new holder
    inToPlayer->SetHasShine(true);
    mShineHolderId = inToPlayer->GetPlayerId();

    // Start point gain for new holder
    PointGainTracker::sInstance->StartPointGain(inToPlayer->GetPlayerId());
}

void World::CheckDashCollisions() //Help with AI
{
    const auto& gameObjects = GetGameObjects();

    // Find all players who are currently dashing
    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        RoboCat* dasher = gameObjects[i]->GetAsCat();
        if (!dasher || !dasher->mIsDashing)
            continue;

        Vector3 dasherLocation = dasher->GetLocation();
        float dasherRadius = dasher->GetCollisionRadius();

        // Check if this dasher collides with the shine holder
        for (size_t j = 0; j < gameObjects.size(); ++j)
        {
            if (i == j)
                continue;

            RoboCat* target = gameObjects[j]->GetAsCat();
            if (!target || !target->HasShine())
                continue;

            // Check collision
            Vector3 targetLocation = target->GetLocation();
            float targetRadius = target->GetCollisionRadius();

            Vector3 delta = targetLocation - dasherLocation;
            float distSq = delta.LengthSq2D();
            float collisionDist = (dasherRadius + targetRadius);

            if (distSq < (collisionDist * collisionDist))
            {
                // Steal the shine!
                TransferShine(std::dynamic_pointer_cast<RoboCat>(gameObjects[j]),
                    std::dynamic_pointer_cast<RoboCat>(gameObjects[i]));
                return;  // One steal per frame
            }
        }
    }
}
