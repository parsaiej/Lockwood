#include "..\LCamera.h"
#include "..\LUtility.h"
#include <glm\gtc\matrix_transform.hpp>

LCamera::LCamera() {

}

LCamera::~LCamera() {

}

const glm::mat4& LCamera::GetViewProj() {
	glm::mat4 view = glm::lookAt(m_Position, m_Target, LUtility::WorldUp);
	glm::mat4 proj = glm::perspective(glm::radians(m_FOV), LUtility::ScreenWidth / (float)LUtility::ScreenHeight, m_ZNear, m_ZFar);
	return proj * view;
}