#include "hdddata.h"

void HddData::SaveToFile(const QString &filePath) {
    auto rootJsonObj = this->rootPtr->toJsonObject();
    rootJsonObj["dirPath"] = this->dirPath;
    QJsonDocument doc(rootJsonObj);
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

void HddData::LoadJson(QString path) {
    auto rootPtr = TreeNode::loadTreeFromFile<HddTreeNode>(path);
    this->rootPtr = rootPtr;
    this->model = make_shared<HddTreeModel>(this->rootPtr);
    this->hasLoaded = true;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    auto docJsonObj = doc.object();
    this->dirPath = docJsonObj["dirPath"].toString();
}

void HddData::TryLoadJson(QString path) {
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}
