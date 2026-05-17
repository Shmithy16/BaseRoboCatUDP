#include "RoboCatPCH.hpp"
#include "PointGainTracker.hpp"


PointGainTracker* PointGainTracker::sInstance = nullptr;

PointGainTracker::PointGainTracker()
{
    sInstance = this;
}

void PointGainTracker::StartPointGain(int playerId)
{
    mActiveGains.push_back({ playerId, 0.0f, 0 });
}

void PointGainTracker::Update(float deltaTime)
{
    for (auto it = mActiveGains.begin(); it != mActiveGains.end(); )
    {
        it->timer += deltaTime;

        int pointsToAward = static_cast<int>(it->timer);

        if (pointsToAward > it->pointsAwarded)
        {
            int newPoints = pointsToAward - it->pointsAwarded;
            ScoreBoardManager::sInstance->IncScore(it->playerId, newPoints);
            it->pointsAwarded = pointsToAward;
        }
        if (it->timer >= 30.0f)
        {
            it = mActiveGains.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void PointGainTracker::StopPointGain(int playerId)
{
    auto it = std::find_if(mActiveGains.begin(), mActiveGains.end(),
        [playerId](const ActivePointGain& gain) { return gain.playerId == playerId; });

    if (it != mActiveGains.end())
    {
        mActiveGains.erase(it);
    }
}
