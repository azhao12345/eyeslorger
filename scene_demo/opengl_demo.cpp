#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#define _USE_MATH_DEFINES 
#include <iostream>
#include <vector>

#include "../inputpipe.h"

using namespace std;
void init(void);
void reshape(int width, int height);
void display(void);
void init_lights();
void set_lights();
void draw_objects();
void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);
struct Point_Light
{
    float position[4];
    float color[3];
    float attenuation_k;
};
struct Triple
{
    float x;
    float y;
    float z;
};
struct Transforms
{
    float translation[3];
    float rotation[3];
    float scaling[3];
    float rotation_angle;
};
struct Object
{
    vector<Triple> vertex_buffer;
    vector<Triple> normal_buffer;
    vector<Transforms> transform_sets;
    float ambient_reflect[3];
    float diffuse_reflect[3];
    float specular_reflect[3];
    float shininess;
};
float cam_position[] = {0, 0, 45.4};
float cam_orientation_axis[] = {0, 0, 1};
float cam_orientation_angle = 0;
float near_param = 1, far_param = 200,
      left_param = -0.5, right_param = 0.5,
      top_param = 0.5, bottom_param = -0.5;
vector<Point_Light> lights;
vector<Object> objects;
int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;
const float step_size = 2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;
bool is_pressed = false;
bool wireframe_mode = false;
void create_lights();
void create_cubes();

void run_video_loop()
{
    //video_loop(update);
    float pos[3];
    float rot[3];
    if(read_marker_data(pos, rot))
    {
        //offset of the camera from the center of the thing
        objects[1].transform_sets[0].translation[0] = -pos[0];
        objects[1].transform_sets[0].translation[1] = -pos[1];
        objects[1].transform_sets[0].translation[2] = pos[2];

        float angle = sqrt(rot[0] * rot[0] 
            + rot[1] * rot[1]
            + rot[2] * rot[2]);

        rot[0] /= angle;
        rot[1] /= angle;
        rot[2] /= angle;

        objects[1].transform_sets[1].rotation[0] = -rot[0];
        objects[1].transform_sets[1].rotation[1] = -rot[1];
        objects[1].transform_sets[1].rotation[2] = rot[2];

        objects[1].transform_sets[1].rotation_angle = angle / M_PI * 180.0;
        
        glutPostRedisplay();
    }
}

