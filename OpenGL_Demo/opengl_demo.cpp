#include <GL/glew.h>
#include <GL/glut.h>

#include <math.h>
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>

#include "opengl_demo.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////

void init(void);
void reshape(int width, int height);
void display(void);

void init_lights();
void set_lights();
void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

/* Index 0 has the x-coordinate
 * Index 1 has the y-coordinate
 * Index 2 has the z-coordinate
 */
float cam_position[] = {0.9, 0, 15.4};
float cam_orientation_axis[] = {0, 0, 1};

/* Angle in degrees.
 */ 
float cam_orientation_angle = 0;

float near_param = 1, far_param = 200,
      left_param = -0.5, right_param = 0.5,
      top_param = 0.5, bottom_param = -0.5;

///////////////////////////////////////////////////////////////////////////////////////////////////

/* Self-explanatory lists of lights and objects.
 */
 
vector<Point_Light> lights;
vector<Object> objects;

///////////////////////////////////////////////////////////////////////////////////////////////////

int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

///////////////////////////////////////////////////////////////////////////////////////////////////

/* The following function prototypes are for helper functions that we made to
 * initialize some point lights and cube objects.
 *
 * Details of the functions will be given in their respective implementations
 * further below.
 */

void create_lights();
void create_cubes();

///////////////////////////////////////////////////////////////////////////////////////////////////

/* From here on are all the function implementations.
 */


void update_camera_matrix()
{
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    
    glFrustum(left_param, right_param,
              bottom_param, top_param,
              near_param, far_param);
    
    
    glTranslatef(-cam_position[0], -cam_position[1], -cam_position[2]);
    
    glMatrixMode(GL_MODELVIEW);
    
}
 

/* 'init' function:
 * 
 * As you would expect, the 'init' function initializes and sets up the
 * program. It should always be called before anything else.
 *
 * Writing an 'init' function is not required by OpenGL. If you wanted to, you
 * could just put all your initializations in the beginning of the 'main'
 * function instead. However, doing so is bad style; it is cleaner to have all
 * your initializations contained within one function.
 * 
 * Before we go into the function itself, it is important to mention that
 * OpenGL works like a state machine. It will do different procedures depending
 * on what state it is in.
 *
 * For instance, OpenGL has different states for its shading procedure. By
 * default, OpenGL is in "flat shading state", meaning it will always use flat
 * shading when we tell it to render anything. With some syntax, we can change
 * the shading procedure from the "flat shading state" to the "Gouraud shading
 * state", and then OpenGL will render everything using Gouraud shading.
 *
 * The most important task of the 'init' function is to set OpenGL to the
 * states that we want it to be in.
 */
void init(void)
{

    glShadeModel(GL_SMOOTH);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_NORMALIZE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    //set up the camera with default params
    update_camera_matrix();
    
    
    create_cubes();
    create_lights();
    
    init_lights();
}

void reshape(int width, int height)
{
    /* The following two lines of code prevent the width and height of the
     * window from ever becoming 0 to prevent divide by 0 errors later.
     * Typically, we let 1x1 square pixel be the smallest size for the window.
     */
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;
    
    glViewport(0, 0, width, height);
    
    mouse_scale_x = (float) (right_param - left_param) / (float) width;
    mouse_scale_y = (float) (top_param - bottom_param) / (float) height;
    
    glutPostRedisplay();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);

    glRotatef(-cam_orientation_angle,
              cam_orientation_axis[0], cam_orientation_axis[1], cam_orientation_axis[2]);
    
    set_lights();
    
    draw_objects();
    
    glutSwapBuffers();
}

void init_lights()
{
    glEnable(GL_LIGHTING);
    
    int num_lights = lights.size();
    
    for(int i = 0; i < num_lights; ++i)
    {

        int light_id = GL_LIGHT0 + i;
        
        glEnable(light_id);
        
        glLightfv(light_id, GL_AMBIENT, lights[i].color);
        glLightfv(light_id, GL_DIFFUSE, lights[i].color);
        glLightfv(light_id, GL_SPECULAR, lights[i].color);
        
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, lights[i].attenuation_k);
    }
}

void set_lights()
{
    int num_lights = lights.size();
    
    for(int i = 0; i < num_lights; ++i)
    {
        int light_id = GL_LIGHT0 + i;
        
        glLightfv(light_id, GL_POSITION, lights[i].position);
    }
}

/* 'draw_objects' function:
 *
 * This function has OpenGL render our objects to the display screen. It
 */
