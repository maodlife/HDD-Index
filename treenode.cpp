#include "treenode.h"
#include <QDir>
#include <queue>
#include <QJsonDocument>
#include <algorithm>
#include <stack>

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

QString TreeNode::getPath() {
    stack<QString> stack;
    stack.push(this->name);
    auto curr = this->parent.lock();
    while (curr != nullptr) {
        stack.push(curr->name);
        curr = curr->parent.lock();
    }
    QString ret;
    while (stack.empty() == false) {
        ret += stack.top();
        ret += "/";
        stack.pop();
    }
    ret.chop(1);
    return ret;
}

std::shared_ptr<TreeNode>
TreeNode::getPtrFromPath(std::shared_ptr<TreeNode> rootPtr, QString path) {
    auto split_path = path.split('/');
    if (split_path[0] != rootPtr->name) {
        return nullptr;
    }
    std::shared_ptr<TreeNode> curr = rootPtr;
    for (int i = 1; i < split_path.size(); ++i) {
        curr = *find_if(curr->childs.begin(), curr->childs.end(),
                        [=](auto &value) { return value->name == split_path[i]; });
    }
    return curr;
}

std::vector<std::shared_ptr<TreeNode>> TreeNode::findIfInTree(
    std::shared_ptr<TreeNode> rootPtr,
    std::function<bool(std::shared_ptr<TreeNode>)> predicate) {
    std::vector<std::shared_ptr<TreeNode>> ret;
    // todo
    return ret;
}
