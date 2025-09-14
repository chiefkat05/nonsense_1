all: ./sources/*.cpp
	g++ -c -g ./sources/*.cpp
	g++ -c ./glad/src/gl.c
	g++ ./*.o -o test -lglfw3
	rm -rf ./*.o

windows: ./sources/*.cpp
	mkdir -p ./winbuild
	x86_64-w64-mingw32-g++ -c -g ./sources.*.cpp
	x86_64-w64-mingw32-g++ -c ./glad/src/gl.c
	x86_64-w64-mingw32-g++ ./*.o -o test.exe -lglfw3 -lgdi32 -static
	rm -rf ./*.o

# web:
# 	mkdir -p ./webbuild
# 	emcc -c -g ./sources/*.cpp
# 	emcc -c ./glad/src/gl.c
# 	emcc --use-port=contrib.glfw3:disableMultiWindow=true -s DISABLE_EXCEPTION_CATCHING=1 ./*.o -o ./webbuild/index.html -s WASM=1

web:
	mkdir -p ./webbuild
	emcc --use-port=emdawnwebgpu --use-port=contrib.glfw3:disableMultiWindow=true -s DISABLE_EXCEPTION_CATCHING=1 -s ENVIRONMENT=web ./sources/*.cpp -o ./webbuild/index.html -s WASM=1 --preload-file shaders --preload-file img --preload-file snd

linux: ./*.cpp
	mkdir -p ./linbuild
	g++ ./*.cpp -o ./linbuild/test
	rm -rf ./*.o

clean:
	rm -rf ./linbuild
	rm -rf ./winbuild
	rm -rf ./webbuild
	rm -rf ./*.o
	rm -rf ./test
	rm -rf ./*.exe