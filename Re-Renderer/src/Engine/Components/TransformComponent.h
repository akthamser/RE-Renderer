#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>
#include<vector>
#include <glm/gtc/constants.hpp> 
#include<string>
#include"Components.h"

#include"../../config.h"




namespace Re_Renderer {

	namespace Components {


		
		struct Transform  {


			Transform() = delete;
			//Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,EntID parentId) : m_LocalPosition(position), m_LocalRotation(glm::radians(rotation)), m_LocalScale(scale) , m_ParentID(parentId) {};
			//Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : m_LocalPosition(position), m_LocalRotation(glm::radians(rotation)), m_LocalScale(scale) {};
			Transform(EntID parentId): m_ParentID(parentId) {};

			bool isDirty = true;

			glm::vec3 getPosition() const { return m_LocalPosition; };
			glm::vec3 getRotation() const { return m_LocalRotation; };
			glm::vec3 getScale() const { return m_LocalScale; };

			void setPosition(glm::vec3 pos) {
				m_LocalPosition = pos;
				updateModel();
			};
			void setRotation(glm::vec3 rot) {
				m_LocalRotation = rot;
				updateModel();
			};
			void  setScale(glm::vec3 scale) {
				m_LocalScale = scale;
				updateModel();
			};
			void setPosition(float x, float y, float z) {
				m_LocalPosition = glm::vec3(x, y, z);
				updateModel();
			};
			void setRotation(float x, float y, float z) {
				m_LocalRotation = glm::radians(glm::vec3(x, y, z));
				updateModel();
			};
			void  setScale(float x, float y, float z) {
				m_LocalScale = glm::vec3(x, y, z);
				updateModel();
			};



			glm::mat4 getModel() { return m_Model; }
			// Model Mat4
			glm::mat4 updateModel() {


				glm::mat4 rot = glm::toMat4(glm::quat(m_LocalRotation));

				m_Model = glm::translate(glm::mat4(1), m_LocalPosition)
					* rot
					* glm::scale(glm::mat4(1), m_LocalScale);

				isDirty = true;
				return m_Model;
			}




			

			EntID  m_ParentID = NullEntID;
			std::vector<EntID> m_ChildrenIDs;

		private:
			glm::vec3 m_LocalPosition = glm::vec3(0); 
			glm::vec3 m_LocalRotation = glm::vec3(0); 
			glm::vec3 m_LocalScale = glm::vec3(1);			

			glm::vec3 m_GlobalPosition = glm::vec3(0);
			glm::vec3 m_GlobalRotation = glm::vec3(0);
			glm::vec3 m_GlobalScale = glm::vec3(1);

			glm::mat4 m_Model = glm::mat4(1);




		};




	}
}