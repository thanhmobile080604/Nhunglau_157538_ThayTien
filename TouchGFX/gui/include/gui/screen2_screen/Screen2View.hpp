#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>


class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void handleTickEvent();

    // Các hàm mới cho joystick
	void handleJoystickInput();
	uint16_t readJoystickX();
	uint16_t readJoystickY();
	void moveLeft();
	void moveRight();
	void softDrop();
	void rotateTetromino(); // Hàm xoay tetromino
	bool canRotate(int newRotation); // Kiểm tra có thể xoay không

    void updateCurrentBoard();
	void fallTetris();
	bool canFall();
	bool canPlaceTetromino(int row, int col);
	void landTetromino();
	void updateDisplay();
	void updateDisplay2();
	void randomTetromino();
	void getTetromino(int shapeType, int rotation);
	void getNextTetromino(int shapeType, int rotation);
	void getCurrentTetromino();
	void updateScoreDisplay();

protected:
    static const uint8_t tetrominoShapes[7][4][4][4];
    bool gameStarted;
    bool needNewTetromino;
    int tickCount;

    uint32_t lastMoveTime;        // Thời gian lần di chuyển cuối
	uint16_t joystickDeadzone;    // Deadzone cho joystick
	uint32_t moveDelay;           // Delay giữa các lần di chuyển (ms)

	int currentRotation; // Thêm biến để lưu rotation hiện tại
	int currentShapeType; // Thêm biến để lưu loại shape hiện tại
	int nextRotation; // Thêm biến để lưu rotation tiep theo
	int nextShapeType; // Thêm biến để lưu loại shape tiep theo
	uint32_t lastRotateTime; // Thêm để tránh xoay quá nhanh
	uint32_t rotateDelay; // Delay giữa các lần xoay

    Box gridBoxes[23][10];
    Box gridBoxes2[4][4];
	uint8_t boardWidth = 10;
	uint8_t boardHeight = 23;
	uint8_t currentBoard[23][10] = {0};
	uint8_t landedBoard[23][10] = {0};
	uint8_t currentTetromino[4][4] = {0};
	uint8_t nextTetromino[4][4] = {0};
	uint8_t rowTetris=0;
	uint8_t colTetris=3;
	uint32_t randomSeed;


	uint32_t score;           // Điểm số hiện tại
	uint8_t level;            // Level hiện tại
	uint16_t linesCleared;    // Tổng số hàng đã xóa

	bool isLineFull(int row);                    // Kiểm tra hàng có đầy không
	void clearLine(int row);                     // Xóa một hàng
	int checkAndClearLines();                    // Kiểm tra và xóa tất cả hàng đầy
	void updateScore(int clearedLines);          // Cập nhật điểm số
	uint32_t getFallDelay();                     // Tính tốc độ rơi theo level
};

#endif // SCREEN2VIEW_HPP
