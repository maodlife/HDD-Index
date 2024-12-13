#include "hdddata.h"

void HddData::SaveToFile(const QString &filePath) {
    auto rootJsonObj = this->rootPtr->toJsonObject();
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
}

void HddData::TryLoadJson(QString path) {
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}
