#include <gui/screen2_screen/Screen2View.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Drawable.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/Unicode.hpp>
#include <stdlib.h>
#include "stm32f4xx_hal.h"

// Thêm ADC handle cho cả 2 trục joystick
extern ADC_HandleTypeDef hadc1; // ADC1 cho trục X (PC3 - ADC1_IN13)
extern ADC_HandleTypeDef hadc2; // ADC2 cho trục Y (PA5 - ADC2_IN5)

Screen2View::Screen2View()
{
    needNewTetromino = true;
    colTetris = 3;
    randomSeed = 1;
    lastMoveTime = 0;
    joystickDeadzone = 500; // Deadzone cho joystick
    moveDelay = 200; // Delay giữa các lần di chuyển (ms)

    currentRotation = 0;
	currentShapeType = 0;
	lastRotateTime = 0;
	rotateDelay = 300; // 300ms delay giữa các lần xoay

	// Khởi tạo biến điểm số và level
	score = 0;
	level = presenter->getGameLevel();
	linesCleared = 0;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
    gameStarted = true;
    needNewTetromino = true;
    rowTetris = 0;
    colTetris = 3;

    // Reset rotation
	currentRotation = 0;
	currentShapeType = 0;

	// Reset điểm số
	score = 0;
	level = presenter->getGameLevel();
	linesCleared = 0;

    randomTetromino();

    uint8_t startX = 5;
    uint8_t startY = 0;
    for (int i = 0; i < boardHeight; i++) {
        for (int j = 0; j < boardWidth; j++) {
            Box& box = gridBoxes[i][j];
            box.setPosition(startX + j * 16, startY-48 + i * 16, 16 - 1, 16 - 1);
            box.setColor(touchgfx::Color::getColorFromRGB(128, 128, 128));
            add(box);
            box.invalidate();
        }
    }

    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Box& box = gridBoxes2[i][j];
			box.setPosition(180 + j * 12, 34 + i * 12, 12 - 1, 12 - 1);
			box.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
			add(box);
			box.invalidate();
		}
	}
    updateScoreDisplay();
    lastMoveTime = HAL_GetTick();
    lastRotateTime = HAL_GetTick();
}

// Hàm kiểm tra hàng đầy
bool Screen2View::isLineFull(int row)
{
    for(int col = 0; col < boardWidth; col++) {
        if(landedBoard[row][col] == 0) {
            return false;
        }
    }
    return true;
}

// Hàm xóa một hàng
void Screen2View::clearLine(int row)
{
    // Di chuyển tất cả các hàng phía trên xuống
    for(int r = row; r > 0; r--) {
        for(int col = 0; col < boardWidth; col++) {
            landedBoard[r][col] = landedBoard[r-1][col];
        }
    }

    // Xóa hàng đầu tiên (hàng 0)
    for(int col = 0; col < boardWidth; col++) {
        landedBoard[0][col] = 0;
    }
}

// Hàm kiểm tra và xóa các hàng đầy
int Screen2View::checkAndClearLines()
{
    int clearedLines = 0;

    // Kiểm tra từ dưới lên trên
    for(int row = boardHeight - 1; row >= 0; row--) {
        if(isLineFull(row)) {
            clearLine(row);
            clearedLines++;
            row++; // Kiểm tra lại hàng này vì có hàng mới rơi xuống
        }
    }

    return clearedLines;
}

// Hàm cập nhật điểm số
void Screen2View::updateScore(int clearedLines)
{
    if(clearedLines > 0) {
        // Hệ thống tính điểm Tetris chuẩn
        int baseScore = 0;
        switch(clearedLines) {
            case 1: baseScore = 40; break;   // Single
            case 2: baseScore = 100; break; // Double
            case 3: baseScore = 300; break; // Triple
            case 4: baseScore = 1200; break; // Tetris
        }

        score += baseScore * level;
        linesCleared += clearedLines;

        // Tăng level mỗi 10 hàng
        if(linesCleared >= level * 10) {
            level++;
        }

        // CẬP NHẬT HIỂN THỊ ĐIỂM SỐ
        updateScoreDisplay();
    }
}

// Hàm cập nhật hiển thị điểm số
void Screen2View::updateScoreDisplay()
{
	touchgfx::Unicode::snprintf(scoreTextBuffer, 10, "%u", score);
    scoreText.setWildcard1(scoreTextBuffer);
    scoreText.invalidate();

    touchgfx::Unicode::snprintf(levelTextBuffer, 10, "%u", level);
	levelText.setWildcard1(levelTextBuffer);
	levelText.invalidate();

	touchgfx::Unicode::snprintf(lineTextBuffer, 10, "%u", linesCleared);
	lineText.setWildcard1(lineTextBuffer);
	lineText.invalidate();
}

