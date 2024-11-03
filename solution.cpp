#include "solution.h"
#include <QCoreApplication>
#include <algorithm>

using namespace std;

Solution::Solution() {}

bool Solution::CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr,
                               std::shared_ptr<TreeNode> rightPtr) {
    if (leftPtr->name != rightPtr->name)
        return false;
    for (const auto &child : leftPtr->childs) {
        auto childName = child->name;
        auto foundInRight =
            find_if(rightPtr->childs.begin(), rightPtr->childs.end(),
                                    [=](const auto &value) { return value->name == childName; });
        if (foundInRight == rightPtr->childs.end())
            return false;
        auto rightChild = *foundInRight;
        if (child->isDir && CheckCanDeclare(child, rightChild) == false)
            return false;
    }
    return true;
}

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

void RepoData::LoadJson(QString path)
{
    auto rootPtr = TreeNode::loadTreeFromFile<RepoTreeNode>(path);
    this->rootPtr = rootPtr;
    this->hasLoaded = true;
}

void RepoData::TryLoadJson(QString path)
{
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}
