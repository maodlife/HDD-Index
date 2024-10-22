#ifndef TREENODE_H
#define TREENODE_H

#include <QAbstractItemModel>
#include <QString>
#include <memory>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>

using namespace std;

class TreeNode {
public:
    TreeNode();
    std::weak_ptr<TreeNode> parent;                // 父目录
    std::vector<std::shared_ptr<TreeNode>> childs; // 子目录/文件
    QString name;                                  // 自己的名字
    QString dirPath;                               // 绝对路径
    bool isDir;                                    // 是否是目录

    // 读文件夹路径, 并访问磁盘目录, 构造一棵树
    static std::shared_ptr<TreeNode> CreateTreeNodeByDirPath(QString path);

    // Json
    // 将当前节点转换为 QJsonObject
    QJsonObject toJsonObject() const;

    // 从 QJsonObject 中构建 TreeNode
    static std::shared_ptr<TreeNode> fromJsonObject(const QJsonObject &json);

    // 保存树结构到 JSON 文件
    static void saveTreeToFile(const std::shared_ptr<TreeNode>& root, const QString& filePath);

    // 从 JSON 文件加载树结构
    static std::shared_ptr<TreeNode> loadTreeFromFile(const QString& filePath);

private:
};

#endif // TREENODE_H
