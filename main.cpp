#include "Helpers.h"
#include <GLFW/glfw3.h>
#include <Eigen/Core>
#include <chrono>
#include <iostream>
#include <set>

char mode_flag = '0';
//insert mode variables
int mouse_click = 0;
//translate mode variables
bool flag = false;
double mouse_pick_x;
double mouse_pick_y;
int pick_triangleno = -1;
int pick_pointno = -1;
//delete mode
std::set<int> deleted_triangle;
//framing mode
bool f_flag = false;
std::chrono::high_resolution_clock::time_point t_start;
std::chrono::high_resolution_clock::time_point t_now;

// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_C;

// Contains the vertex positions
Eigen::MatrixXf V(2,3);
//translate mode use
Eigen::MatrixXf V_pick_origin(2, 3);
//color mode
Eigen::MatrixXf C(3, 3);
//view mode
Eigen::Matrix4f view(4, 4);
//framing mode
Eigen::MatrixXf framing_origin(2, 3);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void insert_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld);
void insert_mode_cursor(double xworld, double yworld);
void translate_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld);
void translate_mode_cursor(double xworld, double yworld);
int pick_triangle(double xworld, double yworld);
float sign(double p1_x, double p1_y, double p2_x, double p2_y, double p3_x, double p3_y);
bool PointInTriangle(double pt_x, double pt_y, double v1_x, double v1_y, double v2_x, double v2_y, double v3_x, double v3_y);
void delete_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld);
bool check_deleted_set(std::set<int> triangles, int i);
void rotation(char flag);
void scale(char flag);
void color_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld);
void color_change(int key);
int cursor_point(double xworld, double yworld, int  pick_triangleno);
void view_control(char flag);
void framing_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Update the position of the first vertex if the keys 1,2, or 3 are pressed
	switch (key)
	{
	case  GLFW_KEY_I:
		V.resize(2, 1);
		V << 0, 0;
		mode_flag = 'i';
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		break;
	case GLFW_KEY_O:
		mode_flag = 'o';
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		break;
	case GLFW_KEY_P:
		mode_flag = 'p';
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		break;
	case GLFW_KEY_H:
		if (action == GLFW_PRESS && mode_flag == 'o')
			rotation('h');
		break;
	case GLFW_KEY_J:
		if (action == GLFW_PRESS && mode_flag == 'o')
			rotation('j');
		break;
	case GLFW_KEY_K:
		if (action == GLFW_PRESS && mode_flag == 'o')
			scale('k');
		break;
	case GLFW_KEY_L:
		if (action == GLFW_PRESS && mode_flag == 'o')
			scale('l');
		break;
	case GLFW_KEY_C:
		mode_flag = 'c';
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		break;
	case GLFW_KEY_1:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(1);
		break;
	case GLFW_KEY_2:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(2);
		break;
	case GLFW_KEY_3:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(3);
		break;
	case GLFW_KEY_4:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(4);
		break;
	case GLFW_KEY_5:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(5);
		break;
	case GLFW_KEY_6:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(6);
		break;
	case GLFW_KEY_7:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(7);
		break;
	case GLFW_KEY_8:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(8);
		break;
	case GLFW_KEY_9:
		if (action == GLFW_PRESS && mode_flag == 'c')
			color_change(9);
		break;
	case GLFW_KEY_EQUAL:
		mode_flag = 'v';
		if (action == GLFW_PRESS)
			view_control('+');
		break;
	case GLFW_KEY_MINUS:
		mode_flag = 'v';
		if (action == GLFW_PRESS)
			view_control('-');
		break;
	case GLFW_KEY_W:
		mode_flag = 'v';
		if (action == GLFW_PRESS)
			view_control('w');
		break;
	case GLFW_KEY_S:
		mode_flag = 'v';
		if (action == GLFW_PRESS)
			view_control('s');
		break;
	case GLFW_KEY_A:
		mode_flag = 'v';
		if (action == GLFW_PRESS)
			view_control('a');
		break;
	case GLFW_KEY_D:
		mode_flag = 'v';
		if (action == GLFW_PRESS)
			view_control('d');
		break;
	case GLFW_KEY_F:
		mode_flag = 'f';
		deleted_triangle.clear();
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		break;
	default:
		break;
	}

	// Upload the change to the GPU
	VBO.update(V);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; 

	switch (mode_flag) {
	case 'i':
		insert_mode_mouse(window, button, action, xworld, yworld);
		break;
	case 'o':
		translate_mode_mouse(window, button, action, xworld, yworld);
		break;
	case 'p':
		delete_mode_mouse(window, button, action, xworld, yworld);
		break;
	case 'c':
		color_mode_mouse(window, button, action, xworld, yworld);
		break;
	case 'f':
		framing_mode_mouse(window, button, action, xworld, yworld);
		break;
	}
    VBO.update(V);
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	glfwGetCursorPos(window, &xpos, &ypos);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	double xworld = ((xpos / double(width)) * 2) - 1;
	double yworld = (((height - 1 - ypos) / double(height)) * 2) - 1; // NOTE: y axis is flipped in glfw

	switch (mode_flag) {
	case 'i':
		insert_mode_cursor(xworld, yworld);
		break;
	case 'o':
		translate_mode_cursor(xworld, yworld);
		break;
	}
	
	
	VBO.update(V);
}

