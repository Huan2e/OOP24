#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

class Grid
{
private:
    vector<vector<int>> grid;
    int BOX_SIZE;
    pair<int, int> GRID_SIZE;

public:
    // 根据行列初始化Grid类
    Grid(int r, int c, int bs) : BOX_SIZE(bs), GRID_SIZE(r, c)
    {
        grid = vector<vector<int>>(r, vector<int>(c, 0));
    }

    // 根据行列对初始化Grid类
    Grid(pair<int, int> gs, int bs) : GRID_SIZE(gs), BOX_SIZE(bs)
    {
        grid = vector<vector<int>>(gs.first, vector<int>(gs.second, 0));
    }

    // 根据矩阵初始化Grid类
    Grid(vector<vector<int>> &g, int b)
    {
        // check vector
        if (g.size() == 0 || g[0].size() == 0)
        {
            throw "Illegal input";
        }

        GRID_SIZE = {g.size(), g[0].size()};
        BOX_SIZE = b;
        grid = vector<vector<int>>(g);
    }

    // 拷贝构造函数
    Grid(const Grid &g) : BOX_SIZE(g.BOX_SIZE)
    {
        GRID_SIZE = g.GRID_SIZE;
        grid = vector<vector<int>>(g.grid);
    }

    // 赋值构造函数
    Grid &operator=(const Grid &g)
    {
        if (this != &g)
        {
            this->grid = g.grid;
            this->GRID_SIZE = g.GRID_SIZE;
            this->BOX_SIZE = g.BOX_SIZE;
        }

        return *this;
    }

    // 序列化
    string serialize() const
    {
        ostringstream oss;
        oss << BOX_SIZE << ',' << GRID_SIZE.first << ',' << GRID_SIZE.second << ',';
        for (int i = 0; i < GRID_SIZE.first; i++)
            for (int j = 0; j < GRID_SIZE.second; j++)
            {
                oss << grid[i][j] << ',';
            }

        return oss.str();
    }

    // 反序列化
    static Grid deserialize(const string &data)
    {
        istringstream iss(data);
        string tmp;
        getline(iss, tmp, ',');
        int bs = stoi(tmp);
        getline(iss, tmp, ',');
        int row = stoi(tmp);
        getline(iss, tmp, ',');
        int col = stoi(tmp);

        vector<vector<int>> grid = vector<vector<int>>(row, vector<int>(col, 0));
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
            {
                getline(iss, tmp, ',');
                grid[i][j] = stoi(tmp);
            }

        return Grid(grid, bs);
    }

    int getRow() { return GRID_SIZE.first; }

    int getColumn() { return GRID_SIZE.second; }

    vector<vector<int>> getGrid() { return grid; }

    // 获取数据
    int getBox(int row, int col)
    {
        if (row >= GRID_SIZE.first || col >= GRID_SIZE.second || row < 0 || col < 0)
        {
            throw "Illegal row or col.";
        }
        else
        {
            return grid[row][col];
        }
    }

    // 根据字符串获得输入，每个数据为一位数
    void inputGrid(string in)
    {
        if (in.size() != GRID_SIZE.first * GRID_SIZE.second)
        {
            throw "No match input.";
        }
        else
        {
            for (int i = 0; i < in.size(); i++)
            {
                if (in[i] < '0' || in[i] > '9')
                {
                    throw "Illegal input.";
                }
            }
            int p = 0;
            for (int i = 0; i < GRID_SIZE.first; i++)
            {
                for (int j = 0; j < GRID_SIZE.second; j++, p++)
                {
                    grid[i][j] = in[p] - '0';
                }
            }
        }
    }

    // 根据矩阵获得输入
    void inputGrid(vector<vector<int>> g)
    {
        if (g.size() == 0 || g[0].size() == 0 || g.size() != GRID_SIZE.first || g[0].size() != GRID_SIZE.second)
        {
            throw "Mismatch input";
        }
        grid = vector<vector<int>>(g);
    }

