class InitBinaryTree {
    int i;
    InitBinaryTree left;
    InitBinaryTree right;

    InitBinaryTree(int _i, InitBinaryTree _left, InitBinaryTree _right) {
        i = _i;
        left = _left;
        right = _right;
    }

    int sumAll() {
        int sum = this.i;
        if (this.left != null) {
            sum += this.left.sumAll();
        }
        if (this.right != null) {
            sum += this.right.sumAll();
        }
        return sum;
    }

    static int maxArray(int[] array) {
        int max = array[0];
        for (int i = 0; i < array.length; i++) {
            if (array[i] > max) {
                max = array[i];
            }
        }
        return max;
    }
}

public class Main {
    public static void main(String[] args) {
        InitBinaryTree tree = new InitBinaryTree(1,
                new InitBinaryTree(2, new InitBinaryTree(3, null, null), new InitBinaryTree(4, null, null)),
                new InitBinaryTree(5, new InitBinaryTree(6, null, null), new InitBinaryTree(7, null, null)));
        System.out.println(tree.sumAll());
        int[] array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        System.out.println(InitBinaryTree.maxArray(array));
    }
}
