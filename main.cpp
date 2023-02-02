#include <iostream>

#include "Prop.hpp"

#include <fstream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "res.hpp"

Engine eng;

float speed = 0.05;

bool mousefocused = false;

Mesh levelm[1000];

int renderm[1000] = {
    2,
    2,
    2,
    2
};

bool useuishader[1000];

bool reloadtex[1000];

bool writetextores[1000];

bool writetextoinit[1000];

bool writeobjtofile[1000];

int currentm = 0;

int currentl = 0;

int shadowmapcnt = 0;

bool rendershadow = true;

char pathtoalbedo[1000][256];

char pathtospec[1000][256];

char classname[1000][256];

char classnamespec[1000][256];

char pathtoobj[1000][256];

char objclassname[1000][256];

ofstream outputres;

ofstream outputinit;

ofstream outputinitj;

ofstream projfile;

ifstream projfilei;

char projfilename[256];

class lmesh{
    public:
    vector<vec3> vert;
    vector<vec3> normals;
    vector<vec2> uv;
};

void loadobj(const char* path, lmesh& toReturn){
	vector<vec3> lvertex;
	vector<vec3> lnormals;
	vector<vec2> luv;
	vector<int> indices;
	vector<int> nindices;
	vector<int> uindices;
    FILE* obj;
	//fopen(obj, path, "r");
    obj = fopen(path, "r");
	string arg;
	int line = 1;
	int nline = 1;
	int uvline = 1;
	int faceline = 0;
	while (1) {
		char lineHeader[128];

		if (obj == 0) {
			exit(-1);
		}

		int res = fscanf(obj, "%s", lineHeader);
		if (res == EOF) {
			break;
		}

		if (strcmp(lineHeader, "#") == 0) {
			fscanf(obj, "%*[^\n]\n");
		}

		if (strcmp(lineHeader, "s") == 0) {
			fscanf(obj, "%*[^\n]\n");
		}

		if (strcmp(lineHeader, "o") == 0) {
			fscanf(obj, "%*[^\n]\n");
		}

		if (strcmp(lineHeader, "v") == 0) {
			lvertex.resize(lvertex.size()+6);
			fscanf(obj, "%f %f %f \n", &lvertex[line].x, &lvertex[line].y, &lvertex[line].z);
            cout << "vertex = " << lvertex[line].x << " " << lvertex[line].y << " " << lvertex[line].z << endl;
			line++;
		}

		if (strcmp(lineHeader, "vn") == 0) {
			lnormals.resize(lnormals.size()+6);
			fscanf(obj, "%f %f %f \n", &lnormals[nline].x, &lnormals[nline].y, &lnormals[nline].z);
            cout << "normals = " << lnormals[nline].x << " " << lnormals[nline].y << " " << lnormals[nline].z << endl;
			nline++;
		}

		if (strcmp(lineHeader, "vt") == 0) {
			luv.resize(luv.size()+4);
			fscanf(obj, "%f %f \n", &luv[uvline].x, &luv[uvline].y);
            cout << "uv = " << luv[uvline].x << " " << luv[uvline].y << endl;
			uvline++;
		}

		if (strcmp(lineHeader, "f") == 0) {
			indices.resize(indices.size()+3);
			uindices.resize(uindices.size()+3);
			nindices.resize(nindices.size()+3);
			fscanf(obj, "%d/%d/%d %d/%d/%d %d/%d/%d \n", &indices[faceline], &uindices[faceline], &nindices[faceline], &indices[faceline+1], &uindices[faceline+1], &nindices[faceline+1], &indices[faceline+2], &uindices[faceline+2], &nindices[faceline+2]);
			faceline = faceline + 3;
		}
	}
	fclose(obj);
    cout << "resizing..." << endl;
    toReturn.vert.resize(faceline);
    toReturn.normals.resize(faceline);
    toReturn.uv.resize(faceline);
    cout << "resized!" << endl;
    for(int i = 0; i != faceline; i++){
        toReturn.vert[i].x = lvertex[indices[i]].x;
		toReturn.vert[i].y = lvertex[indices[i]].y;
		toReturn.vert[i].z = lvertex[indices[i]].z;

        toReturn.normals[i].x = lnormals[nindices[i]].x;
		toReturn.normals[i].y = lnormals[nindices[i]].y;
		toReturn.normals[i].z = lnormals[nindices[i]].z;

        toReturn.uv[i].x = luv[uindices[i]].x;
		toReturn.uv[i].y = luv[uindices[i]].y;
    }
}

