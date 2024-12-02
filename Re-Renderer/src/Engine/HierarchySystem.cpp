#include"HierarchySystem.h"
#include"Scene/Scene.h"


namespace Re_Renderer {


	void HierarchySystem::UpdateGlobalTransforms(Scene& scene) {
	
		//loop over all transforms if root { for each child call updateenitiy(with mustupdate =  root is dirty)}
		auto transforms = scene.Components.getRegistry<Components::Transform>();
		for (auto& t : transforms->getAllComponents()){

			if (t.m_ParentID == NullEntID){

				for (EntID entity : t.m_ChildrenIDs) {
				
					updateEntityTransform(entity,&t,scene,t.isDirty()); 
				}
				

			}

		}
	}

	void HierarchySystem::updateEntityTransform(EntID entityId, Components::Transform* parentTransform, Scene& scene, bool mustUpdate) {
	
		auto transforms = scene.Components.getRegistry<Components::Transform>();
		auto t = transforms->getComponent(entityId);

			// if mustupdate update
		if (mustUpdate) {
			t->updateGlobalPos(parentTransform->getPosition());
			t->updateGlobalRot(parentTransform->getRotation());
			t->updateGlobalScale(parentTransform->getScale());
			t->updateModel(); 
		}
			
		for (EntID entity : t->m_ChildrenIDs) { 

			updateEntityTransform(entity, t, scene, mustUpdate || t->isDirty());
		}
			// loop over childs and call update with (mustupdate = mustupdate || entity is dirty )
	}



	
}