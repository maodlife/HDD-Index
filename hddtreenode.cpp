#include "hddtreenode.h"
#include <QDir>
#include <memory>
#include <queue>
#include <utility>

using namespace std;

HddTreeNode::HddTreeNode() : TreeNode() {}

QJsonObject HddTreeNode::toJsonObject() const
{
    auto json = TreeNode::toJsonObject();
    QJsonObject saveDataJson;
    saveDataJson["path"] = this->saveData.path;
    json["saveData"] = saveDataJson;
    return json;
}

std::shared_ptr<HddTreeNode> HddTreeNode::CreateTreeNodeByDirPath(QString path)
{
    QDir baseDir(path);
    std::shared_ptr<HddTreeNode> rootPtr = std::make_shared<HddTreeNode>();
    auto rootPair = std::make_pair(rootPtr, path);
    rootPtr->name = baseDir.dirName();
    rootPtr->isDir = true;
    queue<decltype(rootPair)> dirQueue;
    dirQueue.push(rootPair);
    while (!dirQueue.empty()) {
        auto currPair = dirQueue.front();
        qDebug() << currPair.second;
        dirQueue.pop();
        if (currPair.first->isDir == false)
            continue;
        QDir dir(currPair.second);
        auto dirList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
        for (auto &item : dirList) {
            auto dirPtr = std::make_shared<HddTreeNode>();
            dirPtr->name = item.fileName();
            dirPtr->parent = currPair.first;
            dirPtr->isDir = item.isDir();
            currPair.first->childs.push_back(dirPtr);
            dirQueue.push(std::make_pair(dirPtr, item.filePath()));
        }
    }
    return rootPtr;
}

void HddTreeNode::fromJsonObjectExtend(const QJsonObject &json) {
    this->saveData.path = json["saveData"]["path"].toString();
}

void HddTreeNode::CompareTree(std::shared_ptr<HddTreeNode> source,
                              std::shared_ptr<HddTreeNode> target) {
                                
                              }
