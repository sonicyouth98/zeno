#pragma once

#include <zeno/utils/disable_copy.h>
#include <zenovis/opengl/common.h>
#include <zeno/utils/Error.h>

namespace zenovis::opengl {

static std::string shader_add_line_info(std::string const &source) {
    std::string res = "1 ";
    int line = 2;
    for (auto const &c: source) {
        res.push_back(c);
        if (c == '\n') {
            res += std::to_string(line) + ' ';
            line++;
        }
    }
    res.push_back('\n');
    return res;
}

struct Shader : zeno::disable_copy {
    GLuint sha;
    GLuint target{GL_ARRAY_BUFFER};

    Shader(GLuint type) {
        CHECK_GL(sha = glCreateShader(type));
    }

    ~Shader() {
        CHECK_GL(glDeleteShader(sha));
    }

    void compile(std::string const &source) const {
        const GLchar *src = source.c_str();
        CHECK_GL(glShaderSource(sha, 1, &src, nullptr));
        CHECK_GL(glCompileShader(sha));
        int status = GL_TRUE;
        CHECK_GL(glGetShaderiv(sha, GL_COMPILE_STATUS, &status));
        if (status != GL_TRUE) {
            GLsizei logLength;
            CHECK_GL(glGetShaderiv(sha, GL_INFO_LOG_LENGTH, &logLength));
            std::vector<GLchar> log(logLength + 1);
            CHECK_GL(glGetShaderInfoLog(sha, logLength, &logLength, log.data()));
            log[logLength] = 0;
            std::string err = "Error compiling shader:\n" +
                                  shader_add_line_info(source) + "\n" +
                                  log.data();
            throw zeno::makeError(std::move(err));
        }
    }
};

struct Program : zeno::disable_copy {
    GLuint pro;

    Program() {
        CHECK_GL(pro = glCreateProgram());
    }

    ~Program() {
        CHECK_GL(glDeleteProgram(pro));
    }

    void attach(Shader const &shader) const {
        CHECK_GL(glAttachShader(pro, shader.sha));
    }

    void link() const {
        CHECK_GL(glLinkProgram(pro));
        int status = GL_TRUE;
        CHECK_GL(glGetProgramiv(pro, GL_LINK_STATUS, &status));
        if (status != GL_TRUE) {
            GLsizei logLength;
            CHECK_GL(glGetProgramiv(pro, GL_INFO_LOG_LENGTH, &status));
            std::vector<GLchar> log(logLength + 1);
            CHECK_GL(glGetProgramInfoLog(pro, logLength, &logLength, log.data()));
            log[logLength] = 0;
            std::string err = (std::string)"Error linking program:\n" + log.data();
            throw zeno::makeError(std::move(err));
        }
    }

    void use() const {
        CHECK_GL(glUseProgram(pro));
    }

    void set_uniformi(const char *name, int val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniform1i(loc, val));
    }

    void set_uniform(const char *name, float val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniform1f(loc, val));
    }

    void set_uniform(const char *name, glm::vec2 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniform2fv(loc, 1, glm::value_ptr(val)));
    }

    void set_uniform(const char *name, glm::vec3 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniform3fv(loc, 1, glm::value_ptr(val)));
    }

    void set_uniform(const char *name, glm::vec4 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniform4fv(loc, 1, glm::value_ptr(val)));
    }

    void set_uniform(const char *name, glm::mat3x3 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
    }

    void set_uniform(const char *name, glm::mat4x4 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
    }

    void set_uniform(const char *name, glm::mat4x3 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniformMatrix4x3fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
    }

    void set_uniform(const char *name, glm::mat3x4 const &val) const {
        GLuint loc;
        CHECK_GL(loc = glGetUniformLocation(pro, name));
        CHECK_GL(glUniformMatrix3x4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
    }
};

} // namespace zenovis::opengl
