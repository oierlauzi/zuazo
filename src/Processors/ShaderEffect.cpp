#include <Processors/ShaderEffect.h>

#include <Graphics/Frame.h>
#include <Graphics/GL/UniqueBinding.h>

#define UNIFORM_BLOCK_BINDING 0

using namespace Zuazo::Processors;

std::map<std::string, std::weak_ptr<Zuazo::Graphics::VertexArray>> ShaderEffect::s_vertexArrays;
std::map<std::string, std::weak_ptr<Zuazo::Graphics::GL::Program>> ShaderEffect::s_shaders;

const std::string ShaderEffect::s_vertShader(
	#include "../../data/shaders/shader_effect.vert"
);

const std::string ShaderEffect::s_fragShaderHeader1(
	#include "../../data/shaders/shader_effect.1.frag"
);

const std::string ShaderEffect::s_fragShaderHeader2(
	#include "../../data/shaders/shader_effect.2.frag"
);

ShaderEffect::ShaderEffect(const Utils::VideoMode& vidMode, const std::string& fragShader) :
		m_fragShaderSrc(fragShader)
{
	//Check if the shader exists
	auto ite=s_shaders.find(m_fragShaderSrc);
	if(ite == s_shaders.end()){
		//Shader was not compiled previously. Compile it now
		m_shader=std::shared_ptr<Graphics::GL::Program>(new Graphics::GL::Program(
				s_vertShader,
				s_fragShaderHeader1 + m_fragShaderSrc + s_fragShaderHeader2
			)
		);
		m_shader->setUniformBlockBinding(UNIFORM_BLOCK_NAME, UNIFORM_BLOCK_BINDING);
		s_shaders[m_fragShaderSrc]=m_shader;
	}else{
		//Exists. Acquire it
		m_shader=ite->second.lock();
	}


	const GLint uniformBlockIndex=m_shader->getUniformBlockIndex(UNIFORM_BLOCK_NAME);
	const GLint uniformCount=m_shader->getUniformCount();
	const size_t size=m_shader->getUniformBlockSize(uniformBlockIndex);

	//Allocate storage for the parameters
	m_data=std::unique_ptr<Utils::Buffer>(new Utils::Buffer(size));

	//Interrogate the shader for requesting the uniform layout
	for(GLint i=0; i<uniformCount; ++i){
		if(m_shader->getUniformBlock(i) == uniformBlockIndex){
			//This uniform is owned by the requested block
			std::string name;
			UniformLayout layout;

			//Fill the data
			layout.index=i;
			m_shader->getUniformInfo(i, &name, &layout.type, &layout.size);
			layout.offset=m_shader->getUniformOffset(i);
			m_shader->getUniformOffset(i);

			m_layout.emplace(name, layout);
		}
	}

	m_videoMode=vidMode;

	open();
}

ShaderEffect::ShaderEffect(const std::string& fragShader) :
	ShaderEffect(Utils::VideoMode(), fragShader)
{
}

ShaderEffect::~ShaderEffect(){
	close();
}

void ShaderEffect::setPixelFormat(const Utils::PixelFormat& pixFmt){
	m_videoMode.pixFmt=pixFmt;

	if(m_drawtable){
		m_drawtable->resize(m_videoMode.toImageAttributes());
	}
}

void ShaderEffect::open(){
	//Check if the shader exists
	auto shaderIte=s_shaders.find(m_fragShaderSrc);
	if(shaderIte == s_shaders.end()){
		//Shader was not compiled previously. Compile it now
		m_shader=std::shared_ptr<Graphics::GL::Program>(new Graphics::GL::Program(
				s_vertShader,
				s_fragShaderHeader1 + m_fragShaderSrc + s_fragShaderHeader2
			)
		);
		s_shaders[m_fragShaderSrc]=m_shader; //Save it for others

		m_shader->setUniformBlockBinding(UNIFORM_BLOCK_NAME, UNIFORM_BLOCK_BINDING);
	}else{
		//Exists. Acquire it
		m_shader=shaderIte->second.lock();
	}

	//Check if the vao exists
	auto vaoIte=s_vertexArrays.find(m_fragShaderSrc);
	if(vaoIte == s_vertexArrays.end()){
		//Vao did not exist, create it
		m_vertexArray=std::shared_ptr<Graphics::VertexArray>(new Graphics::VertexArray);
		s_vertexArrays[m_fragShaderSrc]=m_vertexArray; //Save it for others

		const std::array<Utils::Vec2f, 4> passThroughVertices={
			Utils::Vec2f(-1, -1),
			Utils::Vec2f(-1, +1),
			Utils::Vec2f(+1, -1),
			Utils::Vec2f(+1, +1),
		};

		const std::array<Utils::Vec2f, 4> passThroughTexCoords={
			Utils::Vec2f(0, 0),
			Utils::Vec2f(0, 1),
			Utils::Vec2f(1, 0),
			Utils::Vec2f(1, 1),
		};

		//Upload the data
		m_vertexArray->enableAttribute(m_shader->getAttributeLoc("in_vertices"), passThroughVertices);
		m_vertexArray->enableAttribute(m_shader->getAttributeLoc("in_texCoords"), passThroughTexCoords);
	}else{
		//Exists. Acquire it
		m_vertexArray=vaoIte->second.lock();
	}

	//Create a UBO
	m_ubo=std::unique_ptr<Graphics::ShaderUniform>(
		new Graphics::ShaderUniform(UNIFORM_BLOCK_BINDING, m_data->getSize())
	);
	m_ubo->setData(m_data->get());

	//Create the drawtable
	m_drawtable=std::unique_ptr<Graphics::Drawtable>(
		new Graphics::Drawtable(m_videoMode.toImageAttributes())
	);

	m_videoSourcePad.enable();
}

void ShaderEffect::close(){
	m_drawtable.reset();
	m_ubo.reset();
	m_shader.reset();
	m_vertexArray.reset();

	auto shaderIte=s_shaders.find(m_fragShaderSrc);
	if(shaderIte != s_shaders.end()){
		if(shaderIte->second.expired()){
			//I was the only one with this shader. Delete it
			s_shaders.erase(shaderIte);
		}
	}

	auto vaoIte=s_vertexArrays.find(m_fragShaderSrc);
	if(vaoIte != s_vertexArrays.end()){
		if(vaoIte->second.expired()){
			//I was the only one with this vertex array. Delete it
			s_vertexArrays.erase(vaoIte);
		}
	}

	m_videoSourcePad.disable();
	m_videoSourcePad.reset();
	m_videoConsumerPad.reset();

}

void ShaderEffect::update() const{
	std::shared_ptr<const Graphics::Frame> frame=m_videoConsumerPad.get();

	if(frame){
		auto& res=frame->getAttributes().res;
		if(res != m_videoMode.res){
			m_videoMode.res=res;
			m_drawtable->resize(m_videoMode.toImageAttributes());
		}

		const Graphics::GL::Texture2D& tex=frame->getTexture();
		
		//Start drawing
		m_drawtable->begin();

		//Bind everything
		Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(tex);
		Graphics::GL::UniqueBinding<Graphics::GL::Program> pgmBinding(*m_shader);
		Graphics::GL::UniqueBinding<Graphics::VertexArray> vaoBinding(*m_vertexArray);
		m_ubo->bind();

		//Draw the frame
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		m_videoSourcePad.push(m_drawtable->finish());
	}else{
		m_videoSourcePad.push(); //No frame
	}
}
