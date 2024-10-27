#ifndef REPOTREEMODEL_H
#define REPOTREEMODEL_H

#include "treemodel.h"
#include "repotreenode.h"

class RepoTreeModel : public TreeModel
{
private:
    std::shared_ptr<RepoTreeNode> _repoRootPtr;

public:
    RepoTreeModel(std::shared_ptr<RepoTreeNode> ptr);

    // 在index下创建递归的子目录并声明持有
    void CreateAndDeclare(const QModelIndex &index, QString hddLabel, std::shared_ptr<TreeNode> hddNode);
};

#endif // REPOTREEMODEL_H