    void showGrid()
    {
        for (int i = 0; i < GRID_SIZE.first; i++)
        {
            for (int j = 0; j < GRID_SIZE.second; j++)
            {
                printf("%d\t", grid[i][j]);
            }
            printf("\n");
        }
    }
};

class Sudoku : public Grid
{
private:
    int n;

public:
    // 初始化
    Sudoku(int n = 9, string input = "") : n(n), Grid(n, n, 3)
    {
        // try to input
        if (input != "")
            inputSudoku(input);
    }

    // 拷贝构造函数
    Sudoku(const Sudoku &s) : n(s.n), Grid(s) {}

    // 赋值构造函数
    Sudoku &operator=(const Sudoku &s)
    {
        if (this != &s)
        {
            Grid::operator=(s);
            this->n = s.n;
        }

        return *this;
    }

    // 序列化
    string serialize() const
    {
        ostringstream oss;
        oss << n << ',' << Grid::serialize();
        return oss.str();
    }

    // 反序列化
    static Sudoku deserialize(const string &data)
    {
        istringstream iss(data);
        string tmp;
        getline(iss, tmp, ',');
        int n = stoi(tmp);
        iss >> tmp;
        Grid tmp_grid = Grid::deserialize(tmp);

        Sudoku s(n);
        s.inputGrid(tmp_grid.getGrid());

        return s;
    }

    // 根据字符串输入
    void inputSudoku(string input)
    {
        // check input
        bool flag = true;
        for (int i = 0; flag && i < input.size(); i++)
        {
            if (input[i] < '0' || input[i] > '9')
            {
                flag = false;
            }
        }

        if (!flag)
        {
            throw "Illegal input";
        }
        else
        {
            inputGrid(input);
        }
    }

    // 推断空白格候选数字
    void getInference(bool show = true)
    {
        vector<vector<vector<int>>> condidate(9, vector<vector<int>>(9, vector<int>(10, 0)));

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                int num = getBox(i, j);
                if (num != 0)
                {
                    // check row and column
                    for (int k = 0; k < 9; k++)
                    {
                        condidate[k][j][num] = 1;
                        condidate[i][k][num] = 1;
                    }

                    // check Box
                    for (int k = i % 3 * 3, x = 0; x < 3; x++)
                    {
                        for (int h = j % 3 * 3, y = 0; y < 3; y++)
                        {
                            condidate[k + x][h + y][num] = 1;
                        }
                    }
                }
            }
        }

        if (show)
        {
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    int num = getBox(i, j);
                    if (num == 0)
                    {
                        printf("coordinate(%d, %d): ", i + 1, j + 1);
                        for (int k = 1; k <= 9; k++)
                        {
                            if (condidate[i][j][k] == 0)
                            {
                                printf("%d, ", k);
                            }
                        }
                        printf("\n");
                    }
                }
            }
        }
    }
};

// test
int main()
{

    try
    {
        // 测试代码
        string input = "017903600000080000900000507072010430000402070064370250721000065000030000005601720";
        int n = 9;
        cout << "Init" << endl;
        Sudoku sudoku(n, input);
        sudoku.showGrid();
        cout << "Copy" << endl;
        Sudoku sudoku_copy(sudoku);
        sudoku_copy.showGrid();
        cout << "Assign" << endl;
        Sudoku sudoku_ass = sudoku;
        sudoku_ass.showGrid();
        cout << "Serialize" << endl;
        string ser = sudoku.serialize();
        Sudoku s = Sudoku::deserialize(ser);
        cout << ser << endl;
        s.showGrid();
        cout << "Condidate" << endl;
        sudoku.getInference();

        // 功能使用
        string my_input;
        int my_n = 0;
        cin >> my_input >> my_n;
        Sudoku my_sudoku(my_n, my_input);
        my_sudoku.getInference();
    }
    catch (const char *str)
    {
        cout << str << endl;
    }
    return 0;
}