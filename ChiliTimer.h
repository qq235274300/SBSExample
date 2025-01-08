#pragma once
#include <chrono>
class ChiliTimer
{
public:
	ChiliTimer();
	//返回上次Mark间隔
	float Mark();
	//不修改Last 只返回间隔时间
	float Peek()const;
private:
	std::chrono::steady_clock::time_point last;
};