void objexport(string filename, string classname){
    lmesh Finale;
    loadobj(filename.c_str(), Finale);
    ofstream myfile;
    myfile.open(classname+"_model.java");
    myfile << "package com.example.openglapp;" << endl << endl;
    myfile << "public class " << classname << "_model {" << endl;
    myfile << "public float[] verts = {" << endl;
    cout << "package com.example.openglapp;" << endl << endl;
    cout << "public class model {" << endl;
    cout << "float[] verts = {" << endl;
    for(int i = 0; i != Finale.vert.size(); i++){
        myfile << Finale.vert[i].x << "f, " << Finale.vert[i].y << "f, " << Finale.vert[i].z << "f, " << endl;
        cout << Finale.vert[i].x << "f, " << Finale.vert[i].y << "f, " << Finale.vert[i].z << "f, " << endl;
    }
	myfile << "};" << endl << "}";
    cout << "};" << endl << "}";
	myfile.close();
	//normals
   myfile.open(classname+"_normals.java");\
    myfile << "package com.example.openglapp;" << endl << endl;
    myfile << "public class " << classname << "_normals {" << endl;
    myfile << "public float[] verts = {" << endl;
    cout << "package com.example.openglapp;" << endl << endl;
    cout << "public class normals {" << endl;
    cout << "float[] verts = {" << endl;
    for(int i = 0; i != Finale.vert.size(); i++){
        myfile << Finale.normals[i].x << "f, " << Finale.normals[i].y << "f, " << Finale.normals[i].z << "f, " << endl;
        cout << Finale.normals[i].x << "f, " << Finale.normals[i].y << "f, " << Finale.normals[i].z << "f, " << endl;
    }
    myfile << "};" << endl << "}";
    cout << "};" << endl << "}";
	myfile.close();
	//uv
	 myfile.open(classname+"_uv.java");
    myfile << "package com.example.openglapp;" << endl << endl;
    myfile << "public class " << classname << "_uv {" << endl;
    myfile << "public float[] verts = {" << endl;
    cout << "package com.example.openglapp;" << endl << endl;
    cout << "public class uv {" << endl;
    cout << "float[] verts = {" << endl;
    for(int i = 0; i != Finale.vert.size(); i++){
        myfile << Finale.uv[i].x << "f, " << Finale.uv[i].y << "f, " <<  endl;
        cout << Finale.uv[i].x << "f, " << Finale.uv[i].y << "f, " <<  endl;
    }
    myfile << "};" << endl << "}";
    cout << "};" << endl << "}";
    myfile.close();

    myfile.open(classname+"_model.hpp");
    myfile << "#include <iostream>" << endl << endl;
    myfile << "class " << classname << "_model {" << endl;
    myfile << "public:" << endl;
    myfile << "int totalv = " << Finale.vert.size() << ";" << endl;
    myfile << "float verts [16383] = {" << endl;
    cout << "#include <iostream>;" << endl << endl;
    cout << "class " << classname << "_model {" << endl;
    cout << "float verts [" << Finale.vert.size()*3 << "] = {" << endl;
    for(int i = 0; i != Finale.vert.size(); i++){
        myfile << Finale.vert[i].x << ", " << Finale.vert[i].y << ", " << Finale.vert[i].z << ", " << endl;
        cout << Finale.vert[i].x << ", " << Finale.vert[i].y << ", " << Finale.vert[i].z << ", " << endl;
    }
	myfile << "};" << endl << "};";
    cout << "};" << endl << "};";
	myfile.close();

    myfile.open(classname+"_model.hpp");
    myfile << "#include <iostream>" << endl << endl;
    myfile << "class " << classname << "_model {" << endl;
    myfile << "public:" << endl;
    myfile << "int totalv = " << Finale.vert.size() << ";" << endl;
    myfile << "float verts [16383] = {" << endl;
    cout << "#include <iostream>;" << endl << endl;
    cout << "class " << classname << "_model {" << endl;
    cout << "float verts [" << Finale.vert.size()*3 << "] = {" << endl;
    for(int i = 0; i != Finale.vert.size(); i++){
        myfile << Finale.vert[i].x << ", " << Finale.vert[i].y << ", " << Finale.vert[i].z << ", " << endl;
        cout << Finale.vert[i].x << ", " << Finale.vert[i].y << ", " << Finale.vert[i].z << ", " << endl;
    }
	myfile << "};" << endl << "};";
    cout << "};" << endl << "};";
	myfile.close();

    myfile.open(classname+"_normals.hpp");
    myfile << "#include <iostream>" << endl << endl;
    myfile << "class " << classname << "_normals {" << endl;
    myfile << "public:" << endl;
    myfile << "float normals [16383] = {" << endl;
    cout << "#include <iostream>;" << endl << endl;
    cout << "class " << classname << "_normals {" << endl;
    cout << "std::vector<float> normals = {" << endl;
    for(int i = 0; i != Finale.normals.size(); i++){
        myfile << Finale.normals[i].x << ", " << Finale.normals[i].y << ", " << Finale.normals[i].z << ", " << endl;
        cout << Finale.normals[i].x << ", " << Finale.normals[i].y << ", " << Finale.normals[i].z << ", " << endl;
    }
	myfile << "};" << endl << "};";
    cout << "};" << endl << "};";
	myfile.close();
    myfile.open(classname+"_uv.hpp");
    myfile << "#include <iostream>" << endl << endl;
    myfile << "class " << classname << "_uv {" << endl;
    myfile << "public:" << endl;
    myfile << "float uv [16383] = {" << endl;
    cout << "#include <iostream>;" << endl << endl;
    cout << "class " << classname << "_uv {" << endl;
    cout << "float uv [" << Finale.uv.size()*3 << "] = {" << endl;
    for(int i = 0; i != Finale.uv.size(); i++){
        myfile << Finale.uv[i].x << ", " << Finale.uv[i].y << ", " << endl;
        cout << Finale.uv[i].x << ", " << Finale.uv[i].y << ", " << endl;
    }
	myfile << "};" << endl << "};";
    cout << "};" << endl << "};";
	myfile.close();
}

