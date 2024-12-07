#ifndef HDDDATA_H
#define HDDDATA_H

#include "hddtreemodel.h"
#include "hddtreenode.h"

struct HddData {
    std::shared_ptr<HddTreeNode> rootPtr;
    std::shared_ptr<HddTreeModel> model;
    QString labelName;
    bool hasLoaded = false;
    bool isDirty = false;
    QString dirPath; // 本地磁盘对应root的目录绝对路径

    void SaveToFile(const QString &filePath);

    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

#endif // HDDDATA_H