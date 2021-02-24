class Vector3
{
protected:
	float x, y, z;

public:
	Vector3(float _x, float _y, float _z)
		: x(_x), y(_y), z(_z)
	{

	}

	void inverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}
};
