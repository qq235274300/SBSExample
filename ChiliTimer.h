#pragma once
#include <chrono>
class ChiliTimer
{
public:
	ChiliTimer();
	//�����ϴ�Mark���
	float Mark();
	//���޸�Last ֻ���ؼ��ʱ��
	float Peek()const;
private:
	std::chrono::steady_clock::time_point last;
};

