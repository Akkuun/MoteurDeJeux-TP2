#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include "SceneNode.h"

class SceneManager {
public:
    std::shared_ptr<SceneNode> root;

    SceneManager() : root(std::make_shared<SceneNode>()) {}

    /*
    void update() {
        for (auto &child : root->children) {
            if (child) {
                child->update();
            } else {
                std::cerr << "Erreur: Pointeur enfant null dans SceneNode." << std::endl;
            }
        }
    }
     */

    void draw() {
      //  std::cout << "Drawing scene..." << std::endl;
        root->draw(root->transform.getMatrix());
    }
};

#endif // SCENEMANAGER_H