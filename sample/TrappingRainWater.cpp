/*
Given n non-negative integers representing an elevation map where the width of each bar is 1, compute how much water it can trap after raining.

 

Example 1:


Input: height = [0,1,0,2,1,0,1,3,2,1,2,1]
Output: 6
Explanation: The above elevation map (black section) is represented by array [0,1,0,2,1,0,1,3,2,1,2,1]. In this case, 6 units of rain water (blue section) are being trapped.
Example 2:

Input: height = [4,2,0,3,2,5]
Output: 9
 

Constraints:

n == height.length
1 <= n <= 2 * 104
0 <= height[i] <= 105
*/

#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int trap(vector<int>& height) {
        int n = height.size();
        if(n == 0) return 0;
        int left = 0, right = n-1;
        int ans = 0;
        int left_max = height[left], right_max = height[right];
        while(left < right){
            if(left_max < right_max){
                ans += left_max - height[left];
                left++;
                left_max = max(left_max, height[left]);
            }
            else{
                ans += right_max - height[right];
                right--;
                right_max = max(right_max, height[right]);
            }
        }
        return ans;
    }
};
