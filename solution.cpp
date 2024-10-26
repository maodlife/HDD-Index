#include "solution.h"
#include <QCoreApplication>

Solution::Solution() {}

void HddData::LoadJson(QString path) {
    auto rootPtr = TreeNode::loadTreeFromFile<HddTreeNode>(path);
    this->rootPtr = rootPtr;
    this->hasLoaded = true;
}

void HddData::TryLoadJson(QString path) {
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}