//insert mode "i"
void insert_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld) 
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		std::cout << "mouse_click" << mouse_click << std::endl;
		if (mouse_click%3==0) {
			V.conservativeResize(2, (mouse_click / 3 + 1) * 3);
			C.conservativeResize(3, (mouse_click / 3 + 1) * 3);
		}
		V.col(mouse_click) << xworld, yworld;
		C.col(mouse_click) << 1.0f, 0.0f, 0.0f;
		std::cout << "V:\n" << V << "\n" << "C\n" << C<<std::endl;
		mouse_click++;
	}
	if (mouse_click % 3 == 1 || mouse_click % 3 == 2) {
		glfwSetCursorPosCallback(window, cursor_position_callback);
	}
	VBO.update(V);
	VBO_C.update(C);
}
void insert_mode_cursor(double xworld, double yworld) 
{
	if (mouse_click % 3 == 1) {
		V.col(mouse_click) << xworld, yworld;
		V.col(mouse_click + 1) << xworld, yworld;
	}
	if (mouse_click % 3 == 2) {
		V.col(mouse_click) << xworld, yworld;
	}
	VBO.update(V);
}

//translate mode "o"
void translate_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		flag = true;
		pick_triangleno = pick_triangle(xworld, yworld);
		mouse_pick_x = xworld;
		mouse_pick_y = yworld;
		if (pick_triangleno != -1) {
			V_pick_origin.col(0) = V.col(3 * pick_triangleno);
			V_pick_origin.col(1) = V.col(3 * pick_triangleno + 1);
			V_pick_origin.col(2) = V.col(3 * pick_triangleno + 2);
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		flag = false;

	glfwSetCursorPosCallback(window, cursor_position_callback);
	VBO.update(V);
}
void translate_mode_cursor(double xworld, double yworld) 
{
	if (flag == true&&pick_triangleno!=-1)
	{
		V.col(3 * pick_triangleno) << V_pick_origin.col(0)[0] + xworld - mouse_pick_x, V_pick_origin.col(0)[1] + yworld - mouse_pick_y;
		V.col(3 * pick_triangleno + 1) << V_pick_origin.col(1)[0] + xworld - mouse_pick_x, V_pick_origin.col(1)[1] + yworld - mouse_pick_y;
		V.col(3 * pick_triangleno + 2) << V_pick_origin.col(2)[0] + xworld - mouse_pick_x, V_pick_origin.col(2)[1] + yworld - mouse_pick_y;
	}
	VBO.update(V);
}
//decide the number of triangle which mouse picks 
int pick_triangle(double xworld, double yworld)
{
	int triangle_number = mouse_click / 3;
	for (int i = 0; i < triangle_number; i++)
	{
		if (PointInTriangle(xworld, yworld, V.col(i * 3)[0], V.col(i * 3)[1], V.col(i * 3 + 1)[0], V.col(i * 3 + 1)[1], V.col(i * 3 + 2)[0], V.col(i * 3 + 2)[1]))
		{
			if(!check_deleted_set(deleted_triangle,i))
				return i;
		}
	}
	return -1;
}
//decide whether in the area of triangle
float sign(double p1_x, double p1_y, double p2_x, double p2_y, double p3_x, double p3_y)
{
	return (p1_x - p3_x) * (p2_y - p3_y) - (p2_x - p3_x) * (p1_y - p3_y);
}
bool PointInTriangle(double pt_x, double pt_y, double v1_x, double v1_y, double v2_x, double v2_y, double v3_x, double v3_y)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt_x, pt_y, v1_x, v1_y, v2_x, v2_y);
	d2 = sign(pt_x, pt_y, v2_x, v2_y, v3_x, v3_y);
	d3 = sign(pt_x, pt_y, v3_x, v3_y, v1_x, v1_y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

//delete mode "p"
void delete_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		pick_triangleno = pick_triangle(xworld, yworld);
		if (pick_triangleno != -1) {
			deleted_triangle.insert(pick_triangleno);
			V.col(3 * pick_triangleno) << 0, 0;
			V.col(3 * pick_triangleno + 1) << 0, 0;
			V.col(3 * pick_triangleno + 2) << 0, 0;
		}
	}
	//glfwSetCursorPosCallback(window, cursor_position_callback);
	VBO.update(V);
}
bool check_deleted_set(std::set<int> triangles, int i) {
	bool flag = false;

	for (auto triangle : triangles)
		if (triangle == i)
			flag = true;

	return flag;
}

