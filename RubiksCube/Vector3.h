#pragma once

const double PI =  3.1415926535897932384626433832795;

class CVector3
{
public:
	CVector3();
	CVector3(double x, double y, double z);
	~CVector3(void);
	double x;
	double y;
	double z;
	CVector3 operator+(const CVector3& v);
	CVector3 operator-(const CVector3& v);
	CVector3 operator-();
	CVector3 operator*(double s);
	CVector3 operator/(double s);
	double operator*(const CVector3& v);
	CVector3 operator^(const CVector3& v);
	CVector3& operator+=(const CVector3& v);
	CVector3& operator-=(const CVector3& v);
	CVector3& operator/=(double s);
	CVector3& operator*=(double s);
	CVector3 Unit(void);
	double Length(void);
	CVector3 OrthogonalTo(void);
	double GetAngle(CVector3& v);
	VOID ToFloatArray(float* position);

protected:
	double LengthSquared(void);
};
