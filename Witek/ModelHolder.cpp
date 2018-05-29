#include "ModelHolder.h"

ModelHolder::ModelHolder()
{

    this->vertices = nullptr;
    this->normals = nullptr;
    this->vertexNormals = nullptr;
    this->texCoords = nullptr;
    this->colors = nullptr;

    //this->loadModel("test2.obj","metal.png","metal_spec.png");

    /*std::cout<<"hello there"<<std::endl;
    if(loadFromOBJ("test2.obj")){
        std::cout<<"yep"<<std::endl;
    } else {
        std::cout<<"nope"<<std::endl;
    }

    shaderProgram = new ShaderProgram("vshader.vert",NULL,"fshader.frag");

    tex0=readTexture("metal.png");
	tex1=readTexture("metal_spec.png");

	this->myTexUnit = ModelHolder::nextTexUnit;
	ModelHolder::nextTexUnit+=2;
	std::cout<<ModelHolder::nextTexUnit<<std::endl;

    prepareObject();*/
}
void ModelHolder::loadModel(std::string pathOBJ, std::string pathTEX0, std::string pathTEX1){
    if(loadFromOBJ(pathOBJ.c_str())){
        std::cout<<"yep"<<std::endl;
    } else {
        std::cout<<"nope"<<std::endl;
    }

    shaderProgram = new ShaderProgram("vshader.vert",NULL,"fshader.frag");

    tex0=readTexture(pathTEX0.c_str());
	tex1=readTexture(pathTEX1.c_str());

	myTexUnit = ModelHolder::nextTexUnit;
	ModelHolder::nextTexUnit+=2;
	std::cout<<ModelHolder::nextTexUnit<<std::endl;

    prepareObject();
}
ModelHolder::ModelHolder(std::string pathOBJ, std::string pathTEX0, std::string pathTEX1){
    this->vertices = nullptr;
    this->normals = nullptr;
    this->vertexNormals = nullptr;
    this->texCoords = nullptr;
    this->colors = nullptr;

    this->loadModel(pathOBJ,pathTEX0,pathTEX1);

    /*if(loadFromOBJ(pathOBJ.c_str())){
        std::cout<<"yep"<<std::endl;
    } else {
        std::cout<<"nope"<<std::endl;
    }

    shaderProgram = new ShaderProgram("vshader.vert",NULL,"fshader.frag");

    tex0=readTexture(pathTEX0.c_str());
	tex1=readTexture(pathTEX1.c_str());

	myTexUnit = ModelHolder::nextTexUnit;
	ModelHolder::nextTexUnit+=2;
	std::cout<<ModelHolder::nextTexUnit<<std::endl;

    prepareObject();*/
}

ModelHolder::~ModelHolder()
{
    std::cout<<"Destructor started"<<std::endl;
    delete[] this->vertices;
    //std::cout<<"vertices deleted"<<std::endl;
    delete[] this->normals;
    //std::cout<<"normals deleted"<<std::endl;
    delete[] this->vertexNormals;
    //std::cout<<"VN deleted"<<std::endl;
    delete[] this->texCoords;
    //std::cout<<"texC deleted"<<std::endl;
    delete[] this->colors;
    //std::cout<<"colors deleted"<<std::endl;

    std::cout<<"tabs deleted"<<std::endl;


    glDeleteVertexArrays(1,&this->vao); //Usuniêcie vao
	glDeleteBuffers(1,&this->bufVertices); //Usuniêcie VBO z wierzcho³kami
	glDeleteBuffers(1,&this->bufColors); //Usuniêcie VBO z kolorami
	glDeleteBuffers(1,&this->bufNormals); //Usuniêcie VBO z wektorami normalnymi
	glDeleteBuffers(1,&this->bufTexCoords); //Usunięcie VBO ze współrzednymi teksturowania
	glDeleteTextures(1,&this->tex0); //Usunięcie tekstury z tex0
	glDeleteTextures(1,&this->tex1); //Usunięcie tekstury z tex1
    std::cout<<"Destructor finished"<<std::endl;
}

