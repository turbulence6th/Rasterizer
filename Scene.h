#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <vector>
#include <limits>
#include <math.h>
#include "Camera.h"
#include "Color.h"
#include "Vertex.h"
#include "Translation.h"
#include "Scaling.h"
#include "Rotation.h"
#include "Model.h"

float** multiply(int r1, int c1, int r2, int c2, float** mat1, float** mat2);

class Scene {
public:
	std::vector<Vertex> _vertices;

	std::vector<Camera> _cameras;

	std::vector<Translation> _translations;

	std::vector<Scaling> _scalings;

	std::vector<Rotation> _rotations;

	std::vector<Model> _models;

	Color _background;

public:

	const std::vector<Camera>& Cameras() const {
		return _cameras;
	}
};

extern Scene* CurrentScene;

inline void ReadScene(int argc, char** argv) {
	CurrentScene = new Scene();

	std::fstream camera_input(argv[2]);
	int count_camera;
	camera_input >> count_camera;

	for (int i = 0; i < count_camera; i++) {
		std::string camera_type, camera_id;
		camera_input >> camera_type;
		camera_input >> camera_id;

		Camera* camera = new Camera();
		camera_input >> camera->_position._data[0];
		camera_input >> camera->_position._data[1];
		camera_input >> camera->_position._data[2];

		camera_input >> camera->_gaze._data[0];
		camera_input >> camera->_gaze._data[1];
		camera_input >> camera->_gaze._data[2];

		std::cout << camera->_gaze._data[0] << " " << camera->_gaze._data[1]
				<< " " << camera->_gaze._data[2] << std::endl;

		camera->_space.Forward._data[0] = -camera->_gaze._data[0];
		camera->_space.Forward._data[1] = -camera->_gaze._data[1];
		camera->_space.Forward._data[2] = -camera->_gaze._data[2];

		camera_input >> camera->_space.Up._data[0];
		camera_input >> camera->_space.Up._data[1];
		camera_input >> camera->_space.Up._data[2];

		Vector3 xAxis = camera->_gaze.crossProduct(camera->_space.Up);
		camera->_space.Up = camera->_gaze.crossProduct(xAxis);

		camera->_space.Left = camera->_space.Up.crossProduct(
				camera->_space.Forward) * -1;

		camera_input >> camera->_imagePlane.Left;
		camera_input >> camera->_imagePlane.Right;
		camera_input >> camera->_imagePlane.Bottom;
		camera_input >> camera->_imagePlane.Top;
		camera_input >> camera->_imagePlane.Near;
		camera_input >> camera->_imagePlane.Far;
		camera_input >> camera->_imagePlane.Width;
		camera_input >> camera->_imagePlane.Height;

		camera_input >> camera->_outputFile;

		camera->_space.Forward = camera->_space.Forward.makeUnit();
		camera->_space.Up = camera->_space.Up.makeUnit();
		camera->_space.Left = camera->_space.Left.makeUnit();
		camera->_gaze = camera->_gaze.makeUnit();

		camera->mvp = new float*[3];
		for (int i = 0; i < 3; i++) {
			camera->mvp[i] = new float[4];
		}

		camera->mvp[0][0] = camera->_imagePlane.Width / 2.0;
		camera->mvp[0][1] = 0;
		camera->mvp[0][2] = 0;
		camera->mvp[0][3] = (camera->_imagePlane.Width - 1) / 2.0;

		camera->mvp[1][0] = 0;
		camera->mvp[1][1] = camera->_imagePlane.Height / 2.0;
		camera->mvp[1][2] = 0;
		camera->mvp[1][3] = (camera->_imagePlane.Height - 1) / 2.0;

		camera->mvp[2][0] = 0;
		camera->mvp[2][1] = 0;
		camera->mvp[2][2] = 0.5;
		camera->mvp[2][3] = 0.5;

		camera->mper = new float*[4];
		for (int i = 0; i < 4; i++) {
			camera->mper[i] = new float[4];
		}

		camera->mper[0][0] = 2 * camera->_imagePlane.Near
				/ (camera->_imagePlane.Right - camera->_imagePlane.Left);
		camera->mper[0][1] = 0;
		camera->mper[0][2] = (camera->_imagePlane.Right
				+ camera->_imagePlane.Left)
				/ (camera->_imagePlane.Right - camera->_imagePlane.Left);
		camera->mper[0][3] = 0;

		camera->mper[1][0] = 0;
		camera->mper[1][1] = 2 * camera->_imagePlane.Near
				/ (camera->_imagePlane.Top - camera->_imagePlane.Bottom);
		camera->mper[1][2] = (camera->_imagePlane.Top
				+ camera->_imagePlane.Bottom)
				/ (camera->_imagePlane.Top - camera->_imagePlane.Bottom);
		camera->mper[1][3] = 0;

		camera->mper[2][0] = 0;
		camera->mper[2][1] = 0;
		camera->mper[2][2] = -1
				* (camera->_imagePlane.Far + camera->_imagePlane.Near)
				/ (camera->_imagePlane.Far - camera->_imagePlane.Near);
		camera->mper[2][3] = -1
				* (2 * camera->_imagePlane.Far * camera->_imagePlane.Near)
				/ (camera->_imagePlane.Far - camera->_imagePlane.Near);

		camera->mper[3][0] = 0;
		camera->mper[3][1] = 0;
		camera->mper[3][2] = -1;
		camera->mper[3][3] = 0;

		camera->mcam = new float*[4];
		for (int i = 0; i < 4; i++) {
			camera->mcam[i] = new float[4];
		}

		camera->mcam[0][0] = camera->_space.Left._data[0];
		camera->mcam[0][1] = camera->_space.Left._data[1];
		camera->mcam[0][2] = camera->_space.Left._data[2];
		camera->mcam[0][3] = -1 * (camera->_space.Left * camera->_position);

		camera->mcam[1][0] = camera->_space.Up._data[0];
		camera->mcam[1][1] = camera->_space.Up._data[1];
		camera->mcam[1][2] = camera->_space.Up._data[2];
		camera->mcam[1][3] = -1 * (camera->_space.Up * camera->_position);

		camera->mcam[2][0] = camera->_space.Forward._data[0];
		camera->mcam[2][1] = camera->_space.Forward._data[1];
		camera->mcam[2][2] = camera->_space.Forward._data[2];
		camera->mcam[2][3] = -1 * (camera->_space.Forward * camera->_position);

		camera->mcam[3][0] = 0;
		camera->mcam[3][1] = 0;
		camera->mcam[3][2] = 0;
		camera->mcam[3][3] = 1;

		CurrentScene->_cameras.push_back(*camera);
	}

	std::fstream scene_input(argv[1]);

	scene_input >> CurrentScene->_background._channels[0];
	scene_input >> CurrentScene->_background._channels[1];
	scene_input >> CurrentScene->_background._channels[2];

	std::cout << "Background Color" << std::endl;
	std::cout << CurrentScene->_background._channels[0] << " "
			<< CurrentScene->_background._channels[1] << " "
			<< CurrentScene->_background._channels[2] << std::endl;

	std::string vertexType;
	int vertexCount;
	scene_input >> vertexType;
	scene_input >> vertexCount;

	std::string colorType;
	scene_input >> colorType;

	for (int i = 0; i < vertexCount; i++) {
		Vertex vertex;
		scene_input >> vertex.color._channels[0];
		scene_input >> vertex.color._channels[1];
		scene_input >> vertex.color._channels[2];
		CurrentScene->_vertices.push_back(vertex);
	}

	std::string positionType;
	scene_input >> positionType;

	for (int i = 0; i < vertexCount; i++) {
		Vertex& vertex = CurrentScene->_vertices[i];
		scene_input >> vertex.position._data[0];
		scene_input >> vertex.position._data[1];
		scene_input >> vertex.position._data[2];
	}

	std::cout << "Vertices" << std::endl;
	for (int i = 0; i < vertexCount; i++) {
		Vertex& vertex = CurrentScene->_vertices[i];
		std::cout << vertex.color._channels[0] << " "
				<< vertex.color._channels[1] << " " << vertex.color._channels[2]
				<< " " << vertex.position._data[0] << " "
				<< vertex.position._data[1] << " " << vertex.position._data[2]
				<< std::endl;
	}

	std::string translationType;
	int translationCount;

	scene_input >> translationType;
	scene_input >> translationCount;

	for (int i = 0; i < translationCount; i++) {
		Translation translation;
		scene_input >> translation.t._data[0];
		scene_input >> translation.t._data[1];
		scene_input >> translation.t._data[2];
		CurrentScene->_translations.push_back(translation);
	}

	std::cout << "Translations" << std::endl;
	for (int i = 0; i < translationCount; i++) {
		Translation& translation = CurrentScene->_translations[i];
		std::cout << translation.t._data[0] << " " << translation.t._data[1]
				<< " " << translation.t._data[2] << std::endl;
	}

	std::string scalingType;
	int scalingCount;

	scene_input >> scalingType;
	scene_input >> scalingCount;

	for (int i = 0; i < scalingCount; i++) {
		Scaling scaling;
		scene_input >> scaling.s._data[0];
		scene_input >> scaling.s._data[1];
		scene_input >> scaling.s._data[2];
		CurrentScene->_scalings.push_back(scaling);
	}

	std::cout << "Scalings" << std::endl;
	for (int i = 0; i < scalingCount; i++) {
		Scaling& scaling = CurrentScene->_scalings[i];
		std::cout << scaling.s._data[0] << " " << scaling.s._data[1] << " "
				<< scaling.s._data[2] << std::endl;
	}

	std::string rotationType;
	int rotationCount;

	scene_input >> rotationType;
	scene_input >> rotationCount;

	for (int i = 0; i < rotationCount; i++) {
		Rotation rotation;
		scene_input >> rotation.alpha;
		scene_input >> rotation.r._data[0];
		scene_input >> rotation.r._data[1];
		scene_input >> rotation.r._data[2];
		CurrentScene->_rotations.push_back(rotation);
	}

	std::cout << "Rotation" << std::endl;
	for (int i = 0; i < rotationCount; i++) {
		Rotation& rotation = CurrentScene->_rotations[i];
		std::cout << rotation.alpha << " " << rotation.r._data[0] << " "
				<< rotation.r._data[1] << " " << rotation.r._data[2]
				<< std::endl;
	}

	std::string modelType;
	int modelCount;

	scene_input >> modelType;
	scene_input >> modelCount;

	for (int i = 0; i < modelCount; i++) {
		Model model;
		int modelId, transformationCount, triangleCount;
		scene_input >> modelId;
		scene_input >> model.modelType;

		scene_input >> transformationCount;
		for (int j = 0; j < transformationCount; j++) {
			char transformationType;
			int transformationId;
			scene_input >> transformationType;
			scene_input >> transformationId;
			model._transformation.push_back(
					std::pair<char, int>(transformationType, transformationId));
		}

		scene_input >> triangleCount;
		for (int j = 0; j < triangleCount; j++) {
			Triangle triangle;
			scene_input >> triangle.v[0];
			scene_input >> triangle.v[1];
			scene_input >> triangle.v[2];
			model._triangles.push_back(triangle);
		}

		CurrentScene->_models.push_back(model);
	}

	std::cout << "Models" << std::endl;
	for (int i = 0; i < modelCount; i++) {
		Model& model = CurrentScene->_models[i];
		std::cout << model.modelType << std::endl;
		for (int j = 0; j < model._transformation.size(); j++) {
			std::cout << model._transformation[j].first << " "
					<< model._transformation[j].second << std::endl;
		}

		for (int j = 0; j < model._triangles.size(); j++) {
			Triangle& triangle = model._triangles[j];
			std::cout << triangle.v[0] << " " << triangle.v[1] << " "
					<< triangle.v[2] << std::endl;
		}
	}

	for (int i = 0; i < modelCount; i++) {
		Model& model = CurrentScene->_models[i];
		for (int j = 0; j < model._transformation.size(); j++) {
			std::pair<int, char> transformation = model._transformation[j];
			if (transformation.first == 't') {
				Translation& translation =
						CurrentScene->_translations[transformation.second - 1];
				for (int a = 0; a < model._triangles.size(); a++) {
					Triangle& triangle = model._triangles[a];
					for (int b = 0; b < 3; b++) {
						Vertex& vertex = CurrentScene->_vertices[triangle.v[b]
								- 1];
						if (!vertex.isTransitionUpdated) {
							vertex.position._data[0] = vertex.position._data[0]
									+ translation.t._data[0];
							vertex.position._data[1] = vertex.position._data[1]
									+ translation.t._data[1];
							vertex.position._data[2] = vertex.position._data[2]
									+ translation.t._data[2];
							vertex.isTransitionUpdated = true;
						}

					}
				}
			}

			if (transformation.first == 's') {
				Scaling& scaling = CurrentScene->_scalings[transformation.second
						- 1];
				for (int a = 0; a < model._triangles.size(); a++) {
					Triangle& triangle = model._triangles[a];
					for (int b = 0; b < 3; b++) {
						Vertex& vertex = CurrentScene->_vertices[triangle.v[b]
								- 1];
						if (!vertex.isScalingUpdated) {
							vertex.position._data[0] = vertex.position._data[0]
									* scaling.s._data[0];
							vertex.position._data[1] = vertex.position._data[1]
									* scaling.s._data[1];
							vertex.position._data[2] = vertex.position._data[2]
									* scaling.s._data[2];
							vertex.isScalingUpdated = true;
						}
					}
				}
			}

			if (transformation.first == 'r') {
				Rotation rotation =
						CurrentScene->_rotations[transformation.second - 1];
				for (int a = 0; a < model._triangles.size(); a++) {
					Triangle& triangle = model._triangles[a];
					for (int b = 0; b < 3; b++) {
						Vertex& vertex = CurrentScene->_vertices[triangle.v[b]
								- 1];
						if (!vertex.isRotationUpdated) {
							Vector3 u = rotation.r.makeUnit();
							int min = std::numeric_limits<int>::max();
							int index = 0;
							for (int i = 0; i < 3; i++) {
								if (u._data[i] < min) {
									min = u._data[i];
									index = i;
								}
							}

							Vector3 v;
							if (index == 0) {
								v._data[0] = 0;
								v._data[1] = -u._data[2];
								v._data[2] = u._data[1];
							}

							else if (index == 1) {
								v._data[0] = -u._data[2];
								v._data[1] = 0;
								v._data[2] = u._data[0];
							}

							else if (index == 2) {
								v._data[0] = -u._data[1];
								v._data[1] = u._data[0];
								v._data[2] = 0;
							}

							Vector3 w = u.crossProduct(v);

							float** mPrime = new float*[4];
							for (int i = 0; i < 4; i++) {
								mPrime[i] = new float[4];
							}

							mPrime[0][0] = u._data[0];
							mPrime[0][1] = v._data[0];
							mPrime[0][2] = w._data[0];
							mPrime[0][3] = 0;

							mPrime[1][0] = u._data[1];
							mPrime[1][1] = v._data[1];
							mPrime[1][2] = w._data[1];
							mPrime[1][3] = 0;

							mPrime[2][0] = u._data[2];
							mPrime[2][1] = v._data[2];
							mPrime[2][2] = w._data[2];
							mPrime[2][3] = 0;

							mPrime[3][0] = 0;
							mPrime[3][1] = 0;
							mPrime[3][2] = 0;
							mPrime[3][3] = 1;

							float** m = new float*[4];
							for (int i = 0; i < 4; i++) {
								m[i] = new float[4];
							}

							m[0][0] = u._data[0];
							m[0][1] = u._data[1];
							m[0][2] = u._data[2];
							m[0][3] = 0;

							m[1][0] = v._data[0];
							m[1][1] = v._data[1];
							m[1][2] = v._data[2];
							m[1][3] = 0;

							m[2][0] = w._data[0];
							m[2][1] = w._data[1];
							m[2][2] = w._data[2];
							m[2][3] = 0;

							m[3][0] = 0;
							m[3][1] = 0;
							m[3][2] = 0;
							m[3][3] = 1;

							double pi = 3.1415926535897;

							float** rx = new float*[4];
							for (int i = 0; i < 4; i++) {
								rx[i] = new float[4];
							}

							rx[0][0] = 1;
							rx[0][1] = 0;
							rx[0][2] = 0;
							rx[0][3] = 0;

							rx[1][0] = 0;
							rx[1][1] = cos(rotation.alpha * pi / 180.0);
							rx[1][2] = -sin(rotation.alpha * pi / 180.0);
							rx[2][3] = 0;

							rx[2][0] = 0;
							rx[2][1] = sin(rotation.alpha * pi / 180.0);
							rx[2][2] = cos(rotation.alpha * pi / 180.0);
							rx[2][3] = 0;

							rx[3][0] = 0;
							rx[3][1] = 0;
							rx[3][2] = 0;
							rx[3][3] = 1;

							float** p = new float*[4];
							for (int i = 0; i < 4; i++) {
								p[i] = new float[1];
							}

							p[0][0] = vertex.position._data[0];
							p[1][0] = vertex.position._data[1];
							p[2][0] = vertex.position._data[2];
							p[3][0] = 1;

							float** mul1 = multiply(4, 4, 4, 4, mPrime, rx);
							float** mul2 = multiply(4, 4, 4, 4, mul1, m);
							float** mul3 = multiply(4, 4, 4, 1, mul2, p);

							vertex.position._data[0] = mul3[0][0];
							vertex.position._data[1] = mul3[1][0];
							vertex.position._data[2] = mul3[2][0];

							vertex.isRotationUpdated = true;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < CurrentScene->_vertices.size(); i++) {
		Vertex& vertex = CurrentScene->_vertices[i];
		std::cout << vertex.position._data[0] << " " << vertex.position._data[1]
				<< " " << vertex.position._data[2] << std::endl;
	}

}

#endif //RAYTRACER_SCENE_H
