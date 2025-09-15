all: ./sources/*.cpp
	g++ -c -g ./sources/*.cpp
	g++ -c ./glad/src/gl.c
	g++ -c ./deps/miniaudio.c
	g++ ./*.o -o test -lglfw3
	rm -rf ./*.o

windows: ./sources/*.cpp
	mkdir -p ./winbuild
	x86_64-w64-mingw32-g++ -c -g ./sources/*.cpp
	x86_64-w64-mingw32-g++ -c ./glad/src/gl.c
	x86_64-w64-mingw32-g++ -c ./deps/miniaudio.c
	x86_64-w64-mingw32-g++ ./*.o -o ./winbuild/test.exe -lglfw3 -lgdi32 -static
	rm -rf ./*.o

web:
	mkdir -p ./webbuild
	emcc --use-port=emdawnwebgpu -sSTACK_SIZE=128kb --use-port=contrib.glfw3:disableMultiWindow=true ./deps/miniaudio.c ./sources/*.cpp -o ./webbuild/index.html -s WASM=1 -s DISABLE_EXCEPTION_CATCHING=1 -s ENVIRONMENT=web --preload-file shaders --preload-file img --preload-file snd --preload-file levels

linux: ./sources/*.cpp
	mkdir -p ./linbuild
	g++ -c -g ./sources/*.cpp
	g++ -c ./glad/src/gl.c
	g++ -c ./deps/miniaudio.c
	g++ ./*.o -o ./linbuild/test -lglfw3
	rm -rf ./*.o

clean:
	rm -rf ./linbuild
	rm -rf ./winbuild
	rm -rf ./webbuild
	rm -rf ./*.o
	rm -rf ./test
	rm -rf ./*.exe