void ModelHolder::parseF(std::string line){
    std::vector<std::string> str_vector;

    unsigned long iter = 0;
    unsigned long prev = 0;
    while(iter<line.size()){
        if(line.find(' ',prev+1)>0) iter = line.find(' ',prev+1);
        else iter = line.size();
        str_vector.push_back(line.substr(prev,iter-prev+1));
        prev = iter+1;
    }

    for(int i = 0;i<(int)str_vector.size();i++){
        if(str_vector[i].empty())continue;
        if(str_vector[i].find(' ',0)!=std::string::npos){
                str_vector[i].erase(str_vector[i].size()-1,1);
        }
        if(str_vector[i].find(' ',0)!=std::string::npos){
                std::cout<<"parsing error spaces found"<<std::endl;;
                //str_vector[i].erase(str_vector[i].size()-1,1);
        }
        //std::cout<<str_vector[i]<<std::endl;
    }

    int tmp0, tmp1, tmp2;

    for(int i=0;i<(int)str_vector.size();i++){
        if(str_vector[i].empty())continue;
        if(str_vector[i].find('/')==str_vector[i].rfind('/')){
            ///vetices + textures
            tmp0 = strtol(str_vector[i].substr(0,str_vector[i].find('/')).c_str(),nullptr,0);
            tmp1 = strtol(str_vector[i].substr(str_vector[i].find('/')+1,str_vector[i].size()-str_vector[i].find('/')).c_str(),nullptr,0);
            tmp2 = -1;
            //std::cout<<tmp0<<" "<<tmp1<<std::endl;
        } else if(str_vector[i].find('/')+1==str_vector[i].rfind('/')){
            ///vertices + normals
            tmp0 = strtol(str_vector[i].substr(0,str_vector[i].find('/')).c_str(),nullptr,0);
            tmp1 = -1;
            tmp2 = strtol(str_vector[i].substr(str_vector[i].find('/')+2,str_vector[i].size()-str_vector[i].find('/')).c_str(),nullptr,0);
        } else if(str_vector[i].find('/')==std::string::npos){
            ///vertices only
            tmp0 = strtol(str_vector[i].c_str(),nullptr,0);
            tmp1 = -1;
            tmp2 = -1;
        } else {
            ///the most important faces loader - all of them
            tmp0 = strtol(str_vector[i].substr(0,str_vector[i].find('/')).c_str(),nullptr,0);
            tmp1 = strtol(str_vector[i].substr(str_vector[i].find('/')+1,str_vector[i].rfind('/')-str_vector[i].find('/')).c_str(),nullptr,0);
            tmp2 = strtol(str_vector[i].substr(str_vector[i].rfind('/')+1,str_vector[i].size()-str_vector[i].rfind('/')).c_str(),nullptr,0);
            //std::cout<<tmp0<<" "<<tmp1<<" "<<tmp2<<std::endl;
        }

        if(tmp0!=-1){
            whichVertex.push_back(tmp0-1);
        }
        if(tmp1!=-1){
            whichTexCord.push_back(tmp1-1);
        }
        if(tmp2!=-1){
            whichNormal.push_back(tmp2-1);
        }
    }

}


bool ModelHolder::loadFromOBJ(std::string path){
    std::ifstream in;
    in.open(path.c_str());

    if(!in.is_open()){
        std::cout<<"[load OBJ] file error: "<<std::endl;
        return false;
    }


    std::string tmp;
    float tmp_num;

    std::vector<float> tmp_vertices;
    std::vector<float> tmp_normals;
    std::vector<float> tmp_vertexNormals;
    std::vector<float> tmp_texCoords;
    std::vector<float> tmp_colors;

    while(!in.eof()){
        in>>tmp;
        if(tmp == "v"){
            for(int i=0;i<3;i++){
                in>>tmp_num;
                tmp_vertices.push_back(tmp_num);
            }
            //std::cout<<"v"<<std::endl;
        }else if(tmp == "vt"){
            for(int i=0;i<2;i++){
                in>>tmp_num;
                tmp_texCoords.push_back(tmp_num);
            }
            //std::cout<<"vt"<<std::endl;
        }else if(tmp == "vn"){
            for(int i=0;i<3;i++){
                in>>tmp_num;
                tmp_normals.push_back(tmp_num);
            }
            //std::cout<<"vn"<<std::endl;
        }else if(tmp == "f"){
            getline(in,tmp);
            ///TODO f parser
            this->parseF(tmp.erase(0,1));
            //std::cout<<"face "<<tmp<<std::endl;
        }else if(tmp == "#"){
            getline(in,tmp);
            //std::cout<<"[loadOBJ] comment: "<<tmp<<std::endl;
        }else{
            getline(in,tmp);
            //std::cout<<"[loadOBJ] cannot parse the line: "<<tmp<<std::endl;
        }
    }
    in.close();
    std::cout<<"Reading finished"<<std::endl;

    this->vertices = new float[whichVertex.size()*4];
    this->texCoords = new float[whichTexCord.size()*2];
    this->normals = new float[whichNormal.size()*4];

    this->vertexCount = whichVertex.size();

    std::cout<<this->vertexCount<<std::endl;

    for(int i=0; i<(int)whichVertex.size(); i++){
        //std::cout<<whichVertex[i]<<std::endl;
        for(int j=0;j<3;j++){
            this->vertices[4*i+j] = tmp_vertices[whichVertex[i]*3+j];
            this->normals[4*i+j] = tmp_normals[whichNormal[i]*3+j];
        }
        this->vertices[4*i+3] = 1.0f;
        this->normals[4*i+3] = 0.0f;
        for(int j=0;j<2;j++){
            this->texCoords[2*i+j] = tmp_texCoords[whichTexCord[i]*2+j];
        }
    }

    setGlobalColor(1.0f,0.0f,0.0f,1.0f);

    std::cout<<"Parsing finished"<<std::endl;

    tmp_colors.clear();
    tmp_normals.clear();
    tmp_texCoords.clear();
    tmp_vertexNormals.clear();
    tmp_vertices.clear();
    whichVertex.clear();
    whichTexCord.clear();
    whichNormal.clear();

    /***
    for(int i=0;i<(int)vertexCount;i++){
        std::cout<<i<<" || "<<vertices[4*i+0]<<" "<<vertices[4*i+1]<<" "<<vertices[4*i+2]<<" "<<vertices[4*i+3]<<" || "<<
        normals[4*i+0]<<" "<<normals[4*i+1]<<" "<<normals[4*i+2]<<" "<<normals[4*i+3]<<" || "<<
        colors[4*i+0]<<" "<<colors[4*i+1]<<" "<<colors[4*i+2]<<" "<<colors[4*i+3]<<" "<<std::endl;
    }
    ***/

    return true;
}

