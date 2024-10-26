#ifndef SOLUTION_H
#define SOLUTION_H

#include "treenode.h"
#include "hddtreenode.h"
#include "repotreenode.h"
#include <vector>

struct HddData {
    std::shared_ptr<HddTreeNode> rootPtr;
    QString labelName;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

struct RepoData {
    std::shared_ptr<RepoTreeNode> rootPtr;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

class Solution {
public:
    Solution();
    std::vector<HddData> hddDataList;
    RepoData repoData;

private:
};

#endif // SOLUTION_H