void init(void)
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left_param, right_param,
              bottom_param, top_param,
              near_param, far_param);
    glMatrixMode(GL_MODELVIEW);
    create_cubes();
    create_lights();
    init_lights();
}
void reshape(int width, int height)
{
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
    glTranslatef(-cam_position[0], -cam_position[1], -cam_position[2]);
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);
    glRotatef(-cam_orientation_angle,
              cam_orientation_axis[0], cam_orientation_axis[1], cam_orientation_axis[2]);
    set_lights();
    draw_objects();

    glPushMatrix();

    glBegin(GL_LINE_LOOP);
    glVertex3f(-16, -1, 0);
    glVertex3f(16, -1, 0);
    glVertex3f(16, -19, 0);
    glVertex3f(-16, -19, 0);
    glEnd();

    glPopMatrix();

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
void draw_objects()
{
    int num_objects = objects.size();
    for(int i = 0; i < num_objects; ++i)
    {
        glPushMatrix();
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
    glPushMatrix();
    {
        glTranslatef(0, -103, 0);
        //glutSolidSphere(100, 100, 100);
    }
    glPopMatrix();
}
void mouse_pressed(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouse_x = x;
        mouse_y = y;
        is_pressed = true;
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        is_pressed = false;
    }
}
void mouse_moved(int x, int y)
{
    if(is_pressed)
    {
        x_view_angle += ((float) x - (float) mouse_x) * mouse_scale_x * x_view_step;
        float temp_y_view_angle = y_view_angle +
                                  ((float) y - (float) mouse_y) * mouse_scale_y * y_view_step;
        y_view_angle = (temp_y_view_angle > 90 || temp_y_view_angle < -90) ?
                       y_view_angle : temp_y_view_angle;
        mouse_x = x;
        mouse_y = y;
        glutPostRedisplay();
    }
}
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}
void key_pressed(unsigned char key, int x, int y)
{
    if(key == 'q')
    {
        exit(0);
    }
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        glutPostRedisplay();
    }
    else
    {
        float x_view_rad = deg2rad(x_view_angle);
        if(key == 'w')
        {
            cam_position[2] -= step_size;
            glutPostRedisplay();
        }
        else if(key == 's')
        {
            cam_position[2] += step_size;
            glutPostRedisplay();
        }
    }
}
void create_lights()
{
    Point_Light light1;
    light1.position[0] = -0.8;
    light1.position[1] = 0;
    light1.position[2] = 1;
    light1.position[3] = 1;
    light1.color[0] = 1;
    light1.color[1] = 1;
    light1.color[2] = 1;
    light1.attenuation_k = 0;
    lights.push_back(light1);
    Point_Light light2;
    light2.position[0] = 0.15;
    light2.position[1] = 0.85;
    light2.position[2] = 0.7;
    light2.position[3] = -40;
    light2.color[0] = 1;
    light2.color[1] = 1;
    light2.color[2] = 1;
    light2.attenuation_k = 0;
    lights.push_back(light2);
    Point_Light light3;
    light3.position[0] = 0.5;
    light3.position[1] = -0.5;
    light3.position[2] = 0.85;
    light3.position[3] = 1;
    light3.color[0] = 1;
    light3.color[1] = 1;
    light3.color[2] = 1;
    light3.attenuation_k = 0;
    lights.push_back(light3);
}
void create_cubes()
{
    Object cube1;
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
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal1);
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal1);
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal1);
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal1);
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal1);
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal1);
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal2);
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal2);
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal2);
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal2);
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal2);
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal2);
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal5);
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal5);
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal5);
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal5);
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal5);
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal5);
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal6);
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal6);
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal6);
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal6);
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal6);
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal6);
    cube1.vertex_buffer.push_back(point4);
    cube1.normal_buffer.push_back(normal3);
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal3);
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal3);
    cube1.vertex_buffer.push_back(point7);
    cube1.normal_buffer.push_back(normal3);
    cube1.vertex_buffer.push_back(point8);
    cube1.normal_buffer.push_back(normal3);
    cube1.vertex_buffer.push_back(point3);
    cube1.normal_buffer.push_back(normal3);
    cube1.vertex_buffer.push_back(point1);
    cube1.normal_buffer.push_back(normal4);
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal4);
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal4);
    cube1.vertex_buffer.push_back(point2);
    cube1.normal_buffer.push_back(normal4);
    cube1.vertex_buffer.push_back(point5);
    cube1.normal_buffer.push_back(normal4);
    cube1.vertex_buffer.push_back(point6);
    cube1.normal_buffer.push_back(normal4);
    Object cube2 = cube1;
    Transforms transforms1;
    transforms1.translation[0] = 0;
    transforms1.translation[1] = 0;
    transforms1.translation[2] = 0;
    transforms1.rotation[0] = 1;
    transforms1.rotation[1] = 1;
    transforms1.rotation[2] = 0;
    transforms1.rotation_angle = 0;
    transforms1.scaling[0] = 0.5;
    transforms1.scaling[1] = 0.5;
    transforms1.scaling[2] = 0.5;
    cube1.transform_sets.push_back(transforms1);
    Transforms transforms2;
    transforms2.translation[0] = 0;
    transforms2.translation[1] = 0;
    transforms2.translation[2] = 0;
    transforms2.rotation[0] = 0;
    transforms2.rotation[1] = 1;
    transforms2.rotation[2] = 0;
    transforms2.rotation_angle = 0;
    transforms2.scaling[0] = 1;
    transforms2.scaling[1] = 1;
    transforms2.scaling[2] = 1;
    Transforms transforms3;
    transforms3.translation[0] = 0;
    transforms3.translation[1] = 0;
    transforms3.translation[2] = 0;
    transforms3.rotation[0] = 1;
    transforms3.rotation[1] = 0;
    transforms3.rotation[2] = 0;
    transforms3.rotation_angle = 0;
    transforms3.scaling[0] = 1;
    transforms3.scaling[1] = 1;
    transforms3.scaling[2] = 1;
    cube2.transform_sets.push_back(transforms2);
    cube2.transform_sets.push_back(transforms3);
    objects.push_back(cube1);
    objects.push_back(cube2);
}
int main(int argc, char* argv[])
{
    int xres = 500;
    int yres = 500;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(xres, yres);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_pressed);
    glutMotionFunc(mouse_moved);
    glutKeyboardFunc(key_pressed);
    glutIdleFunc(run_video_loop);
    glutMainLoop();
}