void draw_objects()
{
    int num_objects = objects.size();
    
    for(int i = 0; i < num_objects; ++i)
    {

        glPushMatrix();
        /* The following brace is not necessary, but it keeps things organized.
         */
        {
            int num_transform_sets = objects[i].transform_sets.size();
            
            for(int j = 0; j < num_transform_sets; ++j)
            {
                glTranslatef(objects[i].transform_sets[j].translation[0],
                             objects[i].transform_sets[j].translation[1],
                             objects[i].transform_sets[j].translation[2]);
                glRotatef(objects[i].transform_sets[j].rotation_angle,
                          objects[i].transform_sets[j].rotation[0],
                          objects[i].transform_sets[j].rotation[1],
                          objects[i].transform_sets[j].rotation[2]);
                glScalef(objects[i].transform_sets[j].scaling[0],
                         objects[i].transform_sets[j].scaling[1],
                         objects[i].transform_sets[j].scaling[2]);
            }
            
            glMaterialfv(GL_FRONT, GL_AMBIENT, objects[i].ambient_reflect);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, objects[i].diffuse_reflect);
            glMaterialfv(GL_FRONT, GL_SPECULAR, objects[i].specular_reflect);
            glMaterialf(GL_FRONT, GL_SHININESS, objects[i].shininess);
            
            glVertexPointer(3, GL_FLOAT, 0, &objects[i].vertex_buffer[0]);

            glNormalPointer(GL_FLOAT, 0, &objects[i].normal_buffer[0]);
            
            int buffer_size = objects[i].vertex_buffer.size();
            
            if(!wireframe_mode)
                glDrawArrays(GL_TRIANGLES, 0, buffer_size);
            else
                for(int j = 0; j < buffer_size; j += 3)
                    glDrawArrays(GL_LINE_LOOP, j, 3);
        }
        glPopMatrix();
    }
    
}

//TODO arcball
void mouse_pressed(int button, int state, int x, int y)
{
    /* If the left-mouse button was clicked down, then...
     */
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        /* Store the mouse position in our global variables.
         */
        mouse_x = x;
        mouse_y = y;
        
        /* Since the mouse is being pressed down, we set our 'is_pressed"
         * boolean indicator to true.
         */
        is_pressed = true;
    }
    /* If the left-mouse button was released up, then...
     */
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        /* Mouse is no longer being pressed, so set our indicator to false.
         */
        is_pressed = false;
    }
}

void mouse_moved(int x, int y)
{
}

/* 'deg2rad' function:
 */
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

//with a given camera position
//update the camera fustrum parameters
void update_camera_parameters()
{
    float screen_dim = 10.0;
    left_param = (-screen_dim / 2 - cam_position[0]) / cam_position[2];
    right_param = (screen_dim / 2 - cam_position[0]) / cam_position[2];
    top_param = (screen_dim / 2 - cam_position[1]) / cam_position[2];
    bottom_param = (-screen_dim / 2 - cam_position[1]) / cam_position[2];
}

void key_pressed(unsigned char key, int x, int y)
{
    //camera update stuff test 

    if(key == 'z')
    {
        
        //left_param += 0.1;
        //right_param += 0.1;

        double step = 0.1;
        cam_position[0] -= step;

        double fraction =  near_param / cam_position[2];
        left_param += step * fraction;
        right_param += step * fraction;

        cout << right_param << endl;


        update_camera_matrix();
        glutPostRedisplay();
        return;
    }
    else if(key == 'x')
    {
        update_camera_matrix();
        glutPostRedisplay();
        return;
    }
    /* If 'q' is pressed, quit the program.
     */
    if(key == 'q')
    {
        exit(0);
    }
    /* If 't' is pressed, toggle our 'wireframe_mode' boolean to make OpenGL
     * render our cubes as surfaces of wireframes.
     */
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        /* Tell OpenGL that it needs to re-render our scene with the cubes
         * now as wireframes (or surfaces if they were wireframes before).
         */
        glutPostRedisplay();
    }
    else
    {
        
        float x_view_rad = deg2rad(x_view_angle);
        
        /* 'w' for step forward
         */
        if(key == 'w')
        {
            cam_position[2] -= step_size;
            update_camera_parameters();
        }
        /* 'a' for step left
         */
        else if(key == 'a')
        {
            cam_position[0] -= step_size;
            update_camera_parameters();
        }
        /* 's' for step backward
         */
        else if(key == 's')
        {
            cam_position[2] += step_size;
            update_camera_parameters();
        }
        /* 'd' for step right
         */
        else if(key == 'd')
        {
            cam_position[0] += step_size;
            update_camera_parameters();
        }
        else if(key == 'q')
        {
            cam_position[1] -= step_size;
            update_camera_parameters();
        }
        else if(key == 'e')
        {
            cam_position[1] -= step_size;
            update_camera_parameters();
        }
        update_camera_matrix();
        glutPostRedisplay();
    }
}

