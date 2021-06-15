//
// Created by Dvir on 04/06/2021.
//
#include "sources/BinaryTree.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
using namespace ariel;

int main() {
    BinaryTree<int> tree;
    tree.add_root(10);
    tree.add_left(10, 6)
            .add_left(6, 3)
            .add_right(6, 8)
            .add_left(8, 7)
            .add_right(8, 9)
            .add_right(3, 5);

    tree.add_right(10, 15)
            .add_left(15, 12)
            .add_left(12, 11)
            .add_right(12, 14)
            .add_right(15, 20)
            .add_left(20, 17)
            .add_right(17, 19)
            .add_left(19, 18);
    cout << tree;
    if (is_sorted(tree.begin(), tree.end())) {
        cout << "inorder traversal of BST is sorted" << endl;
    }
    for (int element: tree) {  // this should work like inorder
        cout << element << " ";
    }
    cout << endl;
    
    cout << endl << "in order:" << endl;
    for (auto it=tree.begin_inorder(); it!=tree.end_inorder(); ++it) {
        cout << (*it) << " " ;
    }
    cout << endl << "pre order:" << endl;
    for (auto it=tree.begin_preorder(); it!=tree.end_preorder(); ++it) {
        cout << (*it) << " " ;
    }
    cout << endl << "post order:" << endl;
    for (auto it=tree.begin_postorder(); it!=tree.end_postorder(); ++it) {
        cout << (*it) << " " ;
    }
    cout << endl;


    BinaryTree<int> t2(tree);
    tree.add_root(99);
    cout << *tree.begin_preorder() << endl;
    cout << *t2.begin_preorder() << endl;

    BinaryTree<char> cTree;
    cTree.add_root('m')
            .add_left('m', 'f')
            .add_left('f', 'd')
            .add_right('f', 'b')
            .add_right('m', 'j')
            .add_right('j', 'i')
            .add_left('j', 'h');

    cout << cTree;
    for (auto it = cTree.begin_preorder(); it != cTree.end_preorder(); ++it) {
        cout << (*it) << " ";
    }
    cout << endl;
}
