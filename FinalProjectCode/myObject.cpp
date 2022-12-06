#include "stb_image.h"
#include "myObject.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

static bool LoadObjAndConvert(float bmin[3], float bmax[3],
	std::vector<DrawObject>* drawObjects,
	std::vector<tinyobj::material_t>& materials,
	std::map<std::string, GLuint>& textures,
	const char* filename);

GLuint MyObject::loadTexture(const char* filename)
{
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);

	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int width, height, nrChannels;

	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Error::Texture could not load texture file:" << filename
			<< std::endl;
		return 0;
	}


	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
		GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

void MyObject::createFloorVao()
{
	
	float planeVertices[] = {
		// positions            // normals			//colors		// texcoords
		50.0f, -0.5f,  50.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 1.0f,  50.0f,  0.0f,
		-50.0f, -0.5f,  50.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-50.0f, -0.5f, -50.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f,   0.0f, 50.0f,

		50.0f, -0.5f,  50.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 1.0f,  50.0f,  0.0f,
		-50.0f, -0.5f, -50.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,0.0f,   0.0f, 50.0f,
		50.0f, -0.5f, -50.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f,  50.0f, 50.0f
	};

	unsigned int planeVBO;
	
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glBindVertexArray(0);

 
	 
}


STBIDEF stbi_uc *stbi_load_gif(char const *filename, int **delays, int *width, int *height, int *frames, int *nrChannels, int req_comp)
{
	FILE *f = fopen(filename, "rb");
	unsigned char *result;
	if (!f) return NULL;
	fseek(f, 0, SEEK_END);
	int bufSize = ftell(f);
	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * bufSize);
	if (buf)
	{
		fseek(f, 0, SEEK_SET);
		fread(buf, 1, bufSize, f);
		result = stbi_load_gif_from_memory(buf, bufSize, delays, width, height, frames, nrChannels, req_comp);
		free(buf);
		buf = NULL;
	}
	else
	{
		result = NULL;
	}
	fclose(f);
	return result;
}