void objimport(string filename, Mesh &toExport){
    lmesh Finale;
    loadobj(filename.c_str(), Finale);
    toExport.totalv = Finale.vert.size();
    ofstream myfile;
    myfile.open("verts.tmp");
    for(int i = 0; i!=Finale.vert.size(); i++){
        myfile << Finale.vert[i].x << " " << Finale.vert[i].y << " " << Finale.vert[i].z << endl;
    }
    myfile.close();
    myfile.open("normals.tmp");
    for(int i = 0; i!=Finale.normals.size(); i++){
        myfile << Finale.normals[i].x << " " << Finale.normals[i].y << " " << Finale.normals[i].z << endl;
    }
    myfile.close();
    myfile.open("uv.tmp");
    for(int i = 0; i!=Finale.uv.size(); i++){
        myfile << Finale.uv[i].x << " " << Finale.uv[i].y << endl;
    }
	myfile.close();
    ifstream in;
    in.open("verts.tmp");
    for(int i = 0; i!=Finale.vert.size()*3; i++){
        in >> toExport.vertexes[i];
    }
    in.close();
    in.open("normals.tmp");
    for(int i = 0; i!=Finale.normals.size()*3; i++){
        in >> toExport.normals[i];
    }
    in.close();
    in.open("uv.tmp");
    for(int i = 0; i!=Finale.uv.size()*3; i++){
        in >> toExport.uv[i];
    }
    in.close();
}

