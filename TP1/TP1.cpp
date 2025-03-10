// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/image_utils.h>
#include <memory>
#include "SceneNode.h"
#include "SceneManager.h"

using namespace glm;
using namespace std;

// === Variables Globales ===
GLFWwindow *window;
glm::vec3 camera_position = glm::vec3(0.413079f, 0.819036f, 3.38097f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float angle = 0.0f;
float zoom = 1.0f;
OCTET *HeightMap;
float rotationSurfacespeed = 5.0f;


char rotation_Axis = 'x';


// === Structures ===
struct Texture {
    OCTET *data;
    int w;
    int h;
};

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
    float height;
};


// === Prototypes des Fonctions ===

void processInput(GLFWwindow *window);

void
create_sphere_textured(int stacks, int slices, std::vector <glm::vec3> &vertices, std::vector<unsigned short> &indices,
                       std::vector <glm::vec2> &uvs);


// === Données ===
int resolution = 32;
vector<unsigned short> indices;
vector <vec3> indexed_vertices;
vector <vec2> texCoords;
Texture HeightMapTexture;
vector<float> vertexHeights;
// === Buffers ===

GLuint vertexbuffer;
GLuint texCoordBuffer;
GLuint elementbuffer;
GLuint vertexHeightBuffer;


// === Codes de fonctions ===
void updateTerrain() {
    indexed_vertices.clear();
    indices.clear();

    create_sphere_textured(resolution, resolution, indexed_vertices, indices, texCoords);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), &texCoords[0], GL_STATIC_DRAW);
}

void processInput(GLFWwindow *window) {
    float cameraSpeed = 2.5f * deltaTime;
    //change the state of the camera


//        cout << "ETAT LIBRE" << endl;

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

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void computeMinMaxHeight(const std::vector <glm::vec3> &vertices, float &minHeight, float &maxHeight) {
    minHeight = FLT_MAX;
    maxHeight = -FLT_MAX;
    for (const auto &v: vertices) {
        if (v.y < minHeight) minHeight = v.y;
        if (v.y > maxHeight) maxHeight = v.y;
    }
}

// Fonction qui crée une sphère texturée
void
create_sphere_textured(int stacks, int slices, std::vector <glm::vec3> &vertices, std::vector<unsigned short> &indices,
                       std::vector <glm::vec2> &uvs) {
    vertices.clear();
    indices.clear();
    uvs.clear();

    for (int i = 0; i <= stacks; ++i) {
        float V = i / (float) stacks;
        float phi = V * glm::pi<float>();

        for (int j = 0; j <= slices; ++j) {
            float U = j / (float) slices;
            float theta = U * (glm::pi<float>() * 2);

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vertices.push_back(glm::vec3(x, y, z));
            uvs.push_back(glm::vec2(U, V));
        }
    }

    for (int i = 0; i < slices * stacks + slices; ++i) {
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(i + slices);

        indices.push_back(i + slices + 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}

/*
int main(void) {
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    // Open a window and create its OpenGL context
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

    // Create and compile our GLSL program from the shaders
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);



    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    if (programID == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Get a handle for our "Model View Projection" matrices uniforms
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");




    // CREATE GEOMETRY
    create_sphere_textured(resolution, resolution, indexed_vertices, indices, texCoords);

    // creation du buffer pour les coordonnées de texture

    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);


    //chargement des sommets dans la carte graphique

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);


    glGenBuffers(1, &vertexHeightBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexHeightBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(float), &vertexHeights[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);  // Attribut 2 pour vertexHeight
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    //TEXTURES FOR THE  PLANET
    OCTET *rockImg;
    int nH_rock, nW_rock;
    lire_nb_lignes_colonnes_image_ppm("../img/rock.ppm", &nH_rock, &nW_rock);
    allocation_tableau(rockImg, OCTET, nH_rock * nW_rock * 3);
    lire_image_ppm("../img/rock.ppm", rockImg, nH_rock * nW_rock);



    GLuint textureID2;
    glGenTextures(1, &textureID2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nW_rock, nH_rock, 0, GL_RGB, GL_UNSIGNED_BYTE, rockImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(programID, "textureRock"), 1);


    // Main loop

    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use our shader
        glUseProgram(programID);
        // Compute the MVP matrix from keyboard and mouse input
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
        glm::mat4 Model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around


        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);


        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);



        // Draw the triangles !
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void *) 0);
        glDisableVertexAttribArray(0);
        glEnableVertexAttribArray(2);  // Attribut 2 pour vertexHeight
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);


        glUniform1i(glGetUniformLocation(programID, "textureRock"), 1);



        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        // Measure speed
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Process inputs
        processInput(window);
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}
 */

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    // Open a window and create its OpenGL context
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "TP1 - GLFW", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
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
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    if (programID == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Get a handle for our "Model View Projection" matrices uniforms
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Create SceneManager and root node
    SceneManager sceneManager;
    sceneManager.root->setShader(programID);
    sceneManager.root->createGeometry(resolution);

    // Create child nodes (planets)
    auto planet1 = std::make_shared<SceneNode>();
    planet1->transform.position = glm::vec3(2.0f, 0.0f, 0.0f);
    planet1->setShader(programID);
    planet1->createGeometry(resolution);

    auto planet2 = std::make_shared<SceneNode>();
    planet2->transform.position = glm::vec3(-2.0f, 0.0f, 0.0f);
    planet2->setShader(programID);
    planet2->createGeometry(resolution);
    planet2->debug();

    // Add planets to the root node
    sceneManager.root->children.push_back(planet1);
    sceneManager.root->children.push_back(planet2);

    // Main loop
    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Compute the MVP matrix from keyboard and mouse input
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
        glm::mat4 Model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 MVP = Projection * View * Model;

        // Send our transformation to the currently bound shader
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Draw the scene
        sceneManager.draw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Measure speed
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process inputs
        processInput(window);
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteProgram(programID);
    glfwTerminate();
    return 0;
}