void MyObject::updateObjColor(ColorType ct)
{
	if (currentModels == -1) {
		for (size_t i = 0; i < myDrawModels.size(); i++) {
			myDrawModels[i].colorType = ct;
		}
	}
	else {
		myDrawModels[currentModels].colorType = ct;
	}
}
void MyObject::Update(float dt)
{
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {

		currentModels = -1;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {

		currentModels = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		currentModels = 1;

	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		updateObjColor(TEXTURE);

	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		
		updateObjColor(SINGLE);

	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		
		updateObjColor(GRADIENT);
	}
}
void buildAnimateTextrue(GLuint *animateTextures, unsigned char *data, int width, int height, int frames, int nrChannels) {
	for (int i = 0; i < frames; i++) {
		 
		glGenTextures(1, &animateTextures[i]);
		assert(animateTextures[i] != 0);

		glBindTexture(GL_TEXTURE_2D, animateTextures[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		

		unsigned char* data_frame = data + i * width * height * nrChannels;

		GLenum format = 0;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
			GL_UNSIGNED_BYTE, data_frame);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
	}
	stbi_image_free(data);

}
GifTexture getGifTextures(const char* gif_path) {
	int *delays = NULL, width = 0, height = 0, frames = 1, nrChannels = 0;
	delays = (int *)malloc(sizeof(int) * frames);
	if (delays)
	{
		delays[0] = 0;
	}
	unsigned char *data = stbi_load_gif(gif_path, &delays, &width, &height, &frames, &nrChannels, 0);
	GLuint *animateTextures = (GLuint *)malloc(sizeof(GLuint) * frames);
	buildAnimateTextrue(animateTextures, data, width, height, frames, nrChannels);
	GifTexture gifTexture;
	gifTexture.animateTextures = animateTextures;
	gifTexture.frames = frames;
	free(delays);
	return gifTexture;
}
MyObject::MyObject(GLFWwindow* window)
{
	currentModels = -1;
	this->window = window;
	createFloorVao();
	floorTexture = loadTexture("assets/textures/ground.jpg");

	nframe = 0;
	lastFrameTime = glfwGetTime();
	loadObjs();
}

void MyObject::Draw(Shader& shader, float dtt)
{

	float dt = glfwGetTime() - lastFrameTime;
	if (dt > 0.2) {
		nframe++;

		lastFrameTime = glfwGetTime();
	}
	for (size_t i = 0; i < myDrawModels.size(); i++) {
		MyModels myModels = myDrawModels[i];
		myModels.Draw(shader, nframe);
	}
	  
}
void MyObject::loadObjs()
{
	
	string Air_BalloonPath = "assets/models/balloon/Air_Balloon.obj";
	string solPath = "assets/models/sol/sol.obj";
	string eiffelPath = "assets/models/tower/tower.obj";
	string cloudPath = "assets/models/cloud/cloud.obj";
	MyModels sol;  //Statue_Of_Liberty
	if (false == LoadObjAndConvert(sol.bmin, sol.bmax, &sol.drawObjects, sol.materials, sol.textures,
		solPath.c_str())) {
		return ;
	}
	sol.solidColor = glm::vec3(0.8);
	sol.scaleMat = glm::scale(glm::mat4(1), glm::vec3(3.0f , 3.0f , 3.0f ));
	sol.positionMat = glm::translate(glm::mat4(1), vec3(-33, 0, -12));
	sol.positionMat = glm::rotate(sol.positionMat,glm::radians(-90.0f),glm::vec3(1,0,0));

	GifTexture gifTexture = getGifTextures("assets/textures/sol.gif");
	sol.gifTextures.push_back(gifTexture);

	myDrawModels.push_back(sol);

	MyModels eiffel;  //eiffel tower
	if (false == LoadObjAndConvert(eiffel.bmin, eiffel.bmax, &eiffel.drawObjects, eiffel.materials, eiffel.textures,
		eiffelPath.c_str())) {
		return;
	}
	eiffel.solidColor = glm::vec3(0.8);
	eiffel.scaleMat = glm::scale(glm::mat4(1), glm::vec3(3.0f, 3.0f, 3.0f));
	eiffel.positionMat = glm::translate(glm::mat4(1), glm::vec3(10.0-33, 0.0f, 0.0-12)) *
		glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0));

	GifTexture towerGifTexture = getGifTextures("assets/textures/tower.gif");
	eiffel.gifTextures.push_back(towerGifTexture);
	myDrawModels.push_back(eiffel);

	MyModels cloud;  //cloud
	if (false == LoadObjAndConvert(cloud.bmin, cloud.bmax, &cloud.drawObjects, cloud.materials, cloud.textures,
		cloudPath.c_str())) {
		return;
	}
	cloud.solidColor = glm::vec3(0.8);
	cloud.scaleMat = glm::scale(glm::mat4(1), glm::vec3(2.0f, 2.0f, 2.0f));
	cloud.positionMat = glm::translate(glm::mat4(1), glm::vec3(0.0-33, 2.5f, -18.0))  ;

	
	myDrawModels.push_back(cloud);

	cloud.solidColor = glm::vec3(0.9);
	cloud.scaleMat = glm::scale(glm::mat4(1), glm::vec3(2.0f, 2.0f, 2.0f));
	cloud.positionMat = glm::translate(glm::mat4(1), glm::vec3(8.0-33, 3.0f, -18.0));


	myDrawModels.push_back(cloud);

	MyModels Air_Balloon;  //cloud
	if (false == LoadObjAndConvert(Air_Balloon.bmin, Air_Balloon.bmax, &Air_Balloon.drawObjects, Air_Balloon.materials, Air_Balloon.textures,
		Air_BalloonPath.c_str())) {
		return;
	}
	Air_Balloon.solidColor = glm::vec3(0.8);
	Air_Balloon.scaleMat = glm::scale(glm::mat4(1), glm::vec3(2.0f, 2.0f, 2.0f));
	Air_Balloon.positionMat = glm::translate(glm::mat4(1), glm::vec3(15.0-40, 5.0f, -18.0));

	Air_Balloon.gifTextures.push_back(towerGifTexture);
	myDrawModels.push_back(Air_Balloon);

	Air_Balloon.solidColor = glm::vec3(0.8);
	Air_Balloon.scaleMat = glm::scale(glm::mat4(1), glm::vec3(2.0f, 2.0f, 2.0f));
	Air_Balloon.positionMat = glm::translate(glm::mat4(1), glm::vec3(-15.0-20, 5.0f, -18.0));

	Air_Balloon.gifTextures.push_back(towerGifTexture);
	myDrawModels.push_back(Air_Balloon);
	


}
static std::string GetBaseDir(const std::string& filepath) {
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}