void ModelHolder::setGlobalColor(float r, float g, float b, float a){
    //delete[] this->colors;
    //this->colors = nullptr;
    if(!this->colors){
        std::cout<<"empty colors"<<std::endl;
        this->colors = new float[this->vertexCount*4];
    }

    for(int i=0;i<(int)vertexCount;i++){
        this->colors[i*4+0] = r;
        this->colors[i*4+1] = g;
        this->colors[i*4+2] = b;
        this->colors[i*4+3] = a;
    }
}

void ModelHolder::drawObject(mat4 mP, mat4 mV, mat4 mM, vec4 pos){
	this->shaderProgram->use();

	glUniformMatrix4fv(this->shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(this->shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(this->shaderProgram->getUniformLocation("M"),1, false, glm::value_ptr(mM));
    glUniform1i(this->shaderProgram->getUniformLocation("textureMap0"),myTexUnit);
	glUniform1i(this->shaderProgram->getUniformLocation("textureMap1"),myTexUnit+1);
	glUniform4f(this->shaderProgram->getUniformLocation("position"),pos.x,pos.y,pos.z,pos.a);



	//Powiąż teksturę z uchwytem w tex0 z zerową jednostką teksturującą
	glActiveTexture(GL_TEXTURE0+myTexUnit);
	glBindTexture(GL_TEXTURE_2D,this->tex0);
	//Powiąż teksturę z uchwytem w tex1 z zerową jednostką teksturującą
	glActiveTexture(GL_TEXTURE0+myTexUnit+1);
	glBindTexture(GL_TEXTURE_2D,this->tex1);

	glBindVertexArray(this->vao);

	glDrawArrays(GL_TRIANGLES,0,this->vertexCount);

	glBindVertexArray(0);
}

void ModelHolder::prepareObject(){
    //Zbuduj VBO z danymi obiektu do narysowania
	this->bufVertices=makeBuffer(this->vertices, this->vertexCount, sizeof(float)*4); //VBO ze współrzędnymi wierzchołków
	this->bufColors=makeBuffer(this->colors, this->vertexCount, sizeof(float)*4);//VBO z kolorami wierzchołków
	this->bufNormals=makeBuffer(this->normals, this->vertexCount, sizeof(float)*4);//VBO z wektorami normalnymi wierzchołków
    this->bufTexCoords=makeBuffer(this->texCoords, this->vertexCount,sizeof(float)*2);

	//Zbuduj VAO wiążący atrybuty z konkretnymi VBO
	glGenVertexArrays(1,&this->vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej

	glBindVertexArray(this->vao); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(this->shaderProgram,"vertex",this->bufVertices,4); //"vertex" odnosi się do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute(this->shaderProgram,"color",this->bufColors,4); //"color" odnosi się do deklaracji "in vec4 color;" w vertex shaderze
	assignVBOtoAttribute(this->shaderProgram,"normal",this->bufNormals,4); //"normal" odnosi się do deklaracji "in vec4 normal;" w vertex shaderze
    assignVBOtoAttribute(this->shaderProgram,"texCoord0",this->bufTexCoords,2);

	glBindVertexArray(0); //Dezaktywuj VAO

}

void ModelHolder::IWantToBeaTeapot(){
    delete[] this->vertices;
    delete[] this->colors;
    delete[] this->normals;

    this->vertices = nullptr;
    this->colors = nullptr;
    this->normals = nullptr;

    this->vertices = Models::TeapotInternal::vertices;
    this->colors = Models::TeapotInternal::colors;
    this->normals = Models::TeapotInternal::vertexNormals;
    this->vertexCount = Models::TeapotInternal::vertexCount;

    //setGlobalColor(1.0f,0.0f,0.0f,1.0f);

    glDeleteVertexArrays(1,&this->vao);
	glDeleteBuffers(1,&this->bufVertices);
	glDeleteBuffers(1,&this->bufColors);
	glDeleteBuffers(1,&this->bufNormals);

	prepareObject();
}
