#ifndef REPOTREEMODEL_H
#define REPOTREEMODEL_H

#include "repotreenode.h"
#include "treemodel.h"
#include <memory>

class RepoTreeModel : public TreeModel {
private:
    std::shared_ptr<RepoTreeNode> _repoRootPtr;

public:
    RepoTreeModel(std::shared_ptr<RepoTreeNode> ptr);

    // 返回给定索引处的数据
    QVariant data(const QModelIndex &index, int role) const override;

    // 在index下创建递归的子目录并声明持有
    std::shared_ptr<TreeNode>
    CreateAndDeclare(const QModelIndex &index, QString hddLabel,
                     std::shared_ptr<TreeNode> hddNode, bool declare = true);

    // 新增声明持有
    bool AddDeclare(const QModelIndex &index, QString hddLabel,
                    std::shared_ptr<TreeNode> hddNode);

    // 删除声明持有
    bool RemoveDeclare(const QModelIndex &index, QString hddLabel);

    // 修改声明持有
    bool ChangeDeclare(const QModelIndex &index, QString hddLabel,
                       std::shared_ptr<TreeNode> hddNode);
};

#endif // REPOTREEMODEL_H
