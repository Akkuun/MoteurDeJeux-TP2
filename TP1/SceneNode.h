#ifndef SCENENODE_H
#define SCENENODE_H

#include <vector>
#include <memory>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Transform.h"

class SceneNode {
public:
    Transform transform;
    std::vector<std::shared_ptr<SceneNode>> children;
    GLuint programID;
    struct Geometry {
        GLuint vertexbuffer;
        GLuint elementbuffer;
        GLuint texCoordBuffer;
        std::vector<glm::vec3> vertices;
        std::vector<unsigned short> indices;
        std::vector<glm::vec2> texCoords;
    } geometry;

    SceneNode() : programID(0) {
        glGenBuffers(1, &geometry.vertexbuffer);
        glGenBuffers(1, &geometry.elementbuffer);
        glGenBuffers(1, &geometry.texCoordBuffer);
    }

    void draw(const glm::mat4 &parentTransform) {
        if (programID == 0) {
            std::cerr << "Erreur: programID non initialisé dans SceneNode" << std::endl;
            return;
        }

        glUseProgram(programID);

        glm::mat4 globalTransform = parentTransform * transform.getMatrix();
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");

        if (MatrixID == -1) {
            std::cerr << "Erreur: uniform 'MVP' non trouvé dans le shader." << std::endl;
            return;
        }

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &globalTransform[0][0]);

        if (geometry.vertexbuffer == 0 || geometry.elementbuffer == 0) {
            std::cerr << "Erreur: Buffers OpenGL non initialisés." << std::endl;
            return;
        }

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, geometry.texCoordBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.elementbuffer);
        glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_SHORT, (void *) 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

    }

    void setShader(GLuint shaderProgram) {
        programID = shaderProgram;
    }

    void createGeometry(int resolution) {
        create_sphere_textured(resolution, resolution, geometry.vertices, geometry.indices, geometry.texCoords);

        glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, geometry.vertices.size() * sizeof(glm::vec3), &geometry.vertices[0],
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.indices.size() * sizeof(unsigned short), &geometry.indices[0],
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, geometry.texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, geometry.texCoords.size() * sizeof(glm::vec2), &geometry.texCoords[0],
                     GL_STATIC_DRAW);
    }

    void create_sphere_textured(int stacks, int slices, std::vector<glm::vec3> &vertices,
                                std::vector<unsigned short> &indices, std::vector<glm::vec2> &uvs) {
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

    void debug() {

        //print taille verticies
        std::cout << "Taille vertices : " << geometry.vertices.size() << std::endl;
        //print taille buffer
        std::cout << "Taille buffer : " << geometry.vertexbuffer << std::endl;
        //print taille indices
        std::cout << "Taille indices : " << geometry.indices.size() << std::endl;
        //print taille buffer
        std::cout << "Taille buffer : " << geometry.elementbuffer << std::endl;
        //print taille texCoords
        std::cout << "Taille texCoords : " << geometry.texCoords.size() << std::endl;
        //print taille buffer
        std::cout << "Taille buffer : " << geometry.texCoordBuffer << std::endl;

    }
};

#endif // SCENENODE_H