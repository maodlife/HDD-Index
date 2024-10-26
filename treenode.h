#ifndef TREENODE_H
#define TREENODE_H

#include <QAbstractItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <concepts>
#include <memory>
#include <vector>

class TreeNode {
public:
    TreeNode();
    std::weak_ptr<TreeNode> parent;                // 父目录
    std::vector<std::shared_ptr<TreeNode>> childs; // 子目录/文件
    QString name;                                  // 自己的名字
    QString dirPath;                               // 绝对路径  todo: delete
    bool isDir;                                    // 是否是目录

    // 保存树结构到 JSON 文件
    template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
    static void saveTreeToFile(const std::shared_ptr<NodeType> &root,
                               const QString &filePath);

    // 从 JSON 文件加载树结构
    template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
    static std::shared_ptr<NodeType> loadTreeFromFile(const QString &filePath);

private:
    // 将当前节点转换为 QJsonObject
    virtual QJsonObject toJsonObject() const;

    // 从 QJsonObject 中构建 TreeNode
    template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
    static std::shared_ptr<NodeType> fromJsonObject(const QJsonObject &json);

    virtual void fromJsonObjectExtend(const QJsonObject &json) = 0;
};

#endif // TREENODE_H
