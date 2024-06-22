#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Define Node structure similar to the one used in DecisionTree class
struct Node {
    int criteriaAttrIndex;
    string attrValue;
    int treeIndex;
    bool isLeaf;
    string label;
    vector<int> children;
};

// Function to recursively build the decision tree nodes from JSON
Node buildTree(const json& nodeJson) {
    Node node;
    node.criteriaAttrIndex = nodeJson["criteriaAttrIndex"];
    node.attrValue = nodeJson["attrValue"];
    node.treeIndex = nodeJson["treeIndex"];
    node.isLeaf = nodeJson["isLeaf"];
    node.label = nodeJson["label"];
    node.children = nodeJson["children"].get<vector<int>>();
    return node;
}

// Function to predict based on the loaded decision tree
string predict(const json& treeJson, const vector<string>& instance) {
    int currentNodeIndex = 0; // Start from root node

    while (true) {
        Node currentNode = buildTree(treeJson[currentNodeIndex]);

        if (currentNode.isLeaf) {
            return currentNode.label; // Return the predicted label
        }

        int criteriaAttrIndex = currentNode.criteriaAttrIndex;
        string instanceAttrValue = instance[criteriaAttrIndex];

        bool foundChild = false;
        for (int childIndex : currentNode.children) {
            Node childNode = buildTree(treeJson[childIndex]);
            if (childNode.attrValue == instanceAttrValue) {
                currentNodeIndex = childIndex;
                foundChild = true;
                break;
            }
        }

        if (!foundChild) {
            // If no matching child found, return an error or handle appropriately
            return "Prediction failed";
        }
    }
}

int main() {
    // Read serialized decision tree from JSON file
    ifstream fin_tree("crop_prediction.json");
    json treeJson;
    fin_tree >> treeJson;
    fin_tree.close();

    // Read instance data from JSON file
    ifstream fin_instance("instance.json");
    json instanceJson;
    fin_instance >> instanceJson;
    fin_instance.close();

    // Extract instance data into a vector of strings
    vector<string> instance;
    instance.push_back(instanceJson["N"]);
    instance.push_back(instanceJson["P"]);
    instance.push_back(instanceJson["K"]);
    instance.push_back(instanceJson["temperature"]);
    instance.push_back(instanceJson["humidity"]);
    instance.push_back(instanceJson["ph_level"]);
    instance.push_back(instanceJson["rainfall"]);

    // Perform prediction using the loaded decision tree
    string prediction = predict(treeJson, instance);

    // Output the predicted crop label
    cout << prediction << endl;

    return 0;
}
