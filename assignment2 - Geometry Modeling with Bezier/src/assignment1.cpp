#include "../bezier/mlbezier.h"
#include "../inc/shader_m.h"
#include "../3rdparty/stb_image.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
mlBezier mlbezier;


const unsigned int SCR_WINDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view, projection;

float pitch = 0;
float yaw = -90.0f;
// for mouse input
float lastX = 400, lastY = 300;
bool firstMouse = true;
// for scene scaling
float fov = 60.0f;

unsigned int texture;


GLfloat sun_light_position[] = { 0.0f, 0.0f, 12.0f, 0.0f };
GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat sun_light_diffuse[] = { 1.0f,  0.0f, 1.0f, 1.0f };
GLfloat sun_light_specular[] = { 1.0f, 1.0f, 0.0f, 1.0f };


GLfloat earth_mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat earth_mat_diffuse[] = { 0.0f, 0.0f, 0.5f, 1.0f };
GLfloat earth_mat_specular[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat earth_mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat earth_mat_shininess = 30.0f;


bool dragFlag = false;
int selected_index = -1;
glm::vec2 drag_start; glm::vec2 drag_end;


void drawControlPoint(mlBezier &mlbezier)
{
	glBegin(GL_QUADS);
	for (int i = 0; i < (int)mlbezier.indicesofControlpoints.size() / 4; i++)
	{
		glColor3f(0, 1, 0);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].z
		);
	}
	glEnd();
}
void drawBezierSurface(mlBezier &mlbezier)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::vec3 points_pos[3], points_norm[3];glm::vec2 points_tex[3];
	for (size_t i = 0; i < mlbezier.indicesofP.size(); i += 3) {
		points_pos[0] = mlbezier.P[mlbezier.indicesofP[i]];
		points_pos[1] = mlbezier.P[mlbezier.indicesofP[i+1]];
		points_pos[2] = mlbezier.P[mlbezier.indicesofP[i+2]];

		points_norm[0] = mlbezier.N[mlbezier.indicesofP[i]];
		points_norm[1] = mlbezier.N[mlbezier.indicesofP[i + 1]];
		points_norm[2] = mlbezier.N[mlbezier.indicesofP[i + 2]];
		
		points_tex[0] = mlbezier.st[mlbezier.indicesofP[i]];
		points_tex[1] = mlbezier.st[mlbezier.indicesofP[i + 1]];
		points_tex[2] = mlbezier.st[mlbezier.indicesofP[i + 2]];
	
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
		glNormal3f(points_norm[0].x, points_norm[0].y, points_norm[0].z);
		glTexCoord2f(points_tex[0].x, points_tex[0].y);
		glVertex3f(points_pos[0].x, points_pos[0].y, points_pos[0].z);


		glNormal3f(points_norm[1].x, points_norm[1].y, points_norm[1].z);
		glTexCoord2f(points_tex[1].x, points_tex[1].y);
		glVertex3f(points_pos[1].x, points_pos[1].y, points_pos[1].z);

		glNormal3f(points_norm[2].x, points_norm[2].y, points_norm[2].z);
		glTexCoord2f(points_tex[2].x, points_tex[2].y);
		glVertex3f(points_pos[2].x, points_pos[2].y, points_pos[2].z);
		glEnd();
	}
}
void initPMV()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, SCR_WINDTH / SCR_HEIGHT, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
	(
		0, 0, 7.5,
		0, 0, 0,
		0, 1, 0
	);
}

void changePMV() {
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	
	gluPerspective(fov, SCR_WINDTH / SCR_HEIGHT, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	gluLookAt
	(
		cameraPos.x,cameraPos.y,cameraPos.z,
		cameraPos.x+cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
		cameraUp.x,cameraUp.y,cameraUp.z
	);

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), (float)(SCR_WINDTH / SCR_HEIGHT), 0.1f, 100.0f);

}

void initTexture(unsigned int &texture)
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load("resource/textures/container.jpg", &width, &height, &nrChannels, 0);
	//bind it to texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the active texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// sample: specify texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);
}


void AddLight(mlBezier &mlbezier)
{
	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
int main()
{
	glfwInit();

	GLFWwindow * window = glfwCreateWindow(SCR_WINDTH, SCR_HEIGHT, "hello", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	glewInit();



	mlbezier.divs = 25;
	mlbezier.mlCreateBeizermesh();
	mlbezier.mlTriangularization();




	initTexture(texture);
	
	initPMV();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		changePMV();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		

		AddLight(mlbezier);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawBezierSurface(mlbezier);
		
		glDisable(GL_LIGHTING);
		glPointSize(5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawControlPoint(mlbezier);

		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	float cameraSpeed = 2.5 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // 这个bool变量初始时是设定为true的
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);


}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 60.0f)
		fov = 60.0f;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glm::vec4 viewport = glm::vec4(0, 0, SCR_WINDTH, SCR_HEIGHT);
	glfwGetCursorPos(window, &xpos, &ypos);
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (dragFlag == false) {
			int ind = mlbezier.getSelectedControlPointIndice(xpos, ypos, view, projection, viewport);
			if (ind != -1) {
				dragFlag = true;
				drag_start = glm::vec2(xpos,viewport[3]- ypos);
				selected_index = ind;
			}
		}
		break;
	default:
		return;
	}

	if (action == GLFW_RELEASE) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (dragFlag ==true&&selected_index!=-1) {
			drag_end = glm::vec2(xpos, viewport[3] - ypos);
			mlbezier.updateControlPointPosition(selected_index, (drag_end - drag_start),view,projection,viewport);
			mlbezier.mlCreateBeizermesh();
			mlbezier.mlTriangularization();
			selected_index = -1;
			dragFlag = false;
		}
		break;
	default:
		return;
	}
	return;
}