static bool FileExists(const std::string& abs_filename) {
  bool ret;
  FILE* fp = fopen(abs_filename.c_str(), "rb");
  if (fp) {
    ret = true;
    fclose(fp);
  } else {
    ret = false;
  }

  return ret;
}
static bool LoadObjAndConvert(float bmin[3], float bmax[3],
	std::vector<DrawObject>* drawObjects,
	std::vector<tinyobj::material_t>& materials,
	std::map<std::string, GLuint>& textures,
	const char* filename) {
	tinyobj::attrib_t inattrib;
	std::vector<tinyobj::shape_t> inshapes;

 
	 

	std::string base_dir = GetBaseDir(filename);
	if (base_dir.empty()) {
		base_dir = ".";
	}
#ifdef _WIN32
	base_dir += "\\";
#else
	base_dir += "/";
#endif

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&inattrib, &inshapes, &materials, &warn, &err, filename,
		base_dir.c_str(),true);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

 

	if (!ret) {
		std::cerr << "Failed to load " << filename << std::endl;
		return false;
	}
	 

	printf("# of vertices  = %d\n", (int)(inattrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(inattrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(inattrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)inshapes.size());

	// Append `default` material
	materials.push_back(tinyobj::material_t());

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%d].diffuse_texname = %s\n", int(i),
			materials[i].diffuse_texname.c_str());
	}

	// Load diffuse textures
	{
		for (size_t m = 0; m < materials.size(); m++) {
			tinyobj::material_t* mp = &materials[m];

			if (mp->diffuse_texname.length() > 0) {
				// Only load the texture if it is not already loaded
				if (textures.find(mp->diffuse_texname) == textures.end()) {
					GLuint texture_id;
					int w, h;
					int comp;

					std::string texture_filename = mp->diffuse_texname;
					if (!FileExists(texture_filename)) {
						// Append base dir.
						texture_filename = base_dir + mp->diffuse_texname;
						if (!FileExists(texture_filename)) {
							std::cerr << "Unable to find file: " << mp->diffuse_texname
								<< std::endl;
							exit(1);
						}
					}

					unsigned char* image =
						stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
					if (!image) {
						std::cerr << "Unable to load texture: " << texture_filename
							<< std::endl;
						exit(1);
					}
					std::cout << "Loaded texture: " << texture_filename << ", w = " << w
						<< ", h = " << h << ", comp = " << comp << std::endl;

					glGenTextures(1, &texture_id);
					glBindTexture(GL_TEXTURE_2D, texture_id);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					if (comp == 3) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
							GL_UNSIGNED_BYTE, image);
					}
					else if (comp == 4) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
							GL_UNSIGNED_BYTE, image);
					}
					else {
						assert(0);  // TODO
					}
					glBindTexture(GL_TEXTURE_2D, 0);
					stbi_image_free(image);
					textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
				}
			}
		}
	}

	bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
	bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

	
	tinyobj::attrib_t outattrib;
	std::vector<tinyobj::shape_t> outshapes;
 

	std::vector<tinyobj::shape_t>& shapes =  inshapes;
	tinyobj::attrib_t& attrib =  inattrib;

	{
		for (size_t s = 0; s < shapes.size(); s++) {
			DrawObject o;
			std::vector<float> buffer;  // pos(3float), normal(3float), color(3float)

			// Check for smoothing group and compute smoothing normals
			std::map<int, vec3> smoothVertexNormals;
		

			for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
				tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

				int current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) ||
					(current_material_id >= static_cast<int>(materials.size()))) {
					// Invaid material ID. Use default material.
					current_material_id =
						materials.size() -
						1;  // Default material is added to the last item in `materials`.
				}
			
				float diffuse[3];
				for (size_t i = 0; i < 3; i++) {
					diffuse[i] = materials[current_material_id].diffuse[i];
				}
				float tc[3][2];
				if (attrib.texcoords.size() > 0) {
					if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
						(idx2.texcoord_index < 0)) {
						// face does not contain valid uv index.
						tc[0][0] = 0.0f;
						tc[0][1] = 0.0f;
						tc[1][0] = 0.0f;
						tc[1][1] = 0.0f;
						tc[2][0] = 0.0f;
						tc[2][1] = 0.0f;
					}
					else {
						assert(attrib.texcoords.size() >
							size_t(2 * idx0.texcoord_index + 1));
						assert(attrib.texcoords.size() >
							size_t(2 * idx1.texcoord_index + 1));
						assert(attrib.texcoords.size() >
							size_t(2 * idx2.texcoord_index + 1));

						// Flip Y coord.
						tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
						tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
						tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
						tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
						tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
						tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
					}
				}
				else {
					tc[0][0] = 0.0f;
					tc[0][1] = 0.0f;
					tc[1][0] = 0.0f;
					tc[1][1] = 0.0f;
					tc[2][0] = 0.0f;
					tc[2][1] = 0.0f;
				}

				float v[3][3];
				for (int k = 0; k < 3; k++) {
					int f0 = idx0.vertex_index;
					int f1 = idx1.vertex_index;
					int f2 = idx2.vertex_index;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);

					v[0][k] = attrib.vertices[3 * f0 + k];
					v[1][k] = attrib.vertices[3 * f1 + k];
					v[2][k] = attrib.vertices[3 * f2 + k];
					bmin[k] = std::min(v[0][k], bmin[k]);
					bmin[k] = std::min(v[1][k], bmin[k]);
					bmin[k] = std::min(v[2][k], bmin[k]);
					bmax[k] = std::max(v[0][k], bmax[k]);
					bmax[k] = std::max(v[1][k], bmax[k]);
					bmax[k] = std::max(v[2][k], bmax[k]);
				}

				float n[3][3];
				{
					bool invalid_normal_index = false;
					if (attrib.normals.size() > 0) {
						int nf0 = idx0.normal_index;
						int nf1 = idx1.normal_index;
						int nf2 = idx2.normal_index;

						if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
							// normal index is missing from this face.
							invalid_normal_index = true;
						}
						else {
							for (int k = 0; k < 3; k++) {
								assert(size_t(3 * nf0 + k) < attrib.normals.size());
								assert(size_t(3 * nf1 + k) < attrib.normals.size());
								assert(size_t(3 * nf2 + k) < attrib.normals.size());
								n[0][k] = attrib.normals[3 * nf0 + k];
								n[1][k] = attrib.normals[3 * nf1 + k];
								n[2][k] = attrib.normals[3 * nf2 + k];
							}
						}
					}
					else {
						invalid_normal_index = true;
					}

					

					
				}

				for (int k = 0; k < 3; k++) {
					buffer.push_back(v[k][0]);
					buffer.push_back(v[k][1]);
					buffer.push_back(v[k][2]);
					buffer.push_back(n[k][0]);
					buffer.push_back(n[k][1]);
					buffer.push_back(n[k][2]);
					// Combine normal and diffuse to get color.
					float normal_factor = 0.2;
					float diffuse_factor = 1 - normal_factor;
					float c[3] = { n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
								  n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
								  n[k][2] * normal_factor + diffuse[2] * diffuse_factor };
					float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
					if (len2 > 0.0f) {
						float len = sqrtf(len2);

						c[0] /= len;
						c[1] /= len;
						c[2] /= len;
					}
					buffer.push_back(c[0] * 0.5 + 0.5);
					buffer.push_back(c[1] * 0.5 + 0.5);
					buffer.push_back(c[2] * 0.5 + 0.5);

					buffer.push_back(tc[k][0]);
					buffer.push_back(tc[k][1]);
				}
			}

			o.vb_id = 0;
			o.numTriangles = 0;

			// OpenGL viewer does not support texturing with per-face material.
			if (shapes[s].mesh.material_ids.size() > 0 &&
				shapes[s].mesh.material_ids.size() > s) {
				o.material_id = shapes[s].mesh.material_ids[0];  // use the material ID
																 // of the first face.
			}
			else {
				o.material_id = materials.size() - 1;  // = ID for default material.
			}
			printf("shape[%d] material_id %d\n", int(s), int(o.material_id));

			if (buffer.size() > 0) {
				GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);

				
				glGenVertexArrays(1, &o.vb_id);
				glBindVertexArray(o.vb_id);

			
				o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) /
					3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

				GLuint vertices_VBO;
				glGenBuffers(1, &vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.front(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
				glEnableVertexAttribArray(0);

				//Normals VBO setup
				GLuint normals_VBO;
				glGenBuffers(1, &normals_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
				glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.front(), GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 3));
				glEnableVertexAttribArray(1);

				GLuint color_VBO;
				glGenBuffers(1, &color_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
				glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.front(), GL_STATIC_DRAW);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 6));
				glEnableVertexAttribArray(2);

				//UVs VBO setup
				GLuint uvs_VBO;
				glGenBuffers(1, &uvs_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
				glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.front(), GL_STATIC_DRAW);
				glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 9));
				glEnableVertexAttribArray(3);

				glBindVertexArray(0);

			}

			drawObjects->push_back(o);
		}
	}

	return true;
}