void loadImage(const char* path, unsigned char image[65535], int& resolutionx, int& resolutiony, string classname){
    int pixels[65535];
    fstream readimage;
    readimage.open(path);
    int i1, i2, i3;
    string trash;
    readimage >> trash;
    readimage >> resolutionx >> resolutiony;
    if(resolutionx * resolutiony * 4 > 65535){
        throw runtime_error("file too large!");
    }
    readimage >> i1;
    for(int i = 0; readimage >> i1 >> i2 >> i3; i+=4){
        pixels[i] = i1;
        pixels[i+1] = i2;
        pixels[i+2] = i3;
        pixels[i+3] = 255;
        image[i] = i1;
        image[i+1] = i2;
        image[i+2] = i3;
        image[i+3] = 255;
    }
    readimage.close();

    ofstream myfile;
    myfile.open(classname+"_texture.hpp");
    myfile << "class " << classname << "_texture {" << endl;
    myfile << "public:" << endl;
    myfile << "int resx = " << resolutionx << ";" << endl;
    myfile << "int resy = " << resolutiony << ";" << endl;
    myfile << "unsigned char pixels [65535] = {" << endl;
    for(int i = 0; i != resolutionx*resolutiony*4; i+=4){
        myfile << pixels[i] << "," << pixels[i+1] << "," << pixels[i+2] << "," << pixels[i+3] << "," << endl;
    }
	myfile << "};" << endl << "};";
	myfile.close();
    myfile.open(classname+"_texture.java");
    myfile << "package com.example.openglapp;" << endl << endl;
    myfile << "public class " << classname << "_texture {" << endl;
    myfile << "public ivec2 res = new ivec2(" << resolutionx << ", " << resolutiony << ");" << endl;
    myfile << "public byte[] pixels = {" << endl;
    cout << "package com.example.openglapp;" << endl << endl;
    cout << "public class model {" << endl;
    cout << "public byte[] pixels = {" << endl;
    for(int i = 0; i != resolutionx*resolutiony*4; i+=4){
        myfile << "(byte) " << pixels[i] << "," << "(byte) " << pixels[i+1] << "," << "(byte) " << pixels[i+2] << "," << "(byte) " << pixels[i+3] << "," << endl;
        cout << "(byte) " << pixels[i] << "," << "(byte) " << pixels[i+1] << "," << "(byte) " << pixels[i+2] << "," << "(byte) " << pixels[i+3] << "," << endl;
    }
	myfile << "};" << endl << "}";
    cout << "};" << endl << "}";
	myfile.close();
}

void movecallback(){
    int state = glfwGetKey(eng.window, GLFW_KEY_W);
    if (state == GLFW_PRESS){ //w
        eng.pos.z += cos(eng.rot.y) * cos(eng.rot.x) * speed;
        eng.pos.x += cos(eng.rot.y) * sin(eng.rot.x) * -speed;
    }
    state = glfwGetKey(eng.window, GLFW_KEY_A);
    if (state == GLFW_PRESS){ // a
        eng.pos.x += cos(eng.rot.y) * cos(eng.rot.x) * speed;
        eng.pos.z -= cos(eng.rot.y) * sin(eng.rot.x) * -speed;
    }
    state = glfwGetKey(eng.window, GLFW_KEY_S);
    if (state == GLFW_PRESS){ // s
        eng.pos.z -= cos(eng.rot.y) * cos(eng.rot.x) * speed;
        eng.pos.x -= cos(eng.rot.y) * sin(eng.rot.x) * -speed;
    }
    state = glfwGetKey(eng.window, GLFW_KEY_D);
    if (state == GLFW_PRESS){ //d
        eng.pos.x -= cos(eng.rot.y) * cos(eng.rot.x) * speed;
        eng.pos.z += cos(eng.rot.y) * sin(eng.rot.x) * -speed;
    }
    state = glfwGetKey(eng.window, GLFW_KEY_Q);
    if (state == GLFW_PRESS){ // s
        eng.pos.y -= speed;
    }
    state = glfwGetKey(eng.window, GLFW_KEY_E);
    if (state == GLFW_PRESS){ //d
        eng.pos.y += speed;
    }
}

void mouse(){
    int state = glfwGetMouseButton(eng.window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS){
        mousefocused = true;
    }else{
        mousefocused = false;
    }
}

