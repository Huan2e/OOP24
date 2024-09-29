import numpy as np
import copy

class Grid:
    # 初始化
    def __init__(self, row, col, bs):
        self.GRID_SIZE = (row, col)
        self.BOX_SIZE = bs
        self.grid = np.zeros((row, col), dtype=int)
    
    # 序列化
    def serialize(self):
        return f"{self.BOX_SIZE},{self.GRID_SIZE[0]},{self.GRID_SIZE[1]}," + ",".join(map(str, self.grid.flatten()))

    # 反序列化
    @staticmethod
    def deserialize(data):
        parts = data.split(',')
        bs, row, col = int(parts[0]), int(parts[1]), int(parts[2])
        grid_data = np.array(list(map(int, parts[3:])), dtype=int).reshape((row, col))
        g = Grid(row, col, bs)
        g.grid = grid_data
        return g
    
    # 获取数据
    def getBox(self, row, col):
        if 0 <= row < self.GRID_SIZE[0] and 0 <= col < self.GRID_SIZE[1]:
            return self.grid[row, col]
        else:
            raise ValueError("Illegal row or col.")

    # 根据字符串获得输入，每个数据为一位数
    def inputGrid(self, input):
        if type(input) == str:
            if len(input) != self.GRID_SIZE[0] * self.GRID_SIZE[1]:
                raise ValueError("No match input.")
            elif any((c < '0' or c > '9') for c in input):
                raise ValueError("Illegal input")
            self.grid = np.array([[int(input[i * self.GRID_SIZE[1] + j]) for j in range(self.GRID_SIZE[1])] for i in range(self.GRID_SIZE[0])], dtype=int)
        else:
            raise ValueError("Illegal input.")

    def showGrid(self):
        print(self.grid)

class Sudoku(Grid):
    # 初始化
    def __init__(self, n=9, input=""):
        super().__init__(n, n, 3)
        self.n = n
        if input != "":
            self.inputSudoku(input)

    # 序列化
    def serialize(self):
        return f"{self.n}," + super().serialize()

    # 反序列化
    @staticmethod
    def deserialize(data):
        parts = data.split(',')
        n = int(parts[0])
        bs = int(parts[1])
        row = int(parts[2])
        col = int(parts[3])
        grid_data = "".join(parts[4:])  # Skip n and BOX_SIZE
        return Sudoku(n, grid_data)

    # 输入数独
    def inputSudoku(self, input):
        if any((c < '0' or c > '9') for c in input):
            raise ValueError("Illegal input")
        self.inputGrid(input)
    
    # 获得候选数据
    def getInference(self, show=True):
        candidate = np.zeros((9, 9, 10), dtype=int)

        for i in range(9):
            for j in range(9):
                num = self.getBox(i, j)
                if num != 0:
                    candidate[:, j, num] = 1
                    candidate[i, :, num] = 1
                    for k in range(i % 3 * 3, i % 3 * 3 + 3):
                        for h in range(j % 3 * 3, j % 3 * 3 + 3):
                            candidate[k, h, num] = 1

        if show:
            for i in range(9):
                for j in range(9):
                    if self.getBox(i, j) == 0:
                        flag = [k for k in range(1, 10) if candidate[i, j, k] == 0]
                        if flag:
                            print(f"coordinate({i + 1}, {j + 1}): {', '.join(map(str, flag))}")


if __name__ == "__main__":


    try:
        # 测试代码
        input_str = "017903600000080000900000507072010430000402070064370250721000065000030000005601720"
        n = 9
        # Init
        sudoku = Sudoku(n, input_str)
        sudoku.showGrid()
        # Copy
        sudoku_copy = copy.deepcopy(sudoku)
        sudoku_copy.showGrid()
        # Assign
        sudoku_ass = sudoku
        sudoku_ass.showGrid()
        # Serialize
        ser = sudoku.serialize()
        s = Sudoku.deserialize(ser)
        print(ser)
        s.showGrid()
        # Get condidate
        sudoku.getInference()

        # 功能使用
        my_input = input()
        my_n = input()
        my_sudoku = Sudoku(int(my_n), my_input)
        sudoku.getInference()

    except ValueError as e:
        print(e)