#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <vector>
#include <algorithm>

template<typename TNode, int MaximumChildNodesCount>
  struct NodeFamily {
    const TNode* parent;
    NodeFamily<TNode, MaximumChildNodesCount>* children[MaximumChildNodesCount];

    NodeFamily() {
      for (int i = 0; i < MaximumChildNodesCount; ++i) {
        children[i] = 0;
      }

      parent = 0;
    }
  };

template<typename TNode, int MaximumChildNodesCount>
  class Tree {
    public:
      typedef NodeFamily<TNode, MaximumChildNodesCount> PreparedNodeFamily;

      TNode& getRoot() const {
        return *nodes[0];
      }

      bool hasRoot() const {
        return !nodes.empty();
      }

      TNode* getChildNode(const TNode& parentNode, int childNumber) const {
        for (std::size_t i = 0; i < nodes.size(); ++i) {
          if (nodes[i] == &parentNode) {
            return families[i].children[childNumber]->parent;
          }
        }

        return 0;
      }

      bool hasChildNode(const TNode& parentNode, int childNumber) const {
        for (std::size_t i = 0; i < nodes.size(); ++i) {
          if (nodes[i] == &parentNode) {
            return families[i].children[childNumber] != 0;
          }
        }

        return false;
      }

      bool isLeafNode(const TNode& node) const {
        PreparedNodeFamily* nodeChildren = 0;

        for (std::size_t i = 0; i < nodes.size(); ++i) {
          if (nodes[i] == &node) {
            nodeChildren = families[i];
          }
        }

        if (nodeChildren == 0) {
          return false;
        }

        for (int i = 0; i < MaximumChildNodesCount; ++i) {
          if (nodeChildren->children[i] != 0) {
            return false;
          }
        }

        return true;
      }

      void addNode(TNode* node, const TNode* parentNode, int childNumber) {
        PreparedNodeFamily* nodeFamily = new PreparedNodeFamily();
        nodeFamily->parent = node;

        if (hasRoot()) {
          PreparedNodeFamily* parentFamily = 0;

          for (std::size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i] == parentNode) {
              parentFamily = families[i];
            }
          }

          parentFamily->children[childNumber] = nodeFamily;
        }

        nodes.push_back(node);
        families.push_back(nodeFamily);
      }

    private:
      typedef std::vector<TNode*> Nodes;
      typedef std::vector<PreparedNodeFamily*> Families;

      Nodes nodes;
      Families families;

      bool hasNode(const TNode& node) const {
        for (std::size_t i = 0; i < nodes.size(); ++i) {
          if (nodes[i] == &node) {
            return true;
          }
        }

        return false;
      }

      template<typename TNodeInternal, int MaximumChildNodesCountInternal> friend class TreeWalker;
  };

template<typename TNode, int MaximumChildNodesCount>
  class TreeWalker {
    public:
      TreeWalker(const Tree<TNode, MaximumChildNodesCount>* tree):
        currentFamily(0),
        treeStructure(tree)
      {}

      TreeWalker(const TreeWalker<TNode, MaximumChildNodesCount>& rhs):
        currentFamily(rhs.currentFamily),
        treeStructure(rhs.treeStructure)
      {}

      void setCurrentNode(const TNode& node) {
        for (std::size_t i = 0; i < treeStructure->nodes.size(); ++i) {
          if (treeStructure->nodes[i] == &node) {
            currentFamily = treeStructure->families[i];
            break;
          }
        }
      }

      TNode& getCurrentNode() const {
        return const_cast<TNode&>(*currentFamily->parent);
      }

      bool isLeafNode() const {
        for (int i = 0; i < MaximumChildNodesCount; ++i) {
          if (currentFamily->children[i] != 0) {
            return false;
          }
        }

        return true;
      }

      bool moveToChildNode(int childNumber) {
        if (currentFamily->children[childNumber] == 0) {
          return false;
        }

        currentFamily = currentFamily->children[childNumber];

        return true;
      }

    private:
      const NodeFamily<TNode, MaximumChildNodesCount>* currentFamily;
      const Tree<TNode, MaximumChildNodesCount>* treeStructure;
  };

#endif