#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <map>
#include <sstream>
#include <unordered_map>
#include <nlohmann/json.hpp> // JSON library for C++

using json = nlohmann::json;

using namespace std;

class Table {
public:
    vector<string> attrName;
    vector<vector<string>> data;
    vector<vector<string>> attrValueList;

    void extractAttrValue() {
        attrValueList.resize(attrName.size());
        for (int j = 0; j < attrName.size(); j++) {
            map<string, int> value;
            for (int i = 0; i < data.size(); i++) {
                value[data[i][j]] = 1;
            }
            for (auto iter = value.begin(); iter != value.end(); iter++) {
                attrValueList[j].push_back(iter->first);
            }
        }
    }
};

class Node {
public:
    int criteriaAttrIndex;
    string attrValue;
    int treeIndex;
    bool isLeaf;
    string label;
    vector<int> children;

    Node() {
        isLeaf = false;
    }
};

class DecisionTree {
public:
    Table initialTable;
    vector<Node> tree;

    DecisionTree(Table table) {
        initialTable = table;
        initialTable.extractAttrValue();

        Node root;
        root.treeIndex = 0;
        tree.push_back(root);
        run(initialTable, 0);
        printTree(0, "");
    }

    string guess(vector<string> row) {
        string label = "";
        int leafNode = dfs(row, 0);
        if (leafNode == -1) {
            return "dfs failed";
        }
        label = tree[leafNode].label;
        return label;
    }

    int dfs(vector<string>& row, int here) {
        if (tree[here].isLeaf) {
            return here;
        }

        int criteriaAttrIndex = tree[here].criteriaAttrIndex;
        for (int i = 0; i < tree[here].children.size(); i++) {
            int next = tree[here].children[i];
            if (row[criteriaAttrIndex] == tree[next].attrValue) {
                return dfs(row, next);
            }
        }
        return -1;
    }

    void run(Table table, int nodeIndex) {
        if (isLeafNode(table)) {
            tree[nodeIndex].isLeaf = true;
            tree[nodeIndex].label = table.data.back().back();
            return;
        }

        int selectedAttrIndex = getSelectedAttribute(table);
        map<string, vector<int>> attrValueMap;
        for (int i = 0; i < table.data.size(); i++) {
            attrValueMap[table.data[i][selectedAttrIndex]].push_back(i);
        }

        tree[nodeIndex].criteriaAttrIndex = selectedAttrIndex;
        pair<string, int> majority = getMajorityLabel(table);
        if ((double)majority.second / table.data.size() > 0.8) {
            tree[nodeIndex].isLeaf = true;
            tree[nodeIndex].label = majority.first;
            return;
        }

        for (int i = 0; i < initialTable.attrValueList[selectedAttrIndex].size(); i++) {
            string attrValue = initialTable.attrValueList[selectedAttrIndex][i];
            Table nextTable;
            vector<int> candi = attrValueMap[attrValue];
            for (int i = 0; i < candi.size(); i++) {
                nextTable.data.push_back(table.data[candi[i]]);
            }

            Node nextNode;
            nextNode.attrValue = attrValue;
            nextNode.treeIndex = (int)tree.size();
            tree[nodeIndex].children.push_back(nextNode.treeIndex);
            tree.push_back(nextNode);

            if (nextTable.data.size() == 0) {
                nextNode.isLeaf = true;
                nextNode.label = getMajorityLabel(table).first;
                tree[nextNode.treeIndex] = nextNode;
            } else {
                run(nextTable, nextNode.treeIndex);
            }
        }
    }

    pair<string, int> getMajorityLabel(Table table) {
        string majorLabel = "";
        int majorCount = 0;
        map<string, int> labelCount;
        for (int i = 0; i < table.data.size(); i++) {
            labelCount[table.data[i].back()]++;
            if (labelCount[table.data[i].back()] > majorCount) {
                majorCount = labelCount[table.data[i].back()];
                majorLabel = table.data[i].back();
            }
        }
        return {majorLabel, majorCount};
    }

    bool isLeafNode(Table table) {
        for (int i = 1; i < table.data.size(); i++) {
            if (table.data[0].back() != table.data[i].back()) {
                return false;
            }
        }
        return true;
    }

    int getSelectedAttribute(Table table) {
        int maxAttrIndex = -1;
        double maxAttrValue = 0.0;
        for (int i = 0; i < initialTable.attrName.size() - 1; i++) {
            if (maxAttrValue < getGainRatio(table, i)) {
                maxAttrValue = getGainRatio(table, i);
                maxAttrIndex = i;
            }
        }
        return maxAttrIndex;
    }

