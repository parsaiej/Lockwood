#pragma once

#include <glm\glm.hpp>

class LCamera {
public:
	LCamera();
	~LCamera();

	inline void SetPosition(glm::vec3 _position) { m_Position = _position; };
	inline void SetTarget(glm::vec3 _target)     { m_Target = _target;     };
	inline void SetNear(float _znear)            { m_ZNear = _znear;       };
	inline void SetFar(float _zfar)              { m_ZFar = _zfar;         };
	inline void SetFOV(float _fov)               { m_FOV = _fov;           };


	inline const glm::mat4& GetViewProj();

	inline const glm::vec3& GetPosition() { return m_Position; };
	inline const glm::vec3& GetTarget()   { return m_Target;   };
	inline const float&		GetZNear()    { return m_ZNear;    };
	inline const float&		GetZFar()     { return m_ZFar;     };
	inline const float&		GetFOV()      { return m_FOV;	   };

private:
	glm::vec3 m_Position;
	glm::vec3 m_Target;
	float m_ZNear;
	float m_ZFar;
	float m_FOV;
};