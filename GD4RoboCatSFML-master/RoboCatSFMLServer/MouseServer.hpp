class MouseServer : public Mouse
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new MouseServer()); }
	void HandleDying() override;
	virtual bool HandleCollisionWithCat(RoboCat* inCat) override;
protected:
	MouseServer();

private:
	float mPointGainTimer = 0.0f;
	int mPlayerId = -1;
	bool mIsGainingPoints = false;
	int mLastPointsAwarded = 0;

};

