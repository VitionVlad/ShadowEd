Name = ShadowTech

File = main.cpp

all:
	@echo "compiling..."
	@g++ $(File) imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp  -o $(Name) -lglfw -lGL -lGLEW -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -m64 -g -lalut -lopenal

clean:
	@echo "deleting..."
	@rm -rf $(Name)