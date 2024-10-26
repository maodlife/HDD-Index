#include "treenode.h"
#include <QDir>
#include <queue>
#include <QJsonDocument>

using namespace std;

TreeNode::TreeNode() {}

template <typename NodeType>
requires std::derived_from<NodeType, TreeNode>
void TreeNode::saveTreeToFile(const std::shared_ptr<NodeType> &root, const QString &filePath)
{
    QJsonDocument doc(root->toJsonObject());
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

template<typename NodeType>
requires std::derived_from<NodeType, TreeNode>
std::shared_ptr<NodeType> TreeNode::loadTreeFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return TreeNode::fromJsonObject<NodeType>(doc.object());
}

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

template <typename NodeType>
requires std::derived_from<NodeType, TreeNode>
std::shared_ptr<NodeType> TreeNode::fromJsonObject(const QJsonObject &json)
{
    auto node = std::make_shared<NodeType>();
    node->name = json["name"].toString();
    node->dirPath = json["dirPath"].toString();
    node->isDir = json["isDir"].toBool();
    node->fromJsonObjectExtend(json);  // 额外数据恢复

    QJsonArray childArray = json["childs"].toArray();
    for (const auto &childValue : std::as_const(childArray)) {
        auto childPtr = fromJsonObject<NodeType>(childValue.toObject());
        childPtr->parent = node;
        node->childs.push_back(childPtr);
    }

    return node;
}
