#include "treenode.h"
#include <QDir>
#include <queue>
#include <QJsonDocument>

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
