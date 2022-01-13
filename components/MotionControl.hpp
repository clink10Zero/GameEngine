#pragma once

struct MotionControl {
	struct KeyMappings {
		int moveLeft = GLFW_KEY_A;
		int moveRight = GLFW_KEY_D;
		int moveForward = GLFW_KEY_W;
		int moveBackward = GLFW_KEY_S;
		int moveUp = GLFW_KEY_SPACE;
		int moveDown = -1;
		int turnLeft = GLFW_KEY_Q;
		int turnRight = GLFW_KEY_E;
	};

	KeyMappings keys{};
	float moveSpeed{ 3.f };
	float lookSpeed{ 1.5f };

	glm::vec3 movement{};
};