void playersettingswindow(){
    ImGui::Begin("Player settings");
    ImGui::Checkbox("enable physics", &eng.enablePhysics);
    ImGui::Checkbox("enable colision", &eng.enableColision);
    ImGui::InputText("project filename", projfilename, 256);
    if(ImGui::Button("load project")){
        projfilei.open(projfilename);
        for(int i = 0; i != 1000; i++){
            projfilei >> levelm[i].meshPosition.x >> levelm[i].meshPosition.y >> levelm[i].meshPosition.z >> levelm[i].meshRot.x >> levelm[i].meshRot.y >> levelm[i].meshRot.z >> levelm[i].meshScale.x >> levelm[i].meshScale.y >> levelm[i].meshScale.z >> pathtoalbedo[i] >> pathtospec[i] >> classname[i] >> classnamespec[i] >> objclassname[i] >> useuishader[i] >> reloadtex[i] >> writetextores[i] >> writetextoinit[i] >> writeobjtofile[i];
        }
        projfilei.close();
    }
    if(ImGui::Button("save project")){
        projfile.open(projfilename);
        for(int i = 0; i != 1000; i++){
            projfile << levelm[i].meshPosition.x << " " << levelm[i].meshPosition.y << " " << levelm[i].meshPosition.z << " " << levelm[i].meshRot.x << " " << levelm[i].meshRot.y << " " << levelm[i].meshRot.z << " " << levelm[i].meshScale.x << " " << levelm[i].meshScale.y << " " << levelm[i].meshScale.z << " " << pathtoalbedo[i] << " " << pathtospec[i] << " " << classname[i] << " " << classnamespec[i] << " " << objclassname[i] << " " << useuishader[i] << " " << reloadtex[i] << " " << writetextores[i] << " " << writetextoinit[i] << " " << writeobjtofile[i] << endl;
        }
        projfile.close();
    }
    ImGui::End();
}

