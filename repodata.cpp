#include "repodata.h"

void RepoData::LoadJson(QString path) {
    auto rootPtr = TreeNode::loadTreeFromFile<RepoTreeNode>(path);
    this->rootPtr = rootPtr;
    this->model = make_shared<RepoTreeModel>(this->rootPtr);
    this->hasLoaded = true;
}

void RepoData::TryLoadJson(QString path) {
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}
