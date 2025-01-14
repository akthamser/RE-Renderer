#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>

#include <glm/gtc/constants.hpp> 



namespace Re_Renderer {


	namespace Components {



		struct MovmentController {


			MovmentController(float speed, float sensitivity) :speed(speed), sensitivity(sensitivity)

			{
			};


			float speed;
			float sensitivity;
		private:



		};



	};
};

