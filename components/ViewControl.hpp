#pragma once
#include <GLFW/glfw3.h>
struct ViewControl {
	struct KeyMappings {
		int lookLeft = GLFW_KEY_LEFT;
		int lookRight = GLFW_KEY_RIGHT;
		int lookUp = GLFW_KEY_UP;
		int lookDown = GLFW_KEY_DOWN;
	};

	KeyMappings keys{};
	float lookSpeed{ 1.5f };
};