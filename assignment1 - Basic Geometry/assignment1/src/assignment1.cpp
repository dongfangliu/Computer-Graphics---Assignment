#include"../inc/common.h"
#include "../inc/shader_m.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Setting up camera 
// for translating the camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 model ,view, projection;
glm::mat4 lightmodel= glm::rotate(glm::mat4(), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

float deltaTime = 0;
float lastFrame = 0;
// for rotating the camera
float pitch = 0;
float yaw = -90.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
// for scene scaling 
float fov = 45.0f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


void drawQuad(glm::vec3 color, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,glm::vec3 p4)
{
	glBegin(GL_QUADS);
	glColor3f(color.x, color.y, color.z);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p4.x, p4.y, p4.z);
	glEnd();
}

void drawTri() {
	glBegin(GL_TRIANGLES);
	glColor3f(0.6, 0.2, 0.3);
	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 1, 0);
	
	glEnd();
}
void drawTri(unsigned int tri_VAO,unsigned int tri_VBO,unsigned int tri_EBO,GLfloat * vertices)
{
	unsigned int indices[] = {0,1,2};
	glBindVertexArray(tri_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tri_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,tri_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void drawTetrahedron(unsigned int tetra_VAO, unsigned int tetra_VBO, unsigned int tetra_EBO, GLfloat * vertices)
{
	unsigned int indices[] = { 0,1,2 ,1,3,2,0,3,1,0,2,3};
	glBindVertexArray(tetra_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tetra_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetra_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void drawCube(unsigned int cube_VAO, unsigned int cube_VBO, unsigned int cube_EBO, GLfloat cube_vertices[24])
{
	unsigned int cube_indices[] = { 0,1,5,5,4,0,
		1,2,6,6,5,2,
		2,3,7,7,6,2,
		3,0,4,4,7,3,
		2,1,0,0,3,2,
		4,5,6,6,7,4,
	};
	glBindVertexArray(cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}
glm::vec3 F(double r, double u, double v, glm::vec3 center) {
	return glm::vec3(cos(u)*sin(v)*r, cos(v)*r, sin(u)*sin(v)*r) + center;
}

void getSphereIndices(unsigned int u_division, unsigned int v_division, unsigned int * indices) {
	
	int count = 0;
	for (int i = 0; i < u_division*(v_division+1)-1; i++) {
		if ((i + 1) % (v_division + 1) == 0) {
			continue;
		}
		int right = i + v_division+1;
		int down_l = i + 1;
		int down_r = i + v_division + 2;
		indices[count++] = i;
		indices[count++] = down_l;
		indices[count++] = down_r;
		indices[count++] = i;
		indices[count++] = down_r;
		indices[count++] = right;
	}
}
void getSphereVertices(glm::vec3 center,double r,unsigned int u_division, unsigned int v_division,GLfloat * vertices) {
	double startU = 0;
	double startV = 0;
	double endU = 2 * glm::pi<double>();
	double endV = glm::pi<double>();
	
	double stepU = (endU - startU) / u_division;
	double stepV = (endV - startV) / v_division;

	
	int count = 0;
	for (int i = 0; i<=u_division; i++) { // U-points
		for (int j = 0; j<=v_division; j++) { // V-points
			double u = i*stepU + startU;
			double v = j*stepV + startV;
			glm::vec3 pos = F(r, u, v, center);
			vertices[count++] = pos.x; 
			vertices[count++] = pos.y; 
			vertices[count++] = pos.z;
			vertices[count++] =  center.x- pos.x;
			vertices[count++] =  center.y- pos.y;
			vertices[count++] =  center.z- pos.z;
		}
	}
}
//void drawSphere(unsigned int sphere_VAO, unsigned int sphere_VBO, unsigned int sphere_EBO, glm::vec3 center, double r, unsigned int u_division, unsigned int v_division) {
//	GLfloat * vertices = getSphereVertices(center, r, u_division, v_division);
//	unsigned int * indices = getSphereIndices(u_division, v_division);
//	glBindVertexArray(sphere_VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	glDrawElements(GL_TRIANGLES, (u_division+1)*(v_division+1)*6, GL_UNSIGNED_INT, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}

void ChangePmv(int shaderProgram)
{
	// Update view projection
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
	//test PMV
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void initPMV()
{
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(fov), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
}



int main(int argc, char* argv[])
{
	unsigned int tri_VAO, tri_VBO, tri_EBO, cube_VAO, cube_VBO, cube_EBO, terta_VAO, terta_VBO, terta_EBO, sphere_VAO, sphere_VBO, sphere_EBO;

	GLfloat tri_vertices[] = {
		-1,-1,-1,
		1,1,-1,
		-1,1,-1
	};
	GLfloat cube_vertices[] = {
		0,0,0.5,	-1,-1 ,1 , //0
		0.5,0,0.5,	1 ,-1 ,1 ,//1
		0.5,0,0,	1 ,-1 ,-1 ,//2
		0,0,0,	-1 ,-1 ,-1 ,//3

		0,0.5,0.5,	-1 ,1 ,1 ,//4
		0.5,0.5,0.5,	1 ,1 ,1 ,//5
		0.5,0.5,0,	1 ,1 ,-1 ,//6
		0,0.5,0,	-1 ,1 ,-1,//7
	};
	GLfloat terta_vertices[] = {
		2,0,0,
		3,0,0,
		2.5,1,-0.5,
		2.5,0,-1,
	};
	glm::vec3 sphere_center = glm::vec3(4, 4, 4);
	double sphere_r = 1; const unsigned int u_division = 16; const unsigned int v_division = 10;



	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	
	initPMV();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	Shader baseShader("BaseShader.vs", "BaseShader.fs");
	
	


	//Setting Up Buffers and buffer data
	glGenVertexArrays(1, &cube_VAO); glGenBuffers(1, &cube_VBO); glGenBuffers(1, &cube_EBO);
	glGenVertexArrays(1, &terta_VAO); glGenBuffers(1, &terta_VBO); glGenBuffers(1, &terta_EBO);
	glGenVertexArrays(1, &sphere_VAO); glGenBuffers(1, &sphere_VBO); glGenBuffers(1, &sphere_EBO);
	//SET UP CUBE 
	unsigned int cube_indices[] = { 0,1,5,5,4,0,
		1,2,6,6,5,1,
		2,3,7,7,6,2,
		3,0,4,4,7,3,
		2,1,0,0,3,2,
		4,5,6,6,7,4,
	};
	glBindVertexArray(cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	unsigned int terta_indices[] = { 0,1,2,1,3,2,0,3,1,0,2,3 };
	glBindVertexArray(terta_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, terta_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terta_vertices), terta_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terta_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terta_indices), terta_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLfloat  sphere_vertices[(u_division + 1)*(v_division + 1) * 3*2] = {};
	getSphereVertices(sphere_center, sphere_r, u_division, v_division, sphere_vertices);
	unsigned int  sphere_indices[u_division*v_division * 6] = {};
	getSphereIndices(u_division, v_division,sphere_indices);
	glBindVertexArray(sphere_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	Shader lampShader("lampShader.vs", "lampShader.fs");
	Shader lightingShader("lightingShader.vs", "lightingShader.fs");
	
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		
		ChangePmv(baseShader.ID);
		//draw a cube 
		
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		// view/projection transformations
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setVec3("viewPos", cameraPos);
		lightingShader.setMat4("lightmodel", lightmodel);
		lightingShader.setVec3("lightPos", lightPos);
		glBindVertexArray(cube_VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		lightingShader.setMat4("lightmodel", glm::translate(lightmodel, glm::vec3(3, -3, 2)));
		lightingShader.setVec3("objectColor", 0.0f,1.0f, 0.0f);
		glBindVertexArray(cube_VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		lightingShader.setMat4("lightmodel", glm::translate(lightmodel, glm::vec3(3, 3, 2)));
		lightingShader.setVec3("objectColor", 0.0f, 0.0f, 1.0f);
		glBindVertexArray(cube_VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model",glm::translate(glm::mat4(), lightPos)); 
		glBindVertexArray(terta_VAO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		
		//baseShader.use();
		lightingShader.use();
		lightingShader.setVec3("objectColor", 0.8f, 0.0f, 0.5f);
		lightingShader.setMat4("lightmodel", glm::mat4());
		glBindVertexArray(sphere_VAO);
		glDrawElements(GL_TRIANGLES, (u_division + 1)*(v_division + 1) * 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
	

}

void processInput(GLFWwindow *window)
{

	float cameraSpeed = 2.5f* deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		lightmodel = glm::translate(lightmodel, glm::vec3(1, 0, 0)*deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		lightmodel = glm::translate(lightmodel, glm::vec3(-1, 0, 0)*deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		lightmodel = glm::rotate(lightmodel, glm::radians(30.0f*deltaTime), glm::vec3(1.0f, 1.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		lightmodel = glm::rotate(lightmodel, glm::radians(-30.0f*deltaTime), glm::vec3(1.0f, 1.0f, 0.0f));
	}
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
	if (fov >= 45.0f)
		fov = 45.0f;
}