void meshEdit(){
    ImGui::Begin("Mesh settings");
    ImGui::InputInt("Mesh number", &currentm, 1, 10);
    if(currentm < 0||currentm>1000){
        currentm = 0;
    }
    if(ImGui::Button("Init mesh")){
        glDeleteTextures(1, &levelm[currentm].albedoHandle);
        glDeleteTextures(1, &levelm[currentm].specularHandle);
        if(reloadtex[currentm] == true){
            objimport(pathtoobj[currentm], levelm[currentm]);
            if(writetextores[currentm] == true){
                loadImage(pathtoalbedo[currentm], levelm[currentm].texture, levelm[currentm].texResolution.x, levelm[currentm].texResolution.y, classname[currentm]);
                loadImage(pathtospec[currentm], levelm[currentm].specular, levelm[currentm].texResolution.x, levelm[currentm].texResolution.y, classnamespec[currentm]);
                outputres << "#include " << '"' << classname[currentm] << "_texture.hpp" << '"' << endl;
                outputres << "#include " << '"' << classnamespec[currentm] << "_texture.hpp" << '"' << endl;
            }
            if(writeobjtofile[currentm] == true){
                outputres << "#include " << '"' << objclassname[currentm] << "_model.hpp" << '"' << endl;
                outputres << "#include " << '"' << objclassname[currentm] << "_normals.hpp" << '"' << endl;
                outputres << "#include " << '"' << objclassname[currentm] << "_uv.hpp" << '"' << endl;
                objexport(pathtoobj[currentm], objclassname[currentm]);
            }
            if(writetextoinit[currentm] == true){
                outputinit << "Mesh no" << currentm << ";" << endl;
                outputinitj << "Mesh no" << currentm << ";" << endl;
                string msname = "no" + to_string(currentm);
                outputinit << "void initm" << currentm << "(Engine &eng){" << endl;
                outputinit << "eng.copyFloatArray(" << objclassname[currentm] << "_model().verts, " << msname << ".vertexes);" << endl;
                outputinit << "eng.copyFloatArray(" << objclassname[currentm] << "_normals().normals, " << msname << ".normals);" << endl;
                outputinit << "eng.copyFloatArray(" << objclassname[currentm] << "_uv().uv, " << msname << ".uv);" << endl;
                outputinit << "eng.copyucharArray(" << classname[currentm] << "_texture().pixels, " << msname << ".texture);" << endl;
                outputinit << "eng.copyucharArray(" << classnamespec[currentm] << "_texture().pixels, " << msname << ".specular);" << endl;
                outputinit << msname << ".texResolution.x = " << levelm[currentm].texResolution.x << ";" << endl;
                outputinit << msname << ".texResolution.y = " << levelm[currentm].texResolution.y << ";" << endl;
                outputinit << msname << ".meshPosition.x = " << levelm[currentm].meshPosition.x << ";" << endl;
                outputinit << msname << ".meshPosition.y = " << levelm[currentm].meshPosition.y << ";" << endl;
                outputinit << msname << ".meshPosition.z = " << levelm[currentm].meshPosition.z << ";" << endl;
                outputinit << msname << ".initMesh(fragmentShaderCode, vertexShaderCode);" << endl;
                outputinit << "}" << endl;

                outputinitj << "void initm" << currentm << "(Engine eng){" << endl;
                outputinitj << "eng.copyFloatArray(" << objclassname[currentm] << "_model().verts, " << msname << ".vertexes);" << endl;
                outputinitj << "eng.copyFloatArray(" << objclassname[currentm] << "_normals().normals, " << msname << ".normals);" << endl;
                outputinitj << "eng.copyFloatArray(" << objclassname[currentm] << "_uv().uv, " << msname << ".uv);" << endl;
                outputinitj << "eng.copyucharArray(" << classname[currentm] << "_texture().pixels, " << msname << ".texture);" << endl;
                outputinitj << "eng.copyucharArray(" << classnamespec[currentm] << "_texture().pixels, " << msname << ".specular);" << endl;
                outputinitj << msname << ".texResolution = new ivec2();" << endl;
                outputinitj << msname << ".texResolution.x = " << levelm[currentm].texResolution.x << ";" << endl;
                outputinitj << msname << ".texResolution.y = " << levelm[currentm].texResolution.y << ";" << endl;
                outputinitj << msname << ".meshPosition = new vec3();" << endl;
                outputinitj << msname << ".meshPosition.x = " << levelm[currentm].meshPosition.x << ";" << endl;
                outputinitj << msname << ".meshPosition.y = " << levelm[currentm].meshPosition.y << ";" << endl;
                outputinitj << msname << ".meshPosition.z = " << levelm[currentm].meshPosition.z << ";" << endl;
                outputinitj << msname << ".initMesh(fragmentShaderCode, vertexShaderCode);" << endl;
                outputinitj << "}" << endl;
            }
        }
        switch(useuishader[currentm]){
            case false:
            levelm[currentm].initMesh(fragmentShaderCode, vertexShaderCode);
            break;
            case true:
            levelm[currentm].initMesh(fragmentuiShaderCode, vertexuiShaderCode);
            break;
        }
    }
    ImGui::SliderInt("Mesh render type", &renderm[currentm], 0, 2);
    ImGui::Checkbox("use ui shader", &useuishader[currentm]);
    ImGui::Checkbox("reload resources", &reloadtex[currentm]);
    ImGui::Checkbox("write mesh init to files", &writetextoinit[currentm]);
    ImGui::Checkbox("write texture to app_res file", &writetextores[currentm]);
    ImGui::Checkbox("write obj to app_res file", &writeobjtofile[currentm]);
    float v[3];
    v[0] = levelm[currentm].meshPosition.x;
    v[1] = levelm[currentm].meshPosition.y;
    v[2] = levelm[currentm].meshPosition.z;
    ImGui::InputFloat3("Mesh position", v);
    levelm[currentm].meshPosition.x = v[0];
    levelm[currentm].meshPosition.y = v[1];
    levelm[currentm].meshPosition.z = v[2];
    v[0] = levelm[currentm].meshRot.x;
    v[1] = levelm[currentm].meshRot.y;
    v[2] = levelm[currentm].meshRot.z;
    ImGui::InputFloat3("Mesh rotation", v);
    levelm[currentm].meshRot.x = v[0];
    levelm[currentm].meshRot.y = v[1];
    levelm[currentm].meshRot.z = v[2];
    v[0] = levelm[currentm].meshScale.x;
    v[1] = levelm[currentm].meshScale.y;
    v[2] = levelm[currentm].meshScale.z;
    ImGui::InputFloat3("Mesh scale", v);
    levelm[currentm].meshScale.x = v[0];
    levelm[currentm].meshScale.y = v[1];
    levelm[currentm].meshScale.z = v[2];
    ImGui::Image((ImTextureID)levelm[currentm].albedoHandle, ImVec2(200, 200));
    ImGui::InputText("new albedo texture path:", pathtoalbedo[currentm], 256);
    ImGui::Image((ImTextureID)levelm[currentm].specularHandle, ImVec2(200, 200));
    ImGui::InputText("new specular texture path:", pathtospec[currentm], 256);
    ImGui::InputText("texture class name:", classname[currentm], 256);
    ImGui::InputText("specular class name:", classnamespec[currentm], 256);
    ImGui::InputText("obj path:", pathtoobj[currentm], 256);
    ImGui::InputText("obj class name:", objclassname[currentm], 256);
    ImGui::End();
}

