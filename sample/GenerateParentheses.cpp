/*
Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses.

Example 1:

Input: n = 3
Output: ["((()))","(()())","(())()","()(())","()()()"]

Example 2:

Input: n = 1
Output: ["()"]
 
Constraints:

1 <= n <= 8
*/


#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> res;
        string s;
        generate(n, n, s, res);
        return res;
    }
    
    void generate(int left, int right, string s, vector<string>& res) {
        if (left == 0 && right == 0) {
            res.push_back(s);
            return;
        }
        if (left > 0) {
            generate(left - 1, right, s + '(', res);
        }
        if (right > left) {
            generate(left, right - 1, s + ')', res);
        }
    }
};
