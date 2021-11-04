/*
Given a string containing just the characters '(' and ')', find the length of the longest valid (well-formed) parentheses substring.

Example 1:

Input: s = "(()"
Output: 2
Explanation: The longest valid parentheses substring is "()".

Example 2:

Input: s = ")()())"
Output: 4
Explanation: The longest valid parentheses substring is "()()".

Example 3:

Input: s = ""
Output: 0
 
Constraints:

0 <= s.length <= 3 * 104
s[i] is '(', or ')'.
*/


#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int longestValidParentheses(string s) {
        int n = s.size();
        if(n == 0) return 0;
        int dp[n];
        memset(dp, 0, sizeof(dp));
        int max_len = 0;
        for(int i = 1; i < n; i++){
            if(s[i] == ')'){
                if(s[i-1] == '('){
                    dp[i] = (i >= 2 ? dp[i-2] : 0) + 2;
                }
                else if(i - dp[i-1] > 0 && s[i-dp[i-1]-1] == '('){
                    dp[i] = dp[i-1] + 2 + (i - dp[i-1] >= 2 ? dp[i-dp[i-1]-2] : 0);
                }
            }
            max_len = max(max_len, dp[i]);
        }
        return max_len;
    }
};