//rotation 'h' 'j'
void rotation(char flag)
{
	std::cout << "rotation" << std::endl;
	std::cout << "Triangel: " << pick_triangleno << std::endl;
	double pi = 3.1415926535;
	double center_x = (V.col(pick_triangleno * 3)[0] + V.col(pick_triangleno * 3 + 1)[0] + V.col(pick_triangleno + 2)[0]) / 3;
	double center_y = (V.col(pick_triangleno * 3)[1] + V.col(pick_triangleno * 3 + 1)[1] + V.col(pick_triangleno * 3 + 2)[1]) / 3;
	double angle;
	if (flag == 'h')
	{
		angle = -pi / 18.0;
	}
	if (flag == 'j')
	{
		angle = pi / 18;
	}
	if (pick_triangleno != -1)
	{
		V.col(pick_triangleno * 3)[0] = center_x + (V.col(pick_triangleno * 3)[0] - center_x) * (float)cos(angle) - (V.col(pick_triangleno * 3)[1] - center_y) * (float)sin(angle);
		V.col(pick_triangleno * 3)[1] = center_x + (V.col(pick_triangleno * 3)[0] - center_x) * (float)sin(angle) + (V.col(pick_triangleno * 3)[1] - center_y) * (float)cos(angle);

		V.col(pick_triangleno * 3 + 1)[0] = center_x + (V.col(pick_triangleno * 3 + 1)[0] - center_x) * (float)cos(angle) - (V.col(pick_triangleno * 3 + 1)[1] - center_y) * (float)sin(angle);
		V.col(pick_triangleno * 3 + 1)[1] = center_x + (V.col(pick_triangleno * 3 + 1)[0] - center_x) * (float)sin(angle) + (V.col(pick_triangleno * 3 + 1)[1] - center_y) * (float)cos(angle);

		V.col(pick_triangleno * 3 + 2)[0] = center_x + (V.col(pick_triangleno * 3 + 2)[0] - center_x) * (float)cos(angle) - (V.col(pick_triangleno * 3 + 2)[1] - center_y) * (float)sin(angle);
		V.col(pick_triangleno * 3 + 2)[1] = center_x + (V.col(pick_triangleno * 3 + 2)[0] - center_x) * (float)sin(angle) + (V.col(pick_triangleno * 3 + 2)[1] - center_y) * (float)cos(angle);

		VBO.update(V);
	}
}
//scale 'k' 'l'
void scale(char flag)
{
	double center_x = (V.col(pick_triangleno * 3)[0] + V.col(pick_triangleno * 3 + 1)[0] + V.col(pick_triangleno + 2)[0]) / 3;
	double center_y = (V.col(pick_triangleno * 3)[1] + V.col(pick_triangleno * 3 + 1)[1] + V.col(pick_triangleno * 3 + 2)[1]) / 3;
	Eigen::MatrixXf scale(2, 2);
	if (pick_triangleno != -1)
	{
		if (flag == 'k')
		{
			V.col(pick_triangleno * 3)[0] = center_x + (V.col(pick_triangleno * 3)[0] - center_x) * 1.25;
			V.col(pick_triangleno * 3)[1] = center_y + (V.col(pick_triangleno * 3)[1] - center_y) * 1.25;
			V.col(pick_triangleno * 3 + 1)[0] = center_x + (V.col(pick_triangleno * 3 + 1)[0] - center_x) * 1.25;
			V.col(pick_triangleno * 3 + 1)[1] = center_y + (V.col(pick_triangleno * 3 + 1)[1] - center_y) * 1.25;
			V.col(pick_triangleno * 3 + 2)[0] = center_x + (V.col(pick_triangleno * 3 + 2)[0] - center_x) * 1.25;
			V.col(pick_triangleno * 3 + 2)[1] = center_y + (V.col(pick_triangleno * 3 + 2)[1] - center_y) * 1.25;
		}
		if (flag == 'l')
		{
			V.col(pick_triangleno * 3)[0] = center_x + (V.col(pick_triangleno * 3)[0] - center_x) * 0.75;
			V.col(pick_triangleno * 3)[1] = center_y + (V.col(pick_triangleno * 3)[1] - center_y) * 0.75;
			V.col(pick_triangleno * 3 + 1)[0] = center_x + (V.col(pick_triangleno * 3 + 1)[0] - center_x) * 0.75;
			V.col(pick_triangleno * 3 + 1)[1] = center_y + (V.col(pick_triangleno * 3 + 1)[1] - center_y) * 0.75;
			V.col(pick_triangleno * 3 + 2)[0] = center_x + (V.col(pick_triangleno * 3 + 2)[0] - center_x) * 0.75;
			V.col(pick_triangleno * 3 + 2)[1] = center_y + (V.col(pick_triangleno * 3 + 2)[1] - center_y) * 0.75;
		}
	}
	VBO.update(V);
}