// Hàm tính tốc độ rơi dựa trên level
uint32_t Screen2View::getFallDelay()
{
    // Tốc độ rơi giảm theo level (từ 500ms xuống 50ms)
    uint32_t baseDelay = 500;
    uint32_t reduction = (level - 1) * 150;

    if(reduction >= baseDelay - 50) {
        return 50; // Tốc độ tối đa
    }

    return baseDelay - reduction;
}

// Hàm đọc giá trị ADC từ joystick trục X (PC3 - ADC1_IN13)
uint16_t Screen2View::readJoystickX()
{
    // Cấu hình channel 13 cho trục X của joystick (PC3)
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_13; // PC3 = ADC1_IN13
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return 2048; // Trả về giá trị trung tâm nếu lỗi
    }

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        uint16_t value = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        return value;
    }

    HAL_ADC_Stop(&hadc1);
    return 2048; // Giá trị trung tâm 12-bit ADC
}

// Hàm đọc giá trị ADC từ joystick trục Y (PA5 - ADC2_IN5)
uint16_t Screen2View::readJoystickY()
{
    // Cấu hình channel 5 cho trục Y của joystick (PA5)
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_5; // PA5 = ADC2_IN5
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
        return 2048; // Trả về giá trị trung tâm nếu lỗi
    }

    HAL_ADC_Start(&hadc2);
    if (HAL_ADC_PollForConversion(&hadc2, 100) == HAL_OK) {
        uint16_t value = HAL_ADC_GetValue(&hadc2);
        HAL_ADC_Stop(&hadc2);
        return value;
    }

    HAL_ADC_Stop(&hadc2);
    return 2048; // Giá trị trung tâm 12-bit ADC
}

// Hàm xử lý input từ joystick
void Screen2View::handleJoystickInput()
{
    uint32_t currentTime = HAL_GetTick();

    // Chỉ xử lý input nếu đã đủ thời gian delay
    if (currentTime - lastMoveTime < moveDelay) {
        return;
    }

    uint16_t joystickX = readJoystickX();
    uint16_t joystickY = readJoystickY();

    // Joystick analog thường có giá trị từ 0-4095 (12-bit ADC)
    // Giá trị trung tâm khoảng 2048
    int16_t deltaX = joystickX - 2048;
    int16_t deltaY = joystickY - 2048;

    // Kiểm tra deadzone cho trục X (trái/phải)
    if (abs(deltaX) > joystickDeadzone) {
        if (deltaX < -joystickDeadzone) {
            // Di chuyển sang trái
            moveLeft();
            lastMoveTime = currentTime;
        }
        else if (deltaX > joystickDeadzone) {
            // Di chuyển sang phải
            moveRight();
            lastMoveTime = currentTime;
        }
    }

    // Kiểm tra deadzone cho trục Y (xuống nhanh)
    if (abs(deltaY) > joystickDeadzone) {
    	if (deltaY > joystickDeadzone) {
			// Đẩy joystick xuống để tetromino rơi nhanh
			softDrop();
			lastMoveTime = currentTime;
		}
		else if (deltaY < -joystickDeadzone) {
			// Đẩy joystick lên để xoay tetromino
			if (currentTime - lastRotateTime >= rotateDelay) {
				rotateTetromino();
				lastRotateTime = currentTime;
			}
		}
    }
}

// Hàm xoay tetromino
void Screen2View::rotateTetromino()
{
    if (!gameStarted || needNewTetromino) return;

    // Tính rotation mới (0, 1, 2, 3)
    int newRotation = (currentRotation + 1) % 4;

    // Kiểm tra xem có thể xoay không
    if (canRotate(newRotation)) {
        currentRotation = newRotation;
        getTetromino(currentShapeType, currentRotation);

        // Cập nhật hiển thị ngay lập tức
        updateCurrentBoard();
        updateDisplay();
    }
}

