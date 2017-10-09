//#pragma comment(lib,"libGLEW.a")

#include <GL/glew.h>

#include "textfile.h"

//#include "sphere.h"

#include <glut/glut.h>
#include <iostream>

// GL includes
//#include "Shader.h"
#include "shader_2.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC

// math
#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include <math.h>

//texture
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SOIL.h>

#include "camera.h"


bool keys[1024];
//bool firstMouse = true;
//GLfloat lastX = 400, lastY = 300;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
GLfloat yaw    = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch  =  0.0f;


int MS_PER_CYCLE = 200000;
float frag_size =0.1;
float count_uB = 0.1;
float count_uC = 0.1;
bool vertex_ON;
bool frag_ON;
bool plain;
float  myTime;


// for sphere
const float PI = 3.14159265;
const int na=22;        // vertex grid size
const int nb=11;
const int na3=na*3;     // line in grid size
const int nn=nb*na3;    // whole grid size
GLfloat sphere_pos[nn]; // vertex
GLfloat sphere_nor[nn]; // normal
//GLfloat sphere_col[nn];   // color
GLuint  sphere_ix [na*(nb-1)*6];    // indices
GLuint sphere_vbo[4]={1,1,1,1};
GLuint sphere_vao[4]={1,1,1,1};
GLfloat sphere_uv[nn];
GLfloat sphere_tangent[nn*2];     // tangent
GLfloat sphere_Bi_tangent[nn*2];  // bitanget
unsigned int uv_VBO;
unsigned int tangent_VBO;
unsigned int bitangent_VBO;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}



unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}


