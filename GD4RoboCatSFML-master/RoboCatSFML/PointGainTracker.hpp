#pragma once

//Ben Arrowsmith
//Help with AI

#include <vector>
#include <memory>

struct ActivePointGain
{
    int playerId;
    float timer;
    int pointsAwarded;
};

class PointGainTracker
{
public:
    static PointGainTracker* sInstance;

    PointGainTracker();

    void StartPointGain(int playerId);
    void Update(float deltaTime);
    void StopPointGain(int playerId);

private:
    std::vector<ActivePointGain> mActiveGains;
};
