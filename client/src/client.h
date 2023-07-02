#pragma once

#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine/gl.h"
#include "engine/window.h"

#include "sandbox/util.h"
#include "sandbox/host.h"

#include "world/world.h"
#include "sandbox/packet.h"

void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar *message, const void *userParam)
{
    // Print, log, whatever based on the enums and message
    if (severity >= GL_DEBUG_SEVERITY_HIGH)
        std::cout << std::hex << type << " : " << severity << " : " << message << std::endl;
}

#define MAX_FRAMESKIP 10
#define SKIP (1.0 / (float) UPS)

template <class Host> requires CheckType<Host, ClientHost>
class GameClient {
public:

    GameClient(const Host& h) : host{ std::move(h) } { }

    void init() {

        window.init(GAME_TITLE, 1280, 720);
        window.make_current();
        gl::init();

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        glDebugMessageCallback(debugMessage, NULL);

        glClearColor(0.0, 1.0, 0.0, 1.0);

        glEnable(GL_DEPTH_TEST);

        world.init();
    };

    void run() {

        bool running = true;

        double gameTime = glfwGetTime();

        std::uint32_t loops;
        float interpolation;

        while (running) {

            input();

            host.process(
                    [this]() { /* CLIENT EATS HOST CONNECT IN ENET CODE */ },
                    [&running]() {
                        running = false;
                    },
                    [this](const ByteBuffer& buf) { receive(buf); }
            );

            loops = 0;
            while (glfwGetTime() > gameTime && MAX_FRAMESKIP > loops) {
                update();
                gameTime += SKIP;
                loops++;
            }

            interpolation = (float) (glfwGetTime() + SKIP - gameTime) / SKIP;
            render(interpolation);
            window.present();

            glfwPollEvents();
            if (glfwWindowShouldClose(window.context)) {

                running = false;
            }

        }

        host.close();
    }

    void on_connect() {
        host.send(std::vector<uint8_t>{ ClientPacket::Authenticate }, true);
    }

    void receive(const ByteBuffer& buf) {
        ServerPacket header = static_cast<ServerPacket>(buf.getByte());
        world.read(header, buf);
    }

    void input() {
        world.input(window);
    }

    void update() {
        world.update(host);
    };

    void render(float interpolation) {
        window.resize();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.render(window);
    };

private:
    Engine::Window window;
    Host host;
    ClientWorld world{};
};