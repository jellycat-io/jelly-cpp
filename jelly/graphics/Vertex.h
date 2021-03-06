//
// Created by Maxime Blanc on 06/09/2021.
//

#pragma once

#include "Color.h"
#include <GL/glew.h>

namespace Jelly {
	struct Position {
		float x;
		float y;
	};

	struct UV {
		float u;
		float v;
	};

	struct Vertex {
		Position position;
		Jelly::Color color;
		UV uv;

		void setPosition(float x, float y) {
			position.x = x;
			position.y = y;
		}

		void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255) {
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
		}

		void setUV(float u, float v) {
			uv.u = u;
			uv.v = v;
		}
	};
}