//color mode 'c'
void color_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pick_triangleno = pick_triangle(xworld, yworld);
		if (pick_triangleno != -1)
		{
			pick_pointno = cursor_point(xworld, yworld, pick_triangleno);
		}
	}
}
int cursor_point(double xworld, double yworld, int  pick_triangleno) {
	int pos = -1;
	double p1_x = V.col(3 * pick_triangleno)[0];
	double p1_y = V.col(3 * pick_triangleno)[1];
	double p2_x = V.col(3 * pick_triangleno + 1)[0];
	double p2_y = V.col(3 * pick_triangleno + 1)[1];
	double p3_x = V.col(3 * pick_triangleno + 2)[0];
	double p3_y = V.col(3 * pick_triangleno + 2)[1];
	double pos1 = (xworld - p1_x) * (xworld - p1_x) + (yworld - p1_y) * (yworld - p1_y);
	double pos2 = (xworld - p2_x) * (xworld - p2_x) + (yworld - p2_y) * (yworld - p2_y);
	double pos3 = (xworld - p3_x) * (xworld - p3_x) + (yworld - p3_y) * (yworld - p3_y);

	if (pos1 <= pos2 && pos1 <= pos3)
		pos = 0;
	if (pos2 <= pos1 && pos2 <= pos3)
		pos = 1;
	if (pos3 <= pos1 && pos3 <= pos2)
		pos = 2;

	return pos;
}
void color_change(int key) {
	if (pick_triangleno != -1 && pick_pointno != -1)
	{
		switch (key) 
		{
		case 1:
			C.col(3 * pick_triangleno + pick_pointno) << 1.0, 0.98, 0.8;
			VBO_C.update(C);
			break;
		case 2:
			C.col(3 * pick_triangleno + pick_pointno) << 0.96, 1.0, 0.98;
				VBO_C.update(C);
			break;
		case 3:
			C.col(3 * pick_triangleno + pick_pointno) << 0.90, 0.98, 0.98;
				VBO_C.update(C);
			break;
		case 4:
			C.col(3 * pick_triangleno + pick_pointno) << 1.0, 0.94, 0.96;
				VBO_C.update(C);
			break;
		case 5:
			C.col(3 * pick_triangleno + pick_pointno) << 0.61, 0.80, 0.60;
				VBO_C.update(C);
			break;
		case 6:
			C.col(3 * pick_triangleno + pick_pointno) << 0.54, 0.54, 0.47;
				VBO_C.update(C);
			break;
		case 7:
			C.col(3 * pick_triangleno + pick_pointno) << 0.54, 0.39, 0.03;
				VBO_C.update(C);
			break;
		case 8:
			C.col(3 * pick_triangleno + pick_pointno) << 1.0, 0.38, 0.27;
				VBO_C.update(C);
			break;
		case 9:
			C.col(3 * pick_triangleno + pick_pointno) << 0.06, 0.3, 0.54;
				VBO_C.update(C);
			break;
		default:
			break;
		}
	}
}

