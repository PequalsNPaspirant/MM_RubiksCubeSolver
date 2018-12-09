#include "StdAfx.h"
#include "Camera.h"
#include <math.h>

CCamera::CCamera(void)
{
	m_fTheta = 0;
	m_fPhi = 0;
	m_fDistance = 0;
	m_vUp = CVector3(0, 1, 0);

	m_bFlipped = false;
}

CCamera::~CCamera(void)
{
}

void CCamera::SetDistance(float fDistance)
{
	m_fDistance = fDistance;
}

float CCamera::GetDistance(void)
{
	return m_fDistance;
}

void CCamera::Move(float fDistance)  //zoom in-out
{
	m_fDistance += fDistance;

	if (m_fDistance < 8.0f)
		m_fDistance = 8.0f;

	else if (m_fDistance > 100.0f)
		m_fDistance = 100.0f;
}

CVector3 CCamera::GetLookAt(void)
{
	return m_vLookAt;
}

void CCamera::SetLookAt(CVector3 vLookAt)
{
	m_vLookAt = vLookAt;
}

CVector3 CCamera::GetUp(void)
{
	return m_vUp;
}

void CCamera::SetUp(CVector3 vUp)
{
	m_vUp = vUp;
}

void CCamera::Tilt(int nDegrees)
{
	m_fTheta += nDegrees;

	if (m_fTheta >= 360)
		m_fTheta -= 360;

	else if (m_fTheta < 0)
		m_fTheta += 360;

	if (!m_bFlipped && m_fTheta >= 90 && m_fTheta <= 270)
	{
		m_bFlipped = true;
		m_vUp = -m_vUp;
	}

	else if (m_bFlipped && (m_fTheta <= 90 || m_fTheta >= 270))
	{
		m_bFlipped = false;
		m_vUp = -m_vUp;
	}
}

void CCamera::Rotate(int nDegrees)
{
	if (m_bFlipped)
		nDegrees *= -1;

	m_fPhi += nDegrees;

	if (m_fPhi >= 360)
		m_fPhi -= 360;

	else if (m_fPhi < 0)
		m_fPhi += 360;
}

float CCamera::GetPhi()
{
	return m_fPhi;
}

CVector3 CCamera::GetPosition()
{
	double x, y, z;
	x = m_fDistance * cos(m_fPhi * PI / 180) * cos(m_fTheta * PI / 180);
	y = m_fDistance * sin(m_fTheta * PI / 180);
	z = m_fDistance * cos(m_fTheta * PI / 180) * sin(m_fPhi * PI / 180);

	CVector3 pos(x, y, z);

	return pos - m_vLookAt;
}

BOOL CCamera::IsFlipped()
{
	return m_bFlipped;
}

float CCamera::GetTheta()
{
	return m_fTheta;
}

VOID CCamera::SetPhi(float fPhi)
{
	m_fPhi = 0;
	Rotate((int)(fPhi * 180 / PI));
}

VOID CCamera::SetTheta(float fTheta)
{
	m_fTheta = 0;
	Tilt((int)(fTheta * 180 / PI));
}