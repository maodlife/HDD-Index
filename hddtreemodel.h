#ifndef HDDTREEMODEL_H
#define HDDTREEMODEL_H

#include "treemodel.h"
#include "hddtreenode.h"

class HddTreeModel : public TreeModel {
private:
    std::shared_ptr<HddTreeNode> _hddRootPtr;

public:
    HddTreeModel(std::shared_ptr<HddTreeNode> ptr);

    // 返回给定索引处的数据
    QVariant data(const QModelIndex &index, int role) const override;

    // 声明Index持有Repo中的节点, 不做文件夹递归比较
    void Declare(const QModelIndex &index, QString path);

    // 放弃声明持有
    bool NoDeclare(const QModelIndex &index);
};

#endif // HDDTREEMODEL_H
