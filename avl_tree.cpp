#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

struct Node {
    int key;
    Node *left_ch;
    Node *right_ch;
    Node *parent;

    Node(int key_) {
        key = key_;
        left_ch = nullptr;
        right_ch = nullptr;
        parent = nullptr;
    }
};

int N = 0;
Node *root = nullptr;

void make_tree(const vector<int> &keys, const vector<int> &left,
               const vector<int> &right, Node *node, int i) {
    if (left[i] != -1) {
        node->left_ch = new Node(keys[left[i]]);
        node->left_ch->parent = node;
        make_tree(keys, left, right, node->left_ch, left[i]);
    }
    if (right[i] != -1) {
        node->right_ch = new Node(keys[right[i]]);
        node->right_ch->parent = node;
        make_tree(keys, left, right, node->right_ch, right[i]);
    }
}

void match_numbers(Node *node, map<Node *, int> &new_numbers, int &cur_number) {
    new_numbers[node] = cur_number++;
    if (node->left_ch != nullptr)
        match_numbers(node->left_ch, new_numbers, cur_number);
    if (node->right_ch != nullptr)
        match_numbers(node->right_ch, new_numbers, cur_number);
}

void print_tree(map<Node *, int> &new_numbers, Node *node) {
    int l = (node->left_ch == nullptr) ? 0 : new_numbers[node->left_ch];
    int r = (node->right_ch == nullptr) ? 0 : new_numbers[node->right_ch];
    cout << node->key << ' ' << l << ' ' << r << endl;
    if (l != 0)
        print_tree(new_numbers, node->left_ch);
    if (r != 0)
        print_tree(new_numbers, node->right_ch);
}

void delete_tree(Node *node) {
    if (node == nullptr)
        return;
    node->parent = nullptr;
    if (node->left_ch != nullptr)
        delete_tree(node->left_ch);
    if (node->right_ch != nullptr)
        delete_tree(node->right_ch);
    node->left_ch = nullptr;
    node->right_ch = nullptr;
    delete node;
}

int height_tree(Node *node) {
    int l_height = (node->left_ch == nullptr) ? 0 : height_tree(node->left_ch);
    int r_height = (node->right_ch == nullptr) ? 0 : height_tree(node->right_ch);
    return 1 + max(l_height, r_height);
}

int get_balance(Node *node) {
    if (node == nullptr)
        return 0;
    int l, r;
    l = (node->left_ch == nullptr) ? 0 : height_tree(node->left_ch);
    r = (node->right_ch == nullptr) ? 0 : height_tree(node->right_ch);
    return r - l;
}

void small_left_rotate(Node *node) {
    Node *A = node, *B = A->right_ch;
    Node *y = B->left_ch;
    Node *P = A->parent;

    B->left_ch = A;
    A->parent = B;
    A->right_ch = y;
    if (y != nullptr)
        y->parent = A;

    B->parent = P;
    if (P != nullptr) {
        if (P->left_ch == A)
            P->left_ch = B;
        else
            P->right_ch = B;
    }

    if (node == root)
        root = B;
}

void small_right_rotate(Node *node) {
    Node *A = node, *B = A->left_ch;
    Node *y = B->right_ch;
    Node *P = A->parent;

    B->right_ch = A;
    A->parent = B;
    A->left_ch = y;
    if (y != nullptr)
        y->parent = A;

    B->parent = P;
    if (P != nullptr) {
        if (P->left_ch == A)
            P->left_ch = B;
        else
            P->right_ch = B;
    }

    if (node == root)
        root = B;
}

void big_left_rotate(Node *node) {
    Node *A = node, *B = A->right_ch, *C = B->left_ch;
    Node *y = C->left_ch, *z = C->right_ch;
    Node *P = A->parent;

    C->left_ch = A;
    A->parent = C;
    C->right_ch = B;
    B->parent = C;
    A->right_ch = y;
    if (y != nullptr)
        y->parent = A;
    B->left_ch = z;
    if (z != nullptr)
        z->parent = B;

    C->parent = P;
    if (P != nullptr) {
        if (P->left_ch == A)
            P->left_ch = C;
        else
            P->right_ch = C;
    }

    if (node == root)
        root = C;
}

void big_right_rotate(Node *node) {
    Node *A = node, *B = A->left_ch, *C = B->right_ch;
    Node *y = C->left_ch, *z = C->right_ch;
    Node *P = A->parent;

    C->left_ch = B;
    B->parent = C;
    C->right_ch = A;
    A->parent = C;
    A->left_ch = z;
    if (z != nullptr)
        z->parent = A;
    B->right_ch = y;
    if (y != nullptr)
        y->parent = B;

    C->parent = P;
    if (P != nullptr) {
        if (P->left_ch == A)
            P->left_ch = C;
        else
            P->right_ch = C;
    }

    if (node == root)
        root = C;
}

