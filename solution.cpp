#include "solution.h"
#include <QCoreApplication>
#include <QDir>
#include <QLabel>
#include <QStandardPaths>
#include <algorithm>
#include <qjsonarray.h>
#include <qjsonobject.h>

using namespace std;

Solution::Solution() {
    InitDirPathValue();
    // load json file
    QDir jsonFileDir(ProgramDirPath + "/" + JsonFileDirName);
    auto fileList = jsonFileDir.entryInfoList(QDir::Files);
    for (const auto &file : std::as_const(fileList)) {
        if (file.fileName() == RepoJsonFileName) {
            // repository
            QString path =
                ProgramDirPath + "/" + JsonFileDirName + "/" + RepoJsonFileName;
            this->repoData.TryLoadJson(path);
        } else if (file.fileName() == HddDataLocalDirPathFileName) {
            continue;
        } else {
            HddData hddData;
            hddData.labelName = file.fileName();
            hddData.labelName.chop(4);
            this->hddDataList.push_back(hddData);
        }
    }
    // 对所有hdd data load json, 不然之后还要考虑没有load的情况心智负担太大了
    for (auto &hddData : this->hddDataList) {
        QString path = ProgramDirPath + "/" + JsonFileDirName + "/" +
                       hddData.labelName + ".txt";
        hddData.TryLoadJson(path);
    }
    // set hdd data local dir path
    QString path = ProgramDirPath + "/" + JsonFileDirName + "/" +
                   HddDataLocalDirPathFileName;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        auto docJsonObj = doc.array();
        for (auto p : docJsonObj) {
            if (p.isObject()) {
                auto pObj = p.toObject();
                auto key = pObj["key"].toString();
                auto value = pObj["value"].toString();
                auto foundHddDataIt =
                    std::find_if(hddDataList.begin(), hddDataList.end(),
                                 [key](auto &x) { return x.labelName == key; });
                if (foundHddDataIt != hddDataList.end()) {
                    auto &hddData = *foundHddDataIt;
                    hddData.dirPath = value;
                }
            }
        }
    }

    // init reposity if empty
    if (this->repoData.hasLoaded == false) {
        std::shared_ptr<RepoTreeNode> rootPtr =
            std::make_shared<RepoTreeNode>();
        rootPtr->isDir = true;
        rootPtr->name = "Repository";
        this->repoData.rootPtr = rootPtr;
        this->repoData.model = make_shared<RepoTreeModel>(this->repoData.rootPtr);
        this->repoData.hasLoaded = true;
    }
}

void Solution::SaveRepoData()
{
    QString filePath =
        ProgramDirPath + "/" + JsonFileDirName + "/" + RepoJsonFileName;
    TreeNode::saveTreeToFile(this->repoData.rootPtr, filePath);
    this->repoData.isDirty = false;
}

void Solution::SaveAllHddData() {
    bool haveDirty = false;
    for (auto &hddData : this->hddDataList) {
        if (hddData.isDirty == false)
            continue;
        haveDirty = true;
        QString filePath = ProgramDirPath + "/" + JsonFileDirName + "/" +
                           hddData.labelName + ".txt";
        hddData.SaveToFile(filePath);
        hddData.isDirty = false;
    }
    if (!haveDirty)
        return;
    QString filePath = ProgramDirPath + "/" + JsonFileDirName + "/" +
                       HddDataLocalDirPathFileName;
    QJsonArray dirPathArray;
    for (const auto &hddData : hddDataList) {
        if (hddData.dirPath.isEmpty())
            continue;
        QJsonObject jsonObj;
        jsonObj["key"] = hddData.labelName;
        jsonObj["value"] = hddData.dirPath;
        dirPathArray.append(jsonObj);
    }
    QJsonDocument doc(dirPathArray);
    QFile file(filePath);
    QDir dir;
    if (!dir.mkpath(QFileInfo(filePath).path())) { // 创建所有必要的中间路径
        qWarning("Failed to create directory path");
        return;
    }
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

bool Solution::CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr,
                               std::shared_ptr<TreeNode> rightPtr,
                               QString &errName) {
    if (leftPtr->name != rightPtr->name) {
        errName = leftPtr->name;
        return false;
    }
    for (const auto &child : leftPtr->childs) {
        auto childName = child->name;
        auto foundInRight = find_if(
            rightPtr->childs.begin(), rightPtr->childs.end(),
            [=](const auto &value) { return value->name == childName; });
        if (foundInRight == rightPtr->childs.end()) {
            errName = childName;
            return false;
        }
        auto rightChild = *foundInRight;
        if (child->isDir &&
            CheckCanDeclare(child, rightChild, errName) == false)
            return false;
    }
    return true;
}

void Solution::ExpandAndSetTreeViewNode(QTreeView *treeView,
                                        QModelIndex &index) {
    // 展开到目标节点
    treeView->expand(index.parent());
    // 选中目标节点
    treeView->setCurrentIndex(index);
    // 确保目标节点可见
    treeView->scrollTo(index);
}

void Solution::InitDirPathValue() {
    if (ProgramDirPath.isEmpty()) {
        ProgramDirPath = QStandardPaths::writableLocation(
                             QStandardPaths::DocumentsLocation) +
                         "/HDD-Index";
    }
    JsonFileDirName = "JsonFiles";
    RepoJsonFileName = "RepoTreeData.txt";
    HddDataLocalDirPathFileName = "HddDataDirPath.txt";
}