void lightEdit(){
    ImGui::Begin("Light edit");
    ImGui::Checkbox("enable shadows", &rendershadow);
    ImGui::SliderInt("shadowmaps number", &shadowmapcnt, 0, 9);
    ImGui::SliderInt("light number", &currentl, 0, 9);
    ImGui::Image((ImTextureID)eng.shadowimg[currentl], ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));
    float v[3];
    v[0] = eng.lightPositions[currentl*3];
    v[1] = eng.lightPositions[currentl*3+1];
    v[2] = eng.lightPositions[currentl*3+2];
    ImGui::InputFloat3("Light position", v);
    eng.lightPositions[currentl*3] = v[0];
    eng.lightPositions[currentl*3+1] = v[1];
    eng.lightPositions[currentl*3+2] = v[2];
    v[0] = eng.lightColors[currentl*3];
    v[1] = eng.lightColors[currentl*3+1];
    v[2] = eng.lightColors[currentl*3+2];
    ImGui::InputFloat3("Light color", v);
    eng.lightColors[currentl*3] = v[0];
    eng.lightColors[currentl*3+1] = v[1];
    eng.lightColors[currentl*3+2] = v[2];
    if(ImGui::Button("make light transform same as camera")){
        eng.shadowProj.buildperspectivemat(eng.fov, 0.1, 100, 1, currentl);
        eng.shadowTrans.buildtranslatemat(eng.pos, currentl);
        eng.shadowxrot.buildxrotmat(eng.rot.y, currentl);
        eng.shadowyrot.buildyrotmat(-eng.rot.x, currentl);
    }
    if(ImGui::Button("save transform to init file")){
        eng.shadowProj.buildperspectivemat(eng.fov, 0.1, 100, 1, currentl);
        eng.shadowTrans.buildtranslatemat(eng.pos, currentl);
        eng.shadowxrot.buildxrotmat(eng.rot.y, currentl);
        eng.shadowyrot.buildyrotmat(-eng.rot.x, currentl);
    }
    ImGui::End();
}

