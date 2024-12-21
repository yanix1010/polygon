#include <vector> 
using namespace std;

class Solution {
public:
    int minNumberOperations(vector<int>& target) {
        int operations = 0;
        int previous = 0;

        for (int current : target) {
            if (current > previous) {
                operations += (current - previous);
            }
            previous = current;
        }

        return operations;
    }
};