#ifndef SOLUTION_H
#define SOLUTION_H

#include "treenode.h"
#include <vector>

struct HddData {
    std::shared_ptr<TreeNode> rootPtr;
    QString labelName;
    bool hasLoaded = false;
    bool isDirty = false;
};

class Solution {
public:
    Solution();
    std::vector<HddData> hddDataList;

private:
};

#endif // SOLUTION_H