/* 'create_lights' function:
 *
 * This function is relatively uninteresting. We are just hardcoding all the
 * properties of our point light objects. You do not need to do this because
 * the parser will already have all the point light objects initialized.
 */
void create_lights()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Light 1 Below
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Point_Light light1;
    
    light1.position[0] = -0.8;
    light1.position[1] = 0;
    light1.position[2] = 1;
    light1.position[3] = 1;
    
    light1.color[0] = 1;
    light1.color[1] = 1;
    light1.color[2] = 0;
    light1.attenuation_k = 0.2;
    
    lights.push_back(light1);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Light 2 Below
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Point_Light light2;
    
    light2.position[0] = 0.15;
    light2.position[1] = 0.85;
    light2.position[2] = 0.7;
    light2.position[3] = 1;
    
    light2.color[0] = 1;
    light2.color[1] = 0;
    light2.color[2] = 1;
    
    light2.attenuation_k = 0.1;
    
    lights.push_back(light2);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Light 3 Below
    ///////////////////////////////////////////////////////////////////////////////////////////////    
    
    Point_Light light3;
    
    light3.position[0] = 0.5;
    light3.position[1] = -0.5;
    light3.position[2] = 0.85;
    light3.position[3] = 1;
    
    light3.color[0] = 0;
    light3.color[1] = 1;
    light3.color[2] = 1;
    
    light3.attenuation_k = 0;
    
    lights.push_back(light3);
}

/* 'create_cubes' function:
 *
 * We hardcode all the properties of our cubes in this function. The only
 * relatively interesting part of this function is seeing how we form our
 * vertex and normal arrays. You will be able to form your vertex and normal
 * arrays more elegantly than this because the parser will have all the vertices,
 * normals, and facesets all stored in nice vectors. You will also not have to
 * hardcode any reflectances, transformations, etc, since the parser will also
 * have those all initialized for you.
 *
 * We are rendering our cubes with triangles, so each cube has 12 (triangle) faces
 * in all.
 */
