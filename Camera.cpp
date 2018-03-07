#include "Camera.h"
#include <fstream>
#include "Scene.h"
#include <limits>
#include <math.h>

double pi = 3.1415926535897;

float** multiply(int r1, int c1, int r2, int c2, float** mat1, float** mat2) {
	float** res = new float*[r1];
	for (int i = 0; i < r1; i++) {
		res[i] = new float[c2];
	}

	for (int i = 0; i < r1; ++i)
		for (int j = 0; j < c2; ++j) {
			res[i][j] = 0;
			for (int k = 0; k < c1; ++k) {
				res[i][j] += mat1[i][k] * mat2[k][j];
			}

		}

	return res;
}

void drawLine(Image& image, Vertex& v0, Vertex& v1, float** depth) {

	float slope = (float) (v0.y - v1.y) / (v0.x - v1.x);
	if (slope > 0 && slope <= 1) {
		if (v0.x < v1.x) {
			int y = v0.y;
			float d = 2 * (v0.y - v1.y) + (v1.x - v0.x);
			Color c = v0.color;
			Color dc = (v1.color - v0.color) / (v1.x - v0.x);
			for (int x = v0.x; x < v1.x; x++) {
				float de = ((x - v0.x) * (v1.z - v0.z)) / (v1.x - v0.x) + v0.z;
				if (x >=0 && y>=0 &&x < image._height && y < image._width
						&& de - 0.3 <= depth[x][y]) {
					Color& color = image.Pixel(x, y);
					color = c;
				}

				if (d < 0) {
					y = y + 1;
					d += 2 * ((v0.y - v1.y) + (v1.x - v0.x));
				}

				else {
					d += 2 * (v0.y - v1.y);
				}

				c += dc;
			}
		}

		else {
			drawLine(image, v1, v0, depth);
		}
	}

	else if (slope > 1) {
		if (v0.y < v1.y) {
			int x = v0.x;
			float d = 2 * (v0.x - v1.x) + (v1.y - v0.y);
			Color c = v0.color;
			Color dc = (v1.color - v0.color) / (v1.y - v0.y);
			for (int y = v0.y; y < v1.y; y++) {
				float de = ((y - v0.y) * (v1.z - v0.z)) / (v1.y - v0.y) + v0.z;
				if (x >=0 && y>=0 &&x < image._height && y < image._width
						&& de - 0.3 <= depth[x][y]) {
					Color& color = image.Pixel(x, y);
					color = c;
				}
				if (d < 0) {
					x = x + 1;
					d += 2 * ((v0.x - v1.x) + (v1.y - v0.y));
				}

				else {
					d += 2 * (v0.x - v1.x);
				}

				c += dc;
			}
		}

		else {
			drawLine(image, v1, v0, depth);
		}

	}

	else if (slope < 0 && slope > -1) {
		if (v0.x > v1.x) {
			int y = v0.y;
			float d = 2 * (v0.y - v1.y) - (v1.x - v0.x);
			Color c = v0.color;
			Color dc = (v1.color - v0.color) / -(v1.x - v0.x);
			for (int x = v0.x; x > v1.x; x--) {
				float de = ((x - v0.x) * (v1.z - v0.z)) / (v1.x - v0.x) + v0.z;
				if (x >=0 && y>=0 &&x < image._height && y < image._width
						&& de - 0.3 <= depth[x][y]) {
					Color& color = image.Pixel(x, y);
					color = c;
				}
				if (d < 0) {
					y = y + 1;
					d += 2 * ((v0.y - v1.y) - (v1.x - v0.x));
				}

				else {
					d += 2 * (v0.y - v1.y);
				}

				c += dc;
			}
		}

		else {
			drawLine(image, v1, v0, depth);
		}

	}

	else if (slope < -1) {
		if (v0.y < v1.y) {
			int x = v0.x;
			float d = -2 * (v0.x - v1.x) + (v1.y - v0.y);
			Color c = v0.color;
			Color dc = (v1.color - v0.color) / (v1.y - v0.y);
			for (int y = v0.y; y < v1.y; y++) {
				float de = ((y - v0.y) * (v1.z - v0.z)) / (v1.y - v0.y) + v0.z;
				if (x >=0 && y>=0 &&x < image._height && y < image._width
						&& de - 0.3 <= depth[x][y]) {
					Color& color = image.Pixel(x, y);
					color = c;
				}
				if (d > 0) {
					x = x - 1;
					d -= 2 * (-(v0.x - v1.x) + (v1.y - v0.y));
				}

				else {
					d -= 2 * -(v0.x - v1.x);
				}

				c += dc;
			}
		}

		else {
			drawLine(image, v1, v0, depth);
		}

	}

}

float tripleMin(int a, int b, int c) {
	if (a <= b && a <= c) {
		return a;
	}

	if (b <= a && b <= c) {
		return b;
	}

	return c;

}

float tripleMax(int a, int b, int c) {
	if (a >= b && a >= c) {
		return a;
	}

	if (b >= a && b >= c) {
		return b;
	}

	return c;

}

float f(int x, int y, int x0, int y0, int x1, int y1) {
	return x * (y0 - y1) + y * (x1 - x0) + x0 * y1 - y0 * x1;
}

