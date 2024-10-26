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
