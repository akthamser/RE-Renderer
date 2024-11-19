#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>

#include <glm/gtc/constants.hpp> 

#include"Components.h"

namespace Re_Renderer {


	namespace Components {



		struct Camera  {


			Camera(float fov = 45, float aspectRatio = 4 / 3, float nearPlane = 0.1f, float farPlane = 100)
				:m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClippingPlane(nearPlane), m_FarClippingPlane(farPlane)
			{
			};

			bool isDirty = true;

			float getFOV() const { return m_FOV; }
			float getAspectRatio() const { return m_AspectRatio; }
			float getNearClippingPlane() const { return m_NearClippingPlane; }
			float getFarClippingPlane() const { return m_FarClippingPlane; }

			void setFOV(float fov) { m_FOV = fov; isDirty = true; }
			void setAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio;isDirty = true; }
			void setNearClippingPlane(float nearPlane) { m_NearClippingPlane = nearPlane;isDirty = true; }
			void setFarClippingPlane(float farPlane) { m_FarClippingPlane = farPlane; isDirty = true; }


			const glm::mat4& getViewMatrix() const { return m_View; }
			const glm::mat4& getProjectionMatrix() const { return m_Projection; }

			void setViewMatrix(const glm::mat4& newView) { m_View = newView; }
			void setProjectionMatrix(const glm::mat4& newProjection) { m_Projection = newProjection; }

			void updateViewMatrix(const glm::vec3& position, const glm::vec3& rotation) {

				m_View = glm::lookAt(position, position + getForward(rotation), getUp(rotation));
			}

			void updateProjectionMatrix() {
				m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
			}



		private:
			float m_FOV;
			float m_AspectRatio;
			float m_NearClippingPlane;
			float m_FarClippingPlane;
			glm::mat4 m_View;
			glm::mat4 m_Projection;

			glm::vec3 getForward(glm::vec3 rotation) {
				glm::quat orientation = glm::quat(rotation);
				glm::mat4 rotationMatrix = glm::toMat4(orientation);
				return glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));
			};
			glm::vec3 getRight(glm::vec3 direction) {
				glm::vec3 up = glm::vec3(0, 1, 0);
				return glm::cross(up, direction);
			};
			glm::vec3 getUp(glm::vec3 rotation) {
				glm::quat orientation = glm::quat(rotation);
				glm::mat4 rotationMatrix = glm::toMat4(orientation);
				return glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0));

			}
			glm::vec2 rotateVec2(const glm::vec2& vec, float angleRadians) {
				float cosTheta = cos(angleRadians);
				float sinTheta = sin(angleRadians);

				return glm::vec2(
					vec.x * cosTheta - vec.y * sinTheta,
					vec.x * sinTheta + vec.y * cosTheta
				);
			}

		};


	}



}

