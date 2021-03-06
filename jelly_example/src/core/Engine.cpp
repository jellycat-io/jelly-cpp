//
// Created by Maxime Blanc on 05/09/2021.
//

#include "Engine.h"
#include <fmt/core.h>
#include <jelly/graphics/Color.h>
#include <jelly/utils/Logger.h>
#include <jelly/utils/ResourceManager.h>

Engine::Engine() : _gameState(GameState::PLAY),
                   _screenWidth(1024),
                   _screenHeight(768),
                   _time(0),
                   _maxFPS(60.0f){};

Engine::~Engine() = default;

void Engine::_init() {


	_window.create("jelly Engine", _screenWidth, _screenHeight, 0);

	_camera.init(_screenWidth, _screenHeight);

	_initShaders();

	_spriteBatch.init();
}

void Engine::_initShaders() {
	_shaderProgram.compileShaders("shaders/basic.vert.glsl", "shaders/basic.frag.glsl");
	_shaderProgram.addAttribute("position");
	_shaderProgram.addAttribute("color");
	_shaderProgram.addAttribute("uv");
	_shaderProgram.linkShaders();

	Jelly::Logger::Warn("Shaders initialized ✓", false);
}

void Engine::run() {
	_init();
	_update();
}

void Engine::_update() {
	while (_gameState != GameState::EXIT) {
		// Used for frameTime measuring
		float startTicks = SDL_GetTicks();

		_processInput();
		_time += 0.1;

		_camera.update();

		_draw();
		_calculateFPS();
		_printFPS();

		float frameTicks = SDL_GetTicks() - startTicks;
		// Limit FPS
		if (1000.0f / _maxFPS > frameTicks) {
			SDL_Delay(1000.0f / _maxFPS - frameTicks);
		}
	}

	Jelly::Logger::ClearLogs();
}

void Engine::_draw() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_shaderProgram.use();

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = _shaderProgram.getUniformLocation("sampler");
	glUniform1i(textureLocation, 0);

	// GLint timeLocation = _shaderProgram.getUniformLocation("time");
	// glUniform1f(timeLocation, _time);

	// Set the camera matrix
	GLint pLocation = _shaderProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = _camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	_spriteBatch.begin();

	glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static Jelly::GLTexture tex = Jelly::ResourceManager::GetTexture("textures/Boss_Feral_Kitsune.png");

	for(int i = 0; i < 10000; i++) {
		_spriteBatch.draw(pos + glm::vec4(50 * i, 0, 0, 0), uv, tex.id, 0.0f, Jelly::Color::White());
	}

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);

	_shaderProgram.drop();

	_window.swapBuffer();
}

void Engine::_processInput() {
	SDL_Event e;
	const float CAMERA_SPEED = 20.0f;
	const float SCALE_SPEED = 0.1f;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
			case SDL_KEYDOWN:
				_inputManager.pressKey(e.key.keysym.sym);
				break;
			case SDL_KEYUP:
				_inputManager.releaseKey(e.key.keysym.sym);
				break;
		}
	}

	if(_inputManager.isKeyPressed(SDLK_w)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0f, CAMERA_SPEED));
	}
	if(_inputManager.isKeyPressed(SDLK_s)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0f, -CAMERA_SPEED));
	}
	if(_inputManager.isKeyPressed(SDLK_a)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
	}
	if(_inputManager.isKeyPressed(SDLK_d)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
	}
	if(_inputManager.isKeyPressed(SDLK_q)) {
		_camera.setScale(_camera.getScale() - SCALE_SPEED);
	}
	if(_inputManager.isKeyPressed(SDLK_e)) {
		_camera.setScale(_camera.getScale() + SCALE_SPEED);
	}
}

void Engine::_calculateFPS() {
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static float previousTicks = SDL_GetTicks();
	float currentTicks = SDL_GetTicks();

	_frameTime = currentTicks - previousTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

	previousTicks = currentTicks;

	int count;

	currentFrame++;

	if (currentFrame < NUM_SAMPLES) {
		count = currentFrame;
	} else {
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0.0f;
	for (int i = 0; i < count; i++) {
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	if (frameTimeAverage > 0) {
		_fps = 1000.0f / frameTimeAverage;
	} else {
		_fps = 60.0f;
	}
}

void Engine::_printFPS() {
	static int frameCounter = 0;
	frameCounter++;

	if (frameCounter == 10) {
		Jelly::Logger::Trace(fmt::format("FPS: {}", _fps));
		frameCounter = 0;
	}
}
