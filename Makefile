IMAGE ?= cs107-dev
BUILD_DIR ?= build
CMAKE_BUILD_TYPE ?= Debug

.PHONY: help docker-build docker-shell docker-run docker-debug-shell docker-compose examples clean

help:
	@printf "Targets:\n"
	@printf "  make docker-build        Build the CS107 Docker image\n"
	@printf "  make docker-shell        Start a shell in the CS107 Docker image\n"
	@printf "  make docker-debug-shell  Start a shell with ptrace enabled for gdb\n"
	@printf "  make docker-compose      Start a shell through docker compose\n"
	@printf "  make examples            Configure and build examples with CMake\n"
	@printf "  make clean               Remove generated build output\n"

docker-build:
	docker build -t $(IMAGE) .

docker-shell: docker-build
	docker run --rm -it -e HOME=/root -v "$(CURDIR)":/workspace -w /workspace $(IMAGE)

docker-run: docker-shell

docker-debug-shell: docker-build
	docker run --rm -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -e HOME=/root -v "$(CURDIR)":/workspace -w /workspace $(IMAGE)

docker-compose:
	docker compose run --rm cs107

examples:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	cmake --build $(BUILD_DIR) --target examples --parallel

clean:
	rm -rf $(BUILD_DIR)
