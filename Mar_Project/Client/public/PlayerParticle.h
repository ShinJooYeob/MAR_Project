#pragma once


BEGIN(Client)

// ��ƼŬ �Ӽ�
typedef struct tag_ParticleAttribute
{
	tag_ParticleAttribute()
	{
		_lifeTime = 0.0f;
		_age = 0.0f;
		_isAlive = true;
	}

	// ������ ��ġ / ���� / �� 
	_float3 _position;
	_float3 _velocity;
	_float	_force;

	// �ð� ũ�� ��
	_float       _lifeTime;     // how long the particle lives for before dying  
	_float       _age;          // current age of the particle  
	_float3		_color = _float3(255.f, 255.f, 255.f);
	//	D3DXCOLOR   _color;        // current color of the particle   
	//	D3DXCOLOR   _colorFade;    // how the color fades with respect to time
	bool        _isAlive;

	_float3		_NowparantPos;
	class CParticleObject* SubParticle = nullptr;
}PARTICLEATT;





class CPlayerParticle
{
public:
	enum ePlayerParticle
	{
		Type_Fly, Type_Feather, Type_Tornado, Type_Ash, Type_End
	};






public:
	CPlayerParticle();
	~CPlayerParticle();
};

END