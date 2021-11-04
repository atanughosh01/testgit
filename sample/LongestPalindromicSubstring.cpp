/*
Given a string s, return the longest palindromic substring in s.

Example 1:

Input: s = "babad"
Output: "bab"
Note: "aba" is also a valid answer.
Example 2:


Example 2:

Input: s = "cbbd"
Output: "bb"
Example 3:


Example 3:

Input: s = "a"
Output: "a"
Example 4:


Example 4:

Input: s = "ac"
Output: "a"


Constraints:

1 <= s.length <= 1000
s consist of only digits and English letters.
*/

#include <iostream>
#include <bits/stdc++.h>
using namespace std;


class Solution {

int expandAroundCenter(string s, int left, int right) {
    int L = left, R = right;
    while (L >= 0 && R < s.length() && s[L] == s[R]) {
        L--;
        R++;
    }
    return R - L - 1;
}

public:
    string longestPalindrome(string s) {
        int n = s.length();
        if(n == 0) return "";
        int start = 0, end = 0;
        for(int i = 0; i < n; i++) {
            int len1 = expandAroundCenter(s, i, i);
            int len2 = expandAroundCenter(s, i, i+1);
            int len = max(len1, len2);
            if(len > end - start) {
                start = i - (len - 1)/2;
                end = i + len/2;
            }
        }
        return s.substr(start, end - start + 1);        
    }
};
