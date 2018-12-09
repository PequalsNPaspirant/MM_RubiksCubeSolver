#include "Vector3.h"

#pragma once

class CCamera
{
public:
	CCamera(void);
	~CCamera(void);	
	void SetDistance(float fDistance);
	float GetDistance(void);
	void Move(float fDistance);	
	CVector3 GetLookAt(void);
	void SetLookAt(CVector3 vLookAt);
	CVector3 GetUp(void);
	void SetUp(CVector3 vUp);
	void Tilt(int nDegrees);
	void Rotate(int nDegrees);
	CVector3 GetPosition();
	BOOL IsFlipped();
	float GetPhi();
	float GetTheta();
	VOID SetPhi(float fPhi);
	VOID SetTheta(float fPhi);

private:
	float m_fDistance;
	float m_fPhi;
	float m_fTheta;
	bool m_bFlipped;
	CVector3 m_vLookAt;
	CVector3 m_vUp;
};
