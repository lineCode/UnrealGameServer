#pragma once

class IJob
{
public:
	virtual void Execute() = 0;
	
};

class TJob : public IJob
{

};