//view control mode 'v'
void view_control(char flag)
{
	Eigen::Matrix4f scale(4, 4);
	if (flag == '+')
	{
		scale << 1.2, 0,   0,   0,
			     0,   1.2, 0,   0,
				 0,   0,   1.2, 0,
				 0,   0,   0,   1;
		view = view * scale;
	}
	if (flag == '-')
	{
		scale<< 0.8, 0,   0,   0,
			    0,   0.8, 0,   0,
			    0,   0,   0.8, 0,
			    0,   0,   0,   1;
		view = view * scale;
	}
	if (flag == 'w')
	{
		scale << 0, 0, 0, 0,
				 0, 0, 0, 0.2,
				 0, 0, 0, 0,
				 0, 0, 0, 0;
		view = view + scale;
	}
	if (flag == 's')
	{
		scale << 0, 0, 0, 0,
				 0, 0, 0, -0.2,
				 0, 0, 0, 0,
				 0, 0, 0, 0;
		view = view + scale;
	}
	if (flag == 'a')
	{
		scale << 0, 0, 0, -0.2,
				 0, 0, 0, 0,
				 0, 0, 0, 0,
				 0, 0, 0, 0;
		view = view + scale;
	}
	if (flag == 'd')
	{
		scale << 0, 0, 0, 0.2,
				 0, 0, 0, 0,
				 0, 0, 0, 0,
				 0, 0, 0, 0;
		view = view + scale;
	}
}