void drawTriangle(Image& image, float** depth, Triangle& triangle) {

	Vertex& v0 = CurrentScene->_vertices[triangle.v[0] - 1];
	Vertex& v1 = CurrentScene->_vertices[triangle.v[1] - 1];
	Vertex& v2 = CurrentScene->_vertices[triangle.v[2] - 1];

	int ymin = tripleMin(v0.y, v1.y, v2.y);
	int ymax = tripleMax(v0.y, v1.y, v2.y);

	int xmin = tripleMin(v0.x, v1.x, v2.x);
	int xmax = tripleMax(v0.x, v1.x, v2.x);

	float f012 = f(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
	float f120 = f(v1.x, v1.y, v2.x, v2.y, v0.x, v0.y);
	float f201 = f(v2.x, v2.y, v0.x, v0.y, v1.x, v1.y);

	for (int y = ymin; y <= ymax; y++) {
		for (int x = xmin; x <= xmax; x++) {
			if (x >=0 && y>=0 &&x < image._height && y < image._width) {
				float alpha = f(x, y, v1.x, v1.y, v2.x, v2.y) / f012;
				float beta = f(x, y, v2.x, v2.y, v0.x, v0.y) / f120;
				float gamma = f(x, y, v0.x, v0.y, v1.x, v1.y) / f201;

				if (alpha >= 0 && beta >= 0 && gamma >= 0) {
					float d = (v0.z * alpha) + (v1.z * beta) + (v2.z * gamma);
					if (d <= depth[x][y]) {
						Color& c = image.Pixel(x, y);
						c = (v0.color * alpha) + (v1.color * beta)
								+ (v2.color * gamma);
					}

				}
			}
		}
	}

}

void fillDepth(Image& image, float** depth, Triangle& triangle) {

	Vertex& v0 = CurrentScene->_vertices[triangle.v[0] - 1];
	Vertex& v1 = CurrentScene->_vertices[triangle.v[1] - 1];
	Vertex& v2 = CurrentScene->_vertices[triangle.v[2] - 1];

	int ymin = tripleMin(v0.y, v1.y, v2.y);
	int ymax = tripleMax(v0.y, v1.y, v2.y);

	int xmin = tripleMin(v0.x, v1.x, v2.x);
	int xmax = tripleMax(v0.x, v1.x, v2.x);

	float f012 = f(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
	float f120 = f(v1.x, v1.y, v2.x, v2.y, v0.x, v0.y);
	float f201 = f(v2.x, v2.y, v0.x, v0.y, v1.x, v1.y);

	for (int y = ymin; y <= ymax; y++) {
		for (int x = xmin; x <= xmax; x++) {
			if (x >=0 && y>=0 &&x < image._height && y < image._width) {
				float alpha = f(x, y, v1.x, v1.y, v2.x, v2.y) / f012;
				float beta = f(x, y, v2.x, v2.y, v0.x, v0.y) / f120;
				float gamma = f(x, y, v0.x, v0.y, v1.x, v1.y) / f201;

				if (alpha >= 0 && beta >= 0 && gamma >= 0) {
					float d = (v0.z * alpha) + (v1.z * beta) + (v2.z * gamma);
					if (d < depth[x][y]) {
						depth[x][y] = d;
					}

				}
			}
		}
	}

}

void deleteArray(int a, int b, float** arr) {
	for (int i = 0; i < a; i++) {
		delete[] arr[i];
	}

	delete[] arr;
}

Image Camera::Render() const {
	Image image(_imagePlane.Width, _imagePlane.Height,
			CurrentScene->_background);

	float** depth = new float*[_imagePlane.Height];
	for (int i = 0; i < _imagePlane.Height; i++) {
		depth[i] = new float[_imagePlane.Width];
		for (int j = 0; j < _imagePlane.Width; j++) {
			depth[i][j] = std::numeric_limits<float>::max();
		}
	}

	for (int a = 0; a < CurrentScene->_models.size(); a++) {
		Model& model = CurrentScene->_models[a];
		for (int b = 0; b < model._triangles.size(); b++) {
			Triangle& triangle = model._triangles[b];
			for (int c = 0; c < 3; c++) {
				Vertex& vertex = CurrentScene->_vertices[triangle.v[c] - 1];

				float** v = new float*[4];
				for (int i = 0; i < 4; i++) {
					v[i] = new float[1];
				}

				v[0][0] = vertex.position._data[0];
				v[1][0] = vertex.position._data[1];
				v[2][0] = vertex.position._data[2];
				v[3][0] = 1;

				float** mul1 = multiply(3, 4, 4, 4, mvp, mper);
				float** mul2 = multiply(3, 4, 4, 4, mul1, mcam);
				float** vp = multiply(3, 4, 4, 1, mul2, v);

				float temp = vp[0][0];
				vp[0][0] = vp[1][0];
				vp[1][0] = temp;

				vp[0][0] /= vp[2][0];
				vp[1][0] /= vp[2][0];

				vertex.x = round(vp[0][0]);
				vertex.y = round(vp[1][0]);
				vertex.z = vp[2][0];

				deleteArray(3, 4, mul1);
				deleteArray(3, 4, mul2);
				deleteArray(3, 1, vp);
				deleteArray(4, 1, v);
			}

			fillDepth(image, depth, triangle);
		}
	}

	for (int a = 0; a < CurrentScene->_models.size(); a++) {
		Model& model = CurrentScene->_models[a];
		for (int b = 0; b < model._triangles.size(); b++) {
			Triangle& triangle = model._triangles[b];

			if (model.modelType == 1) {
				drawTriangle(image, depth, triangle);
			}

			else {
				drawLine(image, CurrentScene->_vertices[triangle.v[0] - 1],
						CurrentScene->_vertices[triangle.v[1] - 1], depth);
				drawLine(image, CurrentScene->_vertices[triangle.v[0] - 1],
						CurrentScene->_vertices[triangle.v[2] - 1], depth);
				drawLine(image, CurrentScene->_vertices[triangle.v[1] - 1],
						CurrentScene->_vertices[triangle.v[2] - 1], depth);
			}
		}
	}

	return image;
}

