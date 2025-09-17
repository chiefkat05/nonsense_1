all: ./sources/*.cpp
	g++ -c -g ./sources/*.cpp
	g++ -c ./glad/src/gl.c
	g++ -c ./deps/miniaudio.c
	g++ ./*.o -o test -lglfw3
	rm -rf ./*.o

windows: ./sources/*.cpp
	rm -rf ./winbuild
	mkdir ./winbuild
	cp -r ./img/ ./winbuild/
	cp -r ./snd/ ./winbuild/
	cp -r ./shaders/ ./winbuild/
	cp -r ./levels/ ./winbuild/
	x86_64-w64-mingw32-g++ -c -g ./sources/*.cpp
	x86_64-w64-mingw32-g++ -c ./glad/src/gl.c
	x86_64-w64-mingw32-g++ -c ./deps/miniaudio.c
	x86_64-w64-mingw32-g++ ./*.o -o ./winbuild/test.exe -lglfw3 -lgdi32 -static
	rm -rf ./*.o

web:
	mkdir -p ./webbuild
	emcc --use-port=emdawnwebgpu -sSTACK_SIZE=128kb --use-port=contrib.glfw3:disableMultiWindow=true ./deps/miniaudio.c ./sources/*.cpp -o ./webbuild/index.html -s WASM=1 -s DISABLE_EXCEPTION_CATCHING=1 -s ENVIRONMENT=web --preload-file shaders --preload-file img --preload-file snd --preload-file levels
# 	cp index.html.bak ./webbuild/
# 	rm -rf ./webbuild/index.html
# 	mv ./webbuild/index.html.bak ./webbuild/index.html

linux: ./sources/*.cpp
	rm -rf ./linbuild
	mkdir ./linbuild
	cp -r ./img/ ./linbuild/
	cp -r ./snd/ ./linbuild/
	cp -r ./shaders/ ./linbuild/
	cp -r ./levels/ ./linbuild/
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