//framing mode 'f'
void framing_mode_mouse(GLFWwindow* window, int button, int action, double xworld, double yworld)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::cout << "pressing!!" << std::endl;

		pick_triangleno = pick_triangle(xworld, yworld);
		std::cout << "triangle: " << pick_triangleno << std::endl;
		if (pick_triangleno != -1)
		{
			f_flag = true;
			t_start = std::chrono::high_resolution_clock::now();
			framing_origin.col(0) << V.col(3 * pick_triangleno)[0], V.col(3*pick_triangleno)[1];
			framing_origin.col(1) << V.col(3 * pick_triangleno + 1)[0], V.col(3*pick_triangleno + 1)[1];
			framing_origin.col(2) << V.col(3 * pick_triangleno + 2)[0], V.col(3*pick_triangleno + 2)[1];
			std::cout << "nonono:"<<pick_triangleno << std::endl;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		f_flag = false;
	}
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    VBO.init();

    V.resize(2,3);
    V << 0,  0.5, -0.5, 0.5, -0.5, -0.5;
    VBO.update(V);

	VBO_C.init();

	C.resize(3, 3);
	C << 0, 0, 0,
		0, 0, 0,
		1, 1, 1;
	VBO_C.update(C);
    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
	const GLchar* vertex_shader =
		"#version 150 core\n"
		"in vec2 position;"
		"in vec3 color;"
		"out vec3 f_color;"
		"uniform mat4 view;"
		"void main()"
		"{"
		"    gl_Position =  view * vec4(position, 0.0, 1.0);"
		"    f_color = color;"
		"}";
	const GLchar* fragment_shader =
		"#version 150 core\n"
		"in vec3 f_color;"
		"out vec4 outColor;"
		"void main()"
		"{"
		"    outColor = vec4(f_color, 1.0);"
		"}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position",VBO);
	program.bindVertexAttribArray("color", VBO_C);

    // Save the current time --- it will be used to dynamically change the triangle color
    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	//view
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float aspect_ratio = float(height) / float(width);
	view << aspect_ratio, 0, 0, 0,
		    0,			  1, 0, 0,
		    0,			  0, 1, 0,
		    0,			  0, 0, 1;
	glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());
	// Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        // Set the uniform value depending on the time difference


        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle
        //glDrawArrays(GL_TRIANGLES, 0, 3);

		if (mode_flag == 'i') {
			int number_triangle = mouse_click / 3;
			for (int i = 0; i <number_triangle; i++) {
				//std::cout << "V:\n" << V << "\n" << "C\n" << C << std::endl;
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
			if (mouse_click % 3 == 1) {
				glDrawArrays(GL_LINE_LOOP, mouse_click-1, 2);
			}
			if (mouse_click % 3 == 2) {
				glDrawArrays(GL_LINE_LOOP, mouse_click-2, 3);
			}
		}
		if (mode_flag == 'o') {
			int number_triangle = mouse_click / 3;
			for (int i = 0; i < number_triangle; i++) {
				//if(i!=pick_triangleno)
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
			if (flag == true&&pick_triangleno!=-1) {
				//glUniform3f(program.uniform("triangleColor"), 0.0f, 0.899f, 0.0f);
				C.col(3 * pick_triangleno) << 0, 1, 0;
				C.col(3 * pick_triangleno + 1) << 0, 1, 0;
				C.col(3 * pick_triangleno + 2) << 0, 1, 0;
				VBO_C.update(C);
				glDrawArrays(GL_TRIANGLES, 3 * pick_triangleno, 3);
			}

			if(flag==false&&pick_triangleno!=-1 ){
				C.col(3 * pick_triangleno) << 1, 0, 0;
				C.col(3 * pick_triangleno + 1) << 1, 0, 0;
				C.col(3 * pick_triangleno + 2) << 1, 0, 0;
				VBO_C.update(C);
			}
		}
		if (mode_flag == 'p') {
			int number_triangle = mouse_click / 3;
			for (int i = 0; i < number_triangle; i++) {
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
		}
		if (mode_flag == 'c') {
			int number_triangle = mouse_click / 3;
			for (int i = 0; i < number_triangle; i++) {
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
		}
		if (mode_flag == 'v') {
			int number_triangle = mouse_click / 3;
			for (int i = 0; i < number_triangle; i++) {
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
			glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());
		}
		if (mode_flag == 'f') {
			int number_triangle = mouse_click / 3;
			for (int i = 0; i < number_triangle; i++) {
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
			if (f_flag == true)
			{
				t_now = std::chrono::high_resolution_clock::now();
				float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

				std::cout << "time: " << time<<std::endl;
				std::cout << "picked triangle: " <<pick_triangleno<<std::endl;
				std::cout << "original: \n" << framing_origin << std::endl;
				if (time < 5) {
					V.col(3*pick_triangleno)[1] = framing_origin.col(0)[1] + time/2;
					V.col(3*pick_triangleno+1)[1] = framing_origin.col(1)[1] + time/2;
					V.col(3*pick_triangleno+2)[1] = framing_origin.col(2)[1] + time/2;

					VBO.update(V);

					glDrawArrays(GL_TRIANGLES, 3 * pick_triangleno, 3);
				}

			}
		}


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}