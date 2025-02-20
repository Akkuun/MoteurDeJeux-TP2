// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/image_utils.h>

void processInput(GLFWwindow *window);

void creationTerrain(
        std::vector<glm::vec3> &vertices,
        std::vector<glm::vec2> &texCoords,
        std::vector<unsigned short> &indices,
        int i,
        OCTET *HeightMap);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position = glm::vec3(0.413079f, 0.819036f, 3.38097f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;

glm::mat3 axeMatrix = glm::mat3(0.0f);

OCTET *HeightMap;

float rotationSurfacespeed = 0.0f;

static bool cKeyPressed = false;
static bool key_pressedUp = false;
static bool key_pressedDown = false;
int ETAT_ACTIF = 0;
int ETAT_CLAMP = 0;

int ETAT_LIBRE = 1;
int ETAT_ROTATION = 2;
int ETAT_ORBITAL = 3;

char rotation_Axis = 'x';


/*******************************************************************************/

int main(void) {
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "TP1 - GLFW", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);



    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    if (programID == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }
    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");



    /****************************************/
    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices;


    //read HEIGHTMAP

    int nH2, nW2;
    lire_nb_lignes_colonnes_image_pgm("../img/Heightmap_Rocky.pgm", &nH2, &nW2);
    int nTaille2 = nH2 * nW2;
    allocation_tableau(HeightMap, OCTET, nTaille2);
    lire_image_pgm("../img/Heightmap_Rocky.pgm", HeightMap, nH2 * nW2);

    // Create and bind the texture coordinate buffer
    std::vector<glm::vec2> texCoords;
    creationTerrain(indexed_vertices, texCoords, indices, 40, HeightMap);

    // creation du buffer pour les coordonnées de texture
    GLuint texCoordBuffer;
    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);

    // creation du buffer pour les sommets
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);


    //chargement des sommets dans la carte graphique
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);


    // chargement des indices dans la carte graphique
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    // Load the texture
    OCTET *ImgIn;
    int nH, nW;
    lire_nb_lignes_colonnes_image_ppm("../img/vegetation.ppm", &nH, &nW);
    int nTaille = nH * nW;
    allocation_tableau(ImgIn, OCTET, nTaille * 3);
    lire_image_ppm("../img/vegetation.ppm", ImgIn, nH * nW);







    // création de la texture OpenGL
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nW, nH, 0, GL_RGB, GL_UNSIGNED_BYTE, ImgIn);
    glGenerateMipmap(GL_TEXTURE_2D);

    //parametres de la texture : filtrage et répétition
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    //Buffer pour la texture, on en a besoin pour la transmettre au shader, via la variable myTextureSampler qui est un uniforme
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLuint textureID = glGetUniformLocation(programID, "myTextureSampler");
    glUniform1i(textureID, 0);

// Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do {

        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        /*****************TODO***********************/
        // Model matrix : Matrice d'origine
        glm::mat4 model = glm::mat4(1.0f);
        // View matrix : Matrice de vue qui dépend de la position de la caméra et de la direction de la caméra
        glm::mat4 view = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        // rotation de la surface , 0 de base et dans ETAT rotation on change la valeur de l'angle
        if (ETAT_ACTIF == ETAT_ROTATION) {
            if (rotation_Axis == 'x') model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            if(rotation_Axis == 'y') model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        } else if (ETAT_ACTIF == ETAT_ORBITAL) {
            if (rotation_Axis == 'x') model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
            if (rotation_Axis == 'y') model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        //envoi de la matrice de transformation à la carte graphique (shader)
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        /****************************************/

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void *) 0            // array buffer offset
        );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        glDrawElements(
                GL_TRIANGLES,      // mode
                indices.size() * 3,    // count
                GL_UNSIGNED_SHORT,   // type
                (void *) 0           // element array buffer offset
        );

        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    float cameraSpeed = 2.5f * deltaTime;
    //change the state of the camera

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyPressed) {
        cout << "ETAT ACTIF : " << ETAT_ACTIF << endl;
        ETAT_ACTIF = (ETAT_ACTIF + 1) % 4;
        cKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
        cKeyPressed = false;
    }
    //close the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (ETAT_ACTIF == ETAT_CLAMP) {
        cout << "ETAT CLAMP" << endl;
        // Camera movement
        // Zoom In
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            if (camera_position.z > 2.8f) camera_position += cameraSpeed * camera_target;
        // Zoom Out
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            if (camera_position.z < 7.0f) camera_position -= cameraSpeed * camera_target;
        // Left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

            if (camera_position.x > 0.0f)
                camera_position -= glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;

        }
        // Right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

            if (camera_position.x < 1.7f)
                camera_position += glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;

        }
        //Up
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera_position += cameraSpeed * camera_up;
        //Down
        //we clamp to 0 the y value of the camera position to avoid going under the terrain
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            //make sure that the camera won't go bellow 0.5f

            if (camera_position.y > 0.5f) {
                camera_position -= cameraSpeed * camera_up;
            }
        }
    }
        //ETAT LIBRE
    else if (ETAT_ACTIF == ETAT_LIBRE) {
        cout << "ETAT LIBRE" << endl;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera_position += cameraSpeed * camera_target;
        // Zoom Out
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera_position -= cameraSpeed * camera_target;
        // Left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {


            camera_position -= glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;

        }
        // Right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {


            camera_position += glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;

        }
        //Up
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera_position += cameraSpeed * camera_up;
        //Down
        //we clamp to 0 the y value of the camera position to avoid going under the terrain
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            //make sure that the camera won't go bellow 0.5f
            camera_position -= cameraSpeed * camera_up;

        }

    } else if (ETAT_ACTIF == ETAT_ROTATION) {
        cout << "ETAT ROTATION" << endl;
        float coef = 1;
        // Define the rotation speed


        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !key_pressedUp) {
            rotationSurfacespeed += 10.0f;
            key_pressedUp = true;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
            key_pressedUp = false;
        }
        // DOWN -> DECREASE SPEED
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !key_pressedDown) {
            rotationSurfacespeed -= 10.0f;
            key_pressedDown = true;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
            key_pressedDown = false;
        }

        // Change direction based on key press
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            coef = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            coef = -1;
        }
        // Rotate the surface continuously
        angle += rotationSurfacespeed * deltaTime * coef;
    } else if (ETAT_ACTIF == ETAT_ORBITAL) {

        //camera orbitale
        // up -> AUGMENTE LA vitesse
        // down -> DIMINUE LA vitesse
        // left -> ROTATION SUR L'AXE X
        // right -> ROTATION SUR L'AXE Y

        cout << "ETAT ORBITAL" << endl;
        // Define the rotation speed



        // UP -> INCREASE SPEED
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !key_pressedUp) {
            rotationSurfacespeed += 10.0f;
            key_pressedUp = true;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
            key_pressedUp = false;
        }
        // DOWN -> DECREASE SPEED
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !key_pressedDown) {
            rotationSurfacespeed -= 10.0f;
            key_pressedDown = true;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
            key_pressedDown = false;
        }
        // LEFT -> ROTATE ON X AXIS
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            angle += rotationSurfacespeed * deltaTime;
            rotation_Axis = 'x';
        }
        // RIGHT -> ROTATE ON Y AXIS
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            angle += rotationSurfacespeed * deltaTime;
            rotation_Axis = 'y';
        }

        cout << "ROTATION AXIS : " << rotation_Axis << endl;
        cout << rotation_Axis << endl;
        // Rotate the surface continuously

    }



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//fonction qui :
// - crée un terrain de taille 1x1
// - le découpe en i x i carrés
// vertices -> tableau de sommets
// texCoords -> tableau de coordonnées de texture UV
// indices -> tableau d'indices des sommets pour les triangles
void creationTerrain(std::vector<glm::vec3> &vertices,
                     std::vector<glm::vec2> &texCoords,
                     std::vector<unsigned short> &indices,
                     int i,
                     OCTET *HeightMap) {
    float step = 1.0f / (i - 1);
    for (int y = 0; y < i; ++y) {
        for (int x = 0; x < i; ++x) {
            //get height from the heightmap.pgm
            float height = HeightMap[(y * i + x)] / 255.0f; // Scale height for smoother terrain

            // Adjust the vertex position to center the terrain at (0.5, 0.5)
            vertices.push_back(glm::vec3(x * step - 0.5f, height, y * step - 0.5f));
            texCoords.push_back(glm::vec2(x * step, y * step));
            if (x < i - 1 && y < i - 1) {
                int topLeft = y * i + x;
                int topRight = topLeft + 1;
                int bottomLeft = (y + 1) * i + x;
                int bottomRight = bottomLeft + 1;
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }
    }
}