int main(int argc, char **argv){
    eng.Init(argc, argv);
    eng.shadowProj.buildperspectivemat(90, 0.1, 100, 1, 0);
    //eng.shadowProj.buildorthomat(1, -1, 1, -1, speedf, 100f);
    eng.shadowTrans.buildtranslatemat(vec3(0, 0, -1), 0);
    eng.shadowxrot.buildxrotmat(-0.2f, 0);
    eng.shadowyrot.buildyrotmat(0, 0);
    eng.setLight(0, vec3(0, 1, 2), vec3(1, 1, 0.5f), 1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io= ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(eng.window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    eng.copyFloatArray(cube_model().verts, levelm[0].vertexes);
    eng.copyFloatArray(cube_normals().normals, levelm[0].normals);
    eng.copyFloatArray(cube_uv().uv, levelm[0].uv);
    eng.copyucharArray(cube_texture().pixels, levelm[0].texture);
    eng.copyucharArray(cubespec_texture().pixels, levelm[0].specular);
    levelm[0].totalv = cube_model().totalv;
    levelm[0].texResolution.x = cube_texture().resx;
    levelm[0].texResolution.y = cube_texture().resy;
    levelm[0].meshPosition.z = -1.5f;
    levelm[0].meshPosition.y = 1;
    levelm[0].initMesh(fragmentShaderCode, vertexShaderCode);

    eng.copyFloatArray(cube_model().verts, levelm[1].vertexes);
    eng.copyFloatArray(cube_normals().normals, levelm[1].normals);
    eng.copyFloatArray(cube_uv().uv, levelm[1].uv);
    eng.copyucharArray(cube_texture().pixels, levelm[1].texture);
    eng.copyucharArray(cubespec_texture().pixels, levelm[1].specular);
    levelm[1].totalv = cube_model().totalv;
    levelm[1].texResolution.x = cube_texture().resx;
    levelm[1].texResolution.y = cube_texture().resy;
    levelm[1].meshPosition.z = -1.5f;
    levelm[1].meshPosition.y = 0.5;
    levelm[1].meshPosition.x = 2.5f;
    levelm[1].initMesh(fragmentShaderCode, vertexShaderCode);

    eng.copyFloatArray(plane_model().verts, levelm[2].vertexes);
    eng.copyFloatArray(plane_normals().normals, levelm[2].normals);
    eng.copyFloatArray(plane_uv().uv, levelm[2].uv);
    eng.copyucharArray(cube_texture().pixels, levelm[2].texture);
    eng.copyucharArray(cubespec_texture().pixels, levelm[2].specular);
    levelm[2].totalv = plane_model().totalv;
    levelm[2].texResolution.x = cube_texture().resx;
    levelm[2].texResolution.y = cube_texture().resy;
    levelm[2].meshPosition.y = -0.5f;
    levelm[2].initMesh(fragmentShaderCode, vertexShaderCode);

    eng.copyFloatArray(monitor_model().verts, levelm[3].vertexes);
    eng.copyFloatArray(monitor_normals().normals, levelm[3].normals);
    eng.copyFloatArray(monitor_uv().uv, levelm[3].uv);
    eng.copyucharArray(cube_texture().pixels, levelm[3].texture);
    eng.copyucharArray(cubespec_texture().pixels, levelm[3].specular);
    levelm[3].totalv = monitor_model().totalv;
    levelm[3].texResolution.x = cube_texture().resx;
    levelm[3].texResolution.y = cube_texture().resy;
    levelm[3].meshPosition = vec3(-1.5f, 0.5f, 1.5f);
    levelm[3].meshScale.y = 2;
    levelm[3].initMesh(fragmentShaderCode2, vertexShaderCode);

    vec2 mousepos;

    levelm[0].meshRot.y = 0.5;
    levelm[0].meshScale.y = 1.5;

    outputres.open("app_res.hpp");
    outputres << "#include <iostream>\n";

    outputinit.open("app_init.hpp");
    outputinit << "#include <iostream>\n";
    outputinit << "#include " << '"' << "app_res.hpp" << '"' << endl;

    outputinitj.open("app_init.java");
    outputinitj << "package com.smoke.shadowtechandroid;" << endl;
    outputinitj << "public class ginit{" << endl;
    outputinitj << "private final String fragmentShaderCode = " << '"' << fragmentShaderCode << '";' << endl;
    outputinitj << "private final String vertexShaderCode = " << '"' << vertexShaderCode << '";' << endl;

    while (!glfwWindowShouldClose(eng.window)){
        if(mousefocused == true){
            glfwSetInputMode(eng.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(eng.window, &mousepos.x, &mousepos.y);
            eng.rot.x = mousepos.x/eng.resolution.x;
            eng.rot.y = -mousepos.y/eng.resolution.y;
        }else{
            glfwSetInputMode(eng.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(eng.window, &mousepos.x, &mousepos.y);
        }

        if(rendershadow == true){
            for(int b = 0; b!= shadowmapcnt+1; b++){
                eng.beginShadowPass(b);

                for(int i = 0; i!= 1000; i++){
                    if(renderm[i] == 2&&useuishader[i]==false){
                        levelm[i].Draw(eng);
                    }
                }
            }
        }

        eng.beginMainPass();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        mouse();

        if(mousefocused == false){
            playersettingswindow();
            meshEdit();
            lightEdit();
        }else{
            movecallback();
        }

        for(int i = 0; i!= 1000; i++){
            if(renderm[i] == 2||renderm[i] == 1){
                levelm[i].Draw(eng);
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        eng.endFrame();
    }
    outputinitj <<"}" << endl;
    outputres << "const char* fragmentShaderCode = " << '"' << fragmentShaderCode << '";' << endl;
    outputres << "const char* vertexShaderCode = " << '"' << vertexShaderCode << '";' << endl;
    alutExit();
    return 1;
}