    double getGainRatio(Table table, int attrIndex) {
        return getGain(table, attrIndex) / getSplitInfoAttrD(table, attrIndex);
    }

    double getInfoD(Table table) {
        double ret = 0.0;
        int itemCount = (int)table.data.size();
        map<string, int> labelCount;
        for (int i = 0; i < table.data.size(); i++) {
            labelCount[table.data[i].back()]++;
        }
        for (auto iter = labelCount.begin(); iter != labelCount.end(); iter++) {
            double p = (double)iter->second / itemCount;
            ret += -1.0 * p * log(p) / log(2);
        }
        return ret;
    }

    double getInfoAttrD(Table table, int attrIndex) {
        double ret = 0.0;
        int itemCount = (int)table.data.size();
        map<string, vector<int>> attrValueMap;
        for (int i = 0; i < table.data.size(); i++) {
            attrValueMap[table.data[i][attrIndex]].push_back(i);
        }
        for (auto iter = attrValueMap.begin(); iter != attrValueMap.end(); iter++) {
            Table nextTable;
            for (int i = 0; i < iter->second.size(); i++) {
                nextTable.data.push_back(table.data[iter->second[i]]);
            }
            int nextItemCount = (int)nextTable.data.size();
            ret += (double)nextItemCount / itemCount * getInfoD(nextTable);
        }
        return ret;
    }

    double getGain(Table table, int attrIndex) {
        return getInfoD(table) - getInfoAttrD(table, attrIndex);
    }

    double getSplitInfoAttrD(Table table, int attrIndex) {
        double ret = 0.0;
        int itemCount = (int)table.data.size();
        map<string, vector<int>> attrValueMap;
        for (int i = 0; i < table.data.size(); i++) {
            attrValueMap[table.data[i][attrIndex]].push_back(i);
        }
        for (auto iter = attrValueMap.begin(); iter != attrValueMap.end(); iter++) {
            Table nextTable;
            for (int i = 0; i < iter->second.size(); i++) {
                nextTable.data.push_back(table.data[iter->second[i]]);
            }
            int nextItemCount = (int)nextTable.data.size();
            double d = (double)nextItemCount / itemCount;
            ret += -1.0 * d * log(d) / log(2);
        }
        return ret;
    }

    void printTree(int nodeIndex, string branch) {
        if (tree[nodeIndex].isLeaf) {
            cout << branch << "Label: " << tree[nodeIndex].label << "\n";
        }
        for (int i = 0; i < tree[nodeIndex].children.size(); i++) {
            int childIndex = tree[nodeIndex].children[i];
            string attributeName = initialTable.attrName[tree[nodeIndex].criteriaAttrIndex];
            string attributeValue = tree[childIndex].attrValue;
            printTree(childIndex, branch + attributeName + " = " + attributeValue + ", ");
        }
    }

    // Method to serialize the decision tree to JSON
    json serializeTreeToJson() {
        json treeJson;
        for (const Node& node : tree) {
            json nodeJson;
            nodeJson["criteriaAttrIndex"] = node.criteriaAttrIndex;
            nodeJson["attrValue"] = node.attrValue;
            nodeJson["treeIndex"] = node.treeIndex;
            nodeJson["isLeaf"] = node.isLeaf;
            nodeJson["label"] = node.label;
            nodeJson["children"] = node.children;
            treeJson.push_back(nodeJson);
        }
        return treeJson;
    }
};

class InputReader {
private:
    ifstream fin;
    Table table;
public:
    InputReader(string filename) {
        fin.open(filename);
        if (!fin) {
            cout << filename << " file could not be opened\n";
            exit(1);
        }
        string line;
        bool isAttrName = true;
        while (getline(fin, line)) {
            vector<string> row;
            stringstream ss(line);
            string item;
            while (getline(ss, item, ',')) {
                row.push_back(item);
            }
            if (isAttrName) {
                table.attrName = row;
                isAttrName = false;
            } else {
                table.data.push_back(row);
            }
        }
        fin.close();
    }

    Table getTable() {
        return table;
    }
};

int main(int argc, char* argv[]) {
    InputReader inputReader("Crop_recommendation.csv");
    Table table = inputReader.getTable();

    DecisionTree decisionTree(table);

    // Save the decision tree to a JSON file
    ofstream fout("crop_prediction.json");
    fout << decisionTree.serializeTreeToJson().dump(4);  // Pretty-print with indentation of 4 spaces
    fout.close();

    return 0;
}