void sphere(){
    
    // generate the sphere data
    GLfloat x,y,z,a,b,da,db,r=2.5;
    int ia,ib,ix,iy;
    da=2.0f*M_PI/GLfloat(na);
    db=    M_PI/GLfloat(nb-1);
    // [Generate sphere point data]
    // spherical angles a,b covering whole sphere surface
    for (ix=0.0f,b=-0.5f*M_PI,ib=0;ib<nb;ib++,b+=db)
        for (a=0.0,ia=0;ia<na;ia++,a+=da,ix+=3)
        {
            // unit sphere
            x=cos(b)*cos(a);
            y=cos(b)*sin(a);
            z=sin(b);
            sphere_pos[ix+0]=x*r;
            sphere_pos[ix+1]=y*r;
            sphere_pos[ix+2]=z*r;
            sphere_nor[ix+0]=x;
            sphere_nor[ix+1]=y;
            sphere_nor[ix+2]=z;
            
            
        }
    
    // [Generate GL_TRIANGLE indices]
    for (ix=0,iy=0.0f,ib=1.0f;ib<nb;ib++)
    {
        for (ia=1.0f;ia<na;ia++,iy++)
        {
            // first half of QUAD
            sphere_ix[ix]=iy;      ix++;
            sphere_ix[ix]=iy+1;    ix++;
            sphere_ix[ix]=iy+na;   ix++;
            // second half of QUAD
            sphere_ix[ix]=iy+na;   ix++;
            sphere_ix[ix]=iy+1;    ix++;
            sphere_ix[ix]=iy+na+1; ix++;
        }
        // first half of QUAD
        sphere_ix[ix]=iy;       ix++;
        sphere_ix[ix]=iy+1-na;  ix++;
        sphere_ix[ix]=iy+na;    ix++;
        // second half of QUAD
        sphere_ix[ix]=iy+na;    ix++;
        sphere_ix[ix]=iy-na+1;  ix++;
        sphere_ix[ix]=iy+1;     ix++;
        iy++;
    }
    
    //indices
    //------------

    glm::vec2 firstPoint;
   // GLfloat  duplicatePoints[na*2];
    int idx = 0;
   // int count = 0;
    for (int i = 0; i < nn; i = i+ 3) {

        GLfloat v[3] = {11,11,11};
        glm::vec3 currentP;
        currentP.x =sphere_nor[i+0];
        currentP.y =sphere_nor[i+1];
        currentP.z =sphere_nor[i+2];
        glm::normalize(currentP);
        
        v[0] = currentP.x;
        v[1] = currentP.y;
        v[2] = currentP.z;
        
        
        glm::vec2 textureCoordinates;
        textureCoordinates.x =  (glm::atan(v[0], v[2]) / PI + 1.0)*0.5f;

        
        if (textureCoordinates.x < 0.0f) {
            textureCoordinates.x += 1.0f;
        }
        
        textureCoordinates.y = asin(v[1])/PI * 1.3f + 0.5f;

        sphere_uv[idx+0] = textureCoordinates.x;
        sphere_uv[idx+1] = textureCoordinates.y;
        idx = idx + 2;
    }

   glm::vec3 edge1[2*nb*na];
   glm::vec3 edge2[2*nb*na];
    
   glm::vec2 deltaUV1[2*nb*na];
   glm::vec2 deltaUV2[2*nb*na];
    
    // triangle 1
    // caculate edge1 and edge2. Each quard has two triangles, so two edge1 and two edge2
    // convert position matrix to glm:vec3 format

    glm::vec3 position[5000];
    glm::vec2 uv[5000];
    int currentIdx =0;
    for(int i = 0; i< nn; i = i+3){
        position[currentIdx].x = sphere_nor[i+0];
        position[currentIdx].y = sphere_nor[i+1];
        position[currentIdx].z = sphere_nor[i+2];
        currentIdx ++;
    }
    int PositionSize = currentIdx;
    
    
    currentIdx =0;
    // convert uv matrix to glm::vec2 format
    for(int i = 0; i< 2*nb*na; i = i+2){
        uv[currentIdx].x = sphere_uv[i+0];
        uv[currentIdx].y = sphere_uv[i+1];
        currentIdx ++;
    }
    currentIdx =0;
    int c = 9;
    for(int i =0; i< PositionSize ; i = i+4){
      
        edge1[currentIdx]  = position[i+1]- position[i];
        edge2[currentIdx]  = position[i+2]- position[i];
        currentIdx++;
    }

    currentIdx =0;
   
    c = 9;
    for(int i =0; i< PositionSize ; i = i+4){
        
        deltaUV1[currentIdx] = uv[i+1] - uv[i];
        deltaUV2[currentIdx] = uv[i+2] - uv[i];
        currentIdx++;
    }
    
    int deltaSize = currentIdx;
    currentIdx =0;

    for(int i =0; i< deltaSize ; i = i+1){
        
        GLfloat f = 1.0f / (deltaUV1[i].x * deltaUV2[i].y - deltaUV2[i].x * deltaUV1[i].y);
        glm::vec3 tempTanget;
        tempTanget.x = f * (deltaUV2[i].y * edge1[i].x - deltaUV1[i].y * edge2[i].x);
        tempTanget.y = f * (deltaUV2[i].y * edge1[i].y - deltaUV1[i].y * edge2[i].y);
        tempTanget.z = f * (deltaUV2[i].y * edge1[i].z - deltaUV1[i].y * edge2[i].z);
       
        
        // same tangent for all of three vertices of the triangle.
        sphere_tangent[currentIdx+0] = glm::normalize(tempTanget).x;
        sphere_tangent[currentIdx+1] = glm::normalize(tempTanget).y;
        sphere_tangent[currentIdx+2] = glm::normalize(tempTanget).z;
        
        // bitagent
        glm::vec3 tempbitangent1;
        tempbitangent1.x = f * (-deltaUV2[i].x * edge1[i].x + deltaUV1[i].x * edge2[i].x);
        tempbitangent1.y = f * (-deltaUV2[i].x * edge1[i].y + deltaUV1[i].x * edge2[i].y);
        tempbitangent1.z = f * (-deltaUV2[i].x * edge1[i].z + deltaUV1[i].x * edge2[i].z);
       
        sphere_Bi_tangent[currentIdx+0] = glm::normalize(tempbitangent1).x;
        sphere_Bi_tangent[currentIdx+1] = glm::normalize(tempbitangent1).y;
        sphere_Bi_tangent[currentIdx+2] = glm::normalize(tempbitangent1).z;

         currentIdx = currentIdx + 3;
        
    }

    //---------------------------------
    //--------- triangle 2  ---------
    
    
    glm::vec3 edge3[2*nb*na];
    glm::vec3 edge4[2*nb*na];
    
    glm::vec2 deltaUV3[2*nb*na];
    glm::vec2 deltaUV4[2*nb*na];
    
    
    int Idx =0;
    c = 9;
    for(int i =0; i< PositionSize ; i = i + 4){
        
        edge3[Idx]  = position[i+2]- position[i];
        edge4[Idx]  = position[i+3]- position[i];
        Idx++;
    }
    
    Idx =0;
    c = 9;
    for(int i =0; i< PositionSize ; i = i + 4){
        
        deltaUV3[Idx]  = uv[i+2]- uv[i];
        deltaUV4[Idx]  = uv[i+3]- uv[i];
        
        Idx++;
        
    }
    printf("delta size:%d \n ", deltaSize);
    for(int i =0; i< deltaSize ; i = i+1){
        
        GLfloat f = 1.0f / (deltaUV3[i].x * deltaUV4[i].y - deltaUV4[i].x * deltaUV3[i].y);
        glm::vec3 tempTanget;
        tempTanget.x = f * (deltaUV4[i].y * edge3[i].x - deltaUV3[i].y * edge4[i].x);
        tempTanget.y = f * (deltaUV4[i].y * edge3[i].y - deltaUV3[i].y * edge4[i].y);
        tempTanget.z = f * (deltaUV4[i].y * edge3[i].z - deltaUV3[i].y * edge4[i].z);
        
       
        // same tangent for all of three vertices of the triangle.
        sphere_tangent[currentIdx+0] = glm::normalize(tempTanget).x;
        sphere_tangent[currentIdx+1] = glm::normalize(tempTanget).y;
        sphere_tangent[currentIdx+2] = glm::normalize(tempTanget).z;
        
        // bitangent
        glm::vec3 tempbitangent1;
        tempbitangent1.x = f * (-deltaUV4[i].x * edge3[i].x + deltaUV3[i].x * edge4[i].x);
        tempbitangent1.y = f * (-deltaUV4[i].x * edge3[i].y + deltaUV3[i].x * edge4[i].y);
        tempbitangent1.z = f * (-deltaUV4[i].x * edge3[i].z + deltaUV3[i].x * edge4[i].z);
        
        
        sphere_Bi_tangent[currentIdx+0] = glm::normalize(tempbitangent1).x;
        sphere_Bi_tangent[currentIdx+1] = glm::normalize(tempbitangent1).y;
        sphere_Bi_tangent[currentIdx+2] = glm::normalize(tempbitangent1).z;

        currentIdx = currentIdx + 3;
        
    }
    //-------------
    // [VAO/VBO stuff]
    GLuint i;
    glGenVertexArrays(4,sphere_vao);
    glGenBuffers(4,sphere_vbo);
    glBindVertexArray(sphere_vao[0]);
    
    i=0; // vertex
    glBindBuffer(GL_ARRAY_BUFFER,sphere_vbo[i]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(sphere_pos),sphere_pos,GL_STATIC_DRAW);
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,0,0);
    i=1; // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,sphere_vbo[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(sphere_ix),sphere_ix,GL_STATIC_DRAW);
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i,4,GL_UNSIGNED_INT,GL_FALSE,0,0);
    i=2; // normal
    glBindBuffer(GL_ARRAY_BUFFER,sphere_vbo[i]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(sphere_nor),sphere_nor,GL_STATIC_DRAW);
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,0,0);
    
    i = 3; // uv coordinates
    //unsigned int uv_VBO;
    glGenBuffers(1, &uv_VBO);
    glBindBuffer(GL_ARRAY_BUFFER,uv_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec2) * nn, &sphere_uv[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i,2,GL_FLOAT,GL_FALSE,0,0);
    
    i = 4; // tangent
    //unsigned int tangent_VBO;
    glGenBuffers(1, &tangent_VBO);
    glBindBuffer(GL_ARRAY_BUFFER,tangent_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3) * nn, &sphere_tangent[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,0,0);

    
    i = 5; // bitangent
   // unsigned int bitangent_VBO;
    glGenBuffers(1, &bitangent_VBO);
    glBindBuffer(GL_ARRAY_BUFFER,bitangent_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3) * nn, &sphere_Bi_tangent[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,0,0);
    
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    
    
    
}



