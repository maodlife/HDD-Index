#include "repotreenode.h"
#include <algorithm>

using namespace std;

RepoTreeNode::RepoTreeNode() : TreeNode() {}

QJsonObject RepoTreeNode::toJsonObject() const
{

}

void RepoTreeNode::fromJsonObjectExtend(const QJsonObject &json)
{

}

void RepoTreeNode::AddChild(QString name, std::shared_ptr<TreeNode> rootPtr)
{
    // 找到this的智能指针
    shared_ptr<RepoTreeNode> shared_this = nullptr;
    if (this->parent.lock() == nullptr){
        shared_this = dynamic_pointer_cast<RepoTreeNode>(rootPtr);
    }
    else{
        shared_this = dynamic_pointer_cast<RepoTreeNode>(*find_if(this->parent.lock()->childs.begin(),
                              this->parent.lock()->childs.end(),
                              [this](auto value){
                                  return value->name == this->name;
                              }));
    }
    auto child = make_shared<RepoTreeNode>();
    child->name = name;
    child->isDir = true;
    child->parent = shared_this;
    shared_this->childs.push_back(child);
    shared_this->sortChildByName();
}

std::shared_ptr<RepoTreeNode> RepoTreeNode::CopyHierarchy(std::shared_ptr<RepoTreeNode> repoRootPtr, std::shared_ptr<TreeNode> treeNodePtr)
{
    auto copied = make_shared<RepoTreeNode>();
    copied->name = treeNodePtr->name;
    copied->isDir = treeNodePtr->isDir;
    if (copied->isDir == false){
        // 非目录，叶子结点
        copied->parent = repoRootPtr;
        repoRootPtr->childs.push_back(copied);
        return copied;
    }
    for (const auto &child : treeNodePtr->childs){
        RepoTreeNode::CopyHierarchy(copied, child);
    }
    copied->parent = repoRootPtr;
    repoRootPtr->childs.push_back(copied);
    repoRootPtr->sortChildByName();  // maybe not necessary
    return copied;
}
