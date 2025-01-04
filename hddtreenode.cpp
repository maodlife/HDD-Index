#include "hddtreenode.h"
#include <QDir>
#include <iterator>
#include <memory>
#include <queue>
#include <tuple>
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

HddTreeNodeCompareRet
HddTreeNode::CompareTree(std::shared_ptr<HddTreeNode> source,
                         std::shared_ptr<HddTreeNode> target) {
    std::vector<std::shared_ptr<HddTreeNode>> retRemove;
    std::vector<std::shared_ptr<HddTreeNode>> retAdd;
    for (const auto &child : source->childs) {
        auto sourceChild = dynamic_pointer_cast<HddTreeNode>(child);
        auto foundTarget = std::find_if(
            target->childs.begin(), target->childs.end(), [=](const auto &x) {
                // 这里和下面都没比较isDir，因为这个字段有bug
                return child->name == x->name;
            });
        if (foundTarget == target->childs.end()) {
            retRemove.push_back(sourceChild);
        } else {
            if (sourceChild->isDir) {
                // 同名文件夹，递归查找
                auto [a, b] = HddTreeNode::CompareTree(
                    sourceChild,
                    dynamic_pointer_cast<HddTreeNode>(*foundTarget));
                std::move(a.begin(), a.end(), std::back_inserter(retRemove));
                std::move(b.begin(), b.end(), std::back_inserter(retAdd));
            }
        }
    }
    for (const auto &child : target->childs) {
        auto targetChild = dynamic_pointer_cast<HddTreeNode>(child);
        auto foundSource =
            std::find_if(source->childs.begin(), source->childs.end(),
                         [=](const auto &x) { return child->name == x->name; });
        if (foundSource == source->childs.end()) {
            retAdd.push_back(targetChild);
        }
    }
    return std::make_tuple(retRemove, retAdd);
}
