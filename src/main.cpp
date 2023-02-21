#include "../include/libs.h"
#include "../include/mouse.h"
#include "../include/map.h"
#include "../include/shader.h"

Map map;

GLfloat vertices[] = {
	// Vertices         // Textures
	 0.0f, 0.22f,		 0.0f, 0.22f,  // 0 ===HEXAGON===
	0.44f,  0.0f,		0.44f,  0.0f,  // 1
	0.88f, 0.22f,		0.88f, 0.22f,  // 2
	0.88f, 0.66f,		0.88f, 0.66f,  // 3
	0.44f, 0.88f,		0.44f, 0.88f,  // 4
	 0.0f, 0.66f,		 0.0f, 0.66f,  // 5

	-1.0f, -1.0f,		0.0f, 0.0f,    // 6 ===QUAD===
	-1.0f,  1.0f,		0.0f, 1.0f,    // 7
	 1.0f, -1.0f,		1.0f, 0.0f,    // 8
	 1.0f,  1.0f,		1.0f, 1.0f     // 9
};

GLuint indicies[] = {
	0, 1, 2,  // ===HEXAGON===
	2, 3, 4,
	4, 5, 0,
	0, 2, 4,

	7, 6, 8,  // ===QUAD===
	8, 9, 7
};


void processInput(GLFWwindow* window);


int main() {
	srand((unsigned)time(NULL));
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COPMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(MIN_WIDTH, MIN_HEIGHT, "Trap the cat", NULL, NULL);
	if (window == NULL) {
		std::cout << "Could not create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// set window icon
	GLFWimage images[1];
	images[0].pixels = stbi_load("assets/icon.png", &images[0].width, &images[0].height , 0, 4);
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);


	Shader game("assets/vertex_core.glsl", "assets/fragment_core.glsl");
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Textures	
	Texture t_background("assets/background.png");
	Texture t_cat("assets/cat.png");

	t_background.bind(0);
	t_cat.bind(1);

	glm::mat4 transform = glm::mat4(1.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	float bgTransparency = 0.9f;
	int bgAnimationFrame = 0;

	map.reset();

	while (!glfwWindowShouldClose(window)) {
		glUniform1i(glGetUniformLocation(game.id, "bg_Texture"), 0);
		glUniform1i(glGetUniformLocation(game.id, "cat_Texture"), 1);

		// process input
		if(map.gameOver == false)
			processInput(window);
		if (map.shouldMove == true) {
			if(map.getNumOfMoves() == 0)
				map.randomMove();
			else
				map.move();
			map.shouldMove = false;
		}

		glBindVertexArray(VAO);
		game.activate();
		
		// render background
		glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1f(glGetUniformLocation(game.id, "bg_transparency"), bgTransparency);
		game.setObjectType(BACKGROUND);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(4 * 3 * sizeof(GLuint)));
		
		// render map
		for (unsigned int i = 0; i < NUM_OF_FIELDS; i++)
		{	
			game.setPosition(i);
			if (map.getMapState(i) == WALL) {
				game.setObjectType(WALL);
			}
			else {
				game.setObjectType(EMPTY);
			}
			// draw hexagon
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		}

		// render cat
		if (map.moveAnimation == true) {
			transform = glm::translate(transform, glm::vec3(map.xAxisMove / 65, map.yAxisMove / 87, 0.0f));
			map.moveAnimationFrame++;
			if (map.moveAnimationFrame >= 10) {
				map.moveAnimationFrame = 0;
				map.moveAnimation = false;
			}
		}

		glUniformMatrix4fv(glGetUniformLocation(game.id, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
		game.setObjectType(CAT);
		game.setPosition(map.getCatPosition());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(4 * 3 * sizeof(GLuint)));
		
		// send new frame to window
		glfwPollEvents();
		glfwSwapBuffers(window);
		
		if (map.moveAnimation == false && map.gameOver == false) {
			map.gameOverCheck();
		}

		if (map.gameOver == true) {
			if(bgAnimationFrame < 20)
				bgTransparency = 0.9f - (0.045f * bgAnimationFrame);
			else
				bgTransparency = 0.045f * (bgAnimationFrame - 20.0f);
			Sleep(25);
			printf("%f ", bgTransparency);
			bgAnimationFrame++;
			if (bgAnimationFrame == 20) {
				map.reset();
				transform = glm::mat4(1.0f);
			}
			if (bgAnimationFrame == 40) {
				bgAnimationFrame = 0;
				map.gameOver = false;
			}
		}
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_1)) {
		double x_mouse = Mouse::getMouseX();
		double y_mouse = Mouse::getMouseY();
		printf("x: %.1f y:%.1f\n", x_mouse, y_mouse);
		// reset button
		if (x_mouse >= 85 && x_mouse <= 255
			&& y_mouse >= 726 && y_mouse <= 775) {
			map.gameOver = true;
		}
		double x_hex, y_hex;
		double distance, radius = 27;
		for (int hex = 0; hex < 121; ++hex) {
			x_hex = map.getFieldX(hex);
			y_hex = map.getFieldY(hex);
			distance = sqrt(pow((x_hex - x_mouse), 2) + pow((y_hex - y_mouse), 2));
			if (distance <= radius) {
				map.setWallPosition(hex);
				printf("Hex %d\n", hex);
				break;
			}
		}
	}
}
