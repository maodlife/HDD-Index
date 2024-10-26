#include "hddtreenode.h"
#include <QDir>
#include <queue>

using namespace std;

HddTreeNode::HddTreeNode() : TreeNode() {}

std::shared_ptr<HddTreeNode> HddTreeNode::CreateTreeNodeByDirPath(QString path)
{
    QDir baseDir(path);
    std::shared_ptr<HddTreeNode> rootPtr = std::make_shared<HddTreeNode>();
    rootPtr->dirPath = path;
    rootPtr->name = baseDir.dirName();
    rootPtr->isDir = true;
    queue<shared_ptr<HddTreeNode>> dirQueue;
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
            std::shared_ptr<HddTreeNode> dirPtr = std::make_shared<HddTreeNode>();
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
