#include "treenode.h"
#include <QDir>
#include <queue>
#include <QJsonDocument>

using namespace std;

TreeNode::TreeNode() {}

std::shared_ptr<TreeNode> TreeNode::CreateTreeNodeByDirPath(QString path) {
    QDir baseDir(path);
    std::shared_ptr<TreeNode> rootPtr = std::make_shared<TreeNode>();
    rootPtr->dirPath = path;
    rootPtr->name = baseDir.dirName();
    rootPtr->isDir = true;
    queue<shared_ptr<TreeNode>> dirQueue;
    dirQueue.push(rootPtr);
    while (!dirQueue.empty()) {
        auto currPtr = dirQueue.front();
        qDebug() << currPtr->dirPath;
        dirQueue.pop();
        if (currPtr->isDir == false)
            continue;
        QDir dir(currPtr->dirPath);
        auto dirList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
        for (auto &item : dirList) {
            std::shared_ptr<TreeNode> dirPtr = std::make_shared<TreeNode>();
            dirPtr->dirPath = item.filePath();
            dirPtr->name = item.fileName();
            dirPtr->parent = currPtr;
            dirPtr->isDir = item.isDir();
            currPtr->childs.push_back(dirPtr);
            dirQueue.push(dirPtr);
        }
    }
    return rootPtr;
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

std::shared_ptr<TreeNode> TreeNode::fromJsonObject(const QJsonObject &json) {
    auto node = std::make_shared<TreeNode>();
    node->name = json["name"].toString();
    node->dirPath = json["dirPath"].toString();
    node->isDir = json["isDir"].toBool();

    QJsonArray childArray = json["childs"].toArray();
    for (const auto &childValue : std::as_const(childArray)) {
        auto childPtr = fromJsonObject(childValue.toObject());
        childPtr->parent = node;
        node->childs.push_back(childPtr);
    }

    return node;
}

void TreeNode::saveTreeToFile(const std::shared_ptr<TreeNode> &root, const QString &filePath)
{
    QJsonDocument doc(root->toJsonObject());
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

std::shared_ptr<TreeNode> TreeNode::loadTreeFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return TreeNode::fromJsonObject(doc.object());
}
