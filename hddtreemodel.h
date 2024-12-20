#ifndef HDDTREEMODEL_H
#define HDDTREEMODEL_H

#include "hddtreenode.h"
#include "treemodel.h"
#include <memory>

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

    // 修改声明持有的路径
    void ChangeDeclareRepoPath(std::shared_ptr<HddTreeNode> ptr,
                               QString newRepoPath);

    // 基于path递归创建节点, path开头和rootPtr不同，直接就是第一个孩子
    std::shared_ptr<HddTreeNode>
    CreatePtrByPath(std::shared_ptr<HddTreeNode> rootPtr, QString path);
};

#endif // HDDTREEMODEL_H