// Kiểm tra có thể xoay không
bool Screen2View::canRotate(int newRotation)
{
    // Lưu tetromino hiện tại
    uint8_t tempTetromino[4][4];
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            tempTetromino[i][j] = currentTetromino[i][j];
        }
    }

    // Lấy tetromino với rotation mới
    getTetromino(currentShapeType, newRotation);

    // Kiểm tra xem có thể đặt ở vị trí hiện tại không
    bool canPlace = canPlaceTetromino(rowTetris, colTetris);

    // Nếu không thể đặt, thử wall kick (đẩy ra khỏi tường)
    if (!canPlace) {
        // Thử đẩy sang trái
        if (canPlaceTetromino(rowTetris, colTetris - 1)) {
            colTetris -= 1;
            canPlace = true;
        }
        // Thử đẩy sang phải
        else if (canPlaceTetromino(rowTetris, colTetris + 1)) {
            colTetris += 1;
            canPlace = true;
        }
        // Thử đẩy lên trên (cho I-piece)
        else if (canPlaceTetromino(rowTetris - 1, colTetris)) {
            rowTetris -= 1;
            canPlace = true;
        }
    }

    // Khôi phục tetromino cũ nếu không thể xoay
    if (!canPlace) {
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                currentTetromino[i][j] = tempTetromino[i][j];
            }
        }
    }

    return canPlace;
}

void Screen2View::moveLeft()
{
    if (!gameStarted || needNewTetromino) return;

    int newCol = colTetris - 1;
    if (canPlaceTetromino(rowTetris, newCol)) {
        colTetris = newCol;
        // Cập nhật hiển thị ngay lập tức
        updateCurrentBoard();
        updateDisplay();
    }
}

void Screen2View::moveRight()
{
    if (!gameStarted || needNewTetromino) return;

    int newCol = colTetris + 1;
    if (canPlaceTetromino(rowTetris, newCol)) {
        colTetris = newCol;
        // Cập nhật hiển thị ngay lập tức
        updateCurrentBoard();
        updateDisplay();
    }
}

void Screen2View::softDrop()
{
    if (!gameStarted || needNewTetromino) return;

    // Rơi nhanh - giảm thời gian delay rơi
    if(canFall()) {
        fallTetris();
        // Cập nhật hiển thị ngay lập tức
        updateCurrentBoard();
        updateDisplay();
    }
}

void Screen2View::handleTickEvent()
{
    // LED debug - giảm thời gian delay
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_Delay(10); // Giảm delay để responsive hơn
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);

    if(!gameStarted) return;

    // Xử lý input từ joystick
    handleJoystickInput();

    // Tạo tetromino mới khi cần thiết
    if(needNewTetromino) {
    	getCurrentTetromino();
        randomTetromino();
        updateDisplay2();

        // Reset vị trí tetromino về đầu
        rowTetris = 0;
        colTetris = boardWidth/2 - 2; // Xuất hiện ở giữa
        needNewTetromino = false;

        // Kiểm tra game over
        if(!canPlaceTetromino(rowTetris, colTetris)) {
            gameStarted = false;
            presenter->goToScreen3View();
            return;
        }
    }

    // Cập nhật board và hiển thị
    updateCurrentBoard();

    // Logic rơi tetromino - tốc độ thay đổi theo level
    static uint32_t lastFallTime = 0;
    uint32_t currentTime = HAL_GetTick();

    if (lastFallTime == 0) {
        lastFallTime = currentTime;
    }

    // Tetromino rơi với tốc độ dựa trên level
    uint32_t fallDelay = getFallDelay();
    if (currentTime - lastFallTime >= fallDelay) {
        if(canFall()) {
            fallTetris();
        } else {
        	// Tetromino đã landed
			// Kiểm tra xem có phần nào của tetromino ở trên board không
			bool hasPartsAboveBoard = false;
			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++) {
					if(currentTetromino[i][j] > 0) {
						int row = rowTetris + i;
						if(row < 0) {
							hasPartsAboveBoard = true;
							break;
						}
					}
				}
				if(hasPartsAboveBoard) break;
			}

			// Nếu tetromino landed mà vẫn có phần ở trên board = game over
			if(hasPartsAboveBoard) {
				gameStarted = false;
				return;
			}

			landTetromino();

			// Kiểm tra và xóa các hàng đầy
			int clearedLines = checkAndClearLines();
			if(clearedLines > 0) {
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);
				HAL_Delay(100); // Giảm delay để responsive hơn
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
				updateScore(clearedLines);
				// Có thể thêm hiệu ứng flash hoặc animation ở đây
			}

			needNewTetromino = true;
        }
        lastFallTime = currentTime;
    }

    // Cập nhật hiển thị
    updateDisplay();

    // Giảm delay chung để game responsive hơn
    HAL_Delay(50);
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::updateCurrentBoard()
{
    // Copy landedBoard vào currentBoard
    for(int i=0; i<boardHeight; i++){
        for(int j=0; j<boardWidth; j++){
            currentBoard[i][j] = landedBoard[i][j];
        }
    }

    // Thêm tetromino hiện tại vào currentBoard
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(currentTetromino[i][j] > 0) {
                int row = rowTetris + i;
                int col = colTetris + j;
                // CHỈ vẽ trong board, NHƯNG cho phép tetromino ở trên board (row < 0)
                if(row >= 0 && row < boardHeight && col >= 0 && col < boardWidth) {
                    currentBoard[row][col] = currentTetromino[i][j];
                }
            }
        }
    }
}

