#pragma once
#include"CameraSystem.h"

namespace Re_Renderer {

		CameraSystem::CameraSystem(Window& window) : m_window(window){
			

		};


		void CameraSystem::UpdateCameras(Scene& scene) {


		


			std::shared_ptr<ComponentRegistry<Components::Camera>> CamerasRegestry = scene.Components.getRegistry<Components::Camera>(); 
			std::shared_ptr<ComponentRegistry<Components::Transform>> TransfromRegestry = scene.Components.getRegistry<Components::Transform>();

			std::vector<EntID> entites = CamerasRegestry->getEntitiesIds();

			for (EntID entityId : entites) {
				auto camera = CamerasRegestry->getComponent(entityId);
				auto transfrom = TransfromRegestry->getComponent(entityId);

				if (transfrom->isDirty) {
	
					camera->updateViewMatrix(transfrom->getPosition(),transfrom->getRotation());
					
				}

				float ratio = (float)m_window.Width / m_window.Height;

				if (ratio != camera->getAspectRatio()) {

					camera->setAspectRatio(ratio);
				}

				if (camera->isDirty ) {
					
					camera->updateProjectionMatrix();
				}

			}
			
		}






	


}