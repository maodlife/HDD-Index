#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "treenode.h"
#include <QAbstractItemModel>

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
};

#endif // TREEMODEL_H
