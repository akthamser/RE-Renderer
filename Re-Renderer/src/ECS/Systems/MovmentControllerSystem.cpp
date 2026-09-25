#include"MovmentControllerSystem.h"
#include"../Scene/Scene.h"



namespace Re_Renderer {





void MovmentControllerSystem::UpdateMovment(Scene& scene,float deltatime) {


	bool isFlying = glfwGetMouseButton(m_window.getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	if (isFlying) {
		// HIDE the cursor so we can look around infinitely
		glfwSetInputMode(m_window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		// SHOW the cursor so we can click the UI
		glfwSetInputMode(m_window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		double xPos, yPos;
		glfwGetCursorPos(m_window.getGLFWwindow(), &xPos, &yPos);
		lastXpos = xPos;
		lastYpos = yPos;

		return;
	}


	auto movmentControllersRegistry = scene.Components.getRegistry<Components::MovmentController>(); 
	auto transformsRegistry = scene.Components.getRegistry<Components::Transform>(); 
	auto entites = movmentControllersRegistry->getEntitiesIds();

	for (EntID id : entites)
	{
		Components::MovmentController* movmentcontroller = movmentControllersRegistry->getComponent(id);
		Components::Transform* transform = transformsRegistry->getComponent(id);

		glm::vec2 Input = glm::vec2(0);

		if (glfwGetKey(m_window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			Input.y++;
		}
		if (glfwGetKey(m_window.getGLFWwindow(), GLFW_KEY_S )|| glfwGetKey(m_window.getGLFWwindow(), GLFW_KEY_C) == GLFW_PRESS)
		{
			Input.y--;
		}
		if (glfwGetKey(m_window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
		{
			Input.x++;
		}
		if (glfwGetKey(m_window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
		{
			Input.x--;
		}


		if(Input != glm::vec2(0))
		move(transform, movmentcontroller, Input,deltatime);


		double xPos, yPos;
		glfwGetCursorPos(m_window.getGLFWwindow(), &xPos, &yPos);
		glm::vec2 deltamouse = glm::vec2(lastXpos - xPos, lastYpos - yPos);
		lastXpos = xPos;
		lastYpos = yPos;

		if (deltamouse != glm::vec2(0))
			look(transform, movmentcontroller, deltamouse, deltatime);
	}

}


void MovmentControllerSystem::setup() {

	//glfwSetInputMode(m_window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	lastXpos = m_window.Width / 2;
	lastYpos = m_window.Height / 2;
}




void MovmentControllerSystem::move(Components::Transform* transform, Components::MovmentController* mc, glm::vec2& input,float deltatime){

	glm::vec3 pos = transform->getPosition();
	glm::vec3 forward = transform->getForward();
	glm::vec3 right = transform->getRight();

	glm::vec3 direction = glm::normalize(input.x * transform->getRight() + input.y * transform->getForward());
	glm::vec3 velocity = direction * mc->speed * deltatime;


	transform->setPosition(pos + velocity);

}

void MovmentControllerSystem::look(Components::Transform* transform, Components::MovmentController* mc, glm::vec2& deltamouse, float deltatime){

	glm::vec3 rot = glm::degrees(transform->getRotation());

	float pitch = rot.x + deltamouse.y * mc->sensitivity * deltatime;
	float yaw = rot.y + deltamouse.x * mc->sensitivity * deltatime;

	
	if (pitch > 90.0f)
		pitch = 90.0f;
	if (pitch < -90.0f)
		pitch = -90.0f;
	

	transform->setRotation(pitch, yaw,rot.z);


}




}







