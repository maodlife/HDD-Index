#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "treenode.h"
#include <QAbstractItemModel>
#include <stack>

// 用于给QTreeView显示TreeNode

class TreeModel : public QAbstractItemModel {
private:
    std::shared_ptr<TreeNode> _rootPtr;

public:
    TreeModel(std::shared_ptr<TreeNode> ptr);

    ~TreeModel();

    // 返回某个父项的子项数量
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // 返回列数，通常为1
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // 返回某个索引的父项
    QModelIndex parent(const QModelIndex &index) const override;

    // 返回给定父项下的子项的索引
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    // 返回给定索引处的数据
    QVariant data(const QModelIndex &index, int role) const override;

    // 在index下创建子目录
    void MakeDir(const QModelIndex &index, QString name);

    // 返回给定索引的智能指针
    std::shared_ptr<TreeNode> GetSharedPtr(const QModelIndex &index);

    // 查找treenode指针在model中的index
    QModelIndex findIndexByTreeNode(std::shared_ptr<TreeNode> ptr);
private:
    QModelIndex findIndexByTreeNode(std::stack<QString> stack);
};

#endif // TREEMODEL_H