void Screen2View::fallTetris()
{
    rowTetris++;
}

bool Screen2View::canFall()
{
    return canPlaceTetromino(rowTetris + 1, colTetris);
}

bool Screen2View::canPlaceTetromino(int row, int col)
{
    // Kiểm tra xem tetromino có thể đặt ở vị trí (row, col) không
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(currentTetromino[i][j] > 0) {
                int newRow = row + i;
                int newCol = col + j;

                // Kiểm tra biên trái và phải
                if(newCol < 0 || newCol >= boardWidth) {
                    return false;
                }

                // Kiểm tra biên dưới
                if(newRow >= boardHeight) {
                    return false;
                }

                // CHỈ kiểm tra va chạm với landed tetromino khi newRow >= 0
                // Cho phép tetromino xuất hiện ở trên board (newRow < 0)
                if(newRow >= 0 && landedBoard[newRow][newCol] > 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Screen2View::landTetromino()
{
    // Đặt tetromino vào landedBoard
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(currentTetromino[i][j] > 0) {
                int row = rowTetris + i;
                int col = colTetris + j;
                if(row >= 0 && row < boardHeight && col >= 0 && col < boardWidth) {
                    landedBoard[row][col] = currentTetromino[i][j];
                }
            }
        }
    }
}

void Screen2View::updateDisplay()
{
    // Cập nhật hiển thị grid
    for (int i = 0; i < boardHeight; i++) {
        for (int j = 0; j < boardWidth; j++) {
            Box& box = gridBoxes[i][j];
            if(currentBoard[i][j] > 0){
                box.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0)); // Đen cho tetromino
            }else{
                box.setColor(touchgfx::Color::getColorFromRGB(128, 128, 128)); // Xám cho ô trống
            }
            box.invalidate();
        }
    }
}

void Screen2View::updateDisplay2()
{
    // Cập nhật hiển thị grid
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Box& box = gridBoxes2[i][j];
            if(nextTetromino[i][j] > 0){
                box.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0)); // Đen cho tetromino
            }else{
                box.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255)); // Trang cho ô trống
            }
            box.invalidate();
        }
    }
}

void Screen2View::randomTetromino()
{
    randomSeed = (randomSeed * 1103515245 + 12345) & 0x7fffffff;
    nextShapeType = randomSeed % 7; // Lưu loại shape
    nextRotation = 0; // Bắt đầu với rotation 0

    getNextTetromino(nextShapeType, nextRotation);
}

// Ham lay tetromino tu mang static
void Screen2View::getTetromino(int shapeType, int rotation)
{
    // Dam bao bounds checking
    if(shapeType < 0 || shapeType >= 7) shapeType = 0;
    if(rotation < 0 || rotation >= 4) rotation = 0;

    // Copy tu mang static vao currentTetromino
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            currentTetromino[i][j] = tetrominoShapes[shapeType][rotation][i][j];
        }
    }
}

void Screen2View::getNextTetromino(int shapeType, int rotation)
{
    // Dam bao bounds checking
    if(shapeType < 0 || shapeType >= 7) shapeType = 0;
    if(rotation < 0 || rotation >= 4) rotation = 0;

    // Copy tu mang static vao currentTetromino
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            nextTetromino[i][j] = tetrominoShapes[shapeType][rotation][i][j];
        }
    }
}

void Screen2View::getCurrentTetromino()
{
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            currentTetromino[i][j] = nextTetromino[i][j];
        }
    }
    currentShapeType = nextShapeType;
    currentRotation = nextRotation;
}

const uint8_t Screen2View::tetrominoShapes[7][4][4][4] = {
    // L Shape - 4 rotation states
    {
        // Rotation 0
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 0, 0, 0},
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        }
    },

    // J Shape - 4 rotation states
    {
        // Rotation 0
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {1, 1, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        }
    },

    // O Shape - 4 rotation states (all same)
    {
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // T Shape - 4 rotation states
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // S Shape - 4 rotation states
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // Z Shape - 4 rotation states
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // I Shape - 4 rotation states
    {
        // Rotation 0
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0}
        }
    }
};