void create_cubes()
{
    Object cube1;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Reflectances
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    cube1.ambient_reflect[0] = 0.2;
    cube1.ambient_reflect[1] = 0.2;
    cube1.ambient_reflect[2] = 0.2;
    
    cube1.diffuse_reflect[0] = 0.6;
    cube1.diffuse_reflect[1] = 0.6;
    cube1.diffuse_reflect[2] = 0.6;
    
    cube1.specular_reflect[0] = 1;
    cube1.specular_reflect[1] = 1;
    cube1.specular_reflect[2] = 1;
    
    cube1.shininess = 5.0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Points
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Triple point1;
    point1.x = -1;
    point1.y = -1;
    point1.z = 1;
    
    Triple point2;
    point2.x = 1;
    point2.y = -1;
    point2.z = 1;
    
    Triple point3;
    point3.x = 1;
    point3.y = 1;
    point3.z = 1;
    
    Triple point4;
    point4.x = -1;
    point4.y = 1;
    point4.z = 1;
    
    Triple point5;
    point5.x = -1;
    point5.y = -1;
    point5.z = -1;
    
    Triple point6;
    point6.x = 1;
    point6.y = -1;
    point6.z = -1;
    
    Triple point7;
    point7.x = 1;
    point7.y = 1;
    point7.z = -1;
    
    Triple point8;
    point8.x = -1;
    point8.y = 1;
    point8.z = -1;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Normals
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Triple normal1;
    normal1.x = 0;
    normal1.y = 0;
    normal1.z = 1;
    
    Triple normal2;
    normal2.x = 0;
    normal2.y = 0;
    normal2.z = -1;
    
    Triple normal3;
    normal3.x = 0;
    normal3.y = 1;
    normal3.z = 0;
    
    Triple normal4;
    normal4.x = 0;
    normal4.y = -1;
    normal4.z = 0;
    
    Triple normal5;
    normal5.x = 1;
    normal5.y = 0;
    normal5.z = 0;
    
    Triple normal6;
    normal6.x = -1;
    normal6.y = 0;
    normal6.z = 0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Vertex and Normal Arrays
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    /* We are rendering our cubes with triangles, so each cube has 12 (triangle) faces
     * in all.
     */
    
    /* Face 1: */
    
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal1);
    
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal1);
    
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal1);
    
    /* Face 2: */
    
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal1);
    
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal1);
    
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal1);
    
    /* Face 3: */
    
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal2);
    
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal2);
    
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal2);
    
    /* Face 4: */
    
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal2);
    
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal2);
    
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal2);
    
    /* Face 5: */
    
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal5);
    
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal5);
    
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal5);
    
    /* Face 6: */
    
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal5);
    
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal5);
    
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal5);
    
    /* Face 7: */
    
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal6);
    
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal6);
    
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal6);
    
    /* Face 8: */
    
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal6);
    
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal6);
    
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal6);
    
    /* Face 9: */
    
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal3);
    
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal3);
    
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal3);
    
    /* Face 10: */
    
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal3);
    
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal3);
    
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal3);
    
    /* Face 11: */
    
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal4);
    
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal4);
    
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal4);
    
    /* Face 12: */
    
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal4);
    
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal4);
    
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal4);
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Cube 2
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    /* We are just going to make them identical out of laziness... */
    Object cube2 = cube1;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Transformations for Cube 1
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Transforms transforms1;
    
    transforms1.translation[0] = -0.6;
    transforms1.translation[1] = 0;
    transforms1.translation[2] = 0;
    
    transforms1.rotation[0] = 1;
    transforms1.rotation[1] = 1;
    transforms1.rotation[2] = 0;
    transforms1.rotation_angle = 60;
    
    transforms1.scaling[0] = 0.5;
    transforms1.scaling[1] = 0.5;
    transforms1.scaling[2] = 0.5;
    
    cube1.transform_sets.push_back(transforms1);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Transformations for Cube 2
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Transforms transforms2;
    
    transforms2.translation[0] = 2.0;
    transforms2.translation[1] = 0;
    transforms2.translation[2] = 0;
    
    transforms2.rotation[0] = 0;
    transforms2.rotation[1] = 1;
    transforms2.rotation[2] = 0;
    transforms2.rotation_angle = 135;
    
    transforms2.scaling[0] = 1.5;
    transforms2.scaling[1] = 1.5;
    transforms2.scaling[2] = 1.5;
    
    Transforms transforms3;
    
    transforms3.translation[0] = 0;
    transforms3.translation[1] = 0;
    transforms3.translation[2] = 0;
    
    transforms3.rotation[0] = 1;
    transforms3.rotation[1] = 0;
    transforms3.rotation[2] = 0;
    transforms3.rotation_angle = -0;
    
    transforms3.scaling[0] = 0.5;
    transforms3.scaling[1] = 0.5;
    transforms3.scaling[2] = 0.5;
    
    cube2.transform_sets.push_back(transforms2);
    cube2.transform_sets.push_back(transforms3);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Push to Objects
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    objects.push_back(cube1);
    objects.push_back(cube2);
}

/* The 'main' function:
 *
 * This function is short, but is basically where everything comes together.
 */
int main(int argc, char* argv[])
{
    int xres = 500;
    int yres = 500;
    
    /* 'glutInit' intializes the GLUT (Graphics Library Utility Toolkit) library.
     * This is necessary, since a lot of the functions we used above and below
     * are from the GLUT library.
     *
     * 'glutInit' takes the 'main' function arguments as parameters. This is not
     * too important for us, but it is possible to give command line specifications
     * to 'glutInit' by putting them with the 'main' function arguments.
     */
    glutInit(&argc, argv);
    /* The following line of code tells OpenGL that we need a double buffer,
     * a RGB pixel buffer, and a depth buffer.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /* The following line tells OpenGL to create a program window of size
     * 'xres' by 'yres'.
     */
    glutInitWindowSize(xres, yres);
    /* The following line tells OpenGL to set the program window in the top-left
     * corner of the computer screen (0, 0).
     */
    glutInitWindowPosition(0, 0);
    /* The following line tells OpenGL to name the program window "Test".
     */
    glutCreateWindow("Test");
    
    /* Call our 'init' function...
     */
    init();
    /* Specify to OpenGL our display function.
     */
    glutDisplayFunc(display);
    /* Specify to OpenGL our reshape function.
     */
    glutReshapeFunc(reshape);
    /* Specify to OpenGL our function for handling mouse presses.
     */
    glutMouseFunc(mouse_pressed);
    /* Specify to OpenGL our function for handling mouse movement.
     */
    glutMotionFunc(mouse_moved);
    /* Specify to OpenGL our function for handling key presses.
     */
    glutKeyboardFunc(key_pressed);
    /* The following line tells OpenGL to start the "event processing loop". This
     * is an infinite loop where OpenGL will continuously use our display, reshape,
     * mouse, and keyboard functions to essentially run our program.
     */
    glutMainLoop();
}
