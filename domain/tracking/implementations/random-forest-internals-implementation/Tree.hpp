#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <vector>
#include <algorithm>

template<typename TNode, int MaximumChildNodesCount>
  struct NodeFamily {
    const TNode* parent;
    NodeFamily<TNode, MaximumChildNodesCount>* children[MaximumChildNodesCount];

    NodeFamily() {
      std::fill(children, children + MaximumChildNodesCount, 0);
      parent = 0;
    }
  };

template<typename TNode, int MaximumChildNodesCount>
  class Tree {
    public:
      TNode& getRoot() const {
        return const_cast<TNode&>(externalNodesStorage[0]);
      }

      typedef NodeFamily<TNode, MaximumChildNodesCount> PreparedNodeFamily;
      typedef std::vector<TNode*> Nodes;
      typedef std::vector<PreparedNodeFamily*> Families;

      Nodes nodes;
      Families families;

    private:
      std::vector<TNode> externalNodesStorage;
  };

template<typename TNode, int MaximumChildNodesCount>
  class TreeWalker {
    public:
      TreeWalker(const Tree<TNode, MaximumChildNodesCount>& tree):
        currentFamily(0),
        treeStructure(tree)
      {}

      TreeWalker(const TreeWalker<TNode, MaximumChildNodesCount>& rhs):
        currentFamily(rhs.currentFamily),
        treeStructure(rhs.treeStructure)
      {}

      void setCurrentNode(const TNode& node) {
        for (std::size_t i = 0; i < treeStructure.nodes.size(); ++i) {
          if (treeStructure.nodes[i] == &node) {
            currentFamily = treeStructure.families[i];
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
      const Tree<TNode, MaximumChildNodesCount>& treeStructure;
  };

#endif