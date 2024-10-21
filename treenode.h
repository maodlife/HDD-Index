#ifndef TREENODE_H
#define TREENODE_H

#include <QAbstractItemModel>
#include <QString>
#include <memory>
#include <vector>

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

    // 读字符串, 用字符串内容恢复一棵树
    // todo

    // 导出这棵树的内容到字符串
    // todo

private:
};

#endif // TREENODE_H
