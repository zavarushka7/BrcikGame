WORKSPACE := $(shell pwd)
BUILD_DIR := $(WORKSPACE)/build
CONSOLE_EXEC := $(BUILD_DIR)/brickgame_console
GUI_EXEC := $(BUILD_DIR)/BrickGame
CMAKE_BUILD_DIR := $(BUILD_DIR)/cmake-build
GUI_SOURCE_DIR := $(WORKSPACE)/gui/desktop
TETRIS_LIB := $(BUILD_DIR)/libtetris.so
REPORT_FILE := coverage.html

.PHONY: all install uninstall clean dvi dist test gcov_report ask console gui tetris_lib rebuild format-check format memcheck

all: ask

install: ask
	@echo "Installing game..."

uninstall:
	@echo "Removing game files"
	@rm -rf "$(BUILD_DIR)"

clean:
	@echo "Cleaning build files"
	@rm -rf "$(BUILD_DIR)"
	@rm -f *.gcda *.gcno *.gcov *.out *.o
	@rm -rf coverage_report .snake_high_score.txt .tetris_high_score.txt 

dvi:
	@echo "Opening README.md..."
	xdg-open "$(WORKSPACE)/README.md" 
	

dist: clean all
	@echo "Creating distribution package..."
	@mkdir -p "$(BUILD_DIR)/dist"
	@mkdir -p "$(WORKSPACE)/src"
	@cp "$(CONSOLE_EXEC)" "$(GUI_EXEC)" "$(TETRIS_LIB)" "$(WORKSPACE)/README.md" "$(BUILD_DIR)/dist/" 2>/dev/null || true
	@cd "$(BUILD_DIR)" && tar -czf "$(WORKSPACE)/BrickGame_dist.tar.gz" -C dist .
	@echo "Distribution package created at $(WORKSPACE)/BrickGame_dist.tar.gz"

test: $(BUILD_DIR)/snake_test
	cd $(BUILD_DIR) && ./snake_test

$(BUILD_DIR)/snake_test: tests/snakeTest.cpp brick_game/snake/snakeModel.cpp brick_game/snake/snakeController.cpp brick_game/snake/consoleView.cpp brick_game/tetris/backend.c gui/cli/frontend.c | $(BUILD_DIR) 
	g++ -std=c++17 -DTESTING -I../include -I/usr/include/ncurses \
		--coverage -fprofile-arcs -ftest-coverage \
		$^ -lgtest -lgtest_main -lpthread -lncurses \
		-o $@

gcov_report: test
	cd $(BUILD_DIR) && \
	gcovr --root .. \
		--exclude-unreachable-branches \
		--exclude '/usr/include/.*' \
		--exclude '.*/tests/.*' \
		--filter '../brick_game/snake/snakeController.cpp' \
		--filter '../brick_game/snake/snakeModel.cpp' \
		--html-details -o $(REPORT_FILE)
	@if [ -f "$(BUILD_DIR)/$(REPORT_FILE)" ]; then \
		firefox "file://$(BUILD_DIR)/$(REPORT_FILE)"; \
	else \
		echo "Ошибка: файл отчета не был создан!"; \
		exit 1; \
	fi
ask:
	@choice=$$(whiptail --title "Choosing the game version" --menu "    Select the interface version" 15 40 2 \
	"1" "Console version" \
	"2" "GUI version" \
	3>&1 1>&2 2>&3); \
	if [ $$? -eq 0 ]; then \
		if [ "$$choice" = "1" ]; then \
			$(MAKE) console; \
		elif [ "$$choice" = "2" ]; then \
			$(MAKE) gui; \
		fi; \
	else \
		echo "The choice is canceled"; \
	fi

tetris_lib: $(BUILD_DIR)
	@echo "Building Tetris library..."
	@mkdir -p "$(BUILD_DIR)/tetris"
	@cd "$(BUILD_DIR)/tetris" && \
		cmake "$(WORKSPACE)/brick_game/tetris" -DCMAKE_BUILD_TYPE=Release && \
		make
	@cp "$(BUILD_DIR)/tetris/libtetris.so" "$(TETRIS_LIB)"
	@echo "Tetris library built at $(TETRIS_LIB)"

console: tetris_lib
	@echo "Building console version..."
	@g++ -Wall -Wextra -Werror -std=c++20 "$(WORKSPACE)/brick_game/main.cpp" \
	"$(WORKSPACE)/brick_game/snake/snakeModel.cpp" \
	"$(WORKSPACE)/brick_game/snake/consoleView.cpp" \
	"$(WORKSPACE)/brick_game/snake/snakeController.cpp" \
	"$(WORKSPACE)/gui/cli/frontend.c" \
	-lncurses -L"$(BUILD_DIR)" -ltetris -Wl,-rpath="$(BUILD_DIR)" \
	-o "$(CONSOLE_EXEC)"
	@echo "Console version built at $(CONSOLE_EXEC)"
	@"$(CONSOLE_EXEC)"

gui: $(BUILD_DIR)
	@echo "Building GUI version..."
	@mkdir -p "$(CMAKE_BUILD_DIR)"
	@cd "$(CMAKE_BUILD_DIR)" && \
		rm -rf * && \
		cmake "$(GUI_SOURCE_DIR)" -DCMAKE_BUILD_TYPE=Release && \
		make
	@cp "$(CMAKE_BUILD_DIR)/BrickGameApp" "$(GUI_EXEC)"
	@echo "GUI version built at $(GUI_EXEC)"
	@"$(GUI_EXEC)"

$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

rebuild: clean all

format-check:
	@cp "../materials/linters/.clang-format" ".clang-format"
	@find . -name "*.cc" -o -name "*.h" -o -name "*.cpp" -o -name "*.c" | xargs clang-format -n --Werror
	@rm ".clang-format"

format:
	@cp "../materials/linters/.clang-format" ".clang-format"
	@find . -name "*.cc" -o -name "*.h" -o -name "*.cpp" -o -name "*.c" | xargs clang-format -i --Werror
	@rm ".clang-format"

