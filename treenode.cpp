#include "treenode.h"
#include <QDir>
#include <queue>
#include <QJsonDocument>
#include <algorithm>

using namespace std;

TreeNode::TreeNode() {}

QJsonObject TreeNode::toJsonObject() const {
    QJsonObject json;
    json["name"] = name;
    json["dirPath"] = dirPath;
    json["isDir"] = isDir;

    QJsonArray childArray;
    for (const auto &child : childs) {
        childArray.append(child->toJsonObject());
    }
    json["childs"] = childArray;

    return json;
}

void TreeNode::AddChild(QString name, std::shared_ptr<TreeNode> rootPtr)
{

}

void TreeNode::sortChildByName()
{
    sort(this->childs.begin(), this->childs.end(), [](auto &lhs, auto &rhs){
        return lhs->name < rhs->name;
    });
}

std::shared_ptr<TreeNode> TreeNode::get_shared_ptr(TreeNode *ptr) {
    if (ptr->parent.lock() == nullptr)
        return nullptr;
    auto parent = ptr->parent.lock();
    auto &childs = parent->childs;
    auto ret = find_if(childs.begin(), childs.end(),
                       [=](auto value) { return value->name == ptr->name; });
    return *ret;
}