int main(int argc, char **argv)
{
    // glutInit(&argc, argv);
    // Init GLFW
    glfwInit();

  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    
    glewInit();

    
    Shader shader("basic.vert", "basic.frag");
    //----------------------------------------------------
    unsigned int diffuseMap = loadTexture("/Users/Chiaoysbaby/desktop/test_bump/version_3/worldtex.bmp");
    unsigned int normalMap  = loadTexture("/Users/Chiaoysbaby/desktop/test_bump/version_3/normal.bmp");
    
    
    glUniform1i(glGetUniformLocation(shader.ID, "diffuseMap"), 0);
     glUniform1i(glGetUniformLocation(shader.ID, "normalMap"), 0);
    //----------------------------------------------------
   // glm::vec3 lightPos(0.2f, 1.0f, 0.3f);
     glm::vec3 lightPos(2.0f, 1.0f, 0.5f);
    //    glm::vec3 lightPos(-2.0f, 1.0f, 0.5f);
    //----------------------
    while (!glfwWindowShouldClose(window))
    {
        
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);        

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        shader.use();
        
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glm::mat4 model;
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        
        
        sphere();
        glBindVertexArray(sphere_vao[0]);
        glDrawElements(GL_TRIANGLES,na*(nb-1)*6,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
        
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        shader.setMat4("model", model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(4,sphere_vao);
    glDeleteBuffers(4,sphere_vbo);
    glDeleteBuffers(1,&uv_VBO);
    glDeleteBuffers(1,&tangent_VBO);

    glfwTerminate();
    return 0;
}