Node *find_parent(Node *node, int target) {
    if (target < node->key) {
        if (node->left_ch != nullptr)
            return find_parent(node->left_ch, target);
        else
            return node;
    } else if (target > node->key) {
        if (node->right_ch != nullptr)
            return find_parent(node->right_ch, target);
        else
            return node;
    } else
        return nullptr;
}

void go_to_root(Node *node) {
    if (node == nullptr)
        return;
    int cur_balance = get_balance(node);
    if (cur_balance > 1) {
        if (get_balance(node->right_ch) == -1)
            big_left_rotate(node);
        else
            small_left_rotate(node);
    } else if (cur_balance < -1) {
        if (get_balance(node->left_ch) == 1)
            big_right_rotate(node);
        else
            small_right_rotate(node);
    }
    go_to_root(node->parent);
}

void insert_tree(int target) {
    if (N == 0) {
        ++N;
        root = new Node(target);
    }
    Node *parent = find_parent(root, target);
    if (parent == nullptr)
        return;
    Node *new_node = new Node(target);
    new_node->parent = parent;
    if (target < parent->key)
        parent->left_ch = new_node;
    else
        parent->right_ch = new_node;
    ++N;
    go_to_root(parent->parent);
}

Node *find_tree(Node *node, int target) {
    if (node == nullptr)
        return nullptr;
    if (node->key == target)
        return node;
    else if (target < node->key) {
        if (node->left_ch != nullptr)
            return find_tree(node->left_ch, target);
        else
            return nullptr;
    } else {
        if (node->right_ch != nullptr)
            return find_tree(node->right_ch, target);
        else
            return nullptr;
    }
}

void erase_tree(int key) {
    Node *target = find_tree(root, key);
    if (target == nullptr)
        return;
    if (N == 1 && target == root) {
        delete root;
        root = nullptr;
        N = 0;
        return;
    }
    if (target->left_ch == nullptr && target->right_ch == nullptr) {
        Node *parent = target->parent;
        target->parent = nullptr;
        if (parent != nullptr) {
            if (parent->left_ch == target)
                parent->left_ch = nullptr;
            else
                parent->right_ch = nullptr;
        }
        delete target;
        go_to_root(parent);
    } else if (target->left_ch == nullptr) {
        target->key = target->right_ch->key;
        Node *parent = target;
        target = target->right_ch;
        parent->right_ch = nullptr;
        target->parent = nullptr;
        delete target;
        go_to_root(parent);
    } else {
        Node *R = target->left_ch;
        while (R->right_ch != nullptr)
            R = R->right_ch;

        target->key = R->key;
        Node *parent = R->parent;
        if (R->left_ch == nullptr) {
            if (target != R->parent)
                R->parent->right_ch = nullptr;
            else
                target->left_ch = nullptr;
            R->parent = nullptr;
            delete R;
        } else {
            Node *lch = R->left_ch;
            R->key = lch->key;
            R->left_ch = nullptr;
            lch->parent = nullptr;
            delete lch;
        }
        go_to_root(parent);
    }
    --N;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);


//    cin >> N;
//
//    vector<int> keys;
//    vector<int> left;
//    vector<int> right;
//
//    for (int i = 0; i < N; ++i) {
//        int k, l, r;
//        cin >> k >> l >> r;
//        keys.push_back(k);
//        left.push_back(l - 1);
//        right.push_back(r - 1);
//    }
//    if (N != 0) {
//        root = new Node(keys[0]);
//        make_tree(keys, left, right, root, 0);
//    } else
//        root == nullptr;

    // Do

    int M;
    cin >> M;
    for (int i = 0; i < M; ++i) {
        char c;
        int x;
        cin >> c >> x;
        if (c == 'A') {
            insert_tree(x);
            cout << get_balance(root) << endl;
        } else if (c == 'D') {
            erase_tree(x);
            cout << get_balance(root) << endl;
        } else {
            Node *target = find_tree(root, x);
            if (target == nullptr)
                cout << 'N' << endl;
            else
                cout << 'Y' << endl;
        }
    }

    delete_tree(root);

    // output
//    int cur_number = 1;
//    map<Node *, int> new_number;
//
//    cout << N << endl;
//    if (N == 0)
//        return 0;
//
//    match_numbers(root, new_number, cur_number);
//    print_tree(new_number, root);
//    delete_tree(root);

    return 0;
}
