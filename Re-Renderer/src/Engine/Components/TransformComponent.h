#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>
#include<vector>
#include <glm/gtc/constants.hpp> 
#include<string>
#include"Components.h"
#include"../../Utils/bitmask.h"
#include"../../config.h"




namespace Re_Renderer {

	namespace Components {


		
		struct Transform  {
			
			enum  Flags {
				NONE = 0,
				LOCAL_POS_DIRTY = 1 << 0,  // if local position does not equal positon of last frame
				LOCAL_ROT_DIRTY = 1 << 1,   // if local rotation does not equal positon of last frame
				LOCAL_SCALE_DIRTY = 1 << 2,   // if local scale does not equal positon of last frame

				GLOBAL_POS_DIRTY = 1 << 3,  // if GLOBAL position does not equal positon of last frame
				GLOBAL_ROT_DIRTY = 1 << 4,   // if GLOBAL rotation does not equal positon of last frame
				GLOBAL_SCALE_DIRTY = 1 << 5,   // if GLOBAL scale does not equal positon of last frame

			};

			Transform() = default;


			Bitmask flags;

			glm::vec3 getLocalPosition() const { return m_LocalPosition; };
			glm::vec3 getlocalRotation() const { return m_LocalRotation; };
			glm::vec3 getLocalScale() const { return m_LocalScale; };

			glm::vec3 getPosition() const { return m_GlobalPosition; };
			glm::vec3 getRotation() const { return m_GlobalRotation; };
			glm::vec3 getScale() const { return m_GlobalScale; };

			void setPosition(glm::vec3 pos) {
				m_GlobalPosition = m_GlobalPosition - m_LocalPosition + pos; 
				m_LocalPosition = pos;
				flags.set(Flags::LOCAL_POS_DIRTY | Flags::GLOBAL_POS_DIRTY); 
				updateModel();
			};
			void setRotation(glm::vec3 rot) {
				m_GlobalRotation = m_GlobalRotation - m_LocalRotation + glm::radians(rot);
				m_LocalRotation = glm::radians(rot);
				flags.set(Flags::LOCAL_ROT_DIRTY | Flags::GLOBAL_ROT_DIRTY); 
				updateModel();
			};
			void  setScale(glm::vec3 scale) {
				m_GlobalScale = m_GlobalScale / m_LocalScale * scale;
				m_LocalScale = scale;
				flags.set(Flags::LOCAL_SCALE_DIRTY | Flags::GLOBAL_SCALE_DIRTY);
				updateModel();
			};
			void setPosition(float x, float y, float z) {
				m_GlobalPosition = m_GlobalPosition - m_LocalPosition + glm::vec3(x, y, z);
				m_LocalPosition = glm::vec3(x, y, z);
				flags.set(Flags::LOCAL_POS_DIRTY | Flags::GLOBAL_POS_DIRTY);
				updateModel();
			};
			void setRotation(float x, float y, float z) {
				m_GlobalRotation = m_GlobalRotation - m_LocalRotation + glm::radians(glm::vec3(x, y, z));
				m_LocalRotation = glm::radians(glm::vec3(x, y, z));

				flags.set(Flags::LOCAL_ROT_DIRTY | Flags::GLOBAL_ROT_DIRTY);

				updateModel();
			};
			void  setScale(float x, float y, float z) {
				m_GlobalScale = m_GlobalScale / m_LocalScale * glm::vec3(x, y, z); 
				m_LocalScale = glm::vec3(x, y, z);
				flags.set(Flags::LOCAL_SCALE_DIRTY | Flags::GLOBAL_SCALE_DIRTY);
				updateModel();
			};



			glm::mat4 getModel() { return m_Model; }
			// Model Mat4
			glm::mat4 updateModel() {


				glm::mat4 rot = glm::toMat4(glm::quat(m_GlobalRotation));

				m_Model = glm::translate(glm::mat4(1), m_GlobalPosition)
					* rot
					* glm::scale(glm::mat4(1), m_GlobalScale);

	
				
				return m_Model;
			}

			void updateGlobalPos(glm::vec3 parentPos) {
				m_GlobalPosition = parentPos + m_LocalPosition;
				flags.set( Flags::GLOBAL_POS_DIRTY); 
			}
			void updateGlobalRot(glm::vec3 parentRot) {
				m_GlobalRotation = parentRot + m_LocalRotation;
				flags.set( Flags::GLOBAL_ROT_DIRTY);
			}
			void updateGlobalScale(glm::vec3 parentScale) {
				m_GlobalScale = parentScale * m_LocalScale;
				flags.set(Flags::GLOBAL_SCALE_DIRTY);
			}

			bool isDirty()
			{
				return flags.isSet(Components::Transform::Flags::GLOBAL_POS_DIRTY)
					|| flags.isSet(Components::Transform::Flags::GLOBAL_ROT_DIRTY)
					|| flags.isSet(Components::Transform::Flags::GLOBAL_SCALE_DIRTY)
					|| flags.isSet(Components::Transform::Flags::LOCAL_POS_DIRTY)
					|| flags.isSet(Components::Transform::Flags::LOCAL_ROT_DIRTY)
					|| flags.isSet(Components::Transform::Flags::LOCAL_SCALE_DIRTY);
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