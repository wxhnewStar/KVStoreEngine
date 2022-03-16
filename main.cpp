//
// Created by shainvol on 2022/3/16.
//

#include "skiplist.h"

using namespace  std;

int main() {
    skiplist<int,string> m_sk(16);
    int opCode, key;
    string value;
    while (true) {
        cin >> opCode;
        if (opCode == 1) {
            cin >> key >> value;
            m_sk.insert_element(key,value);
        } else if (opCode == 2) {
            cin >> key;
            m_sk.search_element(key);
        } else if (opCode == 3) {
            cin >> key;
            m_sk.delete_element(key);
        } else if (opCode == 4) {
            m_sk.display_list();
        }
    }
}